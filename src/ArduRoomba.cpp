#include "ArduRoomba.h"

ArduRoomba::ArduRoomba(int rxPin, int txPin, int brcPin)
    : _rxPin(rxPin), _txPin(txPin), _brcPin(brcPin), _irobot(rxPin, txPin) {
    // Constructor implementation
}

void ArduRoomba::roombaSetup()
{
    pinMode(_brcPin, OUTPUT);
    digitalWrite(_brcPin, HIGH);         // Ensure it starts HIGH
    delay(2000);                        // Wait 2 seconds after power on
    for (int i = 0; i < 3; i++) {       // Pulse the BRC pin low three times
        digitalWrite(_brcPin, LOW);     // Bring BRC pin LOW
        delay(100);                     // Wait 100ms
        digitalWrite(_brcPin, HIGH);    // Bring BRC pin back HIGH
        delay(100);                     // Wait 100ms before next pulse
    }
    Serial.println("Attempting connection to iRobot OI");
    delay(150);
    _irobot.begin(19200);
    
    Serial.println("Sending Start to OI");
    delay(150);
    _irobot.write(128);
    
    Serial.println("Sending Safe to OI");
    delay(150);
    _irobot.write(131);

    Serial.println("Connection to iRobot OI SHOULD BE established");
    Serial.println("Verify if CLEAN light has stopped illuminating");
    delay(150);
}

void ArduRoomba::goForward() {
    _irobot.write(137); // Opcode for Drive
    _irobot.write(0x01); // High byte for 500 mm/s
    _irobot.write(0xF4); // Low byte for 500 mm/s
    _irobot.write(0x80); // High byte for radius (straight)
    _irobot.write(_zero); // Low byte for radius (straight)
}

void ArduRoomba::goBackward() {
  _irobot.write(137); // Opcode for Drive
  _irobot.write(0xFE); // High byte for -500 mm/s
  _irobot.write(0x0C); // Low byte for -500 mm/s
  _irobot.write(0x80); // High byte for radius (straight)
  _irobot.write(_zero); // Low byte for radius (straight)
}

void ArduRoomba::turnLeft() {
  // Drive command [137], velocity 200 mm/s, radius 1 (turn in place counterclockwise)
  _irobot.write(137);          // Opcode for Drive
  _irobot.write(_zero);         // Velocity high byte (200 mm/s)
  _irobot.write(0xC8);         // Velocity low byte (200 mm/s)
  _irobot.write(_zero);         // Radius high byte (1)
  _irobot.write(0x01);         // Radius low byte (1)
}

void ArduRoomba::turnRight() {
  // Drive command [137], velocity 200 mm/s, radius -1 (turn in place clockwise)
  _irobot.write(137);          // Opcode for Drive
  _irobot.write(_zero);         // Velocity high byte (200 mm/s)
  _irobot.write(0xC8);         // Velocity low byte (200 mm/s)
  _irobot.write(0xFF);         // Radius high byte (-1)
  _irobot.write(0xFF);         // Radius low byte (-1)
}

void ArduRoomba::halt() {
    _irobot.write(137);
    _irobot.write(_zero);
    _irobot.write(_zero);
    _irobot.write(_zero);
    _irobot.write(_zero);
}

void ArduRoomba::getEncoderCounts() {
  // Opcode for Query List: 149
  // Number of packets: 2 (right and left encoder counts)
  // Packet IDs: 43 (Right Encoder Counts), 44 (Left Encoder Counts)
  byte queryCommand[] = {149, 2, 43, 44};

  // Send command to Roomba to request encoder data
  _irobot.write(queryCommand, sizeof(queryCommand));

  // Wait for the data to be returned
  delay(50);  // Adjust this delay as needed

  // Process returned data
  if (_irobot.available() >= 4) { // Expected data length: 2 bytes per packet * 2 packets
    // Create an array to store the data
    byte data[4];

    // Read the data bytes
    for (int i = 0; i < 4; i++) {
      data[i] = _irobot.read();
    }

    // Extract encoder counts from data array
    unsigned int rightEncoderCounts = (data[0] << 8) | data[1];
    unsigned int leftEncoderCounts = (data[2] << 8) | data[3];

  // Send encoder data to Computer via USB

  Serial.print("Left_Encoder:");
  Serial.print(leftEncoderCounts);
  Serial.print(",");
  Serial.print("Right_Encoder:");
  Serial.println(rightEncoderCounts);
  }

  delay(100);
}

void ArduRoomba::getBatteryStatus() {
  // Request battery charge (Packet ID 25)
  _irobot.write(142);
  _irobot.write(25);
  // Read the response and store in _batteryCharge
  if (_irobot.available() >= 2) {
    _batteryCharge = _irobot.read() << 8;
    _batteryCharge |= _irobot.read();
  }

  // Request battery capacity (Packet ID 26)
  _irobot.write(142);
  _irobot.write(26);
  // Read the response and store in _batteryCapacity
  if (_irobot.available() >= 2) {
    _batteryCapacity = _irobot.read() << 8;
    _batteryCapacity |= _irobot.read();
  }
}

bool ArduRoomba::shouldSeekDock() {
  // Calculate the percentage of battery charge
  float chargePercentage = ((float)_batteryCharge / (float)_batteryCapacity) * 100;
  // If the charge is less than a certain threshold, return true
  //Serial.println(chargePercentage);
  return chargePercentage < 20; // For example, if less than 20%
}

void ArduRoomba::seekDock() {
  // Send command to seek dock
  _irobot.write(143); // Dock command
}
