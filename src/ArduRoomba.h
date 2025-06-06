/**
 * @file ArduRoomba.h
 * @brief Main header file for the ArduRoomba library
 * 
 * This is the main header file that provides a unified interface to the
 * ArduRoomba library. It includes all necessary components and provides
 * both the new modular interface and backward compatibility with the
 * original API.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.3.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBA_H
#define ARDUROOMBA_H

// Include all modular components
#include "ArduRoombaConstants.h"
#include "ArduRoombaTypes.h"
#include "ArduRoombaCore.h"
#include "ArduRoombaSensors.h"
#include "ArduRoombaCommands.h"

namespace ArduRoomba {

/**
 * @brief Main ArduRoomba class providing unified interface
 * 
 * This class provides a unified interface to all ArduRoomba functionality
 * while maintaining backward compatibility with the original API. It
 * combines the modular components (Core, Sensors, Commands) into a
 * single easy-to-use interface.
 * 
 * @example BasicUsage.ino
 * @example SensorDataExample.ino
 * @example StreamUsage.ino
 */
class ArduRoomba {
public:
    // ========================================================================
    // CONSTRUCTOR AND DESTRUCTOR
    // ========================================================================
    
    /**
     * @brief Constructor for ArduRoomba
     * @param rxPin Arduino pin connected to Roomba TX
     * @param txPin Arduino pin connected to Roomba RX  
     * @param brcPin Arduino pin connected to Roomba BRC (Baud Rate Change)
     */
    ArduRoomba(uint8_t rxPin, uint8_t txPin, uint8_t brcPin);
    
    /**
     * @brief Destructor
     */
    ~ArduRoomba();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /**
     * @brief Initialize and setup the Roomba connection
     * 
     * This method performs the complete initialization sequence including
     * hardware setup, communication establishment, and mode configuration.
     * This replaces the old roombaSetup() method with better error handling.
     * 
     * @param baudRate Serial communication baud rate (default: 19200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode initialize(uint32_t baudRate = DEFAULT_BAUD_RATE);
    
    /**
     * @brief Legacy setup method for backward compatibility
     * 
     * @deprecated Use initialize() instead for better error handling
     */
    void roombaSetup();
    
    /**
     * @brief Check if the Roomba is properly initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const;
    
    // ========================================================================
    // MODULAR COMPONENT ACCESS
    // ========================================================================
    
    /**
     * @brief Get reference to the core communication component
     * @return Reference to RoombaCore instance
     */
    RoombaCore& getCore() { return _core; }
    
    /**
     * @brief Get reference to the sensor management component
     * @return Reference to RoombaSensors instance
     */
    RoombaSensors& getSensors() { return _sensors; }
    
    /**
     * @brief Get reference to the command interface component
     * @return Reference to RoombaCommands instance
     */
    RoombaCommands& getCommands() { return _commands; }
    
    // ========================================================================
    // OPEN INTERFACE COMMANDS (Legacy compatibility)
    // ========================================================================
    
    void start();                                      ///< Start the OI
    void baud(char baudCode);                          ///< Set the baud rate
    void safe();                                       ///< Put the OI into Safe mode
    void full();                                       ///< Put the OI into Full mode
    void clean();                                      ///< Start the cleaning mode
    void maxClean();                                   ///< Start the maximum time cleaning mode
    void spot();                                       ///< Start the spot cleaning mode
    void seekDock();                                   ///< Send the robot to the dock
    void schedule(ScheduleStore scheduleData);         ///< Set the schedule
    void setDayTime(char day, char hour, char minute); ///< Set the day and time
    void power();                                      ///< Power down the OI
    
    // ========================================================================
    // ACTUATOR COMMANDS (Legacy compatibility)
    // ========================================================================
    
