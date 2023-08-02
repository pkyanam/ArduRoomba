# ArduRoomba
This library built for the Arduino IDE allows an Arduino Uno to interface with the 500 family of iRobot Roombas via 
Designed around the iRobot ® Roomba 500 Open Interface (OI) Specification.
Instructions for how to use and compatbility info can be found below.

## How to Use
Please refer to the information below before proceeding. Please know that improper wiring may result in damage to your Arduino and/or Roomba unit.
The OI operates at 5V logic levels therefore please ensure your specific board/MCU (if not using an Arduino Uno) is compatible with 5V logic levels.
If not, you WILL have to shift voltage levels or risk damage to your board/MCU.
![image](https://github.com/pkyanam/ArduRoomba/assets/37784174/051c9dfa-7be5-499a-87cb-5ec8794372a9)

## Compatibilty
IDE(s):
Arduino IDE (tested with version 2.1.1)

Board(s) / MCU(s):
Arduino Uno (tested with R3)
Other MCUs/boards MAY be compatible, but have not been tested. If you would like to test another board or MCU, please share your results by creating a new Issue for this repository.

iRobot Roomba model(s):
iRobot Roomba 551 (A model known to be sold at Costco Wholesale)
Other Roomba models part of the 500 family MAY be compatible, but have not been tested. This library is designed to be compatible with the Roomba 500 Open Interface.
