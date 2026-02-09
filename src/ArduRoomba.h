/**
 * @file ArduRoomba.h
 * @brief Main ArduRoomba library interface (Refactored)
 *
 * Clean, modular interface for controlling iRobot Create 2 and compatible Roomba models.
 * Provides simple high-level methods while maintaining modularity for extensions.
 *
 * Architecture:
 * - RoombaSerial: Abstract serial communication (SoftwareSerial/HardwareSerial)
 * - RoombaSensors: Structured sensor data access
 * - RoombaMovement: Fluent movement control with sequences
 * - RoombaActuators: LED, motor, and sound control
 * - RoombaSafety: Battery monitoring and obstacle detection
 */

#ifndef ARDUROOMBA_H
#define ARDUROOMBA_H

#include <Arduino.h>

// Include core components
#include "core/RoombaSerial.h"
#include "core/RoombaSensors.h"
#include "core/RoombaMovement.h"
#include "core/RoombaActuators.h"

/**
 * Roomba configuration structure
 * Centralizes all configurable parameters
 */
struct RoombaConfig {
  uint8_t rxPin;           // RX pin for serial communication
  uint8_t txPin;           // TX pin for serial communication
  uint8_t brcPin;          // BRC (Baud Rate Change) / Device Detect pin
  uint32_t baudRate;       // Serial baud rate (default: 19200)
  bool useHardwareSerial;  // Use HardwareSerial if available
  HardwareSerial* hwSerial; // HardwareSerial instance (if useHardwareSerial)
  uint16_t lowBatteryThreshold;  // Low battery threshold in mV
  uint16_t criticalBatteryThreshold; // Critical battery threshold in mV
  bool enableSafety;       // Enable automatic safety features

  // Default configuration for software serial
  static RoombaConfig createDefault(uint8_t rx = 2, uint8_t tx = 3, uint8_t brc = 4) {
    RoombaConfig config;
    config.rxPin = rx;
    config.txPin = tx;
    config.brcPin = brc;
    config.baudRate = 19200;
    config.useHardwareSerial = false;
    config.hwSerial = nullptr;
    config.lowBatteryThreshold = 13000;
    config.criticalBatteryThreshold = 12000;
    config.enableSafety = true;
    return config;
  }

  #if defined(ESP32)
  // Configuration for ESP32 with hardware serial (Serial2 by default)
  static RoombaConfig createESP32(HardwareSerial* serial = &Serial2, uint8_t brc = 5) {
    RoombaConfig config;
    config.rxPin = 16;  // Default ESP32 RX2
    config.txPin = 17;  // Default ESP32 TX2
    config.brcPin = brc;
    config.baudRate = 19200;
    config.useHardwareSerial = true;
    config.hwSerial = serial;
    config.lowBatteryThreshold = 13000;
    config.criticalBatteryThreshold = 12000;
    config.enableSafety = true;
    return config;
  }
  #elif defined(ESP8266)
  // Configuration for ESP8266 with hardware serial
  static RoombaConfig createESP8266(uint8_t brc = 5) {
    RoombaConfig config;
    config.rxPin = 3;   // ESP8266 RX
    config.txPin = 1;   // ESP8266 TX
    config.brcPin = brc;
    config.baudRate = 19200;
    config.useHardwareSerial = true;
    config.hwSerial = &Serial;
    config.lowBatteryThreshold = 13000;
    config.criticalBatteryThreshold = 12000;
    config.enableSafety = true;
    return config;
  }
  #endif

  #if defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI)
  // Configuration for Uno R4 WiFi with hardware serial (Serial1)
  static RoombaConfig createUnoR4(uint8_t brc = 4) {
    RoombaConfig config;
    config.rxPin = 0;   // Serial1 RX on Uno R4
    config.txPin = 1;   // Serial1 TX on Uno R4
    config.brcPin = brc;
    config.baudRate = 19200;
    config.useHardwareSerial = true;
    config.hwSerial = &Serial1;
    config.lowBatteryThreshold = 13000;
    config.criticalBatteryThreshold = 12000;
    config.enableSafety = true;
    return config;
  }
  #endif
};

/**
 * Main ArduRoomba class
 * Provides unified access to all Roomba functionality
 */
class ArduRoomba {
public:
  // Constructor with pin configuration
  ArduRoomba(uint8_t rxPin, uint8_t txPin, uint8_t brcPin);
  ArduRoomba(const RoombaConfig& config);

  // Destructor
  ~ArduRoomba();

  // Initialization
  bool begin();
  bool begin(uint32_t baudRate);
  void end();
  bool isConnected() const { return _connected; }

  // Component access
  RoombaSensors& sensors() { return *_sensors; }
  RoombaMovement& movement() { return *_movement; }
  RoombaActuators& actuators() { return *_actuators; }
  RoombaSerial* serial() { return _serial; }

