/**
 * @file ArduRoombaBLE.cpp
 * @brief Implementation of BLE control for ESP32 and Uno R4 WiFi
 */

#include "ArduRoombaBLE.h"

#if defined(ESP32) || (defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI))

// ============================================================================
// Common implementation (shared by all platforms)
// ============================================================================

ArduRoombaBLE::ArduRoombaBLE(ArduRoomba& roomba, const char* deviceName)
  : _roomba(roomba), _deviceName(deviceName), _remoteEnabled(true),
    _deviceConnected(false), _oldDeviceConnected(false), _connectionCount(0),
    _commandCallback(nullptr), _lowBatteryThreshold(12000),
    _lastStatusUpdate(0),
    _bleServer(nullptr), _bleService(nullptr), _commandChar(nullptr), _statusChar(nullptr) {
}

ArduRoombaBLE::~ArduRoombaBLE() {
  end();
}

bool ArduRoombaBLE::isAvailable() const {
#if defined(ESP32)
  return true;
#elif defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI)
  return BLE.begin();
#else
  return false;
#endif
}

void ArduRoombaBLE::processCommand(const String& command) {
  if (!_remoteEnabled) {
    Serial.println("Remote control disabled");
    return;
  }

  // Call user callback if set
  if (_commandCallback) {
    _commandCallback(command);
  }

  // Parse command format: "ACTION:SPEED:DURATION"
  int firstColon = command.indexOf(':');
  int secondColon = command.indexOf(':', firstColon + 1);

  String action = command.substring(0, firstColon);
  int speed = 200;
  int duration = 0;

  if (firstColon > 0) {
    if (secondColon > firstColon) {
      speed = command.substring(firstColon + 1, secondColon).toInt();
      duration = command.substring(secondColon + 1).toInt();
    } else {
      speed = command.substring(firstColon + 1).toInt();
    }
  }

  // Check battery
  uint16_t voltage = _roomba.getBatteryVoltage();
  if (voltage > 0 && voltage < _lowBatteryThreshold) {
    Serial.println("Battery too low, ignoring command");
    _roomba.beep();
    return;
  }

  // Process commands
  if (action == "forward") {
    _roomba.moveForward(speed);
  }
  else if (action == "backward") {
    _roomba.moveBackward(speed);
  }
  else if (action == "left") {
    _roomba.turnLeft(speed);
  }
  else if (action == "right") {
    _roomba.turnRight(speed);
  }
  else if (action == "spinLeft") {
    _roomba.spinLeft(speed);
  }
  else if (action == "spinRight") {
    _roomba.spinRight(speed);
  }
  else if (action == "stop") {
    _roomba.stop();
  }
  else if (action == "clean") {
    _roomba.startCleaning();
  }
  else if (action == "spot") {
    _roomba.spotClean();
  }
  else if (action == "dock") {
    _roomba.dock();
  }
  else if (action == "beep") {
    _roomba.beep();
  }

  // Handle timed commands
  if (duration > 0 && action != "stop" && action != "clean" &&
      action != "spot" && action != "dock") {
    delay(duration);
    _roomba.stop();
  }
}

String ArduRoombaBLE::generateStatus() {
  uint16_t voltage = _roomba.getBatteryVoltage();
  bool connected = _roomba.isConnected();
  bool wall = _roomba.isWallDetected();
  bool bumper = _roomba.isBumperPressed();
  bool cliff = _roomba.isCliffDetected();

  // Format: "voltage:connected:wall:bumper:cliff:remote"
  String status = String(voltage) + ":";
  status += (connected ? "1" : "0") + ":";
  status += (wall ? "1" : "0") + ":";
  status += (bumper ? "1" : "0") + ":";
  status += (cliff ? "1" : "0") + ":";
  status += (_remoteEnabled ? "1" : "0");

  return status;
}

// ============================================================================
// ESP32 Implementation
// ============================================================================

#if defined(ESP32)

