/**
 * ArduRoombaHomeAssistant.cpp - Home Assistant MQTT adapter implementation
 *
 * Part of the ArduRoomba library - Smart Home Adapters
 * https://github.com/pkyanam/ArduRoomba
 */

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)

#include "ArduRoombaHomeAssistant.h"

// Static instance pointer for callback
ArduRoombaHomeAssistant* ArduRoombaHomeAssistant::_instance = nullptr;

ArduRoombaHomeAssistant::ArduRoombaHomeAssistant(ArduRoomba& roomba)
  : ArduRoombaSmartHome(roomba), _mqttClient(_wifiClient) {
  _instance = this;
  buildTopics();
}

bool ArduRoombaHomeAssistant::begin(const char* mqttServer, uint16_t mqttPort,
                                     const char* mqttUser, const char* mqttPassword) {
  _mqttServer = mqttServer;
  _mqttPort = mqttPort;
  _mqttUser = mqttUser;
  _mqttPassword = mqttPassword;

  return begin();
}

bool ArduRoombaHomeAssistant::begin() {
  if (!_mqttServer) {
    debugPrint("MQTT server not set");
    return false;
  }

  debugPrint("Initializing Home Assistant adapter");

  _mqttClient.setServer(_mqttServer, _mqttPort);
  _mqttClient.setCallback(mqttCallback);
  _mqttClient.setBufferSize(512);  // Larger buffer for discovery messages

  // Enable remote control by default
  _enabled = true;

  return reconnect();
}

void ArduRoombaHomeAssistant::end() {
  if (_mqttClient.connected()) {
    // Publish offline status
    _mqttClient.publish(_availabilityTopic, "offline", true);
    _mqttClient.disconnect();
  }
  debugPrint("Home Assistant adapter stopped");
}

void ArduRoombaHomeAssistant::handle() {
  if (!_mqttClient.connected()) {
    unsigned long now = millis();
    if (now - _lastReconnectAttempt > 5000) {
      _lastReconnectAttempt = now;
      if (reconnect()) {
        _lastReconnectAttempt = 0;
      }
    }
  } else {
    _mqttClient.loop();

    // Periodic state updates
    unsigned long now = millis();
    if (now - _lastStateUpdate > _updateInterval) {
      _lastStateUpdate = now;
      publishState();
    }
  }
}

bool ArduRoombaHomeAssistant::isConnected() const {
  return _mqttClient.connected();
}

void ArduRoombaHomeAssistant::setMQTTServer(const char* server, uint16_t port) {
  _mqttServer = server;
  _mqttPort = port;
}

void ArduRoombaHomeAssistant::setMQTTCredentials(const char* user, const char* password) {
  _mqttUser = user;
  _mqttPassword = password;
}

void ArduRoombaHomeAssistant::setDeviceId(const char* deviceId) {
  strncpy(_deviceId, deviceId, sizeof(_deviceId) - 1);
  _deviceId[sizeof(_deviceId) - 1] = '\0';
  buildTopics();
}

void ArduRoombaHomeAssistant::setDeviceName(const char* name) {
  strncpy(_deviceName, name, sizeof(_deviceName) - 1);
  _deviceName[sizeof(_deviceName) - 1] = '\0';
}

void ArduRoombaHomeAssistant::setUpdateInterval(unsigned long intervalMs) {
  _updateInterval = intervalMs;
}

void ArduRoombaHomeAssistant::buildTopics() {
  snprintf(_stateTopic, sizeof(_stateTopic), "%s/state", _deviceId);
  snprintf(_commandTopic, sizeof(_commandTopic), "%s/command", _deviceId);
  snprintf(_availabilityTopic, sizeof(_availabilityTopic), "%s/availability", _deviceId);
  snprintf(_batteryTopic, sizeof(_batteryTopic), "%s/battery", _deviceId);
}

