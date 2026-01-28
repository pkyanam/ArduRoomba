/**
 * AdvancedMovement.ino
 *
 * Advanced movement patterns demonstration.
 * Shows complex movement sequences, geometric patterns, and choreographed movements.
 *
 * This example demonstrates:
 * - Movement sequences for complex patterns
 * - Square, circle, and figure-eight patterns
 * - Sensor-based reactive movements
 * - LED and sound feedback during movements
 */

#include "ArduRoomba.h"

// Create Roomba instance
ArduRoomba roomba(2, 3, 4);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  Serial.println("\n=== ArduRoomba Advanced Movement ===");

  roomba.setDebug(false); // Disable debug for cleaner output

  if (roomba.begin()) {
    Serial.println("✓ Roomba connected!");

    // Play startup sound
    roomba.actuators().playStartupSong();
    delay(1000);

    // Run pattern demonstrations
    squarePattern();
    delay(1000);

    circlePattern();
    delay(1000);

    figureEightPattern();
    delay(1000);

    dancePattern();
    delay(1000);

    sensorReactiveDemo();

    Serial.println("\n=== All Patterns Complete ===");
    roomba.beep();

  } else {
    Serial.println("✗ Failed to connect to Roomba!");
  }
}

void loop() {
  // Nothing here
}

/**
 * Drive in a square pattern
 */
void squarePattern() {
  Serial.println("\n--- Square Pattern ---");

  // Set power LED to green
  roomba.setPowerLED(0, 200); // Green, medium brightness

  RoombaSequenceBuilder(roomba)
    .forward(200, 2000)  // Side 1
    .spinLeft(150, 850)   // Turn 90 degrees
    .forward(200, 2000)  // Side 2
    .spinLeft(150, 850)   // Turn 90 degrees
    .forward(200, 2000)  // Side 3
    .spinLeft(150, 850)   // Turn 90 degrees
    .forward(200, 2000)  // Side 4
    .spinLeft(150, 850)   // Turn 90 degrees
    .stop()
    .execute();
}

/**
 * Drive in a circle pattern
 * Uses gentle turn while moving forward
 */
void circlePattern() {
  Serial.println("\n--- Circle Pattern ---");

  // Set power LED to blue
  roomba.setPowerLED(200, 200);

  // For a circle, we turn gently while moving
  // This creates an arc pattern
  for (int i = 0; i < 4; i++) {
    roomba.turnLeft(200);
    delay(3000);
  }
  roomba.stop();
}

/**
 * Drive in a figure-eight pattern
 */
void figureEightPattern() {
  Serial.println("\n--- Figure Eight Pattern ---");

  // Set power LED to orange
  roomba.setPowerLED(255, 200);

  // First circle (left turns)
  for (int i = 0; i < 8; i++) {
    roomba.drive(150, 200);  // Velocity 150, radius 200 (gentle left arc)
    delay(500);
  }

  // Transition
  roomba.stop();
  delay(500);

  // Second circle (right turns)
  for (int i = 0; i < 8; i++) {
    roomba.drive(150, -200); // Velocity 150, radius -200 (gentle right arc)
    delay(500);
  }

  roomba.stop();
}

/**
 * Fun "dance" pattern with lights and sounds
 */
void dancePattern() {
  Serial.println("\n--- Dance Pattern ---");

  for (int i = 0; i < 4; i++) {
    // Forward with debris LED
    roomba.setLED(true, false, false, false);
    roomba.moveForward(250);
    delay(300);

    // Backward with spot LED
    roomba.setLED(false, true, false, false);
    roomba.moveBackward(250);
    delay(300);

    // Spin left with dock LED
    roomba.setLED(false, false, true, false);
    roomba.spinLeft(200);
    delay(400);

    // Spin right with all LEDs
    roomba.setLED(true, true, true, true);
    roomba.spinRight(200);
    delay(400);
  }

  roomba.stop();
  roomba.setLED(0, 0, 0, 0); // All LEDs off
}

/**
 * Sensor-reactive movement
 * Moves forward until detecting obstacles, then reacts
 */
void sensorReactiveDemo() {
  Serial.println("\n--- Sensor Reactive Demo ---");

  roomba.setPowerLED(128, 255); // Yellow, full brightness

  unsigned long startTime = millis();
  int loopCount = 0;

  // Run for 30 seconds or until 5 obstacle encounters
  while (millis() - startTime < 30000 && loopCount < 5) {
    roomba.moveForward(150);

    // Poll sensors while moving
    for (int i = 0; i < 20 && millis() - startTime < 30000; i++) {
      delay(100);

      // Check for obstacles
      if (roomba.isBumperPressed()) {
        Serial.println("Bumper detected!");

        // Reaction: back up and turn
        roomba.moveBackward(150);
        delay(500);
        roomba.stop();
        delay(100);

        roomba.spinLeft(200);
        delay(500);
        roomba.stop();
        delay(100);

        roomba.beep();
        loopCount++;
        break;
      }

      // Check for cliff
      if (roomba.isCliffDetected()) {
        Serial.println("Cliff detected!");

        roomba.moveBackward(200);
        delay(1000);
        roomba.stop();

        roomba.actuators().playSadSong();
        loopCount = 5; // End the demo
        break;
      }

      // Check wall sensor
      if (roomba.isWallDetected()) {
        Serial.println("Wall detected, adjusting course...");
        roomba.turnRight(100);
        delay(300);
        roomba.stop();
        break;
      }
    }
  }

  roomba.stop();
  Serial.print("Obstacle encounters: ");
  Serial.println(loopCount);
}