  // Quick access to common movement commands
  void stop() { _movement->stop(); }
  void moveForward(int16_t speed = 200) { _movement->moveForward(speed); }
  void moveBackward(int16_t speed = 200) { _movement->moveBackward(speed); }
  void turnLeft(int16_t speed = 200) { _movement->turnLeft(speed); }
  void turnRight(int16_t speed = 200) { _movement->turnRight(speed); }
  void spinLeft(int16_t speed = 200) { _movement->spinLeft(speed); }
  void spinRight(int16_t speed = 200) { _movement->spinRight(speed); }

  // Quick access to sensor readings
  uint16_t getBatteryVoltage() { return _sensors->getBatteryVoltage(); }
  int16_t getBatteryCurrent() { return _sensors->getBatteryCurrent(); }
  uint8_t getBatteryPercent() { return _sensors->getBatteryPercent(); }
  bool isBatteryLow() { return _sensors->isBatteryLow(); }
  bool isBatteryCritical() { return _sensors->isBatteryCritical(); }
  bool isBumperPressed() { return _sensors->isBumperPressed(); }
  bool isWallDetected() { return _sensors->isWallDetected(); }
  bool isCliffDetected() { return _sensors->isCliffDetected(); }

  // Quick access to actuator control
  void setLED(bool debris, bool spot, bool dock, bool checkRobot) {
    _actuators->setLED(debris, spot, dock, checkRobot);
  }
  void setPowerLED(uint8_t color, uint8_t intensity = 255) {
    _actuators->setPowerLED(color, intensity);
  }
  void setMotors(bool mainBrush, bool sideBrush, bool vacuum) {
    _actuators->setMotors(mainBrush, sideBrush, vacuum);
  }
  void beep() { _actuators->beep(); }

  // Cleaning modes
  void startCleaning() { _actuators->startCleaning(); }
  void spotClean() { _actuators->startSpotClean(); }
  void dock() { _actuators->seekDock(); }

  // Mode control
  void setSafeMode() { _actuators->setSafeMode(); }
  void setFullMode() { _actuators->setFullMode(); }

  // Configuration
  const RoombaConfig& getConfig() const { return _config; }
  void setConfig(const RoombaConfig& config) { _config = config; }

  // Debug
  void setDebug(bool enable);
  bool isDebug() const { return _debug; }

  // Safety features
  void enableSafety(bool enable) { _config.enableSafety = enable; }
  bool isSafetyEnabled() const { return _config.enableSafety; }
  void updateSafety(); // Call in loop() for automatic safety features

  // Legacy support - getOI() for backward compatibility
  class RoombaOI_Legacy;
  RoombaOI_Legacy& getOI();

private:
  RoombaConfig _config;
  RoombaSerial* _serial;
  RoombaSensors* _sensors;
  RoombaMovement* _movement;
  RoombaActuators* _actuators;
  RoombaOI_Legacy* _legacyOI;
  bool _connected;
  bool _debug;

  // Internal methods
  bool initSerial();
  void pulseBRC();
  void sendStartCommand();
};

/**
 * Legacy compatibility wrapper
 * Provides old RoombaOI interface using new components
 */
class ArduRoomba::RoombaOI_Legacy {
public:
  explicit RoombaOI_Legacy(ArduRoomba* parent) : _parent(parent) {}

  // Legacy methods mapped to new architecture
  void drive(int16_t velocity, int16_t radius) {
    _parent->_movement->drive(velocity, radius);
  }

  void driveDirect(int16_t rightVel, int16_t leftVel) {
    _parent->_movement->driveDirect(rightVel, leftVel);
  }

  void stop() { _parent->_movement->stop(); }

  void setMotors(bool mainBrush, bool sideBrush, bool vacuum) {
    _parent->_actuators->setMotors(mainBrush, sideBrush, vacuum);
  }

  void setLEDs(uint8_t ledBits, uint8_t powerColor, uint8_t powerIntensity) {
    _parent->_actuators->setAllLEDs(ledBits, powerColor, powerIntensity);
  }

  uint16_t getBatteryVoltage() { return _parent->_sensors->getBatteryVoltage(); }
  int16_t getBatteryCurrent() { return _parent->_sensors->getBatteryCurrent(); }
  bool isWallDetected() { return _parent->_sensors->isWallDetected(); }
  bool isBumperPressed() { return _parent->_sensors->isBumperPressed(); }

  void setDebug(bool enable) {
    _parent->_sensors->setDebug(enable);
    _parent->_movement->setDebug(enable);
    _parent->_actuators->setDebug(enable);
  }

  void sendCommand(uint8_t cmd) {
    if (_parent->_serial && _parent->_serial->isActive()) {
      _parent->_serial->write(cmd);
    }
  }

  void sendCommand(uint8_t cmd, const uint8_t* params, uint8_t numParams) {
    if (_parent->_serial && _parent->_serial->isActive() && params) {
      _parent->_serial->write(cmd);
      _parent->_serial->write(params, numParams);
    }
  }

private:
  ArduRoomba* _parent;
};

// Global sequence builder helper
inline RoombaSequence RoombaSequenceBuilder(ArduRoomba& roomba) {
  return RoombaSequence(&roomba.movement());
}

#endif
