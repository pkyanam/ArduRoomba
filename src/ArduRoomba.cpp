/**
 * @file ArduRoomba.cpp
 * @brief Implementation of main ArduRoomba interface (Refactored)
 */

#include "ArduRoomba.h"

// Constructor with individual pins (backward compatible)
ArduRoomba::ArduRoomba(uint8_t rxPin, uint8_t txPin, uint8_t brcPin)
  : _serial(nullptr), _sensors(nullptr), _movement(nullptr),
    _actuators(nullptr), _legacyOI(nullptr), _connected(false), _debug(false) {
  _config = RoombaConfig::createDefault(rxPin, txPin, brcPin);
}

// Constructor with configuration
ArduRoomba::ArduRoomba(const RoombaConfig& config)
  : _config(config), _serial(nullptr), _sensors(nullptr), _movement(nullptr),
    _actuators(nullptr), _legacyOI(nullptr), _connected(false), _debug(false) {
}

// Destructor
ArduRoomba::~ArduRoomba() {
  end();

  delete _legacyOI;
  delete _actuators;
  delete _movement;
  delete _sensors;
  delete _serial;
}

// Initialize with default baud rate
bool ArduRoomba::begin() {
  return begin(_config.baudRate);
}

// Initialize with specific baud rate
bool ArduRoomba::begin(uint32_t baudRate) {
  if (_connected) {
    return true;
  }

  _config.baudRate = baudRate;

  if (_debug) {
    Serial.println("ArduRoomba: Starting initialization...");
  }

  // Initialize serial communication
  if (!initSerial()) {
    if (_debug) Serial.println("ArduRoomba: Serial initialization failed");
    return false;
  }

  // Wait for power stabilization
  delay(2000);

  // Pulse BRC pin to wake up Roomba
  pulseBRC();
  delay(100);

  // Send START command
  sendStartCommand();
  delay(100);

  // Enter safe mode
  _actuators->setSafeMode();
  delay(100);

  _connected = true;

  if (_debug) {
    Serial.println("ArduRoomba: Initialization complete");
  }

  return true;
}

// End connection
void ArduRoomba::end() {
  if (_connected) {
    _actuators->powerOff();
    _serial->end();
    _connected = false;
  }
}

// Set debug mode
void ArduRoomba::setDebug(bool enable) {
  _debug = enable;
  _sensors->setDebug(enable);
  _movement->setDebug(enable);
  _actuators->setDebug(enable);
}

// Update safety features (call in loop)
void ArduRoomba::updateSafety() {
  if (!_config.enableSafety || !_connected) {
    return;
  }

  // Check battery level
  uint16_t voltage = _sensors->getBatteryVoltage();

  if (voltage > 0 && voltage < _config.criticalBatteryThreshold) {
    // Critical battery - stop immediately
    if (_debug) {
      Serial.print("ArduRoomba: CRITICAL battery (");
      Serial.print(voltage);
      Serial.println(" mV), stopping!");
    }
    _movement->stop();
    _actuators->setLED(0, 0, 0, true); // Check robot LED on
    _actuators->playAlertSong();
  }
  else if (voltage > 0 && voltage < _config.lowBatteryThreshold) {
    // Low battery warning
    static unsigned long lastWarning = 0;
    if (millis() - lastWarning > 30000) { // Warn every 30 seconds
      if (_debug) {
        Serial.print("ArduRoomba: Low battery (");
        Serial.print(voltage);
        Serial.println(" mV)");
      }
      lastWarning = millis();
    }
  }

  // Check for bumper press (automatic stop)
  if (_sensors->isBumperPressed()) {
    static unsigned long lastBumperTime = 0;
    if (millis() - lastBumperTime > 500) { // Debounce
      if (_debug) {
        Serial.println("ArduRoomba: Bumper pressed, stopping");
      }
      _movement->stop();
      _actuators->beep();
      lastBumperTime = millis();
    }
  }

  // Check for cliff detection
  if (_sensors->isCliffDetected()) {
    if (_debug) {
      Serial.println("ArduRoomba: Cliff detected, stopping!");
    }
    _movement->stop();
    _actuators->playSadSong();
    // Back up from cliff
    _movement->moveBackward(100);
    delay(500);
    _movement->stop();
  }
}

// Get legacy OI interface
ArduRoomba::RoombaOI_Legacy& ArduRoomba::getOI() {
  if (!_legacyOI) {
    _legacyOI = new RoombaOI_Legacy(this);
  }
  return *_legacyOI;
}

// Private methods

bool ArduRoomba::initSerial() {
  if (_serial) {
    delete _serial;
  }

  // Platform-specific serial initialization
  #if defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI)
    // Uno R4 WiFi: Always use Serial1 (hardware serial)
    if (_debug) {
      Serial.println("ArduRoomba: Using Serial1 (Uno R4 WiFi)");
      Serial.println("ArduRoomba: Note - RX/TX pins must match Serial1 (default RX=0, TX=1)");
    }
    _serial = new HardwareSerialAdapter(&Serial1);
  #elif defined(ESP32) || defined(ESP8266)
    // ESP32/ESP8266: Use hardware serial if specified, default to Serial2 on ESP32
    if (_config.useHardwareSerial && _config.hwSerial) {
      _serial = new HardwareSerialAdapter(_config.hwSerial);
      if (_debug) {
        Serial.println("ArduRoomba: Using specified HardwareSerial");
      }
    } else {
      #ifdef ESP32
        _serial = new HardwareSerialAdapter(&Serial2);
        if (_debug) {
          Serial.println("ArduRoomba: Using Serial2 (default for ESP32)");
        }
      #else
        _serial = new HardwareSerialAdapter(&Serial);
        if (_debug) {
          Serial.println("ArduRoomba: Using Serial (ESP8266)");
        }
      #endif
    }
  #else
    // AVR and other boards: Use SoftwareSerial or HardwareSerial if specified
    if (_config.useHardwareSerial && _config.hwSerial) {
      _serial = new HardwareSerialAdapter(_config.hwSerial);
      if (_debug) {
        Serial.println("ArduRoomba: Using HardwareSerial");
      }
    } else {
      _serial = new SoftwareSerialAdapter(_config.rxPin, _config.txPin);
      if (_debug) {
        Serial.print("ArduRoomba: Using SoftwareSerial on pins ");
        Serial.print(_config.rxPin);
        Serial.print(", ");
        Serial.println(_config.txPin);
      }
    }
  #endif

  _serial->begin(_config.baudRate);

  // Create component instances
  _sensors = new RoombaSensors(_serial);
  _movement = new RoombaMovement(_serial);
  _actuators = new RoombaActuators(_serial);

  return true;
}

void ArduRoomba::pulseBRC() {
  if (_debug) {
    Serial.println("ArduRoomba: Pulsing BRC pin");
  }

  pinMode(_config.brcPin, OUTPUT);
  digitalWrite(_config.brcPin, HIGH);

  // Three pulses to wake Roomba
  for (int i = 0; i < 3; i++) {
    digitalWrite(_config.brcPin, LOW);
    delay(100);
    digitalWrite(_config.brcPin, HIGH);
    delay(100);
  }
}

void ArduRoomba::sendStartCommand() {
  if (_serial && _serial->isActive()) {
    _serial->write(OI_START);
    if (_debug) {
      Serial.println("ArduRoomba: START command sent");
    }
  }
}
