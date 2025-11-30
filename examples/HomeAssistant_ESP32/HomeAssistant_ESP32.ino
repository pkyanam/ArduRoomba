/**
 * HomeAssistant_ESP32.ino - Home Assistant MQTT integration example
 *
 * This example demonstrates how to integrate ArduRoomba with Home Assistant
 * using MQTT. Features include:
 * - Automatic entity discovery in Home Assistant
 * - Vacuum entity with start/stop/dock commands
 * - Battery, wall sensor, and bumper sensor entities
 * - Real-time state updates
 *
 * Hardware Requirements:
 * - ESP32 board
 * - iRobot Create 2 or Roomba 500/600/700 series
 *
 * Software Requirements:
 * - Home Assistant with MQTT integration configured
 * - MQTT broker (Mosquitto recommended)
 * - PubSubClient library (install via Arduino Library Manager)
 *
 * Wiring:
 * - Roomba TX → ESP32 GPIO 16 (RX)
 * - Roomba RX → ESP32 GPIO 17 (TX)
 * - Roomba DD → ESP32 GPIO 4 (BRC)
 * - Common ground between ESP32 and Roomba
 *
 * Setup:
 * 1. Install PubSubClient library in Arduino IDE
 * 2. Configure WiFi and MQTT settings below
 * 3. Upload to ESP32
 * 4. Home Assistant will auto-discover the vacuum entity
 *
 * Home Assistant Commands:
 * - vacuum.start - Start cleaning
 * - vacuum.stop - Stop cleaning
 * - vacuum.return_to_base - Return to dock
 * - vacuum.locate - Beep to locate
 *
 * Part of the ArduRoomba library
 * https://github.com/pkyanam/ArduRoomba
 */

#include <WiFi.h>
#include <ArduRoomba.h>
#include <adapters/ArduRoombaHomeAssistant.h>

// ============================================
// Configuration - Edit these values
// ============================================

// WiFi Configuration
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";

// MQTT Configuration
const char* MQTT_SERVER = "192.168.1.100";  // Your Home Assistant/MQTT broker IP
const uint16_t MQTT_PORT = 1883;
const char* MQTT_USER = nullptr;             // Set if MQTT requires authentication
const char* MQTT_PASSWORD = nullptr;         // Set if MQTT requires authentication

// Device Configuration
const char* DEVICE_ID = "arduroomba_living_room";  // Unique ID for this device
const char* DEVICE_NAME = "Living Room Roomba";    // Display name in Home Assistant

// Roomba Pin Configuration
const int ROOMBA_RX_PIN = 16;  // ESP32 RX from Roomba TX
const int ROOMBA_TX_PIN = 17;  // ESP32 TX to Roomba RX
const int ROOMBA_BRC_PIN = 4;  // Baud Rate Change pin

// ============================================
// Global Objects
// ============================================

ArduRoomba roomba(ROOMBA_RX_PIN, ROOMBA_TX_PIN, ROOMBA_BRC_PIN);
ArduRoombaHomeAssistant homeAssistant(roomba);

// Status LED (optional - uses built-in LED)
const int STATUS_LED = 2;

// ============================================
// Setup
// ============================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ArduRoomba - Home Assistant Integration");
  Serial.println("=======================================");

  // Setup status LED
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  // Connect to WiFi
  connectWiFi();

  // Initialize Roomba
  Serial.println("Initializing Roomba...");
  roomba.begin();
  roomba.setDebug(true);

  // Configure Home Assistant adapter
  homeAssistant.setDeviceId(DEVICE_ID);
  homeAssistant.setDeviceName(DEVICE_NAME);
  homeAssistant.setDebug(true);
  homeAssistant.setUpdateInterval(10000);  // Update state every 10 seconds

  // Set custom command callback (optional)
  homeAssistant.setCommandCallback(onCommand);

  // Connect to MQTT
  Serial.println("Connecting to MQTT broker...");
  if (homeAssistant.begin(MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("Connected to Home Assistant!");
    Serial.println("Check Home Assistant for the new vacuum entity.");
    digitalWrite(STATUS_LED, HIGH);
  } else {
    Serial.println("Failed to connect to MQTT broker");
    Serial.println("Will retry in the main loop...");
  }

  Serial.println();
  Serial.println("Setup complete!");
  Serial.println();
}

// ============================================
// Main Loop
// ============================================

void loop() {
  // Handle Home Assistant MQTT events
  homeAssistant.handle();

  // Update status LED based on connection
  static unsigned long lastBlink = 0;
  if (!homeAssistant.isConnected()) {
    // Blink when disconnected
    if (millis() - lastBlink > 500) {
      lastBlink = millis();
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
    }
  } else {
    digitalWrite(STATUS_LED, HIGH);
  }

  // Print status periodically (for debugging)
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 30000) {
    lastStatus = millis();
    printStatus();
  }
}

// ============================================
// Helper Functions
// ============================================

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
    Serial.println("Check credentials and restart.");
    while (true) {
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
      delay(100);
    }
  }
}

void onCommand(SmartHomeCommand cmd) {
  // Custom command handler - called when Home Assistant sends a command
  // You can add custom logic here

  Serial.print("Received command: ");
  switch (cmd) {
    case SmartHomeCommand::START_CLEANING:
      Serial.println("START_CLEANING");
      break;
    case SmartHomeCommand::STOP:
      Serial.println("STOP");
      break;
    case SmartHomeCommand::DOCK:
      Serial.println("DOCK");
      break;
    case SmartHomeCommand::SPOT_CLEAN:
      Serial.println("SPOT_CLEAN");
      break;
    case SmartHomeCommand::BEEP:
      Serial.println("BEEP");
      break;
    default:
      Serial.println("UNKNOWN");
      break;
  }
}

void printStatus() {
  Serial.println("--- Status ---");
  Serial.print("WiFi: ");
  Serial.println(WiFi.isConnected() ? "Connected" : "Disconnected");
  Serial.print("MQTT: ");
  Serial.println(homeAssistant.isConnected() ? "Connected" : "Disconnected");
  Serial.print("Battery: ");
  Serial.print(roomba.getBatteryVoltage());
  Serial.println(" mV");
  Serial.print("Wall: ");
  Serial.println(roomba.isWallDetected() ? "Yes" : "No");
  Serial.print("Bumper: ");
  Serial.println(roomba.isBumperPressed() ? "Pressed" : "Clear");
  Serial.println("--------------");
}
