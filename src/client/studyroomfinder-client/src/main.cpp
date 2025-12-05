#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <string>
#include "occupancy.hpp"
#include "wifi_config.hpp"

using std::string;

const char* SERVER_URL = "http://192.168.4.1/update";

const unsigned long POST_INTERVAL_MS = 3000;
unsigned long lastPostTime = 0;

const int trigPin1 = 22;
const int echoPin1 = 23;

const int trigPin2 = 19;
const int echoPin2 = 21;

int volumeVal = 50;

// static properties for this counter
string clientName = "CKB Study Lounge";

OccupancyCounter counter(trigPin1, echoPin1, trigPin2, echoPin2);

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi");
  }
}

void postUpdate() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, attempting reconnect...");
    connectToWiFi();
    return;
  }
  
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
  
  // Build JSON payload
  String json = "{\"name\":\"" + String(clientName.c_str()) + 
                "\",\"occupancy\":" + String(counter.getOccupancy()) + 
                ",\"volume\":"+ String(volumeVal) +"}";
  
  Serial.print("POST: ");
  Serial.println(json);
  
  int httpResponseCode = http.POST(json);
  
  if (httpResponseCode > 0) {
    Serial.print("Response: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

void setup() {
  Serial.begin(115200);
  counter.begin();
  connectToWiFi();
}

void loop() {
  counter.update();

  unsigned long now = millis();
  if (now - lastPostTime >= POST_INTERVAL_MS) {
    postUpdate();
    lastPostTime = now;
  }

  int sign = random() % 2 == 0 ? -1 : 1;
  volumeVal += sign;
  if (volumeVal > 90) volumeVal -= 2;
  else if (volumeVal < 10) volumeVal += 2;

  delay(200);
}
