/**
 * @file ArduRoombaWiFi.h
 * @brief Base WiFi extension interface for ArduRoomba
 *
 * Provides common WiFi control functionality with platform-specific implementations.
 * Supports both AP mode (Roomba creates hotspot) and Client mode (connects to network).
 *
 * Supported platforms:
 * - ESP32 (native WiFi and WebServer libraries)
 * - ESP8266 (ESP8266WiFi and ESP8266WebServer libraries)
 * - Arduino Uno R4 WiFi (WiFiS3 library)
 */

#ifndef ARDUROOMBA_WIFI_H
#define ARDUROOMBA_WIFI_H

#include "../ArduRoomba.h"

// WiFi operating modes
enum class WiFiMode : uint8_t {
  AP,      // Access Point - Roomba creates its own network
  CLIENT   // Client - Roomba connects to existing network
};

/**
 * Command protocol for WiFi control
 */
struct RoombaCommand {
  char action[16];   // "forward", "backward", "left", "right", "stop", "clean", "dock", "spinLeft", "spinRight"
  int16_t speed;     // Speed parameter (0-500)
  int16_t duration;  // Duration in milliseconds (0 = continuous)

  RoombaCommand() : speed(200), duration(0) {
    action[0] = '\0';
  }

  RoombaCommand(const char* act, int16_t s = 200, int16_t d = 0)
    : speed(s), duration(d) {
    strncpy(action, act, sizeof(action) - 1);
    action[sizeof(action) - 1] = '\0';
  }
};

/**
 * Command result status
 */
enum class CommandResult : uint8_t {
  SUCCESS,
  UNKNOWN_ACTION,
  INVALID_SPEED,
  LOW_BATTERY,
  BLOCKED,
  ERROR
};

/**
 * Base class for WiFi-enabled Roomba control
 * Platform-specific implementations inherit from this
 */
class ArduRoombaWiFi {
public:
  ArduRoombaWiFi(ArduRoomba& roomba);
  virtual ~ArduRoombaWiFi() {}

  // WiFi setup - must be implemented by platform-specific class
  virtual bool beginAP(const char* ssid, const char* password = nullptr) = 0;
  virtual bool beginClient(const char* ssid, const char* password) = 0;
  virtual void end() = 0;

  // Status
  virtual bool isConnected() const = 0;
  virtual String getModeString() const = 0;
  virtual String getIPAddress() const = 0;
  virtual String getMACAddress() const { return ""; }
  virtual int getRSSI() const { return 0; }

  // HTTP server control
  virtual void startWebServer(uint16_t port = 80);
  virtual void handleClient() = 0;

  // Command processing
  CommandResult processCommand(const RoombaCommand& cmd);

  // Callbacks
  typedef void (*CommandCallback)(const RoombaCommand& cmd, CommandResult result);
  void setCommandCallback(CommandCallback callback) { _commandCallback = callback; }

  // Enable/disable remote control
  void enableRemoteControl(bool enable) { _remoteEnabled = enable; }
  bool isRemoteEnabled() const { return _remoteEnabled; }

  // Safety threshold
  void setLowBatteryThreshold(uint16_t mV) { _lowBatteryThreshold = mV; }
  uint16_t getLowBatteryThreshold() const { return _lowBatteryThreshold; }

protected:
  ArduRoomba& _roomba;
  bool _remoteEnabled;
  CommandCallback _commandCallback;
  uint16_t _lowBatteryThreshold;
  uint16_t _serverPort;

  // Helper to generate HTML control page
  String generateControlPage();

  // Helper to generate JSON status
  String generateStatusJSON();

  // Helper to generate status with extended info
  String generateExtendedStatusJSON();

  // Parse action from string
  bool isValidAction(const String& action) const;
};

#endif
