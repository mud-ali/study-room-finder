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

const char *ssid = SSID;
const char *password = PASSPHRASE;

ESP8266WebServer server(80);

// helper to pick a content-type by extension
String getContentType(const String &path) {
  if (path.endsWith(".html"))
    return "text/html";
  if (path.endsWith(".css"))
    return "text/css";
  if (path.endsWith(".js"))
    return "application/javascript";
  if (path.endsWith(".png"))
    return "image/png";
  if (path.endsWith(".jpg") || path.endsWith(".jpeg"))
    return "image/jpeg";
  if (path.endsWith(".svg"))
    return "image/svg+xml";
  if (path.endsWith(".json"))
    return "application/json";
  if (path.endsWith(".ico"))
    return "image/x-icon";
  return "application/octet-stream";
}

void handleNotFound() {
  String uri = server.uri();

  String fsPath = "/dist" + uri;
  if (fsPath.endsWith("/"))
    fsPath += "index.html";

  if (LittleFS.exists(fsPath)) {
    File f = LittleFS.open(fsPath, "r");
    if (f) {
      server.streamFile(f, getContentType(fsPath));
      f.close();
      return;
    }
  }

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += uri;
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

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
  }

  server.serveStatic("/", LittleFS, "/dist");

  server.on("/", HTTP_GET, []() {
    if (LittleFS.exists("/dist/index.html")) {
      File f = LittleFS.open("/dist/index.html", "r");
      if (f) {
        server.streamFile(f, "text/html");
        f.close();
        return;
      }
    }
    server.send(500, "text/plain", "index.html not found");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) { server.handleClient(); }