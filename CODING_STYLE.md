# ArduRoomba Coding Style Guide

## Overview
This document defines the coding standards for the ArduRoomba library to ensure consistency, maintainability, and AI agent comprehension throughout the codebase.

## File Organization

### Header Files (.h)
```cpp
/**
 * @file FileName.h
 * @brief Brief description of the file's purpose
 * 
 * Detailed description explaining the file's role in the library architecture,
 * its dependencies, and how it fits into the overall system. This helps both
 * human developers and AI agents understand the context and purpose.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef FILENAME_H
#define FILENAME_H

// System includes first
#include <Arduino.h>
#include <SoftwareSerial.h>

// Library includes second
#include "ArduRoombaConstants.h"
#include "ArduRoombaTypes.h"

namespace ArduRoomba {

/**
 * @brief Class description with context for AI agents
 * 
 * Detailed explanation of the class purpose, its role in the system,
 * and how it interacts with other components. Include usage patterns
 * and important behavioral notes.
 */
class ClassName {
public:
    // ========================================================================
    // CONSTRUCTOR AND DESTRUCTOR
    // ========================================================================
    
    /**
     * @brief Constructor description
     * @param param1 Description of parameter
     * @param param2 Description of parameter
     */
    ClassName(Type param1, Type param2);
    
    /**
     * @brief Destructor
     */
    ~ClassName();
    
    // ========================================================================
    // PUBLIC INTERFACE
    // ========================================================================
    
    // Group related methods together with clear section headers
    
private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    // Member variables with clear naming and documentation
    Type _memberVariable;  ///< Description of member variable
};

} // namespace ArduRoomba

#endif // FILENAME_H
```

### Implementation Files (.cpp)
```cpp
/**
 * @file FileName.cpp
 * @brief Implementation of ClassName
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#include "FileName.h"

namespace ArduRoomba {

// ============================================================================
// CONSTRUCTOR AND DESTRUCTOR
// ============================================================================

ClassName::ClassName(Type param1, Type param2)
    : _memberVariable(param1) {
    // Constructor implementation with clear initialization
}

ClassName::~ClassName() {
    // Destructor implementation
}

// ============================================================================
// PUBLIC INTERFACE IMPLEMENTATION
// ============================================================================

// Method implementations with clear section organization

} // namespace ArduRoomba
```

## Naming Conventions

### Classes
- **PascalCase**: `ArduRoomba`, `RoombaCore`, `SensorData`
- **Descriptive**: Names should clearly indicate purpose
- **Context-aware**: Include domain context for AI understanding

### Methods
- **camelCase**: `initialize()`, `getSensorData()`, `setDebugEnabled()`
- **Verb-based**: Start with action verbs
- **Clear intent**: Method name should indicate what it does

### Variables
- **camelCase for public**: `baudRate`, `sensorData`
- **Underscore prefix for private**: `_core`, `_debugEnabled`, `_lastError`
- **Descriptive**: Avoid abbreviations unless widely understood

### Constants
- **UPPER_SNAKE_CASE**: `MAX_PACKET_SIZE`, `DEFAULT_BAUD_RATE`
- **Namespace scoped**: Use namespace to avoid global pollution

### Enums
- **PascalCase for enum name**: `ErrorCode`, `SensorPacket`
- **UPPER_CASE for values**: `SUCCESS`, `TIMEOUT`, `INVALID_PARAMETER`

## Code Organization

### Section Headers
Use consistent section headers to organize code:
```cpp
// ============================================================================
// MAJOR SECTION (80 chars)
// ============================================================================

// ========================================================================
// SUBSECTION (72 chars)
// ========================================================================
```

### Method Organization
1. Constructor/Destructor
2. Initialization methods
3. Core functionality (grouped by purpose)
4. Utility methods
5. Legacy compatibility methods
6. Private methods

### Include Order
1. Corresponding header file (for .cpp files)
2. System includes (`<Arduino.h>`, `<SoftwareSerial.h>`)
3. Library includes (other ArduRoomba headers)
4. Third-party includes

## Documentation Standards

### Doxygen Comments
- Use `/**` for class and method documentation
- Use `///` for inline member documentation
- Include `@brief`, `@param`, `@return`, `@note` as appropriate

### AI Agent Context
Include contextual information that helps AI agents understand:
- **Purpose**: Why this code exists
- **Dependencies**: What it relies on
- **Usage patterns**: How it's typically used
- **State management**: How state is maintained
- **Error conditions**: What can go wrong

### Example Documentation
```cpp
/**
 * @brief Initializes communication with the Roomba device
 * 
 * This method establishes serial communication with the iRobot Open Interface
 * and puts the device into a controllable state. It handles baud rate
 * negotiation and initial handshaking. This is a critical initialization
 * step that must succeed before any other operations can be performed.
 * 
 * AI Context: This method is the entry point for all Roomba interactions.
 * It sets up the hardware communication layer and establishes the protocol
 * state machine. Failure here indicates hardware connection issues.
 * 
 * @param baudRate Communication speed (default: 115200)
 * @return ErrorCode::SUCCESS on successful initialization
 * @return ErrorCode::TIMEOUT if device doesn't respond
 * @return ErrorCode::INVALID_PARAMETER if baudRate is unsupported
 * 
 * @note Must be called before any other Roomba operations
 * @note Automatically handles baud rate change sequence
 */
ErrorCode initialize(uint32_t baudRate = 115200);
```

## Error Handling

### Error Codes
- Use enum class for type safety
- Provide meaningful error codes
- Include error context in documentation

### Error Propagation
- Return error codes from methods that can fail
- Provide both error code and legacy boolean interfaces
- Log errors when debug mode is enabled

## Memory Management

### Arduino Constraints
- Minimize dynamic allocation
- Use stack allocation where possible
- Be mindful of SRAM usage on smaller boards

### Resource Management
- Use RAII principles
- Clean up resources in destructors
- Avoid memory leaks

## Platform Compatibility

### Conditional Compilation
```cpp
#ifdef ESP32
    // ESP32-specific code
#elif defined(ESP8266)
    // ESP8266-specific code
#else
    // Arduino Uno/generic code
#endif
```

### Feature Detection
- Use capability flags for optional features
- Graceful degradation on limited platforms
- Clear documentation of platform requirements

## Testing and Validation

### Example Integration
- Every public method should have example usage
- Examples should demonstrate error handling
- Include both basic and advanced usage patterns

### Debug Support
- Provide debug output for troubleshooting
- Use consistent debug message format
- Allow runtime enable/disable of debug output

## Version Management

### Version Consistency
- Keep all files synchronized to same version
- Update version in library.properties
- Document version changes in examples

### Backward Compatibility
- Maintain legacy API alongside new features
- Clearly mark deprecated methods
- Provide migration guidance

This style guide ensures the ArduRoomba library maintains high code quality,
excellent documentation, and provides the contextual information needed for
both human developers and AI agents to understand and extend the codebase.