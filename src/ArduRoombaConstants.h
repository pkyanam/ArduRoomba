/**
 * @file ArduRoombaConstants.h
 * @brief Constants and enumerations for the ArduRoomba library
 * 
 * This file contains all the constants, magic numbers, and enumerations
 * used throughout the ArduRoomba library. It provides a centralized
 * location for all protocol-specific values and configuration parameters.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBACONSTANTS_H
#define ARDUROOMBACONSTANTS_H

#include <Arduino.h>

namespace ArduRoomba {

// ============================================================================
// TIMING CONSTANTS
// ============================================================================

/** @brief Default refresh delay between sensor readings (ms) */
static const uint8_t REFRESH_DELAY = 40;

/** @brief Stream timeout for sensor data reception (ms) */
static const uint8_t STREAM_TIMEOUT = 16;

/** @brief Default baud rate for iRobot communication */
static const uint32_t DEFAULT_BAUD_RATE = 19200;

/** @brief Setup delay after power on (ms) */
static const uint16_t POWER_ON_DELAY = 2000;

/** @brief BRC pulse duration (ms) */
static const uint8_t BRC_PULSE_DURATION = 100;

/** @brief Number of BRC pulses for initialization */
static const uint8_t BRC_PULSE_COUNT = 3;

// ============================================================================
// STREAM STATE MACHINE
// ============================================================================

/** @brief Stream states for data reception state machine */
enum class StreamState : uint8_t {
    WAIT_HEADER = 0,    ///< Waiting for stream header (19)
    WAIT_SIZE = 1,      ///< Waiting for data size byte
    WAIT_CONTENT = 2,   ///< Waiting for content data
    WAIT_CHECKSUM = 3,  ///< Waiting for checksum byte
    END = 4             ///< Stream reception complete
};

/** @brief Stream header byte value */
static const uint8_t STREAM_HEADER = 19;

// ============================================================================
// OPEN INTERFACE OPCODES
// ============================================================================

/** @brief iRobot Open Interface command opcodes */
enum class OIOpcode : uint8_t {
    START = 128,        ///< Start the OI
    BAUD = 129,         ///< Change baud rate
    CONTROL = 130,      ///< Deprecated (use SAFE or FULL)
    SAFE = 131,         ///< Put OI into Safe mode
    FULL = 132,         ///< Put OI into Full mode
    POWER = 133,        ///< Power down the OI
    SPOT = 134,         ///< Start spot cleaning
    CLEAN = 135,        ///< Start cleaning
    MAX_CLEAN = 136,    ///< Start max time cleaning
    DRIVE = 137,        ///< Control wheels directly
    MOTORS = 138,       ///< Control cleaning motors
    LEDS = 139,         ///< Control LEDs
    SONG = 140,         ///< Define a song
    PLAY = 141,         ///< Play a song
    SENSORS = 142,      ///< Request sensor data
    SEEK_DOCK = 143,    ///< Seek the dock
    PWM_MOTORS = 144,   ///< Control motor PWM
    DRIVE_DIRECT = 145, ///< Control wheels independently
    DRIVE_PWM = 146,    ///< Control wheels with PWM
    STREAM = 148,       ///< Request sensor stream
    QUERY_LIST = 149,   ///< Query list of sensor packets
    SCHEDULING_LEDS = 162, ///< Control scheduling LEDs
    DIGIT_LEDS_RAW = 163,  ///< Control digit LEDs
    SET_DAY_TIME = 168,    ///< Set day and time
    SCHEDULE = 167         ///< Set cleaning schedule
};

// ============================================================================
// SENSOR PACKET IDs
// ============================================================================

/** @brief Sensor packet identifiers for the iRobot Open Interface */
enum class SensorPacket : uint8_t {
    // Group packets
    GROUP_0 = 0,        ///< Sensor group 0 (7-26)
    GROUP_1 = 1,        ///< Sensor group 1 (7-16)
    GROUP_2 = 2,        ///< Sensor group 2 (17-20)
    GROUP_3 = 3,        ///< Sensor group 3 (21-26)
    GROUP_4 = 4,        ///< Sensor group 4 (27-34)
    GROUP_5 = 5,        ///< Sensor group 5 (35-42)
    GROUP_6 = 6,        ///< Sensor group 6 (7-42)
    GROUP_100 = 100,    ///< Sensor group 100 (7-58)
    GROUP_101 = 101,    ///< Sensor group 101 (43-58)
    GROUP_106 = 106,    ///< Sensor group 106 (46-51)
    GROUP_107 = 107,    ///< Sensor group 107 (54-58)

