/**
 * @file RoombaSensors.cpp
 * @brief Implementation of Roomba sensor reading functionality
 */

#include "RoombaSensors.h"

RoombaSensors::RoombaSensors(RoombaSerial* serial)
  : _serial(serial), _debug(false) {
}

BumperData RoombaSensors::readBumpers() {
  BumperData data = {false, false, false, false};

  uint8_t value;
  if (getSensor(SENSOR_BUMPS_DROPS, &value, 1)) {
    data.rightBumper = (value & 0x01) != 0;
    data.leftBumper = (value & 0x02) != 0;
    data.rightWheelDrop = (value & 0x04) != 0;
    data.leftWheelDrop = (value & 0x08) != 0;
  }

  return data;
}

CliffData RoombaSensors::readCliffs() {
  CliffData data = {false, false, false, false};

  uint8_t cliffs[4];
  if (getSensor(SENSOR_CLIFF_LEFT, &cliffs[0], 1)) data.left = cliffs[0] != 0;
  if (getSensor(SENSOR_CLIFF_FRONT_LEFT, &cliffs[1], 1)) data.frontLeft = cliffs[1] != 0;
  if (getSensor(SENSOR_CLIFF_FRONT_RIGHT, &cliffs[2], 1)) data.frontRight = cliffs[2] != 0;
  if (getSensor(SENSOR_CLIFF_RIGHT, &cliffs[3], 1)) data.right = cliffs[3] != 0;

  return data;
}

WallData RoombaSensors::readWalls() {
  WallData data = {false, false};

  uint8_t value;
  if (getSensor(SENSOR_WALL, &value, 1)) data.wall = value != 0;
  if (getSensor(SENSOR_VIRTUAL_WALL, &value, 1)) data.virtualWall = value != 0;

  return data;
}

ButtonData RoombaSensors::readButtons() {
  ButtonData data = {false, false, false, false, false, false, false};

  uint8_t value;
  if (getSensor(SENSOR_BUTTONS, &value, 1)) {
    data.schedule = (value & 0x80) != 0;
    data.day = (value & 0x40) != 0;
    data.hour = (value & 0x20) != 0;
    data.minute = (value & 0x10) != 0;
    data.dock = (value & 0x08) != 0;
    data.spot = (value & 0x04) != 0;
    data.clean = (value & 0x02) != 0;
  }

  return data;
}

MovementData RoombaSensors::readMovement() {
  MovementData data = {0, 0};

  uint8_t bytes[2];

  // Read distance (2 bytes, signed big-endian)
  if (getSensor(SENSOR_DISTANCE, bytes, 2)) {
    data.distance = (int16_t)((bytes[0] << 8) | bytes[1]);
  }

  // Read angle (2 bytes, signed big-endian)
  if (getSensor(SENSOR_ANGLE, bytes, 2)) {
    data.angle = (int16_t)((bytes[0] << 8) | bytes[1]);
  }

  return data;
}

BatteryData RoombaSensors::readBattery() {
  BatteryData data = {0, 0, 0, 0, 0, 0};

  uint8_t bytes[2];

  // Voltage (2 bytes, unsigned big-endian)
  if (getSensor(SENSOR_VOLTAGE, bytes, 2)) {
    data.voltage = (uint16_t)((bytes[0] << 8) | bytes[1]);
  }

  // Current (2 bytes, signed big-endian)
  if (getSensor(SENSOR_CURRENT, bytes, 2)) {
    data.current = (int16_t)((bytes[0] << 8) | bytes[1]);
  }

  // Temperature (1 byte, signed)
  uint8_t temp;
  if (getSensor(SENSOR_TEMPERATURE, &temp, 1)) {
    data.temperature = (int8_t)temp;
  }

  // Charge (2 bytes, unsigned big-endian)
  if (getSensor(SENSOR_BATTERY_CHARGE, bytes, 2)) {
    data.charge = (uint16_t)((bytes[0] << 8) | bytes[1]);
  }

  // Capacity (2 bytes, unsigned big-endian)
  if (getSensor(SENSOR_BATTERY_CAPACITY, bytes, 2)) {
    data.capacity = (uint16_t)((bytes[0] << 8) | bytes[1]);
  }

  // Charging state (1 byte)
  uint8_t state;
  if (getSensor(SENSOR_CHARGING_STATE, &state, 1)) {
    data.chargingState = state;
  }

  return data;
}

