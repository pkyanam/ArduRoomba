/**
 * @file ArduRoombaWiFi.h
 * @brief WiFi management and web server functionality for ArduRoomba
 * 
 * This file provides WiFi connectivity and web server capabilities for
 * wireless-enabled boards (ESP32, ESP8266, Arduino Uno R4 WiFi).
 * It enables remote control of Roomba through REST API and web interface.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.4.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBA_WIFI_H
#define ARDUROOMBA_WIFI_H

#include <Arduino.h>
#include "../ArduRoomba.h"

// Platform-specific WiFi includes
#if defined(ESP32)
    #include <WiFi.h>
    #include <WebServer.h>
    #include <ESPmDNS.h>
    #include <SPIFFS.h>
    #define WIFI_PLATFORM "ESP32"
    #define HAS_FILESYSTEM true
    #define HAS_MDNS true
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266mDNS.h>
    #include <LittleFS.h>
    #define WebServer ESP8266WebServer
    #define WIFI_PLATFORM "ESP8266"
    #define HAS_FILESYSTEM true
    #define HAS_MDNS true
#elif defined(ARDUINO_UNOWIFIR4)
    #include <WiFiS3.h>
    #include <WiFiServer.h>
    #define WIFI_PLATFORM "UNO_R4_WIFI"
    #define HAS_FILESYSTEM false
    #define HAS_MDNS false
#else
    #error "Unsupported platform for WiFi functionality"
#endif

namespace ArduRoomba {

/**
 * @brief WiFi connection status enumeration
 */
enum class WiFiStatus : uint8_t {
    DISCONNECTED = 0,   ///< Not connected to WiFi
    CONNECTING = 1,     ///< Attempting to connect
    CONNECTED = 2,      ///< Successfully connected
    FAILED = 3,         ///< Connection failed
    AP_MODE = 4         ///< Access Point mode active
};

/**
 * @brief WiFi configuration structure
 */
struct WiFiConfig {
    String ssid;            ///< WiFi network name
    String password;        ///< WiFi password
    String hostname;        ///< Device hostname
    String apSSID;          ///< Access Point SSID
    String apPassword;      ///< Access Point password
    uint16_t serverPort;    ///< Web server port
    bool enableAP;          ///< Enable AP mode if connection fails
    uint32_t connectTimeout; ///< Connection timeout (ms)
    
    /**
     * @brief Default constructor with sensible defaults
     */
    WiFiConfig() : 
        hostname("arduroomba"),
        apSSID("ArduRoomba-Setup"),
        apPassword("roomba123"),
        serverPort(80),
        enableAP(true),
        connectTimeout(30000) {}
};

/**
 * @brief Main WiFi management class for ArduRoomba
 * 
 * This class provides comprehensive WiFi connectivity and web server
 * functionality for wireless-enabled Arduino boards. It handles
 * connection management, web server setup, and API endpoint routing.
 */
class ArduRoombaWiFi {
public:
    /**
     * @brief Constructor
     * @param roomba Reference to ArduRoomba instance
     */
    explicit ArduRoombaWiFi(ArduRoomba& roomba);
    
    /**
     * @brief Destructor
     */
    ~ArduRoombaWiFi();
    
    // ========================================================================
    // WIFI CONNECTION MANAGEMENT
    // ========================================================================
    
    /**
     * @brief Initialize WiFi with configuration
     * @param config WiFi configuration structure
     * @return ErrorCode indicating success or failure
     */
    ErrorCode begin(const WiFiConfig& config);
    
    /**
     * @brief Initialize WiFi with simple credentials
     * @param ssid WiFi network name
     * @param password WiFi password
     * @param hostname Device hostname (optional)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode begin(const String& ssid, const String& password, const String& hostname = "arduroomba");
    
    /**
     * @brief Check current WiFi connection status
     * @return Current WiFiStatus
     */
    WiFiStatus getStatus() const;
    
    /**
     * @brief Get WiFi connection information
     * @param ip Reference to store IP address
     * @param rssi Reference to store signal strength
     * @param ssid Reference to store connected SSID
     */
    void getConnectionInfo(IPAddress& ip, int32_t& rssi, String& ssid) const;
    
    /**
     * @brief Attempt to reconnect to WiFi
     * @return ErrorCode indicating success or failure
     */
    ErrorCode reconnect();
    
    /**
     * @brief Disconnect from WiFi
     */
    void disconnect();
    
    // ========================================================================
    // WEB SERVER MANAGEMENT
    // ========================================================================
    
    /**
     * @brief Start the web server
     * @param port Server port (default: 80)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode startWebServer(uint16_t port = 80);
    
    /**
     * @brief Stop the web server
     */
    void stopWebServer();
    
    /**
     * @brief Handle incoming web requests (call in loop())
     */
    void handleClient();
    
    /**
     * @brief Check if web server is running
     * @return true if server is active, false otherwise
     */
    bool isServerRunning() const { return _serverRunning; }
    
    // ========================================================================
    // API ENDPOINT MANAGEMENT
    // ========================================================================
    
    /**
     * @brief Enable or disable CORS headers
     * @param enabled true to enable CORS, false to disable
     */
    void setCORSEnabled(bool enabled) { _corsEnabled = enabled; }
    
    /**
     * @brief Set API authentication key
     * @param apiKey API key for authentication (empty to disable)
     */
    void setAPIKey(const String& apiKey) { _apiKey = apiKey; }
    
