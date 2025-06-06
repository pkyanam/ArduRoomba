/**
 * @file ArduRoombaTypes.h
 * @brief Data structures and type definitions for the ArduRoomba library
 * 
 * This file contains all the data structures, type definitions, and classes
 * used throughout the ArduRoomba library. It provides a centralized location
 * for all data types and ensures consistency across the library.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.3.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBA_TYPES_H
#define ARDUROOMBA_TYPES_H

#include <Arduino.h>
#include "ArduRoombaConstants.h"

namespace ArduRoomba {

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

class RoombaCore;
class RoombaSensors;
class RoombaCommands;

// ============================================================================
// MUSICAL NOTE STRUCTURE
// ============================================================================

/**
 * @brief Represents a musical note for Roomba songs
 * 
 * This structure defines a single musical note that can be played
 * by the Roomba's speaker. Notes are defined using MIDI note numbers
 * and durations in 1/64th second increments.
 */
struct Note {
    uint8_t noteNumber;   ///< MIDI note number (31-127)
    uint8_t noteDuration; ///< Duration in 1/64th seconds (1-255)
    
    /**
     * @brief Default constructor
     */
    Note() : noteNumber(60), noteDuration(32) {}
    
    /**
     * @brief Parameterized constructor
     * @param note MIDI note number (31-127)
     * @param duration Note duration in 1/64th seconds (1-255)
     */
    Note(uint8_t note, uint8_t duration) : noteNumber(note), noteDuration(duration) {}
    
    /**
     * @brief Validates the note parameters
     * @return true if note is valid, false otherwise
     */
    bool isValid() const {
        return (noteNumber >= Song::MIN_NOTE && noteNumber <= Song::MAX_NOTE) &&
               (noteDuration > 0 && noteDuration <= Song::MAX_DURATION);
    }
};

// ============================================================================
// SONG STRUCTURE
// ============================================================================

/**
 * @brief Represents a complete song for the Roomba
 * 
 * This structure defines a complete song that can be stored and played
 * by the Roomba. Each song can contain up to 16 notes and is identified
 * by a song number (0-4).
 */
struct Song {
    uint8_t songNumber; ///< Song identifier (0-4)
    uint8_t songLength; ///< Number of notes in the song (1-16)
    Note notes[Song::MAX_NOTES]; ///< Array of notes in the song
    
    /**
     * @brief Default constructor
     */
    Song() : songNumber(0), songLength(0) {}
    
    /**
     * @brief Parameterized constructor
     * @param number Song number (0-4)
     * @param length Number of notes (1-16)
     */
    Song(uint8_t number, uint8_t length) : songNumber(number), songLength(length) {}
    
    /**
     * @brief Validates the song parameters
     * @return true if song is valid, false otherwise
     */
    bool isValid() const {
        if (songNumber > Song::MAX_SONGS || songLength == 0 || songLength > Song::MAX_NOTES) {
            return false;
        }
        
        for (uint8_t i = 0; i < songLength; i++) {
            if (!notes[i].isValid()) {
                return false;
            }
        }
        return true;
    }
    
    /**
     * @brief Adds a note to the song
     * @param note The note to add
     * @return true if note was added successfully, false if song is full
     */
    bool addNote(const Note& note) {
        if (songLength >= Song::MAX_NOTES || !note.isValid()) {
            return false;
        }
        notes[songLength++] = note;
        return true;
    }
    
    /**
     * @brief Clears all notes from the song
     */
    void clear() {
        songLength = 0;
    }
};

// ============================================================================
// SCHEDULE STRUCTURE
// ============================================================================

/**
 * @brief Represents the cleaning schedule for the Roomba
 * 
 * This structure defines the weekly cleaning schedule, including
 * which days are enabled and the start times for each day.
 */
struct ScheduleData {
    uint8_t enabledDays;    ///< Bitmask of enabled days (bit 0=Sunday, bit 6=Saturday)
    uint8_t sunHour;        ///< Sunday start hour (0-23)
    uint8_t sunMinute;      ///< Sunday start minute (0-59)
    uint8_t monHour;        ///< Monday start hour (0-23)
    uint8_t monMinute;      ///< Monday start minute (0-59)
    uint8_t tueHour;        ///< Tuesday start hour (0-23)
    uint8_t tueMinute;      ///< Tuesday start minute (0-59)
    uint8_t wedHour;        ///< Wednesday start hour (0-23)
    uint8_t wedMinute;      ///< Wednesday start minute (0-59)
    uint8_t thuHour;        ///< Thursday start hour (0-23)
    uint8_t thuMinute;      ///< Thursday start minute (0-59)
    uint8_t friHour;        ///< Friday start hour (0-23)
    uint8_t friMinute;      ///< Friday start minute (0-59)
    uint8_t satHour;        ///< Saturday start hour (0-23)
    uint8_t satMinute;      ///< Saturday start minute (0-59)
    
