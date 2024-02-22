#include "ArduRoomba.h"

ArduRoomba::ArduRoomba(int rxPin, int txPin, int brcPin)
    : _rxPin(rxPin), _txPin(txPin), _brcPin(brcPin), _irobot(rxPin, txPin)
{
  // Constructor implementation
}

// OI commands
void ArduRoomba::start()
{
  _irobot.write(128);
}

void ArduRoomba::baud(char baudCode)
{
  _irobot.write(129);
  _irobot.write(baudCode);
}

void ArduRoomba::safe()
{
  _irobot.write(131);
}

void ArduRoomba::full()
{
  _irobot.write(132);
}

void ArduRoomba::clean()
{
  _irobot.write(135);
}

void ArduRoomba::maxClean()
{
  _irobot.write(136);
}

void ArduRoomba::spot()
{
  _irobot.write(134);
}

void ArduRoomba::seekDock()
{
  _irobot.write(143);
}

void ArduRoomba::schedule(ScheduleStore scheduleData)
{
  _irobot.write(167);
  _irobot.write(scheduleData.sunHour);
  _irobot.write(scheduleData.sunMinute);
  _irobot.write(scheduleData.monHour);
  _irobot.write(scheduleData.monMinute);
  _irobot.write(scheduleData.tueHour);
  _irobot.write(scheduleData.tueMinute);
  _irobot.write(scheduleData.wedHour);
  _irobot.write(scheduleData.wedMinute);
  _irobot.write(scheduleData.thuHour);
  _irobot.write(scheduleData.thuMinute);
  _irobot.write(scheduleData.friHour);
  _irobot.write(scheduleData.friMinute);
  _irobot.write(scheduleData.satHour);
  _irobot.write(scheduleData.satMinute);
}

void ArduRoomba::setDayTime(char day, char hour, char minute)
{
  _irobot.write(168);
  _irobot.write(day);
  _irobot.write(hour);
  _irobot.write(minute);
}

void ArduRoomba::power()
{
  _irobot.write(133);
}

// Actuator commands
void ArduRoomba::drive(int velocity, int radius)
{
  _irobot.write(137);
  _irobot.write((velocity >> 8) & 0xFF);
  _irobot.write(velocity & 0xFF);
  _irobot.write((radius >> 8) & 0xFF);
  _irobot.write(radius & 0xFF);
}

void ArduRoomba::driveDirect(int rightVelocity, int leftVelocity)
{
  _irobot.write(145);
  _irobot.write((rightVelocity >> 8) & 0xFF);
  _irobot.write(rightVelocity & 0xFF);
  _irobot.write((leftVelocity >> 8) & 0xFF);
  _irobot.write(leftVelocity & 0xFF);
}

void ArduRoomba::drivePWM(int rightPWM, int leftPWM)
{
  _irobot.write(146);
  _irobot.write((rightPWM >> 8) & 0xFF);
  _irobot.write(rightPWM & 0xFF);
  _irobot.write((leftPWM >> 8) & 0xFF);
  _irobot.write(leftPWM & 0xFF);
}

void ArduRoomba::motors(byte data)
{
  _irobot.write(138);
  _irobot.write(data);
}

void ArduRoomba::pwmMotors(char mainBrushPWM, char sideBrushPWM, char vacuumPWM)
{
  _irobot.write(144);
  _irobot.write(mainBrushPWM);
  _irobot.write(sideBrushPWM);
  _irobot.write(vacuumPWM);
}

void ArduRoomba::leds(int ledBits, int powerColor, int powerIntensity)
{
  _irobot.write(139);
  _irobot.write(ledBits);
  _irobot.write(powerColor);
  _irobot.write(powerIntensity);
}

void ArduRoomba::schedulingLeds(int weekDayLedBits, int scheduleLedBits)
{
  _irobot.write(162);
  _irobot.write(weekDayLedBits);
  _irobot.write(scheduleLedBits);
}

void ArduRoomba::digitLedsRaw(int digitThree, int digitTwo, int digitOne, int digitZero)
{
  _irobot.write(163);
  _irobot.write(digitThree);
  _irobot.write(digitTwo);
  _irobot.write(digitOne);
  _irobot.write(digitZero);
}

void ArduRoomba::song(Song songData)
{
  _irobot.write(140);
  _irobot.write(songData.songNumber);
  _irobot.write(songData.songLength);
  for (int i = 0; i < songData.songLength; i++)
  {
    _irobot.write(songData.notes[i].noteNumber);
    _irobot.write(songData.notes[i].noteDuration);
  }
}

