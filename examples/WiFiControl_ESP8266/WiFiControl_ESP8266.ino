/**
 * WiFiControl_ESP8266.ino
 *
 * WiFi control example for ESP8266 boards (ESP-01, ESP-12E, NodeMCU, Wemos D1, etc.)
 * Creates a WiFi access point or connects to existing network with web server control.
 *
 * Hardware:
 * - ESP8266 board (any variant)
 * - iRobot Create 2 or compatible Roomba
 *
 * Connections:
 * - Roomba TX (Mini-DIN pin 4) -> ESP8266 GPIO 4 (D2 on NodeMCU)
 * - Roomba RX (Mini-DIN pin 3) -> ESP8266 GPIO 5 (D1 on NodeMCU)
 * - Roomba DD (Mini-DIN pin 5) -> ESP8266 GPIO 2 (D4 on NodeMCU)
 * - GND (Mini-DIN pin 6/7) -> ESP8266 GND
 *
 * Note: Adjust pins based on your specific ESP8266 board variant.
 */

#include "ArduRoomba.h"
#include "extensions/ArduRoombaESP8266WiFi.h"

// Pin configuration for ESP8266
// Common NodeMCU pin mapping:
// D2 = GPIO 4 (RX)
// D1 = GPIO 5 (TX)
// D4 = GPIO 2 (BRC)
const uint8_t ROOMBDA_RX_PIN = 4;
const uint8_t ROOMBDA_TX_PIN = 5;
const uint8_t ROOMBDA_BRC_PIN = 2;

// Roomba instance
ArduRoomba roomba(ROOMBDA_RX_PIN, ROOMBDA_TX_PIN, ROOMBDA_BRC_PIN);

// WiFi configuration - Choose AP or Client mode
#define USE_AP_MODE true

// AP Mode configuration
const char* AP_SSID = "ArduRoomba-ESP8266";
const char* AP_PASSWORD = "roomba123";  // Set to nullptr for open network

// Client mode configuration
// const char* WIFI_SSID = "YourWiFiNetwork";
// const char* WIFI_PASSWORD = "YourPassword";

// WiFi extension
ArduRoombaESP8266WiFi wifi(roomba);

// LED indicator (built-in LED on most ESP8266 boards)
const int LED_PIN = 2;  // GPIO 2 = D4 on NodeMCU (active LOW)
bool ledState = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED off (active low on most boards)

  Serial.println("\n=== ArduRoomba WiFi Control (ESP8266) ===");

  // Initialize Roomba
  Serial.println("Initializing Roomba...");
  if (!roomba.begin()) {
    Serial.println("ERROR: Failed to connect to Roomba!");
    Serial.println("Check wiring and power, then reset.");
    errorBlink();
    while (1) yield(); // ESP8266 needs yield() in loops
  }
  Serial.println("✓ Roomba connected!");

  // Start WiFi
  Serial.println("Starting WiFi...");

#if USE_AP_MODE
  // Access Point mode
  if (!wifi.beginAP(AP_SSID, AP_PASSWORD)) {
    Serial.println("ERROR: Failed to create WiFi AP!");
    errorBlink();
    while (1) yield();
  }
  Serial.println("✓ WiFi AP created!");
#else
  // Client mode
  if (!wifi.beginClient(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("ERROR: Failed to connect to WiFi!");
    errorBlink();
    while (1) yield();
  }
  Serial.println("✓ WiFi connected!");
#endif

  // Start web server
  wifi.startWebServer(80);
  wifi.setLowBatteryThreshold(12000); // Stop commands below 12V

  Serial.println("\n=== Setup Complete ===");
#if USE_AP_MODE
  Serial.println("Connect to WiFi network:");
  Serial.print("  SSID: ");
  Serial.println(AP_SSID);
  if (AP_PASSWORD) {
    Serial.print("  Password: ");
    Serial.println(AP_PASSWORD);
  }
#endif
  Serial.println("\nOpen browser to:");
  Serial.print("  http://");
  Serial.println(wifi.getIPAddress());
  Serial.print("  Signal Strength: ");
  Serial.print(wifi.getRSSI());
  Serial.println(" dBm");
  Serial.println("\nReady for commands!");
}

void loop() {
  // Handle incoming web requests
  wifi.handleClient();

  // LED indicator for activity
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 100) {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? LOW : HIGH);
  }

  // Monitor Roomba safety (battery, bumpers, cliffs)
  static unsigned long lastSafetyCheck = 0;
  if (millis() - lastSafetyCheck > 500) {
    lastSafetyCheck = millis();
    safetyCheck();
  }

  yield(); // ESP8266 needs yield in loops
}

/**
 * Periodic safety check
 * Monitors battery and sensors for safe operation
 */
void safetyCheck() {
  // Check battery status
  uint16_t voltage = roomba.getBatteryVoltage();
  static bool wasLow = false;

  if (voltage > 0 && voltage < 12000) {
    if (!wasLow) {
      Serial.println("WARNING: Battery critically low!");
      roomba.actuators().playAlertSong();
      wifi.enableRemoteControl(false); // Disable remote commands
      wasLow = true;
    }
  } else if (voltage > 0 && voltage < 13500) {
    static unsigned long lastWarning = 0;
    if (millis() - lastWarning > 30000) { // Warn every 30s
      Serial.println("WARNING: Battery low");
      lastWarning = millis();
    }
  } else {
    wasLow = false;
    wifi.enableRemoteControl(true);
  }

  // Optional: Automatic obstacle avoidance when idle
  // Uncomment to enable:
  /*
  if (!wifi.isRemoteEnabled()) {
    if (roomba.isBumperPressed()) {
      Serial.println("Auto-avoidance: Bumper hit");
      roomba.moveBackward(150);
      delay(500);
      roomba.spinRight(150);
      delay(500);
      roomba.stop();
    }
  }
  */
}

/**
 * Error indication - fast blink
 */
void errorBlink() {
  while (true) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    yield();
  }
}