    /**
     * @brief Default constructor - all days disabled
     */
    ScheduleData() : enabledDays(0), sunHour(0), sunMinute(0), monHour(0), monMinute(0),
                     tueHour(0), tueMinute(0), wedHour(0), wedMinute(0),
                     thuHour(0), thuMinute(0), friHour(0), friMinute(0),
                     satHour(0), satMinute(0) {}
    
    /**
     * @brief Validates the schedule data
     * @return true if schedule is valid, false otherwise
     */
    bool isValid() const {
        return (sunHour < 24 && sunMinute < 60) &&
               (monHour < 24 && monMinute < 60) &&
               (tueHour < 24 && tueMinute < 60) &&
               (wedHour < 24 && wedMinute < 60) &&
               (thuHour < 24 && thuMinute < 60) &&
               (friHour < 24 && friMinute < 60) &&
               (satHour < 24 && satMinute < 60);
    }
    
    /**
     * @brief Sets the time for a specific day
     * @param day Day of week (0=Sunday, 6=Saturday)
     * @param hour Hour (0-23)
     * @param minute Minute (0-59)
     * @return true if time was set successfully, false for invalid parameters
     */
    bool setDayTime(uint8_t day, uint8_t hour, uint8_t minute) {
        if (day > 6 || hour >= 24 || minute >= 60) {
            return false;
        }
        
        switch (day) {
            case 0: sunHour = hour; sunMinute = minute; break;
            case 1: monHour = hour; monMinute = minute; break;
            case 2: tueHour = hour; tueMinute = minute; break;
            case 3: wedHour = hour; wedMinute = minute; break;
            case 4: thuHour = hour; thuMinute = minute; break;
            case 5: friHour = hour; friMinute = minute; break;
            case 6: satHour = hour; satMinute = minute; break;
        }
        return true;
    }
    
    /**
     * @brief Enables or disables a specific day
     * @param day Day of week (0=Sunday, 6=Saturday)
     * @param enabled true to enable, false to disable
     */
    void setDayEnabled(uint8_t day, bool enabled) {
        if (day > 6) return;
        
        if (enabled) {
            enabledDays |= (1 << day);
        } else {
            enabledDays &= ~(1 << day);
        }
    }
    
    /**
     * @brief Checks if a specific day is enabled
     * @param day Day of week (0=Sunday, 6=Saturday)
     * @return true if day is enabled, false otherwise
     */
    bool isDayEnabled(uint8_t day) const {
        if (day > 6) return false;
        return (enabledDays & (1 << day)) != 0;
    }
};

// ============================================================================
// SENSOR DATA STRUCTURE
// ============================================================================

/**
 * @brief Comprehensive sensor data structure for the Roomba
 * 
 * This structure contains all sensor data that can be retrieved from
 * the Roomba, including timing information for data freshness tracking.
 */
struct SensorData {
    // Timing and status information
    unsigned long nextRefresh;       ///< Time of next scheduled update (ms)
    unsigned long lastSuccessfulRefresh; ///< Time of last successful update (ms)
    uint16_t failedAttempts;         ///< Number of consecutive failed attempts
    
    // Basic sensor data (single byte values)
    uint8_t irOpcode;                ///< Infrared character omni
    uint8_t songNumber;              ///< Current song number
    uint8_t ioStreamNumPackets;      ///< Number of stream packets
    OIMode mode;                     ///< Current Open Interface mode
    ChargingState chargingState;     ///< Battery charging state
    uint8_t infraredCharacterLeft;   ///< Infrared character left
    uint8_t infraredCharacterRight;  ///< Infrared character right
    int8_t temperature;              ///< Battery temperature (°C)
    
