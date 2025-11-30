# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ArduRoomba is a minimal, focused Arduino library for iRobot Open Interface (OI) communication. It provides essential functionality for controlling iRobot Create 2 and Roomba 500/600/700 series robots without bloat or over-engineering.

**Design Philosophy:**
- Minimal core with essential functionality only
- Smart modularity with clean interfaces for future extensions
- Simple, readable code without over-engineering
- Extensible architecture for WiFi, Bluetooth, RTOS when needed

## Architecture

The library uses a two-layer architecture:

### Layer 1: RoombaOI (Low-level Protocol)
`src/RoombaOI.h/.cpp` - Direct implementation of the iRobot Open Interface protocol.

- Handles serial communication via SoftwareSerial
- Manages BRC (Baud Rate Change) pin for wake-up sequences
- Implements raw OI commands (opcodes 128-148)
- Low-level sensor reading and data parsing
- Safe/Full mode control
- Returns raw sensor data

Key implementation details:
- Uses SoftwareSerial for communication (19200 baud default)
- BRC pin pulsing required for initialization (3 pulses, 100ms HIGH/LOW)
- Initialization sequence: pulse BRC → START command → SAFE mode
- Velocity range: -500 to 500 mm/s (clamped internally)
- Drive constants: DRIVE_STRAIGHT (32768), DRIVE_TURN_CCW (1), DRIVE_TURN_CW (-1)

### Layer 2: ArduRoomba (High-level Interface)
`src/ArduRoomba.h/.cpp` - User-friendly wrapper providing convenient methods.

- Simple movement commands (moveForward, turnLeft, stop, etc.)
- Convenient sensor access with meaningful return types
- Actuator control (brushes, LEDs, sound)
- Debug logging capability
- Exposes underlying RoombaOI via `getOI()` for advanced use

The two-layer design allows:
- Simple usage for common tasks via ArduRoomba
- Direct protocol access via RoombaOI for advanced features
- Future extensions without modifying the core

## Development Workflow

### Library Structure
This is an Arduino library following the standard Arduino library format:
- `library.properties` - Library metadata (name, version, supported architectures)
- `keywords.txt` - Syntax highlighting keywords for Arduino IDE
- `src/` - Library source code (.h and .cpp files)
- `examples/` - Example sketches (.ino files)

### Testing Changes
Since this is an Arduino library, testing requires actual hardware:

1. **With Arduino IDE:**
   - Install library in Arduino/libraries/ folder
   - Open examples via File → Examples → ArduRoomba
   - Upload to Arduino board connected to Roomba

2. **With Arduino CLI:**
   ```bash
   # Compile an example
   arduino-cli compile --fqbn arduino:avr:uno examples/BasicMovement

   # Upload to board
   arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno examples/BasicMovement
   ```

3. **Validation checks without hardware:**
   ```bash
   # Verify library compiles for different architectures
   arduino-cli compile --fqbn arduino:avr:uno examples/BasicMovement
   arduino-cli compile --fqbn arduino:renesas_uno:unor4 examples/BasicMovement
   arduino-cli compile --fqbn esp32:esp32:esp32 examples/BasicMovement
   ```

### Hardware Setup
Standard wiring for examples:
- Roomba TX → Arduino Pin 2 (RX)
- Roomba RX → Arduino Pin 3 (TX)
- Roomba DD → Arduino Pin 4 (BRC)
- Common ground between Arduino and Roomba

Serial monitor should be set to 19200 baud to see debug output.

## Common Sensor Packet IDs

When adding new sensor functionality, reference these OI sensor packet IDs (defined in RoombaOI.h):
- Packet 7: Bumps and wheel drops
- Packet 8: Wall sensor
- Packets 9-12: Cliff sensors
- Packet 18: Buttons
- Packet 21: Charging state
- Packet 22: Voltage (returns 2 bytes, big-endian)
- Packet 23: Current (returns 2 bytes, signed big-endian)
- Packet 25/26: Battery charge/capacity

All multi-byte sensor values are transmitted in big-endian format (high byte first).

## Key Implementation Patterns

### Adding New Movement Commands
Extend ArduRoomba class with high-level method that calls RoombaOI drive commands:
```cpp
// In ArduRoomba.cpp
void ArduRoomba::newMovement() {
  debugPrint("Description");
  _oi.drive(velocity, radius); // or _oi.driveDirect(right, left)
}
```

