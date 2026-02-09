/**
 * BLEControl_UnoR4.ino
 *
 * Bluetooth Low Energy control example for Arduino Uno R4 WiFi.
 * Creates a BLE GATT server for mobile app control.
 *
 * Hardware:
 * - Arduino Uno R4 WiFi
 * - iRobot Create 2 or compatible Roomba
 *
 * Connections (Uno R4 uses Serial1 hardware serial):
 * - Roomba TX (Mini-DIN pin 4) -> Arduino Pin 0 (Serial1 RX)
 * - Roomba RX (Mini-DIN pin 3) -> Arduino Pin 1 (Serial1 TX)
 * - Roomba DD (Mini-DIN pin 5) -> Arduino Pin 4 (BRC)
 * - GND (Mini-DIN pin 6/7) -> Arduino GND
 *
 * IMPORTANT: The Uno R4 WiFi does NOT support SoftwareSerial.
 * It uses Serial1 (hardware serial) on pins 0 and 1.
 * You can still use the USB serial monitor (Serial) for debugging.
 *
 * BLE Service Info:
 * - Service UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
 * - Command Characteristic (Write): beb5483e-36e1-4688-b7f5-ea07361b26a8
 * - Status Characteristic (Read/Notify): beb5483f-36e1-4688-b7f5-ea07361b26a8
 *
 * Command Format: "action:speed:duration"
 * Examples:
 *   - "forward:200:0" - Move forward at 200mm/s continuously
 *   - "left:150:1000" - Turn left at 150mm/s for 1 second
 *   - "stop:0:0" - Stop
 *   - "clean:0:0" - Start cleaning mode
 *   - "dock:0:0" - Return to dock
 *
 * Status Format: "voltage:connected:wall:bumper:cliff:remote"
 * Example: "15800:1:0:0:0:1" (15800mV, connected, no wall, no bumper, no cliff, remote enabled)
 *
 * Mobile App Suggestions:
 * - nRF Connect (Android/iOS) - For testing and development
 * - LightBlue (iOS) - BLE explorer and testing
 * - BLE Scanner (Android) - BLE device scanner
 */

#include "ArduRoomba.h"
#include "extensions/ArduRoombaBLE.h"

// Roomba configuration for Uno R4 WiFi (uses Serial1 on pins 0/1)
// Pin 4 is used for BRC (Baud Rate Change / Device Detect)
ArduRoomba roomba(RoombaConfig::createUnoR4(4));

// BLE configuration
const char* BLE_DEVICE_NAME = "ArduRoomba-R4";

// BLE extension
ArduRoombaBLE ble(roomba, BLE_DEVICE_NAME);

// LED indicator
const int LED_PIN = LED_BUILTIN;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Uno R4 has active HIGH LED

  Serial.println("\n=== ArduRoomba BLE Control (Uno R4 WiFi) ===");

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

  // Initialize BLE
  Serial.println("Starting BLE service...");
  if (!ble.isAvailable()) {
    Serial.println("ERROR: BLE not available on this board!");
    errorBlink();
    while (1) delay(100);
  }

  if (!ble.begin()) {
    Serial.println("ERROR: Failed to initialize BLE!");
    errorBlink();
    while (1) delay(100);
  }

  // Optional: Set custom command callback
  // ble.setCommandCallback(onBLECommand);

  // Enable low battery protection
  ble.enableRemoteControl(true);

  Serial.println("\n=== Setup Complete ===");
  Serial.println("BLE Device Information:");
  Serial.print("  Name: ");
  Serial.println(BLE_DEVICE_NAME);
  Serial.println("  Platform: Uno R4 WiFi");
  Serial.println("\nService UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b");
  Serial.println("\nUse a BLE app to connect:");
  Serial.println("  - nRF Connect (Android/iOS)");
  Serial.println("  - LightBlue (iOS)");
  Serial.println("  - BLE Scanner (Android)");
  Serial.println("\nWaiting for BLE connections...");
  Serial.println("LED will blink when client is connected.");
}

void loop() {
  // Update BLE status (sends notifications to connected clients)
  ble.updateStatus();

  // LED indicator for connection status
  static unsigned long lastBlink = 0;
  static bool ledState = false;

  if (ble.isConnected()) {
    // Fast blink when connected
    if (millis() - lastBlink > 250) {
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  } else {
    // Slow blink when waiting for connection
    if (millis() - lastBlink > 1000) {
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  }

  // Monitor Roomba status periodically
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint > 10000) { // Every 10 seconds
    lastStatusPrint = millis();

    Serial.println("\n--- Status ---");
    Serial.print("BLE Connected: ");
    Serial.println(ble.isConnected() ? "Yes" : "No");
    Serial.print("Connection Count: ");
    Serial.println(ble.getConnectionCount());
    Serial.print("Battery Voltage: ");
    Serial.print(roomba.getBatteryVoltage());
    Serial.println(" mV");
    Serial.print("Battery Percent: ");
    Serial.print(roomba.getBatteryPercent());
    Serial.println("%");
    Serial.print("Wall Detected: ");
    Serial.println(roomba.isWallDetected() ? "Yes" : "No");
    Serial.print("Bumper Pressed: ");
    Serial.println(roomba.isBumperPressed() ? "Yes" : "No");
  }

  // Optional: Safety features
  safetyCheck();

  delay(10);
}

/**
 * Safety check for battery and obstacles
 */
void safetyCheck() {
  // Auto-stop on critically low battery
  uint16_t voltage = roomba.getBatteryVoltage();
  static bool wasCritical = false;

  if (voltage > 0 && voltage < 12000) {
    if (!wasCritical) {
      Serial.println("CRITICAL: Battery very low, stopping!");
      roomba.stop();
      ble.enableRemoteControl(false);  // Disable remote commands
      roomba.actuators().playAlertSong();
      wasCritical = true;
    }
  } else if (voltage > 13500) {
    wasCritical = false;
    ble.enableRemoteControl(true);
  }

  // Optional: Automatic obstacle avoidance when not connected
  // Uncomment to enable:
  /*
  if (!ble.isConnected()) {
    if (roomba.isBumperPressed()) {
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
 * Optional: Custom BLE command handler
 * Uncomment and register in setup() with ble.setCommandCallback(onBLECommand)
 */
/*
void onBLECommand(const String& command) {
  Serial.print("BLE Command received: ");
  Serial.println(command);

  // Parse command
  int firstColon = command.indexOf(':');
  String action = command.substring(0, firstColon);

  // Add custom logic here:
  // - Log commands to SD card
  // - Trigger external relays or devices
  // - Send notifications
  // - Implement custom behaviors

  // Visual feedback
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }
}
*/

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
