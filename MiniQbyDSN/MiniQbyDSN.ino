#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char* ssid = "swami";
const char* password = "swami@335";

ESP8266WebServer server(80);

Servo frontLeftServo;
Servo frontRightServo;
Servo backLeftServo;
Servo backRightServo;

const int ledLeft = D7;
const int ledRight = D8;

unsigned long lastCommandTime = 0;
const unsigned long idleTimeout = 180000;
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
  frontLeftServo.write(limitAngle(midPos + 30));
  delay(50);
  frontRightServo.write(limitAngle(midPos - 30));
  delay(50);
  backLeftServo.write(limitAngle(midPos - 30));
  delay(50);
  backRightServo.write(limitAngle(midPos + 30));
  delay(200);
  frontLeftServo.write(limitAngle(midPos - 30));
  delay(50);
  frontRightServo.write(limitAngle(midPos + 30));
  delay(50);
  backLeftServo.write(limitAngle(midPos + 30));
  delay(50);
  backRightServo.write(limitAngle(midPos - 30));
  delay(200);
}

void moveBackward() {
  frontLeftServo.write(limitAngle(midPos - 30));
  delay(50);
  frontRightServo.write(limitAngle(midPos + 30));
  delay(50);
  backLeftServo.write(limitAngle(midPos + 30));
  delay(50);
  backRightServo.write(limitAngle(midPos - 30));
  delay(200);
  frontLeftServo.write(limitAngle(midPos + 30));
  delay(50);
  frontRightServo.write(limitAngle(midPos - 30));
  delay(50);
  backLeftServo.write(limitAngle(midPos - 30));
  delay(50);
  backRightServo.write(limitAngle(midPos + 30));
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
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>body{text-align:center; font-family:sans-serif; background:#121212; color:white; margin:0; padding-top:10px;}";
  html += ".btn{width:80px; height:80px; margin:8px; font-size:24px; border-radius:15px; border:none; background:#3498db; color:white; cursor:pointer; touch-action: manipulation;}";
  html += ".btn:active{background:#2980b9;} .special{background:#e74c3c;} .action{background:#9b59b6;}</style></head><body>";
  html += "<h2>Quadruped Robot</h2>";

  html += "<div><button class='btn' onclick=\"fetch('/F')\">&#x2191;</button></div>";
  html += "<div><button class='btn' onclick=\"fetch('/L')\">&#x2190;</button>";
  html += "<button class='btn special' onclick=\"fetch('/S')\">&#x25A0;</button>";
  html += "<button class='btn' onclick=\"fetch('/R')\">&#x2192;</button></div>";
  html += "<div><button class='btn' onclick=\"fetch('/B')\">&#x2193;</button></div>";

  html += "<div style='margin-top:10px;'>";
  html += "<button class='btn action' onclick=\"fetch('/C')\" style='font-size:16px'>WAVE</button>";
  html += "<button class='btn action' onclick=\"fetch('/X')\" style='font-size:16px'>SHAKE</button></div>";
  html += "<div><button class='btn action' onclick=\"fetch('/H')\" style='font-size:16px'>HI</button>";
  html += "<button class='btn action' onclick=\"fetch('/D')\" style='font-size:16px'>SIT</button></div>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  frontLeftServo.attach(D1);
  delay(50);
  frontRightServo.attach(D2);
  delay(50);
  backLeftServo.attach(D5);
  delay(50);
  backRightServo.attach(D6);

  pinMode(ledLeft, OUTPUT);
  delay(50);
  pinMode(ledRight, OUTPUT);
  delay(50);
  digitalWrite(ledLeft, HIGH);
  delay(50);
  digitalWrite(ledRight, HIGH);

  stopRobot();
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);

  server.on("/", handleRoot);
  server.on("/F", []() {
    server.send(200);
    moveForward();
    lastCommandTime = millis();
  });
  server.on("/B", []() {
    server.send(200);
    moveBackward();
    lastCommandTime = millis();
  });
  server.on("/L", []() {
    server.send(200);
    turnLeft();
    lastCommandTime = millis();
  });
  server.on("/R", []() {
    server.send(200);
    turnRight();
    lastCommandTime = millis();
  });
  server.on("/S", []() {
    server.send(200);
    stopRobot();
    lastCommandTime = millis();
  });
  server.on("/C", []() {
    server.send(200);
    wave();
    lastCommandTime = millis();
  });
  server.on("/X", []() {
    server.send(200);
    shakeTail();
    lastCommandTime = millis();
  });
  server.on("/H", []() {
    server.send(200);
    hi();
    lastCommandTime = millis();
  });
  server.on("/D", []() {
    server.send(200);
    sit();
    lastCommandTime = millis();
  });

  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - lastCommandTime > idleTimeout) {
    shakeTail();
    lastCommandTime = millis();
  }
}