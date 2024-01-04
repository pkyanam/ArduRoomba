# ArduRoomba
This library built for the Arduino IDE allows an Arduino Uno to interface with the iRobot® Create 2 and compatible iRobot® Roombas (500 series through 800 series non-WiFi models)

Designed around the iRobot® Roomba 500 Open Interface (OI) Specification (included in PDF form as part of this repo). This spec corresponds with the iRobot Create 2 Open Interface.

Instructions for how to use and compatbility info can be found below.

## How to Use
Please refer to the information below before proceeding. Please know that improper wiring may result in damage to your Arduino and/or iRobot unit.

The OI operates at 5V logic levels therefore please ensure your specific board/MCU (if not using an Arduino Uno) is compatible with 5V logic levels.

If not, you WILL have to shift voltage levels or risk damage to your board/MCU. Please refer to Page 3 of the PDF for more detailed info on the iRobot serial connector.



##
This wiring diagram will work for Arduino Uno R3, Arduino Uno R4 Minima, and Arduino Uno R4 WiFi

![WiringDiagram](https://github.com/pkyanam/ArduRoomba/assets/37784174/cb9dd879-04ae-4499-ab68-aed5dfe68eef)

## Compatibilty
IDE(s):

	Arduino IDE (tested with version(s) 2.X.X)

Board(s) / MCU(s):

	Arduino Uno R3
  
	Arduino Uno R4

Other MCUs/boards MAY be compatible, but have not been tested. If you would like to test another board or MCU, please share your results by creating a new Issue for this repository.

iRobot Roomba model(s):

Tested:

	iRobot Roomba 551 (A model known to be sold at Costco Wholesale)

Untested (Non-WiFi from the following families should be compatible, but are not guaranteed to work. If you would like to test another robot from this list, please share your results by creating a new Issue for this repository):

	iRobot® Create 2
  
	iRobot® Roomba 500 series (excluding 551)
  
	iRobot® Roomba 600 series
  
	iRobot® Roomba 700 series
  
	iRobot® Roomba 800 series
  
