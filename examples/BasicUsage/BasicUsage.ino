#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4); // rxPin, txPin, brcPin

void setup()
{
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
