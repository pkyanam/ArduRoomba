/**
 * @file WebServerComplete.ino
 * @brief Complete web server example with full web interface
 * 
 * This example provides a comprehensive web-based control interface for
 * ArduRoomba, including real-time sensor monitoring, movement controls,
 * and cleaning mode management. It demonstrates the full capabilities
 * of the WiFi extension.
 * 
 * Features:
 * - Responsive web interface with touch controls
 * - Real-time sensor data updates
 * - Virtual D-pad for movement control
 * - Battery monitoring and status display
 * - Cleaning mode controls
 * - System diagnostics and statistics
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.4.0
 * @date 2025-06-06
 */

#include "ArduRoomba.h"

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)
#include "wireless/ArduRoombaWiFi.h"

// ============================================================================
// CONFIGURATION
// ============================================================================

// WiFi Configuration
const char* WIFI_SSID = "YourWiFiNetwork";
const char* WIFI_PASSWORD = "YourWiFiPassword";
const char* DEVICE_HOSTNAME = "roomba-control";
const char* API_KEY = ""; // Optional API key

// Hardware Configuration
const uint8_t ROOMBA_RX_PIN = 2;
const uint8_t ROOMBA_TX_PIN = 3;
const uint8_t ROOMBA_BRC_PIN = 4;

// Server Configuration
const uint16_t SERVER_PORT = 80;
const bool ENABLE_CORS = true;
const bool ENABLE_DEBUG = true;

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

ArduRoomba roomba(ROOMBA_RX_PIN, ROOMBA_TX_PIN, ROOMBA_BRC_PIN);
ArduRoombaWiFi wifi(roomba);

// Timing variables
unsigned long lastSensorUpdate = 0;
unsigned long lastStatusPrint = 0;
const unsigned long SENSOR_UPDATE_INTERVAL = 1000;  // Update sensors every 1 second
const unsigned long STATUS_PRINT_INTERVAL = 30000;  // Print status every 30 seconds

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    printHeader();
    
    // Configure WiFi component
    wifi.setDebugEnabled(ENABLE_DEBUG);
    wifi.setCORSEnabled(ENABLE_CORS);
    
    if (strlen(API_KEY) > 0) {
        wifi.setAPIKey(API_KEY);
    }
    
    // Initialize Roomba
    initializeRoomba();
    
    // Initialize WiFi and web server
    initializeWiFi();
    
    // Start sensor monitoring
    startSensorMonitoring();
    
    Serial.println("Setup complete. System ready for connections.");
    Serial.println();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    // Handle web requests
    wifi.handleClient();
    
    // Update sensor data periodically
    if (millis() - lastSensorUpdate > SENSOR_UPDATE_INTERVAL) {
        updateSensorData();
        lastSensorUpdate = millis();
    }
    
    // Print status updates
    if (millis() - lastStatusPrint > STATUS_PRINT_INTERVAL) {
        printSystemStatus();
        lastStatusPrint = millis();
    }
    
    // Handle WiFi reconnection
    handleWiFiReconnection();
    
    delay(10);
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================

void printHeader() {
    Serial.println();
    Serial.println("==========================================");
    Serial.println("ArduRoomba Complete Web Server Example");
    Serial.println("==========================================");
    Serial.println("Platform: " + wifi.getPlatform());
    Serial.println("Version: 2.4.0");
    Serial.println("Features: Full Web Interface + REST API");
    Serial.println("==========================================");
    Serial.println();
}

void initializeRoomba() {
    Serial.println("Initializing Roomba connection...");
    
    ArduRoomba::ErrorCode result = roomba.initialize();
    
    if (result == ArduRoomba::ErrorCode::SUCCESS) {
        Serial.println("✓ Roomba initialized successfully");
        
        // Put in safe mode for sensor access
        roomba.safe();
        delay(100);
        
        // Test sensor reading
        ArduRoomba::SensorData testData;
        if (roomba.updateSensorData(testData) == ArduRoomba::ErrorCode::SUCCESS) {
            Serial.print("✓ Sensor communication working. Battery: ");
            Serial.print(testData.getBatteryPercentage());
            Serial.println("%");
        } else {
            Serial.println("⚠ Sensor communication may have issues");
        }
        
    } else {
        Serial.print("✗ Roomba initialization failed. Error: ");
        Serial.println(static_cast<uint8_t>(result));
        Serial.println("  You can initialize via web interface later");
    }
    
    Serial.println();
}

