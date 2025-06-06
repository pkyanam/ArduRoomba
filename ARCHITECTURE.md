# ArduRoomba Architecture Documentation

## 🏗️ System Architecture Overview

ArduRoomba follows a **layered, modular architecture** designed for scalability, maintainability, and extensibility. The system is built on **event-driven principles** with **clear separation of concerns** and **well-defined interfaces** between components.

## 📐 Architectural Layers

### Layer 1: Hardware Abstraction Layer (HAL)
**Purpose**: Abstract platform-specific hardware differences  
**Components**: Platform detection, pin mapping, communication protocols  
**Responsibility**: Provide unified interface across ESP32, ESP8266, Arduino Uno R4 WiFi

```cpp
// Platform abstraction example
#if defined(ESP32)
    #include <WiFi.h>
    #define PLATFORM_HAS_FILESYSTEM true
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #define PLATFORM_HAS_FILESYSTEM true
#elif defined(ARDUINO_UNOWIFIR4)
    #include <WiFiS3.h>
    #define PLATFORM_HAS_FILESYSTEM false
#endif
```

### Layer 2: Core Communication Layer
**Purpose**: Low-level robot communication and protocol management  
**Components**: `ArduRoombaCore`  
**Responsibility**: Serial communication, command transmission, protocol handling

```cpp
class ArduRoombaCore {
    // Hardware communication
    ErrorCode sendCommand(OIOpcode opcode, const uint8_t* data = nullptr, size_t length = 0);
    ErrorCode receiveData(uint8_t* buffer, size_t maxLength, size_t& received);
    
    // Protocol management
    bool validateChecksum(const uint8_t* data, size_t length);
    void calculateChecksum(uint8_t* data, size_t length);
};
```

### Layer 3: Functional Components Layer
**Purpose**: Specialized functionality with clear responsibilities  
**Components**: `ArduRoombaSensors`, `ArduRoombaCommands`  
**Responsibility**: Domain-specific operations and data management

```cpp
// Sensor management
class ArduRoombaSensors {
    ErrorCode startStreaming(SensorPreset preset);
    ErrorCode updateSensorData(SensorData& data);
    ErrorCode parseSensorPacket(const uint8_t* packet, SensorData& data);
};

// Command management  
class ArduRoombaCommands {
    ErrorCode moveForward(int16_t velocity);
    ErrorCode startCleaning();
    ErrorCode setLEDs(uint8_t ledBits, uint8_t powerColor, uint8_t powerIntensity);
};
```

### Layer 4: Infrastructure Services Layer
**Purpose**: Cross-cutting concerns and system services  
**Components**: Configuration, Events, Logging, Security  
**Responsibility**: System-wide services and utilities

```cpp
// Configuration management
class ArduRoombaConfig {
    ErrorCode load();
    ErrorCode save();
    String getWiFiSSID() const;
    void setWiFiSSID(const String& ssid);
};

// Event system
class ArduRoombaEventSystem {
    void subscribe(EventType type, EventHandler handler);
    void publish(EventType type, const EventData& data);
};
```

### Layer 5: Network & Communication Layer
**Purpose**: Wireless connectivity and external communication  
**Components**: `ArduRoombaWiFi`, WebSocket, REST API  
**Responsibility**: Network management, web server, API endpoints

```cpp
class ArduRoombaWiFi {
    ErrorCode begin(const String& ssid, const String& password);
    ErrorCode startWebServer(uint16_t port = 80);
    void handleClient();
};
```

### Layer 6: Application Interface Layer
**Purpose**: Unified interface and backward compatibility  
**Components**: `ArduRoomba` (main facade)  
**Responsibility**: Simple API, legacy compatibility, component coordination

```cpp
class ArduRoomba {
    // Unified interface
    ErrorCode initialize();
    ErrorCode moveForward(int16_t velocity = 200);
    
    // Component access
    RoombaCore& getCore();
    RoombaSensors& getSensors();
    RoombaCommands& getCommands();
};
```

## 🔄 Data Flow Architecture

### Command Flow (User → Robot)
```
Web Interface → REST API → WiFi Component → Event System → Commands Component → Core → Robot
```

### Sensor Flow (Robot → User)
```
Robot → Core → Sensors Component → Event System → WiFi Component → WebSocket → Web Interface
```

### Configuration Flow
```
Web Interface → REST API → Config Component → Storage → Event System → All Components
```

## 🎯 Design Patterns & Principles

