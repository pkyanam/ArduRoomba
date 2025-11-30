/**
 * ArduRoombaHomeAssistant.h - Home Assistant MQTT adapter for ArduRoomba
 *
 * This adapter enables ArduRoomba integration with Home Assistant via MQTT.
 * Features:
 * - MQTT auto-discovery for automatic entity creation
 * - Vacuum entity with standard commands (start, stop, return_to_base, etc.)
 * - Sensor entities for battery, wall detection, bumper state
 * - State reporting and command handling
 *
 * Requires: PubSubClient library (by Nick O'Leary)
 *
 * Part of the ArduRoomba library - Smart Home Adapters
 * https://github.com/pkyanam/ArduRoomba
 */

#ifndef ARDUROOMBA_HOMEASSISTANT_H
#define ARDUROOMBA_HOMEASSISTANT_H

// Only compile for WiFi-capable boards
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)

#include "ArduRoombaSmartHome.h"

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_UNOWIFIR4)
  #include <WiFiS3.h>
#endif

#include <PubSubClient.h>

// Default MQTT topics
#define HA_DISCOVERY_PREFIX "homeassistant"
#define ARDUROOMBA_DEVICE_ID "arduroomba"

/**
 * Home Assistant MQTT adapter for ArduRoomba
 *
 * Provides integration with Home Assistant through MQTT protocol.
 * Supports MQTT auto-discovery for automatic entity configuration.
 */
class ArduRoombaHomeAssistant : public ArduRoombaSmartHome {
public:
  /**
   * Constructor
   * @param roomba Reference to ArduRoomba instance
   */
  ArduRoombaHomeAssistant(ArduRoomba& roomba);

  /**
   * Initialize MQTT connection
   * @param mqttServer MQTT broker address
   * @param mqttPort MQTT broker port (default 1883)
   * @param mqttUser MQTT username (optional)
   * @param mqttPassword MQTT password (optional)
   * @return true if initialization successful
   */
  bool begin(const char* mqttServer, uint16_t mqttPort = 1883,
             const char* mqttUser = nullptr, const char* mqttPassword = nullptr);

  /**
   * Initialize with default parameters (call setMQTTServer first)
   * @return true if initialization successful
   */
  bool begin() override;

  /**
   * Disconnect and cleanup
   */
  void end() override;

  /**
   * Handle MQTT events - call this in loop()
   */
  void handle() override;

  /**
   * Check if connected to MQTT broker
   * @return true if connected
   */
  bool isConnected() const override;

  /**
   * Set MQTT server details
   * @param server MQTT broker address
   * @param port MQTT broker port
   */
  void setMQTTServer(const char* server, uint16_t port = 1883);

  /**
   * Set MQTT credentials
   * @param user MQTT username
   * @param password MQTT password
   */
  void setMQTTCredentials(const char* user, const char* password);

  /**
   * Set custom device ID (used in topics and discovery)
   * @param deviceId Unique device identifier
   */
  void setDeviceId(const char* deviceId);

  /**
   * Set custom device name (displayed in Home Assistant)
   * @param name Human-readable device name
   */
  void setDeviceName(const char* name);

  /**
   * Publish current state to Home Assistant
   */
  void publishState();

  /**
   * Publish auto-discovery configuration
   * This is automatically called on connect, but can be called manually
   */
  void publishDiscovery();

  /**
   * Set state update interval
   * @param intervalMs Milliseconds between state updates (default 10000)
   */
  void setUpdateInterval(unsigned long intervalMs);

private:
  WiFiClient _wifiClient;
  PubSubClient _mqttClient;

  const char* _mqttServer = nullptr;
  uint16_t _mqttPort = 1883;
  const char* _mqttUser = nullptr;
  const char* _mqttPassword = nullptr;

  char _deviceId[32] = ARDUROOMBA_DEVICE_ID;
  char _deviceName[32] = "ArduRoomba";

  unsigned long _lastStateUpdate = 0;
  unsigned long _updateInterval = 10000;
  unsigned long _lastReconnectAttempt = 0;

  // Topic buffers
  char _stateTopic[64];
  char _commandTopic[64];
  char _availabilityTopic[64];
  char _batteryTopic[64];

  /**
   * MQTT message callback
   */
  static void mqttCallback(char* topic, byte* payload, unsigned int length);
  static ArduRoombaHomeAssistant* _instance;

  /**
   * Handle incoming MQTT message
   */
  void handleMessage(char* topic, byte* payload, unsigned int length);

  /**
   * Parse command string to SmartHomeCommand
   */
  SmartHomeCommand parseCommand(const char* cmd);

  /**
   * Attempt to reconnect to MQTT broker
   */
  bool reconnect();

  /**
   * Build topic strings
   */
  void buildTopics();

  /**
   * Get state string for Home Assistant
   */
  const char* getStateString();

  /**
   * Publish vacuum entity discovery config
   */
  void publishVacuumDiscovery();

  /**
   * Publish sensor discovery configs
   */
  void publishSensorDiscovery();
};

#else
  #error "ArduRoombaHomeAssistant requires a WiFi-capable board (ESP32, ESP8266, or Arduino Uno R4 WiFi)"
#endif

#endif // ARDUROOMBA_HOMEASSISTANT_H