    /**
     * @brief Enable or disable debug output
     * @param enabled true to enable debug output, false to disable
     */
    void setDebugEnabled(bool enabled) { _debugEnabled = enabled; }
    
    // ========================================================================
    // UTILITY METHODS
    // ========================================================================
    
    /**
     * @brief Get platform information
     * @return Platform string (ESP32, ESP8266, UNO_R4_WIFI)
     */
    String getPlatform() const { return WIFI_PLATFORM; }
    
    /**
     * @brief Check if filesystem is available
     * @return true if filesystem is supported, false otherwise
     */
    bool hasFilesystem() const { return HAS_FILESYSTEM; }
    
    /**
     * @brief Check if mDNS is available
     * @return true if mDNS is supported, false otherwise
     */
    bool hasMDNS() const { return HAS_MDNS; }
    
    /**
     * @brief Get web server URL
     * @return Complete URL to access web interface
     */
    String getServerURL() const;
    
    /**
     * @brief Get connection statistics
     * @param uptime Reference to store uptime in milliseconds
     * @param requests Reference to store total requests handled
     * @param errors Reference to store error count
     */
    void getStatistics(unsigned long& uptime, uint32_t& requests, uint16_t& errors) const;
    
    /**
     * @brief Reset statistics
     */
    void resetStatistics();

protected:
    // ========================================================================
    // API ENDPOINT HANDLERS
    // ========================================================================
    
    /**
     * @brief Handle system status endpoint
     */
    void handleStatus();
    
    /**
     * @brief Handle Roomba initialization endpoint
     */
    void handleInitialize();
    
    /**
     * @brief Handle sensor data endpoints
     */
    void handleSensors();
    void handleSensorsAll();
    void handleSensorsBasic();
    void handleSensorsBattery();
    
    /**
     * @brief Handle movement control endpoints
     */
    void handleMovement();
    void handleMovementForward();
    void handleMovementBackward();
    void handleMovementTurnLeft();
    void handleMovementTurnRight();
    void handleMovementStop();
    void handleMovementDrive();
    
    /**
     * @brief Handle cleaning mode endpoints
     */
    void handleCleaning();
    void handleCleaningStart();
    void handleCleaningSpot();
    void handleCleaningDock();
    
    /**
     * @brief Handle actuator control endpoints
     */
    void handleActuators();
    void handleActuatorsMotors();
    void handleActuatorsLEDs();
    void handleActuatorsSound();
    
    /**
     * @brief Handle mode control endpoints
     */
    void handleMode();
    void handleModeSafe();
    void handleModeFull();
    
    /**
     * @brief Handle web interface files
     */
    void handleRoot();
    void handleNotFound();
    void handleOptions(); // For CORS preflight
    
    // ========================================================================
    // UTILITY METHODS
    // ========================================================================
    
    /**
     * @brief Send JSON response
     * @param code HTTP status code
     * @param json JSON response string
     */
    void sendJSON(int code, const String& json);
    
    /**
     * @brief Send error response
     * @param code HTTP status code
     * @param message Error message
     */
    void sendError(int code, const String& message);
    
    /**
     * @brief Add CORS headers to response
     */
    void addCORSHeaders();
    
    /**
     * @brief Check API authentication
     * @return true if authenticated, false otherwise
     */
    bool checkAuthentication();
    
    /**
     * @brief Parse JSON request body
     * @param json Reference to store parsed JSON
     * @return true if parsing successful, false otherwise
     */
    bool parseJSONRequest(String& json);
    
    /**
     * @brief Get request parameter as integer
     * @param name Parameter name
     * @param defaultValue Default value if parameter not found
     * @return Parameter value
     */
    int getIntParam(const String& name, int defaultValue = 0);
    
    /**
     * @brief Debug print method
     * @param message Message to print
     */
    void debugPrint(const String& message);
    
    /**
     * @brief Setup all API routes
     */
    void setupRoutes();

private:
    // ========================================================================
    // MEMBER VARIABLES
    // ========================================================================
    
    ArduRoomba& _roomba;            ///< Reference to Roomba instance
    WiFiConfig _config;             ///< WiFi configuration
    WiFiStatus _status;             ///< Current WiFi status
    
#if defined(ESP32) || defined(ESP8266)
    WebServer* _server;             ///< Web server instance
#elif defined(ARDUINO_UNOWIFIR4)
    WiFiServer* _server;            ///< WiFi server for Uno R4
#endif
    
    bool _serverRunning;            ///< Server running status
    bool _corsEnabled;              ///< CORS enabled flag
    bool _debugEnabled;             ///< Debug output enabled
    String _apiKey;                 ///< API authentication key
    
    // Statistics
    unsigned long _startTime;       ///< WiFi start time
    uint32_t _totalRequests;        ///< Total requests handled
    uint16_t _errorCount;           ///< Total errors encountered
    
    // Connection management
    unsigned long _lastConnectionAttempt; ///< Last connection attempt time
    uint8_t _reconnectAttempts;     ///< Number of reconnection attempts
    static const uint8_t MAX_RECONNECT_ATTEMPTS = 5; ///< Maximum reconnection attempts
    static const unsigned long RECONNECT_INTERVAL = 30000; ///< Reconnection interval (ms)
};

} // namespace ArduRoomba

#endif // ARDUROOMBA_WIFI_H