    // Two-byte sensor values
    uint16_t voltage;                ///< Battery voltage (mV)
    int16_t current;                 ///< Battery current (mA)
    uint16_t batteryCapacity;        ///< Battery capacity (mAh)
    uint16_t batteryCharge;          ///< Current battery charge (mAh)
    uint8_t dirtDetect;              ///< Dirt detect level (0-255)
    int16_t velocity;                ///< Current velocity (mm/s)
    int16_t rightVelocity;           ///< Right wheel velocity (mm/s)
    int16_t leftVelocity;            ///< Left wheel velocity (mm/s)
    int16_t radius;                  ///< Current turning radius (mm)
    uint16_t leftEncoderCounts;      ///< Left encoder counts
    uint16_t rightEncoderCounts;     ///< Right encoder counts
    int16_t leftMotorCurrent;        ///< Left motor current (mA)
    int16_t rightMotorCurrent;       ///< Right motor current (mA)
    int16_t mainBrushMotorCurrent;   ///< Main brush motor current (mA)
    int16_t sideBrushMotorCurrent;   ///< Side brush motor current (mA)
    
    // Signal strength values
    uint16_t wallSignal;             ///< Wall sensor signal strength
    uint16_t cliffLeftSignal;        ///< Cliff left signal strength
    uint16_t cliffFrontLeftSignal;   ///< Cliff front left signal strength
    uint16_t cliffRightSignal;       ///< Cliff right signal strength
    uint16_t cliffFrontRightSignal;  ///< Cliff front right signal strength
    uint16_t lightBumpLeftSignal;    ///< Light bump left signal strength
    uint16_t lightBumpFrontLeftSignal;   ///< Light bump front left signal strength
    uint16_t lightBumpCenterLeftSignal;  ///< Light bump center left signal strength
    uint16_t lightBumpCenterRightSignal; ///< Light bump center right signal strength
    uint16_t lightBumpFrontRightSignal;  ///< Light bump front right signal strength
    uint16_t lightBumpRightSignal;       ///< Light bump right signal strength
    
    // Boolean sensor flags
    bool wall;                       ///< Wall sensor detected
    bool virtualWall;                ///< Virtual wall detected
    bool cliffLeft;                  ///< Cliff detected on left
    bool cliffFrontLeft;             ///< Cliff detected on front left
    bool cliffRight;                 ///< Cliff detected on right
    bool cliffFrontRight;            ///< Cliff detected on front right
    bool songPlaying;                ///< Song currently playing
    
    // Light bumper sensors
    bool lightBumperLeft;            ///< Light bumper left activated
    bool lightBumperFrontLeft;       ///< Light bumper front left activated
    bool lightBumperCenterLeft;      ///< Light bumper center left activated
    bool lightBumperCenterRight;     ///< Light bumper center right activated
    bool lightBumperFrontRight;      ///< Light bumper front right activated
    bool lightBumperRight;           ///< Light bumper right activated
    
    // Charger availability
    bool internalChargerAvailable;   ///< Internal charger available
    bool homeBaseChargerAvailable;   ///< Home base charger available
    
    // Stasis flags
    bool stasisDisabled;             ///< Stasis disabled
    bool stasisToggling;             ///< Stasis toggling
    
    // Button states
    bool cleanButton;                ///< Clean button pressed
    bool spotButton;                 ///< Spot button pressed
    bool dockButton;                 ///< Dock button pressed
    bool minuteButton;               ///< Minute button pressed
    bool hourButton;                 ///< Hour button pressed
    bool dayButton;                  ///< Day button pressed
    bool scheduleButton;             ///< Schedule button pressed (fixed typo)
    bool clockButton;                ///< Clock button pressed
    
    // Wheel and motor overcurrent flags
    bool wheelRightOvercurrent;      ///< Right wheel overcurrent
    bool wheelLeftOvercurrent;       ///< Left wheel overcurrent
    bool mainBrushOvercurrent;       ///< Main brush overcurrent
    bool sideBrushOvercurrent;       ///< Side brush overcurrent
    bool vacuumOvercurrent;          ///< Vacuum overcurrent
    
    // Bump and wheel drop sensors
    bool bumpRight;                  ///< Right bumper activated
    bool bumpLeft;                   ///< Left bumper activated
    bool wheelDropRight;             ///< Right wheel dropped
    bool wheelDropLeft;              ///< Left wheel dropped
    
    /**
     * @brief Default constructor - initializes all values to safe defaults
     */
    SensorData() {
        reset();
    }
    