    // Individual sensor packets
    BUMPS_WHEEL_DROPS = 7,          ///< Bumper and wheel drop sensors
    WALL = 8,                       ///< Wall sensor
    CLIFF_LEFT = 9,                 ///< Cliff sensor left
    CLIFF_FRONT_LEFT = 10,          ///< Cliff sensor front left
    CLIFF_FRONT_RIGHT = 11,         ///< Cliff sensor front right
    CLIFF_RIGHT = 12,               ///< Cliff sensor right
    VIRTUAL_WALL = 13,              ///< Virtual wall sensor
    WHEEL_OVERCURRENTS = 14,        ///< Wheel overcurrent sensors
    DIRT_DETECT = 15,               ///< Dirt detect sensor
    UNUSED_1 = 16,                  ///< Unused byte
    IR_OPCODE = 17,                 ///< Infrared character omni
    BUTTONS = 18,                   ///< Buttons
    DISTANCE = 19,                  ///< Distance traveled
    ANGLE = 20,                     ///< Angle turned
    CHARGING_STATE = 21,            ///< Charging state
    VOLTAGE = 22,                   ///< Battery voltage
    CURRENT = 23,                   ///< Battery current
    TEMPERATURE = 24,               ///< Battery temperature
    BATTERY_CHARGE = 25,            ///< Battery charge
    BATTERY_CAPACITY = 26,          ///< Battery capacity
    WALL_SIGNAL = 27,               ///< Wall signal strength
    CLIFF_LEFT_SIGNAL = 28,         ///< Cliff left signal strength
    CLIFF_FRONT_LEFT_SIGNAL = 29,   ///< Cliff front left signal strength
    CLIFF_FRONT_RIGHT_SIGNAL = 30,  ///< Cliff front right signal strength
    CLIFF_RIGHT_SIGNAL = 31,        ///< Cliff right signal strength
    UNUSED_2 = 32,                  ///< Unused bytes
    UNUSED_3 = 33,                  ///< Unused bytes
    CHARGER_AVAILABLE = 34,         ///< Charger available
    OI_MODE = 35,                   ///< Open Interface mode
    SONG_NUMBER = 36,               ///< Current song number
    SONG_PLAYING = 37,              ///< Song playing flag
    OI_STREAM_NUM_PACKETS = 38,     ///< Number of stream packets
    VELOCITY = 39,                  ///< Requested velocity
    RADIUS = 40,                    ///< Requested radius
    VELOCITY_RIGHT = 41,            ///< Right wheel velocity
    VELOCITY_LEFT = 42,             ///< Left wheel velocity
    ENCODER_COUNTS_LEFT = 43,       ///< Left encoder counts
    ENCODER_COUNTS_RIGHT = 44,      ///< Right encoder counts
    LIGHT_BUMPER = 45,              ///< Light bumper
    LIGHT_BUMP_LEFT_SIGNAL = 46,    ///< Light bump left signal
    LIGHT_BUMP_FRONT_LEFT_SIGNAL = 47,   ///< Light bump front left signal
    LIGHT_BUMP_CENTER_LEFT_SIGNAL = 48,  ///< Light bump center left signal
    LIGHT_BUMP_CENTER_RIGHT_SIGNAL = 49, ///< Light bump center right signal
    LIGHT_BUMP_FRONT_RIGHT_SIGNAL = 50,  ///< Light bump front right signal
    LIGHT_BUMP_RIGHT_SIGNAL = 51,        ///< Light bump right signal
    IR_OPCODE_LEFT = 52,            ///< Infrared character left
    IR_OPCODE_RIGHT = 53,           ///< Infrared character right
    LEFT_MOTOR_CURRENT = 54,        ///< Left motor current
    RIGHT_MOTOR_CURRENT = 55,       ///< Right motor current
    MAIN_BRUSH_CURRENT = 56,        ///< Main brush motor current
    SIDE_BRUSH_CURRENT = 57,        ///< Side brush motor current
    STASIS = 58                     ///< Stasis sensor
};

// ============================================================================
// ROBOT MODES
// ============================================================================

/** @brief iRobot Open Interface modes */
enum class OIMode : uint8_t {
    OFF = 0,        ///< OI not started
    PASSIVE = 1,    ///< Passive mode
    SAFE = 2,       ///< Safe mode
    FULL = 3        ///< Full mode
};

