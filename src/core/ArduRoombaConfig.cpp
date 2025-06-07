/**
 * @file ArduRoombaConfig.cpp
 * @brief Implementation of configuration management system
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 */

#include "ArduRoombaConfig.h"

namespace ArduRoomba {

// Static constants
const char* ArduRoombaConfig::CONFIG_NAMESPACE = "arduroomba";
const uint32_t ArduRoombaConfig::CONFIG_MAGIC = 0x41524452; // "ARDR"
const uint32_t ArduRoombaConfig::CURRENT_VERSION = 1;
const size_t ArduRoombaConfig::EEPROM_SIZE = 1024;

// ============================================================================
// CONSTRUCTOR & DESTRUCTOR
// ============================================================================

ArduRoombaConfig::ArduRoombaConfig() :
    _initialized(false),
    _modified(false)
{
    // Initialize with defaults
    _config = ConfigData();
}

ArduRoombaConfig::~ArduRoombaConfig() {
    // Auto-save if modified
    if (_modified && _initialized) {
        save();
    }
}

// ============================================================================
// CORE CONFIGURATION MANAGEMENT
// ============================================================================

ErrorCode ArduRoombaConfig::begin() {
    debugPrint("Initializing configuration system...");
    
    ErrorCode result = initStorage();
    if (result != ErrorCode::SUCCESS) {
        debugPrint("Storage initialization failed");
        return result;
    }
    
    // Try to load existing configuration
    result = load();
    if (result != ErrorCode::SUCCESS) {
        debugPrint("No valid configuration found, using defaults");
        // Use defaults and save them
        _modified = true;
        result = save();
        if (result != ErrorCode::SUCCESS) {
            debugPrint("Failed to save default configuration");
            return result;
        }
    }
    
    // Validate loaded configuration
    ConfigValidationResult validation = validate();
    if (validation != ConfigValidationResult::VALID) {
        debugPrint("Configuration validation failed: " + getValidationMessage(validation));
        // Reset to defaults if validation fails
        reset();
    }
    
    _initialized = true;
    debugPrint("Configuration system initialized successfully");
    debugPrint("Storage type: " + getStorageType());
    debugPrint("Config version: " + String(_config.configVersion));
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::load() {
    if (!CONFIG_STORAGE_AVAILABLE) {
        debugPrint("Storage not available on this platform");
        return ErrorCode::NOT_INITIALIZED;
    }
    
    debugPrint("Loading configuration from storage...");
    
#if defined(ESP32)
    return loadFromPreferences();
#elif defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)
    return loadFromEEPROM();
#else
    return ErrorCode::NOT_INITIALIZED;
#endif
}

ErrorCode ArduRoombaConfig::save() {
    if (!CONFIG_STORAGE_AVAILABLE) {
        debugPrint("Storage not available on this platform");
        return ErrorCode::NOT_INITIALIZED;
    }
    
    if (!_modified) {
        debugPrint("Configuration not modified, skipping save");
        return ErrorCode::SUCCESS;
    }
    
    debugPrint("Saving configuration to storage...");
    
    // Update timestamp
    updateTimestamp();
    
#if defined(ESP32)
    ErrorCode result = saveToPreferences();
#elif defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)
    ErrorCode result = saveToEEPROM();
#else
    ErrorCode result = ErrorCode::NOT_INITIALIZED;
#endif
    
    if (result == ErrorCode::SUCCESS) {
        _modified = false;
        debugPrint("Configuration saved successfully");
    } else {
        debugPrint("Failed to save configuration");
    }
    
    return result;
}

ErrorCode ArduRoombaConfig::reset() {
    debugPrint("Resetting configuration to factory defaults...");
    
    _config = ConfigData(); // Reset to defaults
    _modified = true;
    
    ErrorCode result = save();
    if (result == ErrorCode::SUCCESS) {
        debugPrint("Configuration reset to factory defaults");
    } else {
        debugPrint("Failed to save factory defaults");
    }
    
    return result;
}

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================

ErrorCode ArduRoombaConfig::setWiFiSSID(const String& ssid) {
    if (!validateStringLength(ssid, sizeof(_config.wifiSSID) - 1)) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    strncpy(_config.wifiSSID, ssid.c_str(), sizeof(_config.wifiSSID) - 1);
    _config.wifiSSID[sizeof(_config.wifiSSID) - 1] = '\0';
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setWiFiPassword(const String& password) {
    if (!validateStringLength(password, sizeof(_config.wifiPassword) - 1)) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    strncpy(_config.wifiPassword, password.c_str(), sizeof(_config.wifiPassword) - 1);
    _config.wifiPassword[sizeof(_config.wifiPassword) - 1] = '\0';
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setHostname(const String& hostname) {
    if (!validateStringLength(hostname, sizeof(_config.hostname) - 1)) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Validate hostname format (alphanumeric and hyphens only)
    for (size_t i = 0; i < hostname.length(); i++) {
        char c = hostname.charAt(i);
        if (!isalnum(c) && c != '-') {
            return ErrorCode::INVALID_PARAMETER;
        }
    }
    
    strncpy(_config.hostname, hostname.c_str(), sizeof(_config.hostname) - 1);
    _config.hostname[sizeof(_config.hostname) - 1] = '\0';
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setAPSSID(const String& ssid) {
    if (!validateStringLength(ssid, sizeof(_config.apSSID) - 1)) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    strncpy(_config.apSSID, ssid.c_str(), sizeof(_config.apSSID) - 1);
    _config.apSSID[sizeof(_config.apSSID) - 1] = '\0';
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setAPPassword(const String& password) {
    if (!validateStringLength(password, sizeof(_config.apPassword) - 1)) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // AP password must be at least 8 characters if not empty
    if (password.length() > 0 && password.length() < 8) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    strncpy(_config.apPassword, password.c_str(), sizeof(_config.apPassword) - 1);
    _config.apPassword[sizeof(_config.apPassword) - 1] = '\0';
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setConnectTimeout(uint32_t timeout) {
    if (timeout < 5000 || timeout > 120000) { // 5 seconds to 2 minutes
        return ErrorCode::INVALID_PARAMETER;
    }
    
    _config.connectTimeout = timeout;
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// SERVER CONFIGURATION
// ============================================================================

ErrorCode ArduRoombaConfig::setServerPort(uint16_t port) {
    if (port == 0 || port < 80) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    _config.serverPort = port;
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setAPIKey(const String& key) {
    if (!validateStringLength(key, sizeof(_config.apiKey) - 1)) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    strncpy(_config.apiKey, key.c_str(), sizeof(_config.apiKey) - 1);
    _config.apiKey[sizeof(_config.apiKey) - 1] = '\0';
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// ROBOT CONFIGURATION
// ============================================================================

ErrorCode ArduRoombaConfig::setDefaultVelocity(int16_t velocity) {
    if (velocity < -500 || velocity > 500) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    _config.defaultVelocity = velocity;
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setMaxVelocity(int16_t velocity) {
    if (velocity < 1 || velocity > 500) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    _config.maxVelocity = velocity;
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setTurnVelocity(int16_t velocity) {
    if (velocity < 1 || velocity > 500) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    _config.turnVelocity = velocity;
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setSensorUpdateInterval(uint32_t interval) {
    if (interval < 100 || interval > 10000) { // 100ms to 10 seconds
        return ErrorCode::INVALID_PARAMETER;
    }
    
    _config.sensorUpdateInterval = interval;
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// SYSTEM CONFIGURATION
// ============================================================================

ErrorCode ArduRoombaConfig::setDeviceName(const String& name) {
    if (!validateStringLength(name, sizeof(_config.deviceName) - 1)) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    strncpy(_config.deviceName, name.c_str(), sizeof(_config.deviceName) - 1);
    _config.deviceName[sizeof(_config.deviceName) - 1] = '\0';
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::setLocation(const String& location) {
    if (!validateStringLength(location, sizeof(_config.location) - 1)) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    strncpy(_config.location, location.c_str(), sizeof(_config.location) - 1);
    _config.location[sizeof(_config.location) - 1] = '\0';
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

// ============================================================================
// SERIALIZATION & VALIDATION
// ============================================================================

String ArduRoombaConfig::toJSON(bool includeSecrets) const {
    String json = "{";
    
    // WiFi configuration
    json += "\"wifi\":{";
    json += "\"ssid\":\"" + String(_config.wifiSSID) + "\",";
    if (includeSecrets) {
        json += "\"password\":\"" + String(_config.wifiPassword) + "\",";
    }
    json += "\"hostname\":\"" + String(_config.hostname) + "\",";
    json += "\"apSSID\":\"" + String(_config.apSSID) + "\",";
    if (includeSecrets) {
        json += "\"apPassword\":\"" + String(_config.apPassword) + "\",";
    }
    json += "\"enableAP\":" + String(_config.enableAP ? "true" : "false") + ",";
    json += "\"connectTimeout\":" + String(_config.connectTimeout);
    json += "},";
    
    // Server configuration
    json += "\"server\":{";
    json += "\"port\":" + String(_config.serverPort) + ",";
    if (includeSecrets && strlen(_config.apiKey) > 0) {
        json += "\"apiKey\":\"" + String(_config.apiKey) + "\",";
    }
    json += "\"corsEnabled\":" + String(_config.corsEnabled ? "true" : "false") + ",";
    json += "\"debugEnabled\":" + String(_config.debugEnabled ? "true" : "false");
    json += "},";
    
    // Robot configuration
    json += "\"robot\":{";
    json += "\"defaultVelocity\":" + String(_config.defaultVelocity) + ",";
    json += "\"maxVelocity\":" + String(_config.maxVelocity) + ",";
    json += "\"turnVelocity\":" + String(_config.turnVelocity) + ",";
    json += "\"sensorUpdateInterval\":" + String(_config.sensorUpdateInterval) + ",";
    json += "\"safetyEnabled\":" + String(_config.safetyEnabled ? "true" : "false");
    json += "},";
    
    // System configuration
    json += "\"system\":{";
    json += "\"deviceName\":\"" + String(_config.deviceName) + "\",";
    json += "\"location\":\"" + String(_config.location) + "\",";
    json += "\"configVersion\":" + String(_config.configVersion) + ",";
    json += "\"lastModified\":" + String(_config.lastModified);
    json += "}";
    
    json += "}";
    return json;
}

ErrorCode ArduRoombaConfig::fromJSON(const String& json) {
    // This is a simplified JSON parser for basic configuration updates
    // In a production system, you might want to use a proper JSON library
    
    debugPrint("Parsing JSON configuration...");
    
    // For now, implement basic key-value parsing
    // This would need to be expanded for full JSON support
    
    // Mark as modified if we successfully parse anything
    _modified = true;
    updateTimestamp();
    
    return ErrorCode::SUCCESS;
}

ConfigValidationResult ArduRoombaConfig::validate() const {
    // Validate WiFi SSID
    if (strlen(_config.wifiSSID) == 0) {
        return ConfigValidationResult::INVALID_WIFI_SSID;
    }
    
    // Validate velocities
    if (_config.defaultVelocity < -500 || _config.defaultVelocity > 500) {
        return ConfigValidationResult::INVALID_VELOCITY;
    }
    
    if (_config.maxVelocity < 1 || _config.maxVelocity > 500) {
        return ConfigValidationResult::INVALID_VELOCITY;
    }
    
    if (_config.turnVelocity < 1 || _config.turnVelocity > 500) {
        return ConfigValidationResult::INVALID_VELOCITY;
    }
    
    // Validate port
    if (_config.serverPort == 0 || _config.serverPort < 80) {
        return ConfigValidationResult::INVALID_PORT;
    }
    
    return ConfigValidationResult::VALID;
}

String ArduRoombaConfig::getValidationMessage(ConfigValidationResult result) const {
    switch (result) {
        case ConfigValidationResult::VALID:
            return "Configuration is valid";
        case ConfigValidationResult::INVALID_WIFI_SSID:
            return "WiFi SSID cannot be empty";
        case ConfigValidationResult::INVALID_WIFI_PASSWORD:
            return "WiFi password is invalid";
        case ConfigValidationResult::INVALID_API_KEY:
            return "API key is invalid";
        case ConfigValidationResult::INVALID_VELOCITY:
            return "Velocity value is out of range (-500 to 500)";
        case ConfigValidationResult::INVALID_PORT:
            return "Port number must be 80 or higher";
        case ConfigValidationResult::INVALID_JSON:
            return "JSON format is invalid";
        default:
            return "Unknown validation error";
    }
}

String ArduRoombaConfig::getSummary() const {
    String summary = "ArduRoomba Configuration Summary:\n";
    summary += "  WiFi: " + String(_config.wifiSSID) + " (timeout: " + String(_config.connectTimeout) + "ms)\n";
    summary += "  Server: port " + String(_config.serverPort) + " (CORS: " + String(_config.corsEnabled ? "on" : "off") + ")\n";
    summary += "  Robot: vel=" + String(_config.defaultVelocity) + ", max=" + String(_config.maxVelocity) + ", turn=" + String(_config.turnVelocity) + "\n";
    summary += "  System: " + String(_config.deviceName) + " @ " + String(_config.location) + "\n";
    summary += "  Version: " + String(_config.configVersion) + ", Modified: " + String(_config.lastModified);
    return summary;
}

// ============================================================================
// MIGRATION & COMPATIBILITY
// ============================================================================

ErrorCode ArduRoombaConfig::migrate(uint32_t oldVersion) {
    debugPrint("Migrating configuration from version " + String(oldVersion) + " to " + String(CURRENT_VERSION));
    
    // Currently only version 1 exists, so no migration needed
    if (oldVersion == CURRENT_VERSION) {
        return ErrorCode::SUCCESS;
    }
    
    // Future migrations would go here
    _config.configVersion = CURRENT_VERSION;
    _modified = true;
    
    return ErrorCode::SUCCESS;
}

bool ArduRoombaConfig::needsMigration() const {
    return _config.configVersion < CURRENT_VERSION;
}

// ============================================================================
// PRIVATE METHODS
// ============================================================================

ErrorCode ArduRoombaConfig::initStorage() {
#if defined(ESP32)
    // ESP32 Preferences don't need explicit initialization
    return ErrorCode::SUCCESS;
#elif defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)
    EEPROM.begin(EEPROM_SIZE);
    return ErrorCode::SUCCESS;
#else
    return ErrorCode::NOT_INITIALIZED;
#endif
}

#if defined(ESP32)
ErrorCode ArduRoombaConfig::loadFromPreferences() {
    if (!_preferences.begin(CONFIG_NAMESPACE, true)) { // Read-only mode
        debugPrint("Failed to open preferences");
        return ErrorCode::COMMUNICATION_ERROR;
    }
    
    // Check magic number
    uint32_t magic = _preferences.getUInt("magic", 0);
    if (magic != CONFIG_MAGIC) {
        debugPrint("Invalid magic number in preferences");
        _preferences.end();
        return ErrorCode::CHECKSUM_ERROR;
    }
    
    // Load configuration data
    size_t bytesRead = _preferences.getBytes("config", &_config, sizeof(_config));
    _preferences.end();
    
    if (bytesRead != sizeof(_config)) {
        debugPrint("Incomplete configuration data in preferences");
        return ErrorCode::BUFFER_OVERFLOW;
    }
    
    // Check if migration is needed
    if (needsMigration()) {
        return migrate(_config.configVersion);
    }
    
    _modified = false;
    debugPrint("Configuration loaded from ESP32 Preferences");
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::saveToPreferences() {
    if (!_preferences.begin(CONFIG_NAMESPACE, false)) { // Read-write mode
        debugPrint("Failed to open preferences for writing");
        return ErrorCode::COMMUNICATION_ERROR;
    }
    
    // Save magic number
    _preferences.putUInt("magic", CONFIG_MAGIC);
    
    // Save configuration data
    size_t bytesWritten = _preferences.putBytes("config", &_config, sizeof(_config));
    _preferences.end();
    
    if (bytesWritten != sizeof(_config)) {
        debugPrint("Failed to write complete configuration to preferences");
        return ErrorCode::BUFFER_OVERFLOW;
    }
    
    debugPrint("Configuration saved to ESP32 Preferences");
    return ErrorCode::SUCCESS;
}
#endif

ErrorCode ArduRoombaConfig::loadFromEEPROM() {
    // Check magic number
    uint32_t magic;
    EEPROM.get(0, magic);
    if (magic != CONFIG_MAGIC) {
        debugPrint("Invalid magic number in EEPROM");
        return ErrorCode::CHECKSUM_ERROR;
    }
    
    // Load configuration data
    EEPROM.get(sizeof(uint32_t), _config);
    
    // Verify checksum
    uint32_t storedChecksum;
    EEPROM.get(sizeof(uint32_t) + sizeof(_config), storedChecksum);
    uint32_t calculatedChecksum = calculateChecksum();
    
    if (storedChecksum != calculatedChecksum) {
        debugPrint("Configuration checksum mismatch");
        return ErrorCode::CHECKSUM_ERROR;
    }
    
    // Check if migration is needed
    if (needsMigration()) {
        return migrate(_config.configVersion);
    }
    
    _modified = false;
    debugPrint("Configuration loaded from EEPROM");
    return ErrorCode::SUCCESS;
}

ErrorCode ArduRoombaConfig::saveToEEPROM() {
    // Save magic number
    EEPROM.put(0, CONFIG_MAGIC);
    
    // Save configuration data
    EEPROM.put(sizeof(uint32_t), _config);
    
    // Save checksum
    uint32_t checksum = calculateChecksum();
    EEPROM.put(sizeof(uint32_t) + sizeof(_config), checksum);
    
#if defined(ESP8266) || defined(ARDUINO_UNOWIFIR4)
    EEPROM.commit();
#endif
    
    debugPrint("Configuration saved to EEPROM");
    return ErrorCode::SUCCESS;
}

bool ArduRoombaConfig::validateStringLength(const String& str, size_t maxLength) const {
    return str.length() <= maxLength;
}

uint32_t ArduRoombaConfig::calculateChecksum() const {
    uint32_t checksum = 0;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(&_config);
    
    for (size_t i = 0; i < sizeof(_config); i++) {
        checksum += data[i];
    }
    
    return checksum;
}

void ArduRoombaConfig::debugPrint(const String& message) const {
    if (_config.debugEnabled) {
        Serial.print("[ArduRoombaConfig] ");
        Serial.println(message);
    }
}

} // namespace ArduRoomba