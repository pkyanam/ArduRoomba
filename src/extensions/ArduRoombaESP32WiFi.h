/**
 * @file ArduRoombaESP32WiFi.h
 * @brief WiFi extension for ESP32 boards
 *
 * Uses WiFi library specific to ESP32.
 * Provides AP and Client modes with HTTP web server control.
 */

#ifndef ARDUROOMBA_ESP32WIFI_H
#define ARDUROOMBA_ESP32WIFI_H

#include "ArduRoombaWiFi.h"

// Only compile for ESP32
#if defined(ESP32)

#include <WiFi.h>
#include <WebServer.h>

class ArduRoombaESP32WiFi : public ArduRoombaWiFi {
public:
  ArduRoombaESP32WiFi(ArduRoomba& roomba);
  ~ArduRoombaESP32WiFi();

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
  WebServer* _server;
  ::WiFiMode _mode;  // Use ::WiFiMode to avoid conflict with our enum
  bool _connected;

  // HTTP request handlers
  void handleRoot();
  void handleCommand();
  void handleStatus();
  void handleNotFound();

  // Helper to parse command from request
  RoombaCommand parseCommand();
};

#endif // ESP32
#endif // ARDUROOMBA_ESP32WIFI_H
