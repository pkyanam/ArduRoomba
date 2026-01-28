/**
 * SimpleControl.ino
 * 
 * Interactive control example using Serial commands.
 * Send single character commands to control the Roomba.
 */

#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4);

void setup() {
  Serial.begin(19200);
  
  if (roomba.begin()) {
    Serial.println("ArduRoomba V3 - Simple Control");
    Serial.println("==============================");
    printHelp();
  } else {
    Serial.println("Failed to connect to Roomba!");
    while(1);
  }
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    processCommand(command);
  }
  
  // Flash dock LED to show we're alive
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  
  if (millis() - lastBlink > 2000) {
    roomba.setLED(false, false, ledState);
    ledState = !ledState;
    lastBlink = millis();
  }
}

void processCommand(char cmd) {
  switch (cmd) {
    case 'w':
    case 'W':
      Serial.println("Moving forward");
      roomba.moveForward(200);
      break;
      
    case 's':
    case 'S':
      Serial.println("Moving backward");
      roomba.moveBackward(200);
      break;
      
    case 'a':
    case 'A':
      Serial.println("Turning left");
      roomba.turnLeft(150);
      break;
      
    case 'd':
    case 'D':
      Serial.println("Turning right");
      roomba.turnRight(150);
      break;
      
    case ' ':
      Serial.println("STOP");
      roomba.stop();
      break;
      
    case 'c':
    case 'C':
      Serial.println("Starting cleaning mode");
      roomba.startCleaning();
      break;
      
    case 'p':
    case 'P':
      Serial.println("Spot cleaning");
      roomba.spotClean();
      break;
      
    case 'h':
    case 'H':
      Serial.println("Going home (dock)");
      roomba.dock();
      break;
      
    case 'b':
    case 'B':
      Serial.println("Beep!");
      roomba.beep();
      break;
      
    case 'm':
    case 'M':
      Serial.println("Toggling main brush");
      toggleBrushes();
      break;
      
    case 'i':
    case 'I':
      printSensorInfo();
      break;
      
    case '?':
      printHelp();
      break;
      
    case '\n':
    case '\r':
      // Ignore newlines
      break;
      
    default:
      Serial.print("Unknown command: ");
      Serial.println(cmd);
      Serial.println("Type '?' for help");
      break;
  }
}

void printHelp() {
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  W - Move forward");
  Serial.println("  S - Move backward");
  Serial.println("  A - Turn left");
  Serial.println("  D - Turn right");
  Serial.println("  SPACE - Stop");
  Serial.println("  C - Start cleaning");
  Serial.println("  P - Spot clean");
  Serial.println("  H - Go home (dock)");
  Serial.println("  B - Beep");
  Serial.println("  M - Toggle brushes");
  Serial.println("  I - Show sensor info");
  Serial.println("  ? - Show this help");
  Serial.println();
}

void toggleBrushes() {
  static bool brushesOn = false;
  brushesOn = !brushesOn;
  
  roomba.setBrushes(brushesOn, brushesOn, false);
  
  Serial.print("Brushes ");
  Serial.println(brushesOn ? "ON" : "OFF");
}

void printSensorInfo() {
  Serial.println("\n=== Sensor Status ===");
  
  uint16_t voltage = roomba.getBatteryVoltage();
  int16_t current = roomba.getBatteryCurrent();
  
  Serial.print("Battery: ");
  Serial.print(voltage);
  Serial.print(" mV, ");
  Serial.print(current);
  Serial.println(" mA");
  
  Serial.print("Wall detected: ");
  Serial.println(roomba.isWallDetected() ? "YES" : "NO");
  
  Serial.print("Bumper pressed: ");
  Serial.println(roomba.isBumperPressed() ? "YES" : "NO");
  
  Serial.println("====================\n");
}