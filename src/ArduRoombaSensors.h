/**
 * @file ArduRoombaSensors.h
 * @brief Sensor management and data parsing for the ArduRoomba library
 * 
 * This file contains the sensor management class that handles all sensor
 * data retrieval, parsing, and streaming functionality. It provides a
 * high-level interface for accessing Roomba sensor information.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.3.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBA_SENSORS_H
#define ARDUROOMBA_SENSORS_H

#include <Arduino.h>
#include "ArduRoombaConstants.h"
#include "ArduRoombaTypes.h"
#include "ArduRoombaCore.h"

namespace ArduRoomba {

/**
 * @brief Sensor management class for iRobot Open Interface
 * 
 * This class provides high-level access to all Roomba sensor data,
 * including individual sensor queries, group queries, and streaming
 * sensor data. It handles data parsing and provides convenient
 * access methods for all sensor information.
 */
class RoombaSensors {
public:
    /**
     * @brief Constructor for RoombaSensors
     * @param core Reference to the RoombaCore instance
     */
    explicit RoombaSensors(RoombaCore& core);
    
    /**
     * @brief Destructor
     */
    ~RoombaSensors();
    
    // ========================================================================
    // INDIVIDUAL SENSOR QUERIES
    // ========================================================================
    
    /**
     * @brief Request a single sensor packet
     * @param packetId Sensor packet ID to request
     * @param data Buffer to store sensor data
     * @param dataSize Size of the data buffer
     * @return ErrorCode indicating success or failure
     */
    ErrorCode requestSensor(SensorPacket packetId, uint8_t* data, uint8_t* dataSize);
    
    /**
     * @brief Request multiple sensor packets
     * @param packetIds Array of sensor packet IDs
     * @param packetCount Number of packets to request
     * @param data Buffer to store sensor data
     * @param dataSize Size of the data buffer
     * @return ErrorCode indicating success or failure
     */
    ErrorCode requestSensors(const SensorPacket* packetIds, uint8_t packetCount, 
                           uint8_t* data, uint8_t* dataSize);
    
    /**
     * @brief Request a sensor group packet
     * @param groupId Sensor group ID (0-6, 100-107)
     * @param data Buffer to store sensor data
     * @param dataSize Size of the data buffer
     * @return ErrorCode indicating success or failure
     */
    ErrorCode requestSensorGroup(uint8_t groupId, uint8_t* data, uint8_t* dataSize);
    
    // ========================================================================
    // STREAMING SENSOR DATA
    // ========================================================================
    
    /**
     * @brief Start streaming sensor data
     * @param sensorList Array of sensor packet IDs to stream
     * @param sensorCount Number of sensors in the list
     * @return ErrorCode indicating success or failure
     */
    ErrorCode startStreaming(const SensorPacket* sensorList, uint8_t sensorCount);
    
    /**
     * @brief Start streaming with predefined sensor set
     * @param preset Predefined sensor set to stream
     * @return ErrorCode indicating success or failure
     */
    ErrorCode startStreaming(SensorPreset preset);
    
    /**
     * @brief Stop streaming sensor data
     * @return ErrorCode indicating success or failure
     */
    ErrorCode stopStreaming();
    
    /**
     * @brief Update sensor data from stream
     * @param sensorData Reference to SensorData structure to update
     * @return ErrorCode indicating success or failure
     */
    ErrorCode updateFromStream(SensorData& sensorData);
    
    /**
     * @brief Check if streaming is currently active
     * @return true if streaming is active, false otherwise
     */
    bool isStreaming() const;
    
    // ========================================================================
    // HIGH-LEVEL SENSOR ACCESS
    // ========================================================================
    
    /**
     * @brief Get all available sensor data
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
     * @brief Get battery information
     * @param voltage Reference to store battery voltage (mV)
     * @param current Reference to store battery current (mA)
     * @param charge Reference to store battery charge (mAh)
     * @param capacity Reference to store battery capacity (mAh)
     * @param temperature Reference to store battery temperature (°C)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getBatteryInfo(uint16_t& voltage, int16_t& current, uint16_t& charge, 
                           uint16_t& capacity, int8_t& temperature);
    
    /**
     * @brief Get cliff sensor data
     * @param left Reference to store left cliff sensor state
     * @param frontLeft Reference to store front left cliff sensor state
     * @param frontRight Reference to store front right cliff sensor state
     * @param right Reference to store right cliff sensor state
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getCliffSensors(bool& left, bool& frontLeft, bool& frontRight, bool& right);
    
    /**
     * @brief Get bumper sensor data
     * @param left Reference to store left bumper state
     * @param right Reference to store right bumper state
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getBumperSensors(bool& left, bool& right);
    
    /**
     * @brief Get wheel drop sensor data
     * @param left Reference to store left wheel drop state
     * @param right Reference to store right wheel drop state
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getWheelDropSensors(bool& left, bool& right);
    
    /**
     * @brief Get button states
     * @param buttons Reference to store button states (bitmask)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getButtonStates(uint8_t& buttons);
    
    /**
     * @brief Get current Open Interface mode
     * @param mode Reference to store current OI mode
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getOIMode(OIMode& mode);
    
    /**
     * @brief Get charging state
     * @param state Reference to store charging state
     * @return ErrorCode indicating success or failure
     */
    ErrorCode getChargingState(ChargingState& state);
    
