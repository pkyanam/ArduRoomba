/**
 * @file ArduRoombaCommands.cpp
 * @brief Implementation of command functionality for the ArduRoomba library
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.3.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#include "ArduRoombaCommands.h"

namespace ArduRoomba {

// ============================================================================
// CONSTRUCTOR AND DESTRUCTOR
// ============================================================================

RoombaCommands::RoombaCommands(RoombaCore& core)
    : _core(core), _debugEnabled(false), _lastCommand(OIOpcode::START),
      _commandsSent(0), _errorCount(0) {
}

RoombaCommands::~RoombaCommands() {
    // Destructor - no cleanup needed
}

// ============================================================================
// OPEN INTERFACE MODE CONTROL
// ============================================================================

ErrorCode RoombaCommands::start() {
    return sendCommand(OIOpcode::START);
}

ErrorCode RoombaCommands::safeMode() {
    return sendCommand(OIOpcode::SAFE);
}

ErrorCode RoombaCommands::fullMode() {
    return sendCommand(OIOpcode::FULL);
}

ErrorCode RoombaCommands::powerDown() {
    return sendCommand(OIOpcode::POWER);
}

ErrorCode RoombaCommands::changeBaudRate(uint32_t baudRate) {
    return _core.changeBaudRate(baudRate);
}

// ============================================================================
// CLEANING MODES
// ============================================================================

ErrorCode RoombaCommands::startCleaning() {
    return sendCommand(OIOpcode::CLEAN);
}

ErrorCode RoombaCommands::startMaxCleaning() {
    return sendCommand(OIOpcode::MAX_CLEAN);
}

ErrorCode RoombaCommands::startSpotCleaning() {
    return sendCommand(OIOpcode::SPOT);
}

ErrorCode RoombaCommands::seekDock() {
    return sendCommand(OIOpcode::SEEK_DOCK);
}

// ============================================================================
// MOVEMENT CONTROL
// ============================================================================

ErrorCode RoombaCommands::drive(int16_t velocity, int16_t radius) {
    if (!isValidVelocity(velocity) || !isValidRadius(radius)) {
        debugPrint("Invalid drive parameters");
        return ErrorCode::INVALID_PARAMETER;
    }
    
    uint8_t params[4];
    params[0] = (velocity >> 8) & 0xFF;  // Velocity high byte
    params[1] = velocity & 0xFF;         // Velocity low byte
    params[2] = (radius >> 8) & 0xFF;    // Radius high byte
    params[3] = radius & 0xFF;           // Radius low byte
    
    debugPrint("Drive velocity", velocity);
    debugPrint("Drive radius", radius);
    
    return sendCommand(OIOpcode::DRIVE, params, 4);
}

ErrorCode RoombaCommands::driveDirect(int16_t rightVelocity, int16_t leftVelocity) {
    if (!isValidVelocity(rightVelocity) || !isValidVelocity(leftVelocity)) {
        debugPrint("Invalid direct drive parameters");
        return ErrorCode::INVALID_PARAMETER;
    }
    
    uint8_t params[4];
    params[0] = (rightVelocity >> 8) & 0xFF;  // Right velocity high byte
    params[1] = rightVelocity & 0xFF;         // Right velocity low byte
    params[2] = (leftVelocity >> 8) & 0xFF;   // Left velocity high byte
    params[3] = leftVelocity & 0xFF;          // Left velocity low byte
    
    debugPrint("Drive direct right", rightVelocity);
    debugPrint("Drive direct left", leftVelocity);
    
    return sendCommand(OIOpcode::DRIVE_DIRECT, params, 4);
}

ErrorCode RoombaCommands::drivePWM(int16_t rightPWM, int16_t leftPWM) {
    if (!isValidPWM(rightPWM) || !isValidPWM(leftPWM)) {
        debugPrint("Invalid PWM drive parameters");
        return ErrorCode::INVALID_PARAMETER;
    }
    
    uint8_t params[4];
    params[0] = (rightPWM >> 8) & 0xFF;  // Right PWM high byte
    params[1] = rightPWM & 0xFF;         // Right PWM low byte
    params[2] = (leftPWM >> 8) & 0xFF;   // Left PWM high byte
    params[3] = leftPWM & 0xFF;          // Left PWM low byte
    
    debugPrint("Drive PWM right", rightPWM);
    debugPrint("Drive PWM left", leftPWM);
    
    return sendCommand(OIOpcode::DRIVE_PWM, params, 4);
}

ErrorCode RoombaCommands::stop() {
    return drive(0, 0);
}

// ============================================================================
// CONVENIENT MOVEMENT METHODS
// ============================================================================

ErrorCode RoombaCommands::moveForward(int16_t velocity) {
    velocity = clampVelocity(abs(velocity)); // Ensure positive
    return drive(velocity, DriveRadius::STRAIGHT);
}

ErrorCode RoombaCommands::moveBackward(int16_t velocity) {
    velocity = -clampVelocity(abs(velocity)); // Ensure negative
    return drive(velocity, DriveRadius::STRAIGHT);
}

ErrorCode RoombaCommands::turnLeft(int16_t velocity) {
    velocity = clampVelocity(abs(velocity)); // Ensure positive
    return drive(velocity, DriveRadius::TURN_IN_PLACE_CCW);
}

ErrorCode RoombaCommands::turnRight(int16_t velocity) {
    velocity = clampVelocity(abs(velocity)); // Ensure positive
    return drive(velocity, DriveRadius::TURN_IN_PLACE_CW);
}

ErrorCode RoombaCommands::turnLeftRadius(int16_t velocity, int16_t radius) {
    velocity = clampVelocity(abs(velocity)); // Ensure positive
    radius = abs(radius); // Ensure positive for left turn
    return drive(velocity, radius);
}

ErrorCode RoombaCommands::turnRightRadius(int16_t velocity, int16_t radius) {
    velocity = clampVelocity(abs(velocity)); // Ensure positive
    radius = -abs(radius); // Ensure negative for right turn
    return drive(velocity, radius);
}

// ============================================================================
// ACTUATOR CONTROL
// ============================================================================

ErrorCode RoombaCommands::setMotors(bool sideBrush, bool vacuum, bool mainBrush, 
                                   bool sideBrushDirection, bool mainBrushDirection) {
    uint8_t motorBits = 0;
    
    if (sideBrush) motorBits |= MotorBits::SIDE_BRUSH;
    if (vacuum) motorBits |= MotorBits::VACUUM;
    if (mainBrush) motorBits |= MotorBits::MAIN_BRUSH;
    if (sideBrushDirection) motorBits |= MotorBits::SIDE_BRUSH_OPPOSITE;
    if (mainBrushDirection) motorBits |= MotorBits::MAIN_BRUSH_OPPOSITE;
    
    debugPrint("Motor bits", motorBits);
    
    return sendCommand(OIOpcode::MOTORS, motorBits);
}

ErrorCode RoombaCommands::setMotorsPWM(int8_t mainBrushPWM, int8_t sideBrushPWM, int8_t vacuumPWM) {
    uint8_t params[3];
    params[0] = static_cast<uint8_t>(mainBrushPWM);
    params[1] = static_cast<uint8_t>(sideBrushPWM);
    params[2] = static_cast<uint8_t>(vacuumPWM);
    
    debugPrint("Main brush PWM", mainBrushPWM);
    debugPrint("Side brush PWM", sideBrushPWM);
    debugPrint("Vacuum PWM", vacuumPWM);
    
    return sendCommand(OIOpcode::PWM_MOTORS, params, 3);
}

// ============================================================================
// LED CONTROL
// ============================================================================

ErrorCode RoombaCommands::setLEDs(bool checkRobot, bool dock, bool spot, bool debris, 
                                 uint8_t powerColor, uint8_t powerIntensity) {
    uint8_t ledBits = 0;
    
    if (checkRobot) ledBits |= LEDBits::CHECK_ROBOT;
    if (dock) ledBits |= LEDBits::DOCK;
    if (spot) ledBits |= LEDBits::SPOT;
    if (debris) ledBits |= LEDBits::DEBRIS;
    
    uint8_t params[3];
    params[0] = ledBits;
    params[1] = powerColor;
    params[2] = powerIntensity;
    
    debugPrint("LED bits", ledBits);
    debugPrint("Power color", powerColor);
    debugPrint("Power intensity", powerIntensity);
    
    return sendCommand(OIOpcode::LEDS, params, 3);
}

ErrorCode RoombaCommands::setPowerLED(uint8_t color, uint8_t intensity) {
    return setLEDs(false, false, false, false, color, intensity);
}

ErrorCode RoombaCommands::setSchedulingLEDs(uint8_t weekdayLEDs, uint8_t scheduleLEDs) {
    uint8_t params[2];
    params[0] = weekdayLEDs;
    params[1] = scheduleLEDs;
    
    debugPrint("Weekday LEDs", weekdayLEDs);
    debugPrint("Schedule LEDs", scheduleLEDs);
    
    return sendCommand(OIOpcode::SCHEDULING_LEDS, params, 2);
}

ErrorCode RoombaCommands::setDigitLEDs(uint8_t digit3, uint8_t digit2, uint8_t digit1, uint8_t digit0) {
    uint8_t params[4];
    params[0] = digit3;
    params[1] = digit2;
    params[2] = digit1;
    params[3] = digit0;
    
    debugPrint("Setting digit LEDs");
    
    return sendCommand(OIOpcode::DIGIT_LEDS_RAW, params, 4);
}

// ============================================================================
// SONG MANAGEMENT
// ============================================================================

ErrorCode RoombaCommands::defineSong(const Song& song) {
    if (!song.isValid()) {
        debugPrint("Invalid song data");
        return ErrorCode::INVALID_PARAMETER;
    }
    
    uint8_t params[2 + (song.songLength * 2)];
    params[0] = song.songNumber;
    params[1] = song.songLength;
    
    for (uint8_t i = 0; i < song.songLength; i++) {
        params[2 + (i * 2)] = song.notes[i].noteNumber;
        params[2 + (i * 2) + 1] = song.notes[i].noteDuration;
    }
    
    debugPrint("Defining song", song.songNumber);
    debugPrint("Song length", song.songLength);
    
    return sendCommand(OIOpcode::SONG, params, 2 + (song.songLength * 2));
}

ErrorCode RoombaCommands::playSong(uint8_t songNumber) {
    if (songNumber > Song::MAX_SONGS) {
        debugPrint("Invalid song number");
        return ErrorCode::INVALID_PARAMETER;
    }
    
    debugPrint("Playing song", songNumber);
    
    return sendCommand(OIOpcode::PLAY, songNumber);
}

ErrorCode RoombaCommands::beep(uint8_t frequency, uint8_t duration, uint8_t songSlot) {
    Song beepSong;
    beepSong.songNumber = songSlot;
    beepSong.songLength = 1;
    beepSong.notes[0] = Note(frequency, duration);
    
    ErrorCode result = defineSong(beepSong);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    delay(50); // Small delay to ensure song is defined
    return playSong(songSlot);
}

ErrorCode RoombaCommands::beepSequence(uint8_t count, uint8_t frequency, uint8_t duration, 
                                      uint8_t pause, uint8_t songSlot) {
    if (count == 0 || count > Song::MAX_NOTES / 2) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    Song beepSong;
    beepSong.songNumber = songSlot;
    beepSong.songLength = 0;
    
    for (uint8_t i = 0; i < count; i++) {
        // Add beep note
        if (!beepSong.addNote(Note(frequency, duration))) {
            break;
        }
        
        // Add pause note (if not the last beep)
        if (i < count - 1) {
            if (!beepSong.addNote(Note(31, pause))) { // Note 31 is very low/quiet
                break;
            }
        }
    }
    
    ErrorCode result = defineSong(beepSong);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    delay(50); // Small delay to ensure song is defined
    return playSong(songSlot);
}

// ============================================================================
// SCHEDULING
// ============================================================================

ErrorCode RoombaCommands::setSchedule(const ScheduleData& schedule) {
    if (!schedule.isValid()) {
        debugPrint("Invalid schedule data");
        return ErrorCode::INVALID_PARAMETER;
    }
    
    uint8_t params[14];
    params[0] = schedule.sunHour;
    params[1] = schedule.sunMinute;
    params[2] = schedule.monHour;
    params[3] = schedule.monMinute;
    params[4] = schedule.tueHour;
    params[5] = schedule.tueMinute;
    params[6] = schedule.wedHour;
    params[7] = schedule.wedMinute;
    params[8] = schedule.thuHour;
    params[9] = schedule.thuMinute;
    params[10] = schedule.friHour;
    params[11] = schedule.friMinute;
    params[12] = schedule.satHour;
    params[13] = schedule.satMinute;
    
    debugPrint("Setting schedule");
    
    return sendCommand(OIOpcode::SCHEDULE, params, 14);
}

ErrorCode RoombaCommands::setDayTime(uint8_t day, uint8_t hour, uint8_t minute) {
    if (day > 6 || hour >= 24 || minute >= 60) {
        debugPrint("Invalid day/time parameters");
        return ErrorCode::INVALID_PARAMETER;
    }
    
    uint8_t params[3];
    params[0] = day;
    params[1] = hour;
    params[2] = minute;
    
    debugPrint("Setting day", day);
    debugPrint("Setting time", hour * 100 + minute);
    
    return sendCommand(OIOpcode::SET_DAY_TIME, params, 3);
}

// ============================================================================
// UTILITY METHODS
// ============================================================================

bool RoombaCommands::isValidVelocity(int16_t velocity) {
    return velocity >= DriveVelocity::MAX_BACKWARD && velocity <= DriveVelocity::MAX_FORWARD;
}

bool RoombaCommands::isValidRadius(int16_t radius) {
    return (radius >= -2000 && radius <= 2000) || 
           radius == DriveRadius::STRAIGHT || 
           radius == DriveRadius::TURN_IN_PLACE_CW || 
           radius == DriveRadius::TURN_IN_PLACE_CCW;
}

bool RoombaCommands::isValidPWM(int16_t pwm) {
    return pwm >= -255 && pwm <= 255;
}

int16_t RoombaCommands::clampVelocity(int16_t velocity) {
    if (velocity > DriveVelocity::MAX_FORWARD) {
        return DriveVelocity::MAX_FORWARD;
    }
    if (velocity < DriveVelocity::MAX_BACKWARD) {
        return DriveVelocity::MAX_BACKWARD;
    }
    return velocity;
}

int16_t RoombaCommands::clampPWM(int16_t pwm) {
    if (pwm > 255) return 255;
    if (pwm < -255) return -255;
    return pwm;
}

// ============================================================================
// DEBUGGING AND DIAGNOSTICS
// ============================================================================

void RoombaCommands::getStatistics(uint32_t& commandsSent, uint16_t& errors) const {
    commandsSent = _commandsSent;
    errors = _errorCount;
}

void RoombaCommands::resetStatistics() {
    _commandsSent = 0;
    _errorCount = 0;
}

// ============================================================================
// INTERNAL METHODS
// ============================================================================

ErrorCode RoombaCommands::sendCommand(OIOpcode opcode) {
    ErrorCode result = _core.sendCommand(opcode);
    _lastCommand = opcode;
    _commandsSent++;
    
    if (result != ErrorCode::SUCCESS) {
        _errorCount++;
    }
    
    return result;
}

ErrorCode RoombaCommands::sendCommand(OIOpcode opcode, uint8_t param) {
    ErrorCode result = _core.sendCommand(opcode, param);
    _lastCommand = opcode;
    _commandsSent++;
    
    if (result != ErrorCode::SUCCESS) {
        _errorCount++;
    }
    
    return result;
}

ErrorCode RoombaCommands::sendCommand(OIOpcode opcode, const uint8_t* params, uint8_t paramCount) {
    ErrorCode result = _core.sendCommand(opcode, params, paramCount);
    _lastCommand = opcode;
    _commandsSent++;
    
    if (result != ErrorCode::SUCCESS) {
        _errorCount++;
    }
    
    return result;
}

void RoombaCommands::debugPrint(const char* message) {
    if (_debugEnabled && message != nullptr) {
        Serial.print("ArduRoombaCommands: ");
        Serial.println(message);
    }
}

void RoombaCommands::debugPrint(const char* message, int value) {
    if (_debugEnabled && message != nullptr) {
        Serial.print("ArduRoombaCommands: ");
        Serial.print(message);
        Serial.print(" = ");
        Serial.println(value);
    }
}

} // namespace ArduRoomba