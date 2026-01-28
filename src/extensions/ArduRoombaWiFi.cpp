/**
 * @file ArduRoombaWiFi.cpp
 * @brief Implementation of base WiFi functionality
 */

#include "ArduRoombaWiFi.h"

ArduRoombaWiFi::ArduRoombaWiFi(ArduRoomba& roomba)
  : _roomba(roomba), _remoteEnabled(true), _commandCallback(nullptr),
    _lowBatteryThreshold(12000), _serverPort(80) {
}

CommandResult ArduRoombaWiFi::processCommand(const RoombaCommand& cmd) {
  if (!_remoteEnabled) {
    return CommandResult::ERROR;
  }

  // Validate action
  String action = String(cmd.action);
  if (!isValidAction(action)) {
    return CommandResult::UNKNOWN_ACTION;
  }

  // Check battery
  uint16_t voltage = _roomba.getBatteryVoltage();
  if (voltage > 0 && voltage < _lowBatteryThreshold) {
    return CommandResult::LOW_BATTERY;
  }

  // Call user callback if set
  if (_commandCallback) {
    _commandCallback(cmd, CommandResult::SUCCESS);
  }

  // Process standard commands
  if (action == "forward") {
    _roomba.moveForward(cmd.speed > 0 ? cmd.speed : 200);
  }
  else if (action == "backward") {
    _roomba.moveBackward(cmd.speed > 0 ? cmd.speed : 200);
  }
  else if (action == "left") {
    _roomba.turnLeft(cmd.speed > 0 ? cmd.speed : 200);
  }
  else if (action == "right") {
    _roomba.turnRight(cmd.speed > 0 ? cmd.speed : 200);
  }
  else if (action == "spinLeft") {
    _roomba.spinLeft(cmd.speed > 0 ? cmd.speed : 200);
  }
  else if (action == "spinRight") {
    _roomba.spinRight(cmd.speed > 0 ? cmd.speed : 200);
  }
  else if (action == "stop") {
    _roomba.stop();
  }
  else if (action == "clean") {
    _roomba.startCleaning();
  }
  else if (action == "spot") {
    _roomba.spotClean();
  }
  else if (action == "dock") {
    _roomba.dock();
  }
  else if (action == "beep") {
    _roomba.beep();
  }

  // Handle timed commands
  if (cmd.duration > 0 && action != "stop" && action != "clean" &&
      action != "spot" && action != "dock") {
    delay(cmd.duration);
    _roomba.stop();
  }

  return CommandResult::SUCCESS;
}

bool ArduRoombaWiFi::isValidAction(const String& action) const {
  return action == "forward" || action == "backward" ||
         action == "left" || action == "right" ||
         action == "spinLeft" || action == "spinRight" ||
         action == "stop" || action == "clean" ||
         action == "spot" || action == "dock" || action == "beep";
}

