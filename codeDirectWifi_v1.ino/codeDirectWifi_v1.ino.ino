#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <EEPROM.h>

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

const int EEPROM_SIZE = 16;
const int EEPROM_MAGIC = 165;

int FL_CENTER = 90;
int FR_CENTER = 90;
int BL_CENTER = 90;
int BR_CENTER = 90;

int frontLeftAngle  = 90;
int frontRightAngle = 90;
int backLeftAngle   = 90;
int backRightAngle  = 90;

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

void loadCalibration() {

  if (EEPROM.read(0) != EEPROM_MAGIC) {

    FL_CENTER = 90;
    FR_CENTER = 90;
    BL_CENTER = 90;
    BR_CENTER = 90;

    return;
  }

  FL_CENTER = limitAngle(EEPROM.read(1));
  FR_CENTER = limitAngle(EEPROM.read(2));
  BL_CENTER = limitAngle(EEPROM.read(3));
  BR_CENTER = limitAngle(EEPROM.read(4));
}

void saveCalibration() {

  EEPROM.write(0, EEPROM_MAGIC);

  EEPROM.write(1, FL_CENTER);
  EEPROM.write(2, FR_CENTER);
  EEPROM.write(3, BL_CENTER);
  EEPROM.write(4, BR_CENTER);

  EEPROM.commit();
}

void stopRobot() {

  currentMotion = MOTION_STOP;
  walkingStep = false;

  frontLeftAngle = FL_CENTER;
  frontRightAngle = FR_CENTER;
  backLeftAngle = BL_CENTER;
  backRightAngle = BR_CENTER;

  frontLeftServo.write(FL_CENTER);
  frontRightServo.write(FR_CENTER);
  backLeftServo.write(BL_CENTER);
  backRightServo.write(BR_CENTER);

  digitalWrite(ledLeft, HIGH);
  digitalWrite(ledRight, HIGH);
}

void stopMotionOnly() {

  currentMotion = MOTION_STOP;
  walkingStep = false;

  digitalWrite(ledLeft, HIGH);
  digitalWrite(ledRight, HIGH);
}

void centerAll() {

  stopMotionOnly();

  frontLeftAngle = FL_CENTER;
  frontRightAngle = FR_CENTER;
  backLeftAngle = BL_CENTER;
  backRightAngle = BR_CENTER;

  frontLeftServo.write(FL_CENTER);
  frontRightServo.write(FR_CENTER);
  backLeftServo.write(BL_CENTER);
  backRightServo.write(BR_CENTER);
}

void setServoAngle(int servo, int angle) {

  angle = limitAngle(angle);

  stopMotionOnly();

  if (servo == 1) {

    frontLeftAngle = angle;
    frontLeftServo.write(angle);

  } else if (servo == 2) {

    frontRightAngle = angle;
    frontRightServo.write(angle);

  } else if (servo == 3) {

    backLeftAngle = angle;
    backLeftServo.write(angle);

  } else if (servo == 4) {

    backRightAngle = angle;
    backRightServo.write(angle);
  }
}

void saveCurrentAsCenter() {

  stopMotionOnly();

  FL_CENTER = frontLeftAngle;
  FR_CENTER = frontRightAngle;
  BL_CENTER = backLeftAngle;
  BR_CENTER = backRightAngle;

  saveCalibration();
}

void resetCalibration() {

  stopMotionOnly();

  FL_CENTER = 90;
  FR_CENTER = 90;
  BL_CENTER = 90;
  BR_CENTER = 90;

  saveCalibration();

  frontLeftAngle = 90;
  frontRightAngle = 90;
  backLeftAngle = 90;
  backRightAngle = 90;

  frontLeftServo.write(90);
  frontRightServo.write(90);
  backLeftServo.write(90);
  backRightServo.write(90);
}