// Server callbacks
class ArduRoombaBLE::ServerCallbacks : public BLEServerCallbacks {
  ArduRoombaBLE* _parent;
public:
  ServerCallbacks(ArduRoombaBLE* parent) : _parent(parent) {}

  void onConnect(BLEServer* server) {
    _parent->_deviceConnected = true;
    _parent->_connectionCount++;
    Serial.println("BLE Client connected");
  }

  void onDisconnect(BLEServer* server) {
    _parent->_deviceConnected = false;
    Serial.println("BLE Client disconnected");

    // Restart advertising
    delay(500);
    server->startAdvertising();
    Serial.println("BLE Advertising restarted");
  }
};

// Command characteristic callbacks
class ArduRoombaBLE::CommandCallbacks : public BLECharacteristicCallbacks {
  ArduRoombaBLE* _parent;
public:
  CommandCallbacks(ArduRoombaBLE* parent) : _parent(parent) {}

  void onWrite(BLECharacteristic* characteristic) {
    String value = characteristic->getValue().c_str();
    if (value.length() > 0) {
      Serial.print("Received BLE command: ");
      Serial.println(value);
      _parent->processCommand(value);
    }
  }
};

void ArduRoombaBLE::initPlatformBLE() {
  BLEDevice::init(_deviceName.c_str());

  BLEServer* server = BLEDevice::createServer();
  _bleServer = server;
  server->setCallbacks(new ServerCallbacks(this));

  BLEService* service = server->createService(BLE_SERVICE_UUID);
  _bleService = service;

  BLECharacteristic* commandChar = service->createCharacteristic(
    BLE_COMMAND_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  _commandChar = commandChar;
  commandChar->setCallbacks(new CommandCallbacks(this));

  BLECharacteristic* statusChar = service->createCharacteristic(
    BLE_STATUS_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  _statusChar = statusChar;
  statusChar->addDescriptor(new BLE2902());

  service->start();
}

void ArduRoombaBLE::advertisePlatformBLE() {
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(BLE_SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(0x06);
  advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

bool ArduRoombaBLE::begin() {
  Serial.println("Initializing BLE (ESP32)...");

  initPlatformBLE();

  // Set initial status
  String status = generateStatus();
  ((BLECharacteristic*)_statusChar)->setValue(status.c_str());

  advertisePlatformBLE();

  Serial.println("BLE Service started");
  Serial.print("Device name: ");
  Serial.println(_deviceName);
  Serial.println("Waiting for connections...");

  return true;
}

void ArduRoombaBLE::end() {
  if (_bleServer) {
    BLEDevice::deinit(true);
    _bleServer = nullptr;
    _bleService = nullptr;
    _commandChar = nullptr;
    _statusChar = nullptr;
  }
}

void ArduRoombaBLE::updateStatus() {
  // Handle connection state changes
  if (_deviceConnected && !_oldDeviceConnected) {
    _oldDeviceConnected = _deviceConnected;
  }

  if (!_deviceConnected && _oldDeviceConnected) {
    _oldDeviceConnected = _deviceConnected;
  }

  // Periodically update status characteristic
  if (_deviceConnected && (millis() - _lastStatusUpdate > STATUS_UPDATE_INTERVAL)) {
    String status = generateStatus();
    ((BLECharacteristic*)_statusChar)->setValue(status.c_str());
    ((BLECharacteristic*)_statusChar)->notify();
    _lastStatusUpdate = millis();
  }
}

#endif // ESP32

// ============================================================================
// Uno R4 WiFi Implementation
// ============================================================================

#if defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI)

ArduRoombaBLE* ArduRoombaBLE::_instance = nullptr;

// Static callbacks for ArduinoBLE
void bleConnected(BLEDevice central) {
  if (ArduRoombaBLE::_instance) {
    ArduRoombaBLE::_instance->onCentralConnected(central);
  }
}

void bleDisconnected(BLEDevice central) {
  if (ArduRoombaBLE::_instance) {
    ArduRoombaBLE::_instance->onCentralDisconnected(central);
  }
}

void bleWritten(BLEDevice central, BLECharacteristic characteristic) {
  if (ArduRoombaBLE::_instance) {
    ArduRoombaBLE::_instance->onCommandWritten(central, characteristic);
  }
}

void ArduRoombaBLE::onCentralConnected(BLEDevice central) {
  _deviceConnected = true;
  _connectionCount++;
  Serial.print("BLE Client connected: ");
  Serial.println(central.address());
}

void ArduRoombaBLE::onCentralDisconnected(BLEDevice central) {
  _deviceConnected = false;
  Serial.print("BLE Client disconnected: ");
  Serial.println(central.address());

  // Restart advertising
  BLE.advertise();
  Serial.println("BLE Advertising restarted");
}

void ArduRoombaBLE::onCommandWritten(BLEDevice central, BLECharacteristic characteristic) {
  String value = characteristic.value();
  String valueStr = "";

  // Convert byte array to string
  for (int i = 0; i < characteristic.valueLength(); i++) {
    valueStr += (char)characteristic[i];
  }

  if (valueStr.length() > 0) {
    Serial.print("Received BLE command: ");
    Serial.println(valueStr);
    processCommand(valueStr);
  }
}

void ArduRoombaBLE::initPlatformBLE() {
  // Set up service and characteristics
  BLEService* service = new BLEService(BLE_SERVICE_UUID);
  _bleService = service;

  BLECharacteristic* commandChar = new BLECharacteristic(
    BLE_COMMAND_CHAR_UUID,
    BLEWrite,
    20  // Max 20 bytes for BLE
  );
  _commandChar = commandChar;

  BLECharacteristic* statusChar = new BLECharacteristic(
    BLE_STATUS_CHAR_UUID,
    BLERead | BLENotify,
    20
  );
  _statusChar = statusChar;

  // Add characteristics to service
  service->addCharacteristic(*commandChar);
  service->addCharacteristic(*statusChar);

  // Set callbacks
  BLE.setEventHandler(BLEConnected, bleConnected);
  BLE.setEventHandler(BLEDisconnected, bleDisconnected);
  commandChar->setEventHandler(BLEWritten, bleWritten);

  // Add service
  BLE.addService(*service);
}

void ArduRoombaBLE::advertisePlatformBLE() {
  BLE.setDeviceName(_deviceName.c_str());
  BLE.setLocalName(_deviceName.c_str());

  BLEAdvertising advertising = BLE.advertising();
  advertising.addServiceUUID(BLE_SERVICE_UUID);
  advertising.setAdvertisedService(*((BLEService*)_bleService));

  BLE.advertise();
}

bool ArduRoombaBLE::begin() {
  Serial.println("Initializing BLE (Uno R4 WiFi)...");

  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE");
    return false;
  }

  _instance = this;

  initPlatformBLE();

  // Set initial status
  String status = generateStatus();
  ((BLECharacteristic*)_statusChar)->setValue((const uint8_t*)status.c_str(), status.length());

  advertisePlatformBLE();

  Serial.println("BLE Service started");
  Serial.print("Device name: ");
  Serial.println(_deviceName);
  Serial.print("Device address: ");
  Serial.println(BLE.address());
  Serial.println("Waiting for connections...");

  return true;
}

void ArduRoombaBLE::end() {
  BLE.end();
  _deviceConnected = false;
}

void ArduRoombaBLE::updateStatus() {
  BLE.poll(); // Process BLE events

  // Periodically update status characteristic
  if (_deviceConnected && (millis() - _lastStatusUpdate > STATUS_UPDATE_INTERVAL)) {
    String status = generateStatus();
    ((BLECharacteristic*)_statusChar)->setValue((const uint8_t*)status.c_str(), status.length());
    ((BLECharacteristic*)_statusChar)->notify();
    _lastStatusUpdate = millis();
  }
}

#endif // ARDUINO_UNOWIFIR4

#endif // ESP32 || ARDUINO_UNOWIFIR4
