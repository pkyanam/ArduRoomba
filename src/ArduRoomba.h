#ifndef ArduRoomba_h
#define ArduRoomba_h

#include <Arduino.h>
#include <SoftwareSerial.h>

class ArduRoomba {
  public:
    // Constructor
    ArduRoomba(int rxPin, int txPin, int brcPin);

    // Initialization
    void roombaSetup();

    // Command functions
    void goForward();
    void goBackward();
    void turnLeft();
    void turnRight();
    void halt();

    // Add more methods related to battery management and serial communications here
    void getEncoderCounts();
    void getBatteryStatus();
    bool shouldSeekDock();
    void seekDock();

  private:
    const byte _zero = 0x00;
    unsigned int _batteryCharge;
    unsigned int _batteryCapacity;
    int _rxPin, _txPin, _brcPin;
    SoftwareSerial _irobot; // SoftwareSerial instance for communication

    // Add private helper methods and variables here
};

#endif
