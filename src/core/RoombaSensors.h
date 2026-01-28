/**
 * @file RoombaSensors.h
 * @brief Roomba sensor data structures and reading interface
 *
 * Provides structured access to all Roomba OI sensor data
 * with organized data structures for different sensor groups.
 */

#ifndef ROOMBA_SENSORS_H
#define ROOMBA_SENSORS_H

#include <Arduino.h>
#include "RoombaSerial.h"

// Forward declaration
class RoombaSensors;

/**
 * Bumper and wheel drop sensor data
 */
struct BumperData {
  bool rightBumper;     // Right bumper pressed
  bool leftBumper;      // Left bumper pressed
  bool rightWheelDrop;  // Right wheel dropped
  bool leftWheelDrop;   // Left wheel dropped
  bool anyBumper() const { return leftBumper || rightBumper; }
  bool anyWheelDrop() const { return leftWheelDrop || rightWheelDrop; }
};

/**
 * Cliff sensor data
 */
struct CliffData {
  bool left;        // Left cliff sensor
  bool frontLeft;   // Front-left cliff sensor
  bool frontRight;  // Front-right cliff sensor
  bool right;       // Right cliff sensor
  bool anyCliff() const { return left || frontLeft || frontRight || right; }
};

/**
 * Wall sensor data
 */
struct WallData {
  bool wall;          // Wall sensor
  bool virtualWall;   // Virtual wall detected
};

/**
 * Button states
 */
struct ButtonData {
  bool schedule;      // Schedule button
  bool day;           // Day button
  bool hour;          // Hour button
  bool minute;        // Minute button
  bool dock;          // Dock button
  bool spot;          // Spot button
  bool clean;         // Clean button
  bool anyButton() const { return schedule || day || hour || minute || dock || spot || clean; }
};

/**
 * Movement data (distance and angle)
 */
struct MovementData {
  int16_t distance;   // Distance in mm (-32768 to 32767)
  int16_t angle;      // Angle in degrees (-32768 to 32767)
};

/**
 * Battery data
 */
struct BatteryData {
  uint16_t voltage;        // Voltage in mV
  int16_t current;         // Current in mA
  uint16_t charge;         // Current battery charge in mAh
  uint16_t capacity;       // Full battery capacity in mAh
  int8_t temperature;      // Temperature in Celsius
  uint8_t chargingState;   // 0=Not charging, 1=Reconditioning, 2=Full charging,
                           // 3=Trickle charging, 4=Waiting, 5=Charging fault

  uint8_t getPercent() const {
    if (capacity == 0) return 0;
    return (charge * 100) / capacity;
  }

  bool isLow() const { return voltage > 0 && voltage < 13000; }  // Below 13V
  bool isCritical() const { return voltage > 0 && voltage < 12000; }  // Below 12V
};

/**
 * All-in-one sensor data structure
 */
struct RoombaSensorData {
  BumperData bumper;
  CliffData cliff;
  WallData wall;
  ButtonData buttons;
  MovementData movement;
  BatteryData battery;
  uint32_t lastUpdate;  // millis() when last updated
};

/**
 * Roomba sensor interface
 * Provides structured access to all Roomba sensors
 */
class RoombaSensors {
public:
  explicit RoombaSensors(RoombaSerial* serial);
  ~RoombaSensors() = default;

  // Individual sensor groups
  BumperData readBumpers();
  CliffData readCliffs();
  WallData readWalls();
  ButtonData readButtons();
  MovementData readMovement();
  BatteryData readBattery();

  // Individual convenience methods
  bool isBumperPressed();
  bool isWallDetected();
  bool isCliffDetected();
  uint16_t getBatteryVoltage();
  int16_t getBatteryCurrent();
  uint8_t getBatteryPercent();
  bool isBatteryLow();
  bool isBatteryCritical();

  // Read all sensors at once into a structure
  RoombaSensorData readAll();

  // Raw sensor access for advanced use
  bool getSensor(uint8_t sensorId, uint8_t* data, uint8_t dataSize);

  // Debug
  void setDebug(bool enable) { _debug = enable; }
  void printSensorData(const RoombaSensorData& data);

private:
  RoombaSerial* _serial;
  bool _debug;

  // Helper methods
  uint8_t readByte(uint16_t timeout = 100);
  bool readBytes(uint8_t* buffer, uint8_t numBytes, uint16_t timeout = 100);
  void debugPrint(const char* msg);
  void debugPrint(const char* msg, int value);
};

#endif
