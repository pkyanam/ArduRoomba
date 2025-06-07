/**
 * @file ArduRoomba.cpp
 * @brief Implementation of the main ArduRoomba class
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#include "ArduRoomba.h"

namespace ArduRoomba {

// ============================================================================
// CONSTRUCTOR AND DESTRUCTOR
// ============================================================================

ArduRoomba::ArduRoomba(uint8_t rxPin, uint8_t txPin, uint8_t brcPin)
    : _core(rxPin, txPin, brcPin), _sensors(_core), _commands(_core),
      _debugEnabled(false), _lastError(ErrorCode::SUCCESS) {
}

ArduRoomba::~ArduRoomba() {
    // Destructor - components handle their own cleanup
}

// ============================================================================
// INITIALIZATION
// ============================================================================

ErrorCode ArduRoomba::initialize(uint32_t baudRate) {
    ErrorCode result = _core.initialize(baudRate);
    updateLastError(result);
    return result;
}

void ArduRoomba::roombaSetup() {
    // Legacy method - calls new initialize method but ignores error code
    initialize();
}

bool ArduRoomba::isInitialized() const {
    return _core.isInitialized();
}

// ============================================================================
// OPEN INTERFACE COMMANDS (Legacy compatibility)
// ============================================================================

void ArduRoomba::start() {
    updateLastError(_commands.start());
}

void ArduRoomba::baud(char baudCode) {
    // Convert legacy baud code to actual baud rate
    uint32_t baudRate;
    switch (baudCode) {
        case 0: baudRate = 300; break;
        case 1: baudRate = 600; break;
        case 2: baudRate = 1200; break;
        case 3: baudRate = 2400; break;
        case 4: baudRate = 4800; break;
        case 5: baudRate = 9600; break;
        case 6: baudRate = 14400; break;
        case 7: baudRate = 19200; break;
        case 8: baudRate = 28800; break;
        case 9: baudRate = 38400; break;
        case 10: baudRate = 57600; break;
        case 11: baudRate = 115200; break;
        default: baudRate = 19200; break;
    }
    updateLastError(_commands.changeBaudRate(baudRate));
}

void ArduRoomba::safe() {
    updateLastError(_commands.safeMode());
}

void ArduRoomba::full() {
    updateLastError(_commands.fullMode());
}

void ArduRoomba::clean() {
    updateLastError(_commands.startCleaning());
}

void ArduRoomba::maxClean() {
    updateLastError(_commands.startMaxCleaning());
}

void ArduRoomba::spot() {
    updateLastError(_commands.startSpotCleaning());
}

void ArduRoomba::seekDock() {
    updateLastError(_commands.seekDock());
}

void ArduRoomba::schedule(ScheduleStore scheduleData) {
    updateLastError(_commands.setSchedule(scheduleData));
}

void ArduRoomba::setDayTime(char day, char hour, char minute) {
    updateLastError(_commands.setDayTime(day, hour, minute));
}

void ArduRoomba::power() {
    updateLastError(_commands.powerDown());
}

// ============================================================================
// ACTUATOR COMMANDS (Legacy compatibility)
// ============================================================================

void ArduRoomba::drive(int velocity, int radius) {
    updateLastError(_commands.drive(velocity, radius));
}

void ArduRoomba::driveDirect(int rightVelocity, int leftVelocity) {
    updateLastError(_commands.driveDirect(rightVelocity, leftVelocity));
}

void ArduRoomba::drivePWM(int rightPWM, int leftPWM) {
    updateLastError(_commands.drivePWM(rightPWM, leftPWM));
}

void ArduRoomba::motors(byte data) {
    bool sideBrush = (data & 0x01) != 0;
    bool vacuum = (data & 0x02) != 0;
    bool mainBrush = (data & 0x04) != 0;
    bool sideBrushDirection = (data & 0x08) != 0;
    bool mainBrushDirection = (data & 0x10) != 0;
    
    updateLastError(_commands.setMotors(sideBrush, vacuum, mainBrush, 
                                       sideBrushDirection, mainBrushDirection));
}

void ArduRoomba::pwmMotors(char mainBrushPWM, char sideBrushPWM, char vacuumPWM) {
    updateLastError(_commands.setMotorsPWM(mainBrushPWM, sideBrushPWM, vacuumPWM));
}

void ArduRoomba::leds(int ledBits, int powerColor, int powerIntensity) {
    bool checkRobot = (ledBits & 0x08) != 0;
    bool dock = (ledBits & 0x04) != 0;
    bool spot = (ledBits & 0x02) != 0;
    bool debris = (ledBits & 0x01) != 0;
    
    updateLastError(_commands.setLEDs(checkRobot, dock, spot, debris, 
                                     powerColor, powerIntensity));
}

void ArduRoomba::schedulingLeds(int weekDayLedBits, int scheduleLedBits) {
    updateLastError(_commands.setSchedulingLEDs(weekDayLedBits, scheduleLedBits));
}

void ArduRoomba::digitLedsRaw(int digitThree, int digitTwo, int digitOne, int digitZero) {
    updateLastError(_commands.setDigitLEDs(digitThree, digitTwo, digitOne, digitZero));
}

void ArduRoomba::song(Song songData) {
    updateLastError(_commands.defineSong(songData));
}

void ArduRoomba::play(int songNumber) {
    updateLastError(_commands.playSong(songNumber));
}

// ============================================================================
// INPUT COMMANDS (Legacy compatibility)
// ============================================================================

void ArduRoomba::sensors(char packetID) {
    uint8_t buffer[64];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = _sensors.requestSensor(static_cast<SensorPacket>(packetID), 
                                             buffer, &bufferSize);
    updateLastError(result);
    
    // Print data to Serial for legacy compatibility
    if (result == ErrorCode::SUCCESS) {
        Serial.print("Packet ID: ");
        Serial.print(packetID, DEC);
        Serial.print(", Data: ");
        for (uint8_t i = 0; i < bufferSize; i++) {
            Serial.print(buffer[i], DEC);
            Serial.print(" ");
        }
        Serial.println();
    }
}

void ArduRoomba::queryList(byte numPackets, byte *packetIDs) {
    if (packetIDs == nullptr || numPackets == 0) {
        updateLastError(ErrorCode::INVALID_PARAMETER);
        return;
    }
    
    // Convert byte array to SensorPacket array
    SensorPacket packets[numPackets];
    for (byte i = 0; i < numPackets; i++) {
        packets[i] = static_cast<SensorPacket>(packetIDs[i]);
    }
    
    uint8_t buffer[64];
    uint8_t bufferSize = sizeof(buffer);
    
    ErrorCode result = _sensors.requestSensors(packets, numPackets, buffer, &bufferSize);
    updateLastError(result);
    
    // Print data to Serial for legacy compatibility
    if (result == ErrorCode::SUCCESS) {
        for (byte i = 0; i < numPackets; i++) {
            Serial.print("Packet ID: ");
            Serial.print(packetIDs[i], DEC);
            Serial.print(", Data: ");
            // Note: This is simplified - actual parsing would be more complex
            Serial.println();
        }
    }
}

void ArduRoomba::queryStream(char sensorlist[]) {
    if (sensorlist == nullptr) {
        updateLastError(ErrorCode::INVALID_PARAMETER);
        return;
    }
    
    SensorPacket newSensorList[BufferSize::SENSOR_LIST];
    uint8_t sensorCount = convertLegacySensorList(sensorlist, newSensorList, BufferSize::SENSOR_LIST);
    
    if (sensorCount > 0) {
        updateLastError(_sensors.startStreaming(newSensorList, sensorCount));
    } else {
        updateLastError(ErrorCode::INVALID_PARAMETER);
    }
}

void ArduRoomba::resetStream() {
    updateLastError(_sensors.stopStreaming());
}

bool ArduRoomba::refreshData(RoombaInfos *infos) {
    if (infos == nullptr) {
        updateLastError(ErrorCode::INVALID_PARAMETER);
        return false;
    }
    
    ErrorCode result = _sensors.updateFromStream(*infos);
    updateLastError(result);
    return result == ErrorCode::SUCCESS;
}

// ============================================================================
// CUSTOM MOVEMENT COMMANDS (Legacy compatibility)
// ============================================================================

void ArduRoomba::goForward() {
    updateLastError(_commands.moveForward(500));
}

void ArduRoomba::goBackward() {
    updateLastError(_commands.moveBackward(500));
}

void ArduRoomba::turnLeft() {
    updateLastError(_commands.turnLeft(200));
}

void ArduRoomba::turnRight() {
    updateLastError(_commands.turnRight(200));
}

void ArduRoomba::halt() {
    updateLastError(_commands.stop());
}

// ============================================================================
// ENHANCED MOVEMENT METHODS
// ============================================================================

ErrorCode ArduRoomba::moveForward(int16_t velocity) {
    ErrorCode result = _commands.moveForward(velocity);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::moveBackward(int16_t velocity) {
    ErrorCode result = _commands.moveBackward(velocity);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::turnLeftInPlace(int16_t velocity) {
    ErrorCode result = _commands.turnLeft(velocity);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::turnRightInPlace(int16_t velocity) {
    ErrorCode result = _commands.turnRight(velocity);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::stopMovement() {
    ErrorCode result = _commands.stop();
    updateLastError(result);
    return result;
}

// ============================================================================
// ENHANCED SENSOR METHODS
// ============================================================================

ErrorCode ArduRoomba::getAllSensorData(SensorData& sensorData) {
    ErrorCode result = _sensors.getAllSensorData(sensorData);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::getBasicSensorData(SensorData& sensorData) {
    ErrorCode result = _sensors.getBasicSensorData(sensorData);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::startSensorStream(RoombaSensors::SensorPreset preset) {
    ErrorCode result = _sensors.startStreaming(preset);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::updateSensorData(SensorData& sensorData) {
    ErrorCode result = _sensors.updateFromStream(sensorData);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::stopSensorStream() {
    ErrorCode result = _sensors.stopStreaming();
    updateLastError(result);
    return result;
}

// ============================================================================
// ENHANCED LED AND SOUND METHODS
// ============================================================================

ErrorCode ArduRoomba::setPowerLED(uint8_t color, uint8_t intensity) {
    ErrorCode result = _commands.setPowerLED(color, intensity);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::beep(uint8_t frequency, uint8_t duration) {
    ErrorCode result = _commands.beep(frequency, duration);
    updateLastError(result);
    return result;
}

ErrorCode ArduRoomba::beepSequence(uint8_t count, uint8_t frequency, uint8_t duration) {
    ErrorCode result = _commands.beepSequence(count, frequency, duration);
    updateLastError(result);
    return result;
}

// ============================================================================
// UTILITY AND DIAGNOSTIC METHODS
// ============================================================================

void ArduRoomba::setDebugEnabled(bool enabled) {
    _debugEnabled = enabled;
    _core.setDebugEnabled(enabled);
    _sensors.setDebugEnabled(enabled);
    _commands.setDebugEnabled(enabled);
}

bool ArduRoomba::isDebugEnabled() const {
    return _debugEnabled;
}

ErrorCode ArduRoomba::getLastError() const {
    return _lastError;
}

void ArduRoomba::getStatistics(uint32_t& bytesSent, uint32_t& bytesReceived, 
                              uint32_t& commandsSent, uint16_t& errors) const {
    uint16_t coreErrors, commandErrors;
    _core.getStatistics(bytesSent, bytesReceived, coreErrors);
    _commands.getStatistics(commandsSent, commandErrors);
    errors = coreErrors + commandErrors;
}

void ArduRoomba::resetStatistics() {
    _core.resetStatistics();
    _commands.resetStatistics();
}

void ArduRoomba::printSensorData(const SensorData& sensorData) {
    _sensors.printSensorData(sensorData);
}

// ============================================================================
// INTERNAL HELPER METHODS
// ============================================================================

void ArduRoomba::updateLastError(ErrorCode error) {
    if (error != ErrorCode::SUCCESS) {
        _lastError = error;
    }
}

uint8_t ArduRoomba::convertLegacySensorList(char legacySensorList[], SensorPacket* newSensorList, uint8_t maxSensors) {
    if (legacySensorList == nullptr || newSensorList == nullptr || maxSensors == 0) {
        return 0;
    }
    
    uint8_t count = 0;
    int length = getLegacySensorListLength(legacySensorList);
    
    for (int i = 0; i < length && count < maxSensors; i++) {
        newSensorList[count] = static_cast<SensorPacket>(legacySensorList[i]);
        count++;
    }
    
    return count;
}

int ArduRoomba::getLegacySensorListLength(char sensorList[]) {
    if (sensorList == nullptr) {
        return 0;
    }
    
    int count = 0;
    for (int i = 0; sensorList[i] != '\0'; i++) {
        count++;
    }
    return count;
}

} // namespace ArduRoomba
