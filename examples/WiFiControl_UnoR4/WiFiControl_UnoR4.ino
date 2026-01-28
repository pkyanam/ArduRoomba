/**
 * WiFiControl_UnoR4.ino
 *
 * WiFi control example for Arduino Uno R4 WiFi
 * Creates a WiFi access point or connects to existing network with web server control.
 *
 * Hardware:
 * - Arduino Uno R4 WiFi
 * - iRobot Create 2 or compatible Roomba (500/600/700 series)
 *
 * Connections:
 * - Roomba TX (Mini-DIN pin 4) -> Arduino Pin 2 (RX)
 * - Roomba RX (Mini-DIN pin 3) -> Arduino Pin 3 (TX)
 * - Roomba DD (Mini-DIN pin 5) -> Arduino Pin 4 (BRC)
 * - GND (Mini-DIN pin 6/7) -> Arduino GND
 *
 * Features:
 * - Responsive web interface for control
 * - Real-time battery and sensor status
 * - Low battery protection
 * - Compatible with mobile and desktop browsers
 */

#include "ArduRoomba.h"
#include "extensions/ArduRoombaWiFiS3.h"

// Roomba configuration
ArduRoomba roomba(2, 3, 4);  // RX, TX, BRC pins

// WiFi configuration - Choose AP or Client mode
#define USE_AP_MODE true

// AP Mode: Roomba creates its own network
const char* AP_SSID = "ArduRoomba-R4";
const char* AP_PASSWORD = "roomba123";  // Set to nullptr for open network

// Client Mode: Roomba connects to your existing WiFi
// const char* WIFI_SSID = "YourWiFiNetwork";
// const char* WIFI_PASSWORD = "YourPassword";

// WiFi extension
ArduRoombaWiFiS3 wifi(roomba);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  Serial.println("\n=== ArduRoomba WiFi Control (Uno R4 WiFi) ===");

  // Initialize Roomba
  Serial.println("Initializing Roomba...");
  if (!roomba.begin()) {
    Serial.println("ERROR: Failed to connect to Roomba!");
    Serial.println("Check wiring and power, then reset.");
    while (1) delay(1000);
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
    while (1) delay(1000);
  }
  Serial.println("✓ WiFi AP created!");
#else
  // Client mode
  if (!wifi.beginClient(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("ERROR: Failed to connect to WiFi!");
    while (1) delay(1000);
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
