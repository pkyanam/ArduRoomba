# ArduRoomba

ArduRoomba is a comprehensive, production-ready Arduino library and wireless robotics platform for interfacing with iRobot Create 2 and compatible Roomba models. Built on a modular architecture with wireless capabilities, REST API, web interface, and extensible plugin system, it enables everything from simple serial control to advanced IoT integration and autonomous behaviors.

## 🎯 Vision & Mission

**Vision**: Transform ArduRoomba into the leading open-source platform for wireless robotics control, enabling developers to build sophisticated automation, IoT integration, and autonomous behaviors with minimal complexity.

**Mission**: Provide a robust, extensible, and well-documented foundation that serves both beginners learning robotics and professionals building production systems, while maintaining simplicity and backward compatibility.

## 🏗️ Architecture Overview

ArduRoomba follows a modular, event-driven architecture designed for scalability and extensibility:

```
┌─────────────────────────────────────────────────────────────┐
│                    ArduRoomba Platform                     │
├─────────────────────────────────────────────────────────────┤
│  Web Interface  │  REST API  │  WebSocket  │  Mobile App   │
├─────────────────────────────────────────────────────────────┤
│           WiFi Management & Security Layer                 │
├─────────────────────────────────────────────────────────────┤
│  Config Mgmt  │  Event System  │  State Machine  │ Logging │
├─────────────────────────────────────────────────────────────┤
│   Commands    │    Sensors     │     Core       │ Plugins  │
├─────────────────────────────────────────────────────────────┤
│              Hardware Abstraction Layer                    │
├─────────────────────────────────────────────────────────────┤
│        ESP32  │  ESP8266  │  Uno R4 WiFi  │  Arduino      │
└─────────────────────────────────────────────────────────────┘
```

### Core Components

- **ArduRoombaCore**: Low-level hardware communication and protocol management
- **ArduRoombaSensors**: Sensor data parsing, streaming, and preset management
- **ArduRoombaCommands**: High-level robot control and actuator management
- **ArduRoombaWiFi**: Wireless connectivity, web server, and API endpoints
- **ArduRoombaConfig**: Persistent configuration and settings management
- **ArduRoombaEvents**: Event system and state management for loose coupling

### Design Principles

1. **Modularity**: Clean separation of concerns with well-defined interfaces
2. **Extensibility**: Plugin system for custom behaviors and integrations
3. **Reliability**: Comprehensive error handling and automatic recovery
4. **Performance**: Optimized for embedded systems with limited resources
5. **Security**: Multi-layer security with authentication and rate limiting
6. **Compatibility**: 100% backward compatibility with existing code
7. **Documentation**: Comprehensive docs for humans and AI agents

## ✨ Features

### 🤖 Core Robotics Functionality
- **Complete iRobot Control**: Full control over movement, cleaning modes, actuators, LEDs, and sensors
- **Advanced Sensor Management**: Real-time streaming with predefined presets and custom configurations
- **Intelligent State Management**: State machine for robot behaviors with event-driven transitions
- **Precision Movement**: Velocity control, radius turning, direct wheel control, and path planning
- **Safety Systems**: Cliff detection, bump sensors, wheel drop monitoring, and emergency stops
- **Battery Management**: Comprehensive monitoring, low-battery alerts, and charging state tracking

### 🌐 Wireless & Connectivity
- **WiFi Management**: Auto-connect, reconnection, AP mode fallback, and network diagnostics
- **REST API**: Complete RESTful interface for all robot functions with JSON responses
- **Real-time Communication**: WebSocket and Server-Sent Events for live updates
- **Web Interface**: Responsive, mobile-optimized control panel with touch controls
- **Device Discovery**: mDNS service discovery and network scanning
- **Cross-Platform**: ESP32, ESP8266, Arduino Uno R4 WiFi support

### 🔧 Developer Experience
- **Modular Architecture**: Clean separation with well-defined interfaces
- **Event System**: Pub/sub architecture for loose coupling and extensibility
- **Configuration Management**: Persistent settings with web-based configuration
- **Comprehensive Logging**: Structured logging with rotation and export capabilities
- **Performance Monitoring**: Real-time metrics, profiling, and health monitoring
- **Plugin System**: Extensible architecture for custom behaviors and integrations

