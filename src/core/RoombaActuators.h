/**
 * @file RoombaActuators.h
 * @brief Roomba actuator control (LEDs, motors, sounds)
 *
 * Provides control for all Roomba outputs including:
 * - LEDs (power, debris, spot, dock, check robot)
 * - Motors (main brush, side brush, vacuum)
 * - Sounds (songs and tones)
 */

#ifndef ROOMBA_ACTUATORS_H
#define ROOMBA_ACTUATORS_H

#include <Arduino.h>
#include "RoombaSerial.h"

/**
 * LED bit masks for OI command
 */
enum class LedMask : uint8_t {
  DEBRIS     = 0x01,  // Debris LED (blue)
  SPOT       = 0x02,  // Spot LED (green)
  DOCK       = 0x04,  // Dock LED (green)
  CHECK_ROBOT = 0x08, // Check Robot LED (red)
};

/**
 * Power LED color (0-255, green to red)
 */
enum class PowerColor : uint8_t {
  RED     = 255,   // Full red
  ORANGE  = 200,   // Orange
  YELLOW  = 128,   // Yellow
  LIME    = 64,    // Lime
  GREEN   = 0,     // Full green
};

/**
 * Note definitions for songs
 */
enum Note : uint8_t {
  NOTE_REST = 0,
  NOTE_A3 = 57,   NOTE_A3s = 58,
  NOTE_B3 = 59,
  NOTE_C4 = 60,   NOTE_C4s = 61,
  NOTE_D4 = 62,   NOTE_D4s = 63,
  NOTE_E4 = 64,
  NOTE_F4 = 65,   NOTE_F4s = 66,
  NOTE_G4 = 67,   NOTE_G4s = 68,
  NOTE_A4 = 69,   NOTE_A4s = 70,
  NOTE_B4 = 71,
  NOTE_C5 = 72,   NOTE_C5s = 73,
  NOTE_D5 = 74,   NOTE_D5s = 75,
  NOTE_E5 = 76,
  NOTE_F5 = 77,   NOTE_F5s = 78,
  NOTE_G5 = 79,   NOTE_G5s = 80,
  NOTE_A5 = 81,   NOTE_A5s = 82,
  NOTE_B5 = 83,
  NOTE_C6 = 84,   NOTE_C6s = 85,
  NOTE_D6 = 86,   NOTE_D6s = 87,
  NOTE_E6 = 88,
  NOTE_F6 = 89,   NOTE_F6s = 90,
  NOTE_G6 = 91,   NOTE_G6s = 92,
};

/**
 * Song note structure
 */
struct SongNote {
  uint8_t note;
  uint8_t duration; // Duration in 1/64 seconds

  SongNote() : note(NOTE_REST), duration(0) {}
  SongNote(uint8_t n, uint8_t d) : note(n), duration(d) {}
};

/**
 * Roomba actuator control interface
 */
class RoombaActuators {
public:
  explicit RoombaActuators(RoombaSerial* serial);
  ~RoombaActuators() = default;

  // LED control
  void setLED(bool debris, bool spot, bool dock, bool checkRobot);
  void setLED(uint8_t ledBits);
  void setPowerLED(uint8_t color, uint8_t intensity = 255);
  void setPowerLED(PowerColor color, uint8_t intensity = 255);
  void setAllLEDs(uint8_t ledBits, uint8_t powerColor, uint8_t powerIntensity);

  // Motor control
  void setMotors(bool mainBrush, bool sideBrush, bool vacuum);
  void setMainBrush(bool on, int8_t direction = 1); // direction: -1, 0, 1
  void setSideBrush(bool on, int8_t direction = 1);
  void setVacuum(bool on);
  void stopAllMotors();

  // Sound control
  void beep();
  void playTone(uint8_t note, uint8_t duration);
  void playSong(uint8_t songNumber);
  void defineSong(uint8_t songNumber, const SongNote* notes, uint8_t noteCount);

  // Predefined songs
  void playStartupSong();
  void playHappySong();
  void playSadSong();
  void playAlertSong();
  void definePredefinedSongs();

  // Cleaning modes
  void startCleaning();
  void startSpotClean();
  void startMaxClean();
  void seekDock();
  void powerOff();

  // Mode control
  void setSafeMode();
  void setFullMode();

  // Debug
  void setDebug(bool enable) { _debug = enable; }

private:
  RoombaSerial* _serial;
  bool _debug;
  bool _songsDefined;

  // Helper methods
  void sendCommand(uint8_t cmd);
  void sendCommand(uint8_t cmd, uint8_t param);
  void sendCommand(uint8_t cmd, uint8_t param1, uint8_t param2);
  void sendCommand(uint8_t cmd, const uint8_t* params, uint8_t numParams);
  void debugPrint(const char* msg);
};

#endif
