/**
 * @file StreamExample.ino
 * @brief Sensor streaming example for the ArduRoomba library
 * 
 * This example demonstrates how to use sensor streaming to continuously
 * monitor Roomba sensor data. It shows both the new modular interface
 * with sensor presets and the legacy interface for backward compatibility.
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

// Sensor data structures
ArduRoomba::SensorData currentData = {};
ArduRoomba::SensorData previousData = {};

// Timing variables
unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 1000; // Print summary every 1 second

void setup() {
    Serial.begin(19200);
    Serial.println("ArduRoomba Sensor Streaming Example");
    Serial.println("===================================");
    
    // Initialize the Roomba
    ArduRoomba::ErrorCode result = roomba.initialize();
    if (result != ArduRoomba::ErrorCode::SUCCESS) {
        Serial.print("Failed to initialize Roomba. Error code: ");
        Serial.println(static_cast<uint8_t>(result));
        return;
    }
    
    Serial.println("Roomba initialized successfully!");
    
    // Enable debug output
    roomba.setDebugEnabled(true);
    
    // Put Roomba in safe mode for sensor access
    roomba.safe();
    delay(100);
    
    // Start streaming with a predefined sensor preset
    Serial.println("Starting sensor stream with BASIC preset...");
    result = roomba.startSensorStream(ArduRoomba::RoombaSensors::SensorPreset::BASIC);
    
    if (result == ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("Sensor streaming started successfully!");
        Serial.println("Monitoring sensor changes...");
        Serial.println();
    } else {
        Serial.print("Failed to start sensor streaming. Error code: ");
        Serial.println(static_cast<uint8_t>(result));
        
        // Fall back to legacy streaming method
        Serial.println("Trying legacy streaming method...");
        setupLegacyStreaming();
    }
}

void loop() {
    // Update sensor data from stream
    ArduRoomba::ErrorCode result = roomba.updateSensorData(currentData);
    
    if (result == ArduRoomba::ErrorCode::SUCCESS) {
        // Print only changes to avoid flooding the serial output
        printSensorChanges();
        
        // Print periodic summary
        if (millis() - lastPrintTime > PRINT_INTERVAL) {
            printSensorSummary();
            lastPrintTime = millis();
        }
        
        // Copy current data to previous for next comparison
        previousData = currentData;
        
    } else if (result == ArduRoomba::ErrorCode::TIMEOUT) {
        // Timeout is normal - just means no new data this cycle
        
    } else {
        // Handle other errors
        if (currentData.failedAttempts > 5) {
            unsigned long timeSinceLastSuccess = millis() - currentData.lastSuccessfulRefresh;
            if (timeSinceLastSuccess > 1000) {
                Serial.print("No sensor data for ");
                Serial.print(timeSinceLastSuccess);
                Serial.println(" ms");
            }
        }
    }
    
    // Small delay to prevent overwhelming the system
    delay(10);
}

/**
 * @brief Print only sensor values that have changed
 */