void forwardStep() {

  int fl1 = limitAngle(FL_CENTER + 45);
  int fr1 = limitAngle(FR_CENTER - 45);
  int bl1 = limitAngle(BL_CENTER - 45);
  int br1 = limitAngle(BR_CENTER + 45);

  int fl2 = limitAngle(FL_CENTER - 45);
  int fr2 = limitAngle(FR_CENTER + 45);
  int bl2 = limitAngle(BL_CENTER + 45);
  int br2 = limitAngle(BR_CENTER - 45);

  if (!walkingStep) {

    frontLeftServo.write(fl1);
    frontRightServo.write(fr1);
    backLeftServo.write(bl1);
    backRightServo.write(br1);

    walkingStep = true;

  } else {

    frontLeftServo.write(fl2);
    frontRightServo.write(fr2);
    backLeftServo.write(bl2);
    backRightServo.write(br2);

    walkingStep = false;
  }
}
3
void backwardStep() {

  int fl1 = limitAngle(FL_CENTER - 45);
  int fr1 = limitAngle(FR_CENTER + 45);
  int bl1 = limitAngle(BL_CENTER + 45);
  int br1 = limitAngle(BR_CENTER - 45);

  int fl2 = limitAngle(FL_CENTER + 45);
  int fr2 = limitAngle(FR_CENTER - 45);
  int bl2 = limitAngle(BL_CENTER - 45);
  int br2 = limitAngle(BR_CENTER + 45);

  if (!walkingStep) {

    frontLeftServo.write(fl1);
    frontRightServo.write(fr1);
    backLeftServo.write(bl1);
    backRightServo.write(br1);

    walkingStep = true;

  } else {

    frontLeftServo.write(fl2);
    frontRightServo.write(fr2);
    backLeftServo.write(bl2);
    backRightServo.write(br2);

    walkingStep = false;
  }
}

void leftStep() {

  int fl1 = limitAngle(FL_CENTER + 40);
  int bl1 = limitAngle(BL_CENTER + 40);

  int fr1 = limitAngle(FR_CENTER - 40);
  int br1 = limitAngle(BR_CENTER - 40);

  int fl2 = limitAngle(FL_CENTER - 40);
  int bl2 = limitAngle(BL_CENTER - 40);

  int fr2 = limitAngle(FR_CENTER + 40);
  int br2 = limitAngle(BR_CENTER + 40);

  if (!walkingStep) {

    frontLeftServo.write(fl1);
    backLeftServo.write(bl1);

    frontRightServo.write(fr1);
    backRightServo.write(br1);

    walkingStep = true;

  } else {

    frontLeftServo.write(fl2);
    backLeftServo.write(bl2);

    frontRightServo.write(fr2);
    backRightServo.write(br2);

    walkingStep = false;
  }
}

void rightStep() {

  int fl1 = limitAngle(FL_CENTER - 40);
  int bl1 = limitAngle(BL_CENTER - 40);

  int fr1 = limitAngle(FR_CENTER + 40);
  int br1 = limitAngle(BR_CENTER + 40);

  int fl2 = limitAngle(FL_CENTER + 40);
  int bl2 = limitAngle(BL_CENTER + 40);

  int fr2 = limitAngle(FR_CENTER - 40);
  int br2 = limitAngle(BR_CENTER - 40);

  if (!walkingStep) {

    frontLeftServo.write(fl1);
    backLeftServo.write(bl1);

    frontRightServo.write(fr1);
    backRightServo.write(br1);

    walkingStep = true;

  } else {

    frontLeftServo.write(fl2);
    backLeftServo.write(bl2);

    frontRightServo.write(fr2);
    backRightServo.write(br2);

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

    } else if (currentMotion == MOTION_BACKWARD) {

      backwardStep();

    } else if (currentMotion == MOTION_LEFT) {

      leftStep();

    } else if (currentMotion == MOTION_RIGHT) {

      rightStep();
    }
  }
}

void wave() {

  stopMotionOnly();

  digitalWrite(ledLeft, LOW);

  frontLeftServo.write(
    limitAngle(FL_CENTER - 50)
  );

  delay(50);

  backLeftServo.write(
    limitAngle(BL_CENTER + 50)
  );

  delay(250);

  frontLeftServo.write(
    limitAngle(FL_CENTER + 10)
  );

  delay(50);

  backLeftServo.write(
    limitAngle(BL_CENTER - 10)
  );

  delay(250);

  frontRightServo.write(
    limitAngle(FR_CENTER + 50)
  );

  delay(50);

  backRightServo.write(
    limitAngle(BR_CENTER - 50)
  );

  delay(250);

  frontRightServo.write(
    limitAngle(FR_CENTER - 10)
  );

  delay(50);

  backRightServo.write(
    limitAngle(BR_CENTER + 10)
  );

  delay(250);

  digitalWrite(ledLeft, HIGH);

  centerAll();
}

