/**
 * ArduRoombaAlexa.cpp - Amazon Alexa adapter implementation
 *
 * Part of the ArduRoomba library - Smart Home Adapters
 * https://github.com/pkyanam/ArduRoomba
 */

#if defined(ESP32) || defined(ESP8266)

#include "ArduRoombaAlexa.h"

// Static instance pointer for callback
ArduRoombaAlexa* ArduRoombaAlexa::_instance = nullptr;

ArduRoombaAlexa::ArduRoombaAlexa(ArduRoomba& roomba)
  : ArduRoombaSmartHome(roomba) {
  _instance = this;

  // Initialize custom device array
  for (int i = 0; i < MAX_CUSTOM_DEVICES; i++) {
    _customDevices[i].id = 0;
    _customDevices[i].onCallback = nullptr;
    _customDevices[i].offCallback = nullptr;
  }
}

bool ArduRoombaAlexa::begin() {
  return begin(_mainDeviceName);
}

bool ArduRoombaAlexa::begin(const char* mainDeviceName) {
  if (!WiFi.isConnected()) {
    debugPrint("WiFi not connected");
    return false;
  }

  _mainDeviceName = mainDeviceName;

  debugPrint("Initializing Alexa adapter");

  // Enable fauxmo
  _fauxmo.createServer(true);
  _fauxmo.setPort(80);
  _fauxmo.enable(true);

  // Set callback
  _fauxmo.onSetState(fauxmoCallback);

  // Setup default devices
  setupDevices();

  // Enable remote control by default
  _enabled = true;
  _initialized = true;

  debugPrint("Alexa adapter ready");
  debugPrint("Say 'Alexa, discover devices' to find your Roomba");

  return true;
}

void ArduRoombaAlexa::end() {
  _fauxmo.enable(false);
  _initialized = false;
  debugPrint("Alexa adapter stopped");
}

void ArduRoombaAlexa::handle() {
  if (_initialized) {
    _fauxmo.handle();
  }
}

bool ArduRoombaAlexa::isConnected() const {
  return _initialized && WiFi.isConnected();
}

void ArduRoombaAlexa::setMainDeviceName(const char* name) {
  _mainDeviceName = name;
}

void ArduRoombaAlexa::setDockDeviceName(const char* name) {
  _dockDeviceName = name;
}

void ArduRoombaAlexa::setSpotDeviceName(const char* name) {
  _spotDeviceName = name;
}

void ArduRoombaAlexa::setLocateDeviceName(const char* name) {
  _locateDeviceName = name;
}

void ArduRoombaAlexa::enableExtraDevices(bool enable) {
  _extraDevices = enable;
}

void ArduRoombaAlexa::setupDevices() {
  // Main Roomba device - ON starts cleaning, OFF stops
  _mainDeviceId = _fauxmo.addDevice(_mainDeviceName);
  debugPrint("Added main device: ");
  if (_debug) Serial.println(_mainDeviceName);

  if (_extraDevices) {
    // Dock device - ON sends to dock
    _dockDeviceId = _fauxmo.addDevice(_dockDeviceName);
    debugPrint("Added dock device: ");
    if (_debug) Serial.println(_dockDeviceName);

    // Spot clean device - ON starts spot clean
    _spotDeviceId = _fauxmo.addDevice(_spotDeviceName);
    debugPrint("Added spot device: ");
    if (_debug) Serial.println(_spotDeviceName);

    // Locate device - ON beeps
    _locateDeviceId = _fauxmo.addDevice(_locateDeviceName);
    debugPrint("Added locate device: ");
    if (_debug) Serial.println(_locateDeviceName);
  }
}

unsigned char ArduRoombaAlexa::addDevice(const char* name,
                                          void (*onCallback)(),
                                          void (*offCallback)()) {
  if (_customDeviceCount >= MAX_CUSTOM_DEVICES) {
    debugPrint("Max custom devices reached");
    return 0;
  }

  unsigned char id = _fauxmo.addDevice(name);
  if (id) {
    _customDevices[_customDeviceCount].id = id;
    _customDevices[_customDeviceCount].onCallback = onCallback;
    _customDevices[_customDeviceCount].offCallback = offCallback;
    _customDeviceCount++;

    debugPrint("Added custom device: ");
    if (_debug) Serial.println(name);
  }
  return id;
}

void ArduRoombaAlexa::triggerDiscovery() {
  // Fauxmo handles discovery automatically via SSDP
  // This method exists for API consistency
  debugPrint("Discovery is automatic - say 'Alexa, discover devices'");
}

fauxmoESP& ArduRoombaAlexa::getFauxmo() {
  return _fauxmo;
}

void ArduRoombaAlexa::fauxmoCallback(unsigned char device_id, const char* device_name,
                                      bool state, unsigned char value) {
  if (_instance) {
    _instance->handleStateChange(device_id, device_name, state, value);
  }
}

void ArduRoombaAlexa::handleStateChange(unsigned char device_id, const char* device_name,
                                         bool state, unsigned char value) {
  debugPrint("Device: ");
  if (_debug) {
    Serial.print(device_name);
    Serial.print(" -> ");
    Serial.println(state ? "ON" : "OFF");
  }

  // Check custom devices first
  for (int i = 0; i < _customDeviceCount; i++) {
    if (_customDevices[i].id == device_id) {
      if (state && _customDevices[i].onCallback) {
        _customDevices[i].onCallback();
      } else if (!state && _customDevices[i].offCallback) {
        _customDevices[i].offCallback();
      }
      return;
    }
  }

  // Handle built-in devices
  if (device_id == _mainDeviceId) {
    // Main device: ON = clean, OFF = stop
    if (state) {
      executeCommand(SmartHomeCommand::START_CLEANING);
    } else {
      executeCommand(SmartHomeCommand::STOP);
    }
  } else if (device_id == _dockDeviceId) {
    // Dock device: ON = dock
    if (state) {
      executeCommand(SmartHomeCommand::DOCK);
    }
  } else if (device_id == _spotDeviceId) {
    // Spot device: ON = spot clean
    if (state) {
      executeCommand(SmartHomeCommand::SPOT_CLEAN);
    }
  } else if (device_id == _locateDeviceId) {
    // Locate device: ON = beep
    if (state) {
      executeCommand(SmartHomeCommand::BEEP);
    }
  }
}

#endif // ESP32 || ESP8266