    /**
     * @brief Resets all sensor data to default values
     */
    void reset() {
        nextRefresh = 0;
        lastSuccessfulRefresh = 0;
        failedAttempts = 0;
        
        irOpcode = 0;
        songNumber = 0;
        ioStreamNumPackets = 0;
        mode = OIMode::OFF;
        chargingState = ChargingState::NOT_CHARGING;
        infraredCharacterLeft = 0;
        infraredCharacterRight = 0;
        temperature = 0;
        
        voltage = 0;
        current = 0;
        batteryCapacity = 0;
        batteryCharge = 0;
        dirtDetect = 0;
        velocity = 0;
        rightVelocity = 0;
        leftVelocity = 0;
        radius = 0;
        leftEncoderCounts = 0;
        rightEncoderCounts = 0;
        leftMotorCurrent = 0;
        rightMotorCurrent = 0;
        mainBrushMotorCurrent = 0;
        sideBrushMotorCurrent = 0;
        
        wallSignal = 0;
        cliffLeftSignal = 0;
        cliffFrontLeftSignal = 0;
        cliffRightSignal = 0;
        cliffFrontRightSignal = 0;
        lightBumpLeftSignal = 0;
        lightBumpFrontLeftSignal = 0;
        lightBumpCenterLeftSignal = 0;
        lightBumpCenterRightSignal = 0;
        lightBumpFrontRightSignal = 0;
        lightBumpRightSignal = 0;
        
        wall = false;
        virtualWall = false;
        cliffLeft = false;
        cliffFrontLeft = false;
        cliffRight = false;
        cliffFrontRight = false;
        songPlaying = false;
        
        lightBumperLeft = false;
        lightBumperFrontLeft = false;
        lightBumperCenterLeft = false;
        lightBumperCenterRight = false;
        lightBumperFrontRight = false;
        lightBumperRight = false;
        
        internalChargerAvailable = false;
        homeBaseChargerAvailable = false;
        
        stasisDisabled = false;
        stasisToggling = false;
        
        cleanButton = false;
        spotButton = false;
        dockButton = false;
        minuteButton = false;
        hourButton = false;
        dayButton = false;
        scheduleButton = false;
        clockButton = false;
        
        wheelRightOvercurrent = false;
        wheelLeftOvercurrent = false;
        mainBrushOvercurrent = false;
        sideBrushOvercurrent = false;
        vacuumOvercurrent = false;
        
        bumpRight = false;
        bumpLeft = false;
        wheelDropRight = false;
        wheelDropLeft = false;
    }
    
    /**
     * @brief Checks if sensor data is fresh (recently updated)
     * @param maxAge Maximum age in milliseconds
     * @return true if data is fresh, false if stale
     */
    bool isFresh(unsigned long maxAge = 1000) const {
        return (millis() - lastSuccessfulRefresh) < maxAge;
    }
    
    /**
     * @brief Gets the age of the sensor data in milliseconds
     * @return Age of data in milliseconds
     */
    unsigned long getAge() const {
        return millis() - lastSuccessfulRefresh;
    }
    
    /**
     * @brief Checks if any cliff sensor is triggered
     * @return true if any cliff sensor is active
     */
    bool hasCliffDetection() const {
        return cliffLeft || cliffFrontLeft || cliffRight || cliffFrontRight;
    }
    
    /**
     * @brief Checks if any bumper is triggered
     * @return true if any bumper is active
     */
    bool hasBumpDetection() const {
        return bumpLeft || bumpRight;
    }
    
    /**
     * @brief Checks if any wheel is dropped
     * @return true if any wheel is dropped
     */
    bool hasWheelDrop() const {
        return wheelDropLeft || wheelDropRight;
    }
    
    /**
     * @brief Checks if any motor has overcurrent
     * @return true if any motor has overcurrent
     */
    bool hasOvercurrent() const {
        return wheelLeftOvercurrent || wheelRightOvercurrent || 
               mainBrushOvercurrent || sideBrushOvercurrent || vacuumOvercurrent;
    }
    
    /**
     * @brief Gets battery charge percentage (0-100)
     * @return Battery charge percentage, or -1 if capacity is unknown
     */
    int getBatteryPercentage() const {
        if (batteryCapacity == 0) return -1;
        return (batteryCharge * 100) / batteryCapacity;
    }
};

// ============================================================================
// LEGACY COMPATIBILITY TYPEDEFS
// ============================================================================

// These typedefs maintain backward compatibility with existing code
typedef Note Note;
typedef Song Song;
typedef SensorData RoombaInfos;
typedef ScheduleData ScheduleStore;

} // namespace ArduRoomba

#endif // ARDUROOMBA_TYPES_H