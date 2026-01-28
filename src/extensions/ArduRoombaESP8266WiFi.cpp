/**
 * @file ArduRoombaESP8266WiFi.cpp
 * @brief Implementation of WiFi control for ESP8266
 */

#include "ArduRoombaESP8266WiFi.h"

#if defined(ESP8266)

ArduRoombaESP8266WiFi::ArduRoombaESP8266WiFi(ArduRoomba& roomba)
  : ArduRoombaWiFi(roomba), _server(nullptr), _wifiMode(WiFiMode::AP), _connected(false) {
}

ArduRoombaESP8266WiFi::~ArduRoombaESP8266WiFi() {
  end();
}

bool ArduRoombaESP8266WiFi::beginAP(const char* ssid, const char* password) {
  Serial.print("Creating WiFi AP: ");
  Serial.println(ssid);

  _wifiMode = WiFiMode::AP;

  // Create access point
  bool success;
  if (password && strlen(password) > 0) {
    success = WiFi.softAP(ssid, password);
  } else {
    success = WiFi.softAP(ssid);
  }

  if (!success) {
    Serial.println("Failed to create AP");
    return false;
  }

  delay(100);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  _connected = true;
  return true;
}

bool ArduRoombaESP8266WiFi::beginClient(const char* ssid, const char* password) {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  _wifiMode = WiFiMode::CLIENT;

  // Connect to WiFi
  WiFi.begin(ssid, password);

  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
    return false;
  }

  IPAddress ip = WiFi.localIP();
  Serial.print("Connected! IP address: ");
  Serial.println(ip);

  _connected = true;
  return true;
}

void ArduRoombaESP8266WiFi::end() {
  if (_server) {
    _server->stop();
    delete _server;
    _server = nullptr;
  }

  if (_wifiMode == WiFiMode::AP) {
    WiFi.softAPdisconnect(true);
  } else {
    WiFi.disconnect(true);
  }

  _connected = false;
}

bool ArduRoombaESP8266WiFi::isConnected() const {
  if (_wifiMode == WiFiMode::CLIENT) {
    return WiFi.status() == WL_CONNECTED;
  }
  return _connected;
}

String ArduRoombaESP8266WiFi::getModeString() const {
  return _wifiMode == WiFiMode::AP ? "AP" : "Client";
}

String ArduRoombaESP8266WiFi::getIPAddress() const {
  if (_wifiMode == WiFiMode::AP) {
    return WiFi.softAPIP().toString();
  }
  return WiFi.localIP().toString();
}

String ArduRoombaESP8266WiFi::getMACAddress() const {
  return WiFi.macAddress();
}

int ArduRoombaESP8266WiFi::getRSSI() const {
  if (_wifiMode == WiFiMode::CLIENT && WiFi.status() == WL_CONNECTED) {
    return WiFi.RSSI();
  }
  return 0;
}

void ArduRoombaESP8266WiFi::startWebServer(uint16_t port) {
  if (_server) {
    _server->stop();
    delete _server;
  }

  _server = new ESP8266WebServer(port);
  _serverPort = port;

  // Setup routes
  _server->on("/", [this]() { handleRoot(); });
  _server->on("/cmd", HTTP_GET, [this]() { handleCommand(); });
  _server->on("/status", [this]() { handleStatus(); });
  _server->onNotFound([this]() { handleNotFound(); });

  _server->begin();

  Serial.print("Web server started on port ");
  Serial.println(port);
  Serial.print("Access at: http://");
  Serial.println(getIPAddress());
}

void ArduRoombaESP8266WiFi::handleClient() {
  if (_server) {
    _server->handleClient();
  }
}

void ArduRoombaESP8266WiFi::handleRoot() {
  String html = generateControlPage();
  _server->send(200, "text/html", html);
}

void ArduRoombaESP8266WiFi::handleCommand() {
  RoombaCommand cmd = parseCommand();

  CommandResult result = processCommand(cmd);

  if (result == CommandResult::SUCCESS) {
    _server->sendHeader("Access-Control-Allow-Origin", "*");
    _server->send(200, "text/plain", "OK");
  } else if (result == CommandResult::LOW_BATTERY) {
    _server->sendHeader("Access-Control-Allow-Origin", "*");
    _server->send(503, "text/plain", "Low Battery");
  } else {
    _server->sendHeader("Access-Control-Allow-Origin", "*");
    _server->send(400, "text/plain", "Bad Request");
  }
}

void ArduRoombaESP8266WiFi::handleStatus() {
  String json = generateStatusJSON();
  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->send(200, "application/json", json);
}

void ArduRoombaESP8266WiFi::handleNotFound() {
  _server->send(404, "text/plain", "Not Found");
}

RoombaCommand ArduRoombaESP8266WiFi::parseCommand() {
  RoombaCommand cmd;

  if (_server->hasArg("action")) {
    String action = _server->arg("action");
    strncpy(cmd.action, action.c_str(), sizeof(cmd.action) - 1);
    cmd.action[sizeof(cmd.action) - 1] = '\0';
  }

  if (_server->hasArg("speed")) {
    cmd.speed = _server->arg("speed").toInt();
  }

  if (_server->hasArg("duration")) {
    cmd.duration = _server->arg("duration").toInt();
  }

  return cmd;
}

#endif // ESP8266