### Adding New Sensor Methods
1. Add to RoombaOI for raw sensor access
2. Wrap in ArduRoomba for convenient access with bool/int returns

Example pattern from existing code:
```cpp
// RoombaOI.cpp - Raw sensor reading
uint16_t RoombaOI::getBatteryVoltage() {
  uint8_t data[2];
  if (getSensor(SENSOR_VOLTAGE, data, 2)) {
    return (data[0] << 8) | data[1]; // Big-endian conversion
  }
  return 0;
}

// ArduRoomba.cpp - Convenient wrapper
uint16_t ArduRoomba::getBatteryVoltage() {
  return _oi.getBatteryVoltage();
}
```

### Debug Output Pattern
Both classes use consistent debug logging:
```cpp
void debugPrint(const char* msg);           // Message only
void debugPrint(const char* msg, int value); // Message + value
```
Debug output prefixed with class name ("RoombaOI: " or "ArduRoomba: ") and only prints when `_debug` flag is enabled.

## Wireless Extensions Architecture

The `src/extensions/` directory contains modular wireless control extensions. Extensions do not modify the core RoombaOI or ArduRoomba classes - they compose with them.

### Layer 3: Wireless Control Extensions

#### ArduRoombaWiFi (Base Class)
`src/extensions/ArduRoombaWiFi.h/.cpp` - Abstract base class for WiFi control

- Defines common interface for AP and Client modes
- HTTP web server framework with control page generation
- Command processing with callback support
- Status JSON generation
- Platform-agnostic command structure

**Platform-Specific Implementations:**

1. **ArduRoombaWiFiS3** (`ArduRoombaWiFiS3.h/.cpp`)
   - For Arduino Uno R4 WiFi only (uses `ARDUINO_UNOWIFIR4` define)
   - Uses WiFiS3 library (Renesas RA4M1 WiFi module)
   - Manual HTTP request parsing (no WebServer library)
   - Supports both AP and Client modes

2. **ArduRoombaESP32WiFi** (`ArduRoombaESP32WiFi.h/.cpp`)
   - For ESP32 boards only (uses `ESP32` define)
   - Uses ESP32 WiFi and WebServer libraries
   - Clean route-based HTTP handling
   - Supports both AP and Client modes

#### ArduRoombaBLE (ESP32 Only)
`src/extensions/ArduRoombaBLE.h/.cpp` - Bluetooth Low Energy control for ESP32

- BLE GATT server implementation
- Standard UUIDs for mobile app compatibility
- Command characteristic (write) and Status characteristic (read/notify)
- Auto-reconnect advertising
- Command format: "action:speed:duration" (e.g., "forward:200:1000")
- Status format: "voltage:connected:wall:bumper:remote" (e.g., "15800:1:0:0:1")

**BLE Service Structure:**
- Service UUID: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`
- Command Char: `beb5483e-36e1-4688-b7f5-ea07361b26a8` (Write)
- Status Char: `beb5483f-36e1-4688-b7f5-ea07361b26a8` (Read/Notify)

### Using Extensions

Extensions compose with ArduRoomba instances:

```cpp
ArduRoomba roomba(2, 3, 4);
ArduRoombaWiFiS3 wifi(roomba);  // WiFi extension wraps roomba

void setup() {
  roomba.begin();
  wifi.beginAP("ArduRoomba", "password");
  wifi.startWebServer();
}

void loop() {
  wifi.handleClient();  // Extension handles web requests
}
```

### WiFi Extension Endpoints

All WiFi implementations provide these HTTP endpoints:
- `GET /` - HTML control page (responsive web UI)
- `GET /cmd?action=forward&speed=200&duration=1000` - Execute command
- `GET /status` - JSON status response

### Adding New Extensions

Future extensions should follow this pattern:
1. Compose with ArduRoomba (don't inherit or modify core)
2. Use platform defines (`#if defined(...)`) for platform-specific code
3. Provide consistent API across platforms where possible
4. Include examples demonstrating usage

## Smart Home Adapters Architecture

The `src/adapters/` directory contains smart home platform integrations. Adapters are separate from wireless extensions - they provide integration with external smart home ecosystems.

### Layer 4: Smart Home Adapters