bool ArduRoombaHomeAssistant::reconnect() {
  debugPrint("Connecting to MQTT broker...");

  String clientId = String(_deviceId) + "_" + String(random(0xffff), HEX);

  bool connected = false;
  if (_mqttUser && _mqttPassword) {
    connected = _mqttClient.connect(clientId.c_str(), _mqttUser, _mqttPassword,
                                     _availabilityTopic, 0, true, "offline");
  } else {
    connected = _mqttClient.connect(clientId.c_str(), _availabilityTopic, 0, true, "offline");
  }

  if (connected) {
    debugPrint("Connected to MQTT broker");

    // Publish online status
    _mqttClient.publish(_availabilityTopic, "online", true);

    // Subscribe to command topic
    _mqttClient.subscribe(_commandTopic);
    debugPrint("Subscribed to command topic");

    // Publish discovery configuration
    publishDiscovery();

    // Publish initial state
    publishState();

    return true;
  } else {
    debugPrint("MQTT connection failed, rc=", _mqttClient.state());
    return false;
  }
}

void ArduRoombaHomeAssistant::mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (_instance) {
    _instance->handleMessage(topic, payload, length);
  }
}

void ArduRoombaHomeAssistant::handleMessage(char* topic, byte* payload, unsigned int length) {
  // Null-terminate the payload
  char message[64];
  size_t copyLen = min((size_t)length, sizeof(message) - 1);
  memcpy(message, payload, copyLen);
  message[copyLen] = '\0';

  debugPrint("Received command: ");
  if (_debug) Serial.println(message);

  // Parse and execute command
  SmartHomeCommand cmd = parseCommand(message);
  executeCommand(cmd);

  // Publish updated state
  publishState();
}

SmartHomeCommand ArduRoombaHomeAssistant::parseCommand(const char* cmd) {
  if (strcmp(cmd, "start") == 0 || strcmp(cmd, "clean") == 0) {
    return SmartHomeCommand::START_CLEANING;
  } else if (strcmp(cmd, "stop") == 0 || strcmp(cmd, "pause") == 0) {
    return SmartHomeCommand::STOP;
  } else if (strcmp(cmd, "return_to_base") == 0 || strcmp(cmd, "dock") == 0) {
    return SmartHomeCommand::DOCK;
  } else if (strcmp(cmd, "locate") == 0 || strcmp(cmd, "beep") == 0) {
    return SmartHomeCommand::BEEP;
  } else if (strcmp(cmd, "spot") == 0) {
    return SmartHomeCommand::SPOT_CLEAN;
  } else if (strcmp(cmd, "forward") == 0) {
    return SmartHomeCommand::FORWARD;
  } else if (strcmp(cmd, "backward") == 0) {
    return SmartHomeCommand::BACKWARD;
  } else if (strcmp(cmd, "turn_left") == 0) {
    return SmartHomeCommand::TURN_LEFT;
  } else if (strcmp(cmd, "turn_right") == 0) {
    return SmartHomeCommand::TURN_RIGHT;
  }
  return SmartHomeCommand::UNKNOWN;
}

const char* ArduRoombaHomeAssistant::getStateString() {
  if (_docked) return "docked";
  if (_cleaning) return "cleaning";
  return "idle";
}

void ArduRoombaHomeAssistant::publishState() {
  if (!_mqttClient.connected()) return;

  RoombaState state = getState();

  // Build JSON state payload
  char payload[256];
  snprintf(payload, sizeof(payload),
    "{"
    "\"state\":\"%s\","
    "\"battery_level\":%d,"
    "\"battery_voltage\":%u,"
    "\"battery_current\":%d,"
    "\"wall_detected\":%s,"
    "\"bumper_pressed\":%s,"
    "\"connected\":%s"
    "}",
    getStateString(),
    map(state.batteryVoltage, 12000, 16800, 0, 100),  // Approximate battery %
    state.batteryVoltage,
    state.batteryCurrent,
    state.wallDetected ? "true" : "false",
    state.bumperPressed ? "true" : "false",
    state.connected ? "true" : "false"
  );

  _mqttClient.publish(_stateTopic, payload, true);

  // Also publish battery level separately for battery sensor
  char batteryPayload[16];
  snprintf(batteryPayload, sizeof(batteryPayload), "%d",
           constrain(map(state.batteryVoltage, 12000, 16800, 0, 100), 0, 100));
  _mqttClient.publish(_batteryTopic, batteryPayload, true);

  debugPrint("Published state");
}

