/**
 * SensorReading.ino
 *
 * Demonstrates reading all types of sensor data from the Roomba.
 * Shows structured sensor data access with the new v4 API.
 *
 * This example demonstrates:
 * - Individual sensor group reading (bumpers, cliffs, walls, buttons)
 * - Battery monitoring
 * - Movement data (distance, angle)
 * - Complete sensor snapshot
 * - LED feedback based on sensor states
 */

#include "ArduRoomba.h"

// Create Roomba instance
ArduRoomba roomba(2, 3, 4);

// Timing for periodic sensor reads
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_READ_INTERVAL = 1000; // 1 second

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  Serial.println("\n=== ArduRoomba Sensor Reading Example ===");

  roomba.setDebug(false);

  if (roomba.begin()) {
    Serial.println("✓ Roomba connected!");

    // Beep to indicate ready
    roomba.beep();

    // Print initial sensor report
    printFullSensorReport();

  } else {
    Serial.println("✗ Failed to connect to Roomba!");
  }
}

void loop() {
  // Periodic sensor reading
  if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = millis();

    // Quick sensor summary
    printSensorSummary();

    // LED feedback based on sensors
    updateLEDsFromSensors();
  }
}

/**
 * Print a comprehensive sensor report once
 */
void printFullSensorReport() {
  Serial.println("\n--- Full Sensor Report ---");

  // Read all sensors at once
  RoombaSensorData data = roomba.sensors().readAll();

  // Print bumper data
  Serial.print("Bumpers: Left=");
  Serial.print(data.bumper.leftBumper);
  Serial.print(" Right=");
  Serial.print(data.bumper.rightBumper);
  Serial.print(" [");
  Serial.print(data.bumper.anyBumper() ? "PRESSED" : "Clear");
  Serial.println("]");

  // Print cliff data
  Serial.print("Cliffs: L=");
  Serial.print(data.cliff.left);
  Serial.print(" FL=");
  Serial.print(data.cliff.frontLeft);
  Serial.print(" FR=");
  Serial.print(data.cliff.frontRight);
  Serial.print(" R=");
  Serial.print(data.cliff.right);
  Serial.print(" [");
  Serial.print(data.cliff.anyCliff() ? "DETECTED" : "Clear");
  Serial.println("]");

  // Print wall data
  Serial.print("Wall Sensor: ");
  Serial.println(data.wall.wall ? "DETECTED" : "Clear");
  Serial.print("Virtual Wall: ");
  Serial.println(data.wall.virtualWall ? "DETECTED" : "Clear");

  // Print button states
  Serial.print("Buttons: ");
  if (data.buttons.clean) Serial.print("CLEAN ");
  if (data.buttons.spot) Serial.print("SPOT ");
  if (data.buttons.dock) Serial.print("DOCK ");
  if (data.buttons.day) Serial.print("DAY ");
  if (data.buttons.hour) Serial.print("HOUR ");
  if (data.buttons.minute) Serial.print("MINUTE ");
  Serial.println();

  // Print movement data
  Serial.print("Movement: Distance=");
  Serial.print(data.movement.distance);
  Serial.print("mm Angle=");
  Serial.print(data.movement.angle);
  Serial.println("deg");

  // Print battery data
  Serial.println("\n--- Battery Status ---");
  Serial.print("Voltage: ");
  Serial.print(data.battery.voltage);
  Serial.println(" mV");

  Serial.print("Current: ");
  Serial.print(data.battery.current);
  Serial.println(" mA");

  Serial.print("Charge: ");
  Serial.print(data.battery.charge);
  Serial.print(" / ");
  Serial.print(data.battery.capacity);
  Serial.print(" mAh (");
  Serial.print(data.battery.getPercent());
  Serial.println("%)");

  Serial.print("Temperature: ");
  Serial.print(data.battery.temperature);
  Serial.println(" C");

  Serial.print("Charging State: ");
  switch (data.battery.chargingState) {
    case 0: Serial.println("Not charging"); break;
    case 1: Serial.println("Reconditioning"); break;
    case 2: Serial.println("Full charging"); break;
    case 3: Serial.println("Trickle charging"); break;
    case 4: Serial.println("Waiting"); break;
    case 5: Serial.println("Charging fault"); break;
    default: Serial.println("Unknown"); break;
  }

  if (data.battery.isLow()) {
    Serial.println("⚠️  BATTERY LOW!");
  }
  if (data.battery.isCritical()) {
    Serial.println("⚠️  BATTERY CRITICAL!");
  }

  Serial.println("------------------------\n");
}

/**
 * Print a one-line sensor summary
 */
void printSensorSummary() {
  // Quick access methods
  uint16_t voltage = roomba.getBatteryVoltage();
  uint8_t percent = roomba.getBatteryPercent();
  bool bumper = roomba.isBumperPressed();
  bool wall = roomba.isWallDetected();
  bool cliff = roomba.isCliffDetected();

  Serial.print("[");
  Serial.print(millis() / 1000);
  Serial.print("s] Bat: ");
  Serial.print(voltage);
  Serial.print("mV (");
  Serial.print(percent);
  Serial.print("%) | ");

  if (bumper) Serial.print("🚧 BUMPER ");
  if (wall) Serial.print("🧱 WALL ");
  if (cliff) Serial.print("⬇️ CLIFF ");

  if (!bumper && !wall && !cliff) Serial.print("Clear");

  Serial.println();
}

/**
 * Update LEDs based on sensor states
 * Provides visual feedback of what the Roomba is detecting
 */
void updateLEDsFromSensors() {
  bool debris = false;   // Blue LED
  bool spot = false;     // Green LED
  bool dock = false;     // Green LED
  bool checkRobot = false; // Red LED

  uint16_t voltage = roomba.getBatteryVoltage();

  // Low battery warning - red LED
  if (voltage > 0 && voltage < 13000) {
    checkRobot = true;
  }

  // Bumper pressed - debris LED
  if (roomba.isBumperPressed()) {
    debris = true;
  }

  // Wall detected - spot LED
  if (roomba.isWallDetected()) {
    spot = true;
  }

  // Cliff detected - dock LED
  if (roomba.isCliffDetected()) {
    dock = true;
  }

  // Set power LED color based on battery
  uint8_t powerColor = 0; // Green
  uint8_t powerIntensity = 100;

  if (voltage > 0 && voltage < 14500) {
    powerColor = 64; // Yellow-green
  }
  if (voltage > 0 && voltage < 13000) {
    powerColor = 128; // Yellow
  }
  if (voltage > 0 && voltage < 12000) {
    powerColor = 255; // Red
    powerIntensity = 255; // Full brightness
  }

  roomba.setLED(debris, spot, dock, checkRobot);
  roomba.setPowerLED(powerColor, powerIntensity);
}
