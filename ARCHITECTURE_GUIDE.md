# ArduRoomba Architecture Guide

## Overview

ArduRoomba is a modular Arduino library implementing a comprehensive robotics platform for iRobot Create 2 and compatible Roomba models. The architecture follows modern software engineering principles with clear separation of concerns, robust error handling, and extensible design patterns.

## Architecture Principles

### 1. Modular Design
- **Separation of Concerns**: Each module handles a specific aspect of robot functionality
- **Loose Coupling**: Modules interact through well-defined interfaces
- **High Cohesion**: Related functionality is grouped together
- **Dependency Injection**: Components receive dependencies rather than creating them

### 2. Layered Architecture
```
┌─────────────────────────────────────────────────────────┐
│                 Application Layer                       │
│              (User Code & Examples)                     │
├─────────────────────────────────────────────────────────┤
│                   Facade Layer                          │
│                 (ArduRoomba Class)                      │
├─────────────────────────────────────────────────────────┤
│                 Service Layer                           │
│     (Sensors, Commands, Config, WiFi Modules)          │
├─────────────────────────────────────────────────────────┤
│                 Core Layer                              │
│            (Communication & Protocol)                   │
├─────────────────────────────────────────────────────────┤
│                Foundation Layer                         │
│           (Types, Constants, Utilities)                 │
└─────────────────────────────────────────────────────────┘
```

### 3. Error Handling Strategy
- **Explicit Error Codes**: All operations return meaningful error codes
- **Error Propagation**: Errors bubble up through the layers
- **Graceful Degradation**: System continues operating when possible
- **Debug Support**: Comprehensive logging for troubleshooting

## Core Components

### 1. Foundation Layer

#### ArduRoombaTypes.h
**Purpose**: Defines all data types, enums, and structures used throughout the library.

**Key Types**:
- `ErrorCode`: Comprehensive error enumeration
- `SensorPacket`: Type-safe sensor identifiers
- `SensorData`: Structured sensor information
- `ScheduleData`: Scheduling configuration

**AI Context**: This file establishes the type system that ensures compile-time safety and provides semantic meaning to data throughout the codebase.

#### ArduRoombaConstants.h
**Purpose**: Centralizes all protocol constants, command codes, and configuration values.

**Key Constants**:
- iRobot Open Interface command codes
- Timing requirements and delays
- Hardware configuration limits
- Protocol-specific values

**AI Context**: This file contains the authoritative definition of the iRobot protocol implementation, serving as the single source of truth for all protocol-related values.

### 2. Core Layer

#### ArduRoombaCore (RoombaCore class)
**Purpose**: Hardware abstraction layer implementing the iRobot Open Interface protocol.

**Responsibilities**:
- Serial communication management
- Protocol state machine
- Baud rate negotiation
- Command transmission and response handling
- Error detection and recovery

**Key Methods**:
- `initialize()`: Establishes communication with robot
- `sendCommand()`: Transmits commands with validation
- `readResponse()`: Receives and validates responses
- `setMode()`: Manages OI mode transitions

**AI Context**: This is the foundational layer that all other components depend on. It encapsulates all hardware-specific details and protocol complexities, providing a clean interface for higher-level operations.

**State Management**:
```cpp
enum class OIMode {
    OFF,        // Robot not initialized
    PASSIVE,    // Can read sensors, cannot control
    SAFE,       // Can control, safety features active
    FULL        // Complete control, safety features disabled
};
```

### 3. Service Layer

#### ArduRoombaSensors (RoombaSensors class)
**Purpose**: Sensor data acquisition and processing system.

**Responsibilities**:
- Sensor packet management
- Data streaming coordination
- Sensor value interpretation
- Data validation and filtering

**Key Features**:
- Type-safe sensor access
- Streaming data support
- Automatic data validation
- Configurable update rates

**AI Context**: This module abstracts the complexity of sensor data acquisition, providing a clean interface for accessing robot state information. It handles the intricacies of the sensor packet system and provides meaningful data structures.

#### ArduRoombaCommands (RoombaCommands class)
**Purpose**: High-level command interface for robot control.

**Responsibilities**:
- Movement command generation
- Actuator control
- Command sequencing
- Safety validation

**Key Command Categories**:
- **Movement**: Drive, turn, stop operations
- **Cleaning**: Start, stop, dock commands
- **Actuators**: Brush, vacuum, LED control
- **Audio**: Song playback and sound effects

**AI Context**: This module provides the primary interface for robot control, translating high-level intentions into low-level protocol commands. It includes safety checks and command validation.

#### ArduRoombaConfig (RoombaConfig class)
**Purpose**: Configuration and settings management system.

**Responsibilities**:
- Persistent settings storage
- Configuration validation
- Default value management
- Settings migration

**Key Features**:
- EEPROM-based persistence
- Type-safe configuration access
- Automatic validation
- Version migration support

**AI Context**: This module enables persistent configuration across power cycles, allowing for customizable robot behavior and user preferences. It provides a foundation for advanced features requiring state persistence.

#### ArduRoombaWiFi (RoombaWiFi class) [Platform-Specific]
**Purpose**: Wireless connectivity and web interface system.

**Responsibilities**:
- WiFi connection management
- Web server implementation
- REST API endpoints
- Real-time communication

**Platform Support**:
- ESP32: Full feature support
- ESP8266: Core functionality
- Arduino Uno R4 WiFi: Basic support

