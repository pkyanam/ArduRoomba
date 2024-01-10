# ArduRoomba

ArduRoomba is an Arduino library that enables interfacing with iRobot Create 2 and compatible iRobot Roomba models (500 series through 800 series non-WiFi models). Developed based on the iRobot Create 2 Open Interface (OI) Specification, this library is specifically designed for Arduino Uno R3 and Uno R4 Minima/WiFi.

## Features
- Control iRobot Roomba/Create 2 devices using Arduino.
- Based on the iRobot Create 2 Open Interface Specification.
- Compatible with Arduino Uno R3 and Uno R4 Minima/WiFi.

## Installation
There are two ways to install the ArduRoomba library:
1. **Via Arduino IDE's Library Manager**: Open your Arduino IDE, go to `Sketch` > `Include Library` > `Manage Libraries...`, then search for "ArduRoomba" and install it.
2. **Manual Installation**: Download this repository and include it in your Arduino IDE. For detailed installation instructions, refer to the [Arduino Library Installation Guide](https://www.arduino.cc/en/guide/libraries).

## How to Use
Before using the library, ensure your wiring is compatible with 5V logic levels, as the OI operates at these levels. Improper wiring may result in damage to your Arduino or iRobot unit. Detailed information on the iRobot serial connector can be found in the included PDF (Page 3).

### Wiring Diagram
The following wiring diagram is suitable for Arduino Uno R3, Arduino Uno R4 Minima, and Arduino Uno R4 WiFi:

![WiringDiagram](https://github.com/pkyanam/ArduRoomba/assets/37784174/cb9dd879-04ae-4499-ab68-aed5dfe68eef)

### Basic Usage
Include `ArduRoomba.h` in your sketch to use the library. Example sketches (`BasicUsage.ino` and `RemoteControl.ino`) are provided for reference.

## Compatibility
### IDEs:
- Arduino IDE (tested with version 2.X.X)

### Boards/MCUs:
- Arduino Uno R3
- Arduino Uno R4 Minima/WiFi
- Other boards/MCUs may be compatible but have not been tested.

### iRobot Roomba Models:
- Tested: iRobot Roomba 551 (Costco Wholesale model)
- Compatible (Untested): iRobot Create 2, iRobot Roomba 500, 600, 700, and 800 series (excluding WiFi models).

## Future Roadmap and Features
- [ ] ESP32/ESP8266 support (add support for 3.3v MCU/boards with disclaimer regarding voltage level shifting)
- [ ] Local Alexa integration (add support for Alexa to discover WiFi enabled MCU/boards over local network for smart assistant control)
- [ ] Expose more features from OI (add support to control more functions of iRobot unit and retrieve additional sensor information)
- [ ] More features may be announced and/or added in future

## Contributing
If you add additional features to this library and would like to merge your development, please create a pull request and I will take a look at it as soon as possible. If you test this library with other boards or iRobot models, please share your results by creating an issue in this repository.

## Authors and Acknowledgments
- Preetham Kyanam <preetham@preetham.org>

## License
This project is licensed under the GNU General Public License v3.0 (GPL-3.0). For more details, see the LICENSE file in this repository.

For additional information, visit the [project page](https://github.com/pkyanam/ArduRoomba).