void initializeWiFi() {
    Serial.println("Starting WiFi connection...");
    Serial.print("Connecting to: ");
    Serial.println(WIFI_SSID);
    
    ArduRoomba::ErrorCode result = wifi.begin(WIFI_SSID, WIFI_PASSWORD, DEVICE_HOSTNAME);
    
    if (result == ArduRoomba::ErrorCode::SUCCESS) {
        IPAddress ip;
        int32_t rssi;
        String ssid;
        wifi.getConnectionInfo(ip, rssi, ssid);
        
        Serial.println("✓ WiFi connected successfully");
        Serial.print("  IP Address: ");
        Serial.println(ip);
        Serial.print("  Signal: ");
        Serial.print(rssi);
        Serial.println(" dBm");
        
        // Start web server
        Serial.println();
        Serial.println("Starting web server...");
        
        ArduRoomba::ErrorCode serverResult = wifi.startWebServer(SERVER_PORT);
        
        if (serverResult == ArduRoomba::ErrorCode::SUCCESS) {
            Serial.println("✓ Web server started successfully");
            printAccessInformation();
        } else {
            Serial.print("✗ Web server failed to start. Error: ");
            Serial.println(static_cast<uint8_t>(serverResult));
        }
        
    } else {
        Serial.print("✗ WiFi connection failed. Error: ");
        Serial.println(static_cast<uint8_t>(result));
        Serial.println("  Check credentials and network availability");
    }
    
    Serial.println();
}

void startSensorMonitoring() {
    if (roomba.isInitialized()) {
        Serial.println("Starting sensor monitoring...");
        
        // Start basic sensor streaming
        ArduRoomba::ErrorCode result = roomba.startSensorStream(
            ArduRoomba::RoombaSensors::SensorPreset::BASIC
        );
        
        if (result == ArduRoomba::ErrorCode::SUCCESS) {
            Serial.println("✓ Sensor streaming started");
        } else {
            Serial.print("⚠ Sensor streaming failed. Error: ");
            Serial.println(static_cast<uint8_t>(result));
        }
    }
    
    Serial.println();
}

void printAccessInformation() {
    Serial.println();
    Serial.println("==========================================");
    Serial.println("WEB INTERFACE ACCESS INFORMATION");
    Serial.println("==========================================");
    Serial.println();
    Serial.print("Main Interface: ");
    Serial.println(wifi.getServerURL());
    Serial.println();
    
    if (wifi.hasMDNS()) {
        Serial.print("mDNS Address: http://");
        Serial.print(DEVICE_HOSTNAME);
        Serial.println(".local");
        Serial.println();
    }
    
    Serial.println("API Endpoints:");
    Serial.println("  System Status:    GET  /api/status");
    Serial.println("  Initialize:       POST /api/initialize");
    Serial.println("  All Sensors:      GET  /api/sensors/all");
    Serial.println("  Move Forward:     POST /api/movement/forward");
    Serial.println("  Move Backward:    POST /api/movement/backward");
    Serial.println("  Turn Left:        POST /api/movement/turn-left");
    Serial.println("  Turn Right:       POST /api/movement/turn-right");
    Serial.println("  Stop Movement:    POST /api/movement/stop");
    Serial.println("  Start Cleaning:   POST /api/cleaning/start");
    Serial.println("  Spot Clean:       POST /api/cleaning/spot");
    Serial.println("  Return to Dock:   POST /api/cleaning/dock");
    Serial.println();
    
    if (strlen(API_KEY) > 0) {
        Serial.println("Authentication: Include 'X-API-Key: " + String(API_KEY) + "' header");
        Serial.println();
    }
    
    Serial.println("Example Commands:");
    String baseUrl = wifi.getServerURL();
    Serial.println("  curl " + baseUrl + "/api/status");
    Serial.println("  curl -X POST " + baseUrl + "/api/movement/forward");
    Serial.println("  curl -X POST " + baseUrl + "/api/cleaning/start");
    Serial.println();
    
    Serial.println("Web Interface Features:");
    Serial.println("  • Real-time sensor monitoring");
    Serial.println("  • Touch-friendly movement controls");
    Serial.println("  • Battery status and history");
    Serial.println("  • Cleaning mode management");
    Serial.println("  • System diagnostics");
    Serial.println("  • Mobile-responsive design");
    Serial.println();
    Serial.println("==========================================");
}

// ============================================================================
// RUNTIME FUNCTIONS
// ============================================================================

void updateSensorData() {
    if (!roomba.isInitialized()) return;
    
    ArduRoomba::SensorData data;
    ArduRoomba::ErrorCode result = roomba.updateSensorData(data);
    
    if (result == ArduRoomba::ErrorCode::SUCCESS) {
        // Check for important status changes
        static int lastBatteryPercent = -1;
        static bool lastCliffDetection = false;
        static bool lastBumpDetection = false;
        
        int currentBattery = data.getBatteryPercentage();
        bool currentCliff = data.hasCliffDetection();
        bool currentBump = data.hasBumpDetection();
        
        // Report significant battery changes
        if (lastBatteryPercent != -1 && abs(currentBattery - lastBatteryPercent) >= 5) {
            Serial.print("Battery level changed: ");
            Serial.print(currentBattery);
            Serial.println("%");
        }
        lastBatteryPercent = currentBattery;
        
        // Report safety alerts
        if (currentCliff && !lastCliffDetection) {
            Serial.println("⚠ CLIFF DETECTED - Stopping for safety");
            roomba.stopMovement();
        }
        lastCliffDetection = currentCliff;
        
        if (currentBump && !lastBumpDetection) {
            Serial.println("⚠ OBSTACLE DETECTED - Bumper activated");
        }
        lastBumpDetection = currentBump;
        
        // Low battery warning
        if (currentBattery < 20 && currentBattery > 0) {
            static unsigned long lastLowBatteryWarning = 0;
            if (millis() - lastLowBatteryWarning > 60000) { // Warn every minute
                Serial.println("⚠ LOW BATTERY WARNING: " + String(currentBattery) + "%");
                lastLowBatteryWarning = millis();
            }
        }
    }
}