#### ArduRoombaSmartHome (Base Class)
`src/adapters/ArduRoombaSmartHome.h` - Abstract base class for smart home integrations

- Defines common interface for all smart home adapters
- Composes with ArduRoomba (same pattern as extensions)
- Provides standard command enum (`SmartHomeCommand`)
- State tracking and reporting (`RoombaState` struct)
- Command execution with callback support
- Debug logging

**Key Methods:**
```cpp
virtual bool begin() = 0;          // Initialize adapter
virtual void end() = 0;            // Shutdown adapter
virtual void handle() = 0;         // Process events (call in loop)
virtual bool isConnected() const = 0;
RoombaState getState();            // Get current Roomba state
void enableRemoteControl(bool);    // Enable/disable commands
void setCommandCallback(void (*)(SmartHomeCommand));
```

#### ArduRoombaHomeAssistant
`src/adapters/ArduRoombaHomeAssistant.h/.cpp` - Home Assistant MQTT integration

- MQTT protocol with PubSubClient library
- Auto-discovery for automatic entity creation in Home Assistant
- Vacuum entity with start/stop/dock/locate commands
- Sensor entities: battery percentage, wall detection, bumper state
- Real-time state updates at configurable intervals
- Supports ESP32, ESP8266, Arduino Uno R4 WiFi

**Required Library:** PubSubClient (by Nick O'Leary)

**MQTT Topics:**
- `{device_id}/state` - JSON state payload
- `{device_id}/command` - Command input (start, stop, return_to_base, locate)
- `{device_id}/availability` - Online/offline status
- `{device_id}/battery` - Battery percentage

**Discovery Topics (auto-configured):**
- `homeassistant/vacuum/{device_id}/config`
- `homeassistant/sensor/{device_id}_battery/config`
- `homeassistant/binary_sensor/{device_id}_wall/config`
- `homeassistant/binary_sensor/{device_id}_bumper/config`

#### ArduRoombaAlexa
`src/adapters/ArduRoombaAlexa.h/.cpp` - Amazon Alexa voice control

- Uses fauxmoESP library (Belkin WeMo emulation)
- Local network control (no cloud services required)
- Multiple virtual devices for different commands
- Zero configuration on Alexa side
- Supports ESP32 and ESP8266 only (fauxmoESP requirement)

**Required Library:** fauxmoESP (by Xose Pérez)

**Default Voice Commands:**
- "Alexa, turn on Roomba" - Start cleaning
- "Alexa, turn off Roomba" - Stop
- "Alexa, turn on Roomba Dock" - Return to base
- "Alexa, turn on Roomba Spot" - Spot clean
- "Alexa, turn on Roomba Locate" - Beep

**Customization:**
```cpp
alexa.setMainDeviceName("Living Room Vacuum");
alexa.setDockDeviceName("Dock Vacuum");
alexa.enableExtraDevices(false);  // Main device only
alexa.addDevice("Custom", onCallback, offCallback);  // Custom device
```

### Using Smart Home Adapters

```cpp
// Home Assistant example
ArduRoomba roomba(16, 17, 5);
ArduRoombaHomeAssistant ha(roomba);

void setup() {
  WiFi.begin("SSID", "password");
  roomba.begin();
  ha.setDeviceId("my_roomba");
  ha.begin("mqtt.local", 1883, "user", "pass");
}

void loop() {
  ha.handle();
}

// Alexa example
ArduRoomba roomba(16, 17, 5);
ArduRoombaAlexa alexa(roomba);

void setup() {
  WiFi.begin("SSID", "password");
  roomba.begin();
  alexa.begin("Roomba");
}

void loop() {
  alexa.handle();
}
```

### Adding New Adapters

New smart home adapters should:
1. Inherit from `ArduRoombaSmartHome` base class
2. Implement all pure virtual methods
3. Use platform defines for platform-specific code
4. Call `executeCommand()` to process received commands
5. Use `getState()` for status reporting
6. Include example sketch demonstrating usage

## Supported Architectures

Defined in library.properties:
- `avr` - Arduino Uno R3, Mega, etc.
- `renesas_uno` - Arduino Uno R4
- `esp8266` - ESP8266-based boards
- `esp32` - ESP32-based boards

Changes should maintain compatibility with all supported architectures.
