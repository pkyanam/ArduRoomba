#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4); // rxPin, txPin, brcPin

void setup()
{
  Serial.begin(19200);
  roomba.roombaSetup();

  // Additional setup code
}

void loop()
{
  // Check if data is available to read
  if (Serial.available() > 0)
  {
    // Serial.println("serial available check complete");
    //  Read the incoming byte:
    char incomingByte = Serial.read();

    // Compare incoming byte to commands and call corresponding function
    switch (incomingByte)
    {
    case 'w': // Assuming 'w' is the command to go forward
      // Serial.println("inside w switch");
      roomba.halt();
      delay(50);
      roomba.goForward();
      delay(50);
      Serial.println("Sent goForward to OI");
      break;
    case 's': // Assuming 's' is the command to go backward
      roomba.halt();
      delay(50);
      roomba.goBackward();
      delay(50);
      Serial.println("Sent goBackward to OI");
      break;
    case 'a': // Assuming 'a' is the command to turn left
      roomba.halt();
      delay(50);
      roomba.turnLeft();
      delay(50);
      Serial.println("Sent turnLeft to OI");
      break;
    case 'd': // Assuming 'd' is the command to turn right
      roomba.halt();
      delay(50);
      roomba.turnRight();
      delay(50);
      Serial.println("Sent turnRight to OI");
      break;
    case 'e': // Assuming 'e' is the command to stop
      roomba.halt();
      delay(50);
      Serial.println("Sent halt to OI");
      break;
    case 'r':
      roomba.roombaSetup();
    default:
      // If the incoming byte doesn't match any command, you can handle it here
      break;
    }
  }
}