String ArduRoombaWiFi::generateControlPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ArduRoomba Control</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      text-align: center;
      background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
      color: #fff;
      padding: 20px;
      min-height: 100vh;
    }
    h1 { margin-bottom: 20px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }
    .container { max-width: 400px; margin: 0 auto; }
    .status {
      background: rgba(255,255,255,0.1);
      backdrop-filter: blur(10px);
      border-radius: 12px;
      padding: 15px;
      margin-bottom: 20px;
    }
    .status-item { display: flex; justify-content: space-between; margin: 5px 0; }
    .status-label { opacity: 0.8; }
    .status-value { font-weight: bold; }
    .controls {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 10px;
      margin-bottom: 20px;
    }
    button {
      padding: 20px;
      font-size: 24px;
      background: rgba(255,255,255,0.2);
      border: none;
      border-radius: 12px;
      color: white;
      cursor: pointer;
      transition: all 0.2s;
      backdrop-filter: blur(5px);
    }
    button:hover { background: rgba(255,255,255,0.3); transform: scale(1.05); }
    button:active { transform: scale(0.95); }
    .forward { grid-column: 2; }
    .left { grid-column: 1; grid-row: 2; }
    .stop {
      grid-column: 2; grid-row: 2;
      background: rgba(231, 76, 60, 0.8) !important;
      font-size: 18px;
      font-weight: bold;
    }
    .right { grid-column: 3; grid-row: 2; }
    .backward { grid-column: 2; grid-row: 3; }
    .spin-left { grid-column: 1; grid-row: 3; font-size: 14px; }
    .spin-right { grid-column: 3; grid-row: 3; font-size: 14px; }
    .actions { display: flex; flex-wrap: wrap; gap: 10px; justify-content: center; }
    .actions button {
      padding: 12px 20px;
      font-size: 14px;
      background: rgba(39, 174, 96, 0.8);
    }
    .slider-container {
      background: rgba(255,255,255,0.1);
      border-radius: 12px;
      padding: 15px;
      margin-bottom: 20px;
    }
    input[type=range] {
      width: 100%;
      height: 8px;
      border-radius: 4px;
      background: rgba(255,255,255,0.3);
      outline: none;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🤖 ArduRoomba</h1>
    <div class="status">
      <div class="status-item">
        <span class="status-label">Battery</span>
        <span class="status-value" id="voltage">-- mV</span>
      </div>
      <div class="status-item">
        <span class="status-label">Status</span>
        <span class="status-value" id="status">Connected</span>
      </div>
    </div>
    <div class="slider-container">
      <label>Speed: <span id="speedValue">200</span> mm/s</label>
      <input type="range" id="speed" min="0" max="500" value="200">
    </div>
    <div class="controls">
      <button class="forward" onclick="send('forward')">▲</button>
      <button class="left" onclick="send('left')">◀</button>
      <button class="stop" onclick="send('stop')">⏹</button>
      <button class="right" onclick="send('right')">▶</button>
      <button class="backward" onclick="send('backward')">▼</button>
      <button class="spin-left" onclick="send('spinLeft')">↺</button>
      <button class="spin-right" onclick="send('spinRight')">↻</button>
    </div>
    <div class="actions">
      <button onclick="send('clean')">🧹 Clean</button>
      <button onclick="send('spot')">⚡ Spot</button>
      <button onclick="send('dock')">🏠 Dock</button>
      <button onclick="send('beep')">🔔 Beep</button>
    </div>
  </div>
  <script>
    let currentSpeed = 200;

    document.getElementById('speed').addEventListener('input', function(e) {
      currentSpeed = e.target.value;
      document.getElementById('speedValue').textContent = currentSpeed;
    });

    function send(action) {
      fetch('/cmd?action=' + action + '&speed=' + currentSpeed)
        .then(r => r.text())
        .then(t => console.log(t))
        .catch(e => console.error(e));
    }

    function updateStatus() {
      fetch('/status')
        .then(r => r.json())
        .then(d => {
          document.getElementById('voltage').textContent = d.voltage + ' mV';
          const statusEl = document.getElementById('status');
          if (d.voltage < 13000) {
            statusEl.textContent = '⚠️ Low Battery';
            statusEl.style.color = '#f1c40f';
          } else if (d.bumper) {
            statusEl.textContent = '⚠️ Bumper Hit';
            statusEl.style.color = '#e74c3c';
          } else {
            statusEl.textContent = '✓ Ready';
            statusEl.style.color = '#2ecc71';
          }
        })
        .catch(e => console.error(e));
    }

    setInterval(updateStatus, 2000);
    updateStatus();
  </script>
</body>
</html>
)rawliteral";
  return html;
}

String ArduRoombaWiFi::generateStatusJSON() {
  uint16_t voltage = _roomba.getBatteryVoltage();
  bool connected = _roomba.isConnected();
  bool bumper = _roomba.isBumperPressed();
  bool wall = _roomba.isWallDetected();

  String json = "{";
  json += "\"voltage\":" + String(voltage) + ",";
  json += "\"connected\":" + String(connected ? "true" : "false") + ",";
  json += "\"bumper\":" + String(bumper ? "true" : "false") + ",";
  json += "\"wall\":" + String(wall ? "true" : "false") + ",";
  json += "\"remote_enabled\":" + String(_remoteEnabled ? "true" : "false");
  json += "}";

  return json;
}

String ArduRoombaWiFi::generateExtendedStatusJSON() {
  uint16_t voltage = _roomba.getBatteryVoltage();
  int16_t current = _roomba.getBatteryCurrent();
  uint8_t percent = _roomba.getBatteryPercent();
  bool connected = _roomba.isConnected();
  bool bumper = _roomba.isBumperPressed();
  bool wall = _roomba.isWallDetected();
  bool cliff = _roomba.isCliffDetected();

  String json = "{";
  json += "\"voltage\":" + String(voltage) + ",";
  json += "\"current\":" + String(current) + ",";
  json += "\"battery_percent\":" + String(percent) + ",";
  json += "\"connected\":" + String(connected ? "true" : "false") + ",";
  json += "\"bumper\":" + String(bumper ? "true" : "false") + ",";
  json += "\"wall\":" + String(wall ? "true" : "false") + ",";
  json += "\"cliff\":" + String(cliff ? "true" : "false") + ",";
  json += "\"remote_enabled\":" + String(_remoteEnabled ? "true" : "false") + ",";
  json += "\"mode\":\"" + getModeString() + "\",";
  json += "\"ip\":\"" + getIPAddress() + "\"";
  json += "}";

  return json;
}

void ArduRoombaWiFi::startWebServer(uint16_t port) {
  _serverPort = port;
  // Implemented by platform-specific class
}
