/**
 * @file ArduRoombaWiFiS3.cpp
 * @brief Implementation of WiFi control for Arduino Uno R4 WiFi
 */

#include "ArduRoombaWiFiS3.h"

#if defined(ARDUINO_UNOWIFIR4) || defined(ARDUINO_UNOR4_WIFI)

ArduRoombaWiFiS3::ArduRoombaWiFiS3(ArduRoomba& roomba)
  : ArduRoombaWiFi(roomba), _server(nullptr), _mode(WIFI_MODE_AP), _connected(false) {
}

ArduRoombaWiFiS3::~ArduRoombaWiFiS3() {
  end();
}

bool ArduRoombaWiFiS3::beginAP(const char* ssid, const char* password) {
  Serial.print("Creating WiFi AP: ");
  Serial.println(ssid);

  _mode = WIFI_MODE_AP;

  // Create access point
  int status;
  if (password && strlen(password) > 0) {
    status = WiFi.beginAP(ssid, password);
  } else {
    status = WiFi.beginAP(ssid);
  }

  if (status != WL_AP_LISTENING) {
    Serial.println("Failed to create AP");
    return false;
  }

  delay(1000);

  IPAddress ip = WiFi.localIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  _connected = true;
  return true;
}

bool ArduRoombaWiFiS3::beginClient(const char* ssid, const char* password) {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  _mode = WIFI_MODE_CLIENT;

  // Attempt to connect
  int status = WiFi.begin(ssid, password);

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

void ArduRoombaWiFiS3::end() {
  if (_server) {
    delete _server;
    _server = nullptr;
  }
  WiFi.end();
  _connected = false;
}

bool ArduRoombaWiFiS3::isConnected() const {
  if (_mode == WIFI_MODE_CLIENT) {
    return WiFi.status() == WL_CONNECTED;
  }
  return _connected;
}

String ArduRoombaWiFiS3::getModeString() const {
  return _mode == WIFI_MODE_AP ? "AP" : "Client";
}

String ArduRoombaWiFiS3::getIPAddress() const {
  IPAddress ip = WiFi.localIP();
  return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}

String ArduRoombaWiFiS3::getMACAddress() const {
  return String("TODO"); // WiFiS3 doesn't have simple MAC access
}

int ArduRoombaWiFiS3::getRSSI() const {
  if (_mode == WIFI_MODE_CLIENT && WiFi.status() == WL_CONNECTED) {
    return WiFi.RSSI();
  }
  return 0;
}

void ArduRoombaWiFiS3::startWebServer(uint16_t port) {
  if (_server) {
    delete _server;
  }

  _server = new WiFiServer(port);
  _server->begin();

  Serial.print("Web server started on port ");
  Serial.println(port);
  Serial.print("Access at: http://");
  Serial.println(getIPAddress());
}

void ArduRoombaWiFiS3::handleClient() {
  if (!_server) return;

  WiFiClient client = _server->available();
  if (client) {
    handleHTTPRequest(client);
  }
}

void ArduRoombaWiFiS3::handleHTTPRequest(WiFiClient& client) {
  String request = "";
  bool currentLineIsBlank = true;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      request += c;

      if (c == '\n' && currentLineIsBlank) {
        // End of HTTP request, process it

        // Parse request line (first line)
        int firstSpace = request.indexOf(' ');
        int secondSpace = request.indexOf(' ', firstSpace + 1);
        String path = request.substring(firstSpace + 1, secondSpace);

        // Handle different endpoints
        if (path.startsWith("/cmd")) {
          // Command endpoint: /cmd?action=forward&speed=200
          RoombaCommand cmd = parseCommand(request);
          CommandResult result = processCommand(cmd);

          if (result == CommandResult::SUCCESS) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Connection: close");
            client.println();
            client.println("OK");
          } else if (result == CommandResult::LOW_BATTERY) {
            client.println("HTTP/1.1 503 Service Unavailable");
            client.println("Content-Type: text/plain");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Connection: close");
            client.println();
            client.println("Low Battery");
          } else {
            client.println("HTTP/1.1 400 Bad Request");
            client.println("Content-Type: text/plain");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Connection: close");
            client.println();
            client.println("Bad Request");
          }
        }
        else if (path.startsWith("/status")) {
          // Status endpoint: returns JSON
          String json = generateStatusJSON();

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Connection: close");
          client.println();
          client.println(json);
        }
        else {
          // Main control page
          String html = generateControlPage();

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println(html);
        }
        break;
      }

      if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }

  delay(1);
  client.stop();
}

String ArduRoombaWiFiS3::parseGETParameter(const String& request, const String& param) {
  String searchStr = param + "=";
  int startIdx = request.indexOf(searchStr);

  if (startIdx == -1) {
    return "";
  }

  startIdx += searchStr.length();
  int endIdx = request.indexOf('&', startIdx);

  if (endIdx == -1) {
    endIdx = request.indexOf(' ', startIdx);
  }

  if (endIdx == -1) {
    endIdx = request.length();
  }

  return request.substring(startIdx, endIdx);
}

RoombaCommand ArduRoombaWiFiS3::parseCommand(const String& request) {
  RoombaCommand cmd;

  String action = parseGETParameter(request, "action");
  String speedStr = parseGETParameter(request, "speed");
  String durationStr = parseGETParameter(request, "duration");

  strncpy(cmd.action, action.c_str(), sizeof(cmd.action) - 1);
  cmd.action[sizeof(cmd.action) - 1] = '\0';
  cmd.speed = speedStr.length() > 0 ? speedStr.toInt() : 200;
  cmd.duration = durationStr.length() > 0 ? durationStr.toInt() : 0;

  return cmd;
}

#endif // ARDUINO_UNOWIFIR4
