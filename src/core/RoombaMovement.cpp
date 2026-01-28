/**
 * @file RoombaMovement.cpp
 * @brief Implementation of Roomba movement control
 */

#include "RoombaMovement.h"

// RoombaMovement implementation
RoombaMovement::RoombaMovement(RoombaSerial* serial)
  : _serial(serial), _debug(false), _currentDirection(Direction::STOP), _currentVelocity(0) {
}

void RoombaMovement::stop() {
  drive(0, 0);
  _currentDirection = Direction::STOP;
  _currentVelocity = 0;
  debugPrint("Stopped");
}

void RoombaMovement::moveForward(int16_t speed) {
  clampVelocity(speed);
  drive(speed, DRIVE_STRAIGHT);
  _currentDirection = Direction::FORWARD;
  _currentVelocity = speed;
  debugPrint("Moving forward", speed);
}

void RoombaMovement::moveBackward(int16_t speed) {
  clampVelocity(speed);
  drive(-speed, DRIVE_STRAIGHT);
  _currentDirection = Direction::BACKWARD;
  _currentVelocity = -speed;
  debugPrint("Moving backward", speed);
}

void RoombaMovement::turnLeft(int16_t speed) {
  clampVelocity(speed);
  drive(speed, DRIVE_TURN_CCW);
  _currentDirection = Direction::LEFT;
  _currentVelocity = speed;
  debugPrint("Turning left", speed);
}

void RoombaMovement::turnRight(int16_t speed) {
  clampVelocity(speed);
  drive(speed, DRIVE_TURN_CW);
  _currentDirection = Direction::RIGHT;
  _currentVelocity = speed;
  debugPrint("Turning right", speed);
}

void RoombaMovement::spinLeft(int16_t speed) {
  clampVelocity(speed);
  driveDirect(speed, -speed);
  _currentDirection = Direction::SPIN_LEFT;
  _currentVelocity = speed;
  debugPrint("Spinning left", speed);
}

void RoombaMovement::spinRight(int16_t speed) {
  clampVelocity(speed);
  driveDirect(-speed, speed);
  _currentDirection = Direction::SPIN_RIGHT;
  _currentVelocity = speed;
  debugPrint("Spinning right", speed);
}

void RoombaMovement::drive(int16_t velocity, int16_t radius) {
  clampVelocity(velocity);

  uint8_t params[4];
  params[0] = (velocity >> 8) & 0xFF;  // High byte
  params[1] = velocity & 0xFF;         // Low byte
  params[2] = (radius >> 8) & 0xFF;    // High byte
  params[3] = radius & 0xFF;           // Low byte

  sendCommand(OI_DRIVE, params, 4);
}

void RoombaMovement::driveDirect(int16_t rightVel, int16_t leftVel) {
  clampVelocity(rightVel);
  clampVelocity(leftVel);

  uint8_t params[4];
  params[0] = (rightVel >> 8) & 0xFF;  // Right high byte
  params[1] = rightVel & 0xFF;         // Right low byte
  params[2] = (leftVel >> 8) & 0xFF;   // Left high byte
  params[3] = leftVel & 0xFF;          // Left low byte

  sendCommand(OI_DRIVE_DIRECT, params, 4);
}

void RoombaMovement::moveForward(int16_t speed, uint32_t duration) {
  moveForward(speed);
  if (duration > 0) {
    delay(duration);
    stop();
  }
}

void RoombaMovement::moveBackward(int16_t speed, uint32_t duration) {
  moveBackward(speed);
  if (duration > 0) {
    delay(duration);
    stop();
  }
}

void RoombaMovement::turnLeft(int16_t speed, uint32_t duration) {
  turnLeft(speed);
  if (duration > 0) {
    delay(duration);
    stop();
  }
}

void RoombaMovement::turnRight(int16_t speed, uint32_t duration) {
  turnRight(speed);
  if (duration > 0) {
    delay(duration);
    stop();
  }
}

void RoombaMovement::spinLeft(int16_t speed, uint32_t duration) {
  spinLeft(speed);
  if (duration > 0) {
    delay(duration);
    stop();
  }
}

void RoombaMovement::spinRight(int16_t speed, uint32_t duration) {
  spinRight(speed);
  if (duration > 0) {
    delay(duration);
    stop();
  }
}

