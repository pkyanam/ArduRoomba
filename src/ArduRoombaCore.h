/**
 * @file ArduRoombaCore.h
 * @brief Core communication and low-level functionality for the ArduRoomba library
 * 
 * This file contains the core communication layer that handles the low-level
 * serial communication with the iRobot Open Interface. It manages the hardware
 * setup, data streaming, and basic protocol handling.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.3.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBA_CORE_H
#define ARDUROOMBA_CORE_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "ArduRoombaConstants.h"
#include "ArduRoombaTypes.h"

namespace ArduRoomba {

/**
 * @brief Core communication class for iRobot Open Interface
 * 
 * This class handles all low-level communication with the iRobot device,
 * including hardware initialization, serial communication, and data streaming.
 * It provides the foundation for higher-level sensor and command classes.
 */
class RoombaCore {
public:
    /**
     * @brief Constructor for RoombaCore
     * @param rxPin Arduino pin connected to Roomba TX
     * @param txPin Arduino pin connected to Roomba RX
     * @param brcPin Arduino pin connected to Roomba BRC (Baud Rate Change)
     */
    RoombaCore(uint8_t rxPin, uint8_t txPin, uint8_t brcPin);
    
    /**
     * @brief Destructor
     */
    ~RoombaCore();
    
    // ========================================================================
    // INITIALIZATION AND SETUP
    // ========================================================================
    
    /**
     * @brief Initialize the Roomba connection
     * 
     * This method performs the complete initialization sequence including:
     * - Hardware pin setup
     * - BRC pin pulsing for wake-up
     * - Serial communication establishment
     * - Open Interface startup
     * 
     * @param baudRate Serial communication baud rate (default: 19200)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode initialize(uint32_t baudRate = DEFAULT_BAUD_RATE);
    
    /**
     * @brief Check if the core is properly initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return _initialized; }
    
    /**
     * @brief Get the current baud rate
     * @return Current baud rate
     */
    uint32_t getBaudRate() const { return _baudRate; }
    
    // ========================================================================
    // LOW-LEVEL COMMUNICATION
    // ========================================================================
    
    /**
     * @brief Send a single byte command to the Roomba
     * @param opcode Command opcode to send
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendCommand(OIOpcode opcode);
    
    /**
     * @brief Send a command with a single parameter
     * @param opcode Command opcode to send
     * @param param Single byte parameter
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendCommand(OIOpcode opcode, uint8_t param);
    
    /**
     * @brief Send a command with two parameters
     * @param opcode Command opcode to send
     * @param param1 First parameter
     * @param param2 Second parameter
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendCommand(OIOpcode opcode, uint8_t param1, uint8_t param2);
    
    /**
     * @brief Send a command with multiple parameters
     * @param opcode Command opcode to send
     * @param params Array of parameters
     * @param paramCount Number of parameters
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendCommand(OIOpcode opcode, const uint8_t* params, uint8_t paramCount);
    
    /**
     * @brief Send a 16-bit value as two bytes (high byte first)
     * @param value 16-bit value to send
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendInt16(int16_t value);
    
    /**
     * @brief Send raw bytes to the Roomba
     * @param data Pointer to data buffer
     * @param length Number of bytes to send
     * @return ErrorCode indicating success or failure
     */
    ErrorCode sendRawData(const uint8_t* data, uint8_t length);
    
    // ========================================================================
    // DATA RECEPTION
    // ========================================================================
    
    /**
     * @brief Read available bytes from the Roomba
     * @param buffer Buffer to store received data
     * @param maxLength Maximum number of bytes to read
     * @param timeout Timeout in milliseconds
     * @return Number of bytes actually read
     */
    uint8_t readData(uint8_t* buffer, uint8_t maxLength, uint16_t timeout = 100);
    
    /**
     * @brief Check if data is available for reading
     * @return Number of bytes available
     */
    uint8_t available() const;
    
    /**
     * @brief Read a single byte with timeout
     * @param data Pointer to store the byte
     * @param timeout Timeout in milliseconds
     * @return ErrorCode indicating success or failure
     */
    ErrorCode readByte(uint8_t* data, uint16_t timeout = 100);
    
    // ========================================================================
    // STREAMING INTERFACE
    // ========================================================================
    