void hi() {

  stopMotionOnly();

  delay(100);

  backLeftServo.write(
    limitAngle(BL_CENTER - 70)
  );

  delay(100);

  backRightServo.write(
    limitAngle(BR_CENTER + 70)
  );

  delay(300);

  for (int i = 0; i < 3; i++) {

    frontLeftServo.write(
      limitAngle(FL_CENTER - 80)
    );

    delay(200);

    frontLeftServo.write(
      limitAngle(FL_CENTER - 50)
    );

    delay(200);
  }

  centerAll();
}

void sit() {

  stopMotionOnly();

  frontLeftServo.write(
    limitAngle(FL_CENTER - 80)
  );

  delay(100);

  frontRightServo.write(
    limitAngle(FR_CENTER + 80)
  );

  delay(100);

  backLeftServo.write(
    limitAngle(BL_CENTER - 80)
  );

  delay(100);

  backRightServo.write(
    limitAngle(BR_CENTER + 80)
  );

  delay(500);
}

void shakeTail() {

  stopMotionOnly();

  for (int i = 0; i < 5; i++) {

    backLeftServo.write(
      limitAngle(BL_CENTER - 30)
    );

    delay(50);

    backRightServo.write(
      limitAngle(BR_CENTER + 30)
    );

    delay(100);

    backLeftServo.write(
      limitAngle(BL_CENTER + 30)
    );

    delay(50);

    backRightServo.write(
      limitAngle(BR_CENTER - 30)
    );

    delay(100);
  }

  centerAll();
}

void handleJoystick() {

  if (!server.hasArg("x") ||
      !server.hasArg("y")) {

    server.send(
      400,
      "text/plain",
      "Missing joystick data"
    );

    return;
  }

  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();

  lastJoystickCommand = millis();

  const int deadZone = 25;

  if (
    abs(x) < deadZone &&
    abs(y) < deadZone
  ) {

    stopRobot();

    server.send(
      200,
      "text/plain",
      "STOP"
    );

    return;
  }

  if (abs(y) > abs(x)) {

    if (y < 0) {

      currentMotion = MOTION_FORWARD;

    } else {

      currentMotion = MOTION_BACKWARD;
    }

  } else {

    if (x < 0) {

      currentMotion = MOTION_LEFT;

    } else {

      currentMotion = MOTION_RIGHT;
    }
  }

  server.send(
    200,
    "text/plain",
    "OK"
  );
}

void handleCalibration() {

  String json = "{";

  json += "\"fl\":";
  json += String(frontLeftAngle);

  json += ",\"fr\":";
  json += String(frontRightAngle);

  json += ",\"bl\":";
  json += String(backLeftAngle);

  json += ",\"br\":";
  json += String(backRightAngle);

  json += ",\"flCenter\":";
  json += String(FL_CENTER);

  json += ",\"frCenter\":";
  json += String(FR_CENTER);

  json += ",\"blCenter\":";
  json += String(BL_CENTER);

  json += ",\"brCenter\":";
  json += String(BR_CENTER);

  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}

void handleRoot() {

  String html = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta name="viewport"
content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">

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
    padding: 10px 15px 30px;
}

h1 {
    font-size: 27px;
    margin: 8px 0 4px;
}

.subtitle {
    color: #999;
    font-size: 14px;
    margin-bottom: 5px;
}

.status {
    font-size: 17px;
    color: #2ecc71;
    margin: 8px;
    font-weight: bold;
}

#joystick {
    width: 260px;
    height: 260px;
    background: #252525;
    border: 3px solid #444;
    border-radius: 50%;
    margin: 10px auto;
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

.stopButton {
    width: 130px;
    height: 50px;
    border: none;
    border-radius: 12px;
    background: #e74c3c;
    color: white;
    font-size: 17px;
    font-weight: bold;
    margin: 5px;
}

.actions {
    margin-top: 8px;
}

.actionButton {
    width: 90px;
    height: 50px;
    margin: 5px;
    border: none;
    border-radius: 12px;
    background: #9b59b6;
    color: white;
    font-size: 14px;
}

.calibration {
    max-width: 600px;
    margin: 25px auto 0;
    background: #1b1b1b;
    border-radius: 18px;
    padding: 18px;
}

.calibration h2 {
    margin: 5px 0;
    font-size: 22px;
}

.calibrationText {
    color: #888;
    font-size: 13px;
    margin-bottom: 18px;
}

.servoBox {
    background: #252525;
    border-radius: 14px;
    padding: 15px;
    margin: 10px 0;
    text-align: left;
}

