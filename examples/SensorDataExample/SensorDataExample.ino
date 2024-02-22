#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4); // rxPin, txPin, brcPin

void setup()
{
    Serial.begin(19200);
    roomba.roombaSetup();
}

void loop()
{
    // Read and print the sensor data (occurs every 15ms)
    roomba.sensors(100);
}
