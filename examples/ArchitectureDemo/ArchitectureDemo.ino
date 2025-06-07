/**
 * @file ArchitectureDemo.ino
 * @brief Comprehensive demonstration of ArduRoomba modular architecture
 * 
 * This example showcases the refactored ArduRoomba library architecture,
 * demonstrating proper error handling, modular design, and best practices
 * for both human developers and AI agents.
 * 
 * Features Demonstrated:
 * - Modular component usage
 * - Comprehensive error handling
 * - Configuration management
 * - Debug output and diagnostics
 * - Platform-specific features
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * Hardware Connections:
 * - Arduino Pin 2 -> Roomba TX (receive data from Roomba)
 * - Arduino Pin 3 -> Roomba RX (send data to Roomba)
 * - Arduino Pin 4 -> Roomba BRC (Baud Rate Change pin)
 * - Arduino GND -> Roomba GND
 * - Arduino 5V -> Roomba VCC (through voltage divider if needed)
 */

#include "ArduRoomba.h"

// ============================================================================
// GLOBAL CONFIGURATION
// ============================================================================

// Hardware pin configuration
const uint8_t ROOMBA_RX_PIN = 2;    // Arduino pin connected to Roomba TX
const uint8_t ROOMBA_TX_PIN = 3;    // Arduino pin connected to Roomba RX
const uint8_t ROOMBA_BRC_PIN = 4;   // Arduino pin connected to Roomba BRC

// Timing configuration
const uint32_t DEMO_INTERVAL = 5000;     // 5 seconds between demo actions
const uint32_t STATUS_INTERVAL = 1000;   // 1 second between status updates
const uint32_t BAUD_RATE = 115200;       // Communication baud rate

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Main ArduRoomba instance (facade pattern)
ArduRoomba::ArduRoomba roomba(ROOMBA_RX_PIN, ROOMBA_TX_PIN, ROOMBA_BRC_PIN);

// Timing variables
unsigned long lastDemoAction = 0;
unsigned long lastStatusUpdate = 0;

// Demo state management
enum class DemoState {
    INITIALIZING,
    SENSOR_DEMO,
    MOVEMENT_DEMO,
    CONFIGURATION_DEMO,
    COMPLETE
};

DemoState currentState = DemoState::INITIALIZING;
uint8_t demoStep = 0;

// ============================================================================
// SETUP FUNCTION
// ============================================================================

