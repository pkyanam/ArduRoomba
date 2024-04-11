#ifndef ArduRoomba_h
#define ArduRoomba_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define ARDUROOMBA_REFRESH_DELAY 40
#define ARDUROOMBA_STREAM_TIMEOUT 16 // stream time slot = 15ms

#define ARDUROOMBA_STREAM_WAIT_HEADER 0
#define ARDUROOMBA_STREAM_WAIT_SIZE 1
#define ARDUROOMBA_STREAM_WAIT_CONTENT 2
#define ARDUROOMBA_STREAM_WAIT_CHECKSUM 3
#define ARDUROOMBA_STREAM_END 4


#define ARDUROOMBA_SENSOR_BUMPANDWEELSDROPS 7
#define ARDUROOMBA_SENSOR_WALL 8
#define ARDUROOMBA_SENSOR_CLIFFLEFT 9
#define ARDUROOMBA_SENSOR_CLIFFFRONTLEFT 10
#define ARDUROOMBA_SENSOR_CLIFFFRONTRIGHT 11
#define ARDUROOMBA_SENSOR_CLIFFRIGHT 12
#define ARDUROOMBA_SENSOR_VIRTUALWALL 13
#define ARDUROOMBA_SENSOR_WHEELOVERCURRENTS 14
#define ARDUROOMBA_SENSOR_DIRTDETECT 15
#define ARDUROOMBA_SENSOR_IROPCODE 17
#define ARDUROOMBA_SENSOR_BUTTONS 18
#define ARDUROOMBA_SENSOR_CHARGINGSTATE 21
#define ARDUROOMBA_SENSOR_VOLTAGE 22
#define ARDUROOMBA_SENSOR_CURRENT 23
#define ARDUROOMBA_SENSOR_TEMPERATURE 24
#define ARDUROOMBA_SENSOR_BATTERYCHARGE 25
#define ARDUROOMBA_SENSOR_BATTERYCAPACITY 26
#define ARDUROOMBA_SENSOR_WALLSIGNAL 27
#define ARDUROOMBA_SENSOR_CLIFFLEFTSIGNAL 28
#define ARDUROOMBA_SENSOR_CLIFFFRONTLEFTSIGNAL 29
#define ARDUROOMBA_SENSOR_CLIFFFRONTRIGHTSIGNAL 30
#define ARDUROOMBA_SENSOR_CLIFFRIGHTSIGNAL 31
#define ARDUROOMBA_SENSOR_CHARGERAVAILABLE 34
#define ARDUROOMBA_SENSOR_MODE 35
#define ARDUROOMBA_SENSOR_SONGNUMBER 36
#define ARDUROOMBA_SENSOR_SONGPLAYING 37
#define ARDUROOMBA_SENSOR_IOSTREAMNUMPACKETS 38
#define ARDUROOMBA_SENSOR_VELOCITY 39
#define ARDUROOMBA_SENSOR_RADIUS 40
#define ARDUROOMBA_SENSOR_RIGHTVELOCITY 41
#define ARDUROOMBA_SENSOR_LEFTVELOCITY 42
#define ARDUROOMBA_SENSOR_LEFTENCODERCOUNTS 43
#define ARDUROOMBA_SENSOR_RIGHTENCODERCOUNTS 44
#define ARDUROOMBA_SENSOR_LIGHTBUMPER 45
#define ARDUROOMBA_SENSOR_LIGHTBUMPLEFTSIGNAL 46
#define ARDUROOMBA_SENSOR_LIGHTBUMPFRONTLEFTSIGNAL 47
#define ARDUROOMBA_SENSOR_LIGHTBUMPCENTERLEFTSIGNAL 48
#define ARDUROOMBA_SENSOR_LIGHTBUMPCENTERRIGHTSIGNAL 49
#define ARDUROOMBA_SENSOR_LIGHTBUMPFRONTRIGHTSIGNAL 50
#define ARDUROOMBA_SENSOR_LIGHTBUMPRIGHTSIGNAL 51
#define ARDUROOMBA_SENSOR_INFRAREDCHARACTERLEFT 52
#define ARDUROOMBA_SENSOR_INFRAREDCHARACTERRIGHT 53
#define ARDUROOMBA_SENSOR_LEFTMOTORCURRENT 54
#define ARDUROOMBA_SENSOR_RIGHTMOTORCURRENT 55
#define ARDUROOMBA_SENSOR_MAINBRUSHMOTORCURRENT 56
#define ARDUROOMBA_SENSOR_SIDEBRUSHMOTORCURRENT 57
#define ARDUROOMBA_SENSOR_STASIS 58

class ArduRoomba
{
public:
  // Constructor
  ArduRoomba(int rxPin, int txPin, int brcPin); // Constructor

  // Custom structs to use in main code.
  struct Note
  {
    byte noteNumber;   // MIDI note number (31 - 127)
    byte noteDuration; // Duration of the note in 1/64th of a second
  };

  struct Song
  {
    byte songNumber; // Song number (0 - 4)
    byte songLength; // Number of notes in the song (1 - 16)
    Note notes[16];  // Array of notes, up to 16
  };

  struct RoombaInfos {
    long nextRefresh;       // time of next update
    long lastSuccedRefresh; // time of last successfull update
    int  attempt;           // number of failed attempts since last success

    byte irOpcode;
    byte songNumber;
    byte ioStreamNumPackets;
    byte mode;
    byte chargingState;
    byte infraredCharacterLeft;
    byte infraredCharacterRight;
    char temperature;
    
    int voltage;
    int current;
    
