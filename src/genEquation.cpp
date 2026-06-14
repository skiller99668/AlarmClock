#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#include "config.h"

const char* apiKey = LLM_API_KEY;

int ANS;
String eq;

int difficulty = 1; // out of 55 for now

int getNum(int x) {
  return random(x);
}

void setAns(int a, int b, String sign)
{
    if (sign == "+") { ANS = a + b; }
    else if (sign == "-") { ANS = a - b; }
    else if (sign == "*") { ANS = a * b; }
    else { ANS = a/b; }
}

void equation()
{
    String sign;
    int a;
    int b;
    switch(getNum(4))
    {
        case 0:
            sign = "+";
            break;
        case 1:
            sign = "-";
            break;
        case 2:
            sign = "*";
            break;
        case 3:
            sign = "/";
            break;
    }
    if (sign == "/")
    {
        b = getNum(10) + 1;
        int result = getNum(100) + 1;
        a = result * b;
    }
    else if (sign == "*")
    {
        a = getNum(20) + 1;
        b = getNum(20) + 1;
    }
    else
    {
        a = getNum(80) + 20;
        b = getNum(80) + 20;

        if (b > a)
        {
            int temp = b;
            b = a;
            a = temp;
        }
    }

    setAns(a, b, sign);

    eq = String(a) + sign + String(b);

}

void adjustDifficulty(float elapsed) 
{
    if (elapsed > 45 && difficulty > 1) difficulty--;
    else if (elapsed < 45 && difficulty < 55) difficulty++;
    //Serial.println("Difficulty: " + String(difficulty) + "/55");
}

// Convert "sqrt(144)" -> "\x01144" so the LCD shows √144 (1 column for the symbol).
// eq is display-only; ANS is what gets compared, so this swap is purely cosmetic.
String toDisplay(String s)
{
    int i;
    while ((i = s.indexOf("sqrt(")) != -1)
    {
        int close = s.indexOf(')', i + 5);
        if (close == -1) break;
        s = s.substring(0, i) + '\x01' + s.substring(i + 5, close) + s.substring(close + 1);
    }
    return s;
}

void getEquation()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
        delay(250);

    if (WiFi.status() != WL_CONNECTED) {  // no network -> local fallback
        equation();
        return;
    }

    // pick a random SHAPE (structure only) so each alarm looks different;
    // the difficulty band below decides which operations are actually allowed.
    static const char* forms[] = {
        "leads with a bracketed group",
        "ends with a bracketed group",
        "features an even division",
        "features a subtraction between two parts",
        "is a left-to-right chain without brackets",
        "mixes precedence like a*b+c"
    };
    String form = forms[esp_random() % 6];   // esp_random() = true RNG, no seeding needed

    // difficulty controls which operations are allowed (band changes at 15/30/45)...
    const char* band;
    if (difficulty <= 15)
        band = "use only + - * / with no powers or square roots";
    else if (difficulty <= 30)
        band = "use + - * / and you may use squares (exponent 2), but no square roots and no exponent above 2";
    else if (difficulty <= 45)
        band = "use + - * /, square roots of perfect squares, and exponents up to 4";
    else
        band = "use + - * /, square roots of perfect squares, and exponents up to 6";

    // ...and how many operations and how large the numbers get (smooth per level)
    int ops = 2 + (difficulty - 1) / 12;   // ~2 at level 1, up to 6 near level 55
    if (ops > 6) ops = 6;
    int maxNum = 10 + difficulty * 2;      // grows with level
    if (maxNum > 64) maxNum = 64;          // keep ordinary numbers to 2 digits (sqrt is exempt)

    WiFiClientSecure client;
    client.setInsecure();                    // skip cert validation (fine for personal use)

    HTTPClient http;
    http.begin(client, LLM_URL);
    http.setTimeout(15000); // hosted models can take a few seconds to respond
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + apiKey);

    String body =
        "{"
            "\"model\":\"" LLM_MODEL "\","
            "\"max_tokens\":1024,"
            "\"temperature\":0.8,"
            "\"reasoning_effort\":\"low\","
            "\"response_format\":{\"type\":\"json_object\"},"
            "\"messages\":[{\"role\":\"user\",\"content\":\""
                "Generate ONE math equation for a wake-up alarm "
                "at difficulty level "
                + String(difficulty) +
                " out of 55, where 1 is trivial and 55 is expert. "
                "For this level, "
                + band +
                ". Aim for about "
                + String(ops) +
                " operations with ordinary numbers up to "
                + String(maxNum) +
                " (square roots may use perfect squares up to three digits), "
                "and at most two sets of brackets. "
                "Write square roots as sqrt(N) of perfect squares "
                "only, powers use ^, follow BEDMAS. "
                "The result must be a positive whole number under 2000000000. "
                "On the 16-column display sqrt(N) shows as 1 plus the digits "
                "of N. It MUST fit within 16 columns: if it would not fit, "
                "use fewer operations or smaller numbers. Fitting is required. "
                "Shape it so it "
                + form +
                ". Respond in JSON only: "
                "{\\\"equation\\\": \\\"7 + 5\\\", \\\"answer\\\": 12}"
            "\"}]}";
    int code = http.POST(body);
    if (code == 200) 
    {
        JsonDocument filter;
        filter["choices"][0]["message"]["content"] = true;

        JsonDocument doc;
        DeserializationError err = deserializeJson(
            doc, http.getString(), DeserializationOption::Filter(filter));

        String text = doc["choices"][0]["message"]["content"].as<String>();

        JsonDocument eq_doc;
        DeserializationError eqErr = deserializeJson(eq_doc, text);
        String rawEq = eq_doc["equation"].as<String>();
        ANS = eq_doc["answer"].as<int>();
        eq  = toDisplay(rawEq);               // sqrt(N) -> √N for the LCD

        // bad parse, empty, or too wide for the 16-col display -> local fallback
        if (err || eqErr || rawEq.length() == 0 || eq.length() > 16)
        {
            equation();
        }
    }
    else // if AI fails go back to old equation generation
    {
        equation();
    }
    http.end();

    WiFi.disconnect(true); // disconnect cuz only need wifi for equation generation
}


