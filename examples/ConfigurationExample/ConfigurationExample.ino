/**
 * @file ConfigurationExample.ino
 * @brief Demonstrates the ArduRoomba Configuration Management System
 * 
 * This example shows how to use the new Configuration Management System
 * to store and manage settings persistently. It eliminates the need for
 * hardcoded values and enables web-based configuration.
 * 
 * Features demonstrated:
 * - Persistent configuration storage
 * - Web-based configuration interface
 * - Configuration validation
 * - Factory reset capabilities
 * - JSON serialization for API access
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * Hardware Requirements:
 * - WiFi-capable board (ESP32, ESP8266, or Arduino Uno R4 WiFi)
 * - iRobot Roomba/Create 2 compatible robot
 * 
 * Wiring:
 * - Pin 2 -> Roomba TX (receive data from Roomba)
 * - Pin 3 -> Roomba RX (send data to Roomba)
 * - Pin 4 -> Roomba BRC (Baud Rate Change pin)
 * - GND -> Roomba GND
 * - VCC -> Roomba VCC (with appropriate voltage level)
 * 
 * Usage:
 * 1. Upload this sketch to your board
 * 2. Open Serial Monitor to see configuration status
 * 3. Connect to the WiFi network or access point
 * 4. Visit the web interface to configure settings
 * 5. Settings are automatically saved and persist across reboots
 */

#include "ArduRoomba.h"

// Only compile for supported platforms
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)
#include "wireless/ArduRoombaWiFi.h"
#include "core/ArduRoombaConfig.h"

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

const uint8_t ROOMBA_RX_PIN = 2;  // Arduino pin connected to Roomba TX
const uint8_t ROOMBA_TX_PIN = 3;  // Arduino pin connected to Roomba RX  
const uint8_t ROOMBA_BRC_PIN = 4; // Arduino pin connected to Roomba BRC

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

ArduRoomba roomba(ROOMBA_RX_PIN, ROOMBA_TX_PIN, ROOMBA_BRC_PIN);
ArduRoombaWiFi wifi(roomba);
ArduRoomba::ArduRoombaConfig config;

// Status tracking
unsigned long lastStatusPrint = 0;
const unsigned long STATUS_INTERVAL = 30000;  // Print status every 30 seconds

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("ArduRoomba Configuration Example v2.5.0");
    Serial.println("Demonstrates persistent configuration management");
    Serial.println();
    
    // Initialize configuration system
    Serial.println("Initializing configuration system...");
    config.setDebugEnabled(true);
    
    ArduRoomba::ErrorCode configResult = config.begin();
    if (configResult == ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("✓ Configuration system ready");
        Serial.println("Storage: " + config.getStorageType());
    } else {
        Serial.println("✗ Configuration system failed");
    }
    
    // Initialize Roomba
    Serial.println("Initializing Roomba...");
    ArduRoomba::ErrorCode roombaResult = roomba.initialize();
    if (roombaResult == ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("✓ Roomba initialized");
        roomba.safe();
    } else {
        Serial.println("✗ Roomba initialization failed");
    }
    
    // Initialize WiFi with configuration
    Serial.println("Starting WiFi...");
    wifi.setDebugEnabled(config.isDebugEnabled());
    
    String ssid = config.getWiFiSSID();
    if (ssid.length() > 0) {
        wifi.begin(ssid, config.getWiFiPassword(), config.getHostname());
    } else {
        Serial.println("No WiFi configured - starting AP mode");
        // Will start in AP mode automatically
    }
    
    // Start web server
    wifi.startWebServer(config.getServerPort());
    
    Serial.println("Setup complete!");
    Serial.println("Access web interface at: " + wifi.getServerURL());
    Serial.println();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    wifi.handleClient();
    
    if (millis() - lastStatusPrint > STATUS_INTERVAL) {
        printStatus();
        lastStatusPrint = millis();
    }
    
    delay(10);
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void printStatus() {
    Serial.println("--- Status ---");
    Serial.println("WiFi: " + String(wifi.getStatus() == ArduRoomba::WiFiStatus::CONNECTED ? "Connected" : "Disconnected"));
    Serial.println("Config modified: " + String(config.isModified() ? "Yes" : "No"));
    Serial.println("Device: " + config.getDeviceName() + " @ " + config.getLocation());
    Serial.println();
}

#else

void setup() {
    Serial.begin(115200);
    Serial.println("This example requires a WiFi-capable board (ESP32, ESP8266, Uno R4 WiFi)");
}

void loop() {
    delay(1000);
}

#endif