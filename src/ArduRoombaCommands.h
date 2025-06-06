/**
 * @file ArduRoombaCommands.h
 * @brief High-level command interface for the ArduRoomba library
 * 
 * This file contains the command management class that provides a high-level
 * interface for controlling the Roomba. It includes movement commands, actuator
 * control, LED control, song management, and scheduling functionality.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.3.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBA_COMMANDS_H
#define ARDUROOMBA_COMMANDS_H

#include <Arduino.h>
#include "ArduRoombaConstants.h"
#include "ArduRoombaTypes.h"
#include "ArduRoombaCore.h"

namespace ArduRoomba {

/**
 * @brief High-level command interface for iRobot Open Interface
 * 
 * This class provides convenient methods for controlling all aspects of
 * the Roomba, including movement, cleaning modes, actuators, LEDs, songs,
 * and scheduling. It builds on the low-level core communication to provide
 * a user-friendly interface.
 */
class RoombaCommands {
public:
    /**
     * @brief Constructor for RoombaCommands
     * @param core Reference to the RoombaCore instance
     */
    explicit RoombaCommands(RoombaCore& core);
    
    /**
     * @brief Destructor
     */
    ~RoombaCommands();
    
    // ========================================================================
    // OPEN INTERFACE MODE CONTROL
    // ========================================================================
    
    /**
     * @brief Start the Open Interface
     * @return ErrorCode indicating success or failure
     */
    ErrorCode start();
    
    /**
     * @brief Put the OI into Safe mode
     * @return ErrorCode indicating success or failure
     */
    ErrorCode safeMode();
    
    /**
     * @brief Put the OI into Full mode
     * @return ErrorCode indicating success or failure
     */
    ErrorCode fullMode();
    
    /**
     * @brief Power down the OI
     * @return ErrorCode indicating success or failure
     */
    ErrorCode powerDown();
    
    /**
     * @brief Change the baud rate
     * @param baudRate New baud rate (300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode changeBaudRate(uint32_t baudRate);
    
    // ========================================================================
    // CLEANING MODES
    // ========================================================================
    
    /**
     * @brief Start the default cleaning mode
     * @return ErrorCode indicating success or failure
     */
    ErrorCode startCleaning();
    
    /**
     * @brief Start maximum time cleaning mode
     * @return ErrorCode indicating success or failure
     */
    ErrorCode startMaxCleaning();
    
    /**
     * @brief Start spot cleaning mode
     * @return ErrorCode indicating success or failure
     */
    ErrorCode startSpotCleaning();
    
    /**
     * @brief Send the robot to seek the dock
     * @return ErrorCode indicating success or failure
     */
    ErrorCode seekDock();
    
    // ========================================================================
    // MOVEMENT CONTROL
    // ========================================================================
    
    /**
     * @brief Drive the robot with velocity and radius
     * @param velocity Velocity in mm/s (-500 to 500)
     * @param radius Turning radius in mm (-2000 to 2000, special values: 32768=straight, 1=CCW, -1=CW)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode drive(int16_t velocity, int16_t radius);
    
    /**
     * @brief Drive the robot with independent wheel velocities
     * @param rightVelocity Right wheel velocity in mm/s (-500 to 500)
     * @param leftVelocity Left wheel velocity in mm/s (-500 to 500)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode driveDirect(int16_t rightVelocity, int16_t leftVelocity);
    
    /**
     * @brief Drive the robot with PWM values
     * @param rightPWM Right wheel PWM (-255 to 255)
     * @param leftPWM Left wheel PWM (-255 to 255)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode drivePWM(int16_t rightPWM, int16_t leftPWM);
    
    /**
     * @brief Stop the robot immediately
     * @return ErrorCode indicating success or failure
     */
    ErrorCode stop();
    
    // ========================================================================
    // CONVENIENT MOVEMENT METHODS
    // ========================================================================
    
