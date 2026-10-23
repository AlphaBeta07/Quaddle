#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char* ssid = "Quadruped-Robot";
const char* password = "robot123";

ESP8266WebServer server(80);

Servo frontLeftServo;
Servo frontRightServo;
Servo backLeftServo;
Servo backRightServo;

const int frontLeftPin  = D1;
const int frontRightPin = D2;
const int backLeftPin   = D3;
const int backRightPin  = D4;

const int ledLeft  = D7;
const int ledRight = D8;

int midPos = 90;

enum RobotMotion {
  MOTION_STOP,
  MOTION_FORWARD,
  MOTION_BACKWARD,
  MOTION_LEFT,
  MOTION_RIGHT
};

RobotMotion currentMotion = MOTION_STOP;

unsigned long lastStepTime = 0;
unsigned long lastJoystickCommand = 0;

const unsigned long stepInterval = 350;
const unsigned long commandTimeout = 500;

bool walkingStep = false;

int limitAngle(int angle) {
  if (angle < 10) return 10;
  if (angle > 170) return 170;
  return angle;
}

void stopRobot() {

  currentMotion = MOTION_STOP;
  walkingStep = false;

  frontLeftServo.write(midPos);
  frontRightServo.write(midPos);
  backLeftServo.write(midPos);
  backRightServo.write(midPos);

  digitalWrite(ledLeft, HIGH);
  digitalWrite(ledRight, HIGH);
}

void forwardStep() {

  if (!walkingStep) {
    frontLeftServo.write(limitAngle(midPos + 45));
    frontRightServo.write(limitAngle(midPos - 45));
    backLeftServo.write(limitAngle(midPos - 45));
    backRightServo.write(limitAngle(midPos + 45));
    walkingStep = true;
  }
  else {
    frontLeftServo.write(limitAngle(midPos - 45));
    frontRightServo.write(limitAngle(midPos + 45));
    backLeftServo.write(limitAngle(midPos + 45));
    backRightServo.write(limitAngle(midPos - 45));
    walkingStep = false;
  }
}

void backwardStep() {

  if (!walkingStep) {
    frontLeftServo.write(limitAngle(midPos - 45));
    frontRightServo.write(limitAngle(midPos + 45));
    backLeftServo.write(limitAngle(midPos + 45));
    backRightServo.write(limitAngle(midPos - 45));
    walkingStep = true;
  }
  else {
    frontLeftServo.write(limitAngle(midPos + 45));
    frontRightServo.write(limitAngle(midPos - 45));
    backLeftServo.write(limitAngle(midPos - 45));
    backRightServo.write(limitAngle(midPos + 45));
    walkingStep = false;
  }
}

void leftStep() {

  if (!walkingStep) {
    frontRightServo.write(limitAngle(midPos - 40));
    backRightServo.write(limitAngle(midPos - 40));
    frontLeftServo.write(midPos);
    backLeftServo.write(midPos);
    walkingStep = true;
  }
  else {
    frontRightServo.write(limitAngle(midPos + 40));
    backRightServo.write(limitAngle(midPos + 40));
    frontLeftServo.write(midPos);
    backLeftServo.write(midPos);
    walkingStep = false;
  }
}

void rightStep() {

  if (!walkingStep) {
    frontLeftServo.write(limitAngle(midPos + 40));
    backLeftServo.write(limitAngle(midPos + 40));
    frontRightServo.write(midPos);
    backRightServo.write(midPos);
    walkingStep = true;
  }
  else {

    frontLeftServo.write(limitAngle(midPos - 40));
    backLeftServo.write(limitAngle(midPos - 40));
    frontRightServo.write(midPos);
    backRightServo.write(midPos);
    walkingStep = false;
  }
}

void updateMotion() {

  if (currentMotion == MOTION_STOP) {
    return;
  }

  unsigned long currentTime = millis();

  if (currentTime - lastStepTime >= stepInterval) {
    lastStepTime = currentTime;
    if (currentMotion == MOTION_FORWARD) {
      forwardStep();
    }
    else if (currentMotion == MOTION_BACKWARD) {
      backwardStep();
    }
    else if (currentMotion == MOTION_LEFT) {
      leftStep();
    }
    else if (currentMotion == MOTION_RIGHT) {
      rightStep();
    }
  }
}

