/**
 * @file ArduRoombaConfig.h
 * @brief Configuration management system for ArduRoomba
 * 
 * This file provides persistent configuration storage and management for
 * ArduRoomba settings. It enables web-based configuration, eliminates
 * hardcoded values, and provides factory reset capabilities.
 * 
 * @author Preetham Kyanam <preetham@preetham.org>
 * @version 2.5.0
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025 Preetham Kyanam
 * Licensed under GNU General Public License v3.0 (GPL-3.0)
 */

#ifndef ARDUROOMBACONFIG_H
#define ARDUROOMBACONFIG_H

#include <Arduino.h>
#include "../ArduRoombaConstants.h"

// Platform-specific storage includes
#if defined(ESP32)
    #include <Preferences.h>
    #define CONFIG_STORAGE_AVAILABLE true
    #define CONFIG_STORAGE_TYPE "Preferences"
#elif defined(ESP8266)
    #include <EEPROM.h>
    #define CONFIG_STORAGE_AVAILABLE true
    #define CONFIG_STORAGE_TYPE "EEPROM"
#elif defined(ARDUINO_UNOWIFIR4)
    #include <EEPROM.h>
    #define CONFIG_STORAGE_AVAILABLE true
    #define CONFIG_STORAGE_TYPE "EEPROM"
#else
    #define CONFIG_STORAGE_AVAILABLE false
    #define CONFIG_STORAGE_TYPE "None"
#endif

namespace ArduRoomba {

/**
 * @brief Configuration validation result
 */
enum class ConfigValidationResult : uint8_t {
    VALID = 0,              ///< Configuration is valid
    INVALID_WIFI_SSID = 1,  ///< WiFi SSID is invalid
    INVALID_WIFI_PASSWORD = 2, ///< WiFi password is invalid
    INVALID_API_KEY = 3,    ///< API key is invalid
    INVALID_VELOCITY = 4,   ///< Velocity value is out of range
    INVALID_PORT = 5,       ///< Port number is invalid
    INVALID_JSON = 6        ///< JSON format is invalid
};

/**
 * @brief Configuration structure for all ArduRoomba settings
 */
struct ConfigData {
    // WiFi Configuration
    char wifiSSID[64];          ///< WiFi network name (max 63 chars + null)
    char wifiPassword[64];      ///< WiFi password (max 63 chars + null)
    char hostname[32];          ///< Device hostname (max 31 chars + null)
    char apSSID[32];           ///< Access Point SSID for setup mode
    char apPassword[32];       ///< Access Point password
    bool enableAP;             ///< Enable AP mode if connection fails
    uint32_t connectTimeout;   ///< WiFi connection timeout (ms)
    
    // Server Configuration
    uint16_t serverPort;       ///< Web server port
    char apiKey[64];           ///< API authentication key (empty = disabled)
    bool corsEnabled;          ///< Enable CORS headers
    bool debugEnabled;         ///< Enable debug output
    
    // Robot Configuration
    int16_t defaultVelocity;   ///< Default movement velocity (mm/s)
    int16_t maxVelocity;       ///< Maximum allowed velocity (mm/s)
    int16_t turnVelocity;      ///< Default turning velocity (mm/s)
    uint32_t sensorUpdateInterval; ///< Sensor update interval (ms)
    bool safetyEnabled;        ///< Enable safety features (cliff detection, etc.)
    
    // System Configuration
    uint32_t configVersion;    ///< Configuration version for migration
    uint32_t lastModified;     ///< Last modification timestamp
    char deviceName[32];       ///< User-friendly device name
    char location[32];         ///< Device location description
    
