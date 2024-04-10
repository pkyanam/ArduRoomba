#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4); // rxPin, txPin, brcPin
ArduRoomba::RoombaInfos infos = {};
ArduRoomba::RoombaInfos update = {};

  // warning don't request to many sensors
  // stream data time slot = 15ms
  // if the roomba doesn't have time to return all the sensor's data
  // the stream will be unstable
  char sensorlist[] = {ARDUROOMBA_SENSOR_MODE,
                       ARDUROOMBA_SENSOR_TEMPERATURE,
                       ARDUROOMBA_SENSOR_VOLTAGE,
                       ARDUROOMBA_SENSOR_BATTERYCHARGE,
                       ARDUROOMBA_SENSOR_BUMPANDWEELSDROPS,
                       ARDUROOMBA_SENSOR_WALL,
                       ARDUROOMBA_SENSOR_CLIFFLEFT,
                       ARDUROOMBA_SENSOR_CLIFFFRONTLEFT,
                       ARDUROOMBA_SENSOR_CLIFFRIGHT,
                       ARDUROOMBA_SENSOR_CLIFFFRONTRIGHT};

void setup() {
  Serial.begin(19200);
  roomba.roombaSetup();
  roomba.safe();

  roomba.resetStream();
  roomba.queryStream(sensorlist);
}

void loop() {
  if (roomba.refreshData(&update)) {
    printDiffAndHydrate(&infos,&update);
  } else if (update.attempt > 2) {
    long noSerialDuration = millis() - update.lastSuccedRefresh;
    if (noSerialDuration > 300) {
      Serial.print("NO SERIAL (");
      Serial.print(noSerialDuration, DEC);
      Serial.println("ms)");
    }
    delay(100);
  }
}

void printDiffAndHydrate(ArduRoomba::RoombaInfos *a, ArduRoomba::RoombaInfos *b) {
  if (b->mode != a->mode) {
    a->mode = b->mode;
    Serial.print("mode = ");
    Serial.println(a->mode);
  }
  if (b->wall != a->wall) {
    a->wall = b->wall;
    Serial.print("wall = ");
    Serial.println(a->wall);
  }
  if (b->cliffLeft != a->cliffLeft) {
    a->cliffLeft = b->cliffLeft;
    Serial.print("cliffLeft = ");
    Serial.println(a->cliffLeft);
  }
  if (b->cliffFrontLeft != a->cliffFrontLeft) {
    a->cliffFrontLeft = b->cliffFrontLeft;
    Serial.print("cliffFrontLeft = ");
    Serial.println(a->cliffFrontLeft);
  }
  if (b->cliffRight != a->cliffRight) {
    a->cliffRight = b->cliffRight;
    Serial.print("cliffRight = ");
    Serial.println(a->cliffRight);
  }
  if (b->cliffFrontRight != a->cliffFrontRight) {
    a->cliffFrontRight = b->cliffFrontRight;
    Serial.print("cliffFrontRight = ");
    Serial.println(a->cliffFrontRight);
  }
  if (b->voltage != a->voltage) {
    a->voltage = b->voltage;
    Serial.print("voltage = ");
    Serial.println(a->voltage);
  }
  if (b->batteryCharge != a->batteryCharge) {
    a->batteryCharge = b->batteryCharge;
    Serial.print("batteryCharge = ");
    Serial.println(a->batteryCharge);
  }
  if (b->temperature != a->temperature) {
    a->temperature = b->temperature;
    Serial.print("temperature = ");
    Serial.println(a->temperature, DEC); // print temperature as an ASCII-encoded decimal ( range -128 to 127)
  }
  if (a->bumpRight != b->bumpRight) {
    a->bumpRight = b->bumpRight;
    Serial.print("bumpRight = ");
    Serial.println(a->bumpRight);
  }
  if (a->bumpLeft != b->bumpLeft) {
    a->bumpLeft = b->bumpLeft;
    Serial.print("bumpLeft = ");
    Serial.println(a->bumpLeft);
  }
  if (a->wheelDropRight != b->wheelDropRight) {
    a->wheelDropRight = b->wheelDropRight;
    Serial.print("wheelDropRight = ");
    Serial.println(a->wheelDropRight);
  }
  if (a->wheelDropLeft != b->wheelDropLeft) {
    a->wheelDropLeft = b->wheelDropLeft;
    Serial.print("wheelDropLeft = ");
    Serial.println(a->wheelDropLeft);
  }
}