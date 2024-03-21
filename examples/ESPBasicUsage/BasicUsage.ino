#include "ArduRoomba.h"

// Example tested for ESP01 board, see docs/esp01-wiring.png
ArduRoomba roomba(3, 1, 2); // rxPin, txPin, brcPin

void setup() {
  Serial.begin(19200);
  roomba.roombaSetup();

  // Additional setup code
  roomba.goForward();
  delay(500);
  roomba.goBackward();
  delay(500);
  roomba.turnLeft();
  delay(500);
  roomba.turnRight();
  delay(500);
  roomba.halt();
}

void loop()
{
  //
}