void wave() {

  stopRobot();

  digitalWrite(ledLeft, LOW);

  frontLeftServo.write(limitAngle(midPos - 50));
  delay(50);
  backLeftServo.write(limitAngle(midPos + 50));
  delay(250);
  frontLeftServo.write(limitAngle(midPos + 10));
  delay(50);
  backLeftServo.write(limitAngle(midPos - 10));
  delay(250);
  frontRightServo.write(limitAngle(midPos + 50));
  delay(50);
  backRightServo.write(limitAngle(midPos - 50));
  delay(250);
  frontRightServo.write(limitAngle(midPos - 10));
  delay(50);
  backRightServo.write(limitAngle(midPos + 10));
  delay(250);
  digitalWrite(ledLeft, HIGH);

  stopRobot();
}

void hi() {

  stopRobot();

  delay(100);

  backLeftServo.write(limitAngle(20));
  delay(100);
  backRightServo.write(limitAngle(160));
  delay(300);

  for (int i = 0; i < 3; i++) {
    frontLeftServo.write(limitAngle(10));
    delay(200);
    frontLeftServo.write(limitAngle(40));
    delay(200);
  }

  stopRobot();
}

void sit() {

  stopRobot();

  frontLeftServo.write(limitAngle(10));
  delay(100);
  frontRightServo.write(limitAngle(170));
  delay(100);
  backLeftServo.write(limitAngle(10));
  delay(100);
  backRightServo.write(limitAngle(170));
  delay(500);
}

void shakeTail() {

  stopRobot();

  for (int i = 0; i < 5; i++) {

    backLeftServo.write(limitAngle(midPos - 30));
    delay(50);
    backRightServo.write(limitAngle(midPos + 30));
    delay(100);
    backLeftServo.write(limitAngle(midPos + 30));
    delay(50);
    backRightServo.write(limitAngle(midPos - 30));
    delay(100);
  }

  stopRobot();
}

void handleJoystick() {

  if (!server.hasArg("x") || !server.hasArg("y")) {
    server.send(400, "text/plain", "Missing joystick data");
    return;
  }

  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();

  lastJoystickCommand = millis();

  const int deadZone = 25;

  if (abs(x) < deadZone && abs(y) < deadZone) {
    stopRobot();
    server.send(200, "text/plain", "STOP");
    return;
  }

  if (abs(y) > abs(x)) {
    if (y < 0) {
      currentMotion = MOTION_FORWARD;
    }
    else {
      currentMotion = MOTION_BACKWARD;
    }

  }
  else {
    if (x < 0) {
      currentMotion = MOTION_LEFT;
    }
    else {
      currentMotion = MOTION_RIGHT;
    }
  }

  server.send(200, "text/plain", "OK");
}

void handleRoot() {

  String html = R"rawliteral(

<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>Quadruped Robot</title>
<style>
* {
    box-sizing: border-box;
    -webkit-user-select: none;
    user-select: none;
}
body {
    margin: 0;
    background: #121212;
    color: white;
    font-family: Arial, sans-serif;
    text-align: center;
    overflow: hidden;
}
h1 {
    font-size: 25px;
    margin: 18px 0 5px 0;
}
.status {
    font-size: 16px;
    color: #aaaaaa;
    margin-bottom: 10px;
}
#joystick {
    width: 260px;
    height: 260px;
    background: #252525;
    border: 3px solid #444;
    border-radius: 50%;
    margin: 15px auto;
    position: relative;
    touch-action: none;
    box-shadow: inset 0 0 30px #111;
}
#stick {
    width: 95px;
    height: 95px;
    background: #3498db;
    border-radius: 50%;
    position: absolute;
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
    box-shadow: 0 5px 15px #000;
    touch-action: none;
}
.centerDot {
    width: 10px;
    height: 10px;
    background: #777;
    border-radius: 50%;
    position: absolute;
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
}
.actions {
    margin-top: 10px;
}
button {
    width: 85px;
    height: 55px;
    margin: 5px;
    border: none;
    border-radius: 12px;
    color: white;
    font-size: 14px;
    background: #9b59b6;
}
button:active {
    transform: scale(0.95);
}
.stop {
    background: #e74c3c;
}
</style>
</head>
<body>
<h1>Quadruped Robot</h1>
<div class="status" id="status">
STOPPED
</div>
<div id="joystick">
    <div class="centerDot"></div>
    <div id="stick"></div>
