/**
 * BasicMovement.ino
 *
 * Simple example showing basic movement commands with ArduRoomba v4.
 * Demonstrates forward/backward movement, turning, and stopping.
 *
 * Hardware:
 * - Arduino Uno/Mega/Nano or compatible
 * - iRobot Create 2 or compatible Roomba (500/600/700 series)
 *
 * Connections:
 * - Roomba TX (Mini-DIN pin 4) -> Arduino Pin 2 (RX)
 * - Roomba RX (Mini-DIN pin 3) -> Arduino Pin 3 (TX)
 * - Roomba DD (Mini-DIN pin 5) -> Arduino Pin 4 (BRC)
 * - GND (Mini-DIN pin 6/7) -> Arduino GND
 *
 * For ESP32, use RoombaConfig::createESP32() for hardware serial.
 */

#include "ArduRoomba.h"

// Create Roomba instance with pin configuration
// RX=2, TX=3, BRC=4 is the standard wiring for Arduino boards
ArduRoomba roomba(2, 3, 4);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait for Serial (timeout 3s)

  Serial.println("\n=== ArduRoomba Basic Movement Example ===");

  // Enable debug output to see what's happening
  roomba.setDebug(true);

  // Initialize connection to Roomba
  Serial.println("Connecting to Roomba...");
  if (roomba.begin()) {
    Serial.println("✓ Roomba connected successfully!");

    // Run movement demonstrations
    movementDemo();
    sequenceDemo();

    Serial.println("\n=== Demo Complete ===");

  } else {
    Serial.println("✗ Failed to connect to Roomba!");
    Serial.println("Check wiring and power.");
  }
}

void loop() {
  // Nothing to do in main loop for this example
}

/**
 * Basic movement demonstration
 * Shows individual movement commands
 */
void movementDemo() {
  Serial.println("\n--- Basic Movement Demo ---");

  // Move forward at 200 mm/s
  Serial.println("Moving forward...");
  roomba.moveForward(200);
  delay(2000);
  roomba.stop();

  delay(500);

  // Move backward
  Serial.println("Moving backward...");
  roomba.moveBackward(150);
  delay(1500);
  roomba.stop();

  delay(500);

  // Turn right (in-place rotation)
  Serial.println("Spinning right...");
  roomba.spinRight(150);
  delay(1000);
  roomba.stop();

  delay(500);

  // Turn left (in-place rotation)
  Serial.println("Spinning left...");
  roomba.spinLeft(150);
  delay(1000);
  roomba.stop();

  delay(500);

  // Gentle turn (not in-place)
  Serial.println("Gentle left turn...");
  roomba.turnLeft(100);
  delay(1500);
  roomba.stop();

  // Beep when done
  roomba.beep();
}

/**
 * Sequence demonstration
 * Shows the fluent API for chaining movements
 */
void sequenceDemo() {
  Serial.println("\n--- Sequence Demo ---");

  // Create a movement sequence
  // This builds a chain of movements that execute in order
  RoombaSequence seq(roomba.movement());

  // Chain movements together
  seq.forward(200, 2000)    // Forward 200mm/s for 2 seconds
     .backward(150, 1500)   // Backward 150mm/s for 1.5 seconds
     .spinLeft(200, 1000)   // Spin left 200mm/s for 1 second
     .spinRight(200, 1000)  // Spin right 200mm/s for 1 second
     .stop()                // Stop
     .execute();            // Run the sequence

  roomba.beep();

  // Or use the global helper for a more compact syntax
  RoombaSequenceBuilder(roomba)
    .forward(150, 1000)
    .right(150, 500)
    .forward(150, 1000)
    .stop()
    .execute();

  roomba.beep();
}