    void drive(int velocity, int radius);                                         ///< Drive the robot
    void driveDirect(int rightVelocity, int leftVelocity);                        ///< Drive the robot directly
    void drivePWM(int rightPWM, int leftPWM);                                     ///< Drive the robot with PWM
    void motors(byte data);                                                       ///< Control the motors
    void pwmMotors(char mainBrushPWM, char sideBrushPWM, char vacuumPWM);         ///< Control the PWM of the motors
    void leds(int ledBits, int powerColor, int powerIntensity);                   ///< Control the LEDs
    void schedulingLeds(int weekDayLedBits, int scheduleLedBits);                 ///< Control the scheduling LEDs
    void digitLedsRaw(int digitThree, int digitTwo, int digitOne, int digitZero); ///< Control the digit LEDs
    void song(Song songData);                                                     ///< Load a song
    void play(int songNumber);                                                    ///< Play a song
    
    // ========================================================================
    // INPUT COMMANDS (Legacy compatibility)
    // ========================================================================
    
    void sensors(char packetID);                      ///< Request a sensor packet
    void queryList(byte numPackets, byte *packetIDs); ///< Request a list of sensor packets
    void queryStream(char sensorlist[]);              ///< Request a list of sensor packets to stream
    void resetStream();                               ///< Request an empty list of sensor packets to stream
    bool refreshData(RoombaInfos *infos);             ///< Read stream slot
    
    // ========================================================================
    // CUSTOM MOVEMENT COMMANDS (Legacy compatibility)
    // ========================================================================
    
    void goForward();   ///< Move the Roomba forward
    void goBackward();  ///< Move the Roomba backward
    void turnLeft();    ///< Turn the Roomba left
    void turnRight();   ///< Turn the Roomba right
    void halt();        ///< Stop the Roomba
    
    // ========================================================================
    // ENHANCED MOVEMENT METHODS
    // ========================================================================
    
    /**
     * @brief Move forward with specified velocity
     * @param velocity Velocity in mm/s (default: 200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode moveForward(int16_t velocity = 200);
    
    /**
     * @brief Move backward with specified velocity
     * @param velocity Velocity in mm/s (default: 200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode moveBackward(int16_t velocity = 200);
    
    /**
     * @brief Turn left in place with specified velocity
     * @param velocity Velocity in mm/s (default: 200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode turnLeftInPlace(int16_t velocity = 200);
    
    /**
     * @brief Turn right in place with specified velocity
     * @param velocity Velocity in mm/s (default: 200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode turnRightInPlace(int16_t velocity = 200);
    
    /**
     * @brief Stop the robot
     * @return ErrorCode indicating success or failure
     */
    ErrorCode stopMovement();
    
    // ========================================================================
    // ENHANCED SENSOR METHODS
    // ========================================================================
    
    /**
     * @brief Get all sensor data
     * @param sensorData Reference to SensorData structure to populate
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getAllSensorData(SensorData& sensorData);
    
    /**
     * @brief Get basic sensor data (bumpers, cliffs, wall, battery)
     * @param sensorData Reference to SensorData structure to populate
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getBasicSensorData(SensorData& sensorData);
    
    /**
     * @brief Start streaming sensor data with preset
     * @param preset Predefined sensor set to stream
     * @return ErrorCode indicating success or failure
     */
    ErrorCode startSensorStream(RoombaSensors::SensorPreset preset);
    
    /**
     * @brief Update sensor data from stream
     * @param sensorData Reference to SensorData structure to update
     * @return ErrorCode indicating success or failure
     */
    ErrorCode updateSensorData(SensorData& sensorData);
    
    /**
     * @brief Stop sensor streaming
     * @return ErrorCode indicating success or failure
     */
    ErrorCode stopSensorStream();
    
    // ========================================================================
    // ENHANCED LED AND SOUND METHODS
    // ========================================================================
    