// ============================================================================
// CHARGING STATES
// ============================================================================

/** @brief Battery charging states */
enum class ChargingState : uint8_t {
    NOT_CHARGING = 0,           ///< Not charging
    RECONDITIONING_CHARGING = 1, ///< Reconditioning charging
    FULL_CHARGING = 2,          ///< Full charging
    TRICKLE_CHARGING = 3,       ///< Trickle charging
    WAITING = 4,                ///< Waiting
    CHARGING_FAULT_CONDITION = 5 ///< Charging fault condition
};

// ============================================================================
// DRIVE CONSTANTS
// ============================================================================

/** @brief Drive velocity limits (mm/s) */
namespace DriveVelocity {
    static const int16_t MAX_FORWARD = 500;    ///< Maximum forward velocity
    static const int16_t MAX_BACKWARD = -500;  ///< Maximum backward velocity
    static const int16_t STOP = 0;             ///< Stop velocity
    static const int16_t DEFAULT_TURN = 200;   ///< Default turning velocity
}

/** @brief Drive radius constants (mm) */
namespace DriveRadius {
    static const int16_t STRAIGHT = 32768;     ///< Drive straight
    static const int16_t TURN_IN_PLACE_CW = -1;    ///< Turn in place clockwise
    static const int16_t TURN_IN_PLACE_CCW = 1;    ///< Turn in place counter-clockwise
}

// ============================================================================
// MOTOR CONTROL
// ============================================================================

/** @brief Motor control bit masks */
namespace MotorBits {
    static const uint8_t SIDE_BRUSH = 0x01;    ///< Side brush motor
    static const uint8_t VACUUM = 0x02;        ///< Vacuum motor
    static const uint8_t MAIN_BRUSH = 0x04;    ///< Main brush motor
    static const uint8_t SIDE_BRUSH_OPPOSITE = 0x08; ///< Side brush opposite direction
    static const uint8_t MAIN_BRUSH_OPPOSITE = 0x10; ///< Main brush opposite direction
}

// ============================================================================
// LED CONTROL
// ============================================================================

/** @brief LED control bit masks */
namespace LEDBits {
    static const uint8_t CHECK_ROBOT = 0x08;   ///< Check robot LED
    static const uint8_t DOCK = 0x04;          ///< Dock LED
    static const uint8_t SPOT = 0x02;          ///< Spot LED
    static const uint8_t DEBRIS = 0x01;        ///< Debris LED
}

/** @brief Power LED color values */
namespace PowerLEDColor {
    static const uint8_t GREEN = 0;     ///< Green color
    static const uint8_t RED = 255;     ///< Red color
}

// ============================================================================
// SONG CONSTANTS
// ============================================================================

/** @brief Song and note constraints */
namespace Song {
    static const uint8_t MAX_SONGS = 4;        ///< Maximum number of songs
    static const uint8_t MAX_NOTES = 16;       ///< Maximum notes per song
    static const uint8_t MIN_NOTE = 31;        ///< Minimum MIDI note number
    static const uint8_t MAX_NOTE = 127;       ///< Maximum MIDI note number
    static const uint8_t MAX_DURATION = 255;   ///< Maximum note duration (1/64 seconds)
}

// ============================================================================
// BUFFER SIZES
// ============================================================================

/** @brief Buffer size constants */
namespace BufferSize {
    static const uint8_t STREAM_BUFFER = 100;  ///< Stream data buffer size
    static const uint8_t SENSOR_LIST = 60;     ///< Maximum sensors in stream list
    static const uint8_t SENSOR_READ = 64;     ///< Sensor read buffer size
}

// ============================================================================
// ERROR CODES
// ============================================================================

/** @brief Error codes for library operations */
enum class ErrorCode : uint8_t {
    SUCCESS = 0,            ///< Operation successful
    TIMEOUT = 1,            ///< Communication timeout
    CHECKSUM_ERROR = 2,     ///< Checksum validation failed
    INVALID_PARAMETER = 3,  ///< Invalid parameter provided
    BUFFER_OVERFLOW = 4,    ///< Buffer overflow detected
    COMMUNICATION_ERROR = 5, ///< General communication error
    NOT_INITIALIZED = 6,    ///< Library not properly initialized
    UNKNOWN_ERROR = 255     ///< Unknown error occurred
};

} // namespace ArduRoomba

// ============================================================================
// LEGACY COMPATIBILITY DEFINES
// ============================================================================
// These are maintained for backward compatibility with existing code