    /**
     * @brief Default constructor with sensible defaults
     */
    ConfigData() {
        // WiFi defaults
        strcpy(wifiSSID, "");
        strcpy(wifiPassword, "");
        strcpy(hostname, "arduroomba");
        strcpy(apSSID, "ArduRoomba-Setup");
        strcpy(apPassword, "roomba123");
        enableAP = true;
        connectTimeout = 30000;
        
        // Server defaults
        serverPort = 80;
        strcpy(apiKey, "");
        corsEnabled = true;
        debugEnabled = false;
        
        // Robot defaults
        defaultVelocity = 200;
        maxVelocity = 500;
        turnVelocity = 150;
        sensorUpdateInterval = 1000;
        safetyEnabled = true;
        
        // System defaults
        configVersion = 1;
        lastModified = 0;
        strcpy(deviceName, "ArduRoomba");
        strcpy(location, "Home");
    }
};

/**
 * @brief Configuration management class for ArduRoomba
 * 
 * This class provides comprehensive configuration management including:
 * - Persistent storage across reboots
 * - Web-based configuration interface
 * - Configuration validation and migration
 * - Factory reset capabilities
 * - JSON serialization for API access
 */
class ArduRoombaConfig {
public:
    /**
     * @brief Constructor
     */
    ArduRoombaConfig();
    
    /**
     * @brief Destructor
     */
    ~ArduRoombaConfig();
    
    // ========================================================================
    // CORE CONFIGURATION MANAGEMENT
    // ========================================================================
    
    /**
     * @brief Initialize the configuration system
     * @return ErrorCode indicating success or failure
     */
    ErrorCode begin();
    
    /**
     * @brief Load configuration from persistent storage
     * @return ErrorCode indicating success or failure
     */
    ErrorCode load();
    
    /**
     * @brief Save current configuration to persistent storage
     * @return ErrorCode indicating success or failure
     */
    ErrorCode save();
    
    /**
     * @brief Reset configuration to factory defaults
     * @return ErrorCode indicating success or failure
     */
    ErrorCode reset();
    
    /**
     * @brief Check if configuration storage is available
     * @return true if storage is available, false otherwise
     */
    bool isStorageAvailable() const { return CONFIG_STORAGE_AVAILABLE; }
    
    /**
     * @brief Get storage type string
     * @return Storage type description
     */
    String getStorageType() const { return CONFIG_STORAGE_TYPE; }
    
    // ========================================================================
    // WIFI CONFIGURATION
    // ========================================================================
    
    /**
     * @brief Get WiFi SSID
     * @return WiFi network name
     */
    String getWiFiSSID() const { return String(_config.wifiSSID); }
    
    /**
     * @brief Set WiFi SSID
     * @param ssid WiFi network name (max 63 characters)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setWiFiSSID(const String& ssid);
    
    /**
     * @brief Get WiFi password
     * @return WiFi password
     */
    String getWiFiPassword() const { return String(_config.wifiPassword); }
    
    /**
     * @brief Set WiFi password
     * @param password WiFi password (max 63 characters)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setWiFiPassword(const String& password);
    
    /**
     * @brief Get device hostname
     * @return Device hostname
     */
    String getHostname() const { return String(_config.hostname); }
    
    /**
     * @brief Set device hostname
     * @param hostname Device hostname (max 31 characters)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setHostname(const String& hostname);
    
    /**
     * @brief Get Access Point SSID
     * @return AP SSID for setup mode
     */
    String getAPSSID() const { return String(_config.apSSID); }
    
    /**
     * @brief Set Access Point SSID
     * @param ssid AP SSID (max 31 characters)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setAPSSID(const String& ssid);
    
    /**
     * @brief Get Access Point password
     * @return AP password
     */
    String getAPPassword() const { return String(_config.apPassword); }
    
    /**
     * @brief Set Access Point password
     * @param password AP password (max 31 characters)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setAPPassword(const String& password);
    
    /**
     * @brief Check if AP mode is enabled
     * @return true if AP mode enabled, false otherwise
     */
    bool isAPEnabled() const { return _config.enableAP; }
    
    /**
     * @brief Enable or disable AP mode
     * @param enabled true to enable AP mode, false to disable
     */
    void setAPEnabled(bool enabled) { _config.enableAP = enabled; _modified = true; }
    
    /**
     * @brief Get WiFi connection timeout
     * @return Connection timeout in milliseconds
     */
    uint32_t getConnectTimeout() const { return _config.connectTimeout; }
    
