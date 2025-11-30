/**
 * AlexaControl_ESP32.ino - Amazon Alexa voice control example
 *
 * This example demonstrates how to control ArduRoomba with Amazon Alexa
 * using the Fauxmo library. No cloud services or Alexa skills required!
 *
 * Features:
 * - Local network voice control
 * - Multiple virtual devices for different commands
 * - Zero cloud dependency
 * - Works with any Alexa-enabled device
 *
 * Hardware Requirements:
 * - ESP32 board
 * - iRobot Create 2 or Roomba 500/600/700 series
 *
 * Software Requirements:
 * - fauxmoESP library (install via Arduino Library Manager)
 *
 * Wiring:
 * - Roomba TX → ESP32 GPIO 16 (RX)
 * - Roomba RX → ESP32 GPIO 17 (TX)
 * - Roomba DD → ESP32 GPIO 4 (BRC)
 * - Common ground between ESP32 and Roomba
 *
 * Setup:
 * 1. Install fauxmoESP library in Arduino IDE
 * 2. Configure WiFi settings below
 * 3. Upload to ESP32
 * 4. Say "Alexa, discover devices"
 * 5. Alexa will find your Roomba devices
 *
 * Default Voice Commands:
 * - "Alexa, turn on Roomba" - Start cleaning
 * - "Alexa, turn off Roomba" - Stop cleaning
 * - "Alexa, turn on Roomba Dock" - Return to charging dock
 * - "Alexa, turn on Roomba Spot" - Start spot cleaning
 * - "Alexa, turn on Roomba Locate" - Beep to find the Roomba
 *
 * Note: Device names can be customized in the configuration below.
 *
 * Part of the ArduRoomba library
 * https://github.com/pkyanam/ArduRoomba
 */

#include <WiFi.h>
#include <ArduRoomba.h>
#include <adapters/ArduRoombaAlexa.h>

// ============================================
// Configuration - Edit these values
// ============================================

// WiFi Configuration
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";

// Device Names (what you say to Alexa)
// Customize these to your preference
const char* DEVICE_NAME_MAIN = "Roomba";        // "Alexa, turn on Roomba"
const char* DEVICE_NAME_DOCK = "Roomba Dock";   // "Alexa, turn on Roomba Dock"
const char* DEVICE_NAME_SPOT = "Roomba Spot";   // "Alexa, turn on Roomba Spot"
const char* DEVICE_NAME_LOCATE = "Find Roomba"; // "Alexa, turn on Find Roomba"

// Set to false to only create the main device
const bool ENABLE_EXTRA_DEVICES = true;

// Roomba Pin Configuration
const int ROOMBA_RX_PIN = 16;  // ESP32 RX from Roomba TX
const int ROOMBA_TX_PIN = 17;  // ESP32 TX to Roomba RX
const int ROOMBA_BRC_PIN = 4;  // Baud Rate Change pin

// ============================================
// Global Objects
// ============================================

ArduRoomba roomba(ROOMBA_RX_PIN, ROOMBA_TX_PIN, ROOMBA_BRC_PIN);
ArduRoombaAlexa alexa(roomba);

// Status LED (optional - uses built-in LED)
const int STATUS_LED = 2;

// ============================================
// Setup
// ============================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ArduRoomba - Alexa Voice Control");
  Serial.println("=================================");

  // Setup status LED
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  // Connect to WiFi
  connectWiFi();

  // Initialize Roomba
  Serial.println("Initializing Roomba...");
  roomba.begin();
  roomba.setDebug(true);

  // Configure Alexa adapter
  alexa.setMainDeviceName(DEVICE_NAME_MAIN);
  alexa.setDockDeviceName(DEVICE_NAME_DOCK);
  alexa.setSpotDeviceName(DEVICE_NAME_SPOT);
  alexa.setLocateDeviceName(DEVICE_NAME_LOCATE);
  alexa.enableExtraDevices(ENABLE_EXTRA_DEVICES);
  alexa.setDebug(true);

  // Set custom command callback (optional)
  alexa.setCommandCallback(onCommand);

  // Initialize Alexa integration
  if (alexa.begin()) {
    Serial.println();
    Serial.println("Alexa integration ready!");
    Serial.println();
    Serial.println("Say 'Alexa, discover devices' to find your Roomba.");
    Serial.println();
    Serial.println("Available voice commands:");
    Serial.print("  - 'Alexa, turn on ");
    Serial.print(DEVICE_NAME_MAIN);
    Serial.println("' - Start cleaning");
    Serial.print("  - 'Alexa, turn off ");
    Serial.print(DEVICE_NAME_MAIN);
    Serial.println("' - Stop");

    if (ENABLE_EXTRA_DEVICES) {
      Serial.print("  - 'Alexa, turn on ");
      Serial.print(DEVICE_NAME_DOCK);
      Serial.println("' - Return to dock");
      Serial.print("  - 'Alexa, turn on ");
      Serial.print(DEVICE_NAME_SPOT);
      Serial.println("' - Spot clean");
      Serial.print("  - 'Alexa, turn on ");
      Serial.print(DEVICE_NAME_LOCATE);
      Serial.println("' - Beep");
    }
    Serial.println();

    digitalWrite(STATUS_LED, HIGH);
  } else {
    Serial.println("Failed to initialize Alexa adapter");
    Serial.println("Make sure WiFi is connected.");
  }

  // Example: Add a custom device
  // Uncomment to add a custom "Roomba Forward" device
  /*
  alexa.addDevice("Roomba Forward",
    []() { roomba.moveForward(150); },  // ON callback
    []() { roomba.stop(); }              // OFF callback
  );
  */

  Serial.println("Setup complete!");
  Serial.println();
}

// ============================================
// Main Loop
// ============================================

void loop() {
  // Handle Alexa events - IMPORTANT: must be called frequently
  alexa.handle();

  // Update status LED based on connection
  static unsigned long lastBlink = 0;
  if (!WiFi.isConnected()) {
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
    Serial.println();

    // Important: Alexa needs to be on the same network
    Serial.println("Make sure your Alexa device is on the same network!");
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
  // Custom command handler - called when Alexa triggers a command
  // The default action is already executed, this is for additional logic

  Serial.print("Alexa command received: ");
  switch (cmd) {
    case SmartHomeCommand::START_CLEANING:
      Serial.println("START_CLEANING");
      // Add custom logic here, e.g., turn on a status light
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
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Alexa: ");
  Serial.println(alexa.isConnected() ? "Ready" : "Not Ready");
  Serial.print("Battery: ");
  Serial.print(roomba.getBatteryVoltage());
  Serial.println(" mV");
  Serial.println("--------------");
}
