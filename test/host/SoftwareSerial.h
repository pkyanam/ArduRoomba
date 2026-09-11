#ifndef ARDUROOMBA_HOST_SOFTWARE_SERIAL_H
#define ARDUROOMBA_HOST_SOFTWARE_SERIAL_H

#include <cstddef>
#include <cstdint>

class SoftwareSerial {
public:
  SoftwareSerial(uint8_t, uint8_t) {}
  void begin(uint32_t) {}
  void end() {}
  size_t write(uint8_t) { return 1; }
  size_t write(const uint8_t*, size_t len) { return len; }
  int available() { return 0; }
  int read() { return -1; }
  void flush() {}
};

#endif