### 1. Facade Pattern
**Implementation**: `ArduRoomba` class  
**Purpose**: Provide simple interface hiding complex subsystem interactions  
**Benefits**: Easy to use, backward compatibility, controlled access

```cpp
class ArduRoomba {
private:
    RoombaCore _core;
    RoombaSensors _sensors;
    RoombaCommands _commands;
    
public:
    // Simple interface hiding complexity
    ErrorCode moveForward(int16_t velocity = 200) {
        return _commands.moveForward(velocity);
    }
};
```

### 2. Observer Pattern (Event System)
**Implementation**: Event subscription/publication  
**Purpose**: Loose coupling between components  
**Benefits**: Extensibility, maintainability, testability

```cpp
// Components subscribe to events
eventSystem.subscribe(EventType::SENSOR_UPDATE, [this](const Event& e) {
    handleSensorUpdate(e);
});

// Components publish events
eventSystem.publish(EventType::BATTERY_LOW, batteryData);
```

### 3. Strategy Pattern
**Implementation**: Platform-specific implementations  
**Purpose**: Runtime algorithm selection based on platform  
**Benefits**: Platform abstraction, code reuse, maintainability

```cpp
class WiFiStrategy {
public:
    virtual ErrorCode connect(const String& ssid, const String& password) = 0;
    virtual bool isConnected() = 0;
};

class ESP32WiFiStrategy : public WiFiStrategy { /* ESP32 implementation */ };
class ESP8266WiFiStrategy : public WiFiStrategy { /* ESP8266 implementation */ };
```

### 4. State Machine Pattern
**Implementation**: Robot behavior management  
**Purpose**: Manage complex state transitions and behaviors  
**Benefits**: Predictable behavior, easy debugging, extensible states

```cpp
class RoombaStateMachine {
    enum class State { IDLE, MOVING, CLEANING, DOCKING, ERROR };
    
    State currentState = State::IDLE;
    
    ErrorCode transition(State newState) {
        if (isValidTransition(currentState, newState)) {
            onStateExit(currentState);
            currentState = newState;
            onStateEnter(newState);
            return ErrorCode::SUCCESS;
        }
        return ErrorCode::INVALID_PARAMETER;
    }
};
```

### 5. Factory Pattern
**Implementation**: Sensor preset creation  
**Purpose**: Create complex objects with predefined configurations  
**Benefits**: Encapsulation, reusability, consistency

```cpp
class SensorPresetFactory {
public:
    static std::vector<SensorPacket> createPreset(SensorPreset preset) {
        switch (preset) {
            case SensorPreset::BASIC:
                return {SensorPacket::VOLTAGE, SensorPacket::CURRENT, SensorPacket::BUMP_SENSORS};
            case SensorPreset::NAVIGATION:
                return {SensorPacket::ENCODERS, SensorPacket::VELOCITY, SensorPacket::RADIUS};
            // ...
        }
    }
};
```

## 🔌 Component Interfaces & Dependencies

### Dependency Graph
```
ArduRoomba (Facade)
├── ArduRoombaCore (no dependencies)
├── ArduRoombaSensors (depends on Core)
├── ArduRoombaCommands (depends on Core)
├── ArduRoombaWiFi (depends on Core, Sensors, Commands)
├── ArduRoombaConfig (depends on Storage)
├── ArduRoombaEvents (no dependencies)
└── ArduRoombaLogger (depends on Storage, Events)
```

### Interface Contracts
Each component defines clear interfaces with:
- **Input validation**: Parameter checking and sanitization
- **Error handling**: Comprehensive error codes and recovery
- **State management**: Internal state consistency
- **Event integration**: Publish relevant events
- **Configuration**: Respect configuration settings

## 📊 Memory Architecture

### Memory Layout Strategy
```
┌─────────────────────────────────────┐
│           Stack Memory              │ ← Local variables, function calls
├─────────────────────────────────────┤
│           Heap Memory               │ ← Dynamic allocations (minimal)
├─────────────────────────────────────┤
│         Static Variables            │ ← Component instances, buffers
├─────────────────────────────────────┤
│        Program Memory (Flash)       │ ← Code, constants, web files
└─────────────────────────────────────┘
```

### Memory Management Principles
1. **Minimize Dynamic Allocation**: Use static buffers where possible
2. **Pool Allocation**: Pre-allocate buffers for common operations
3. **Stack Optimization**: Minimize deep call stacks
4. **Flash Utilization**: Store static data in program memory
5. **Memory Monitoring**: Track usage and detect leaks