**AI Context**: This module extends the library's capabilities to wireless operation, enabling remote control and monitoring. It's conditionally compiled based on platform capabilities.

### 4. Facade Layer

#### ArduRoomba (Main Class)
**Purpose**: Unified interface providing simplified access to all library functionality.

**Design Pattern**: Facade Pattern
- Simplifies complex subsystem interactions
- Provides backward compatibility
- Centralizes error handling
- Manages component lifecycle

**Key Responsibilities**:
- Component orchestration
- Error aggregation and reporting
- Legacy API compatibility
- Resource management

**AI Context**: This is the primary entry point for users and the main coordination point for all library operations. It implements the facade pattern to hide complexity while providing comprehensive functionality.

## Data Flow Architecture

### 1. Command Flow
```
User Code → ArduRoomba → RoombaCommands → RoombaCore → Robot
                ↓              ↓             ↓
            Error Handling → Validation → Protocol
```

### 2. Sensor Flow
```
Robot → RoombaCore → RoombaSensors → ArduRoomba → User Code
           ↓             ↓              ↓
       Protocol → Data Processing → Type Safety
```

### 3. Configuration Flow
```
User Settings → RoombaConfig → EEPROM Storage
                     ↓
              Validation & Persistence
```

## Error Handling Architecture

### Error Code Hierarchy
```cpp
enum class ErrorCode : uint8_t {
    SUCCESS = 0,
    
    // Communication Errors (1-19)
    TIMEOUT = 1,
    COMMUNICATION_FAILED = 2,
    INVALID_RESPONSE = 3,
    
    // Parameter Errors (20-39)
    INVALID_PARAMETER = 20,
    OUT_OF_RANGE = 21,
    
    // State Errors (40-59)
    NOT_INITIALIZED = 40,
    INVALID_STATE = 41,
    
    // Hardware Errors (60-79)
    HARDWARE_ERROR = 60,
    SENSOR_ERROR = 61,
    
    // System Errors (80-99)
    MEMORY_ERROR = 80,
    UNKNOWN_ERROR = 99
};
```

### Error Propagation Strategy
1. **Detection**: Errors detected at the lowest level
2. **Propagation**: Errors bubble up through layers
3. **Aggregation**: Facade layer collects and reports errors
4. **Recovery**: Automatic recovery where possible
5. **Reporting**: Debug output and error state tracking

## Memory Management

### Arduino Constraints
- **SRAM Limitations**: Careful memory usage on Uno (2KB SRAM)
- **Flash Optimization**: Efficient code organization
- **Stack Management**: Avoid deep recursion
- **Dynamic Allocation**: Minimize heap usage

### Resource Management Strategy
- **RAII Principles**: Resources managed by object lifecycle
- **Static Allocation**: Prefer stack allocation
- **Resource Pooling**: Reuse buffers and objects
- **Cleanup Guarantees**: Destructors handle cleanup

## Platform Compatibility

### Supported Platforms
| Platform | Core | Sensors | Commands | Config | WiFi |
|----------|------|---------|----------|--------|------|
| Arduino Uno R3 | ✅ | ✅ | ✅ | ✅ | ❌ |
| Arduino Uno R4 WiFi | ✅ | ✅ | ✅ | ✅ | ✅ |
| ESP32 | ✅ | ✅ | ✅ | ✅ | ✅ |
| ESP8266 | ✅ | ✅ | ✅ | ✅ | ✅ |

### Conditional Compilation
```cpp
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)
    #include "wireless/ArduRoombaWiFi.h"
    #define ARDUROOMBA_WIFI_SUPPORT
#endif
```

## Extension Points

### 1. Adding New Sensors
1. Add sensor packet ID to `SensorPacket` enum
2. Update sensor data structures
3. Implement parsing logic in `RoombaSensors`
4. Add accessor methods

### 2. Adding New Commands
1. Define command constants
2. Implement command methods in `RoombaCommands`
3. Add validation logic
4. Update facade interface

### 3. Adding New Platforms
1. Define platform detection macros
2. Implement platform-specific code
3. Update conditional compilation
4. Test platform compatibility

## Testing Strategy

### Unit Testing
- Component isolation testing
- Mock dependencies for testing
- Error condition validation
- Edge case coverage

### Integration Testing
- Component interaction testing
- Protocol compliance validation
- Hardware compatibility testing
- Performance benchmarking

### Example-Based Testing
- Real-world usage scenarios
- Documentation validation
- API usability testing
- Platform compatibility verification

## Documentation Standards

### Code Documentation
- **Doxygen Comments**: Comprehensive API documentation
- **AI Context**: Specific guidance for AI agents
- **Usage Examples**: Practical implementation guidance
- **Architecture Notes**: Design decision explanations

### User Documentation
- **Getting Started**: Quick setup guides
- **API Reference**: Complete method documentation
- **Examples**: Working code samples
- **Troubleshooting**: Common issues and solutions

## Future Architecture Considerations

### Scalability
- Plugin architecture for extensions
- Event-driven communication
- Asynchronous operation support
- Multi-robot coordination

### Performance
- Interrupt-driven sensor reading
- DMA-based communication
- Optimized memory usage
- Real-time operation support

### Maintainability
- Automated testing framework
- Continuous integration
- Version management
- Backward compatibility strategy

This architecture guide provides the foundation for understanding, maintaining, and extending the ArduRoomba library. It serves as both a reference for human developers and a comprehensive context for AI agents working with the codebase.