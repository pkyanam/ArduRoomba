/**
 * @file ArduRoombaCore.cpp
 * @brief Implementation of core communication functionality for the ArduRoomba library
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.3.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#include "ArduRoombaCore.h"

namespace ArduRoomba {

// ============================================================================
// CONSTRUCTOR AND DESTRUCTOR
// ============================================================================

RoombaCore::RoombaCore(uint8_t rxPin, uint8_t txPin, uint8_t brcPin)
    : _rxPin(rxPin), _txPin(txPin), _brcPin(brcPin), _baudRate(DEFAULT_BAUD_RATE),
      _serial(rxPin, txPin), _initialized(false), _streamingActive(false),
      _debugEnabled(false), _lastError(ErrorCode::SUCCESS),
      _streamSensorCount(0), _streamState(StreamState::WAIT_HEADER),
      _streamBufferIndex(0), _expectedStreamSize(0),
      _bytesSent(0), _bytesReceived(0), _errorCount(0) {
    // Initialize buffers
    memset(_streamBuffer, 0, sizeof(_streamBuffer));
    memset(_streamSensorList, 0, sizeof(_streamSensorList));
}

RoombaCore::~RoombaCore() {
    if (_streamingActive) {
        stopStream();
    }
}

// ============================================================================
// INITIALIZATION AND SETUP
// ============================================================================

ErrorCode RoombaCore::initialize(uint32_t baudRate) {
    if (_initialized) {
        debugPrint("Already initialized");
        return ErrorCode::SUCCESS;
    }
    
    _baudRate = baudRate;
    
    // Setup BRC pin
    pinMode(_brcPin, OUTPUT);
    digitalWrite(_brcPin, HIGH);
    
    debugPrint("Waiting after power on...");
    delay(POWER_ON_DELAY);
    
    // Pulse BRC pin to wake up Roomba
    ErrorCode result = pulseBRC();
    if (result != ErrorCode::SUCCESS) {
        setLastError(result);
        return result;
    }
    
    debugPrint("Starting serial communication...");
    delay(150);
    _serial.begin(_baudRate);
    
    debugPrint("Sending START command...");
    delay(150);
    result = sendCommand(OIOpcode::START);
    if (result != ErrorCode::SUCCESS) {
        setLastError(result);
        return result;
    }
    
    debugPrint("Sending SAFE mode command...");
    delay(150);
    result = sendCommand(OIOpcode::SAFE);
    if (result != ErrorCode::SUCCESS) {
        setLastError(result);
        return result;
    }
    
    _initialized = true;
    debugPrint("Roomba initialization complete");
    debugPrint("Verify CLEAN light has stopped illuminating");
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// LOW-LEVEL COMMUNICATION
// ============================================================================

ErrorCode RoombaCore::sendCommand(OIOpcode opcode) {
    if (!_initialized) {
        setLastError(ErrorCode::NOT_INITIALIZED);
        return ErrorCode::NOT_INITIALIZED;
    }
    
    uint8_t command = static_cast<uint8_t>(opcode);
    _serial.write(command);
    _bytesSent++;
    
    debugPrint("Sent command", command);
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaCore::sendCommand(OIOpcode opcode, uint8_t param) {
    ErrorCode result = sendCommand(opcode);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    _serial.write(param);
    _bytesSent++;
    
    debugPrint("Sent parameter", param);
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaCore::sendCommand(OIOpcode opcode, uint8_t param1, uint8_t param2) {
    ErrorCode result = sendCommand(opcode, param1);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    _serial.write(param2);
    _bytesSent++;
    
    debugPrint("Sent parameter", param2);
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaCore::sendCommand(OIOpcode opcode, const uint8_t* params, uint8_t paramCount) {
    ErrorCode result = sendCommand(opcode);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    for (uint8_t i = 0; i < paramCount; i++) {
        _serial.write(params[i]);
        _bytesSent++;
    }
    
    debugPrint("Sent parameters, count", paramCount);
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaCore::sendInt16(int16_t value) {
    if (!_initialized) {
        setLastError(ErrorCode::NOT_INITIALIZED);
        return ErrorCode::NOT_INITIALIZED;
    }
    
    // Send high byte first, then low byte
    _serial.write((value >> 8) & 0xFF);
    _serial.write(value & 0xFF);
    _bytesSent += 2;
    
    debugPrint("Sent 16-bit value", value);
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaCore::sendRawData(const uint8_t* data, uint8_t length) {
    if (!_initialized) {
        setLastError(ErrorCode::NOT_INITIALIZED);
        return ErrorCode::NOT_INITIALIZED;
    }
    
    if (data == nullptr || length == 0) {
        setLastError(ErrorCode::INVALID_PARAMETER);
        return ErrorCode::INVALID_PARAMETER;
    }
    
    for (uint8_t i = 0; i < length; i++) {
        _serial.write(data[i]);
    }
    _bytesSent += length;
    
    debugPrint("Sent raw data, length", length);
    return ErrorCode::SUCCESS;
}

// ============================================================================
// DATA RECEPTION
// ============================================================================

uint8_t RoombaCore::readData(uint8_t* buffer, uint8_t maxLength, uint16_t timeout) {
    if (!_initialized || buffer == nullptr || maxLength == 0) {
        return 0;
    }
    
    unsigned long startTime = millis();
    uint8_t bytesRead = 0;
    
    while (bytesRead < maxLength && (millis() - startTime) < timeout) {
        if (_serial.available()) {
            buffer[bytesRead] = _serial.read();
            bytesRead++;
            _bytesReceived++;
        }
    }
    
    debugPrint("Read bytes", bytesRead);
    return bytesRead;
}

uint8_t RoombaCore::available() const {
    if (!_initialized) {
        return 0;
    }
    return _serial.available();
}

ErrorCode RoombaCore::readByte(uint8_t* data, uint16_t timeout) {
    if (!_initialized) {
        setLastError(ErrorCode::NOT_INITIALIZED);
        return ErrorCode::NOT_INITIALIZED;
    }
    
    if (data == nullptr) {
        setLastError(ErrorCode::INVALID_PARAMETER);
        return ErrorCode::INVALID_PARAMETER;
    }
    
    unsigned long startTime = millis();
    
    while (!_serial.available()) {
        if ((millis() - startTime) >= timeout) {
            setLastError(ErrorCode::TIMEOUT);
            return ErrorCode::TIMEOUT;
        }
    }
    
    *data = _serial.read();
    _bytesReceived++;
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// STREAMING INTERFACE
// ============================================================================

ErrorCode RoombaCore::startStream(const uint8_t* sensorList, uint8_t sensorCount) {
    if (!_initialized) {
        setLastError(ErrorCode::NOT_INITIALIZED);
        return ErrorCode::NOT_INITIALIZED;
    }
    
    if (sensorList == nullptr || sensorCount == 0 || sensorCount > BufferSize::SENSOR_LIST) {
        setLastError(ErrorCode::INVALID_PARAMETER);
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Stop any existing stream
    if (_streamingActive) {
        stopStream();
    }
    
    // Copy sensor list
    _streamSensorCount = sensorCount;
    memcpy(_streamSensorList, sensorList, sensorCount);
    
    // Send stream command
    ErrorCode result = sendCommand(OIOpcode::STREAM, sensorCount);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    // Send sensor list
    result = sendRawData(sensorList, sensorCount);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    _streamingActive = true;
    _streamState = StreamState::WAIT_HEADER;
    _streamBufferIndex = 0;
    
    debugPrint("Started streaming with sensors", sensorCount);
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaCore::stopStream() {
    if (!_initialized) {
        setLastError(ErrorCode::NOT_INITIALIZED);
        return ErrorCode::NOT_INITIALIZED;
    }
    
    // Send empty stream command
    ErrorCode result = sendCommand(OIOpcode::STREAM, ZERO_BYTE);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    _streamingActive = false;
    _streamSensorCount = 0;
    _streamState = StreamState::WAIT_HEADER;
    _streamBufferIndex = 0;
    
    debugPrint("Stopped streaming");
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaCore::readStreamData(uint8_t* buffer, uint8_t* bufferSize) {
    if (!_initialized) {
        setLastError(ErrorCode::NOT_INITIALIZED);
        return ErrorCode::NOT_INITIALIZED;
    }
    
    if (!_streamingActive) {
        setLastError(ErrorCode::COMMUNICATION_ERROR);
        return ErrorCode::COMMUNICATION_ERROR;
    }
    
    if (buffer == nullptr || bufferSize == nullptr) {
        setLastError(ErrorCode::INVALID_PARAMETER);
        return ErrorCode::INVALID_PARAMETER;
    }
    
    unsigned long timeout = millis() + STREAM_TIMEOUT;
    
    // Wait for data to be available
    while (!_serial.available()) {
        if (millis() > timeout) {
            setLastError(ErrorCode::TIMEOUT);
            return ErrorCode::TIMEOUT;
        }
    }
    
    // Process incoming stream data
    while (_serial.available() && _streamState != StreamState::END) {
        uint8_t byte = _serial.read();
        _bytesReceived++;
        
        switch (_streamState) {
            case StreamState::WAIT_HEADER:
                if (byte == STREAM_HEADER) {
                    _streamState = StreamState::WAIT_SIZE;
                    debugPrint("Stream header received");
                }
                break;
                
            case StreamState::WAIT_SIZE:
                _expectedStreamSize = byte;
                _streamBufferIndex = 0;
                _streamState = StreamState::WAIT_CONTENT;
                debugPrint("Stream size", _expectedStreamSize);
                break;
                
            case StreamState::WAIT_CONTENT:
                if (_streamBufferIndex < _expectedStreamSize && 
                    _streamBufferIndex < BufferSize::STREAM_BUFFER) {
                    _streamBuffer[_streamBufferIndex++] = byte;
                    
                    if (_streamBufferIndex >= _expectedStreamSize) {
                        _streamState = StreamState::WAIT_CHECKSUM;
                    }
                } else {
                    setLastError(ErrorCode::BUFFER_OVERFLOW);
                    return ErrorCode::BUFFER_OVERFLOW;
                }
                break;
                
            case StreamState::WAIT_CHECKSUM:
                // Validate checksum
                if (validateStreamChecksum(_streamBuffer, _expectedStreamSize, byte)) {
                    // Copy data to output buffer
                    uint8_t copySize = min(_expectedStreamSize, *bufferSize);
                    memcpy(buffer, _streamBuffer, copySize);
                    *bufferSize = copySize;
                    
                    _streamState = StreamState::END;
                    debugPrint("Stream data received successfully");
                    return ErrorCode::SUCCESS;
                } else {
                    setLastError(ErrorCode::CHECKSUM_ERROR);
                    _streamState = StreamState::WAIT_HEADER; // Reset for next stream
                    return ErrorCode::CHECKSUM_ERROR;
                }
                break;
                
            case StreamState::END:
                // Should not reach here
                break;
        }
    }
    
    // Reset state for next stream packet
    _streamState = StreamState::WAIT_HEADER;
    
    setLastError(ErrorCode::COMMUNICATION_ERROR);
    return ErrorCode::COMMUNICATION_ERROR;
}

// ============================================================================
// HARDWARE CONTROL
// ============================================================================

ErrorCode RoombaCore::pulseBRC(uint8_t pulseCount, uint8_t pulseDuration) {
    debugPrint("Pulsing BRC pin", pulseCount);
    
    for (uint8_t i = 0; i < pulseCount; i++) {
        digitalWrite(_brcPin, LOW);
        delay(pulseDuration);
        digitalWrite(_brcPin, HIGH);
        delay(pulseDuration);
    }
    
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaCore::changeBaudRate(uint32_t newBaudRate) {
    if (!_initialized) {
        setLastError(ErrorCode::NOT_INITIALIZED);
        return ErrorCode::NOT_INITIALIZED;
    }
    
    // Map baud rate to OI baud code
    uint8_t baudCode;
    switch (newBaudRate) {
        case 300:    baudCode = 0; break;
        case 600:    baudCode = 1; break;
        case 1200:   baudCode = 2; break;
        case 2400:   baudCode = 3; break;
        case 4800:   baudCode = 4; break;
        case 9600:   baudCode = 5; break;
        case 14400:  baudCode = 6; break;
        case 19200:  baudCode = 7; break;
        case 28800:  baudCode = 8; break;
        case 38400:  baudCode = 9; break;
        case 57600:  baudCode = 10; break;
        case 115200: baudCode = 11; break;
        default:
            setLastError(ErrorCode::INVALID_PARAMETER);
            return ErrorCode::INVALID_PARAMETER;
    }
    
    // Send baud change command
    ErrorCode result = sendCommand(OIOpcode::BAUD, baudCode);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    // Wait for command to process
    delay(100);
    
    // Change our serial baud rate
    _serial.end();
    delay(100);
    _serial.begin(newBaudRate);
    _baudRate = newBaudRate;
    
    debugPrint("Changed baud rate to", newBaudRate);
    return ErrorCode::SUCCESS;
}

// ============================================================================
// DEBUGGING AND DIAGNOSTICS
// ============================================================================

void RoombaCore::getStatistics(uint32_t& bytesSent, uint32_t& bytesReceived, uint16_t& errors) const {
    bytesSent = _bytesSent;
    bytesReceived = _bytesReceived;
    errors = _errorCount;
}

void RoombaCore::resetStatistics() {
    _bytesSent = 0;
    _bytesReceived = 0;
    _errorCount = 0;
}

// ============================================================================
// INTERNAL METHODS
// ============================================================================

bool RoombaCore::validateStreamChecksum(const uint8_t* data, uint8_t dataSize, uint8_t receivedChecksum) {
    if (data == nullptr) {
        return false;
    }
    
    // Calculate checksum: header + size + data + checksum should sum to 0
    uint8_t calculatedChecksum = STREAM_HEADER + dataSize + receivedChecksum;
    
    for (uint8_t i = 0; i < dataSize; i++) {
        calculatedChecksum += data[i];
    }
    
    bool isValid = (calculatedChecksum & 0xFF) == 0;
    
    if (!isValid) {
        debugPrint("Checksum validation failed");
        _errorCount++;
    }
    
    return isValid;
}

void RoombaCore::debugPrint(const char* message) {
    if (_debugEnabled && message != nullptr) {
        Serial.print("ArduRoombaCore: ");
        Serial.println(message);
    }
}

void RoombaCore::debugPrint(const char* message, int value) {
    if (_debugEnabled && message != nullptr) {
        Serial.print("ArduRoombaCore: ");
        Serial.print(message);
        Serial.print(" = ");
        Serial.println(value);
    }
}

} // namespace ArduRoomba