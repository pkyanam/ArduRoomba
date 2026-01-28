/**
 * WiFiControl_ESP32.ino
 *
 * WiFi control example for ESP32 boards
 * Creates a WiFi access point or connects to existing network with web server control.
 *
 * Hardware:
 * - ESP32 board (ESP32-DevKitC, ESP32-WROOM, NodeMCU-32, etc.)
 * - iRobot Create 2 or compatible Roomba (500/600/700 series)
 *
 * Connections:
 * - Roomba TX (Mini-DIN pin 4) -> ESP32 GPIO 16 (RX2)
 * - Roomba RX (Mini-DIN pin 3) -> ESP32 GPIO 17 (TX2)
 * - Roomba DD (Mini-DIN pin 5) -> ESP32 GPIO 5 (BRC)
 * - GND (Mini-DIN pin 6/7) -> ESP32 GND
 *
 * Note: This example uses Hardware Serial 2 for better performance.
 * You can also use SoftwareSerial by changing the pin configuration.
 *
 * Features:
 * - Responsive web interface for control
 * - Real-time battery and sensor status
 * - Low battery protection
 * - Compatible with mobile and desktop browsers
 */

#include "ArduRoomba.h"
#include "extensions/ArduRoombaESP32WiFi.h"

// Roomba configuration with ESP32 hardware serial
// Uses Serial2 by default for better performance
RoombaConfig config = RoombaConfig::createESP32(&Serial2, 5);
ArduRoomba roomba(config);

// WiFi configuration - Choose AP or Client mode
#define USE_AP_MODE true

// AP Mode: Roomba creates its own network
const char* AP_SSID = "ArduRoomba-ESP32";
const char* AP_PASSWORD = "roomba123";  // Set to nullptr for open network

// Client Mode: Roomba connects to your existing WiFi
// const char* WIFI_SSID = "YourWiFiNetwork";
// const char* WIFI_PASSWORD = "YourPassword";

// WiFi extension
ArduRoombaESP32WiFi wifi(roomba);

// LED indicator (built-in LED on most ESP32 boards)
const int LED_PIN = 2;  // GPIO 2

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("\n=== ArduRoomba WiFi Control (ESP32) ===");

  // Initialize Roomba
  Serial.println("Initializing Roomba...");
  if (!roomba.begin()) {
    Serial.println("ERROR: Failed to connect to Roomba!");
    Serial.println("Check wiring and power, then reset.");
    errorBlink();
    while (1) delay(100);
  }
  Serial.println("✓ Roomba connected!");

  // Play startup sound
  roomba.actuators().playStartupSong();

  // Start WiFi
  Serial.println("Starting WiFi...");

#if USE_AP_MODE
  // Access Point mode
  if (!wifi.beginAP(AP_SSID, AP_PASSWORD)) {
    Serial.println("ERROR: Failed to create WiFi AP!");
    errorBlink();
    while (1) delay(100);
  }
  Serial.println("✓ WiFi AP created!");
#else
  // Client mode
  if (!wifi.beginClient(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("ERROR: Failed to connect to WiFi!");
    errorBlink();
    while (1) delay(100);
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

#if !USE_AP_MODE
  Serial.print("  Signal Strength: ");
  Serial.print(wifi.getRSSI());
  Serial.println(" dBm");
#endif

  Serial.println("\nReady for commands!");
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  // Handle incoming web requests
  wifi.handleClient();

  // Monitor Roomba safety
  static unsigned long lastSafetyCheck = 0;
  if (millis() - lastSafetyCheck > 500) {
    lastSafetyCheck = millis();
    safetyCheck();
  }

  delay(10);
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
      digitalWrite(LED_PIN, LOW);
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
    digitalWrite(LED_PIN, HIGH);
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
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

// Optional: Custom command handler
// Uncomment and register with wifi.setCommandCallback(onCommand) in setup()
/*
void onCommand(const RoombaCommand& cmd, CommandResult result) {
  Serial.print("Command: ");
  Serial.print(cmd.action);
  Serial.print(" | Speed: ");
  Serial.print(cmd.speed);
  Serial.print(" | Result: ");

  switch (result) {
    case CommandResult::SUCCESS: Serial.println("OK"); break;
    case CommandResult::LOW_BATTERY: Serial.println("LOW BATTERY"); break;
    case CommandResult::UNKNOWN_ACTION: Serial.println("UNKNOWN"); break;
    default: Serial.println("ERROR"); break;
  }

  // Add custom logic here:
  // - Log commands to SD card
  // - Trigger external devices
  // - Send notifications
}
*/