void printSensorChanges() {
    // Check for mode changes
    if (currentData.mode != previousData.mode) {
        Serial.print("Mode changed: ");
        Serial.println(static_cast<uint8_t>(currentData.mode));
    }
    
    // Check for battery changes (only significant changes)
    if (abs(currentData.voltage - previousData.voltage) > 10) {
        Serial.print("Voltage: ");
        Serial.print(currentData.voltage);
        Serial.println(" mV");
    }
    
    if (abs(currentData.batteryCharge - previousData.batteryCharge) > 10) {
        Serial.print("Battery charge: ");
        Serial.print(currentData.batteryCharge);
        Serial.println(" mAh");
    }
    
    // Check for temperature changes
    if (currentData.temperature != previousData.temperature) {
        Serial.print("Temperature: ");
        Serial.print(currentData.temperature);
        Serial.println(" °C");
    }
    
    // Check for sensor state changes
    if (currentData.wall != previousData.wall) {
        Serial.print("Wall sensor: ");
        Serial.println(currentData.wall ? "DETECTED" : "clear");
    }
    
    if (currentData.bumpLeft != previousData.bumpLeft) {
        Serial.print("Left bumper: ");
        Serial.println(currentData.bumpLeft ? "PRESSED" : "released");
    }
    
    if (currentData.bumpRight != previousData.bumpRight) {
        Serial.print("Right bumper: ");
        Serial.println(currentData.bumpRight ? "PRESSED" : "released");
    }
    
    // Check for cliff sensors
    if (currentData.cliffLeft != previousData.cliffLeft) {
        Serial.print("Left cliff: ");
        Serial.println(currentData.cliffLeft ? "DETECTED" : "clear");
    }
    
    if (currentData.cliffFrontLeft != previousData.cliffFrontLeft) {
        Serial.print("Front-left cliff: ");
        Serial.println(currentData.cliffFrontLeft ? "DETECTED" : "clear");
    }
    
    if (currentData.cliffFrontRight != previousData.cliffFrontRight) {
        Serial.print("Front-right cliff: ");
        Serial.println(currentData.cliffFrontRight ? "DETECTED" : "clear");
    }
    
    if (currentData.cliffRight != previousData.cliffRight) {
        Serial.print("Right cliff: ");
        Serial.println(currentData.cliffRight ? "DETECTED" : "clear");
    }
    
    // Check for wheel drops
    if (currentData.wheelDropLeft != previousData.wheelDropLeft) {
        Serial.print("Left wheel: ");
        Serial.println(currentData.wheelDropLeft ? "DROPPED" : "normal");
    }
    
    if (currentData.wheelDropRight != previousData.wheelDropRight) {
        Serial.print("Right wheel: ");
        Serial.println(currentData.wheelDropRight ? "DROPPED" : "normal");
    }
}

/**
 * @brief Print a periodic summary of all sensor data
 */
void printSensorSummary() {
    Serial.println("--- Sensor Summary ---");
    Serial.print("Mode: ");
    Serial.print(static_cast<uint8_t>(currentData.mode));
    Serial.print(" | Battery: ");
    Serial.print(currentData.getBatteryPercentage());
    Serial.print("% (");
    Serial.print(currentData.voltage);
    Serial.print("mV) | Temp: ");
    Serial.print(currentData.temperature);
    Serial.println("°C");
    
    Serial.print("Sensors: ");
    if (currentData.wall) Serial.print("WALL ");
    if (currentData.bumpLeft) Serial.print("BUMP-L ");
    if (currentData.bumpRight) Serial.print("BUMP-R ");
    if (currentData.hasCliffDetection()) Serial.print("CLIFF ");
    if (currentData.hasWheelDrop()) Serial.print("WHEEL-DROP ");
    Serial.println();
    
    Serial.print("Data age: ");
    Serial.print(currentData.getAge());
    Serial.print("ms | Failed attempts: ");
    Serial.println(currentData.failedAttempts);
    Serial.println();
}

/**
 * @brief Setup legacy streaming for backward compatibility
 */
void setupLegacyStreaming() {
    // Legacy sensor list - warning: don't request too many sensors
    // Stream data time slot = 15ms, if the Roomba doesn't have time to 
    // return all sensor data, the stream will be unstable
    char sensorlist[] = {
        ARDUROOMBA_SENSOR_MODE,
        ARDUROOMBA_SENSOR_TEMPERATURE,
        ARDUROOMBA_SENSOR_VOLTAGE,
        ARDUROOMBA_SENSOR_BATTERYCHARGE,
        ARDUROOMBA_SENSOR_BUMPANDWEELSDROPS,
        ARDUROOMBA_SENSOR_WALL,
        ARDUROOMBA_SENSOR_CLIFFLEFT,
        ARDUROOMBA_SENSOR_CLIFFFRONTLEFT,
        ARDUROOMBA_SENSOR_CLIFFRIGHT,
        ARDUROOMBA_SENSOR_CLIFFFRONTRIGHT,
        '\0' // Null terminator
    };
    
    roomba.resetStream();
    delay(100);
    roomba.queryStream(sensorlist);
    
    Serial.println("Legacy streaming setup complete.");
}