bool RoombaSensors::isBumperPressed() {
  BumperData b = readBumpers();
  return b.anyBumper();
}

bool RoombaSensors::isWallDetected() {
  WallData w = readWalls();
  return w.wall;
}

bool RoombaSensors::isCliffDetected() {
  CliffData c = readCliffs();
  return c.anyCliff();
}

uint16_t RoombaSensors::getBatteryVoltage() {
  BatteryData b = readBattery();
  return b.voltage;
}

int16_t RoombaSensors::getBatteryCurrent() {
  BatteryData b = readBattery();
  return b.current;
}

uint8_t RoombaSensors::getBatteryPercent() {
  BatteryData b = readBattery();
  return b.getPercent();
}

bool RoombaSensors::isBatteryLow() {
  BatteryData b = readBattery();
  return b.isLow();
}

bool RoombaSensors::isBatteryCritical() {
  BatteryData b = readBattery();
  return b.isCritical();
}

RoombaSensorData RoombaSensors::readAll() {
  RoombaSensorData data;

  data.bumper = readBumpers();
  data.cliff = readCliffs();
  data.wall = readWalls();
  data.buttons = readButtons();
  data.movement = readMovement();
  data.battery = readBattery();
  data.lastUpdate = millis();

  return data;
}

bool RoombaSensors::getSensor(uint8_t sensorId, uint8_t* data, uint8_t dataSize) {
  if (!_serial || !data || !_serial->isActive()) return false;

  _serial->write(OI_SENSORS);
  _serial->write(sensorId);
  _serial->flush();
  delay(15); // Wait for response

  return readBytes(data, dataSize, 100);
}

uint8_t RoombaSensors::readByte(uint16_t timeout) {
  unsigned long start = millis();
  while (_serial->available() == 0 && (millis() - start) < timeout) {
    // Wait
  }
  return _serial->available() > 0 ? (uint8_t)_serial->read() : 0;
}

bool RoombaSensors::readBytes(uint8_t* buffer, uint8_t numBytes, uint16_t timeout) {
  if (!buffer || !_serial) return false;

  unsigned long start = millis();
  uint8_t bytesRead = 0;

  while (bytesRead < numBytes && (millis() - start) < timeout) {
    if (_serial->available() > 0) {
      buffer[bytesRead++] = (uint8_t)_serial->read();
    }
  }

  return bytesRead == numBytes;
}

void RoombaSensors::debugPrint(const char* msg) {
  if (_debug && msg) {
    Serial.print("RoombaSensors: ");
    Serial.println(msg);
  }
}

void RoombaSensors::debugPrint(const char* msg, int value) {
  if (_debug && msg) {
    Serial.print("RoombaSensors: ");
    Serial.print(msg);
    Serial.print(" = ");
    Serial.println(value);
  }
}

void RoombaSensors::printSensorData(const RoombaSensorData& data) {
  Serial.println("=== Roomba Sensor Data ===");

  Serial.print("Bumpers: L=");
  Serial.print(data.bumper.leftBumper);
  Serial.print(" R=");
  Serial.println(data.bumper.rightBumper);

  Serial.print("Cliffs: L=");
  Serial.print(data.cliff.left);
  Serial.print(" FL=");
  Serial.print(data.cliff.frontLeft);
  Serial.print(" FR=");
  Serial.print(data.cliff.frontRight);
  Serial.print(" R=");
  Serial.println(data.cliff.right);

  Serial.print("Wall: ");
  Serial.println(data.wall.wall ? "Yes" : "No");

  Serial.print("Battery: ");
  Serial.print(data.battery.voltage);
  Serial.print("mV (");
  Serial.print(data.battery.getPercent());
  Serial.println("%)");

  Serial.print("Charging State: ");
  Serial.println(data.battery.chargingState);

  Serial.println("========================");
}
