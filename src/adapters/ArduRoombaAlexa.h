/**
 * ArduRoombaAlexa.h - Amazon Alexa adapter for ArduRoomba
 *
 * This adapter enables ArduRoomba control via Amazon Alexa using Fauxmo.
 * Fauxmo emulates Belkin WeMo smart switches, allowing local control
 * without requiring any cloud services or Alexa skills.
 *
 * Features:
 * - Multiple virtual devices for different commands
 * - Local network control (no cloud required)
 * - Works with any Alexa-enabled device
 * - Zero configuration needed on Alexa side
 *
 * Requires: fauxmoESP library (by Xose Pérez)
 *
 * Default voice commands (discoverable by Alexa):
 * - "Alexa, turn on Roomba" - Start cleaning
 * - "Alexa, turn off Roomba" - Stop/Pause
 * - "Alexa, turn on Roomba Dock" - Return to dock
 * - "Alexa, turn on Roomba Spot" - Spot clean
 * - "Alexa, turn on Roomba Locate" - Beep to locate
 *
 * Part of the ArduRoomba library - Smart Home Adapters
 * https://github.com/pkyanam/ArduRoomba
 */

#ifndef ARDUROOMBA_ALEXA_H
#define ARDUROOMBA_ALEXA_H

// Only compile for ESP32 and ESP8266 (fauxmoESP requirement)
#if defined(ESP32) || defined(ESP8266)

#include "ArduRoombaSmartHome.h"

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include <fauxmoESP.h>

// Default device names (can be customized)
#define ALEXA_DEVICE_MAIN "Roomba"
#define ALEXA_DEVICE_DOCK "Roomba Dock"
#define ALEXA_DEVICE_SPOT "Roomba Spot"
#define ALEXA_DEVICE_LOCATE "Roomba Locate"

/**
 * Amazon Alexa adapter for ArduRoomba
 *
 * Provides voice control integration with Amazon Alexa through
 * Belkin WeMo emulation (fauxmoESP). No cloud services required.
 */
class ArduRoombaAlexa : public ArduRoombaSmartHome {
public:
  /**
   * Constructor
   * @param roomba Reference to ArduRoomba instance
   */
  ArduRoombaAlexa(ArduRoomba& roomba);

  /**
   * Initialize Alexa adapter with default device names
   * @return true if initialization successful
   */
  bool begin() override;

  /**
   * Initialize Alexa adapter with custom main device name
   * @param mainDeviceName Name for the main Roomba device
   * @return true if initialization successful
   */
  bool begin(const char* mainDeviceName);

  /**
   * Shutdown the adapter
   */
  void end() override;

  /**
   * Handle Alexa events - call this in loop()
   */
  void handle() override;

  /**
   * Check if Alexa adapter is ready
   * @return true if WiFi connected and adapter initialized
   */
  bool isConnected() const override;

  /**
   * Set custom device name for main Roomba control
   * Must be called before begin()
   * @param name Device name (what you say to Alexa)
   */
  void setMainDeviceName(const char* name);

  /**
   * Set custom device name for dock command
   * Must be called before begin()
   * @param name Device name
   */
  void setDockDeviceName(const char* name);

  /**
   * Set custom device name for spot clean command
   * Must be called before begin()
   * @param name Device name
   */
  void setSpotDeviceName(const char* name);

  /**
   * Set custom device name for locate (beep) command
   * Must be called before begin()
   * @param name Device name
   */
  void setLocateDeviceName(const char* name);

  /**
   * Enable/disable additional command devices
   * When disabled, only the main device is created
   * Must be called before begin()
   * @param enable true to enable extra devices (default: true)
   */
  void enableExtraDevices(bool enable);

  /**
   * Add a custom device with callback
   * @param name Device name
   * @param onCallback Function called when turned ON
   * @param offCallback Function called when turned OFF
   * @return Device ID or 0 if failed
   */
  unsigned char addDevice(const char* name,
                          void (*onCallback)() = nullptr,
                          void (*offCallback)() = nullptr);

  /**
   * Trigger device discovery
   * Alexa should auto-discover, but this can help in some cases
   */
  void triggerDiscovery();

  /**
   * Get reference to underlying fauxmoESP instance
   * For advanced configuration
   * @return Reference to fauxmoESP
   */
  fauxmoESP& getFauxmo();

private:
  fauxmoESP _fauxmo;
  bool _initialized = false;
  bool _extraDevices = true;

  const char* _mainDeviceName = ALEXA_DEVICE_MAIN;
  const char* _dockDeviceName = ALEXA_DEVICE_DOCK;
  const char* _spotDeviceName = ALEXA_DEVICE_SPOT;
  const char* _locateDeviceName = ALEXA_DEVICE_LOCATE;

  // Device IDs
  unsigned char _mainDeviceId = 0;
  unsigned char _dockDeviceId = 0;
  unsigned char _spotDeviceId = 0;
  unsigned char _locateDeviceId = 0;

  // Custom device callbacks
  static const int MAX_CUSTOM_DEVICES = 4;
  struct CustomDevice {
    unsigned char id;
    void (*onCallback)();
    void (*offCallback)();
  };
  CustomDevice _customDevices[MAX_CUSTOM_DEVICES];
  int _customDeviceCount = 0;

  // Static callback for fauxmo
  static void fauxmoCallback(unsigned char device_id, const char* device_name,
                             bool state, unsigned char value);
  static ArduRoombaAlexa* _instance;

  /**
   * Handle device state change
   */
  void handleStateChange(unsigned char device_id, const char* device_name,
                         bool state, unsigned char value);

  /**
   * Setup default devices
   */
  void setupDevices();
};

#else
  #error "ArduRoombaAlexa requires ESP32 or ESP8266 (fauxmoESP requirement)"
#endif

#endif // ARDUROOMBA_ALEXA_H