void handleWiFiReconnection() {
    static ArduRoomba::WiFiStatus lastStatus = ArduRoomba::WiFiStatus::DISCONNECTED;
    ArduRoomba::WiFiStatus currentStatus = wifi.getStatus();
    
    if (currentStatus != lastStatus) {
        switch (currentStatus) {
            case ArduRoomba::WiFiStatus::DISCONNECTED:
                Serial.println("⚠ WiFi connection lost");
                break;
            case ArduRoomba::WiFiStatus::CONNECTING:
                Serial.println("🔄 Attempting WiFi reconnection...");
                break;
            case ArduRoomba::WiFiStatus::CONNECTED:
                Serial.println("✓ WiFi reconnected successfully");
                break;
            case ArduRoomba::WiFiStatus::AP_MODE:
                Serial.println("📡 Access Point mode activated");
                break;
        }
        lastStatus = currentStatus;
    }
    
    // Attempt reconnection if disconnected
    if (currentStatus == ArduRoomba::WiFiStatus::DISCONNECTED) {
        static unsigned long lastReconnectAttempt = 0;
        if (millis() - lastReconnectAttempt > 30000) { // Try every 30 seconds
            wifi.reconnect();
            lastReconnectAttempt = millis();
        }
    }
}

void printSystemStatus() {
    Serial.println("=== System Status ===");
    
    // WiFi Status
    ArduRoomba::WiFiStatus wifiStatus = wifi.getStatus();
    Serial.print("WiFi: ");
    switch (wifiStatus) {
        case ArduRoomba::WiFiStatus::CONNECTED:
            {
                IPAddress ip;
                int32_t rssi;
                String ssid;
                wifi.getConnectionInfo(ip, rssi, ssid);
                Serial.print("Connected to ");
                Serial.print(ssid);
                Serial.print(" (");
                Serial.print(ip);
                Serial.print(", ");
                Serial.print(rssi);
                Serial.print(" dBm)");
            }
            break;
        case ArduRoomba::WiFiStatus::AP_MODE:
            Serial.print("Access Point Mode");
            break;
        case ArduRoomba::WiFiStatus::DISCONNECTED:
            Serial.print("Disconnected");
            break;
        default:
            Serial.print("Unknown");
            break;
    }
    Serial.println();
    
    // Server Statistics
    unsigned long uptime;
    uint32_t requests;
    uint16_t errors;
    wifi.getStatistics(uptime, requests, errors);
    
    Serial.print("Server: ");
    if (wifi.isServerRunning()) {
        Serial.print("Running | Uptime: ");
        Serial.print(uptime / 1000);
        Serial.print("s | Requests: ");
        Serial.print(requests);
        Serial.print(" | Errors: ");
        Serial.print(errors);
    } else {
        Serial.print("Stopped");
    }
    Serial.println();
    
    // Roomba Status
    Serial.print("Roomba: ");
    if (roomba.isInitialized()) {
        ArduRoomba::SensorData data;
        if (roomba.updateSensorData(data) == ArduRoomba::ErrorCode::SUCCESS) {
            Serial.print("Connected | Battery: ");
            Serial.print(data.getBatteryPercentage());
            Serial.print("% | Mode: ");
            Serial.print(static_cast<uint8_t>(data.mode));
            
            if (data.hasCliffDetection()) Serial.print(" | CLIFF");
            if (data.hasBumpDetection()) Serial.print(" | BUMP");
            if (data.hasWheelDrop()) Serial.print(" | WHEEL-DROP");
        } else {
            Serial.print("Communication Error");
        }
    } else {
        Serial.print("Not Initialized");
    }
    Serial.println();
    
    // Memory Status
    Serial.print("Memory: ");
#if defined(ESP32) || defined(ESP8266)
    uint32_t freeHeap = ESP.getFreeHeap();
    Serial.print(freeHeap);
    Serial.print(" bytes free");
    
    if (freeHeap < 10000) {
        Serial.print(" ⚠ LOW");
    }
#else
    Serial.print("N/A");
#endif
    Serial.println();
    
    Serial.println();
}

#else
// ============================================================================
// UNSUPPORTED PLATFORM
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("==========================================");
    Serial.println("ArduRoomba Complete Web Server Example");
    Serial.println("==========================================");
    Serial.println();
    Serial.println("ERROR: This example requires a WiFi-capable board.");
    Serial.println();
    Serial.println("Supported platforms:");
    Serial.println("  • ESP32 (recommended)");
    Serial.println("  • ESP8266");
    Serial.println("  • Arduino Uno R4 WiFi");
    Serial.println();
    Serial.println("Please use a supported board or try the basic examples.");
}

void loop() {
    delay(1000);
}

#endif // Platform check