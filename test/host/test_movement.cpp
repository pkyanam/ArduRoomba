#include <cassert>
#include <cstdint>
#include <vector>

#include "core/RoombaMovement.h"
#include "core/RoombaActuators.h"

HostSerial Serial;

class FakeRoombaSerial : public RoombaSerial {
public:
  std::vector<uint8_t> bytes;

  void begin(uint32_t) override {}
  void end() override {}
  void write(uint8_t value) override { bytes.push_back(value); }
  void write(const uint8_t* data, size_t len) override {
    bytes.insert(bytes.end(), data, data + len);
  }
  int available() override { return 0; }
  int read() override { return -1; }
  void flush() override {}
  bool isActive() const override { return true; }

  void clear() { bytes.clear(); }
};

static void expect(const std::vector<uint8_t>& actual,
                   std::initializer_list<uint8_t> expected) {
  assert(actual == std::vector<uint8_t>(expected));
}

int main() {
  FakeRoombaSerial serial;
  RoombaMovement movement(&serial);

  movement.moveForward(200);
  expect(serial.bytes, {OI_DRIVE, 0x00, 0xC8, 0x80, 0x00});

  serial.clear();
  movement.moveBackward(-200);
  expect(serial.bytes, {OI_DRIVE, 0xFF, 0x38, 0x80, 0x00});

  serial.clear();
  movement.drive(900, 3000);
  expect(serial.bytes, {OI_DRIVE, 0x01, 0xF4, 0x07, 0xD0});

  serial.clear();
  RoombaSequence sequence(&movement);
  sequence.forward(100, 0).delay(10).stop();
  assert(sequence.length() == 3);
  sequence.execute();
  assert(sequence.length() == 3);

  serial.clear();
  RoombaActuators actuators(&serial);
  actuators.setLED(true, true, true, true);
  expect(serial.bytes, {OI_LEDS, 0x39, 0x00, 0xFF});

  serial.clear();
  actuators.setMotors(true, true, true);
  expect(serial.bytes, {OI_MOTORS, 0x07});

  return 0;
}