void ArduRoombaHomeAssistant::publishDiscovery() {
  publishVacuumDiscovery();
  publishSensorDiscovery();
  debugPrint("Published discovery config");
}

void ArduRoombaHomeAssistant::publishVacuumDiscovery() {
  char topic[128];
  char payload[512];

  snprintf(topic, sizeof(topic),
    "%s/vacuum/%s/config", HA_DISCOVERY_PREFIX, _deviceId);

  snprintf(payload, sizeof(payload),
    "{"
    "\"name\":\"%s\","
    "\"unique_id\":\"%s_vacuum\","
    "\"state_topic\":\"%s\","
    "\"command_topic\":\"%s\","
    "\"availability_topic\":\"%s\","
    "\"payload_available\":\"online\","
    "\"payload_not_available\":\"offline\","
    "\"value_template\":\"{{ value_json.state }}\","
    "\"supported_features\":[\"start\",\"stop\",\"return_home\",\"locate\"],"
    "\"device\":{"
      "\"identifiers\":[\"%s\"],"
      "\"name\":\"%s\","
      "\"model\":\"ArduRoomba\","
      "\"manufacturer\":\"ArduRoomba\","
      "\"sw_version\":\"3.1.0\""
    "}"
    "}",
    _deviceName,
    _deviceId,
    _stateTopic,
    _commandTopic,
    _availabilityTopic,
    _deviceId,
    _deviceName
  );

  _mqttClient.publish(topic, payload, true);
}

void ArduRoombaHomeAssistant::publishSensorDiscovery() {
  char topic[128];
  char payload[512];

  // Battery sensor
  snprintf(topic, sizeof(topic),
    "%s/sensor/%s_battery/config", HA_DISCOVERY_PREFIX, _deviceId);

  snprintf(payload, sizeof(payload),
    "{"
    "\"name\":\"%s Battery\","
    "\"unique_id\":\"%s_battery\","
    "\"state_topic\":\"%s\","
    "\"unit_of_measurement\":\"%%\","
    "\"device_class\":\"battery\","
    "\"availability_topic\":\"%s\","
    "\"device\":{"
      "\"identifiers\":[\"%s\"]"
    "}"
    "}",
    _deviceName,
    _deviceId,
    _batteryTopic,
    _availabilityTopic,
    _deviceId
  );

  _mqttClient.publish(topic, payload, true);

  // Wall sensor (binary)
  snprintf(topic, sizeof(topic),
    "%s/binary_sensor/%s_wall/config", HA_DISCOVERY_PREFIX, _deviceId);

  snprintf(payload, sizeof(payload),
    "{"
    "\"name\":\"%s Wall Sensor\","
    "\"unique_id\":\"%s_wall\","
    "\"state_topic\":\"%s\","
    "\"value_template\":\"{{ value_json.wall_detected }}\","
    "\"payload_on\":\"true\","
    "\"payload_off\":\"false\","
    "\"device_class\":\"occupancy\","
    "\"availability_topic\":\"%s\","
    "\"device\":{"
      "\"identifiers\":[\"%s\"]"
    "}"
    "}",
    _deviceName,
    _deviceId,
    _stateTopic,
    _availabilityTopic,
    _deviceId
  );

  _mqttClient.publish(topic, payload, true);

  // Bumper sensor (binary)
  snprintf(topic, sizeof(topic),
    "%s/binary_sensor/%s_bumper/config", HA_DISCOVERY_PREFIX, _deviceId);

  snprintf(payload, sizeof(payload),
    "{"
    "\"name\":\"%s Bumper\","
    "\"unique_id\":\"%s_bumper\","
    "\"state_topic\":\"%s\","
    "\"value_template\":\"{{ value_json.bumper_pressed }}\","
    "\"payload_on\":\"true\","
    "\"payload_off\":\"false\","
    "\"device_class\":\"problem\","
    "\"availability_topic\":\"%s\","
    "\"device\":{"
      "\"identifiers\":[\"%s\"]"
    "}"
    "}",
    _deviceName,
    _deviceId,
    _stateTopic,
    _availabilityTopic,
    _deviceId
  );

  _mqttClient.publish(topic, payload, true);
}

#endif // ESP32 || ESP8266 || ARDUINO_UNOWIFIR4
