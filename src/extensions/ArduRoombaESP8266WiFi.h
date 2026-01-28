/**
 * @file ArduRoombaESP8266WiFi.h
 * @brief WiFi extension for ESP8266 boards
 *
 * Uses ESP8266WiFi and ESP8266WebServer libraries specific to ESP8266.
 * Provides AP and Client modes with HTTP web server control.
 *
 * Requires:
 * - ESP8266WiFi library (included with ESP8266 core)
 * - ESP8266WebServer library (included with ESP8266 core)
 */

#ifndef ARDUROOMBA_ESP8266WIFI_H
#define ARDUROOMBA_ESP8266WIFI_H

#include "ArduRoombaWiFi.h"

// Only compile for ESP8266
#if defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

class ArduRoombaESP8266WiFi : public ArduRoombaWiFi {
public:
  ArduRoombaESP8266WiFi(ArduRoomba& roomba);
  ~ArduRoombaESP8266WiFi();

  // WiFi setup
  bool beginAP(const char* ssid, const char* password = nullptr) override;
  bool beginClient(const char* ssid, const char* password) override;
  void end() override;

  // Status
  bool isConnected() const override;
  String getModeString() const override;
  String getIPAddress() const override;
  String getMACAddress() const override;
  int getRSSI() const override;

  // HTTP server
  void startWebServer(uint16_t port = 80) override;
  void handleClient() override;

private:
  ESP8266WebServer* _server;
  WiFiMode _wifiMode;
  bool _connected;

  // HTTP request handlers
  void handleRoot();
  void handleCommand();
  void handleStatus();
  void handleNotFound();

  // Helper to parse command from request
  RoombaCommand parseCommand();
};

#endif // ESP8266
#endif // ARDUROOMBA_ESP8266WIFI_H