## 🔄 Event System Architecture

### Event Types & Flow
```cpp
enum class EventType {
    // System events
    SYSTEM_STARTUP,
    SYSTEM_SHUTDOWN,
    CONFIG_CHANGED,
    
    // Network events
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,
    CLIENT_CONNECTED,
    CLIENT_DISCONNECTED,
    
    // Robot events
    SENSOR_UPDATE,
    MOVEMENT_STARTED,
    MOVEMENT_STOPPED,
    CLEANING_STARTED,
    CLEANING_STOPPED,
    
    // Safety events
    CLIFF_DETECTED,
    BUMP_DETECTED,
    BATTERY_LOW,
    ERROR_OCCURRED
};
```

### Event Processing Pipeline
1. **Event Generation**: Components create events
2. **Event Queuing**: Events queued for processing
3. **Event Dispatch**: Events sent to subscribers
4. **Event Handling**: Subscribers process events
5. **Event Logging**: Events logged for debugging

## 🌐 Network Architecture

### WiFi Connection Management
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Station Mode  │    │  Access Point   │    │   Mesh Network  │
│                 │    │      Mode       │    │   (Future)      │
│ Connect to      │    │ Create hotspot  │    │ Multi-device    │
│ existing WiFi   │    │ for setup       │    │ coordination    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Web Server Architecture
```
HTTP Request → Router → Authentication → Rate Limiting → Handler → Response
                ↓
            CORS Headers → JSON Serialization → Client
```

### Real-time Communication Stack
```
WebSocket/SSE → Message Queue → Event System → Components
                     ↓
              Sensor Data → JSON → Client Updates
```

## 🔒 Security Architecture

### Multi-Layer Security Model
```
┌─────────────────────────────────────┐
│        Application Layer            │ ← Input validation, business logic
├─────────────────────────────────────┤
│        Authentication Layer         │ ← User auth, session management
├─────────────────────────────────────┤
│        Transport Layer              │ ← HTTPS, rate limiting
├─────────────────────────────────────┤
│        Network Layer                │ ← WiFi security, firewall
└─────────────────────────────────────┘
```

### Security Components
- **Authentication**: Multi-user support with secure sessions
- **Authorization**: Role-based access control
- **Rate Limiting**: DDoS protection and abuse prevention
- **Input Validation**: Sanitization and parameter checking
- **Secure Storage**: Encrypted configuration and credentials
- **Audit Logging**: Security event tracking

## 📈 Performance Architecture

### Performance Optimization Strategies
1. **Asynchronous Processing**: Non-blocking operations
2. **Efficient Serialization**: Optimized JSON generation
3. **Memory Pooling**: Pre-allocated buffers
4. **Caching**: Frequently accessed data
5. **Compression**: Reduce network traffic

### Monitoring & Metrics
```cpp
class PerformanceMonitor {
    void recordMetric(const String& name, float value);
    void startTimer(const String& operation);
    void endTimer(const String& operation);
    PerformanceReport getReport();
};
```

## 🧪 Testing Architecture

### Testing Pyramid
```
┌─────────────────┐
│   E2E Tests     │ ← Full system integration
├─────────────────┤
│ Integration     │ ← Component interaction
│     Tests       │
├─────────────────┤
│   Unit Tests    │ ← Individual components
└─────────────────┘
```

### Test Infrastructure
- **Mock Hardware**: Simulate robot responses
- **Test Fixtures**: Predefined test data
- **Automated Testing**: CI/CD integration
- **Performance Testing**: Load and stress testing
- **Hardware Testing**: Real robot validation

## 🔮 Future Architecture Considerations

### Scalability Enhancements
- **Microservices**: Split into smaller, focused services
- **Message Queuing**: Robust inter-component communication
- **Load Balancing**: Multiple robot coordination
- **Cloud Integration**: Hybrid edge/cloud architecture

### Advanced Features
- **Machine Learning**: On-device inference capabilities
- **Computer Vision**: Camera integration and processing
- **Voice Control**: Speech recognition and synthesis
- **Autonomous Navigation**: SLAM and path planning

### Ecosystem Integration
- **Plugin Architecture**: Third-party extensions
- **API Gateway**: Centralized API management
- **Service Discovery**: Dynamic service registration
- **Configuration Management**: Centralized config service

---

This architecture provides a solid foundation for current functionality while enabling future enhancements and scalability. The modular design ensures that components can be developed, tested, and deployed independently while maintaining system coherence and reliability.