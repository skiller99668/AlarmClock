#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "your-wifi-name";
const char* password = "your-wifi-password";
const char* apiKey = "your-claude-api-key";

int ANS;
String eq;

int difficulty = 1; // out of 55 for now

void adjustDifficulty(float elapsed) {
    if (elapsed > 45 && difficulty > 1) difficulty--;
    else if (elapsed < 45 && difficulty < 55) difficulty++;
    Serial.println("Difficulty: " + String(difficulty) + "/55");
}
void getEquation() {
    HTTPClient http;
    http.begin("https://api.anthropic.com/v1/messages");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", apiKey);
    http.addHeader("anthropic-version", "2023-06-01");

    String body = "{\"model\":\"claude-haiku-4-5-20251001\",\"max_tokens\":100,\"messages\":[{\"role\":\"user\",\"content\":\"Generate a math equation at difficulty level " + String(difficulty) + " out of 55. Level 1 is very simple like 3 + 5. Level 55 is extremely complex with many operators, large numbers, powers, square roots, and brackets. Scale complexity smoothly between those extremes. Follow BEDMAS. Respond in JSON only, no extra text. Format: {\\\"equation\\\": \\\"7 + 5\\\", \\\"answer\\\": 12}\"}]}";
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
    http.end();
    // for testing without API calls:
    // if (difficulty == "easy") {
    //     eq = "7 + 5";
    //     ANS = 12;
    // } else if (difficulty == "medium") {
    //     eq = "12 + 4 * 3";
    //     ANS = 24;
    // } else {
    //     eq = "7 * 19 - 34 + 6 * 2";
    //     ANS = 99;
    // }
}