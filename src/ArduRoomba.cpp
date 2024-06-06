#include "ArduRoomba.h"

ArduRoomba::ArduRoomba(int rxPin, int txPin, int brcPin)
    : _rxPin(rxPin), _txPin(txPin), _brcPin(brcPin), _irobot(rxPin, txPin)
{
  // Constructor implementation
}

byte ArduRoomba::_parseOneByteStreamBuffer(byte *packets, int &start) 
{
  byte v = packets[start];
  start++;
  return v;
}

int ArduRoomba::_parseTwoByteStreamBuffer(byte *packets, int &start) 
{
  int v = (int)(packets[start] * 256 + packets[start + 1]);
  start += 2;
  return v;
}

void ArduRoomba::_parseAndFillOneByteStreamBuffer(byte *packets, int &start, bool *bytes) 
{
  byte oneByteParsedPacket = (byte)_parseOneByteStreamBuffer(packets, start);
  for(int i=0; i < 7; i++) {
    bytes[i]=(oneByteParsedPacket >> i) & 1;
  }
}

bool ArduRoomba::_parseStreamBuffer(byte *packets, int len, RoombaInfos *infos) 
{
  int i = 0;
  byte packetID;
  bool oneByteParsedPacketBits[7]={false};
  while (i < len) {
    packetID = (byte)_parseOneByteStreamBuffer(packets, i);
    switch (packetID) {
    case ARDUROOMBA_SENSOR_MODE:
      infos->mode = (byte)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_IOSTREAMNUMPACKETS:
      infos->ioStreamNumPackets = (byte)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_SONGNUMBER:
      infos->songNumber = (byte)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_IROPCODE:
      infos->irOpcode = (byte)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_INFRAREDCHARACTERLEFT:
      infos->infraredCharacterLeft = (byte)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_INFRAREDCHARACTERRIGHT:
      infos->infraredCharacterRight = (byte)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_DIRTDETECT:
      infos->dirtdetect = (int)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CHARGINGSTATE:
      infos->chargingState = (byte)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_VOLTAGE:
      infos->voltage = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CURRENT:
      infos->current = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_VELOCITY:
      infos->velocity = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LEFTMOTORCURRENT:
      infos->leftMotorCurrent = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_RIGHTMOTORCURRENT:
      infos->rightMotorCurrent = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_MAINBRUSHMOTORCURRENT:
      infos->mainBrushMotorCurrent = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_SIDEBRUSHMOTORCURRENT:
      infos->sideBrushMotorCurrent = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_RIGHTVELOCITY:
      infos->rightVelocity = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LEFTVELOCITY:
      infos->leftVelocity = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_RADIUS:
      infos->radius = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_WALLSIGNAL:
      infos->wallSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CLIFFLEFTSIGNAL:
      infos->cliffLeftSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CLIFFFRONTLEFTSIGNAL:
      infos->cliffFrontLeftSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CLIFFRIGHTSIGNAL:
      infos->cliffRightSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CLIFFFRONTRIGHTSIGNAL:
      infos->cliffFrontRightSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LIGHTBUMPLEFTSIGNAL:
      infos->lightBumpLeftSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LIGHTBUMPFRONTLEFTSIGNAL:
      infos->lightBumpFrontLeftSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LIGHTBUMPCENTERLEFTSIGNAL:
      infos->lightBumpCenterLeftSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LIGHTBUMPCENTERRIGHTSIGNAL:
      infos->lightBumpCenterLeftSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LIGHTBUMPFRONTRIGHTSIGNAL:
      infos->lightBumpFrontRightSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LIGHTBUMPRIGHTSIGNAL:
      infos->lightBumpRightSignal = (unsigned int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_TEMPERATURE:
      infos->temperature = (char)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_BATTERYCHARGE:
      infos->batteryCharge = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_LEFTENCODERCOUNTS:
      infos->leftEncoderCounts = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_RIGHTENCODERCOUNTS:
      infos->rightEncoderCounts = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_BATTERYCAPACITY:
      infos->batteryCapacity = (int)_parseTwoByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_WALL:
      infos->wall = (bool)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_SONGPLAYING:
      infos->songPlaying = (bool)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_VIRTUALWALL:
      infos->virtualWall = (bool)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CLIFFLEFT:
      infos->cliffLeft = (bool)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CLIFFFRONTLEFT:
      infos->cliffFrontLeft = (bool)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CLIFFRIGHT:
      infos->cliffRight = (bool)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_CLIFFFRONTRIGHT:
      infos->cliffFrontRight = (bool)_parseOneByteStreamBuffer(packets, i);
      break;
    case ARDUROOMBA_SENSOR_BUMPANDWEELSDROPS:
      _parseAndFillOneByteStreamBuffer(packets, i, oneByteParsedPacketBits);
      infos->bumpRight = oneByteParsedPacketBits[0];
      infos->bumpLeft = oneByteParsedPacketBits[1];
      infos->wheelDropRight =oneByteParsedPacketBits[2];
      infos->wheelDropLeft = oneByteParsedPacketBits[3];
      break;
    case ARDUROOMBA_SENSOR_WHEELOVERCURRENTS:
      _parseAndFillOneByteStreamBuffer(packets, i, oneByteParsedPacketBits);
      infos->sideBrushOvercurrent = oneByteParsedPacketBits[0];
      infos->vacuumOvercurrent = oneByteParsedPacketBits[1];
      infos->mainBrushOvercurrent = oneByteParsedPacketBits[2];
      infos->wheelRightOvercurrent = oneByteParsedPacketBits[3];
      infos->wheelLeftOvercurrent = oneByteParsedPacketBits[4];
      break;
    case ARDUROOMBA_SENSOR_BUTTONS:
      _parseAndFillOneByteStreamBuffer(packets, i, oneByteParsedPacketBits);
      infos->cleanButton = oneByteParsedPacketBits[0];
      infos->spotButton = oneByteParsedPacketBits[1];
      infos->dockButton = oneByteParsedPacketBits[2];
      infos->minuteButton = oneByteParsedPacketBits[3];
      infos->hourButton = oneByteParsedPacketBits[4];
      infos->dayButton = oneByteParsedPacketBits[5];
      infos->scheludeButton = oneByteParsedPacketBits[6];
      infos->clockButton = oneByteParsedPacketBits[7];
      break;
    case ARDUROOMBA_SENSOR_LIGHTBUMPER:
      _parseAndFillOneByteStreamBuffer(packets, i, oneByteParsedPacketBits);
      infos->lightBumperLeft = oneByteParsedPacketBits[0];
      infos->lightBumperFrontLeft = oneByteParsedPacketBits[1];
      infos->lightBumperCenterLeft = oneByteParsedPacketBits[2];
      infos->lightBumperCenterRight = oneByteParsedPacketBits[3];
      infos->lightBumperFrontRight = oneByteParsedPacketBits[4];
      infos->lightBumperRight = oneByteParsedPacketBits[5];
      break;
    case ARDUROOMBA_SENSOR_CHARGERAVAILABLE:
      _parseAndFillOneByteStreamBuffer(packets, i, oneByteParsedPacketBits);
      infos->internalChargerAvailable = oneByteParsedPacketBits[0];
      infos->homeBaseChargerAvailable = oneByteParsedPacketBits[1];
      break;
    case ARDUROOMBA_SENSOR_STASIS:
      _parseAndFillOneByteStreamBuffer(packets, i, oneByteParsedPacketBits);
      infos->stasisToggling = oneByteParsedPacketBits[0];
      infos->stasisDisabled = oneByteParsedPacketBits[1];
      break;
    default:
      Serial.print("ArduRoomba::_parseStreamBuffer error: Unhandled Packet ID (");
      Serial.print(packetID, DEC);
      Serial.print(")\n");
      return false;
      break;
    }
  }
  return true;
}

bool ArduRoomba::_readStream() 
{
  unsigned long timeout =
      millis() + ARDUROOMBA_STREAM_TIMEOUT; // stream start every 15ms
  while (!_irobot.available()) {
    if (millis() > timeout) {
      Serial.print("ArduRoomba::_readStream enable to read stream (serial timeout)\n");
      return false; // Timed out
    }
  }

  int state = ARDUROOMBA_STREAM_WAIT_HEADER;
  _streamBufferCursor = 0;
  byte streamSize;
  byte lastChunk;
  byte checksum = 0;
  while (_irobot.available()) {
    byte chunk = _irobot.read();
    switch (state) {
    case ARDUROOMBA_STREAM_WAIT_HEADER:
      if (chunk == 19) {
        state = ARDUROOMBA_STREAM_WAIT_SIZE;
      }
      break;
    case ARDUROOMBA_STREAM_WAIT_SIZE:
      streamSize = chunk;
      state = ARDUROOMBA_STREAM_WAIT_CONTENT;
      break;
    case ARDUROOMBA_STREAM_WAIT_CONTENT:
      if (_streamBufferCursor < streamSize) {
        _streamBuffer[_streamBufferCursor] = chunk;
        _streamBufferCursor++;
      } else {
        state = ARDUROOMBA_STREAM_WAIT_CHECKSUM;
      }
      break;
    case ARDUROOMBA_STREAM_WAIT_CHECKSUM:
      lastChunk = chunk;
      state = ARDUROOMBA_STREAM_END;
      break;
    }
    checksum += chunk;
  }

  bool isChecksum = (checksum & 0xFF) == 0;
  bool isStreamEnd = state == ARDUROOMBA_STREAM_END;
  return isStreamEnd && isChecksum;
}

int ArduRoomba::_sensorsListLength(char sensorlist[]) 
{
  int i;
  int count = 0;
  for (i = 0; sensorlist[i] != '\0'; i++) {
    count++;
  }
  return count;
}

void ArduRoomba::queryStream(char sensorlist[]) 
{
  Serial.print("ArduRoomba::queryStream:\n");
  _nbSensorsStream = _sensorsListLength(sensorlist);
  _irobot.write(148);
  _irobot.write(_nbSensorsStream);
  for (int i = 0; i < _nbSensorsStream; i++) {
    _sensorsStream[i] = sensorlist[i];
    Serial.print(" ");
    Serial.print(_sensorsStream[i], DEC);
    Serial.print("\n");
    _irobot.write(_sensorsStream[i]);
  }
}

void ArduRoomba::resetStream()
{
  Serial.print("ArduRoomba::resetStream\n");
  _irobot.write(148);
  _irobot.write(_zero);
}

bool ArduRoomba::refreshData(RoombaInfos *stateInfos) 
{
  long now = millis();
  if (now > stateInfos->nextRefresh) {
    stateInfos->nextRefresh = now + ARDUROOMBA_REFRESH_DELAY;
    stateInfos->attempt++;
    if (_readStream() &&
        _parseStreamBuffer(_streamBuffer, _streamBufferCursor, stateInfos)) {
      stateInfos->lastSuccedRefresh = now;
      stateInfos->attempt=0;
      return true;
    }
    return false;
  }
  return false;
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