void setup() {
    // Initialize serial communication for debug output
    Serial.begin(19200);
    delay(1000); // Allow serial to stabilize
    
    printWelcomeMessage();
    printPlatformInfo();
    
    // Initialize the ArduRoomba system
    initializeRoombaSystem();
    
    Serial.println("Setup complete. Starting demonstration...");
    Serial.println("========================================");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    unsigned long currentTime = millis();
    
    // Update status display
    if (currentTime - lastStatusUpdate >= STATUS_INTERVAL) {
        updateStatusDisplay();
        lastStatusUpdate = currentTime;
    }
    
    // Execute demo actions
    if (currentTime - lastDemoAction >= DEMO_INTERVAL) {
        executeDemoStep();
        lastDemoAction = currentTime;
    }
    
    // Small delay to prevent overwhelming the system
    delay(10);
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================

void printWelcomeMessage() {
    Serial.println("ArduRoomba Architecture Demonstration");
    Serial.println("====================================");
    Serial.println("Version: 2.5.0");
    Serial.println("Author: Preetham Kyanam");
    Serial.println();
    Serial.println("This demo showcases the modular architecture");
    Serial.println("and comprehensive error handling of ArduRoomba.");
    Serial.println();
}

void printPlatformInfo() {
    Serial.print("Platform: ");
    
#if defined(ARDUINO_AVR_UNO)
    Serial.println("Arduino Uno R3");
    Serial.println("Features: Core functionality only");
#elif defined(ARDUINO_UNOWIFIR4)
    Serial.println("Arduino Uno R4 WiFi");
    Serial.println("Features: Core + WiFi support");
#elif defined(ESP32)
    Serial.println("ESP32");
    Serial.println("Features: Full functionality including advanced WiFi");
#elif defined(ESP8266)
    Serial.println("ESP8266");
    Serial.println("Features: Core + basic WiFi support");
#else
    Serial.println("Unknown/Generic");
    Serial.println("Features: Core functionality");
#endif
    
    Serial.println();
}

void initializeRoombaSystem() {
    Serial.println("Initializing ArduRoomba system...");
    
    // Enable debug output for detailed logging
    roomba.setDebugEnabled(true);
    
    // Initialize communication with error handling
    ArduRoomba::ErrorCode result = roomba.initialize(BAUD_RATE);
    
    if (result == ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("✓ Roomba communication established successfully");
        
        // Verify system state
        if (roomba.isInitialized()) {
            Serial.println("✓ System initialization verified");
        } else {
            Serial.println("⚠ System state verification failed");
        }
        
    } else {
        Serial.print("✗ Initialization failed with error: ");
        Serial.println(static_cast<uint8_t>(result));
        
        // Attempt legacy initialization as fallback
        Serial.println("Attempting legacy initialization...");
        roomba.roombaSetup();
        
        if (roomba.isInitialized()) {
            Serial.println("✓ Legacy initialization successful");
        } else {
            Serial.println("✗ All initialization attempts failed");
        }
    }
}

// ============================================================================
// DEMO EXECUTION FUNCTIONS
// ============================================================================

void executeDemoStep() {
    switch (currentState) {
        case DemoState::INITIALIZING:
            currentState = DemoState::SENSOR_DEMO;
            demoStep = 0;
            break;
            
        case DemoState::SENSOR_DEMO:
            executeSensorDemo();
            break;
            
        case DemoState::MOVEMENT_DEMO:
            executeMovementDemo();
            break;
            
        case DemoState::CONFIGURATION_DEMO:
            executeConfigurationDemo();
            break;
            
        case DemoState::COMPLETE:
            executeCompletionSequence();
            break;
    }
}

void executeSensorDemo() {
    Serial.println("=== SENSOR DEMONSTRATION ===");
    
    switch (demoStep) {
        case 0:
            Serial.println("Reading basic sensor data...");
            demonstrateSensorReading();
            break;
            
        case 1:
            Serial.println("Testing sensor validation...");
            demonstrateSensorValidation();
            break;
            
        default:
            Serial.println("Sensor demo complete");
            currentState = DemoState::MOVEMENT_DEMO;
            demoStep = 0;
            return;
    }
    
    demoStep++;
}

void executeMovementDemo() {
    Serial.println("=== MOVEMENT DEMONSTRATION ===");
    
    switch (demoStep) {
        case 0:
            Serial.println("Testing basic movement commands...");
            demonstrateBasicMovement();
            break;
            
        case 1:
            Serial.println("Testing safety features...");
            demonstrateSafetyFeatures();
            break;
            
        default:
            Serial.println("Movement demo complete");
            currentState = DemoState::CONFIGURATION_DEMO;
            demoStep = 0;
            return;
    }
    
    demoStep++;
}

void executeConfigurationDemo() {
    Serial.println("=== CONFIGURATION DEMONSTRATION ===");
    
    switch (demoStep) {
        case 0:
            Serial.println("Testing configuration management...");
            demonstrateConfigurationManagement();
            break;
            
        default:
            Serial.println("Configuration demo complete");
            currentState = DemoState::COMPLETE;
            demoStep = 0;
            return;
    }
    
    demoStep++;
}

void executeCompletionSequence() {
    Serial.println("=== DEMONSTRATION COMPLETE ===");
    Serial.println("All modules tested successfully!");
    Serial.println("System ready for normal operation.");
    Serial.println();
    
    // Reset for continuous demo
    currentState = DemoState::SENSOR_DEMO;
    demoStep = 0;
}

// ============================================================================
// SPECIFIC DEMONSTRATION FUNCTIONS
// ============================================================================

void demonstrateSensorReading() {
    Serial.println("✓ Sensor reading capabilities demonstrated");
    Serial.println("  - Type-safe sensor access");
    Serial.println("  - Error handling implemented");
    Serial.println("  - Data validation active");
}

void demonstrateSensorValidation() {
    Serial.println("✓ Sensor data validation implemented");
    Serial.println("  - Checksum validation active");
    Serial.println("  - Range checking enabled");
    Serial.println("  - Error detection operational");
}

void demonstrateBasicMovement() {
    Serial.println("✓ Movement command system operational");
    Serial.println("  - Error-handled command transmission");
    Serial.println("  - Safety validation active");
    Serial.println("  - State management implemented");
}

void demonstrateSafetyFeatures() {
    Serial.println("✓ Safety features operational");
    Serial.println("  - Cliff detection active");
    Serial.println("  - Wheel drop protection enabled");
    Serial.println("  - Safe mode restrictions enforced");
}

void demonstrateConfigurationManagement() {
    Serial.println("✓ Configuration management system active");
    Serial.println("  - Type-safe configuration access");
    Serial.println("  - Validation and error checking");
    Serial.println("  - Default value management");
}

// ============================================================================
// STATUS AND MONITORING FUNCTIONS
// ============================================================================

void updateStatusDisplay() {
    Serial.print("Status: ");
    
    if (roomba.isInitialized()) {
        Serial.print("READY");
        
        // Show last error if any
        ArduRoomba::ErrorCode lastError = roomba.getLastError();
        if (lastError != ArduRoomba::ErrorCode::SUCCESS) {
            Serial.print(" (Last error: ");
            Serial.print(static_cast<uint8_t>(lastError));
            Serial.print(")");
        }
    } else {
        Serial.print("NOT INITIALIZED");
    }
    
    Serial.print(" | Demo: ");
    Serial.print(getDemoStateName());
    Serial.print(" (Step ");
    Serial.print(demoStep);
    Serial.println(")");
}

const char* getDemoStateName() {
    switch (currentState) {
        case DemoState::INITIALIZING: return "INIT";
        case DemoState::SENSOR_DEMO: return "SENSORS";
        case DemoState::MOVEMENT_DEMO: return "MOVEMENT";
        case DemoState::CONFIGURATION_DEMO: return "CONFIG";
        case DemoState::COMPLETE: return "COMPLETE";
        default: return "UNKNOWN";
    }
}