    /**
     * @brief Set power LED color and intensity
     * @param color Color value (0=green, 255=red)
     * @param intensity Intensity value (0-255)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setPowerLED(uint8_t color, uint8_t intensity);
    
    /**
     * @brief Create and play a simple beep
     * @param frequency MIDI note number (31-127)
     * @param duration Duration in 1/64th seconds
     * @return ErrorCode indicating success or failure
     */
    ErrorCode beep(uint8_t frequency = 72, uint8_t duration = 32);
    
    /**
     * @brief Create and play a sequence of beeps
     * @param count Number of beeps
     * @param frequency MIDI note number
     * @param duration Duration of each beep
     * @return ErrorCode indicating success or failure
     */
    ErrorCode beepSequence(uint8_t count, uint8_t frequency = 72, uint8_t duration = 16);
    
    // ========================================================================
    // UTILITY AND DIAGNOSTIC METHODS
    // ========================================================================
    
    /**
     * @brief Enable or disable debug output for all components
     * @param enabled true to enable debug output, false to disable
     */
    void setDebugEnabled(bool enabled);
    
    /**
     * @brief Check if debug output is enabled
     * @return true if debug is enabled, false otherwise
     */
    bool isDebugEnabled() const;
    
    /**
     * @brief Get the last error code from any component
     * @return Last error code encountered
     */
    ErrorCode getLastError() const;
    
    /**
     * @brief Get comprehensive statistics from all components
     * @param bytesSent Reference to store total bytes sent
     * @param bytesReceived Reference to store total bytes received
     * @param commandsSent Reference to store total commands sent
     * @param errors Reference to store total errors
     */
    void getStatistics(uint32_t& bytesSent, uint32_t& bytesReceived, 
                      uint32_t& commandsSent, uint16_t& errors) const;
    
    /**
     * @brief Reset all statistics
     */
    void resetStatistics();
    
    /**
     * @brief Print current sensor data to Serial (for debugging)
     * @param sensorData Sensor data to print
     */
    void printSensorData(const SensorData& sensorData);
    
    // ========================================================================
    // LEGACY TYPE DEFINITIONS (for backward compatibility)
    // ========================================================================
    
    typedef Note Note;                    ///< Legacy Note type
    typedef Song Song;                    ///< Legacy Song type
    typedef SensorData RoombaInfos;       ///< Legacy RoombaInfos type
    typedef ScheduleData ScheduleStore;   ///< Legacy ScheduleStore type
    
private:
    // ========================================================================
    // MEMBER VARIABLES
    // ========================================================================
    
    RoombaCore _core;           ///< Core communication component
    RoombaSensors _sensors;     ///< Sensor management component
    RoombaCommands _commands;   ///< Command interface component
    
    bool _debugEnabled;         ///< Global debug flag
    ErrorCode _lastError;       ///< Last error from any component
    
    // Legacy compatibility members
    const byte _zero = 0x00;    ///< Zero byte constant for legacy methods
    
    // ========================================================================
    // INTERNAL HELPER METHODS
    // ========================================================================
    
    /**
     * @brief Update last error from component operations
     * @param error Error code to set
     */
    void updateLastError(ErrorCode error);
    
    /**
     * @brief Convert legacy sensor list to new format
     * @param legacySensorList Legacy char array sensor list
     * @return Number of sensors converted
     */
    uint8_t convertLegacySensorList(char legacySensorList[], SensorPacket* newSensorList, uint8_t maxSensors);
    
    /**
     * @brief Get length of legacy sensor list
     * @param sensorList Legacy sensor list
     * @return Length of the list
     */
    int getLegacySensorListLength(char sensorList[]);
};

} // namespace ArduRoomba

// ============================================================================
// GLOBAL CONVENIENCE TYPEDEF (for backward compatibility)
// ============================================================================

/**
 * @brief Global typedef for backward compatibility
 * 
 * This allows existing code to continue using "ArduRoomba" without
 * the namespace qualifier.
 */
typedef ArduRoomba::ArduRoomba ArduRoomba;

#endif // ARDUROOMBA_H