    /**
     * @brief Start sensor data streaming
     * @param sensorList Array of sensor packet IDs to stream
     * @param sensorCount Number of sensors in the list
     * @return ErrorCode indicating success or failure
     */
    ErrorCode startStream(const uint8_t* sensorList, uint8_t sensorCount);
    
    /**
     * @brief Stop sensor data streaming
     * @return ErrorCode indicating success or failure
     */
    ErrorCode stopStream();
    
    /**
     * @brief Read and parse streaming data
     * @param buffer Buffer to store parsed data
     * @param bufferSize Size of the buffer
     * @return ErrorCode indicating success or failure
     */
    ErrorCode readStreamData(uint8_t* buffer, uint8_t* bufferSize);
    
    /**
     * @brief Check if streaming is currently active
     * @return true if streaming is active, false otherwise
     */
    bool isStreaming() const { return _streamingActive; }
    
    // ========================================================================
    // HARDWARE CONTROL
    // ========================================================================
    
    /**
     * @brief Pulse the BRC pin to wake up the Roomba
     * @param pulseCount Number of pulses to send (default: 3)
     * @param pulseDuration Duration of each pulse in ms (default: 100)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode pulseBRC(uint8_t pulseCount = BRC_PULSE_COUNT, 
                       uint8_t pulseDuration = BRC_PULSE_DURATION);
    
    /**
     * @brief Change the baud rate of communication
     * @param newBaudRate New baud rate to use
     * @return ErrorCode indicating success or failure
     */
    ErrorCode changeBaudRate(uint32_t newBaudRate);
    
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
     * @brief Get the last error code
     * @return Last error code encountered
     */
    ErrorCode getLastError() const { return _lastError; }
    
    /**
     * @brief Get communication statistics
     * @param bytesSent Reference to store bytes sent count
     * @param bytesReceived Reference to store bytes received count
     * @param errors Reference to store error count
     */
    void getStatistics(uint32_t& bytesSent, uint32_t& bytesReceived, uint16_t& errors) const;
    
    /**
     * @brief Reset communication statistics
     */
    void resetStatistics();
    
protected:
    // ========================================================================
    // INTERNAL METHODS
    // ========================================================================
    
    /**
     * @brief Internal method to read stream header and validate format
     * @return ErrorCode indicating success or failure
     */
    ErrorCode readStreamHeader(uint8_t* dataSize);
    
    /**
     * @brief Internal method to validate stream checksum
     * @param data Pointer to stream data
     * @param dataSize Size of stream data
     * @param receivedChecksum Checksum received from stream
     * @return true if checksum is valid, false otherwise
     */
    bool validateStreamChecksum(const uint8_t* data, uint8_t dataSize, uint8_t receivedChecksum);
    
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
    
    /**
     * @brief Set the last error code
     * @param error Error code to set
     */
    void setLastError(ErrorCode error) { _lastError = error; }
    
private:
    // ========================================================================
    // MEMBER VARIABLES
    // ========================================================================
    
    // Hardware configuration
    uint8_t _rxPin;                 ///< Arduino RX pin
    uint8_t _txPin;                 ///< Arduino TX pin
    uint8_t _brcPin;                ///< Arduino BRC pin
    uint32_t _baudRate;             ///< Current baud rate
    
    // Communication objects
    SoftwareSerial _serial;         ///< Software serial for Roomba communication
    
    // State tracking
    bool _initialized;              ///< Initialization status
    bool _streamingActive;          ///< Streaming status
    bool _debugEnabled;             ///< Debug output status
    ErrorCode _lastError;           ///< Last error encountered
    
    // Stream management
    uint8_t _streamBuffer[BufferSize::STREAM_BUFFER]; ///< Stream data buffer
    uint8_t _streamSensorList[BufferSize::SENSOR_LIST]; ///< Active sensor list
    uint8_t _streamSensorCount;     ///< Number of sensors in stream
    StreamState _streamState;       ///< Current stream state
    uint8_t _streamBufferIndex;     ///< Current buffer index
    uint8_t _expectedStreamSize;    ///< Expected stream data size
    
    // Statistics
    uint32_t _bytesSent;            ///< Total bytes sent
    uint32_t _bytesReceived;        ///< Total bytes received
    uint16_t _errorCount;           ///< Total errors encountered
    
    // Constants
    static const uint8_t ZERO_BYTE = 0x00; ///< Zero byte constant
};

} // namespace ArduRoomba

#endif // ARDUROOMBA_CORE_H