.servoTitle {
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.servoName {
    font-size: 18px;
    font-weight: bold;
}

.angle {
    color: #3498db;
    font-size: 20px;
    font-weight: bold;
}

.adjustRow {
    display: flex;
    align-items: center;
    justify-content: space-between;
    margin-top: 14px;
}

.minusButton,
.plusButton {
    width: 80px;
    height: 58px;
    border: none;
    border-radius: 12px;
    color: white;
    font-size: 34px;
    font-weight: bold;
    touch-action: manipulation;
}

.minusButton {
    background: #e74c3c;
}

.plusButton {
    background: #27ae60;
}

.minusButton:active {
    transform: scale(0.94);
}

.plusButton:active {
    transform: scale(0.94);
}

.currentAngle {
    min-width: 110px;
    text-align: center;
    font-size: 26px;
    font-weight: bold;
    color: #3498db;
}

.calibrationButtons {
    margin-top: 15px;
}

.centerButton,
.saveButton,
.resetButton {
    width: 145px;
    height: 48px;
    border: none;
    border-radius: 12px;
    color: white;
    font-size: 14px;
    margin: 5px;
}

.centerButton {
    background: #3498db;
}

.saveButton {
    background: #27ae60;
}

.resetButton {
    background: #555;
}

</style>

</head>

<body>

<h1>🤖 Quadruped Robot</h1>

<div class="subtitle">
Joystick Control
</div>

<div class="status" id="status">
STOPPED
</div>

<div id="joystick">

<div class="centerDot"></div>

<div id="stick"></div>

</div>

<button class="stopButton"
onclick="stopRobot()">
STOP
</button>

<div class="actions">

<button class="actionButton"
onclick="action('/C')">
WAVE
</button>

<button class="actionButton"
onclick="action('/X')">
SHAKE
</button>

<button class="actionButton"
onclick="action('/H')">
HI
</button>

<button class="actionButton"
onclick="action('/D')">
SIT
</button>

</div>


<div class="calibration">

<h2>⚙ Servo Calibration</h2>

<div class="calibrationText">
Adjust each servo until all four legs touch the ground.
</div>


<div class="servoBox">

<div class="servoTitle">

<span class="servoName">
Front Left
</span>

<span class="angle"
id="flValue">
90°
</span>

</div>

<div class="adjustRow">

<button class="minusButton"
onclick="adjustServo(1,-1)">
−
</button>

<div class="currentAngle"
id="flAngle">
90°
</div>

<button class="plusButton"
onclick="adjustServo(1,1)">
+
</button>

</div>

</div>


<div class="servoBox">

<div class="servoTitle">

<span class="servoName">
Front Right
</span>

<span class="angle"
id="frValue">
90°
</span>

</div>

<div class="adjustRow">

<button class="minusButton"
onclick="adjustServo(2,-1)">
−
</button>

<div class="currentAngle"
id="frAngle">
90°
</div>

<button class="plusButton"
onclick="adjustServo(2,1)">
+
</button>

</div>

</div>


<div class="servoBox">

<div class="servoTitle">

<span class="servoName">
Back Left
</span>

<span class="angle"
id="blValue">
90°
</span>

</div>

<div class="adjustRow">

<button class="minusButton"
onclick="adjustServo(3,-1)">
−
</button>

<div class="currentAngle"
id="blAngle">
90°
</div>

<button class="plusButton"
onclick="adjustServo(3,1)">
+
</button>

</div>

</div>


<div class="servoBox">

<div class="servoTitle">

<span class="servoName">
Back Right
</span>

<span class="angle"
id="brValue">
90°
</span>

</div>

<div class="adjustRow">

<button class="minusButton"
onclick="adjustServo(4,-1)">
−
</button>

<div class="currentAngle"
id="brAngle">
90°
</div>

<button class="plusButton"
onclick="adjustServo(4,1)">
+
</button>

</div>

</div>


<div class="calibrationButtons">

<button class="centerButton"
onclick="centerAll()">
CENTER
</button>

<button class="saveButton"
onclick="saveCenter()">
SAVE AS CENTER
</button>

<button class="resetButton"
onclick="resetCalibration()">
RESET 90°
</button>

</div>

</div>


<script>

const joystick =
document.getElementById("joystick");

const stick =
document.getElementById("stick");

const statusText =
document.getElementById("status");

let dragging = false;

let sendTimer = null;

let currentX = 0;

let currentY = 0;

const maxDistance = 82;


let servoAngles = {
    1: 90,
    2: 90,
    3: 90,
    4: 90
};


function updateServoDisplay() {

    document.getElementById("flValue").innerHTML =
        servoAngles[1] + "°";

    document.getElementById("flAngle").innerHTML =
        servoAngles[1] + "°";

    document.getElementById("frValue").innerHTML =
        servoAngles[2] + "°";

    document.getElementById("frAngle").innerHTML =
        servoAngles[2] + "°";

    document.getElementById("blValue").innerHTML =
        servoAngles[3] + "°";

    document.getElementById("blAngle").innerHTML =
        servoAngles[3] + "°";

    document.getElementById("brValue").innerHTML =
        servoAngles[4] + "°";

    document.getElementById("brAngle").innerHTML =
        servoAngles[4] + "°";
}


function loadCalibration() {

    fetch("/calibration")
    .then(response => response.json())
    .then(data => {

        servoAngles[1] = data.fl;
        servoAngles[2] = data.fr;
        servoAngles[3] = data.bl;
        servoAngles[4] = data.br;

        updateServoDisplay();

    })
    .catch(() => {});
}


function sendJoystick() {

    if (!dragging) return;

    fetch(
        "/joystick?x=" +
        currentX +
        "&y=" +
        currentY
    ).catch(() => {});
}


function startJoystick(e) {

    e.preventDefault();

    dragging = true;

    updateJoystick(e);

    if (sendTimer === null) {

        sendTimer =
        setInterval(
            sendJoystick,
            100
        );
    }
}


function updateJoystick(e) {

    if (!dragging) return;

    e.preventDefault();

    const rect =
    joystick.getBoundingClientRect();

    let clientX;
    let clientY;

    if (e.touches) {

        clientX =
        e.touches[0].clientX;

        clientY =
        e.touches[0].clientY;

    } else {

        clientX =
        e.clientX;

        clientY =
        e.clientY;
    }

    let x =
    clientX -
    (rect.left + rect.width / 2);

    let y =
    clientY -
    (rect.top + rect.height / 2);

    let distance =
    Math.sqrt(x * x + y * y);

    if (distance > maxDistance) {

        x =
        x / distance *
        maxDistance;

        y =
        y / distance *
        maxDistance;
    }

    stick.style.left =
    "calc(50% + " +
    x +
    "px)";

    stick.style.top =
    "calc(50% + " +
    y +
    "px)";

    currentX =
    Math.round(
        x / maxDistance * 100
    );

    currentY =
    Math.round(
        y / maxDistance * 100
    );

    updateStatus();
}


function updateStatus() {

    if (
        Math.abs(currentX) < 20 &&
        Math.abs(currentY) < 20
    ) {

        statusText.innerHTML =
        "STOPPED";

    } else if (
        Math.abs(currentY) >
        Math.abs(currentX)
    ) {

        if (currentY < 0) {

            statusText.innerHTML =
            "FORWARD";

        } else {

            statusText.innerHTML =
            "BACKWARD";
        }

    } else {

        if (currentX < 0) {

            statusText.innerHTML =
            "TURNING LEFT";

        } else {

            statusText.innerHTML =
            "TURNING RIGHT";
        }
    }
}


function stopJoystick() {

    dragging = false;

    currentX = 0;
    currentY = 0;

    stick.style.left = "50%";
    stick.style.top = "50%";

    statusText.innerHTML =
    "STOPPED";

    if (sendTimer !== null) {

        clearInterval(sendTimer);

        sendTimer = null;
    }

    fetch(
        "/joystick?x=0&y=0"
    ).catch(() => {});
}


function stopMotionOnly() {

    dragging = false;

    currentX = 0;
    currentY = 0;

    stick.style.left = "50%";
    stick.style.top = "50%";

    statusText.innerHTML =
    "STOPPED";

    if (sendTimer !== null) {

        clearInterval(sendTimer);

        sendTimer = null;
    }
}


function stopRobot() {

    stopJoystick();

    fetch("/S").catch(() => {});
}


function action(url) {

    stopJoystick();

    fetch(url).catch(() => {});
}


function adjustServo(servo, change) {

    stopMotionOnly();

    let newAngle =
    servoAngles[servo] + change;

    if (newAngle < 10)
        newAngle = 10;

    if (newAngle > 170)
        newAngle = 170;

    servoAngles[servo] =
    newAngle;

    updateServoDisplay();

    fetch(
        "/servo?servo=" +
        servo +
        "&angle=" +
        newAngle
    ).catch(() => {});
}


function centerAll() {

    stopMotionOnly();

    fetch("/center")
    .then(() => {

        loadCalibration();

    })
    .catch(() => {});
}


function saveCenter() {

    stopMotionOnly();

    fetch("/savecenter")
    .then(() => {

        alert(
            "Current positions saved as new center!"
        );

        loadCalibration();

    })
    .catch(() => {});
}


function resetCalibration() {

    stopMotionOnly();

    fetch("/reset")
    .then(() => {

        loadCalibration();

    })
    .catch(() => {});
}


joystick.addEventListener(
    "mousedown",
    startJoystick
);

document.addEventListener(
    "mousemove",
    updateJoystick
);

document.addEventListener(
    "mouseup",
    stopJoystick
);


joystick.addEventListener(
    "touchstart",
    startJoystick,
    { passive: false }
);

document.addEventListener(
    "touchmove",
    updateJoystick,
    { passive: false }
);

document.addEventListener(
    "touchend",
    stopJoystick
);

document.addEventListener(
    "touchcancel",
    stopJoystick
);


loadCalibration();

</script>

</body>

</html>

)rawliteral";

  server.send(
    200,
    "text/html; charset=UTF-8",
    html
  );
}

