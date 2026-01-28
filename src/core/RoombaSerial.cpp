/**
 * @file RoombaSerial.cpp
 * @brief Implementation of Roomba serial communication adapters
 */

#include "RoombaSerial.h"

#if !defined(ESP32) && !defined(ESP8266)
  #include <SoftwareSerial.h>
#endif

// SoftwareSerialAdapter implementation
SoftwareSerialAdapter::SoftwareSerialAdapter(uint8_t rxPin, uint8_t txPin)
  : _serial(nullptr), _rxPin(rxPin), _txPin(txPin) {
}

SoftwareSerialAdapter::~SoftwareSerialAdapter() {
  end();
}

void SoftwareSerialAdapter::begin(uint32_t baudRate) {
  if (_serial) {
    delete _serial;
  }
  _serial = new ::SoftwareSerial(_rxPin, _txPin);
  _serial->begin(baudRate);
}

void SoftwareSerialAdapter::end() {
  if (_serial) {
    _serial->end();
    delete _serial;
    _serial = nullptr;
  }
}

void SoftwareSerialAdapter::write(uint8_t data) {
  if (_serial) {
    _serial->write(data);
  }
}

void SoftwareSerialAdapter::write(const uint8_t* data, size_t len) {
  if (_serial) {
    _serial->write(data, len);
  }
}

int SoftwareSerialAdapter::available() {
  return _serial ? _serial->available() : 0;
}

int SoftwareSerialAdapter::read() {
  return _serial ? _serial->read() : -1;
}

void SoftwareSerialAdapter::flush() {
  if (_serial) {
    _serial->flush();
  }
}

// HardwareSerialAdapter implementation
HardwareSerialAdapter::HardwareSerialAdapter(::HardwareSerial* serial)
  : _serial(serial) {
}

void HardwareSerialAdapter::begin(uint32_t baudRate) {
  if (_serial) {
    _serial->begin(baudRate);
  }
}

void HardwareSerialAdapter::end() {
  if (_serial) {
    _serial->end();
  }
}

void HardwareSerialAdapter::write(uint8_t data) {
  if (_serial) {
    _serial->write(data);
  }
}

void HardwareSerialAdapter::write(const uint8_t* data, size_t len) {
  if (_serial) {
    _serial->write(data, len);
  }
}

int HardwareSerialAdapter::available() {
  return _serial ? _serial->available() : 0;
}

int HardwareSerialAdapter::read() {
  return _serial ? _serial->read() : -1;
}

void HardwareSerialAdapter::flush() {
  if (_serial) {
    _serial->flush();
  }
}
