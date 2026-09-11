#ifndef ARDUROOMBA_HOST_ARDUINO_H
#define ARDUROOMBA_HOST_ARDUINO_H

#include <cstddef>
#include <cstdint>

class HardwareSerial {};

class HostSerial {
public:
  template <typename T> void print(const T&) {}
  template <typename T> void println(const T&) {}
};

extern HostSerial Serial;

inline void delay(uint32_t) {}

#endif