    int batteryCapacity;
    int batteryCharge;
    int dirtdetect;
    int velocity;
    int rightVelocity;
    int leftVelocity;
    int radius;
    int leftEncoderCounts;
    int rightEncoderCounts;
    int leftMotorCurrent;
    int rightMotorCurrent;
    int mainBrushMotorCurrent;
    int sideBrushMotorCurrent;

    unsigned int wallSignal;
    unsigned int cliffLeftSignal;
    unsigned int cliffFrontLeftSignal;
    unsigned int cliffRightSignal;
    unsigned int cliffFrontRightSignal;
    unsigned int lightBumpLeftSignal;
    unsigned int lightBumpFrontLeftSignal;
    unsigned int lightBumpCenterLeftSignal;
    unsigned int lightBumpCenterRightSignal;
    unsigned int lightBumpFrontRightSignal;
    unsigned int lightBumpRightSignal;

    bool wall;
    bool virtualWall;
    bool cliffLeft;
    bool cliffFrontLeft;
    bool cliffRight;
    bool cliffFrontRight;
    bool songPlaying;


    //ARDUROOMBA_SENSOR_LIGHTBUMPER
    bool lightBumperLeft;
    bool lightBumperFrontLeft;
    bool lightBumperCenterLeft;
    bool lightBumperCenterRight;
    bool lightBumperFrontRight;
    bool lightBumperRight;
  
    // ARDUROOMBA_SENSOR_CHARGERAVAILABLE
    bool internalChargerAvailable;
    bool homeBaseChargerAvailable;

    // ARDUROOMBA_SENSOR_STASIS
    bool stasisDisabled;
    bool stasisToggling;

    // ARDUROOMBA_SENSOR_BUTTONS
    bool cleanButton;
    bool spotButton;
    bool dockButton;
    bool minuteButton;
    bool hourButton;
    bool dayButton;
    bool scheludeButton;
    bool clockButton;

    // ARDUROOMBA_SENSOR_WHEELOVERCURRENTS
    bool wheelRightOvercurrent;
    bool wheelLeftOvercurrent;
    bool mainBrushOvercurrent;
    bool sideBrushOvercurrent;
    bool vacuumOvercurrent;       // no wacuum for serie 600

    // ARDUROOMBA_SENSOR_WHEELOVERCURRENTS
    bool bumpRight;
    bool bumpLeft;
    bool wheelDropRight;
    bool wheelDropLeft;
  };

  struct ScheduleStore
  {
    byte days;
    byte sunHour;
    byte sunMinute;
    byte monHour;
    byte monMinute;
    byte tueHour;
    byte tueMinute;
    byte wedHour;
    byte wedMinute;
    byte thuHour;
    byte thuMinute;
    byte friHour;
    byte friMinute;
    byte satHour;
    byte satMinute;
  };

  // OI commands
  void start();                                      // Start the OI
  void baud(char baudCode);                          // Set the baud rate
  void safe();                                       // Put the OI into Safe mode
  void full();                                       // Put the OI into Full mode
  void clean();                                      // Start the cleaning mode
  void maxClean();                                   // Start the maximum time cleaning mode
  void spot();                                       // Start the spot cleaning mode
  void seekDock();                                   // Send the robot to the dock
  void schedule(ScheduleStore scheduleData);         // Set the schedule
  void setDayTime(char day, char hour, char minute); // Set the day and time
  void power();                                      // Power down the OI

  // Actuator commands
  void drive(int velocity, int radius);                                         // Drive the robot
  void driveDirect(int rightVelocity, int leftVelocity);                        // Drive the robot directly
  void drivePWM(int rightPWM, int leftPWM);                                     // Drive the robot with PWM
  void motors(byte data);                                                       // Control the motors
  void pwmMotors(char mainBrushPWM, char sideBrushPWM, char vacuumPWM);         // Control the PWM of the motors
  void leds(int ledBits, int powerColor, int powerIntensity);                   // Control the LEDs
  void schedulingLeds(int weekDayLedBits, int scheduleLedBits);                 // Control the scheduling LEDs
  void digitLedsRaw(int digitThree, int digitTwo, int digitOne, int digitZero); // Control the digit LEDs
  void song(Song songData);                                                     // Load a song
  void play(int songNumber);                                                    // Play a song

  // Input commands
  void sensors(char packetID);                      // Request a sensor packet
  void queryList(byte numPackets, byte *packetIDs); // Request a list of sensor packets

  void queryStream(char sensorlist[]);              // Request a list of sensor packets to stream
  void resetStream();                               // Request an empty list of sensor packets to stream
  bool refreshData(RoombaInfos *infos);             // Read stream slot

  // Custom commands
  void roombaSetup(); // Setup the Roomba
  void goForward();   // Move the Roomba forward
  void goBackward();  // Move the Roomba backward
  void turnLeft();    // Turn the Roomba left
  void turnRight();   // Turn the Roomba right
  void halt();        // Stop the Roomba

private:
  const byte _zero = 0x00;
  int _rxPin, _txPin, _brcPin;
  SoftwareSerial _irobot; // SoftwareSerial instance for communication with the Roomba
  
  byte _streamBuffer[100] = {}; 
  int _nbSensorsStream = 0; // number of requested sensors stream
  int _streamBufferCursor = 0; 
  char _sensorsStream[60]; // max 52 sensors in OpenInterface spec

  int _sensorsListLength(char sensorlist[]); // determines the size of the table
  bool _readStream(); // read stream data and fill _streamBuffer and return checksum result
  bool _parseStreamBuffer(byte *packets, int len, RoombaInfos *infos); // parse _streamBuffer and return checksum result
  byte _parseOneByteStreamBuffer(byte *packets, int &start);
  int _parseTwoByteStreamBuffer(byte *packets, int &start);
  void _parseAndFillOneByteStreamBuffer(byte *packets, int &start, bool *bytes);
};

#endif
