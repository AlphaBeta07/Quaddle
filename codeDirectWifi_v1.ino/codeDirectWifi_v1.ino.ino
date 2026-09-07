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

int limitAngle(int angle) {
  if (angle < 10) return 10;
  if (angle > 170) return 170;
  return angle;
}

void stopRobot() {
  frontLeftServo.write(midPos);
  delay(60);
  frontRightServo.write(midPos);
  delay(60);
  backLeftServo.write(midPos);
  delay(60);
  backRightServo.write(midPos);
  delay(60);
}

void moveForward() {
  frontLeftServo.write(limitAngle(midPos + 45));
  delay(50);
  frontRightServo.write(limitAngle(midPos - 45));
  delay(50);
  backLeftServo.write(limitAngle(midPos - 45));
  delay(50);
  backRightServo.write(limitAngle(midPos + 45));
  delay(200);

  frontLeftServo.write(limitAngle(midPos - 45));
  delay(50);
  frontRightServo.write(limitAngle(midPos + 45));
  delay(50);
  backLeftServo.write(limitAngle(midPos + 45));
  delay(50);
  backRightServo.write(limitAngle(midPos - 45));
  delay(200);
}

void moveBackward() {
  frontLeftServo.write(limitAngle(midPos - 45));
  delay(50);
  frontRightServo.write(limitAngle(midPos + 45));
  delay(50);
  backLeftServo.write(limitAngle(midPos + 45));
  delay(50);
  backRightServo.write(limitAngle(midPos - 45));
  delay(200);

  frontLeftServo.write(limitAngle(midPos + 45));
  delay(50);
  frontRightServo.write(limitAngle(midPos - 45));
  delay(50);
  backLeftServo.write(limitAngle(midPos - 45));
  delay(50);
  backRightServo.write(limitAngle(midPos + 45));
  delay(200);
}

void turnRight() {
  stopRobot();

  frontLeftServo.write(limitAngle(midPos + 40));
  delay(60);
  backLeftServo.write(limitAngle(midPos + 40));
  delay(60);
  frontLeftServo.write(limitAngle(midPos - 40));
  delay(60);
  backLeftServo.write(limitAngle(midPos - 40));
  delay(60);
}

void turnLeft() {
  stopRobot();

  frontRightServo.write(limitAngle(midPos - 40));
  delay(60);
  backRightServo.write(limitAngle(midPos - 40));
  delay(60);
  frontRightServo.write(limitAngle(midPos + 40));
  delay(60);
  backRightServo.write(limitAngle(midPos + 40));
  delay(60);
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

void handleRoot() {
  String html = "<html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";

  html += "<style>";

  html += "body{";
  html += "text-align:center;";
  html += "font-family:sans-serif;";
  html += "background:#121212;";
  html += "color:white;";
  html += "margin:0;";
  html += "padding-top:10px;";
  html += "}";
  html += ".btn{";
  html += "width:80px;";
  html += "height:80px;";
  html += "margin:8px;";
  html += "font-size:24px;";
  html += "border-radius:15px;";
  html += "border:none;";
  html += "background:#3498db;";
  html += "color:white;";
  html += "cursor:pointer;";
  html += "touch-action:manipulation;";
  html += "}";
  html += ".btn:active{";
  html += "background:#2980b9;";
  html += "}";
  html += ".special{";
  html += "background:#e74c3c;";
  html += "}";
  html += ".action{";
  html += "background:#9b59b6;";
  html += "}";
  html += "</style>";
  html += "</head><body>";
  html += "<h2>Quadruped Robot</h2>";
  html += "<div>";
  html += "<button class='btn' onclick=\"fetch('/F')\">";
  html += "&#x2191;";
  html += "</button>";
  html += "</div>";
  html += "<div>";
  html += "<button class='btn' onclick=\"fetch('/L')\">";
  html += "&#x2190;";
  html += "</button>";
  html += "<button class='btn special' onclick=\"fetch('/S')\">";
  html += "&#x25A0;";
  html += "</button>";
  html += "<button class='btn' onclick=\"fetch('/R')\">";
  html += "&#x2192;";
  html += "</button>";
  html += "</div>";
  html += "<div>";
  html += "<button class='btn' onclick=\"fetch('/B')\">";
  html += "&#x2193;";
  html += "</button>";
  html += "</div>";
  html += "<div style='margin-top:10px;'>";
  html += "<button class='btn action' ";
  html += "onclick=\"fetch('/C')\" ";
  html += "style='font-size:16px'>";
  html += "WAVE";
  html += "</button>";
  html += "<button class='btn action' ";
  html += "onclick=\"fetch('/X')\" ";
  html += "style='font-size:16px'>";
  html += "SHAKE";
  html += "</button>";
  html += "</div>";
  html += "<div>";
  html += "<button class='btn action' ";
  html += "onclick=\"fetch('/H')\" ";
  html += "style='font-size:16px'>";
  html += "HI";
  html += "</button>";
  html += "<button class='btn action' ";
  html += "onclick=\"fetch('/D')\" ";
  html += "style='font-size:16px'>";
  html += "SIT";
  html += "</button>";
  html += "</div>";
  html += "</body></html>";
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
  delay(50);
  pinMode(ledRight, OUTPUT);
  delay(50);
  digitalWrite(ledLeft, HIGH);
  delay(50);
  digitalWrite(ledRight, HIGH);

  stopRobot();

  delay(1000);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("Quadruped Robot WiFi Started");
  Serial.print("WiFi Name: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("Robot IP Address: ");
  Serial.println(WiFi.softAPIP());

  delay(1000);

  server.on("/", handleRoot);
  server.on("/F", []() {
    server.send(200, "text/plain", "FORWARD");
    moveForward();
  });
  server.on("/B", []() {
    server.send(200, "text/plain", "BACKWARD");
    moveBackward();
  });
  server.on("/L", []() {
    server.send(200, "text/plain", "LEFT");
    turnLeft();
  });
  server.on("/R", []() {
    server.send(200, "text/plain", "RIGHT");
    turnRight();
  });
  server.on("/S", []() {
    server.send(200, "text/plain", "STOP");
    stopRobot();
  });
  server.on("/C", []() {
    server.send(200, "text/plain", "WAVE");
    wave();
  });
  server.on("/X", []() {
    server.send(200, "text/plain", "SHAKE");
    shakeTail();
  });
  server.on("/H", []() {
    server.send(200, "text/plain", "HI");
    hi();
  });
  server.on("/D", []() {
    server.send(200, "text/plain", "SIT");
    sit();
  });

  server.begin();
  Serial.println("Web server started");
  Serial.println("Open http://192.168.4.1");
}

void loop() {
  server.handleClient();
}