</div>
<div class="actions">
<button onclick="action('/C')">WAVE</button>
<button onclick="action('/X')">SHAK</button>
<button onclick="action('/H')">HI</button>
<button onclick="action('/D')">SIT</button>
</div>
<button class="stop" onclick="stopRobot()">STOP</button>
<script>
const joystick = document.getElementById("joystick");
const stick = document.getElementById("stick");
const statusText = document.getElementById("status");
let dragging = false;
let sendTimer = null;
let currentX = 0;
let currentY = 0;
const maxDistance = 82;
function sendJoystick() {
    if (!dragging) return;
    fetch("/joystick?x=" + currentX + "&y=" + currentY)
    .catch(() => {});
}
function startJoystick(e) {
    e.preventDefault();
    dragging = true;
    updateJoystick(e);
    if (sendTimer === null) {
        sendTimer = setInterval(
            sendJoystick,
            100
        );
    }
}
function updateJoystick(e) {
    if (!dragging) return;
    const rect = joystick.getBoundingClientRect();
    let clientX;
    let clientY;
    if (e.touches) {
        clientX = e.touches[0].clientX;
        clientY = e.touches[0].clientY;
    } else {
        clientX = e.clientX;
        clientY = e.clientY;
    }
    let x = clientX - (rect.left + rect.width / 2);
    let y = clientY - (rect.top + rect.height / 2);
    let distance = Math.sqrt(x*x + y*y);
    if (distance > maxDistance) {
        x = x / distance * maxDistance;
        y = y / distance * maxDistance;
    }
    stick.style.left = "calc(50% + " + x + "px)";
    stick.style.top = "calc(50% + " + y + "px)";
    currentX = Math.round(x / maxDistance * 100);
    currentY = Math.round(y / maxDistance * 100);
    updateStatus();
}
function updateStatus() {
    if (Math.abs(currentX) < 20 &&
        Math.abs(currentY) < 20) {
        statusText.innerHTML = "STOPPED";
    } else if (Math.abs(currentY) > Math.abs(currentX)) {
        if (currentY < 0) 
          statusText.innerHTML = "FORWARD";
        else
          statusText.innerHTML = "BACKWARD";
    } else {
        if (currentX < 0)
          statusText.innerHTML = "TURNING LEFT";
        else
          statusText.innerHTML = "TURNING RIGHT";
    }
}
function stopJoystick() {
    dragging = false;
    currentX = 0;
    currentY = 0;
    stick.style.left = "50%";
    stick.style.top = "50%";
    statusText.innerHTML = "STOPPED";
    if (sendTimer !== null) {
        clearInterval(sendTimer);
        sendTimer = null;
    }
    fetch("/joystick?x=0&y=0")
    .catch(() => {});
}
function stopRobot() {
    stopJoystick();
    fetch("/S")
    .catch(() => {});

}
function action(url) {
    stopJoystick();
    fetch(url)
    .catch(() => {});
}
joystick.addEventListener("mousedown", startJoystick);
document.addEventListener("mousemove", updateJoystick);
document.addEventListener("mouseup", stopJoystick);
joystick.addEventListener("touchstart", startJoystick, {passive:false});
document.addEventListener("touchmove", updateJoystick, {passive:false});
document.addEventListener("touchend", stopJoystick);
document.addEventListener("touchcancel", stopJoystick);
</script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void setup() {

  Serial.begin(115200);

  frontLeftServo.attach(frontLeftPin);
  delay(50);

  frontRightServo.attach(frontRightPin);
  delay(50);

  backLeftServo.attach(backLeftPin);
  delay(50);

  backRightServo.attach(backRightPin);
  delay(50);

  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  digitalWrite(ledLeft, HIGH);
  digitalWrite(ledRight, HIGH);

  stopRobot();

  delay(500);

  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("==============================");
  Serial.println("QUADRUPED ROBOT");
  Serial.println("==============================");

  Serial.print("WiFi Name: ");
  Serial.println(ssid);

  Serial.print("Password: ");
  Serial.println(password);

  Serial.print("Robot IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);

  server.on("/joystick", handleJoystick);

  server.on("/S", []() {

    stopRobot();

    server.send(
      200,
      "text/plain",
      "STOP"
    );

  });

  server.on("/C", []() {

    wave();

    server.send(
      200,
      "text/plain",
      "WAVE"
    );

  });

  server.on("/X", []() {

    shakeTail();

    server.send(
      200,
      "text/plain",
      "SHAKE"
    );

  });

  server.on("/H", []() {

    hi();

    server.send(
      200,
      "text/plain",
      "HI"
    );

  });

  server.on("/D", []() {

    sit();

    server.send(
      200,
      "text/plain",
      "SIT"
    );

  });

  server.begin();

  Serial.println("Web server started");
  Serial.println("Open http://192.168.4.1");
}

void loop() {

  server.handleClient();

  updateMotion();

  if (
    currentMotion != MOTION_STOP &&
    millis() - lastJoystickCommand >
    commandTimeout
  ) {

    stopRobot();

  }
}
