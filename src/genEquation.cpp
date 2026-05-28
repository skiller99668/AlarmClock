#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* apiKey = API_KEY;

int ANS;
String eq;

int difficulty = 1; // out of 55 for now

//WiFi.begin(ssid, password);
//while (WiFi.status() != WL_CONNECTED) delay(500);

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

    eq = String(a) + " " + sign + " " + String(b);

}

void adjustDifficulty(float elapsed) 
{
    if (elapsed > 45 && difficulty > 1) difficulty--;
    else if (elapsed < 45 && difficulty < 55) difficulty++;
    //Serial.println("Difficulty: " + String(difficulty) + "/55");
}

void getEquation() 
{
    HTTPClient http;
    http.begin("https://api.anthropic.com/v1/messages");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", apiKey);
    http.addHeader("anthropic-version", "2023-06-01");

    String body = "{\"model\":\"claude-haiku-4-5-20251001\",\"max_tokens\":100,\"messages\":[{\"role\":\"user\",\"content\":\"Generate a math equation at difficulty level " + String(difficulty) + " out of 55. Level 1 is very simple like 3 + 5. Level 55 is extremely complex with many operators, large numbers, powers, square roots, and brackets. Scale complexity smoothly between those extremes. Follow BEDMAS. Ensuer the equation evaluates to a whole number. Respond in JSON only, no extra text. Max 16 characters including spaces. Format: {\\\"equation\\\": \\\"7 + 5\\\", \\\"answer\\\": 12}\"}]}";
    int code = http.POST(body);
    if (code == 200) {
        StaticJsonDocument<1024> doc;
        deserializeJson(doc, http.getString());
        String text = doc["content"][0]["text"].as<String>();
        
        StaticJsonDocument<256> eq_doc;
        deserializeJson(eq_doc, text);
        eq = eq_doc["equation"].as<String>();
        ANS = eq_doc["answer"].as<int>();
    }
    else // if AI fails go back to old equation generation
    {
        equation();
    }
    http.end();
}

