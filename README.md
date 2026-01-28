# ArduRoomba

**A modular, extensible Arduino library for controlling iRobot Roomba robots.**

[![GitHub](https://img.shields.io/github/license/pkyanam/ArduRoomba)](https://github.com/pkyanam/ArduRoomba)
[![GitHub release](https://img.shields.io/github/v/release/pkyanam/ArduRoomba)](https://github.com/pkyanam/ArduRoomba/releases)

Breathe new life into legacy Roomba robots. Complete iRobot Open Interface support with WiFi web control and Bluetooth LE for Arduino Uno R4, ESP32, ESP8266, and more.

## Features

- **Complete OI Protocol** - Full iRobot Open Interface implementation
- **Modular Architecture** - Clean separation of concerns with extensible components
- **Multiple Platforms** - Arduino Uno/Mega, Uno R4 WiFi, ESP32, ESP8266
- **WiFi Control** - Responsive web interface for AP and Client modes
- **Bluetooth LE** - Low-energy control for ESP32 and Uno R4 WiFi
- **Structured Sensors** - Organized sensor data with convenient access methods
- **Movement Sequences** - Fluent API for complex movement patterns
- **Safety Features** - Built-in battery monitoring and obstacle detection

## Table of Contents

- [Quick Start](#quick-start)
- [Architecture](#architecture)
- [Installation](#installation)
- [Hardware Setup](#hardware-setup)
- [Basic Usage](#basic-usage)
- [Advanced Features](#advanced-features)
- [WiFi Control](#wifi-control)
- [Bluetooth Control](#bluetooth-control)
- [API Reference](#api-reference)
- [Examples](#examples)
- [Platform Support](#platform-support)
- [Contributing](#contributing)
- [License](#license)

## Quick Start

```cpp
#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4); // RX, TX, BRC pins

void setup() {
  roomba.begin();
  roomba.moveForward(200); // 200 mm/s
  delay(2000);
  roomba.stop();
}

void loop() {}
```

## Architecture

ArduRoomba v4 uses a modular, layered architecture:

```
┌─────────────────────────────────────────────────────────────┐
│                        ArduRoomba                           │
│                    (Main User Interface)                    │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │RoombaMovement│  │RoombaSensors│  │RoombaActuators│        │
│  │             │  │             │  │              │          │
│  │ - Commands  │  │ - Bumpers   │  │ - LEDs       │          │
│  │ - Sequences │  │ - Cliffs    │  │ - Motors     │          │
│  │ - Drive     │  │ - Battery   │  │ - Sounds     │          │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│                     RoombaSerial                            │
│           (Serial Communication Abstraction)                 │
│  ┌────────────────────┐  ┌────────────────────┐            │
│  │ SoftwareSerial     │  │  HardwareSerial     │           │
│  │ (Uno/Mega)         │  │  (ESP32/ESP8266)    │            │
│  └────────────────────┘  └────────────────────┘            │
├─────────────────────────────────────────────────────────────┤
│                    Extensions                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │ WiFi Control │  │ BLE Control  │  │ (Future)     │        │
│  │ - ESP32      │  │ - ESP32      │  │              │         │
│  │ - ESP8266    │  │ - Uno R4     │  │              │         │
│  │ - Uno R4     │  │              │  │              │         │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└─────────────────────────────────────────────────────────────┘
```

## Installation

### Arduino Library Manager

1. Open Arduino IDE
2. Go to **Sketch** → **Include Library** → **Manage Libraries**
3. Search for "ArduRoomba"
4. Click **Install**

### Manual Installation

```bash
git clone https://github.com/pkyanam/ArduRoomba.git
# Copy to Arduino libraries folder
cp -r ArduRoomba ~/Documents/Arduino/libraries/
```

### PlatformIO

```ini
[env:uno]
lib_deps = pkyanam/ArduRoomba

[env:esp32]
lib_deps = pkyanam/ArduRoomba

[env:esp8266]
lib_deps = pkyanam/ArduRoomba
```

## Hardware Setup

### Standard Wiring (Arduino Uno/Mega)

| Roomba Mini-DIN | Arduino Pin |
|-----------------|-------------|
| TX (Pin 4)      | 2 (RX)      |
| RX (Pin 3)      | 3 (TX)      |
| DD (Pin 5)      | 4 (BRC)     |
| GND (Pin 6/7)   | GND         |

### ESP32 Wiring

| Roomba Mini-DIN | ESP32 Pin   |
|-----------------|-------------|
| TX (Pin 4)      | GPIO 16 (RX2)|
| RX (Pin 3)      | GPIO 17 (TX2)|
| DD (Pin 5)      | GPIO 5 (BRC) |
| GND (Pin 6/7)   | GND         |

### Uno R4 WiFi Wiring

Same as Arduino Uno/Mega (uses SoftwareSerial).

## Basic Usage

### Simple Movement

```cpp
#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4);

void setup() {
  roomba.begin();

  // Move forward at 200 mm/s for 2 seconds
  roomba.moveForward(200);
  delay(2000);

  // Stop
  roomba.stop();
}

void loop() {}
```

### Using RoombaConfig

```cpp
// For ESP32 with hardware serial
RoombaConfig config = RoombaConfig::createESP32(&Serial2, 5);
ArduRoomba roomba(config);

// For standard software serial
RoombaConfig config = RoombaConfig::createDefault(2, 3, 4);
ArduRoomba roomba(config);
```

### Accessing Components

```cpp
// Direct component access
roomba.sensors().readAll();        // Read all sensors
roomba.movement().drive(100, 200);  // Custom drive
roomba.actuators().beep();         // Play sound
```

## Advanced Features

### Movement Sequences

Chain movements together with the fluent API:

```cpp
// Create a sequence
RoombaSequence seq(roomba.movement());

seq.forward(200, 2000)   // 2 seconds forward
   .backward(150, 1000)  // 1 second backward
   .spinLeft(200, 1000)   // 1 second spin
   .stop()
   .execute();

// Or use the helper
RoombaSequenceBuilder(roomba)
  .forward(150, 1000)
  .right(150, 500)
  .forward(150, 1000)
  .stop()
  .execute();
```

### Structured Sensor Data

```cpp
// Read all sensors at once
RoombaSensorData data = roomba.sensors().readAll();

// Access structured data
if (data.bumper.anyBumper()) {
  Serial.println("Bumper hit!");
}

if (data.battery.isLow()) {
  Serial.println("Battery low!");
}

// Or use convenience methods
if (roomba.isBumperPressed()) {
  roomba.stop();
}

uint8_t batteryPercent = roomba.getBatteryPercent();
```

### LED and Sound Control

```cpp
// Set individual LEDs
roomba.setLED(true, false, false, false); // Debris LED on
roomba.setPowerLED(128, 255);             // Yellow at full brightness

// Play predefined songs
roomba.actuators().playStartupSong();
roomba.actuators().playHappySong();
roomba.actuators().playSadSong();
roomba.actuators().playAlertSong();

// Define custom songs
SongNote song[] = {
  SongNote(NOTE_C4, 16),
  SongNote(NOTE_E4, 16),
  SongNote(NOTE_G4, 16)
};
roomba.actuators().defineSong(0, song, 3);
roomba.actuators().playSong(0);
```

## WiFi Control

### ESP32 WiFi

```cpp
#include "ArduRoomba.h"
#include "extensions/ArduRoombaESP32WiFi.h"

RoombaConfig config = RoombaConfig::createESP32(&Serial2, 5);
ArduRoomba roomba(config);
ArduRoombaESP32WiFi wifi(roomba);

void setup() {
  roomba.begin();

  // Create AP mode
  wifi.beginAP("ArduRoomba", "password123");
  wifi.startWebServer(80);

  Serial.print("Control at: http://");
  Serial.println(wifi.getIPAddress());
}

void loop() {
  wifi.handleClient();
}
```

### ESP8266 WiFi

```cpp
#include "ArduRoomba.h"
#include "extensions/ArduRoombaESP8266WiFi.h"

ArduRoomba roomba(4, 5, 2); // RX, TX, BRC for ESP8266
ArduRoombaESP8266WiFi wifi(roomba);

void setup() {
  roomba.begin();
  wifi.beginAP("ArduRoomba", "password123");
  wifi.startWebServer(80);
}

void loop() {
  wifi.handleClient();
}
```

### Uno R4 WiFi

```cpp
#include "ArduRoomba.h"
#include "extensions/ArduRoombaWiFiS3.h"

ArduRoomba roomba(2, 3, 4);
ArduRoombaWiFiS3 wifi(roomba);

void setup() {
  roomba.begin();
  wifi.beginAP("ArduRoomba", "password123");
  wifi.startWebServer(80);
}

void loop() {
  wifi.handleClient();
}
```

### WiFi Web Interface

All WiFi implementations provide a responsive web interface:

- Directional controls (forward, back, left, right, spin)
- Adjustable speed slider
- Cleaning mode buttons (Clean, Spot, Dock)
- Real-time battery and sensor status

**HTTP Endpoints:**

| Endpoint | Description |
|----------|-------------|
| `GET /` | Web control interface |
| `GET /cmd?action=X&speed=Y` | Execute command |
| `GET /status` | JSON status response |

**Example curl command:**

```bash
curl "http://192.168.4.1/cmd?action=forward&speed=250"
```

## Bluetooth Control

### ESP32 BLE

```cpp
#include "ArduRoomba.h"
#include "extensions/ArduRoombaBLE.h"

RoombaConfig config = RoombaConfig::createESP32(&Serial2, 5);
ArduRoomba roomba(config);
ArduRoombaBLE ble(roomba, "ArduRoomba");

void setup() {
  roomba.begin();
  ble.begin();
}

void loop() {
  ble.updateStatus(); // Must call in loop
}
```

### Uno R4 WiFi BLE

```cpp
#include "ArduRoomba.h"
#include "extensions/ArduRoombaBLE.h"

ArduRoomba roomba(2, 3, 4);
ArduRoombaBLE ble(roomba, "ArduRoomba");

void setup() {
  roomba.begin();
  ble.begin();
}

void loop() {
  ble.updateStatus();
}
```

### BLE Protocol

**Command Format:** `action:speed:duration`

Examples:
- `forward:200:0` - Move forward continuously
- `left:150:1000` - Turn left for 1 second
- `stop:0:0` - Stop

**Status Format:** `voltage:connected:wall:bumper:cliff:remote`

**BLE UUIDs:**
- Service: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`
- Command: `beb5483e-36e1-4688-b7f5-ea07361b26a8` (Write)
- Status: `beb5483f-36e1-4688-b7f5-ea07361b26a8` (Read/Notify)

## API Reference

### Main Class: ArduRoomba

#### Lifecycle

| Method | Description |
|--------|-------------|
| `ArduRoomba(rx, tx, brc)` | Constructor with pins |
| `ArduRoomba(config)` | Constructor with config |
| `begin()` | Initialize connection |
| `end()` | Close connection |
| `isConnected()` | Connection status |

#### Movement

| Method | Description |
|--------|-------------|
| `moveForward(speed)` | Move forward (0-500 mm/s) |
| `moveBackward(speed)` | Move backward |
| `turnLeft(speed)` | Turn left (arc) |
| `turnRight(speed)` | Turn right (arc) |
| `spinLeft(speed)` | Spin in place left |
| `spinRight(speed)` | Spin in place right |
| `stop()` | Stop movement |

#### Sensors

| Method | Description |
|--------|-------------|
| `getBatteryVoltage()` | Voltage in mV |
| `getBatteryCurrent()` | Current in mA |
| `getBatteryPercent()` | Battery percentage |
| `isBatteryLow()` | Below 13V |
| `isBatteryCritical()` | Below 12V |
| `isBumperPressed()` | Any bumper hit |
| `isWallDetected()` | Wall sensor |
| `isCliffDetected()` | Any cliff sensor |

#### Actuators

| Method | Description |
|--------|-------------|
| `setLED(d, s, dock, check)` | Set indicator LEDs |
| `setPowerLED(color, intensity)` | Set power LED color |
| `setMotors(main, side, vac)` | Control brushes |
| `beep()` | Play beep |
| `startCleaning()` | Start clean mode |
| `spotClean()` | Spot clean |
| `dock()` | Return to dock |

### Component Classes

#### RoombaMovement

```cpp
roomba.movement().drive(velocity, radius);
roomba.movement().driveDirect(leftVel, rightVel);
roomba.movement().moveForward(speed, duration); // Timed
```

#### RoombaSensors

```cpp
BumperData b = roomba.sensors().readBumpers();
CliffData c = roomba.sensors().readCliffs();
WallData w = roomba.sensors().readWalls();
BatteryData bat = roomba.sensors().readBattery();
RoombaSensorData all = roomba.sensors().readAll();
```

#### RoombaActuators

```cpp
roomba.actuators().setAllLEDs(bits, color, intensity);
roomba.actuators().defineSong(num, notes, count);
roomba.actuators().playSong(num);
roomba.actuators().setSafeMode();
roomba.actuators().setFullMode();
```

## Examples

| Example | Description |
|---------|-------------|
| `BasicMovement` | Simple movement commands |
| `AdvancedMovement` | Sequences and patterns |
| `SensorReading` | All sensor data types |
| `SimpleControl` | Serial command interface |
| `WiFiControl_UnoR4` | WiFi for Uno R4 |
| `WiFiControl_ESP32` | WiFi for ESP32 |
| `WiFiControl_ESP8266` | WiFi for ESP8266 |
| `BLEControl_ESP32` | BLE for ESP32 |
| `BLEControl_UnoR4` | BLE for Uno R4 |

## Platform Support

| Board | Basic | WiFi | BLE | Notes |
|-------|-------|------|-----|-------|
| Arduino Uno | ✅ | ❌ | ❌ | SoftwareSerial |
| Arduino Mega | ✅ | ❌ | ❌ | SoftwareSerial |
| Arduino Nano | ✅ | ❌ | ❌ | SoftwareSerial |
| Uno R4 WiFi | ✅ | ✅ | ✅ | WiFiS3 + ArduinoBLE |
| ESP32 | ✅ | ✅ | ✅ | HardwareSerial |
| ESP8266 | ✅ | ✅ | ❌ | HardwareSerial |
| ESP8266 (NodeMCU/D1) | ✅ | ✅ | ❌ | Pin mapping varies |

### Supported Roomba Models

- iRobot Create 2
- Roomba 500 series
- Roomba 600 series
- Roomba 700 series
- Roomba 800 series (basic support)

## Safety Features

The library includes built-in safety features:

```cpp
// Enable automatic safety checks
roomba.enableSafety(true);

// Call in loop for automatic safety
void loop() {
  roomba.updateSafety(); // Checks battery, bumpers, cliffs
}

// Customize thresholds
RoombaConfig config;
config.lowBatteryThreshold = 13000;     // mV
config.criticalBatteryThreshold = 12000; // mV
config.enableSafety = true;
```

Safety behaviors:
- **Low battery warning** - Below 13V
- **Critical battery stop** - Below 12V
- **Bumper auto-stop** - Stops on impact
- **Cliff avoidance** - Backs up from cliffs

## Troubleshooting

### Roomba won't connect

1. Check wiring (TX↔RX, RX↔TX, GND↔GND)
2. Verify Roomba is powered on (press CLEAN button)
3. Check BRC pin connection
4. Try resetting both Arduino and Roomba

### WiFi won't start

1. Check platform-specific library installation
2. Verify SSID/password is correct
3. For client mode, check 2.4GHz WiFi support

### BLE not advertising

1. Ensure platform supports BLE (Uno R4, ESP32)
2. Check for name conflicts with nearby devices
3. Try using nRF Connect app to scan for device

## Contributing

Contributions are welcome! Areas for contribution:

- Additional platform support
- New sensor integrations
- Improved web UI
- Additional examples
- Documentation improvements

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## License

MIT License - See [LICENSE](LICENSE) file for details.

Free for commercial and non-commercial use. Build cool things!

## Changelog

### v4.0.0 (Current)
- **Major refactor** with modular architecture
- **ESP8266 WiFi support** added
- **Uno R4 WiFi BLE support** added
- **Movement sequences** with fluent API
- **Structured sensor data** types
- **Improved safety features**
- **Better examples and documentation**

### v3.1.0
- Initial WiFi and BLE support for ESP32

### v3.0.0
- Initial public release

## Credits

- **Preetham Kyanam** - Author and Maintainer
- **Contributors** - See [GitHub contributors](https://github.com/pkyanam/ArduRoomba/graphs/contributors)

## Related Projects

- [iRobot Open Interface Spec](https://www.irobotweb.com/about-irobot/Corporate-Information)
- [Create 2 Documentation](https://www.irobotweb.com/about-irobot/roomba)
- [Roomba hacking community](https://www.robotreviews.com/)

---

**Built with ❤️ for robot enthusiasts**
