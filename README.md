# ArduRoomba

ArduRoomba is a library for Arduino that facilitates interfacing with iRobot Create 2 devices and compatible iRobot Roombas (500 series through 800 series non-WiFi models). This library is tested with Uno R3 and Uno R4 Minima/WiFi, and it is based on the iRobot Roomba 500 Open Interface (OI) Specification.

## Features
- Control iRobot Roomba/Create 2 devices using Arduino.
- Utilizes the iRobot Roomba 500 series Open Interface Specification.
- Compatible with Arduino Uno R3 and Uno R4 Minima/WiFi.

## Installation
To install the ArduRoomba library, download this repository and include it in your Arduino IDE. For more detailed instructions, refer to the [Arduino Library Installation Guide](https://www.arduino.cc/en/guide/libraries).

## How to Use
Before proceeding, ensure your wiring is correct and compatible with 5V logic levels. Improper wiring may damage your Arduino or iRobot unit. Refer to Page 3 of the included PDF for detailed information on the iRobot serial connector.

### Wiring Diagram
This wiring diagram is suitable for Arduino Uno R3, Arduino Uno R4 Minima, and Arduino Uno R4 WiFi:

![WiringDiagram](https://github.com/pkyanam/ArduRoomba/assets/37784174/cb9dd879-04ae-4499-ab68-aed5dfe68eef)

### Basic Usage
To use the library, include `ArduRoomba.h` in your sketch. Refer to the included example sketches (`BasicUsage.ino` and `RemoteControl.ino`) for usage examples.

## Compatibility
### IDEs:
- Arduino IDE (tested with version 2.X.X)

### Boards/MCUs:
- Arduino Uno R3
- Arduino Uno R4
- Other boards/MCUs may be compatible but are not tested.

### iRobot Roomba Models:
- Tested: iRobot Roomba 551 (Costco Wholesale model)
- Untested but potentially compatible: iRobot Create 2, iRobot Roomba 500, 600, 700, and 800 series (excluding WiFi models).

## Contributing
If you test this library with other boards or iRobot models, please share your results by creating an issue in this repository.

## Authors and Acknowledgments
- Preetham Kyanam <preetham@preetham.org>

## License
This project is released under the [MIT License](https://opensource.org/licenses/MIT). For more details, see the LICENSE file in this repository.

For more information, visit the [project page](https://github.com/pkyanam/ArduRoomba).

