#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "config.h"
#include "room_data.h"

#ifndef SSID
#define SSID ""
#endif
#ifndef PASSPHRASE
#define PASSPHRASE ""
#endif

const char *ssid = SSID;
const char *password = PASSPHRASE;

WebServer server(80);

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

  server.on("/", HTTP_GET, []() {
    if (LittleFS.exists("/dist/index.html")) {
      File f = LittleFS.open("/dist/index.html", "r");
      if (f) {
        server.streamFile(f, "text/html");
        f.close();
        return;
      }
    }
    Serial.println("no files found");
    server.send(500, "text/plain", "index.html not found");
  });

  server.on("/update", HTTP_POST, []() {
    if (!server.hasArg("plain")) {
      server.send(400, "application/json",
                  "{\"error\":\"Please include a JSON body\"}");
      return;
    }

    String body = server.arg("plain");
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    const char *name = doc["name"];
    if (!name) {
      server.send(400, "application/json", "{\"error\":\"Missing name\"}");
      return;
    }

    bool roomExists = false;
    for (int i = 0; i < allRooms.size(); i++) {
      if (allRooms.at(i).name == name) {
        if (doc["occupancy"].is<unsigned int>()) {
          allRooms[i].occupancy = doc["occupancy"].as<unsigned int>();
        }
        if (doc["volume"].is<unsigned int>()) {
          allRooms.at(i).volume = doc["volume"].as<unsigned int>();
        }
        roomExists = true;
        break;
      }
    }

    if (!roomExists) {
        roomData newRoom = {
            name,
            doc["occupancy"].as<unsigned int>(),
            doc["volume"].as<unsigned int>(),
            100
        };
        allRooms.push_back(newRoom);
    }

    server.send(200, "application/json", "{\"error\":false}");
  });

  server.on("/refresh", HTTP_GET, []() {
    JsonDocument doc;
    JsonArray rooms = doc.to<JsonArray>();

    for (int i = 0; i < allRooms.size(); i++) {
      JsonObject room = rooms.add<JsonObject>();
      room["name"] = allRooms.at(i).name;
      room["occupancy"] = allRooms.at(i).occupancy;
      room["volume"] = allRooms.at(i).volume;
      room["maxOccupancy"] = allRooms.at(i).maxOccupancy;
    }

    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
  });

  server.serveStatic("/", LittleFS, "/dist/");
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) { server.handleClient(); }