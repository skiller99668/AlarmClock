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
    if (elapsed > 45 && difficulty > 1)
    {
        difficulty--;
    } 
    else if (elapsed < 45 && difficulty < 55)
    {
        difficulty++;
    } 
    else if (elapsed < 15 && difficulty < 54)
    {
        difficulty += 2;
    }
     else if (elapsed > 70 && difficulty > 2)
    {
        difficulty -= 2;
    }
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

    // pick a random SHAPE (structure only) so each alarm looks different.
    // Each form is tagged with the operations it needs; we only offer forms
    // whose operations the current difficulty unlocks, so a low ("+ - only")
    // level never gets asked for a square root, power, etc. it isn't allowed.
    enum { kMul = 1, kDiv = 2, kBrk = 4, kPow = 8, kRoot = 16 };
    struct Form { const char* text; uint8_t need; };
    static const Form forms[] = {
        {"leads with a bracketed group",                                kBrk},
        {"ends with a bracketed group",                                 kBrk},
        {"features an even division (divisor divides evenly)",          kDiv},
        {"features a subtraction between two parts",                    0},
        {"is a left-to-right chain without brackets",                   0},
        {"mixes precedence like a*b+c",                                 kMul},
        {"has nested brackets like (a+(b*c))",                          kBrk | kMul},
        {"is a product of two sums like (a+b)*(c+d)",                   kBrk | kMul},
        {"alternates + and - like a-b+c-d",                            0},
        {"uses a power as one term in a sum or difference",             kPow},
        {"has a square root as one term in a larger expression",        kRoot},
        {"is a chain of three multiplications",                         kMul},
        {"has a large number divided by a bracketed sub-expression",    kDiv | kBrk},
        {"uses subtraction inside brackets then multiplies the result", kBrk | kMul},
        {"mixes division and addition in the form a/b+c*d",             kDiv | kMul},
    };

    // which operation classes this difficulty unlocks (mirrors the bands below)
    uint8_t allowed = 0;
    if (difficulty >= 6)  allowed |= kMul | kDiv;  // multiplication & division
    if (difficulty >= 16) allowed |= kBrk;         // brackets
    if (difficulty >= 21) allowed |= kPow;         // powers / exponents
    if (difficulty >= 31) allowed |= kRoot;        // square roots

    // keep only forms whose required ops are all unlocked, then pick one
    const int FORM_COUNT = sizeof(forms) / sizeof(forms[0]);
    const char* picks[FORM_COUNT];
    int n = 0;
    for (int i = 0; i < FORM_COUNT; i++)
        if ((forms[i].need & ~allowed) == 0)
            picks[n++] = forms[i].text;
    String form = picks[esp_random() % n];   // esp_random() = true RNG, no seeding needed

    // difficulty controls which operations are allowed (band changes every 5 levels)
    const char* band;
    if (difficulty <= 5)
        band = "use only + and -, no multiplication, no division, no powers, no roots";
    else if (difficulty <= 10)
        band = "use + - * /, no powers or roots";
    else if (difficulty <= 15)
        band = "use + - * /, no powers or roots";
    else if (difficulty <= 20)
        band = "use + - * / with brackets, no powers or roots";
    else if (difficulty <= 25)
        band = "use + - * / with brackets; you may also use squares (exponent 2 only), no roots";
    else if (difficulty <= 30)
        band = "use + - * / with brackets and squares (exponent 2); you may also use cubes (exponent 3), no roots";
    else if (difficulty <= 35)
        band = "use + - * / with brackets, exponents up to 3, and square roots of small perfect squares (4,9,16,25,36,49)";
    else if (difficulty <= 40)
        band = "use + - * / with brackets, exponents up to 4, and square roots of perfect squares up to 144";
    else if (difficulty <= 45)
        band = "use + - * / with brackets, exponents up to 4, and square roots of perfect squares up to 225";
    else if (difficulty <= 50)
        band = "use + - * / with brackets, exponents up to 5, and square roots of perfect squares up to 400";
    else
        band = "use + - * / with brackets, exponents up to 6, and square roots of perfect squares up to 900";

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
                "On the 15-column display sqrt(N) shows as 1 plus the digits "
                "of N. It MUST fit within 15 columns: if it would not fit, "
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


