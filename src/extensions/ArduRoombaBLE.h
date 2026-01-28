/**
 * @file ArduRoombaBLE.h
 * @brief Bluetooth Low Energy extension for ESP32 and Arduino Uno R4 WiFi
 *
 * Provides BLE GATT server for mobile app control of Roomba.
 * Uses standard BLE services and characteristics for easy integration
 * with mobile apps and BLE explorers.
 *
 * Supported platforms:
 * - ESP32 (using BLEDevice library)
 * - Arduino Uno R4 WiFi (using ArduinoBLE library)
 *
 * BLE Service UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
 * Command Characteristic (Write): beb5483e-36e1-4688-b7f5-ea07361b26a8
 * Status Characteristic (Read/Notify): beb5483f-36e1-4688-b7f5-ea07361b26a8
 *
 * Command Format: "action:speed:duration" (e.g., "forward:200:1000")
 * Status Format: "voltage:connected:wall:bumper:cliff:remote"
 */

#ifndef ARDUROOMBA_BLE_H
#define ARDUROOMBA_BLE_H

#include "../ArduRoomba.h"

// BLE UUIDs - standard for all platforms
#define BLE_SERVICE_UUID         "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_COMMAND_CHAR_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_STATUS_CHAR_UUID     "beb5483f-36e1-4688-b7f5-ea07361b26a8"

// Platform-specific includes
#if defined(ESP32)
  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
  #define ARDUROOMBA_BLE_PLATFORM "ESP32"
#elif defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI)
  #include <ArduinoBLE.h>
  #define ARDUROOMBA_BLE_PLATFORM "Uno R4 WiFi"
#else
  #define ARDUROOMBA_BLE_PLATFORM "None"
#endif

/**
 * BLE extension for Roomba control
 * Provides GATT server for mobile app connectivity
 */
class ArduRoombaBLE {
public:
  ArduRoombaBLE(ArduRoomba& roomba, const char* deviceName = "ArduRoomba");
  ~ArduRoombaBLE();

  // BLE setup
  bool begin();
  void end();

  // Status
  bool isConnected() const { return _deviceConnected; }
  int getConnectionCount() const { return _connectionCount; }
  bool isAvailable() const;

  // Update status (call in loop to send notifications)
  void updateStatus();

  // Command callback
  typedef void (*CommandCallback)(const String& command);
  void setCommandCallback(CommandCallback callback) { _commandCallback = callback; }

  // Enable/disable remote control
  void enableRemoteControl(bool enable) { _remoteEnabled = enable; }
  bool isRemoteEnabled() const { return _remoteEnabled; }

  // Device info
  const char* getDeviceName() const { return _deviceName.c_str(); }
  const char* getPlatform() const { return ARDUROOMBA_BLE_PLATFORM; }

private:
  ArduRoomba& _roomba;
  String _deviceName;
  bool _remoteEnabled;
  bool _deviceConnected;
  bool _oldDeviceConnected;
  int _connectionCount;
  CommandCallback _commandCallback;
  uint16_t _lowBatteryThreshold;

  unsigned long _lastStatusUpdate;
  static const unsigned long STATUS_UPDATE_INTERVAL = 2000; // 2 seconds

  // Platform-specific implementation data
  void* _bleServer;      // BLEServer* for ESP32, BLEService* for Uno R4
  void* _bleService;     // BLEService* for ESP32
  void* _commandChar;    // BLECharacteristic*
  void* _statusChar;     // BLECharacteristic*

  // Platform-specific methods
  void processCommand(const String& command);
  String generateStatus();
  void initPlatformBLE();
  void advertisePlatformBLE();

#if defined(ESP32)
  // ESP32 specific
  class ServerCallbacks;
  class CommandCallbacks;
  friend class ServerCallbacks;
  friend class CommandCallbacks;
#elif defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI)
  // Uno R4 WiFi specific
  static ArduRoombaBLE* _instance; // For static callback
  void onCentralConnected(BLEDevice central);
  void onCentralDisconnected(BLEDevice central);
  void onCommandWritten(BLEDevice central, BLECharacteristic characteristic);
#endif
};

#endif