void ArduRoomba::play(int songNumber)
{
  _irobot.write(141);
  _irobot.write(songNumber);
}

// Input commands
void ArduRoomba::sensors(char packetID)
{
  // Read the data and print it to the serial console
  Serial.print("Packet ID: ");
  Serial.print(packetID, DEC);
  Serial.print(", Data: ");

  _irobot.write(142);
  _irobot.write(packetID);

  delay(15);

  // Read the data in chunks
  while (_irobot.available() > 0)
  {
    byte buffer[64];
    size_t len = _irobot.readBytes(buffer, sizeof(buffer));
    for (size_t i = 0; i < len; i++)
    {
      Serial.print(buffer[i], DEC);
      Serial.print(" ");
    }
  }
  Serial.println();
}

void ArduRoomba::queryList(byte numPackets, byte *packetIDs)
{
  _irobot.write(149);
  _irobot.write(numPackets);
  for (int i = 0; i < numPackets; i++)
  {
    _irobot.write(packetIDs[i]);
  }

  // Read the data and print it to the serial console
  for (int i = 0; i < numPackets; i++)
  {
    Serial.print("Packet ID: ");
    Serial.print(packetIDs[i], DEC);
    Serial.print(", Data: ");
    while (_irobot.available() > 0)
    {
      Serial.print(_irobot.read(), DEC);
      Serial.print(" ");
    }
    Serial.println();
  }
}

// Custom commands
void ArduRoomba::roombaSetup()
{
  pinMode(_brcPin, OUTPUT);
  digitalWrite(_brcPin, HIGH); // Ensure it starts HIGH
  delay(2000);                 // Wait 2 seconds after power on
  for (int i = 0; i < 3; i++)
  {                              // Pulse the BRC pin low three times
    digitalWrite(_brcPin, LOW);  // Bring BRC pin LOW
    delay(100);                  // Wait 100ms
    digitalWrite(_brcPin, HIGH); // Bring BRC pin back HIGH
    delay(100);                  // Wait 100ms before next pulse
  }
  Serial.println("Attempting connection to iRobot OI");
  delay(150);
  _irobot.begin(19200);

  Serial.println("Sending Start to OI");
  delay(150);
  start();

  Serial.println("Sending Safe to OI");
  delay(150);
  safe();

  Serial.println("Connection to iRobot OI SHOULD BE established");
  Serial.println("Verify if CLEAN light has stopped illuminating");
  delay(150);
}

void ArduRoomba::goForward()
{
  _irobot.write(137);   // Opcode for Drive
  _irobot.write(0x01);  // High byte for 500 mm/s
  _irobot.write(0xF4);  // Low byte for 500 mm/s
  _irobot.write(0x80);  // High byte for radius (straight)
  _irobot.write(_zero); // Low byte for radius (straight)
}

void ArduRoomba::goBackward()
{
  _irobot.write(137);   // Opcode for Drive
  _irobot.write(0xFE);  // High byte for -500 mm/s
  _irobot.write(0x0C);  // Low byte for -500 mm/s
  _irobot.write(0x80);  // High byte for radius (straight)
  _irobot.write(_zero); // Low byte for radius (straight)
}

void ArduRoomba::turnLeft()
{
  // Drive command [137], velocity 200 mm/s, radius 1 (turn in place counterclockwise)
  _irobot.write(137);   // Opcode for Drive
  _irobot.write(_zero); // Velocity high byte (200 mm/s)
  _irobot.write(0xC8);  // Velocity low byte (200 mm/s)
  _irobot.write(_zero); // Radius high byte (1)
  _irobot.write(0x01);  // Radius low byte (1)
}

void ArduRoomba::turnRight()
{
  // Drive command [137], velocity 200 mm/s, radius -1 (turn in place clockwise)
  _irobot.write(137);   // Opcode for Drive
  _irobot.write(_zero); // Velocity high byte (200 mm/s)
  _irobot.write(0xC8);  // Velocity low byte (200 mm/s)
  _irobot.write(0xFF);  // Radius high byte (-1)
  _irobot.write(0xFF);  // Radius low byte (-1)
}

void ArduRoomba::halt()
{
  _irobot.write(137);
  _irobot.write(_zero);
  _irobot.write(_zero);
  _irobot.write(_zero);
  _irobot.write(_zero);
}
