/**
 * @file RoombaActuators.cpp
 * @brief Implementation of Roomba actuator control
 */

#include "RoombaActuators.h"

RoombaActuators::RoombaActuators(RoombaSerial* serial)
  : _serial(serial), _debug(false), _songsDefined(false) {
}

// LED control
void RoombaActuators::setLED(bool debris, bool spot, bool dock, bool checkRobot) {
  uint8_t ledBits = 0;
  if (debris) ledBits |= (uint8_t)LedMask::DEBRIS;
  if (spot) ledBits |= (uint8_t)LedMask::SPOT;
  if (dock) ledBits |= (uint8_t)LedMask::DOCK;
  if (checkRobot) ledBits |= (uint8_t)LedMask::CHECK_ROBOT;
  setLED(ledBits);
}

void RoombaActuators::setLED(uint8_t ledBits) {
  uint8_t params[3] = {ledBits, 0, 255}; // Green power LED by default
  sendCommand(OI_LEDS, params, 3);
}

void RoombaActuators::setPowerLED(uint8_t color, uint8_t intensity) {
  uint8_t params[3] = {0, color, intensity};
  sendCommand(OI_LEDS, params, 3);
}

void RoombaActuators::setPowerLED(PowerColor color, uint8_t intensity) {
  setPowerLED((uint8_t)color, intensity);
}

void RoombaActuators::setAllLEDs(uint8_t ledBits, uint8_t powerColor, uint8_t powerIntensity) {
  uint8_t params[3] = {ledBits, powerColor, powerIntensity};
  sendCommand(OI_LEDS, params, 3);
}

// Motor control
void RoombaActuators::setMotors(bool mainBrush, bool sideBrush, bool vacuum) {
  uint8_t motorBits = 0;
  if (sideBrush) motorBits |= 0x01;
  if (vacuum) motorBits |= 0x02;
  if (mainBrush) motorBits |= 0x04;

  sendCommand(OI_MOTORS, motorBits);
  debugPrint("Motors set", motorBits);
}

void RoombaActuators::setMainBrush(bool on, int8_t direction) {
  uint8_t motorBits = 0;
  if (on) {
    motorBits |= 0x04; // Main brush on
    // Note: OI doesn't support individual brush direction control
  }
  sendCommand(OI_MOTORS, motorBits);
}

void RoombaActuators::setSideBrush(bool on, int8_t direction) {
  uint8_t motorBits = 0;
  if (on) {
    motorBits |= 0x01; // Side brush on
  }
  sendCommand(OI_MOTORS, motorBits);
}

void RoombaActuators::setVacuum(bool on) {
  uint8_t motorBits = 0;
  if (on) {
    motorBits |= 0x02; // Vacuum on
  }
  sendCommand(OI_MOTORS, motorBits);
}

void RoombaActuators::stopAllMotors() {
  sendCommand(OI_MOTORS, 0);
  debugPrint("All motors stopped");
}

// Sound control
void RoombaActuators::beep() {
  // Simple beep using single note song
  SongNote note(NOTE_C5, 32); // Middle C for 0.5 seconds
  defineSong(0, &note, 1);
  delay(20);
  playSong(0);
}

void RoombaActuators::playTone(uint8_t note, uint8_t duration) {
  SongNote n(note, duration);
  defineSong(0, &n, 1);
  delay(20);
  playSong(0);
  debugPrint("Playing tone", note);
}

void RoombaActuators::playSong(uint8_t songNumber) {
  sendCommand(OI_PLAY, songNumber);
}

void RoombaActuators::defineSong(uint8_t songNumber, const SongNote* notes, uint8_t noteCount) {
  if (noteCount == 0 || noteCount > 16) return; // Max 16 notes per song

  // Calculate total data size: song number, note count, then 2 bytes per note
  uint8_t dataSize = 2 + (noteCount * 2);
  uint8_t* data = new uint8_t[dataSize];

  data[0] = songNumber;
  data[1] = noteCount;

  for (uint8_t i = 0; i < noteCount; i++) {
    data[2 + (i * 2)] = notes[i].note;
    data[2 + (i * 2) + 1] = notes[i].duration;
  }

  sendCommand(OI_SONG, data, dataSize);
  delete[] data;
}