#define ARDUROOMBA_REFRESH_DELAY ArduRoomba::REFRESH_DELAY
#define ARDUROOMBA_STREAM_TIMEOUT ArduRoomba::STREAM_TIMEOUT

#define ARDUROOMBACONSTANTS_HEADER static_cast<uint8_t>(ArduRoomba::StreamState::WAIT_HEADER)
#define ARDUROOMBA_STREAM_WAIT_SIZE static_cast<uint8_t>(ArduRoomba::StreamState::WAIT_SIZE)
#define ARDUROOMBA_STREAM_WAIT_CONTENT static_cast<uint8_t>(ArduRoomba::StreamState::WAIT_CONTENT)
#define ARDUROOMBA_STREAM_WAIT_CHECKSUM static_cast<uint8_t>(ArduRoomba::StreamState::WAIT_CHECKSUM)
#define ARDUROOMBA_STREAM_END static_cast<uint8_t>(ArduRoomba::StreamState::END)

#define ARDUROOMBA_SENSOR_BUMPANDWEELSDROPS static_cast<uint8_t>(ArduRoomba::SensorPacket::BUMPS_WHEEL_DROPS)
#define ARDUROOMBA_SENSOR_WALL static_cast<uint8_t>(ArduRoomba::SensorPacket::WALL)
#define ARDUROOMBA_SENSOR_CLIFFLEFT static_cast<uint8_t>(ArduRoomba::SensorPacket::CLIFF_LEFT)
#define ARDUROOMBA_SENSOR_CLIFFFRONTLEFT static_cast<uint8_t>(ArduRoomba::SensorPacket::CLIFF_FRONT_LEFT)
#define ARDUROOMBA_SENSOR_CLIFFFRONTRIGHT static_cast<uint8_t>(ArduRoomba::SensorPacket::CLIFF_FRONT_RIGHT)
#define ARDUROOMBA_SENSOR_CLIFFRIGHT static_cast<uint8_t>(ArduRoomba::SensorPacket::CLIFF_RIGHT)
#define ARDUROOMBA_SENSOR_VIRTUALWALL static_cast<uint8_t>(ArduRoomba::SensorPacket::VIRTUAL_WALL)
#define ARDUROOMBA_SENSOR_WHEELOVERCURRENTS static_cast<uint8_t>(ArduRoomba::SensorPacket::WHEEL_OVERCURRENTS)
#define ARDUROOMBA_SENSOR_DIRTDETECT static_cast<uint8_t>(ArduRoomba::SensorPacket::DIRT_DETECT)
#define ARDUROOMBA_SENSOR_IROPCODE static_cast<uint8_t>(ArduRoomba::SensorPacket::IR_OPCODE)
#define ARDUROOMBA_SENSOR_BUTTONS static_cast<uint8_t>(ArduRoomba::SensorPacket::BUTTONS)
#define ARDUROOMBA_SENSOR_CHARGINGSTATE static_cast<uint8_t>(ArduRoomba::SensorPacket::CHARGING_STATE)
#define ARDUROOMBA_SENSOR_VOLTAGE static_cast<uint8_t>(ArduRoomba::SensorPacket::VOLTAGE)
#define ARDUROOMBA_SENSOR_CURRENT static_cast<uint8_t>(ArduRoomba::SensorPacket::CURRENT)
#define ARDUROOMBA_SENSOR_TEMPERATURE static_cast<uint8_t>(ArduRoomba::SensorPacket::TEMPERATURE)
#define ARDUROOMBA_SENSOR_BATTERYCHARGE static_cast<uint8_t>(ArduRoomba::SensorPacket::BATTERY_CHARGE)
#define ARDUROOMBA_SENSOR_BATTERYCAPACITY static_cast<uint8_t>(ArduRoomba::SensorPacket::BATTERY_CAPACITY)
#define ARDUROOMBA_SENSOR_WALLSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::WALL_SIGNAL)
#define ARDUROOMBA_SENSOR_CLIFFLEFTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::CLIFF_LEFT_SIGNAL)
#define ARDUROOMBA_SENSOR_CLIFFFRONTLEFTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::CLIFF_FRONT_LEFT_SIGNAL)
#define ARDUROOMBA_SENSOR_CLIFFFRONTRIGHTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::CLIFF_FRONT_RIGHT_SIGNAL)
#define ARDUROOMBA_SENSOR_CLIFFRIGHTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::CLIFF_RIGHT_SIGNAL)
#define ARDUROOMBA_SENSOR_CHARGERAVAILABLE static_cast<uint8_t>(ArduRoomba::SensorPacket::CHARGER_AVAILABLE)
#define ARDUROOMBA_SENSOR_MODE static_cast<uint8_t>(ArduRoomba::SensorPacket::OI_MODE)
#define ARDUROOMBA_SENSOR_SONGNUMBER static_cast<uint8_t>(ArduRoomba::SensorPacket::SONG_NUMBER)
#define ARDUROOMBA_SENSOR_SONGPLAYING static_cast<uint8_t>(ArduRoomba::SensorPacket::SONG_PLAYING)
#define ARDUROOMBA_SENSOR_IOSTREAMNUMPACKETS static_cast<uint8_t>(ArduRoomba::SensorPacket::OI_STREAM_NUM_PACKETS)
#define ARDUROOMBA_SENSOR_VELOCITY static_cast<uint8_t>(ArduRoomba::SensorPacket::VELOCITY)
#define ARDUROOMBA_SENSOR_RADIUS static_cast<uint8_t>(ArduRoomba::SensorPacket::RADIUS)
#define ARDUROOMBA_SENSOR_RIGHTVELOCITY static_cast<uint8_t>(ArduRoomba::SensorPacket::VELOCITY_RIGHT)
#define ARDUROOMBA_SENSOR_LEFTVELOCITY static_cast<uint8_t>(ArduRoomba::SensorPacket::VELOCITY_LEFT)
#define ARDUROOMBA_SENSOR_LEFTENCODERCOUNTS static_cast<uint8_t>(ArduRoomba::SensorPacket::ENCODER_COUNTS_LEFT)
#define ARDUROOMBA_SENSOR_RIGHTENCODERCOUNTS static_cast<uint8_t>(ArduRoomba::SensorPacket::ENCODER_COUNTS_RIGHT)
#define ARDUROOMBA_SENSOR_LIGHTBUMPER static_cast<uint8_t>(ArduRoomba::SensorPacket::LIGHT_BUMPER)
#define ARDUROOMBA_SENSOR_LIGHTBUMPLEFTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::LIGHT_BUMP_LEFT_SIGNAL)
#define ARDUROOMBA_SENSOR_LIGHTBUMPFRONTLEFTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::LIGHT_BUMP_FRONT_LEFT_SIGNAL)
#define ARDUROOMBA_SENSOR_LIGHTBUMPCENTERLEFTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::LIGHT_BUMP_CENTER_LEFT_SIGNAL)
#define ARDUROOMBA_SENSOR_LIGHTBUMPCENTERRIGHTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::LIGHT_BUMP_CENTER_RIGHT_SIGNAL)
#define ARDUROOMBA_SENSOR_LIGHTBUMPFRONTRIGHTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::LIGHT_BUMP_FRONT_RIGHT_SIGNAL)
#define ARDUROOMBA_SENSOR_LIGHTBUMPRIGHTSIGNAL static_cast<uint8_t>(ArduRoomba::SensorPacket::LIGHT_BUMP_RIGHT_SIGNAL)
#define ARDUROOMBA_SENSOR_INFRAREDCHARACTERLEFT static_cast<uint8_t>(ArduRoomba::SensorPacket::IR_OPCODE_LEFT)
#define ARDUROOMBA_SENSOR_INFRAREDCHARACTERRIGHT static_cast<uint8_t>(ArduRoomba::SensorPacket::IR_OPCODE_RIGHT)
#define ARDUROOMBA_SENSOR_LEFTMOTORCURRENT static_cast<uint8_t>(ArduRoomba::SensorPacket::LEFT_MOTOR_CURRENT)
#define ARDUROOMBA_SENSOR_RIGHTMOTORCURRENT static_cast<uint8_t>(ArduRoomba::SensorPacket::RIGHT_MOTOR_CURRENT)
#define ARDUROOMBA_SENSOR_MAINBRUSHMOTORCURRENT static_cast<uint8_t>(ArduRoomba::SensorPacket::MAIN_BRUSH_CURRENT)
#define ARDUROOMBA_SENSOR_SIDEBRUSHMOTORCURRENT static_cast<uint8_t>(ArduRoomba::SensorPacket::SIDE_BRUSH_CURRENT)
#define ARDUROOMBA_SENSOR_STASIS static_cast<uint8_t>(ArduRoomba::SensorPacket::STASIS)

#endif // ARDUROOMBACONSTANTS_H