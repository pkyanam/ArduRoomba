/**
 * @file BasicUsage.ino
 * @brief Basic usage example for the ArduRoomba library
 * 
 * This example demonstrates basic movement commands using the ArduRoomba library.
 * It shows both the legacy interface (for backward compatibility) and the new
 * enhanced interface with error handling.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * Hardware connections:
 * - Arduino Pin 2 -> Roomba TX (receive data from Roomba)
 * - Arduino Pin 3 -> Roomba RX (send data to Roomba)
 * - Arduino Pin 4 -> Roomba BRC (Baud Rate Change pin)
 * - Arduino GND -> Roomba GND
 * - Arduino 5V -> Roomba VCC (through voltage divider if needed)
 */

#include "ArduRoomba.h"

// Create ArduRoomba instance
ArduRoomba roomba(2, 3, 4); // rxPin, txPin, brcPin

void setup() {
    Serial.begin(19200);
    Serial.println("ArduRoomba Basic Usage Example");
    Serial.println("==============================");
    
    // Initialize the Roomba with error handling (new interface)
    ArduRoomba::ErrorCode result = roomba.initialize();
    if (result == ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("Roomba initialized successfully!");
        
        // Enable debug output to see what's happening
        roomba.setDebugEnabled(true);
        
        // Demonstrate basic movements with error handling
        demonstrateMovements();
        
    } else {
        Serial.print("Failed to initialize Roomba. Error code: ");
        Serial.println(static_cast<uint8_t>(result));
        
        // Fall back to legacy initialization for compatibility
        Serial.println("Trying legacy initialization...");
        roomba.roombaSetup();
        
        // Use legacy movement commands
        demonstrateLegacyMovements();
    }
}

void loop() {
    // Main loop - could add sensor monitoring or remote control here
    delay(1000);
}

/**
 * @brief Demonstrate movements using the new enhanced interface
 */
void demonstrateMovements() {
    Serial.println("\nDemonstrating enhanced movement interface:");
    
    // Move forward with custom velocity and error checking
    Serial.println("Moving forward...");
    ArduRoomba::ErrorCode result = roomba.moveForward(300); // 300 mm/s
    if (result != ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("Failed to move forward!");
    }
    delay(1000);
    
    // Move backward
    Serial.println("Moving backward...");
    result = roomba.moveBackward(300);
    if (result != ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("Failed to move backward!");
    }
    delay(1000);
    
    // Turn left in place
    Serial.println("Turning left...");
    result = roomba.turnLeftInPlace(200);
    if (result != ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("Failed to turn left!");
    }
    delay(1000);
    
    // Turn right in place
    Serial.println("Turning right...");
    result = roomba.turnRightInPlace(200);
    if (result != ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("Failed to turn right!");
    }
    delay(1000);
    
    // Stop the robot
    Serial.println("Stopping...");
    result = roomba.stopMovement();
    if (result != ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("Failed to stop!");
    }
    
    // Play a success beep
    Serial.println("Playing success beep...");
    roomba.beep(72, 32); // Middle C for 0.5 seconds
    
    Serial.println("Movement demonstration complete!");
}

/**
 * @brief Demonstrate movements using the legacy interface (for backward compatibility)
 */
void demonstrateLegacyMovements() {
    Serial.println("\nDemonstrating legacy movement interface:");
    
    Serial.println("Moving forward...");
    roomba.goForward();
    delay(1000);
    
    Serial.println("Moving backward...");
    roomba.goBackward();
    delay(1000);
    
    Serial.println("Turning left...");
    roomba.turnLeft();
    delay(1000);
    
    Serial.println("Turning right...");
    roomba.turnRight();
    delay(1000);
    
    Serial.println("Stopping...");
    roomba.halt();
    
    Serial.println("Legacy movement demonstration complete!");
}
