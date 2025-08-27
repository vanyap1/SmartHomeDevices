#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SHT21.h"

#include <ESP8266HTTPClient.h>

const char* ssid     = "Internet2"; 
const char* password = "vp198808";

#define RLED 13
#define GLED 14
#define BLED 12
#define VBAT A0
#define VBAT_CORRECTION 0.0106217

#define SERIALBAUD 115200
String str_hum;
String str_temp;

#define SLEEP_TIME_SEC 1800 // 30 хвилин

SHT21 sht;
float humidity = 0.0;
float temperature = 0.0;
float vbat_raw = 0.0;





void setup() {
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  digitalWrite(RLED, HIGH);
  digitalWrite(GLED, HIGH);
  digitalWrite(BLED, HIGH);

  Serial.begin(SERIALBAUD);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();

  Wire.begin(4, 5);
  sht.begin();
  Serial.println("SHT21 sensor initialized");

  // Чекаємо підключення до WiFi
  int wifiTries = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTries < 30) {
    delay(500);
    wifiTries++;
    Serial.print(".");
  }
  Serial.println();


}




void loop() {
  // Зчитування температури та вологості з SHT21
  float temp = sht.getTemperature();
  float hum = sht.getHumidity();
  float vbat = analogRead(VBAT) * VBAT_CORRECTION;

  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" C, Hum: "); Serial.print(hum);
  Serial.print(" %, VBAT: "); Serial.println(vbat);

  // Формуємо JSON
  String mac = WiFi.macAddress();
  String json = "{";
  json += "\"battery_voltage\":" + String(vbat, 2) + ",";
  json += "\"channels\": [" + String(temp, 2) + "," + String(hum, 2) + ",0,0,0,0,0,0],";
  json += "\"input_state\":0,";
  json += "\"mac\":\"" + mac + "\",";
  json += "\"output_state\":0,";
  json += "\"userKey\":\"key\"";
  json += "}";

  bool postSuccess = false;
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://192.168.1.5:8000/sensor/data/");
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(json);
    Serial.print("HTTP POST code: "); Serial.println(httpCode);
    if (httpCode > 0 && httpCode < 400) {
      postSuccess = true;
    }
    http.end();
  }

  // Миготіння LED
  if (postSuccess) {
    if (vbat < 3.7) {
      digitalWrite(RLED, LOW); // Червоний
      digitalWrite(GLED, HIGH);
      delay(300);
      digitalWrite(RLED, HIGH);
    } else {
      digitalWrite(GLED, LOW); // Зелений
      digitalWrite(RLED, HIGH);
      delay(300);
      digitalWrite(GLED, HIGH);
    }
  } else {
    // Якщо не вдалося — коротке миготіння обома
    digitalWrite(BLED, LOW);
    delay(200);
    digitalWrite(BLED, HIGH);

  }

  Serial.println("Going to sleep...");
  ESP.deepSleep(SLEEP_TIME_SEC * 1000000);
}