    /**
     * @brief Move the robot forward at default speed
     * @param velocity Velocity in mm/s (default: 200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode moveForward(int16_t velocity = 200);
    
    /**
     * @brief Move the robot backward at default speed
     * @param velocity Velocity in mm/s (default: 200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode moveBackward(int16_t velocity = 200);
    
    /**
     * @brief Turn the robot left in place
     * @param velocity Velocity in mm/s (default: 200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode turnLeft(int16_t velocity = 200);
    
    /**
     * @brief Turn the robot right in place
     * @param velocity Velocity in mm/s (default: 200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode turnRight(int16_t velocity = 200);
    
    /**
     * @brief Turn the robot left with specified radius
     * @param velocity Velocity in mm/s
     * @param radius Turning radius in mm
     * @return ErrorCode indicating success or failure
     */
    ErrorCode turnLeftRadius(int16_t velocity, int16_t radius);
    
    /**
     * @brief Turn the robot right with specified radius
     * @param velocity Velocity in mm/s
     * @param radius Turning radius in mm
     * @return ErrorCode indicating success or failure
     */
    ErrorCode turnRightRadius(int16_t velocity, int16_t radius);
    
    // ========================================================================
    // ACTUATOR CONTROL
    // ========================================================================
    
    /**
     * @brief Control the cleaning motors
     * @param sideBrush Enable side brush
     * @param vacuum Enable vacuum (not available on 600 series)
     * @param mainBrush Enable main brush
     * @param sideBrushDirection Side brush direction (false=inward, true=outward)
     * @param mainBrushDirection Main brush direction (false=inward, true=outward)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setMotors(bool sideBrush, bool vacuum, bool mainBrush, 
                       bool sideBrushDirection = false, bool mainBrushDirection = false);
    
    /**
     * @brief Control motor PWM values
     * @param mainBrushPWM Main brush PWM (-127 to 127)
     * @param sideBrushPWM Side brush PWM (-127 to 127)
     * @param vacuumPWM Vacuum PWM (-127 to 127)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setMotorsPWM(int8_t mainBrushPWM, int8_t sideBrushPWM, int8_t vacuumPWM);
    
    // ========================================================================
    // LED CONTROL
    // ========================================================================
    
    /**
     * @brief Control the status LEDs
     * @param checkRobot Check robot LED state
     * @param dock Dock LED state
     * @param spot Spot LED state
     * @param debris Debris LED state
     * @param powerColor Power LED color (0=green, 255=red)
     * @param powerIntensity Power LED intensity (0-255)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setLEDs(bool checkRobot, bool dock, bool spot, bool debris, 
                     uint8_t powerColor = 0, uint8_t powerIntensity = 255);
    
    /**
     * @brief Set power LED color and intensity
     * @param color Color value (0=green, 255=red)
     * @param intensity Intensity value (0-255)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setPowerLED(uint8_t color, uint8_t intensity);
    
    /**
     * @brief Control the scheduling LEDs
     * @param weekdayLEDs Weekday LED bitmask
     * @param scheduleLEDs Schedule LED bitmask
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setSchedulingLEDs(uint8_t weekdayLEDs, uint8_t scheduleLEDs);
    
    /**
     * @brief Control the 7-segment display digits
     * @param digit3 Leftmost digit (0-9, or special characters)
     * @param digit2 Second digit
     * @param digit1 Third digit
     * @param digit0 Rightmost digit
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setDigitLEDs(uint8_t digit3, uint8_t digit2, uint8_t digit1, uint8_t digit0);
    
    // ========================================================================
    // SONG MANAGEMENT
    // ========================================================================
    
    /**
     * @brief Define a song
     * @param song Song structure containing notes and metadata
     * @return ErrorCode indicating success or failure
     */
    ErrorCode defineSong(const Song& song);
    