    // ========================================================================
    // SENSOR PRESETS
    // ========================================================================
    
    /**
     * @brief Predefined sensor sets for common use cases
     */
    enum class SensorPreset {
        BASIC,          ///< Basic sensors (bumpers, cliffs, wall, battery)
        NAVIGATION,     ///< Navigation sensors (encoders, velocity, radius)
        SAFETY,         ///< Safety sensors (cliffs, bumpers, wheel drops, overcurrents)
        BATTERY,        ///< Battery sensors (voltage, current, charge, capacity, temperature)
        BUTTONS,        ///< Button and IR sensors
        LIGHT_BUMPERS,  ///< Light bumper sensors
        ALL_SENSORS     ///< All available sensors
    };
    
    /**
     * @brief Get sensor list for a preset
     * @param preset Sensor preset
     * @param sensorList Buffer to store sensor list
     * @param maxSensors Maximum number of sensors in buffer
     * @return Number of sensors in the preset
     */
    static uint8_t getPresetSensorList(SensorPreset preset, SensorPacket* sensorList, uint8_t maxSensors);
    
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
     * @brief Print sensor data to Serial (for debugging)
     * @param sensorData Sensor data to print
     */
    void printSensorData(const SensorData& sensorData);
    
    /**
     * @brief Print differences between two sensor data sets
     * @param oldData Previous sensor data
     * @param newData Current sensor data
     */
    void printSensorDifferences(const SensorData& oldData, const SensorData& newData);
    
protected:
    // ========================================================================
    // INTERNAL PARSING METHODS
    // ========================================================================
    
    /**
     * @brief Parse stream buffer and update sensor data
     * @param buffer Stream data buffer
     * @param bufferSize Size of stream data
     * @param sensorData Reference to SensorData to update
     * @return ErrorCode indicating success or failure
     */
    ErrorCode parseStreamBuffer(const uint8_t* buffer, uint8_t bufferSize, SensorData& sensorData);
    
    /**
     * @brief Parse a single byte from stream buffer
     * @param buffer Stream data buffer
     * @param index Reference to current buffer index (will be incremented)
     * @return Parsed byte value
     */
    uint8_t parseOneByte(const uint8_t* buffer, uint8_t& index);
    
    /**
     * @brief Parse two bytes from stream buffer as signed 16-bit value
     * @param buffer Stream data buffer
     * @param index Reference to current buffer index (will be incremented by 2)
     * @return Parsed 16-bit value
     */
    int16_t parseTwoBytes(const uint8_t* buffer, uint8_t& index);
    
    /**
     * @brief Parse two bytes from stream buffer as unsigned 16-bit value
     * @param buffer Stream data buffer
     * @param index Reference to current buffer index (will be incremented by 2)
     * @return Parsed 16-bit value
     */
    uint16_t parseTwoBytesUnsigned(const uint8_t* buffer, uint8_t& index);
    
    /**
     * @brief Parse bit flags from a single byte
     * @param buffer Stream data buffer
     * @param index Reference to current buffer index (will be incremented)
     * @param flags Array to store parsed bit flags (8 elements)
     */
    void parseBitFlags(const uint8_t* buffer, uint8_t& index, bool* flags);
    
    /**
     * @brief Parse individual sensor packet from buffer
     * @param packetId Sensor packet ID
     * @param buffer Data buffer
     * @param index Reference to current buffer index
     * @param sensorData Reference to SensorData to update
     * @return ErrorCode indicating success or failure
     */
    ErrorCode parseSensorPacket(SensorPacket packetId, const uint8_t* buffer, 
                              uint8_t& index, SensorData& sensorData);
    
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
    
    // Current streaming configuration
    SensorPacket _streamSensors[BufferSize::SENSOR_LIST]; ///< Current streaming sensor list
    uint8_t _streamSensorCount;     ///< Number of sensors in stream
    bool _streamingActive;          ///< Streaming status
    
    // Timing for refresh management
    unsigned long _lastRefreshTime; ///< Last sensor refresh time
    uint16_t _refreshInterval;      ///< Refresh interval in milliseconds
};

} // namespace ArduRoomba

#endif // ARDUROOMBA_SENSORS_H