    /**
     * @brief Set WiFi connection timeout
     * @param timeout Connection timeout in milliseconds
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setConnectTimeout(uint32_t timeout);
    
    // ========================================================================
    // SERVER CONFIGURATION
    // ========================================================================
    
    /**
     * @brief Get web server port
     * @return Server port number
     */
    uint16_t getServerPort() const { return _config.serverPort; }
    
    /**
     * @brief Set web server port
     * @param port Server port number (1-65535)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setServerPort(uint16_t port);
    
    /**
     * @brief Get API key
     * @return API authentication key (empty if disabled)
     */
    String getAPIKey() const { return String(_config.apiKey); }
    
    /**
     * @brief Set API key
     * @param key API authentication key (empty to disable, max 63 characters)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setAPIKey(const String& key);
    
    /**
     * @brief Check if CORS is enabled
     * @return true if CORS enabled, false otherwise
     */
    bool isCORSEnabled() const { return _config.corsEnabled; }
    
    /**
     * @brief Enable or disable CORS
     * @param enabled true to enable CORS, false to disable
     */
    void setCORSEnabled(bool enabled) { _config.corsEnabled = enabled; _modified = true; }
    
    /**
     * @brief Check if debug output is enabled
     * @return true if debug enabled, false otherwise
     */
    bool isDebugEnabled() const { return _config.debugEnabled; }
    
    /**
     * @brief Enable or disable debug output
     * @param enabled true to enable debug, false to disable
     */
    void setDebugEnabled(bool enabled) { _config.debugEnabled = enabled; _modified = true; }
    
    // ========================================================================
    // ROBOT CONFIGURATION
    // ========================================================================
    
    /**
     * @brief Get default movement velocity
     * @return Default velocity in mm/s
     */
    int16_t getDefaultVelocity() const { return _config.defaultVelocity; }
    
    /**
     * @brief Set default movement velocity
     * @param velocity Default velocity in mm/s (-500 to 500)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setDefaultVelocity(int16_t velocity);
    
    /**
     * @brief Get maximum allowed velocity
     * @return Maximum velocity in mm/s
     */
    int16_t getMaxVelocity() const { return _config.maxVelocity; }
    
    /**
     * @brief Set maximum allowed velocity
     * @param velocity Maximum velocity in mm/s (1 to 500)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setMaxVelocity(int16_t velocity);
    
    /**
     * @brief Get default turning velocity
     * @return Turn velocity in mm/s
     */
    int16_t getTurnVelocity() const { return _config.turnVelocity; }
    
    /**
     * @brief Set default turning velocity
     * @param velocity Turn velocity in mm/s (1 to 500)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setTurnVelocity(int16_t velocity);
    
    /**
     * @brief Get sensor update interval
     * @return Update interval in milliseconds
     */
    uint32_t getSensorUpdateInterval() const { return _config.sensorUpdateInterval; }
    
    /**
     * @brief Set sensor update interval
     * @param interval Update interval in milliseconds (100 to 10000)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setSensorUpdateInterval(uint32_t interval);
    
    /**
     * @brief Check if safety features are enabled
     * @return true if safety enabled, false otherwise
     */
    bool isSafetyEnabled() const { return _config.safetyEnabled; }
    
    /**
     * @brief Enable or disable safety features
     * @param enabled true to enable safety, false to disable
     */
    void setSafetyEnabled(bool enabled) { _config.safetyEnabled = enabled; _modified = true; }
    
    // ========================================================================
    // SYSTEM CONFIGURATION
    // ========================================================================
    
    /**
     * @brief Get device name
     * @return User-friendly device name
     */
    String getDeviceName() const { return String(_config.deviceName); }
    
    /**
     * @brief Set device name
     * @param name User-friendly device name (max 31 characters)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setDeviceName(const String& name);
    
    /**
     * @brief Get device location
     * @return Device location description
     */
    String getLocation() const { return String(_config.location); }
    
    /**
     * @brief Set device location
     * @param location Device location description (max 31 characters)
     * @return ErrorCode indicating success or failure
     */
    ErrorCode setLocation(const String& location);
    
    /**
     * @brief Get configuration version
     * @return Configuration version number
     */
    uint32_t getConfigVersion() const { return _config.configVersion; }
    