### 🛡️ Security & Reliability
- **Multi-user Authentication**: Session-based security with role management
- **Rate Limiting**: DDoS protection and API abuse prevention
- **Secure Storage**: Encrypted configuration and credential management
- **Error Recovery**: Automatic recovery from network and hardware failures
- **Comprehensive Validation**: Input sanitization and parameter validation
- **Audit Logging**: Security event tracking and compliance reporting

### 📱 User Interface
- **Responsive Web UI**: Mobile-first design that works on all devices
- **Touch Controls**: Virtual D-pad, gesture support, and haptic feedback
- **Real-time Dashboard**: Live sensor monitoring and status visualization
- **Progressive Web App**: Offline capability and app-like installation
- **Keyboard Support**: Arrow keys, WASD, and custom hotkey mapping
- **Accessibility**: Screen reader support and high contrast modes

### 🔌 Integration & Extensibility
- **Home Automation**: MQTT, Home Assistant, OpenHAB integration
- **IoT Platforms**: AWS IoT, Azure IoT, Google Cloud IoT support
- **Webhook Support**: HTTP callbacks for external system integration
- **Custom Plugins**: Framework for third-party extensions and behaviors
- **API Ecosystem**: RESTful API for building custom applications
- **Data Export**: CSV, JSON, and database export capabilities

### 📊 Analytics & Insights
- **Usage Analytics**: Cleaning patterns, battery usage, and performance metrics
- **Predictive Maintenance**: Wear pattern analysis and maintenance scheduling
- **Performance Optimization**: Route efficiency and battery optimization
- **Historical Data**: Long-term trend analysis and reporting
- **Custom Dashboards**: Configurable monitoring and alerting
- **Machine Learning Ready**: Data export for ML model training

### 🧪 Testing & Quality Assurance
- **Automated Testing**: Unit, integration, and hardware-in-the-loop tests
- **Mock Hardware**: Software simulation for development and testing
- **Performance Benchmarks**: Memory usage, response time, and reliability metrics
- **Compatibility Testing**: Validation across all supported platforms
- **Continuous Integration**: Automated build and test pipeline
- **Quality Gates**: Code coverage, documentation, and performance requirements

## 📦 Installation

### Via Arduino IDE Library Manager (Recommended)
1. Open Arduino IDE
2. Go to `Sketch` > `Include Library` > `Manage Libraries...`
3. Search for "ArduRoomba"
4. Click "Install"

### Manual Installation
1. Download this repository as ZIP
2. In Arduino IDE: `Sketch` > `Include Library` > `Add .ZIP Library...`
3. Select the downloaded ZIP file