void setup() {

  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);

  loadCalibration();

  frontLeftServo.attach(frontLeftPin);
  delay(100);

  frontRightServo.attach(frontRightPin);
  delay(100);

  backLeftServo.attach(backLeftPin);
  delay(100);

  backRightServo.attach(backRightPin);
  delay(100);

  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  digitalWrite(ledLeft, HIGH);
  digitalWrite(ledRight, HIGH);

  centerAll();

  delay(500);

  WiFi.mode(WIFI_AP);

  WiFi.softAP(
    ssid,
    password
  );

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

  Serial.println();

  Serial.print("Front Left Center: ");
  Serial.println(FL_CENTER);

  Serial.print("Front Right Center: ");
  Serial.println(FR_CENTER);

  Serial.print("Back Left Center: ");
  Serial.println(BL_CENTER);

  Serial.print("Back Right Center: ");
  Serial.println(BR_CENTER);

  Serial.println();


  server.on(
    "/",
    handleRoot
  );


  server.on(
    "/calibration",
    handleCalibration
  );


  server.on(
    "/joystick",
    handleJoystick
  );


  server.on(
    "/servo",
    []() {

      if (
        !server.hasArg("servo") ||
        !server.hasArg("angle")
      ) {

        server.send(
          400,
          "text/plain",
          "Missing parameters"
        );

        return;
      }

      int servo =
      server.arg("servo").toInt();

      int angle =
      server.arg("angle").toInt();

      if (
        servo < 1 ||
        servo > 4
      ) {

        server.send(
          400,
          "text/plain",
          "Invalid servo"
        );

        return;
      }

      setServoAngle(
        servo,
        angle
      );

      server.send(
        200,
        "text/plain",
        "SERVO UPDATED"
      );
    }
  );


  server.on(
    "/center",
    []() {

      centerAll();

      server.send(
        200,
        "text/plain",
        "CENTERED"
      );
    }
  );


  server.on(
    "/savecenter",
    []() {

      saveCurrentAsCenter();

      server.send(
        200,
        "text/plain",
        "CALIBRATION SAVED"
      );
    }
  );


  server.on(
    "/reset",
    []() {

      resetCalibration();

      server.send(
        200,
        "text/plain",
        "RESET TO 90"
      );
    }
  );


  server.on(
    "/S",
    []() {

      stopRobot();

      server.send(
        200,
        "text/plain",
        "STOP"
      );
    }
  );


  server.on(
    "/C",
    []() {

      wave();

      server.send(
        200,
        "text/plain",
        "WAVE"
      );
    }
  );


  server.on(
    "/X",
    []() {

      shakeTail();

      server.send(
        200,
        "text/plain",
        "SHAKE"
      );
    }
  );


  server.on(
    "/H",
    []() {

      hi();

      server.send(
        200,
        "text/plain",
        "HI"
      );
    }
  );


  server.on(
    "/D",
    []() {

      sit();

      server.send(
        200,
        "text/plain",
        "SIT"
      );
    }
  );


  server.begin();

  Serial.println(
    "Web server started"
  );

  Serial.println(
    "Open http://192.168.4.1"
  );
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