/**
 * @file ArduRoombaWiFiS3.h
 * @brief WiFi extension for Arduino Uno R4 WiFi
 *
 * Uses WiFiS3 library specific to Arduino Uno R4 WiFi board.
 * Provides AP and Client modes with HTTP web server control.
 */

#ifndef ARDUROOMBA_WIFIS3_H
#define ARDUROOMBA_WIFIS3_H

#include "ArduRoombaWiFi.h"

// Only compile for Uno R4 WiFi
#if defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI)

#include <WiFiS3.h>

class ArduRoombaWiFiS3 : public ArduRoombaWiFi {
public:
  ArduRoombaWiFiS3(ArduRoomba& roomba);
  ~ArduRoombaWiFiS3();

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
  WiFiServer* _server;
  WiFiMode _mode;
  bool _connected;

  void handleHTTPRequest(WiFiClient& client);
  String parseGETParameter(const String& request, const String& param);
  RoombaCommand parseCommand(const String& request);
};

#endif // ARDUINO_UNOWIFIR4
#endif // ARDUROOMBA_WIFIS3_H