void RoombaMovement::execute(const MovementCommand& cmd) {
  switch (cmd.direction) {
    case Direction::FORWARD:
      moveForward(cmd.velocity, cmd.duration);
      break;
    case Direction::BACKWARD:
      moveBackward(cmd.velocity, cmd.duration);
      break;
    case Direction::LEFT:
      turnLeft(cmd.velocity, cmd.duration);
      break;
    case Direction::RIGHT:
      turnRight(cmd.velocity, cmd.duration);
      break;
    case Direction::SPIN_LEFT:
      spinLeft(cmd.velocity, cmd.duration);
      break;
    case Direction::SPIN_RIGHT:
      spinRight(cmd.velocity, cmd.duration);
      break;
    case Direction::STOP:
      stop();
      break;
  }
}

void RoombaMovement::clampVelocity(int16_t& velocity) {
  if (velocity > MAX_VELOCITY) velocity = MAX_VELOCITY;
  if (velocity < MIN_VELOCITY) velocity = MIN_VELOCITY;
}

void RoombaMovement::sendCommand(uint8_t cmd) {
  if (_serial && _serial->isActive()) {
    _serial->write(cmd);
  }
}

void RoombaMovement::sendCommand(uint8_t cmd, const uint8_t* params, uint8_t numParams) {
  if (_serial && _serial->isActive() && params) {
    _serial->write(cmd);
    _serial->write(params, numParams);
  }
}

void RoombaMovement::debugPrint(const char* msg) {
  if (_debug && msg) {
    Serial.print("RoombaMovement: ");
    Serial.println(msg);
  }
}

void RoombaMovement::debugPrint(const char* msg, int value) {
  if (_debug && msg) {
    Serial.print("RoombaMovement: ");
    Serial.print(msg);
    Serial.print(" = ");
    Serial.println(value);
  }
}

// RoombaSequence implementation
RoombaSequence::RoombaSequence(RoombaMovement* movement)
  : _movement(movement), _count(0) {
}

RoombaSequence::~RoombaSequence() {
}

RoombaSequence& RoombaSequence::forward(int16_t speed, uint32_t duration) {
  return add(MovementCommand(Direction::FORWARD, speed, DRIVE_STRAIGHT, duration));
}

RoombaSequence& RoombaSequence::backward(int16_t speed, uint32_t duration) {
  return add(MovementCommand(Direction::BACKWARD, speed, DRIVE_STRAIGHT, duration));
}

RoombaSequence& RoombaSequence::left(int16_t speed, uint32_t duration) {
  return add(MovementCommand(Direction::LEFT, speed, DRIVE_TURN_CCW, duration));
}

RoombaSequence& RoombaSequence::right(int16_t speed, uint32_t duration) {
  return add(MovementCommand(Direction::RIGHT, speed, DRIVE_TURN_CW, duration));
}

RoombaSequence& RoombaSequence::spinLeft(int16_t speed, uint32_t duration) {
  return add(MovementCommand(Direction::SPIN_LEFT, speed, 0, duration));
}

RoombaSequence& RoombaSequence::spinRight(int16_t speed, uint32_t duration) {
  return add(MovementCommand(Direction::SPIN_RIGHT, speed, 0, duration));
}

RoombaSequence& RoombaSequence::stop() {
  return add(MovementCommand(Direction::STOP, 0, 0, 0));
}

RoombaSequence& RoombaSequence::delay(uint32_t ms) {
  if (_count < MAX_COMMANDS) {
    _commands[_count].direction = Direction::STOP;
    _commands[_count].velocity = 0;
    _commands[_count].radius = 0;
    _commands[_count].duration = ms;
    _count++;
  }
  return *this;
}

RoombaSequence& RoombaSequence::add(const MovementCommand& cmd) {
  if (_count < MAX_COMMANDS) {
    _commands[_count] = cmd;
    _count++;
  }
  return *this;
}

void RoombaSequence::execute() {
  for (uint8_t i = 0; i < _count; i++) {
    const MovementCommand& cmd = _commands[i];

    if (cmd.direction == Direction::STOP && cmd.velocity == 0 && cmd.duration > 0) {
      // Just a delay
      delay(cmd.duration);
    } else {
      _movement->execute(cmd);
    }
  }
}

void RoombaSequence::clear() {
  _count = 0;
}
