/**
 * @file ArduRoombaWiFi.cpp
 * @brief Implementation of WiFi management and web server functionality
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 */

#include "ArduRoombaWiFi.h"

namespace ArduRoomba {

// ============================================================================
// CONSTRUCTOR & DESTRUCTOR
// ============================================================================

ArduRoombaWiFi::ArduRoombaWiFi(ArduRoomba& roomba) :
    _roomba(roomba),
    _status(WiFiStatus::DISCONNECTED),
    _server(nullptr),
    _serverRunning(false),
    _corsEnabled(true),
    _debugEnabled(false),
    _startTime(0),
    _totalRequests(0),
    _errorCount(0),
    _lastConnectionAttempt(0),
    _reconnectAttempts(0)
{
}

ArduRoombaWiFi::~ArduRoombaWiFi() {
    stopWebServer();
    disconnect();
}

// ============================================================================
// WIFI CONNECTION MANAGEMENT
// ============================================================================

ErrorCode ArduRoombaWiFi::begin(const WiFiConfig& config) {
    _config = config;
    return begin(_config.ssid, _config.password, _config.hostname);
}

ErrorCode ArduRoombaWiFi::begin(const String& ssid, const String& password, const String& hostname) {
    debugPrint("Starting WiFi connection...");
    
    // Store configuration
    _config.ssid = ssid;
    _config.password = password;
    _config.hostname = hostname;
    _startTime = millis();
    _status = WiFiStatus::CONNECTING;
    
    // Set hostname if supported
#if defined(ESP32)
    WiFi.setHostname(_config.hostname.c_str());
#elif defined(ESP8266)
    WiFi.hostname(_config.hostname);
#endif
    
    // Begin WiFi connection
    WiFi.begin(_config.ssid.c_str(), _config.password.c_str());
    
    // Wait for connection with timeout
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && 
           (millis() - startTime) < _config.connectTimeout) {
        delay(500);
        debugPrint(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        _status = WiFiStatus::CONNECTED;
        debugPrint("WiFi connected!");
        debugPrint("IP address: " + WiFi.localIP().toString());
        
        // Setup mDNS if available
#if HAS_MDNS
        if (MDNS.begin(_config.hostname.c_str())) {
            debugPrint("mDNS responder started: " + _config.hostname + ".local");
        }
#endif
        
        return ErrorCode::SUCCESS;
    } else {
        _status = WiFiStatus::FAILED;
        debugPrint("WiFi connection failed!");
        
        // Start AP mode if enabled
        if (_config.enableAP) {
            debugPrint("Starting Access Point mode...");
            WiFi.softAP(_config.apSSID.c_str(), _config.apPassword.c_str());
            _status = WiFiStatus::AP_MODE;
            debugPrint("AP started: " + _config.apSSID);
            debugPrint("AP IP: " + WiFi.softAPIP().toString());
            return ErrorCode::SUCCESS;
        }
        
        return ErrorCode::COMMUNICATION_ERROR;
    }
}

WiFiStatus ArduRoombaWiFi::getStatus() const {
    return _status;
}

void ArduRoombaWiFi::getConnectionInfo(IPAddress& ip, int32_t& rssi, String& ssid) const {
    if (_status == WiFiStatus::CONNECTED) {
        ip = WiFi.localIP();
        rssi = WiFi.RSSI();
        ssid = WiFi.SSID();
    } else if (_status == WiFiStatus::AP_MODE) {
        ip = WiFi.softAPIP();
        rssi = 0;
        ssid = _config.apSSID;
    } else {
        ip = IPAddress(0, 0, 0, 0);
        rssi = 0;
        ssid = "";
    }
}

ErrorCode ArduRoombaWiFi::reconnect() {
    if (millis() - _lastConnectionAttempt < RECONNECT_INTERVAL) {
        return ErrorCode::TIMEOUT;
    }
    
    _lastConnectionAttempt = millis();
    _reconnectAttempts++;
    
    if (_reconnectAttempts > MAX_RECONNECT_ATTEMPTS) {
        debugPrint("Max reconnection attempts reached");
        return ErrorCode::COMMUNICATION_ERROR;
    }
    
    debugPrint("Attempting to reconnect... (attempt " + String(_reconnectAttempts) + ")");
    
    WiFi.disconnect();
    delay(1000);
    
    return begin(_config.ssid, _config.password, _config.hostname);
}

void ArduRoombaWiFi::disconnect() {
    debugPrint("Disconnecting WiFi...");
    WiFi.disconnect();
    _status = WiFiStatus::DISCONNECTED;
}

// ============================================================================
// WEB SERVER MANAGEMENT
// ============================================================================

ErrorCode ArduRoombaWiFi::startWebServer(uint16_t port) {
    if (_serverRunning) {
        debugPrint("Web server already running");
        return ErrorCode::SUCCESS;
    }
    
    _config.serverPort = port;
    
#if defined(ESP32) || defined(ESP8266)
    _server = new WebServer(port);
    setupRoutes();
    _server->begin();
#elif defined(ARDUINO_UNOWIFIR4)
    _server = new WiFiServer(port);
    _server->begin();
#endif
    
    _serverRunning = true;
    debugPrint("Web server started on port " + String(port));
    debugPrint("Access at: " + getServerURL());
    
    return ErrorCode::SUCCESS;
}

void ArduRoombaWiFi::stopWebServer() {
    if (!_serverRunning) return;
    
#if defined(ESP32) || defined(ESP8266)
    if (_server) {
        _server->stop();
        delete _server;
        _server = nullptr;
    }
#elif defined(ARDUINO_UNOWIFIR4)
    if (_server) {
        delete _server;
        _server = nullptr;
    }
#endif
    
    _serverRunning = false;
    debugPrint("Web server stopped");
}

void ArduRoombaWiFi::handleClient() {
    if (!_serverRunning) return;
    
    // Check WiFi connection and attempt reconnect if needed
    if (_status == WiFiStatus::CONNECTED && WiFi.status() != WL_CONNECTED) {
        _status = WiFiStatus::DISCONNECTED;
        debugPrint("WiFi connection lost");
        
        // Attempt automatic reconnection
        if (_reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
            reconnect();
        }
    }
    
#if defined(ESP32) || defined(ESP8266)
    if (_server) {
        _server->handleClient();
    }
#elif defined(ARDUINO_UNOWIFIR4)
    // Handle Uno R4 WiFi client connections manually
    // This is a simplified implementation for basic functionality
    // TODO: Implement full HTTP parsing for Uno R4 WiFi
#endif
}

// ============================================================================
// API ENDPOINT SETUP
// ============================================================================

void ArduRoombaWiFi::setupRoutes() {
#if defined(ESP32) || defined(ESP8266)
    if (!_server) return;
    
    // System endpoints
    _server->on("/api/status", HTTP_GET, [this]() { handleStatus(); });
    _server->on("/api/initialize", HTTP_POST, [this]() { handleInitialize(); });
    
    // Sensor endpoints
    _server->on("/api/sensors", HTTP_GET, [this]() { handleSensors(); });
    _server->on("/api/sensors/all", HTTP_GET, [this]() { handleSensorsAll(); });
    _server->on("/api/sensors/basic", HTTP_GET, [this]() { handleSensorsBasic(); });
    _server->on("/api/sensors/battery", HTTP_GET, [this]() { handleSensorsBattery(); });
    
    // Movement endpoints
    _server->on("/api/movement", HTTP_POST, [this]() { handleMovement(); });
    _server->on("/api/movement/forward", HTTP_POST, [this]() { handleMovementForward(); });
    _server->on("/api/movement/backward", HTTP_POST, [this]() { handleMovementBackward(); });
    _server->on("/api/movement/turn-left", HTTP_POST, [this]() { handleMovementTurnLeft(); });
    _server->on("/api/movement/turn-right", HTTP_POST, [this]() { handleMovementTurnRight(); });
    _server->on("/api/movement/stop", HTTP_POST, [this]() { handleMovementStop(); });
    _server->on("/api/movement/drive", HTTP_POST, [this]() { handleMovementDrive(); });
    
    // Cleaning endpoints
    _server->on("/api/cleaning", HTTP_POST, [this]() { handleCleaning(); });
    _server->on("/api/cleaning/start", HTTP_POST, [this]() { handleCleaningStart(); });
    _server->on("/api/cleaning/spot", HTTP_POST, [this]() { handleCleaningSpot(); });
    _server->on("/api/cleaning/dock", HTTP_POST, [this]() { handleCleaningDock(); });
    
    // Mode endpoints
    _server->on("/api/mode/safe", HTTP_POST, [this]() { handleModeSafe(); });
    _server->on("/api/mode/full", HTTP_POST, [this]() { handleModeFull(); });
    
    // Actuator endpoints
    _server->on("/api/actuators/motors", HTTP_POST, [this]() { handleActuatorsMotors(); });
    _server->on("/api/actuators/leds", HTTP_POST, [this]() { handleActuatorsLEDs(); });
    _server->on("/api/actuators/sound", HTTP_POST, [this]() { handleActuatorsSound(); });
    
    // Web interface
    _server->on("/", HTTP_GET, [this]() { handleRoot(); });
    
    // CORS preflight
    _server->on("/api/*", HTTP_OPTIONS, [this]() { handleOptions(); });
    
    // 404 handler
    _server->onNotFound([this]() { handleNotFound(); });
#endif
}

// ============================================================================
// API ENDPOINT HANDLERS
// ============================================================================

void ArduRoombaWiFi::handleStatus() {
    _totalRequests++;
    
    if (!checkAuthentication()) return;
    addCORSHeaders();
    
    // Get WiFi info
    IPAddress ip;
    int32_t rssi;
    String ssid;
    getConnectionInfo(ip, rssi, ssid);
    
    // Get Roomba sensor data
    SensorData sensorData;
    bool sensorSuccess = (_roomba.updateSensorData(sensorData) == ErrorCode::SUCCESS);
    
    String json = "{";
    json += "\"system\":{";
    json += "\"platform\":\"" + getPlatform() + "\",";
    json += "\"version\":\"2.4.0\",";
    json += "\"uptime\":" + String(millis() - _startTime) + ",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"requests\":" + String(_totalRequests) + ",";
    json += "\"errors\":" + String(_errorCount);
    json += "},";
    json += "\"wifi\":{";
    json += "\"status\":\"" + String(static_cast<uint8_t>(_status)) + "\",";
    json += "\"ssid\":\"" + ssid + "\",";
    json += "\"ip\":\"" + ip.toString() + "\",";
    json += "\"rssi\":" + String(rssi);
    json += "},";
    json += "\"roomba\":{";
    json += "\"initialized\":" + String(_roomba.isInitialized() ? "true" : "false") + ",";
    json += "\"connected\":" + String(sensorSuccess ? "true" : "false");
    if (sensorSuccess) {
        json += ",\"battery\":" + String(sensorData.getBatteryPercentage());
        json += ",\"voltage\":" + String(sensorData.voltage);
        json += ",\"mode\":" + String(static_cast<uint8_t>(sensorData.mode));
    }
    json += "}";
    json += "}";
    
    sendJSON(200, json);
}

void ArduRoombaWiFi::handleInitialize() {
    _totalRequests++;
    
    if (!checkAuthentication()) return;
    addCORSHeaders();
    
    ErrorCode result = _roomba.initialize();
    
    String json = "{";
    json += "\"success\":" + String(result == ErrorCode::SUCCESS ? "true" : "false") + ",";
    json += "\"error_code\":" + String(static_cast<uint8_t>(result)) + ",";
    json += "\"message\":\"";
    
    switch (result) {
        case ErrorCode::SUCCESS:
            json += "Roomba initialized successfully";
            break;
        case ErrorCode::TIMEOUT:
            json += "Initialization timeout";
            break;
        case ErrorCode::COMMUNICATION_ERROR:
            json += "Communication error";
            break;
        default:
            json += "Unknown error";
            break;
    }
    
    json += "\"}";
    sendJSON(result == ErrorCode::SUCCESS ? 200 : 500, json);
}

void ArduRoombaWiFi::handleSensorsAll() {
    _totalRequests++;
    
    if (!checkAuthentication()) return;
    addCORSHeaders();
    
    SensorData data;
    ErrorCode result = _roomba.updateSensorData(data);
    
    if (result != ErrorCode::SUCCESS) {
        sendError(500, "Failed to read sensor data");
        return;
    }
    
    String json = "{";
    json += "\"timestamp\":" + String(millis()) + ",";
    json += "\"battery\":{";
    json += "\"percentage\":" + String(data.getBatteryPercentage()) + ",";
    json += "\"voltage\":" + String(data.voltage) + ",";
    json += "\"current\":" + String(data.current) + ",";
    json += "\"charge\":" + String(data.batteryCharge) + ",";
    json += "\"capacity\":" + String(data.batteryCapacity) + ",";
    json += "\"temperature\":" + String(data.temperature);
    json += "},";
    json += "\"sensors\":{";
    json += "\"wall\":" + String(data.wall ? "true" : "false") + ",";
    json += "\"cliff_left\":" + String(data.cliffLeft ? "true" : "false") + ",";
    json += "\"cliff_front_left\":" + String(data.cliffFrontLeft ? "true" : "false") + ",";
    json += "\"cliff_front_right\":" + String(data.cliffFrontRight ? "true" : "false") + ",";
    json += "\"cliff_right\":" + String(data.cliffRight ? "true" : "false") + ",";
    json += "\"bump_left\":" + String(data.bumpLeft ? "true" : "false") + ",";
    json += "\"bump_right\":" + String(data.bumpRight ? "true" : "false") + ",";
    json += "\"wheel_drop_left\":" + String(data.wheelDropLeft ? "true" : "false") + ",";
    json += "\"wheel_drop_right\":" + String(data.wheelDropRight ? "true" : "false");
    json += "},";
    json += "\"mode\":" + String(static_cast<uint8_t>(data.mode)) + ",";
    json += "\"age\":" + String(data.getAge());
    json += "}";
    
    sendJSON(200, json);
}

void ArduRoombaWiFi::handleMovementForward() {
    _totalRequests++;
    
    if (!checkAuthentication()) return;
    addCORSHeaders();
    
    int velocity = getIntParam("velocity", 200);
    ErrorCode result = _roomba.moveForward(velocity);
    
    String json = "{";
    json += "\"success\":" + String(result == ErrorCode::SUCCESS ? "true" : "false") + ",";
    json += "\"command\":\"forward\",";
    json += "\"velocity\":" + String(velocity) + ",";
    json += "\"error_code\":" + String(static_cast<uint8_t>(result));
    json += "}";
    
    sendJSON(result == ErrorCode::SUCCESS ? 200 : 500, json);
}

void ArduRoombaWiFi::handleMovementStop() {
    _totalRequests++;
    
    if (!checkAuthentication()) return;
    addCORSHeaders();
    
    ErrorCode result = _roomba.stopMovement();
    
    String json = "{";
    json += "\"success\":" + String(result == ErrorCode::SUCCESS ? "true" : "false") + ",";
    json += "\"command\":\"stop\",";
    json += "\"error_code\":" + String(static_cast<uint8_t>(result));
    json += "}";
    
    sendJSON(result == ErrorCode::SUCCESS ? 200 : 500, json);
}

// ============================================================================
// UTILITY METHODS
// ============================================================================

void ArduRoombaWiFi::sendJSON(int code, const String& json) {
#if defined(ESP32) || defined(ESP8266)
    if (_server) {
        _server->send(code, "application/json", json);
    }
#endif
}

void ArduRoombaWiFi::sendError(int code, const String& message) {
    _errorCount++;
    String json = "{\"error\":\"" + message + "\",\"code\":" + String(code) + "}";
    sendJSON(code, json);
}

void ArduRoombaWiFi::addCORSHeaders() {
    if (!_corsEnabled) return;
    
#if defined(ESP32) || defined(ESP8266)
    if (_server) {
        _server->sendHeader("Access-Control-Allow-Origin", "*");
        _server->sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        _server->sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-API-Key");
    }
#endif
}

bool ArduRoombaWiFi::checkAuthentication() {
    if (_apiKey.length() == 0) return true; // No authentication required
    
#if defined(ESP32) || defined(ESP8266)
    if (_server) {
        String providedKey = _server->header("X-API-Key");
        if (providedKey != _apiKey) {
            sendError(401, "Invalid API key");
            return false;
        }
    }
#endif
    
    return true;
}

int ArduRoombaWiFi::getIntParam(const String& name, int defaultValue) {
#if defined(ESP32) || defined(ESP8266)
    if (_server && _server->hasArg(name)) {
        return _server->arg(name).toInt();
    }
#endif
    return defaultValue;
}

String ArduRoombaWiFi::getServerURL() const {
    IPAddress ip;
    int32_t rssi;
    String ssid;
    getConnectionInfo(ip, rssi, ssid);
    
    String url = "http://" + ip.toString();
    if (_config.serverPort != 80) {
        url += ":" + String(_config.serverPort);
    }
    return url;
}

void ArduRoombaWiFi::getStatistics(unsigned long& uptime, uint32_t& requests, uint16_t& errors) const {
    uptime = millis() - _startTime;
    requests = _totalRequests;
    errors = _errorCount;
}

void ArduRoombaWiFi::resetStatistics() {
    _startTime = millis();
    _totalRequests = 0;
    _errorCount = 0;
}

void ArduRoombaWiFi::debugPrint(const String& message) {
    if (_debugEnabled) {
        Serial.print("[ArduRoombaWiFi] ");
        Serial.println(message);
    }
}

// ============================================================================
// PLACEHOLDER IMPLEMENTATIONS FOR REMAINING HANDLERS
// ============================================================================

void ArduRoombaWiFi::handleSensors() { handleSensorsAll(); }
void ArduRoombaWiFi::handleSensorsBasic() { /* TODO: Implement basic sensor subset */ }
void ArduRoombaWiFi::handleSensorsBattery() { /* TODO: Implement battery-only sensors */ }
void ArduRoombaWiFi::handleMovement() { sendError(400, "Specify movement command"); }
void ArduRoombaWiFi::handleMovementBackward() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleMovementTurnLeft() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleMovementTurnRight() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleMovementDrive() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleCleaning() { sendError(400, "Specify cleaning command"); }
void ArduRoombaWiFi::handleCleaningStart() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleCleaningSpot() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleCleaningDock() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleActuators() { sendError(400, "Specify actuator command"); }
void ArduRoombaWiFi::handleActuatorsMotors() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleActuatorsLEDs() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleActuatorsSound() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleMode() { sendError(400, "Specify mode command"); }
void ArduRoombaWiFi::handleModeSafe() { /* TODO: Implement */ }
void ArduRoombaWiFi::handleModeFull() { /* TODO: Implement */ }

void ArduRoombaWiFi::handleRoot() {
    String html = "<!DOCTYPE html><html><head><title>ArduRoomba Control</title></head>";
    html += "<body><h1>ArduRoomba WiFi Control</h1>";
    html += "<p>API endpoints available at /api/</p>";
    html += "<p>Status: <a href='/api/status'>/api/status</a></p>";
    html += "</body></html>";
    
#if defined(ESP32) || defined(ESP8266)
    if (_server) {
        _server->send(200, "text/html", html);
    }
#endif
}

void ArduRoombaWiFi::handleNotFound() {
    sendError(404, "Endpoint not found");
}

void ArduRoombaWiFi::handleOptions() {
    addCORSHeaders();
#if defined(ESP32) || defined(ESP8266)
    if (_server) {
        _server->send(200, "text/plain", "");
    }
#endif
}

} // namespace ArduRoomba