void RoombaActuators::playStartupSong() {
  if (!_songsDefined) definePredefinedSongs();
  playSong(0);
}

void RoombaActuators::playHappySong() {
  if (!_songsDefined) definePredefinedSongs();
  playSong(1);
}

void RoombaActuators::playSadSong() {
  if (!_songsDefined) definePredefinedSongs();
  playSong(2);
}

void RoombaActuators::playAlertSong() {
  if (!_songsDefined) definePredefinedSongs();
  playSong(3);
}

void RoombaActuators::definePredefinedSongs() {
  // Song 0: Startup - ascending notes
  SongNote startup[] = {
    SongNote(NOTE_C4, 16),
    SongNote(NOTE_E4, 16),
    SongNote(NOTE_G4, 16),
    SongNote(NOTE_C5, 32)
  };
  defineSong(0, startup, 4);

  // Song 1: Happy - major arpeggio
  SongNote happy[] = {
    SongNote(NOTE_C5, 12),
    SongNote(NOTE_E5, 12),
    SongNote(NOTE_G5, 12),
    SongNote(NOTE_C6, 24)
  };
  defineSong(1, happy, 4);

  // Song 2: Sad - descending minor
  SongNote sad[] = {
    SongNote(NOTE_A4, 20),
    SongNote(NOTE_G4, 20),
    SongNote(NOTE_F4, 20),
    SongNote(NOTE_D4, 40)
  };
  defineSong(2, sad, 4);

  // Song 3: Alert - quick beeps
  SongNote alert[] = {
    SongNote(NOTE_G5, 8),
    SongNote(NOTE_REST, 8),
    SongNote(NOTE_G5, 8),
    SongNote(NOTE_REST, 8),
    SongNote(NOTE_G5, 8)
  };
  defineSong(3, alert, 5);

  _songsDefined = true;
}

// Cleaning modes
void RoombaActuators::startCleaning() {
  sendCommand(OI_CLEAN);
  debugPrint("Clean mode started");
}

void RoombaActuators::startSpotClean() {
  sendCommand(OI_SPOT);
  debugPrint("Spot clean started");
}

void RoombaActuators::startMaxClean() {
  sendCommand(OI_MAX_CLEAN);
  debugPrint("Max clean started");
}

void RoombaActuators::seekDock() {
  sendCommand(OI_SEEK_DOCK);
  debugPrint("Seeking dock");
}

void RoombaActuators::powerOff() {
  sendCommand(OI_POWER);
  debugPrint("Powering off");
}

// Mode control
void RoombaActuators::setSafeMode() {
  sendCommand(OI_SAFE);
  debugPrint("Safe mode activated");
}

void RoombaActuators::setFullMode() {
  sendCommand(OI_FULL);
  debugPrint("Full mode activated");
}

// Helper methods
void RoombaActuators::sendCommand(uint8_t cmd) {
  if (_serial && _serial->isActive()) {
    _serial->write(cmd);
  }
}

void RoombaActuators::sendCommand(uint8_t cmd, uint8_t param) {
  if (_serial && _serial->isActive()) {
    _serial->write(cmd);
    _serial->write(param);
  }
}

void RoombaActuators::sendCommand(uint8_t cmd, uint8_t param1, uint8_t param2) {
  if (_serial && _serial->isActive()) {
    _serial->write(cmd);
    _serial->write(param1);
    _serial->write(param2);
  }
}

void RoombaActuators::sendCommand(uint8_t cmd, const uint8_t* params, uint8_t numParams) {
  if (_serial && _serial->isActive() && params) {
    _serial->write(cmd);
    _serial->write(params, numParams);
  }
}

void RoombaActuators::debugPrint(const char* msg) {
  if (_debug && msg) {
    Serial.print("RoombaActuators: ");
    Serial.println(msg);
  }
}

void RoombaActuators::debugPrint(const char* msg, int value) {
  if (_debug && msg) {
    Serial.print("RoombaActuators: ");
    Serial.print(msg);
    Serial.print(" = ");
    Serial.println(value);
  }
}
