/**
 * @file ArduRoombaCore.h
 * @brief Hardware abstraction layer for iRobot Open Interface communication
 * 
 * This file implements the core communication layer that provides a hardware
 * abstraction for the iRobot Open Interface protocol. It manages low-level
 * serial communication, baud rate negotiation, protocol state management,
 * and data streaming functionality. This layer serves as the foundation
 * for all higher-level robot operations.
 * 
 * Architecture Context for AI Agents:
 * - **Hardware Abstraction Layer**: Isolates protocol details from application logic
 * - **State Machine Management**: Handles OI mode transitions and protocol states
 * - **Communication Protocol**: Implements iRobot OI specification compliance
 * - **Error Detection**: Provides robust error detection and recovery mechanisms
 * - **Resource Management**: Manages serial port resources and timing constraints
 * 
 * Protocol Implementation:
 * - Supports iRobot Create 2 and Roomba 500/600/700 series
 * - Handles baud rate changes and protocol initialization
 * - Manages command/response cycles and timing requirements
 * - Provides streaming data acquisition capabilities
 * - Implements checksum validation and error detection
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBACORE_H
#define ARDUROOMBACORE_H

// ============================================================================
// SYSTEM INCLUDES
// ============================================================================
#include <Arduino.h>
#include <SoftwareSerial.h>

// ============================================================================
// LIBRARY INCLUDES
// ============================================================================
#include "ArduRoombaConstants.h"  // Protocol constants and command definitions
#include "ArduRoombaTypes.h"      // Type definitions and error codes

namespace ArduRoomba {

/**
 * @brief Hardware abstraction layer implementing iRobot Open Interface protocol
 * 
 * This class provides the foundational communication layer for all ArduRoomba
 * operations. It implements the complete iRobot Open Interface specification,
 * managing hardware connections, protocol state machines, and data streaming.
 * All higher-level components (Sensors, Commands) depend on this core layer.
 * 
 * Architecture Design for AI Agents:
 * - **Protocol State Machine**: Manages OI modes (Passive, Safe, Full)
 * - **Hardware Abstraction**: Isolates pin management and serial communication
 * - **Error Recovery**: Implements robust error detection and recovery strategies
 * - **Resource Management**: Handles serial port lifecycle and timing constraints
 * - **Data Validation**: Provides checksum validation and protocol compliance
 * 
 * Communication Flow:
 * 1. Hardware initialization and pin configuration
 * 2. Baud rate negotiation and protocol handshake
 * 3. Mode transition to enable robot control
 * 4. Command transmission with response validation
 * 5. Continuous sensor data streaming (optional)
 * 6. Error detection and recovery procedures
 * 
 * Protocol Compliance:
 * - iRobot Create 2 Open Interface Specification
 * - Roomba 500/600/700 series compatibility
 * - Timing requirements and command sequencing
 * - Checksum validation for data integrity
 * 
 * Thread Safety: This class is NOT thread-safe. External synchronization
 * is required if accessed from multiple contexts.
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

#endif // ARDUROOMBACORE_H