    /**
     * @brief Get last modification timestamp
     * @return Last modification time (millis())
     */
    uint32_t getLastModified() const { return _config.lastModified; }
    
    // ========================================================================
    // SERIALIZATION & VALIDATION
    // ========================================================================
    
    /**
     * @brief Convert configuration to JSON string
     * @param includeSecrets Include sensitive data (passwords, API keys)
     * @return JSON representation of configuration
     */
    String toJSON(bool includeSecrets = false) const;
    
    /**
     * @brief Load configuration from JSON string
     * @param json JSON configuration string
     * @return ErrorCode indicating success or failure
     */
    ErrorCode fromJSON(const String& json);
    
    /**
     * @brief Validate current configuration
     * @return ConfigValidationResult indicating validation status
     */
    ConfigValidationResult validate() const;
    
    /**
     * @brief Get validation error message
     * @param result Validation result to get message for
     * @return Human-readable error message
     */
    String getValidationMessage(ConfigValidationResult result) const;
    
    /**
     * @brief Check if configuration has been modified since last save
     * @return true if modified, false otherwise
     */
    bool isModified() const { return _modified; }
    
    /**
     * @brief Get configuration summary for debugging
     * @return Summary string with key configuration values
     */
    String getSummary() const;
    
    // ========================================================================
    // MIGRATION & COMPATIBILITY
    // ========================================================================
    
    /**
     * @brief Migrate configuration from older version
     * @param oldVersion Previous configuration version
     * @return ErrorCode indicating success or failure
     */
    ErrorCode migrate(uint32_t oldVersion);
    
    /**
     * @brief Check if configuration needs migration
     * @return true if migration needed, false otherwise
     */
    bool needsMigration() const;
    
    /**
     * @brief Get raw configuration data (for advanced use)
     * @return Reference to internal configuration structure
     */
    const ConfigData& getRawConfig() const { return _config; }

private:
    // ========================================================================
    // PRIVATE MEMBERS
    // ========================================================================
    
    ConfigData _config;         ///< Current configuration data
    bool _initialized;          ///< Initialization status
    bool _modified;             ///< Modification flag
    
#if defined(ESP32)
    Preferences _preferences;   ///< ESP32 preferences storage
#endif // ARDUROOMBACONFIG_H
    // PRIVATE METHODS
    // ========================================================================
    
    /**
     * @brief Initialize storage system
     * @return ErrorCode indicating success or failure
     */
    ErrorCode initStorage();
    
    /**
     * @brief Load configuration from EEPROM
     * @return ErrorCode indicating success or failure
     */
    ErrorCode loadFromEEPROM();
    
    /**
     * @brief Save configuration to EEPROM
     * @return ErrorCode indicating success or failure
     */
    ErrorCode saveToEEPROM();
    
#if defined(ESP32)
    /**
     * @brief Load configuration from ESP32 Preferences
     * @return ErrorCode indicating success or failure
     */
    ErrorCode loadFromPreferences();
    
    /**
     * @brief Save configuration to ESP32 Preferences
     * @return ErrorCode indicating success or failure
     */
    ErrorCode saveToPreferences();
#endif
    
    /**
     * @brief Validate string parameter length
     * @param str String to validate
     * @param maxLength Maximum allowed length
     * @return true if valid, false otherwise
     */
    bool validateStringLength(const String& str, size_t maxLength) const;
    
    /**
     * @brief Update last modified timestamp
     */
    void updateTimestamp() { _config.lastModified = millis(); _modified = true; }
    
    /**
     * @brief Calculate configuration checksum
     * @return Checksum value
     */
    uint32_t calculateChecksum() const;
    
    /**
     * @brief Debug print method
     * @param message Message to print
     */
    void debugPrint(const String& message) const;
    
    // Constants
    static const char* CONFIG_NAMESPACE;    ///< Storage namespace
    static const uint32_t CONFIG_MAGIC;     ///< Magic number for validation
    static const uint32_t CURRENT_VERSION;  ///< Current configuration version
    static const size_t EEPROM_SIZE;        ///< EEPROM size for configuration
};

} // namespace ArduRoomba

#endif // ARDUROOMBACONFIG_H