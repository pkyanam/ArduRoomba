/**
 * @file RoombaSerial.h
 * @brief Abstract interface for Roomba serial communication
 *
 * Provides abstraction layer for different serial implementations:
 * - SoftwareSerial for Arduino Uno/Mega
 * - HardwareSerial for ESP32/ESP8266
 * - Allows runtime selection of serial type
 */

#ifndef ROOMBA_SERIAL_H
#define ROOMBA_SERIAL_H

#include <Arduino.h>

// Include SoftwareSerial on platforms that support it
#if !defined(ESP32) && !defined(ESP8266) && !defined(ARDUINO_UNOWIFIR4) && !defined(ARDUINO_UNOR4_WIFI)
  #include <SoftwareSerial.h>
#endif

// OI Command opcodes - moved here for shared access
#define OI_START        128
#define OI_BAUD         129
#define OI_SAFE         131
#define OI_FULL         132
#define OI_POWER        133
#define OI_SPOT         134
#define OI_CLEAN        135
#define OI_MAX_CLEAN    136
#define OI_DRIVE        137
#define OI_MOTORS       138
#define OI_LEDS         139
#define OI_SONG         140
#define OI_PLAY         141
#define OI_SENSORS      142
#define OI_SEEK_DOCK    143
#define OI_DRIVE_DIRECT 145
#define OI_STREAM       148

// Common sensor packet IDs
#define SENSOR_BUMPS_DROPS      7
#define SENSOR_WALL             8
#define SENSOR_CLIFF_LEFT       9
#define SENSOR_CLIFF_FRONT_LEFT 10
#define SENSOR_CLIFF_FRONT_RIGHT 11
#define SENSOR_CLIFF_RIGHT      12
#define SENSOR_VIRTUAL_WALL     13
#define SENSOR_BUTTONS          18
#define SENSOR_DISTANCE         19
#define SENSOR_ANGLE            20
#define SENSOR_CHARGING_STATE   21
#define SENSOR_VOLTAGE          22
#define SENSOR_CURRENT          23
#define SENSOR_TEMPERATURE      24
#define SENSOR_BATTERY_CHARGE   25
#define SENSOR_BATTERY_CAPACITY 26

// Drive constants
#define DRIVE_STRAIGHT     32768
#define DRIVE_TURN_CCW     1
#define DRIVE_TURN_CW      -1
#define MAX_VELOCITY       500
#define MIN_VELOCITY       -500

/**
 * Abstract interface for Roomba serial communication
 */
class RoombaSerial {
public:
  virtual ~RoombaSerial() = default;

  // Initialization
  virtual void begin(uint32_t baudRate) = 0;
  virtual void end() = 0;

  // Write operations
  virtual void write(uint8_t data) = 0;
  virtual void write(const uint8_t* data, size_t len) = 0;

  // Read operations
  virtual int available() = 0;
  virtual int read() = 0;

  // Flush
  virtual void flush() = 0;

  // Status
  virtual bool isActive() const = 0;
};

/**
 * SoftwareSerial adapter for boards without extra hardware serial
 * Note: Not available on ESP32, ESP8266, or Uno R4 WiFi
 */
#if !defined(ESP32) && !defined(ESP8266) && !defined(ARDUINO_UNOWIFIR4) && !defined(ARDUINO_UNOR4_WIFI)
class SoftwareSerialAdapter : public RoombaSerial {
public:
  SoftwareSerialAdapter(uint8_t rxPin, uint8_t txPin);
  ~SoftwareSerialAdapter() override;

  void begin(uint32_t baudRate) override;
  void end() override;
  void write(uint8_t data) override;
  void write(const uint8_t* data, size_t len) override;
  int available() override;
  int read() override;
  void flush() override;
  bool isActive() const override;

private:
  ::SoftwareSerial* _serial;
  uint8_t _rxPin, _txPin;
};
#endif

/**
 * HardwareSerial adapter for boards with multiple serial ports (ESP32, ESP8266, Mega)
 */
class HardwareSerialAdapter : public RoombaSerial {
public:
  explicit HardwareSerialAdapter(::HardwareSerial* serial);
  ~HardwareSerialAdapter() override = default;

  void begin(uint32_t baudRate) override;
  void end() override;
  void write(uint8_t data) override;
  void write(const uint8_t* data, size_t len) override;
  int available() override;
  int read() override;
  void flush() override;
  bool isActive() const override { return _serial != nullptr; }

private:
  ::HardwareSerial* _serial;
};

#endif