    /**
     * @brief Play a previously defined song
     * @param songNumber Song number to play (0-4)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode playSong(uint8_t songNumber);
    
    /**
     * @brief Create and play a simple beep
     * @param frequency MIDI note number (31-127)
     * @param duration Duration in 1/64th seconds
     * @param songSlot Song slot to use (0-4)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode beep(uint8_t frequency = 72, uint8_t duration = 32, uint8_t songSlot = 0);
    
    /**
     * @brief Create and play a sequence of beeps
     * @param count Number of beeps
     * @param frequency MIDI note number
     * @param duration Duration of each beep
     * @param pause Duration of pause between beeps
     * @param songSlot Song slot to use (0-4)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode beepSequence(uint8_t count, uint8_t frequency = 72, uint8_t duration = 16, 
                          uint8_t pause = 16, uint8_t songSlot = 0);
    
    // ========================================================================
    // SCHEDULING
    // ========================================================================
    
    /**
     * @brief Set the cleaning schedule
     * @param schedule Schedule data structure
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setSchedule(const ScheduleData& schedule);
    
    /**
     * @brief Set the current day and time
     * @param day Day of week (0=Sunday, 6=Saturday)
     * @param hour Hour (0-23)
     * @param minute Minute (0-59)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setDayTime(uint8_t day, uint8_t hour, uint8_t minute);
    
    // ========================================================================
    // UTILITY METHODS
    // ========================================================================
    
    /**
     * @brief Validate velocity parameter
     * @param velocity Velocity to validate
     * @return true if velocity is valid, false otherwise
     */
    static bool isValidVelocity(int16_t velocity);
    
    /**
     * @brief Validate radius parameter
     * @param radius Radius to validate
     * @return true if radius is valid, false otherwise
     */
    static bool isValidRadius(int16_t radius);
    
    /**
     * @brief Validate PWM parameter
     * @param pwm PWM value to validate
     * @return true if PWM is valid, false otherwise
     */
    static bool isValidPWM(int16_t pwm);
    
    /**
     * @brief Clamp velocity to valid range
     * @param velocity Velocity to clamp
     * @return Clamped velocity
     */
    static int16_t clampVelocity(int16_t velocity);
    
    /**
     * @brief Clamp PWM to valid range
     * @param pwm PWM value to clamp
     * @return Clamped PWM value
     */
    static int16_t clampPWM(int16_t pwm);
    
    // ========================================================================
    // DEBUGGING AND DIAGNOSTICS
    // ========================================================================
    
    /**
     * @brief Enable or disable debug output
     * @param enabled true to enable debug output, false to disable
     */
    void setDebugEnabled(bool enabled) { _debugEnabled = enabled; }
    
    /**
     * @brief Check if debug output is enabled
     * @return true if debug is enabled, false otherwise
     */
    bool isDebugEnabled() const { return _debugEnabled; }
    
    /**
     * @brief Get the last command sent
     * @return Last command opcode
     */
    OIOpcode getLastCommand() const { return _lastCommand; }
    
    /**
     * @brief Get command execution statistics
     * @param commandsSent Reference to store commands sent count
     * @param errors Reference to store error count
     */
    void getStatistics(uint32_t& commandsSent, uint16_t& errors) const;
    
    /**
     * @brief Reset command statistics
     */
    void resetStatistics();
    
protected:
    // ========================================================================
    // INTERNAL METHODS
    // ========================================================================
    
    /**
     * @brief Internal method to send command and update statistics
     * @param opcode Command opcode
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendCommand(OIOpcode opcode);
    
    /**
     * @brief Internal method to send command with parameter
     * @param opcode Command opcode
     * @param param Parameter value
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendCommand(OIOpcode opcode, uint8_t param);
    
    /**
     * @brief Internal method to send command with multiple parameters
     * @param opcode Command opcode
     * @param params Parameter array
     * @param paramCount Number of parameters
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendCommand(OIOpcode opcode, const uint8_t* params, uint8_t paramCount);
    
    /**
     * @brief Internal debug print method
     * @param message Message to print (if debug is enabled)
     */
    void debugPrint(const char* message);
    
    /**
     * @brief Internal debug print method with value
     * @param message Message to print
     * @param value Value to print
     */
    void debugPrint(const char* message, int value);
    
private:
    // ========================================================================
    // MEMBER VARIABLES
    // ========================================================================
    
    RoombaCore& _core;              ///< Reference to core communication object
    bool _debugEnabled;             ///< Debug output enabled flag
    OIOpcode _lastCommand;          ///< Last command sent
    
    // Statistics
    uint32_t _commandsSent;         ///< Total commands sent
    uint16_t _errorCount;           ///< Total errors encountered
};

} // namespace ArduRoomba

#endif // ARDUROOMBA_COMMANDS_H