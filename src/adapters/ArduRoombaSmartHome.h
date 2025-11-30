/**
 * ArduRoombaSmartHome.h - Base class for smart home adapters
 *
 * This is a base class that defines the interface for smart home integrations.
 * Concrete implementations include Home Assistant (MQTT) and Alexa (Fauxmo).
 *
 * Part of the ArduRoomba library - Smart Home Adapters
 * https://github.com/pkyanam/ArduRoomba
 */

#ifndef ARDUROOMBA_SMARTHOME_H
#define ARDUROOMBA_SMARTHOME_H

#include <Arduino.h>
#include "../ArduRoomba.h"

// Smart home command types
enum class SmartHomeCommand {
  START_CLEANING,
  STOP,
  DOCK,
  SPOT_CLEAN,
  FORWARD,
  BACKWARD,
  TURN_LEFT,
  TURN_RIGHT,
  BEEP,
  UNKNOWN
};

// Roomba state for smart home reporting
struct RoombaState {
  bool cleaning;
  bool docked;
  uint16_t batteryVoltage;
  int16_t batteryCurrent;
  bool bumperPressed;
  bool wallDetected;
  bool connected;
};

/**
 * Base class for smart home adapters
 *
 * This class defines the interface that all smart home adapters must implement.
 * It uses composition with ArduRoomba rather than inheritance.
 */
class ArduRoombaSmartHome {
public:
  /**
   * Constructor
   * @param roomba Reference to ArduRoomba instance
   */
  ArduRoombaSmartHome(ArduRoomba& roomba) : _roomba(roomba), _enabled(false), _debug(false) {}

  virtual ~ArduRoombaSmartHome() {}

  /**
   * Initialize the smart home adapter
   * @return true if initialization successful
   */
  virtual bool begin() = 0;

  /**
   * Shutdown the adapter
   */
  virtual void end() = 0;

  /**
   * Process events - call this in loop()
   */
  virtual void handle() = 0;

  /**
   * Check if adapter is connected to smart home service
   * @return true if connected
   */
  virtual bool isConnected() const = 0;

  /**
   * Get current Roomba state for smart home reporting
   * @return RoombaState structure
   */
  RoombaState getState() {
    RoombaState state;
    state.batteryVoltage = _roomba.getBatteryVoltage();
    state.batteryCurrent = _roomba.getBatteryCurrent();
    state.bumperPressed = _roomba.isBumperPressed();
    state.wallDetected = _roomba.isWallDetected();
    state.cleaning = _cleaning;
    state.docked = _docked;
    state.connected = _roomba.isConnected();
    return state;
  }

  /**
   * Enable/disable remote control via smart home
   * @param enable true to enable remote control
   */
  void enableRemoteControl(bool enable) { _enabled = enable; }

  /**
   * Check if remote control is enabled
   * @return true if enabled
   */
  bool isRemoteControlEnabled() const { return _enabled; }

  /**
   * Enable debug output
   * @param enable true to enable debug messages
   */
  void setDebug(bool enable) { _debug = enable; }

  /**
   * Set command callback for custom handling
   * @param callback Function to call when command received
   */
  void setCommandCallback(void (*callback)(SmartHomeCommand cmd)) {
    _commandCallback = callback;
  }

protected:
  ArduRoomba& _roomba;
  bool _enabled;
  bool _debug;
  bool _cleaning = false;
  bool _docked = false;
  void (*_commandCallback)(SmartHomeCommand) = nullptr;

  /**
   * Execute a smart home command
   * @param cmd Command to execute
   */
  void executeCommand(SmartHomeCommand cmd) {
    if (!_enabled) {
      debugPrint("Remote control disabled, ignoring command");
      return;
    }

    // Call custom callback if set
    if (_commandCallback) {
      _commandCallback(cmd);
    }

    switch (cmd) {
      case SmartHomeCommand::START_CLEANING:
        debugPrint("Starting cleaning");
        _roomba.startCleaning();
        _cleaning = true;
        _docked = false;
        break;

      case SmartHomeCommand::STOP:
        debugPrint("Stopping");
        _roomba.stop();
        _cleaning = false;
        break;

      case SmartHomeCommand::DOCK:
        debugPrint("Docking");
        _roomba.dock();
        _cleaning = false;
        _docked = true;
        break;

      case SmartHomeCommand::SPOT_CLEAN:
        debugPrint("Spot cleaning");
        _roomba.spotClean();
        _cleaning = true;
        _docked = false;
        break;

      case SmartHomeCommand::FORWARD:
        debugPrint("Moving forward");
        _roomba.moveForward();
        break;

      case SmartHomeCommand::BACKWARD:
        debugPrint("Moving backward");
        _roomba.moveBackward();
        break;

      case SmartHomeCommand::TURN_LEFT:
        debugPrint("Turning left");
        _roomba.turnLeft();
        break;

      case SmartHomeCommand::TURN_RIGHT:
        debugPrint("Turning right");
        _roomba.turnRight();
        break;

      case SmartHomeCommand::BEEP:
        debugPrint("Beeping");
        _roomba.beep();
        break;

      default:
        debugPrint("Unknown command");
        break;
    }
  }

  /**
   * Debug print helper
   * @param msg Message to print
   */
  void debugPrint(const char* msg) {
    if (_debug) {
      Serial.print("SmartHome: ");
      Serial.println(msg);
    }
  }

  /**
   * Debug print helper with value
   * @param msg Message to print
   * @param value Value to append
   */
  void debugPrint(const char* msg, int value) {
    if (_debug) {
      Serial.print("SmartHome: ");
      Serial.print(msg);
      Serial.println(value);
    }
  }
};

#endif // ARDUROOMBA_SMARTHOME_H
