/**
 * @file ArduRoombaSensors.cpp
 * @brief Implementation of sensor management functionality for the ArduRoomba library
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.3.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#include "ArduRoombaSensors.h"

namespace ArduRoomba {

// ============================================================================
// CONSTRUCTOR AND DESTRUCTOR
// ============================================================================

RoombaSensors::RoombaSensors(RoombaCore& core)
    : _core(core), _debugEnabled(false), _streamSensorCount(0), 
      _streamingActive(false), _lastRefreshTime(0), _refreshInterval(REFRESH_DELAY) {
    memset(_streamSensors, 0, sizeof(_streamSensors));
}

RoombaSensors::~RoombaSensors() {
    if (_streamingActive) {
        stopStreaming();
    }
}

// ============================================================================
// INDIVIDUAL SENSOR QUERIES
// ============================================================================

ErrorCode RoombaSensors::requestSensor(SensorPacket packetId, uint8_t* data, uint8_t* dataSize) {
    if (data == nullptr || dataSize == nullptr) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    uint8_t packet = static_cast<uint8_t>(packetId);
    ErrorCode result = _core.sendCommand(OIOpcode::SENSORS, packet);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    // Wait for response
    delay(15);
    
    // Read response data
    uint8_t bytesRead = _core.readData(data, *dataSize, 100);
    *dataSize = bytesRead;
    
    debugPrint("Requested sensor packet", packet);
    debugPrint("Received bytes", bytesRead);
    
    return (bytesRead > 0) ? ErrorCode::SUCCESS : ErrorCode::TIMEOUT;
}

ErrorCode RoombaSensors::requestSensors(const SensorPacket* packetIds, uint8_t packetCount, 
                                      uint8_t* data, uint8_t* dataSize) {
    if (packetIds == nullptr || data == nullptr || dataSize == nullptr || packetCount == 0) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Convert SensorPacket enum to uint8_t array
    uint8_t packets[packetCount];
    for (uint8_t i = 0; i < packetCount; i++) {
        packets[i] = static_cast<uint8_t>(packetIds[i]);
    }
    
    ErrorCode result = _core.sendCommand(OIOpcode::QUERY_LIST, packetCount);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    result = _core.sendRawData(packets, packetCount);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    // Wait for response
    delay(15);
    
    // Read response data
    uint8_t bytesRead = _core.readData(data, *dataSize, 100);
    *dataSize = bytesRead;
    
    debugPrint("Requested sensor packets", packetCount);
    debugPrint("Received bytes", bytesRead);
    
    return (bytesRead > 0) ? ErrorCode::SUCCESS : ErrorCode::TIMEOUT;
}

ErrorCode RoombaSensors::requestSensorGroup(uint8_t groupId, uint8_t* data, uint8_t* dataSize) {
    if (data == nullptr || dataSize == nullptr) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    ErrorCode result = _core.sendCommand(OIOpcode::SENSORS, groupId);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    // Wait for response
    delay(15);
    
    // Read response data
    uint8_t bytesRead = _core.readData(data, *dataSize, 100);
    *dataSize = bytesRead;
    
    debugPrint("Requested sensor group", groupId);
    debugPrint("Received bytes", bytesRead);
    
    return (bytesRead > 0) ? ErrorCode::SUCCESS : ErrorCode::TIMEOUT;
}

// ============================================================================
// STREAMING SENSOR DATA
// ============================================================================

ErrorCode RoombaSensors::startStreaming(const SensorPacket* sensorList, uint8_t sensorCount) {
    if (sensorList == nullptr || sensorCount == 0 || sensorCount > BufferSize::SENSOR_LIST) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Convert SensorPacket enum to uint8_t array
    uint8_t packets[sensorCount];
    for (uint8_t i = 0; i < sensorCount; i++) {
        packets[i] = static_cast<uint8_t>(sensorList[i]);
        _streamSensors[i] = sensorList[i];
    }
    
    ErrorCode result = _core.startStream(packets, sensorCount);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    _streamSensorCount = sensorCount;
    _streamingActive = true;
    
    debugPrint("Started streaming with sensors", sensorCount);
    return ErrorCode::SUCCESS;
}

ErrorCode RoombaSensors::startStreaming(SensorPreset preset) {
    SensorPacket sensorList[BufferSize::SENSOR_LIST];
    uint8_t sensorCount = getPresetSensorList(preset, sensorList, BufferSize::SENSOR_LIST);
    
    if (sensorCount == 0) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    return startStreaming(sensorList, sensorCount);
}

ErrorCode RoombaSensors::stopStreaming() {
    ErrorCode result = _core.stopStream();
    if (result == ErrorCode::SUCCESS) {
        _streamingActive = false;
        _streamSensorCount = 0;
        debugPrint("Stopped streaming");
    }
    return result;
}

ErrorCode RoombaSensors::updateFromStream(SensorData& sensorData) {
    if (!_streamingActive) {
        return ErrorCode::COMMUNICATION_ERROR;
    }
    
    unsigned long now = millis();
    if (now < sensorData.nextRefresh) {
        return ErrorCode::SUCCESS; // Not time for refresh yet
    }
    
    sensorData.nextRefresh = now + _refreshInterval;
    sensorData.failedAttempts++;
    
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = _core.readStreamData(buffer, &bufferSize);
    if (result == ErrorCode::SUCCESS) {
        result = parseStreamBuffer(buffer, bufferSize, sensorData);
        if (result == ErrorCode::SUCCESS) {
            sensorData.lastSuccessfulRefresh = now;
            sensorData.failedAttempts = 0;
            debugPrint("Stream data updated successfully");
        }
    }
    
    return result;
}

bool RoombaSensors::isStreaming() const {
    return _streamingActive && _core.isStreaming();
}

// ============================================================================
// HIGH-LEVEL SENSOR ACCESS
// ============================================================================

ErrorCode RoombaSensors::getAllSensorData(SensorData& sensorData) {
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensorGroup(100, buffer, &bufferSize);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    // Parse the group 100 data (contains most sensors)
    uint8_t index = 0;
    return parseStreamBuffer(buffer, bufferSize, sensorData);
}

ErrorCode RoombaSensors::getBasicSensorData(SensorData& sensorData) {
    const SensorPacket basicSensors[] = {
        SensorPacket::BUMPS_WHEEL_DROPS,
        SensorPacket::WALL,
        SensorPacket::CLIFF_LEFT,
        SensorPacket::CLIFF_FRONT_LEFT,
        SensorPacket::CLIFF_FRONT_RIGHT,
        SensorPacket::CLIFF_RIGHT,
        SensorPacket::VOLTAGE,
        SensorPacket::CURRENT,
        SensorPacket::BATTERY_CHARGE,
        SensorPacket::BATTERY_CAPACITY
    };
    
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensors(basicSensors, sizeof(basicSensors)/sizeof(basicSensors[0]), 
                                    buffer, &bufferSize);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    return parseStreamBuffer(buffer, bufferSize, sensorData);
}

ErrorCode RoombaSensors::getBatteryInfo(uint16_t& voltage, int16_t& current, uint16_t& charge, 
                                      uint16_t& capacity, int8_t& temperature) {
    const SensorPacket batterySensors[] = {
        SensorPacket::VOLTAGE,
        SensorPacket::CURRENT,
        SensorPacket::BATTERY_CHARGE,
        SensorPacket::BATTERY_CAPACITY,
        SensorPacket::TEMPERATURE
    };
    
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensors(batterySensors, sizeof(batterySensors)/sizeof(batterySensors[0]), 
                                    buffer, &bufferSize);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    SensorData tempData;
    result = parseStreamBuffer(buffer, bufferSize, tempData);
    if (result == ErrorCode::SUCCESS) {
        voltage = tempData.voltage;
        current = tempData.current;
        charge = tempData.batteryCharge;
        capacity = tempData.batteryCapacity;
        temperature = tempData.temperature;
    }
    
    return result;
}

ErrorCode RoombaSensors::getCliffSensors(bool& left, bool& frontLeft, bool& frontRight, bool& right) {
    const SensorPacket cliffSensors[] = {
        SensorPacket::CLIFF_LEFT,
        SensorPacket::CLIFF_FRONT_LEFT,
        SensorPacket::CLIFF_FRONT_RIGHT,
        SensorPacket::CLIFF_RIGHT
    };
    
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensors(cliffSensors, sizeof(cliffSensors)/sizeof(cliffSensors[0]), 
                                    buffer, &bufferSize);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    SensorData tempData;
    result = parseStreamBuffer(buffer, bufferSize, tempData);
    if (result == ErrorCode::SUCCESS) {
        left = tempData.cliffLeft;
        frontLeft = tempData.cliffFrontLeft;
        frontRight = tempData.cliffFrontRight;
        right = tempData.cliffRight;
    }
    
    return result;
}

ErrorCode RoombaSensors::getBumperSensors(bool& left, bool& right) {
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensor(SensorPacket::BUMPS_WHEEL_DROPS, buffer, &bufferSize);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    if (bufferSize > 0) {
        uint8_t bumpData = buffer[0];
        right = (bumpData & 0x01) != 0;
        left = (bumpData & 0x02) != 0;
    }
    
    return result;
}

ErrorCode RoombaSensors::getWheelDropSensors(bool& left, bool& right) {
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensor(SensorPacket::BUMPS_WHEEL_DROPS, buffer, &bufferSize);
    if (result != ErrorCode::SUCCESS) {
        return result;
    }
    
    if (bufferSize > 0) {
        uint8_t dropData = buffer[0];
        right = (dropData & 0x04) != 0;
        left = (dropData & 0x08) != 0;
    }
    
    return result;
}

ErrorCode RoombaSensors::getButtonStates(uint8_t& buttons) {
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensor(SensorPacket::BUTTONS, buffer, &bufferSize);
    if (result == ErrorCode::SUCCESS && bufferSize > 0) {
        buttons = buffer[0];
    }
    
    return result;
}

ErrorCode RoombaSensors::getOIMode(OIMode& mode) {
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensor(SensorPacket::OI_MODE, buffer, &bufferSize);
    if (result == ErrorCode::SUCCESS && bufferSize > 0) {
        mode = static_cast<OIMode>(buffer[0]);
    }
    
    return result;
}

ErrorCode RoombaSensors::getChargingState(ChargingState& state) {
    uint8_t buffer[BufferSize::STREAM_BUFFER];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = requestSensor(SensorPacket::CHARGING_STATE, buffer, &bufferSize);
    if (result == ErrorCode::SUCCESS && bufferSize > 0) {
        state = static_cast<ChargingState>(buffer[0]);
    }
    
    return result;
}

// ============================================================================
// SENSOR PRESETS
// ============================================================================

uint8_t RoombaSensors::getPresetSensorList(SensorPreset preset, SensorPacket* sensorList, uint8_t maxSensors) {
    if (sensorList == nullptr || maxSensors == 0) {
        return 0;
    }
    
    uint8_t count = 0;
    
    switch (preset) {
        case SensorPreset::BASIC:
            if (maxSensors >= 6) {
                sensorList[count++] = SensorPacket::BUMPS_WHEEL_DROPS;
                sensorList[count++] = SensorPacket::WALL;
                sensorList[count++] = SensorPacket::CLIFF_LEFT;
                sensorList[count++] = SensorPacket::CLIFF_FRONT_LEFT;
                sensorList[count++] = SensorPacket::CLIFF_FRONT_RIGHT;
                sensorList[count++] = SensorPacket::CLIFF_RIGHT;
                sensorList[count++] = SensorPacket::VOLTAGE;
                sensorList[count++] = SensorPacket::BATTERY_CHARGE;
            }
            break;
            
        case SensorPreset::NAVIGATION:
            if (maxSensors >= 5) {
                sensorList[count++] = SensorPacket::VELOCITY;
                sensorList[count++] = SensorPacket::RADIUS;
                sensorList[count++] = SensorPacket::VELOCITY_LEFT;
                sensorList[count++] = SensorPacket::VELOCITY_RIGHT;
                sensorList[count++] = SensorPacket::ENCODER_COUNTS_LEFT;
                sensorList[count++] = SensorPacket::ENCODER_COUNTS_RIGHT;
            }
            break;
            
        case SensorPreset::SAFETY:
            if (maxSensors >= 8) {
                sensorList[count++] = SensorPacket::BUMPS_WHEEL_DROPS;
                sensorList[count++] = SensorPacket::CLIFF_LEFT;
                sensorList[count++] = SensorPacket::CLIFF_FRONT_LEFT;
                sensorList[count++] = SensorPacket::CLIFF_FRONT_RIGHT;
                sensorList[count++] = SensorPacket::CLIFF_RIGHT;
                sensorList[count++] = SensorPacket::WHEEL_OVERCURRENTS;
                sensorList[count++] = SensorPacket::VIRTUAL_WALL;
                sensorList[count++] = SensorPacket::WALL;
            }
            break;
            
        case SensorPreset::BATTERY:
            if (maxSensors >= 5) {
                sensorList[count++] = SensorPacket::VOLTAGE;
                sensorList[count++] = SensorPacket::CURRENT;
                sensorList[count++] = SensorPacket::BATTERY_CHARGE;
                sensorList[count++] = SensorPacket::BATTERY_CAPACITY;
                sensorList[count++] = SensorPacket::TEMPERATURE;
                sensorList[count++] = SensorPacket::CHARGING_STATE;
            }
            break;
            
        case SensorPreset::BUTTONS:
            if (maxSensors >= 3) {
                sensorList[count++] = SensorPacket::BUTTONS;
                sensorList[count++] = SensorPacket::IR_OPCODE;
                sensorList[count++] = SensorPacket::IR_OPCODE_LEFT;
                sensorList[count++] = SensorPacket::IR_OPCODE_RIGHT;
            }
            break;
            
        case SensorPreset::LIGHT_BUMPERS:
            if (maxSensors >= 7) {
                sensorList[count++] = SensorPacket::LIGHT_BUMPER;
                sensorList[count++] = SensorPacket::LIGHT_BUMP_LEFT_SIGNAL;
                sensorList[count++] = SensorPacket::LIGHT_BUMP_FRONT_LEFT_SIGNAL;
                sensorList[count++] = SensorPacket::LIGHT_BUMP_CENTER_LEFT_SIGNAL;
                sensorList[count++] = SensorPacket::LIGHT_BUMP_CENTER_RIGHT_SIGNAL;
                sensorList[count++] = SensorPacket::LIGHT_BUMP_FRONT_RIGHT_SIGNAL;
                sensorList[count++] = SensorPacket::LIGHT_BUMP_RIGHT_SIGNAL;
            }
            break;
            
        case SensorPreset::ALL_SENSORS:
            // Add all commonly used sensors (limited by maxSensors)
            if (maxSensors >= 15) {
                sensorList[count++] = SensorPacket::OI_MODE;
                sensorList[count++] = SensorPacket::TEMPERATURE;
                sensorList[count++] = SensorPacket::VOLTAGE;
                sensorList[count++] = SensorPacket::BATTERY_CHARGE;
                sensorList[count++] = SensorPacket::BUMPS_WHEEL_DROPS;
                sensorList[count++] = SensorPacket::WALL;
                sensorList[count++] = SensorPacket::CLIFF_LEFT;
                sensorList[count++] = SensorPacket::CLIFF_FRONT_LEFT;
                sensorList[count++] = SensorPacket::CLIFF_RIGHT;
                sensorList[count++] = SensorPacket::CLIFF_FRONT_RIGHT;
                sensorList[count++] = SensorPacket::CHARGING_STATE;
                sensorList[count++] = SensorPacket::CURRENT;
                sensorList[count++] = SensorPacket::BATTERY_CAPACITY;
                sensorList[count++] = SensorPacket::BUTTONS;
                sensorList[count++] = SensorPacket::VELOCITY;
            }
            break;
    }
    
    return count;
}

// ============================================================================
// DEBUGGING AND DIAGNOSTICS
// ============================================================================

void RoombaSensors::printSensorData(const SensorData& sensorData) {
    if (!_debugEnabled) return;
    
    Serial.println("=== Sensor Data ===");
    Serial.print("Mode: "); Serial.println(static_cast<uint8_t>(sensorData.mode));
    Serial.print("Voltage: "); Serial.print(sensorData.voltage); Serial.println(" mV");
    Serial.print("Current: "); Serial.print(sensorData.current); Serial.println(" mA");
    Serial.print("Battery Charge: "); Serial.print(sensorData.batteryCharge); Serial.println(" mAh");
    Serial.print("Battery Capacity: "); Serial.print(sensorData.batteryCapacity); Serial.println(" mAh");
    Serial.print("Temperature: "); Serial.print(sensorData.temperature); Serial.println(" °C");
    Serial.print("Charging State: "); Serial.println(static_cast<uint8_t>(sensorData.chargingState));
    
    Serial.print("Bumpers - Left: "); Serial.print(sensorData.bumpLeft);
    Serial.print(", Right: "); Serial.println(sensorData.bumpRight);
    
    Serial.print("Cliffs - Left: "); Serial.print(sensorData.cliffLeft);
    Serial.print(", Front Left: "); Serial.print(sensorData.cliffFrontLeft);
    Serial.print(", Front Right: "); Serial.print(sensorData.cliffFrontRight);
    Serial.print(", Right: "); Serial.println(sensorData.cliffRight);
    
    Serial.print("Wall: "); Serial.println(sensorData.wall);
    Serial.print("Virtual Wall: "); Serial.println(sensorData.virtualWall);
    
    Serial.print("Wheel Drops - Left: "); Serial.print(sensorData.wheelDropLeft);
    Serial.print(", Right: "); Serial.println(sensorData.wheelDropRight);
    
    Serial.println("==================");
}

void RoombaSensors::printSensorDifferences(const SensorData& oldData, const SensorData& newData) {
    if (!_debugEnabled) return;
    
    if (newData.mode != oldData.mode) {
        Serial.print("Mode changed: "); Serial.println(static_cast<uint8_t>(newData.mode));
    }
    if (newData.voltage != oldData.voltage) {
        Serial.print("Voltage: "); Serial.println(newData.voltage);
    }
    if (newData.current != oldData.current) {
        Serial.print("Current: "); Serial.println(newData.current);
    }
    if (newData.batteryCharge != oldData.batteryCharge) {
        Serial.print("Battery Charge: "); Serial.println(newData.batteryCharge);
    }
    if (newData.temperature != oldData.temperature) {
        Serial.print("Temperature: "); Serial.println(newData.temperature);
    }
    if (newData.bumpLeft != oldData.bumpLeft) {
        Serial.print("Bump Left: "); Serial.println(newData.bumpLeft);
    }
    if (newData.bumpRight != oldData.bumpRight) {
        Serial.print("Bump Right: "); Serial.println(newData.bumpRight);
    }
    if (newData.cliffLeft != oldData.cliffLeft) {
        Serial.print("Cliff Left: "); Serial.println(newData.cliffLeft);
    }
    if (newData.cliffFrontLeft != oldData.cliffFrontLeft) {
        Serial.print("Cliff Front Left: "); Serial.println(newData.cliffFrontLeft);
    }
    if (newData.cliffFrontRight != oldData.cliffFrontRight) {
        Serial.print("Cliff Front Right: "); Serial.println(newData.cliffFrontRight);
    }
    if (newData.cliffRight != oldData.cliffRight) {
        Serial.print("Cliff Right: "); Serial.println(newData.cliffRight);
    }
    if (newData.wall != oldData.wall) {
        Serial.print("Wall: "); Serial.println(newData.wall);
    }
    if (newData.wheelDropLeft != oldData.wheelDropLeft) {
        Serial.print("Wheel Drop Left: "); Serial.println(newData.wheelDropLeft);
    }
    if (newData.wheelDropRight != oldData.wheelDropRight) {
        Serial.print("Wheel Drop Right: "); Serial.println(newData.wheelDropRight);
    }
}

// ============================================================================
// INTERNAL PARSING METHODS
// ============================================================================

ErrorCode RoombaSensors::parseStreamBuffer(const uint8_t* buffer, uint8_t bufferSize, SensorData& sensorData) {
    if (buffer == nullptr || bufferSize == 0) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    uint8_t index = 0;
    
    while (index < bufferSize) {
        if (index >= bufferSize) {
            break;
        }
        
        SensorPacket packetId = static_cast<SensorPacket>(buffer[index]);
        index++;
        
        ErrorCode result = parseSensorPacket(packetId, buffer, index, sensorData);
        if (result != ErrorCode::SUCCESS) {
            debugPrint("Failed to parse sensor packet", static_cast<uint8_t>(packetId));
            return result;
        }
    }
    
    return ErrorCode::SUCCESS;
}

uint8_t RoombaSensors::parseOneByte(const uint8_t* buffer, uint8_t& index) {
    uint8_t value = buffer[index];
    index++;
    return value;
}

int16_t RoombaSensors::parseTwoBytes(const uint8_t* buffer, uint8_t& index) {
    int16_t value = (static_cast<int16_t>(buffer[index]) << 8) | buffer[index + 1];
    index += 2;
    return value;
}

uint16_t RoombaSensors::parseTwoBytesUnsigned(const uint8_t* buffer, uint8_t& index) {
    uint16_t value = (static_cast<uint16_t>(buffer[index]) << 8) | buffer[index + 1];
    index += 2;
    return value;
}

void RoombaSensors::parseBitFlags(const uint8_t* buffer, uint8_t& index, bool* flags) {
    uint8_t byte = parseOneByte(buffer, index);
    for (uint8_t i = 0; i < 8; i++) {
        flags[i] = (byte & (1 << i)) != 0;
    }
}

ErrorCode RoombaSensors::parseSensorPacket(SensorPacket packetId, const uint8_t* buffer, 
                                         uint8_t& index, SensorData& sensorData) {
    bool bitFlags[8];
    
    switch (packetId) {
        case SensorPacket::OI_MODE:
            sensorData.mode = static_cast<OIMode>(parseOneByte(buffer, index));
            break;
            
        case SensorPacket::OI_STREAM_NUM_PACKETS:
            sensorData.ioStreamNumPackets = parseOneByte(buffer, index);
            break;
            
        case SensorPacket::SONG_NUMBER:
            sensorData.songNumber = parseOneByte(buffer, index);
            break;
            
        case SensorPacket::IR_OPCODE:
            sensorData.irOpcode = parseOneByte(buffer, index);
            break;
            
        case SensorPacket::IR_OPCODE_LEFT:
            sensorData.infraredCharacterLeft = parseOneByte(buffer, index);
            break;
            
        case SensorPacket::IR_OPCODE_RIGHT:
            sensorData.infraredCharacterRight = parseOneByte(buffer, index);
            break;
            
        case SensorPacket::DIRT_DETECT:
            sensorData.dirtDetect = parseOneByte(buffer, index);
            break;
            
        case SensorPacket::CHARGING_STATE:
            sensorData.chargingState = static_cast<ChargingState>(parseOneByte(buffer, index));
            break;
            
        case SensorPacket::VOLTAGE:
            sensorData.voltage = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::CURRENT:
            sensorData.current = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::VELOCITY:
            sensorData.velocity = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::LEFT_MOTOR_CURRENT:
            sensorData.leftMotorCurrent = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::RIGHT_MOTOR_CURRENT:
            sensorData.rightMotorCurrent = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::MAIN_BRUSH_CURRENT:
            sensorData.mainBrushMotorCurrent = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::SIDE_BRUSH_CURRENT:
            sensorData.sideBrushMotorCurrent = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::VELOCITY_RIGHT:
            sensorData.rightVelocity = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::VELOCITY_LEFT:
            sensorData.leftVelocity = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::RADIUS:
            sensorData.radius = parseTwoBytes(buffer, index);
            break;
            
        case SensorPacket::WALL_SIGNAL:
            sensorData.wallSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::CLIFF_LEFT_SIGNAL:
            sensorData.cliffLeftSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::CLIFF_FRONT_LEFT_SIGNAL:
            sensorData.cliffFrontLeftSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::CLIFF_RIGHT_SIGNAL:
            sensorData.cliffRightSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::CLIFF_FRONT_RIGHT_SIGNAL:
            sensorData.cliffFrontRightSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::LIGHT_BUMP_LEFT_SIGNAL:
            sensorData.lightBumpLeftSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::LIGHT_BUMP_FRONT_LEFT_SIGNAL:
            sensorData.lightBumpFrontLeftSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::LIGHT_BUMP_CENTER_LEFT_SIGNAL:
            sensorData.lightBumpCenterLeftSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::LIGHT_BUMP_CENTER_RIGHT_SIGNAL:
            sensorData.lightBumpCenterRightSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::LIGHT_BUMP_FRONT_RIGHT_SIGNAL:
            sensorData.lightBumpFrontRightSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::LIGHT_BUMP_RIGHT_SIGNAL:
            sensorData.lightBumpRightSignal = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::TEMPERATURE:
            sensorData.temperature = static_cast<int8_t>(parseOneByte(buffer, index));
            break;
            
        case SensorPacket::BATTERY_CHARGE:
            sensorData.batteryCharge = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::ENCODER_COUNTS_LEFT:
            sensorData.leftEncoderCounts = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::ENCODER_COUNTS_RIGHT:
            sensorData.rightEncoderCounts = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::BATTERY_CAPACITY:
            sensorData.batteryCapacity = parseTwoBytesUnsigned(buffer, index);
            break;
            
        case SensorPacket::WALL:
            sensorData.wall = parseOneByte(buffer, index) != 0;
            break;
            
        case SensorPacket::SONG_PLAYING:
            sensorData.songPlaying = parseOneByte(buffer, index) != 0;
            break;
            
        case SensorPacket::VIRTUAL_WALL:
            sensorData.virtualWall = parseOneByte(buffer, index) != 0;
            break;
            
        case SensorPacket::CLIFF_LEFT:
            sensorData.cliffLeft = parseOneByte(buffer, index) != 0;
            break;
            
        case SensorPacket::CLIFF_FRONT_LEFT:
            sensorData.cliffFrontLeft = parseOneByte(buffer, index) != 0;
            break;
            
        case SensorPacket::CLIFF_RIGHT:
            sensorData.cliffRight = parseOneByte(buffer, index) != 0;
            break;
            
        case SensorPacket::CLIFF_FRONT_RIGHT:
            sensorData.cliffFrontRight = parseOneByte(buffer, index) != 0;
            break;
            
        case SensorPacket::BUMPS_WHEEL_DROPS:
            parseBitFlags(buffer, index, bitFlags);
            sensorData.bumpRight = bitFlags[0];
            sensorData.bumpLeft = bitFlags[1];
            sensorData.wheelDropRight = bitFlags[2];
            sensorData.wheelDropLeft = bitFlags[3];
            break;
            
        case SensorPacket::WHEEL_OVERCURRENTS:
            parseBitFlags(buffer, index, bitFlags);
            sensorData.sideBrushOvercurrent = bitFlags[0];
            sensorData.vacuumOvercurrent = bitFlags[1];
            sensorData.mainBrushOvercurrent = bitFlags[2];
            sensorData.wheelRightOvercurrent = bitFlags[3];
            sensorData.wheelLeftOvercurrent = bitFlags[4];
            break;
            
        case SensorPacket::BUTTONS:
            parseBitFlags(buffer, index, bitFlags);
            sensorData.cleanButton = bitFlags[0];
            sensorData.spotButton = bitFlags[1];
            sensorData.dockButton = bitFlags[2];
            sensorData.minuteButton = bitFlags[3];
            sensorData.hourButton = bitFlags[4];
            sensorData.dayButton = bitFlags[5];
            sensorData.scheduleButton = bitFlags[6];
            sensorData.clockButton = bitFlags[7];
            break;
            
        case SensorPacket::LIGHT_BUMPER:
            parseBitFlags(buffer, index, bitFlags);
            sensorData.lightBumperLeft = bitFlags[0];
            sensorData.lightBumperFrontLeft = bitFlags[1];
            sensorData.lightBumperCenterLeft = bitFlags[2];
            sensorData.lightBumperCenterRight = bitFlags[3];
            sensorData.lightBumperFrontRight = bitFlags[4];
            sensorData.lightBumperRight = bitFlags[5];
            break;
            
        case SensorPacket::CHARGER_AVAILABLE:
            parseBitFlags(buffer, index, bitFlags);
            sensorData.internalChargerAvailable = bitFlags[0];
            sensorData.homeBaseChargerAvailable = bitFlags[1];
            break;
            
        case SensorPacket::STASIS:
            parseBitFlags(buffer, index, bitFlags);
            sensorData.stasisToggling = bitFlags[0];
            sensorData.stasisDisabled = bitFlags[1];
            break;
            
        default:
            debugPrint("Unhandled sensor packet ID", static_cast<uint8_t>(packetId));
            return ErrorCode::INVALID_PARAMETER;
    }
    
    return ErrorCode::SUCCESS;
}

void RoombaSensors::debugPrint(const char* message) {
    if (_debugEnabled && message != nullptr) {
        Serial.print("ArduRoombaSensors: ");
        Serial.println(message);
    }
}

void RoombaSensors::debugPrint(const char* message, int value) {
    if (_debugEnabled && message != nullptr) {
        Serial.print("ArduRoombaSensors: ");
        Serial.print(message);
        Serial.print(" = ");
        Serial.println(value);
    }
}

} // namespace ArduRoomba