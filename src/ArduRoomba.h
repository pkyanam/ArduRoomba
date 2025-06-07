/**
 * @file ArduRoomba.h
 * @brief Main unified interface for the ArduRoomba library
 * 
 * This is the primary header file that provides a comprehensive, unified interface
 * to the ArduRoomba library ecosystem. It orchestrates all modular components
 * (Core, Sensors, Commands, Configuration, WiFi) into a single, easy-to-use API
 * while maintaining backward compatibility with legacy code.
 * 
 * Architecture Context for AI Agents:
 * - This class serves as the main facade pattern implementation
 * - It aggregates specialized components rather than implementing functionality directly
 * - Provides both modern error-handling interface and legacy boolean interface
 * - Acts as the primary entry point for all Roomba operations
 * - Manages component lifecycle and inter-component communication
 * 
 * Usage Patterns:
 * - Instantiate once per Roomba device
 * - Call initialize() before any operations
 * - Use error codes for robust error handling
 * - Enable debug mode for troubleshooting
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBA_H
#define ARDUROOMBA_H

// ============================================================================
// SYSTEM INCLUDES
// ============================================================================
#include <Arduino.h>

// ============================================================================
// LIBRARY INCLUDES - MODULAR COMPONENTS
// ============================================================================
// Core type definitions and constants (foundation layer)
#include "ArduRoombaConstants.h"
#include "ArduRoombaTypes.h"

// Core communication layer (hardware abstraction)
#include "ArduRoombaCore.h"

// Specialized functionality layers
#include "ArduRoombaSensors.h"    // Sensor data acquisition and processing
#include "ArduRoombaCommands.h"   // Movement and control commands

// Optional modular components (conditionally included)
#include "core/ArduRoombaConfig.h"    // Configuration management system

// Platform-specific wireless support (conditionally compiled)
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)
    #include "wireless/ArduRoombaWiFi.h"  // WiFi connectivity and web interface
#endif

namespace ArduRoomba {

/**
 * @brief Main ArduRoomba facade class providing unified interface to all library functionality
 * 
 * This class implements the Facade design pattern, providing a simplified, unified interface
 * to the complex subsystem of modular ArduRoomba components. It orchestrates interactions
 * between Core (communication), Sensors (data acquisition), Commands (control), Configuration
 * (settings management), and optionally WiFi (wireless connectivity) components.
 * 
 * Architecture Design for AI Agents:
 * - **Facade Pattern**: Simplifies complex subsystem interactions
 * - **Component Aggregation**: Contains instances of specialized components
 * - **Error Propagation**: Translates component errors to unified error codes
 * - **State Management**: Maintains overall system state and component coordination
 * - **Legacy Compatibility**: Provides backward-compatible API alongside modern interface
 * 
 * Component Relationships:
 * - RoombaCore: Hardware communication and protocol management
 * - RoombaSensors: Sensor data acquisition and processing
 * - RoombaCommands: Movement and control command execution
 * - RoombaConfig: Persistent configuration and settings management
 * - RoombaWiFi: Wireless connectivity and web interface (platform-dependent)
 * 
 * Usage Lifecycle:
 * 1. Construction: Initialize with hardware pin configuration
 * 2. Initialization: Call initialize() to establish communication
 * 3. Operation: Use sensor/command methods for robot control
 * 4. Configuration: Optionally manage persistent settings
 * 5. Cleanup: Automatic resource cleanup in destructor
 * 
 * Error Handling Strategy:
 * - Modern API: Returns ErrorCode enum for detailed error information
 * - Legacy API: Returns boolean for backward compatibility
 * - Debug Mode: Provides detailed logging for troubleshooting
 * - Error State: Maintains last error for diagnostic purposes
 * 
 * @example BasicUsage.ino Basic movement and sensor reading
 * @example SensorDataExample.ino Comprehensive sensor data acquisition
 * @example StreamUsage.ino Real-time sensor data streaming
 * @example ConfigurationExample.ino Persistent settings management
 * @example WiFiBasicControl.ino Wireless control interface
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
    // CORE COMPONENT INSTANCES
    // ========================================================================
    // These are the fundamental building blocks of the ArduRoomba system.
    // Each component handles a specific aspect of robot functionality.
    
    RoombaCore _core;           ///< Hardware communication layer - manages serial protocol, baud rate, and low-level I/O
    RoombaSensors _sensors;     ///< Sensor data acquisition system - handles all sensor reading and data processing
    RoombaCommands _commands;   ///< Command execution engine - translates high-level commands to OI protocol
    
    // ========================================================================
    // SYSTEM STATE MANAGEMENT
    // ========================================================================
    // These members track the overall state of the ArduRoomba system for
    // debugging, error handling, and operational status monitoring.
    
    bool _debugEnabled;         ///< Global debug output flag - controls verbose logging across all components
    ErrorCode _lastError;       ///< Most recent error code - maintained for diagnostic and error recovery purposes
    
    // ========================================================================
    // LEGACY COMPATIBILITY SUPPORT
    // ========================================================================
    // These members support backward compatibility with the original ArduRoomba API.
    // They enable existing code to work without modification while providing
    // a migration path to the new modular architecture.
    
    const byte _zero = 0x00;    ///< Zero byte constant used by legacy protocol methods
    
    // ========================================================================
    // INTERNAL HELPER METHODS
    // ========================================================================
    // These private methods provide internal functionality for error management,
    // legacy API support, and component coordination. They are not exposed to
    // users but are critical for maintaining system integrity and compatibility.
    
    /**
     * @brief Updates the global error state from component operations
     * 
     * This method centralizes error state management across all components.
     * It ensures that the most recent error is always available for diagnostic
     * purposes and maintains consistency in error reporting.
     * 
     * AI Context: This is the central error aggregation point. All component
     * errors flow through here to maintain a unified error state.
     * 
     * @param error Error code from component operation
     */
    void updateLastError(ErrorCode error);
    
    /**
     * @brief Converts legacy char array sensor lists to modern SensorPacket format
     * 
     * This method bridges the gap between the original ArduRoomba API and the
     * new type-safe sensor system. It translates legacy sensor specifications
     * into the modern enum-based sensor packet system.
     * 
     * AI Context: This is a compatibility layer that enables legacy code to
     * work with the new sensor architecture without modification.
     * 
     * @param legacySensorList Legacy char array sensor specification
     * @param newSensorList Output array for converted sensor packets
     * @param maxSensors Maximum number of sensors to convert
     * @return Number of sensors successfully converted
     */
    uint8_t convertLegacySensorList(char legacySensorList[], SensorPacket* newSensorList, uint8_t maxSensors);
    
    /**
     * @brief Calculates the length of a legacy null-terminated sensor list
     * 
     * Legacy sensor lists use char arrays with specific termination patterns.
     * This method safely determines the length for conversion and processing.
     * 
     * AI Context: Helper method for legacy compatibility - handles the quirks
     * of the original sensor list format.
     * 
     * @param sensorList Legacy sensor list to measure
     * @return Length of the sensor list
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
