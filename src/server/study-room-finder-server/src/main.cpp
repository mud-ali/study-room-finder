#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <WiFiClient.h>

#include "config.h"

#ifndef SSID
#define SSID ""
#endif
#ifndef PASSPHRASE
#define PASSPHRASE ""
#endif

const char* ssid = SSID;
const char* password = PASSPHRASE;

ESP8266WebServer server(80);

void handleRoot() {
  if (!LittleFS.begin()) {
    server.send(500, "text/plain", "Failed to mount file system");
    return;
  }

  File file = LittleFS.open("/dist/index.html", "r");

  String html = file.readString();
  file.close();
  server.send(200, "text/html", html);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  int wifiConnected = WiFi.begin(ssid, password);
  if (wifiConnected == WL_CONNECT_FAILED) {
    Serial.println("failed to connect wifi");
  }
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.serveStatic("/", LittleFS, "/dist");

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) { server.handleClient(); }