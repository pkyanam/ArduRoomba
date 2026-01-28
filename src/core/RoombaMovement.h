/**
 * @file RoombaMovement.h
 * @brief Roomba movement control with fluent API
 *
 * Provides high-level movement commands with chainable API
 * for complex movement sequences.
 */

#ifndef ROOMBA_MOVEMENT_H
#define ROOMBA_MOVEMENT_H

#include <Arduino.h>
#include "RoombaSerial.h"

/**
 * Movement direction enumeration
 */
enum class Direction : uint8_t {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  SPIN_LEFT,
  SPIN_RIGHT,
  STOP
};

/**
 * Movement command structure
 */
struct MovementCommand {
  Direction direction;
  int16_t velocity;
  int16_t radius;
  uint32_t duration;

  MovementCommand() : direction(Direction::STOP), velocity(0), radius(0), duration(0) {}

  MovementCommand(Direction dir, int16_t vel = 200, int16_t rad = DRIVE_STRAIGHT, uint32_t dur = 0)
    : direction(dir), velocity(vel), radius(rad), duration(dur) {}
};

/**
 * Roomba movement control interface
 * Provides fluent API for movement commands
 */
class RoombaMovement {
public:
  explicit RoombaMovement(RoombaSerial* serial);
  ~RoombaMovement() = default;

  // Basic immediate commands
  void stop();
  void moveForward(int16_t speed = 200);
  void moveBackward(int16_t speed = 200);
  void turnLeft(int16_t speed = 200);
  void turnRight(int16_t speed = 200);
  void spinLeft(int16_t speed = 200);
  void spinRight(int16_t speed = 200);

  // Low-level drive commands
  void drive(int16_t velocity, int16_t radius);
  void driveDirect(int16_t rightVel, int16_t leftVel);

  // Timed movements (blocking)
  void moveForward(int16_t speed, uint32_t duration);
  void moveBackward(int16_t speed, uint32_t duration);
  void turnLeft(int16_t speed, uint32_t duration);
  void turnRight(int16_t speed, uint32_t duration);
  void spinLeft(int16_t speed, uint32_t duration);
  void spinRight(int16_t speed, uint32_t duration);

  // Execute movement command
  void execute(const MovementCommand& cmd);

  // Debug
  void setDebug(bool enable) { _debug = enable; }

  // Get current movement state
  Direction getCurrentDirection() const { return _currentDirection; }
  int16_t getCurrentVelocity() const { return _currentVelocity; }

private:
  RoombaSerial* _serial;
  bool _debug;
  Direction _currentDirection;
  int16_t _currentVelocity;

  // Helper methods
  void clampVelocity(int16_t& velocity);
  void sendCommand(uint8_t cmd);
  void sendCommand(uint8_t cmd, const uint8_t* params, uint8_t numParams);
  void debugPrint(const char* msg);
  void debugPrint(const char* msg, int value);
};

/**
 * Fluent movement builder for complex sequences
 * Example:
 *   RoombaSequence(sequence)
 *     .forward(200, 1000)
 *     .left(150, 500)
 *     .forward(200, 1000)
 *     .stop()
 *     .execute();
 */
class RoombaSequence {
public:
  explicit RoombaSequence(RoombaMovement* movement);
  ~RoombaSequence();

  // Chainable movement commands
  RoombaSequence& forward(int16_t speed = 200, uint32_t duration = 0);
  RoombaSequence& backward(int16_t speed = 200, uint32_t duration = 0);
  RoombaSequence& left(int16_t speed = 200, uint32_t duration = 0);
  RoombaSequence& right(int16_t speed = 200, uint32_t duration = 0);
  RoombaSequence& spinLeft(int16_t speed = 200, uint32_t duration = 0);
  RoombaSequence& spinRight(int16_t speed = 200, uint32_t duration = 0);
  RoombaSequence& stop();
  RoombaSequence& delay(uint32_t ms);

  // Add custom command
  RoombaSequence& add(const MovementCommand& cmd);

  // Execute the sequence
  void execute();

  // Clear the sequence
  void clear();

  // Get sequence length
  size_t length() const { return _count; }

private:
  static const uint8_t MAX_COMMANDS = 32;
  RoombaMovement* _movement;
  MovementCommand _commands[MAX_COMMANDS];
  uint8_t _count;
};

#endif
