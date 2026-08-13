## MiniQ - The World’s Simplest Quadruped Robot

MiniQ is a beginner-friendly, Wi-Fi-controlled quadruped robot powered by the ESP8266 (Wemos D1 Mini). Designed to be the "world's simplest" robot dog, it utilizes a 1-DOF (Degree of Freedom) design per leg, eliminating the need for complex inverse kinematics or heavy mathematics.

The robot hosts its own web interface, allowing you to control it directly from a browser without installing any apps.

## Tutorial
[![Project Review](https://img.youtube.com/vi/zgDmtwAQpZ0/0.jpg)](https://youtu.be/zgDmtwAQpZ0)

## 🌟 Features

Simple Hardware: Uses only 4 servos and common, cheap components.

Wi-Fi Control: Controlled via a mobile-friendly web interface hosted on the ESP8266.

No App Required: Works on any device with a browser.

Easy Assembly: Fully 3D printed with a snap-in design for servos.


## 📦 Bill of Materials (BOM)

Microcontroller: Wemos D1 Mini (ESP8266) 

Actuators: 4x SG90 Micro Servos 

Power: 3.7V LiPo Battery

Charging: TP4056 Charging Module (with battery protection) 

Voltage Regulation: 5V Step-Up (Boost) Converter 

Capacitor: 1500µF (placed on servo power line to handle startup current) 

Switch: Slide switch 

## 🔌 Circuit & Wiring

<img src="SchematicPro.png" alt="Logo" width="550">

## 💻 Installation & Code

Edit Credentials: Update the following lines with your Wi-Fi details:

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";


## 🚀 Usage

After uploading, open the Serial Monitor in Arduino IDE (set baud rate to 115200).

Reset the board. Copy the IP Address displayed in the monitor.

Paste the IP address into your web browser (phone or PC).

You will see the control interface to drive the robot!.

## 🤝 Support & Links

Patreon (Support & Files): https://www.patreon.com/posts/miniq-quadruped-147329608

Cults3D (STL Files): https://cults3d.com/en/3d-model/game/world-simplest-quadruped-robot

If you like this project, please consider supporting me on Patreon or subscribing to the YouTube channel!.

Channel : https://www.youtube.com/@DsnIndustries/videos