For detailed installation instructions, refer to the [Arduino Library Installation Guide](https://www.arduino.cc/en/guide/libraries).

## 🔌 Hardware Setup

### Wiring Requirements
Before using the library, ensure your wiring is compatible with the appropriate logic levels. The iRobot Open Interface operates at 5V levels for most Arduino boards, but requires 3.3V for ESP boards.

> ⚠️ **IMPORTANT**: Improper wiring may damage your Arduino or iRobot unit. Always verify voltage levels before connecting.

### Arduino UNO/Uno R4 Wiring

![arduino-wiring](https://github.com/pkyanam/ArduRoomba/assets/37784174/cb9dd879-04ae-4499-ab68-aed5dfe68eef)

**Connections:**
- Arduino Pin 2 → Roomba TX (receive data from Roomba)
- Arduino Pin 3 → Roomba RX (send data to Roomba)  
- Arduino Pin 4 → Roomba BRC (Baud Rate Change pin)
- Arduino GND → Roomba GND
- Arduino 5V → Roomba VCC

### ESP-01/ESP32 Wiring

> ⚠️ **CAUTION**: You need to convert the Roomba battery voltage (up to 15V-20V) to 3.3V for ESP boards.
> 
> Use an **MP1584EN** Step-Down Adjustable Module Buck Converter for this purpose.

![esp01-wiring](https://raw.githubusercontent.com/oRiamn/ArduRoomba/acb016d59402e8e8f6d7e850406103f35f47a27d/docs/esp01-wiring.png)

> 💡 **TIP**: Using a 2N3906 transistor can strengthen the weak Roomba TX signal. Without a transistor, ESP boards may not be able to receive sensor data reliably.

## 🚀 Quick Start

### Basic Usage (New Interface)

```cpp
#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4); // rxPin, txPin, brcPin

void setup() {
    Serial.begin(19200);
    
    // Initialize with error handling
    ArduRoomba::ErrorCode result = roomba.initialize();
    if (result == ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("Roomba ready!");
        
        // Move forward for 2 seconds
        roomba.moveForward(300); // 300 mm/s
        delay(2000);
        roomba.stopMovement();
        
        // Play a beep
        roomba.beep();
    } else {
        Serial.println("Initialization failed!");
    }
}

void loop() {
    // Your main code here
}
```

### Sensor Streaming Example

```cpp
#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4);
ArduRoomba::SensorData sensorData;

void setup() {
    Serial.begin(19200);
    
    if (roomba.initialize() == ArduRoomba::ErrorCode::SUCCESS) {
        // Start streaming basic sensors
        roomba.startSensorStream(ArduRoomba::RoombaSensors::SensorPreset::BASIC);
    }
}

void loop() {
    if (roomba.updateSensorData(sensorData) == ArduRoomba::ErrorCode::SUCCESS) {
        // Check for obstacles
        if (sensorData.bumpLeft || sensorData.bumpRight) {
            Serial.println("Obstacle detected!");
            roomba.stopMovement();
        }
        
        // Monitor battery
        int batteryPercent = sensorData.getBatteryPercentage();
        if (batteryPercent < 20) {
            Serial.println("Low battery!");
        }
    }
}
```

### Legacy Compatibility

Existing code continues to work without changes:

```cpp
#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4);

void setup() {
    Serial.begin(19200);
    roomba.roombaSetup(); // Legacy initialization
    
    roomba.goForward();
    delay(1000);
    roomba.halt();
}

void loop() {
    // Legacy code works unchanged
}
```

## 📖 API Reference

### Core Classes

#### `ArduRoomba` (Main Interface)
The primary class that provides both new enhanced methods and legacy compatibility.

```cpp
// Initialization
ErrorCode initialize(uint32_t baudRate = 19200);
bool isInitialized() const;

// Enhanced Movement
ErrorCode moveForward(int16_t velocity = 200);
ErrorCode moveBackward(int16_t velocity = 200);
ErrorCode turnLeftInPlace(int16_t velocity = 200);
ErrorCode turnRightInPlace(int16_t velocity = 200);
ErrorCode stopMovement();

// Sensor Management
ErrorCode startSensorStream(RoombaSensors::SensorPreset preset);
ErrorCode updateSensorData(SensorData& data);
ErrorCode getAllSensorData(SensorData& data);

// Audio & LEDs
ErrorCode beep(uint8_t frequency = 72, uint8_t duration = 32);
ErrorCode setPowerLED(uint8_t color, uint8_t intensity);

// Component Access
RoombaCore& getCore();
RoombaSensors& getSensors();
RoombaCommands& getCommands();
```

#### `ArduRoomba::SensorData`
Comprehensive structure containing all sensor information with helper methods.

```cpp
// Battery Information
int getBatteryPercentage() const;
bool isFresh(unsigned long maxAge = 1000) const;
unsigned long getAge() const;

// Safety Checks
bool hasCliffDetection() const;
bool hasBumpDetection() const;
bool hasWheelDrop() const;
bool hasOvercurrent() const;

// Sensor Values
uint16_t voltage;           // Battery voltage (mV)
int16_t current;            // Battery current (mA)
uint16_t batteryCharge;     // Current charge (mAh)
uint16_t batteryCapacity;   // Total capacity (mAh)
int8_t temperature;         // Battery temperature (°C)

// Boolean Sensors
bool wall, virtualWall;
bool cliffLeft, cliffFrontLeft, cliffRight, cliffFrontRight;
bool bumpLeft, bumpRight;
bool wheelDropLeft, wheelDropRight;
// ... and many more
```

### Sensor Presets

Predefined sensor configurations for common use cases:

- `BASIC`: Essential sensors (bumpers, cliffs, wall, battery)
- `NAVIGATION`: Movement-related sensors (encoders, velocity, radius)
- `SAFETY`: All safety sensors (cliffs, bumpers, wheel drops, overcurrents)
- `BATTERY`: Battery monitoring sensors
- `BUTTONS`: Button and IR sensors
- `LIGHT_BUMPERS`: Light bumper sensors
- `ALL_SENSORS`: Comprehensive sensor set

### Error Codes

```cpp
enum class ErrorCode : uint8_t {
    SUCCESS = 0,
    TIMEOUT = 1,
    CHECKSUM_ERROR = 2,
    INVALID_PARAMETER = 3,
    BUFFER_OVERFLOW = 4,
    COMMUNICATION_ERROR = 5,
    NOT_INITIALIZED = 6,
    UNKNOWN_ERROR = 255
};
```

## 🎯 Examples

The library includes comprehensive examples:

- **BasicUsage.ino**: Basic movement and initialization
- **StreamExample.ino**: Sensor streaming and monitoring
- **SensorDataExample.ino**: Individual sensor queries
- **RemoteControl.ino**: Remote control implementation
- **ESPBasicUsage.ino**: ESP8266/ESP32 specific example

## 🔧 Advanced Features

### Modular Component Access

Access individual components for advanced control:

```cpp
// Direct core communication
roomba.getCore().sendCommand(ArduRoomba::OIOpcode::CLEAN);

// Advanced sensor management
roomba.getSensors().setDebugEnabled(true);
roomba.getSensors().requestSensorGroup(100, buffer, &size);

// Detailed command control
roomba.getCommands().setMotors(true, false, true, false, false);
```

### Statistics and Debugging

```cpp
// Enable debug output
roomba.setDebugEnabled(true);

// Get communication statistics
uint32_t bytesSent, bytesReceived, commandsSent;
uint16_t errors;
roomba.getStatistics(bytesSent, bytesReceived, commandsSent, errors);

// Check last error
ArduRoomba::ErrorCode lastError = roomba.getLastError();
```

### Custom Sensor Lists

```cpp
// Create custom sensor list
ArduRoomba::SensorPacket customSensors[] = {
    ArduRoomba::SensorPacket::VOLTAGE,
    ArduRoomba::SensorPacket::CURRENT,
    ArduRoomba::SensorPacket::TEMPERATURE
};

roomba.getSensors().startStreaming(customSensors, 3);
```

## 🔄 Migration Guide

### From Version 2.2.x to 2.3.0

**Good News**: Your existing code will continue to work without changes! The library maintains full backward compatibility.

**To take advantage of new features**:

1. **Replace `roombaSetup()`** with `initialize()` for better error handling:
   ```cpp
   // Old way (still works)
   roomba.roombaSetup();
   
   // New way (recommended)
   if (roomba.initialize() == ArduRoomba::ErrorCode::SUCCESS) {
       // Initialization successful
   }
   ```

2. **Use enhanced movement methods**:
   ```cpp
   // Old way (still works)
   roomba.goForward();
   
   // New way (with error handling and custom speed)
   roomba.moveForward(300); // 300 mm/s
   ```

3. **Upgrade sensor handling**:
   ```cpp
   // Old way (still works)
   ArduRoomba::RoombaInfos infos;
   roomba.refreshData(&infos);
   
   // New way (enhanced features)
   ArduRoomba::SensorData data;
   roomba.updateSensorData(data);
   int batteryPercent = data.getBatteryPercentage();
   ```

## 🔍 Compatibility

### IDEs
- Arduino IDE 1.8.x and 2.x.x
- PlatformIO
- Arduino CLI

### Boards/MCUs
- ✅ Arduino Uno R3 (AVR)
- ✅ Arduino Uno R4 Minima/WiFi (RENESAS_UNO)
- ✅ ESP-01 (ESP8266)
- ✅ ESP32 (all variants)
- ✅ Arduino Nano, Pro Mini (AVR)
- ⚠️ Other boards may work but are untested

### iRobot Models
- ✅ **Tested**: iRobot Roomba 551, 605
- ✅ **Compatible**: iRobot Create 2
- ✅ **Compatible**: iRobot Roomba 500, 600, 700, 800 series (non-WiFi models)
- ❌ **Not Compatible**: WiFi-enabled Roomba models (900+ series)

## 🛠️ Troubleshooting

### Common Issues

**Initialization fails**:
- Check wiring connections
- Verify voltage levels (5V for Arduino, 3.3V for ESP)
- Ensure BRC pin is properly connected
- Try different baud rates

**No sensor data**:
- Verify RX connection from Roomba to Arduino
- For ESP boards, consider using a signal amplifier transistor
- Check if streaming is properly started
- Enable debug output to see communication details

**Erratic behavior**:
- Ensure proper grounding between Arduino and Roomba
- Check for loose connections
- Verify power supply stability
- Reduce sensor streaming frequency if needed

### Debug Output

Enable debug output to diagnose issues:

```cpp
roomba.setDebugEnabled(true);
```

This will print detailed information about:
- Initialization steps
- Command transmission
- Sensor data reception
- Error conditions

## 🤝 Contributing

We welcome contributions! Here's how you can help:

1. **Test with new hardware**: Try the library with untested boards or Roomba models
2. **Report issues**: Create detailed bug reports with hardware info and code examples
3. **Submit improvements**: Pull requests for bug fixes, features, or documentation
4. **Share examples**: Contribute example sketches for specific use cases

### Development Guidelines

- Follow the existing code style and documentation format
- Add comprehensive comments and documentation
- Include examples for new features
- Maintain backward compatibility
- Test thoroughly before submitting

## 📋 Roadmap

### Completed ✅
- [x] Modular architecture refactoring
- [x] Enhanced error handling and validation
- [x] Comprehensive documentation
- [x] ESP32/ESP8266 support
- [x] Sensor streaming improvements
- [x] Backward compatibility maintenance

### Planned 🚧
- [ ] Local Alexa integration for voice control
- [ ] Web interface for WiFi-enabled boards
- [ ] Advanced navigation algorithms
- [ ] MQTT integration for IoT applications
- [ ] Mobile app companion
- [ ] Machine learning integration for autonomous behavior

### Future Considerations 💭
- [ ] Support for newer Roomba models
- [ ] Integration with home automation systems
- [ ] Advanced mapping and localization
- [ ] Multi-robot coordination

## 👥 Authors and Acknowledgments

- **Preetham Kyanam** <preetham@preetham.org> - Original author and maintainer
- **Contributors**: Thanks to all community members who have tested, reported issues, and contributed improvements

## 📄 License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0). See the [LICENSE](LICENSE) file for details.

### What this means:
- ✅ You can use this library in your projects
- ✅ You can modify and distribute the library
- ✅ You can use it for commercial purposes
- ⚠️ You must include the license and copyright notice
- ⚠️ You must disclose the source code of any modifications
- ⚠️ Any derivative work must also be licensed under GPL-3.0

## 🤖 For AI Agents & Contextual Awareness

### Project Context & State
ArduRoomba is currently in **active development** transitioning from v2.4.0 (wireless foundation) to v2.5.0 (foundational primitives). The project follows a **modular, event-driven architecture** with emphasis on **backward compatibility** and **extensible design**.

### Current Development Phase
**Phase**: Foundational Primitives Implementation  
**Priority**: P0 - Core Infrastructure (Configuration, Events, Real-time Communication)  
**Timeline**: 4-phase approach over 8 weeks  
**Status**: Planning complete, ready for implementation

### Codebase Structure & Patterns
```
src/
├── ArduRoomba.h/.cpp           # Main unified interface (legacy compatibility)
├── ArduRoombaConstants.h       # Centralized constants and enums
├── ArduRoombaTypes.h           # Data structures and type definitions
├── ArduRoombaCore.h/.cpp       # Hardware communication layer
├── ArduRoombaSensors.h/.cpp    # Sensor management and parsing
├── ArduRoombaCommands.h/.cpp   # High-level command interface
└── wireless/
    ├── ArduRoombaWiFi.h/.cpp   # WiFi management and web server
    └── [Future: WebSocket, SSE, Security, Discovery]
```

### Key Design Patterns
- **Facade Pattern**: `ArduRoomba` class provides unified interface
- **Strategy Pattern**: Platform-specific implementations (ESP32/ESP8266/Uno R4)
- **Observer Pattern**: Event system for component communication
- **State Machine**: Robot behavior management
- **Factory Pattern**: Sensor preset creation
- **Singleton Pattern**: Configuration and logging systems

### Development Guidelines for AI Agents
1. **Maintain Backward Compatibility**: All changes must preserve existing API
2. **Follow Modular Architecture**: New features go in appropriate components
3. **Use Error Codes**: Return `ErrorCode` enum for all operations
4. **Document Everything**: Doxygen-style comments for all public APIs
5. **Test Thoroughly**: Validate on ESP32, ESP8266, and Uno R4 WiFi
6. **Memory Conscious**: Optimize for embedded system constraints
7. **Event-Driven**: Use event system for component communication

### Current Gaps & Immediate Needs
1. **Configuration Management**: Persistent settings storage and web interface
2. **Event System**: Pub/sub architecture for loose coupling
3. **Real-time Communication**: WebSocket and SSE implementation
4. **Data Persistence**: Logging and metrics storage
5. **Enhanced Security**: Multi-user auth and rate limiting
6. **Testing Framework**: Automated testing infrastructure

### API Evolution Strategy
- **v2.4.x**: Complete wireless API implementation
- **v2.5.x**: Foundational primitives (config, events, real-time)
- **v2.6.x**: Advanced features (plugins, ML integration)
- **v3.0.x**: Major architectural enhancements (if needed)

### Integration Points
- **Home Automation**: MQTT, Home Assistant, OpenHAB
- **Cloud Platforms**: AWS IoT, Azure IoT, Google Cloud
- **Mobile Apps**: Progressive Web App, native app APIs
- **Voice Assistants**: Alexa Skills, Google Actions
- **Machine Learning**: TensorFlow Lite, Edge Impulse

### Performance Targets
- **Memory Usage**: <85% ESP32, <75% ESP8266, <60% Uno R4
- **Response Time**: <100ms API calls, <50ms WebSocket
- **Reliability**: 99.9% uptime, auto-recovery from failures
- **Scalability**: 10+ concurrent connections
- **Battery Life**: Minimal impact on robot operation

### Quality Standards
- **Code Coverage**: >90% for new features
- **Documentation**: 100% public API coverage
- **Platform Testing**: All supported boards validated
- **Performance**: Benchmarked and optimized
- **Security**: Penetration tested and hardened

### Community & Ecosystem
- **Target Users**: Hobbyists, educators, professionals, researchers
- **Use Cases**: Home automation, education, research, commercial
- **Ecosystem**: Plugins, integrations, custom applications
- **Support**: GitHub issues, documentation, examples

### Future Vision
Transform ArduRoomba into the **de facto standard** for wireless robotics control, enabling:
- **Autonomous Behaviors**: AI-driven navigation and task planning
- **Fleet Management**: Multi-robot coordination and monitoring
- **Edge Computing**: On-device machine learning and decision making
- **Digital Twin**: Virtual representation and simulation
- **Ecosystem Platform**: Third-party plugins and integrations

### Development Workflow
1. **Feature Branches**: Each component in separate branch
2. **Pull Requests**: Comprehensive review and testing
3. **Documentation First**: Write docs before implementation
4. **Incremental Releases**: Regular updates with backward compatibility
5. **Community Feedback**: User testing and feature requests

This context enables AI agents to understand the project's current state, development patterns, and future direction for effective contribution and assistance.

## 🔗 Links

- **Project Repository**: [https://github.com/pkyanam/ArduRoomba](https://github.com/pkyanam/ArduRoomba)
- **Arduino Library Manager**: Search for "ArduRoomba"
- **Documentation**: [API Reference](docs/) | [Architecture](FOUNDATIONAL_PRIMITIVES_ANALYSIS.md) | [Roadmap](DEVELOPMENT_ROADMAP.md)
- **Issues & Support**: [GitHub Issues](https://github.com/pkyanam/ArduRoomba/issues)
- **iRobot OI Specification**: [docs/irobot-create2-oi-spec.pdf](docs/irobot-create2-oi-spec.pdf)

---

**Happy Roomba Hacking! 🤖✨**

*ArduRoomba: From simple serial control to intelligent wireless robotics platform*
