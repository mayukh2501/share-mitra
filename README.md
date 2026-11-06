# SHRAM MITRA
**GROUP 20 
AMRITANSHU ADITYA RA2411003010239
MAYUKH MONDAL RA2411003010239**

### Smart Autonomous Assistance Robot

**An integrated robotics and IoT system for autonomous navigation, smart identification, environmental sensing and real-time monitoring.**


## 1. Introduction

SHRAM MITRA is a four-wheel autonomous assistance robot developed to bring together robotics, embedded systems, sensor technology and the Internet of Things (IoT) in a single platform.

The project focuses on developing a mobile robotic system capable of navigating a predefined path, identifying individuals through RFID cards, initiating attendance recording and monitoring its surrounding environment.

Built around an ESP32-based controller, the robot integrates multiple sensors and hardware modules to perform different tasks. A five-channel infrared sensor array is used for line-following navigation, while an RFID reader handles user identification. An ESP32-CAM is intended to capture photographs during attendance verification, with cloud integration planned through Google Sheets and Google Drive.

Additional modules, including an ultrasonic sensor, PIR motion sensor and DHT11, extend the robot's sensing capabilities. An OLED display provides information about the robot's current operations, while a buzzer is intended to provide audio feedback.

The aim is to create a modular robotic platform where navigation, identification, sensing and data management work together as part of one system.
--

## 2. Project Objectives

The main objectives of SHRAM MITRA are:

- Develop a four-wheel autonomous mobile robot.
- Implement line-following navigation using infrared sensors.
- Establish RFID-based identification and attendance management.
- Integrate camera-based photograph capture.
- Enable wireless communication and cloud-based data management.
- Detect obstacles using ultrasonic sensing.
- Identify nearby movement using a PIR sensor.
- Monitor environmental temperature and humidity.
- Provide visual feedback through an OLED display.
- Develop a centralized dashboard for monitoring robot activities.

---

## 3. System Architecture

The robot is divided into functional subsystems, each responsible for a specific operation. The ESP32 acts as the central controller, processing sensor inputs and coordinating the connected hardware.

```text
                     SHRAM MITRA
                          |
                          |
                   ESP32 CONTROLLER
                          |
          +---------------+---------------+
          |               |               |
          |               |               |
      NAVIGATION     IDENTIFICATION    MONITORING
          |               |               |
     5-Channel IR      RC522 RFID        DHT11
       Sensors         ESP32-CAM         PIR
          |               |             HC-SR04
          |               |
          v               v
     L298N DRIVER     IMAGE CAPTURE
          |               |
          v               v
     FOUR DC MOTORS   CLOUD CONNECTIVITY
                          |
                 +--------+--------+
                 |                 |
            GOOGLE SHEETS     GOOGLE DRIVE
                 |                 |
                 +--------+--------+
                          |
                   LIVE DASHBOARD

                 USER INTERFACE
                 OLED DISPLAY
                 BUZZER
```

### Functional Subsystems

| Subsystem | Responsibility |
|---|---|
| Processing | Central control and sensor data processing |
| Navigation | Line detection and movement control |
| Mobility | Four-wheel motor operation |
| Identification | RFID-based user identification |
| Visual Verification | Photograph capture |
| Obstacle Detection | Distance measurement |
| Motion Detection | Detection of movement |
| Environmental Monitoring | Temperature and humidity measurement |
| User Interface | Display and audio feedback |
| IoT Integration | Wireless communication and cloud data management |

---

## 4. Hardware Components

The following components form the hardware foundation of SHRAM MITRA.

| Component | Application |
|---|---|
| Boodskap ESP32 IoT Board | Main processing and control unit |
| 4WD Robot Chassis | Mechanical structure |
| Four DC Geared Motors | Robot movement |
| L298N Motor Driver | Motor direction and speed control |
| Five-Channel IR Sensor Array | Line-following navigation |
| RC522 RFID Reader | RFID identification |
| RFID Cards/Tags | User identification |
| ESP32-CAM | Photograph capture |
| HC-SR04 Ultrasonic Sensor | Obstacle detection |
| PIR Motion Sensor | Motion detection |
| DHT11 Sensor | Temperature and humidity monitoring |
| OLED Display | System information and instructions |
| Buzzer | Audible notifications |
| Battery and Power Supply | Power distribution |

---

## 5. Sensor and Module Implementation

### 5.1 Five-Channel IR Sensor Array

The five-channel infrared sensor array is responsible for detecting the navigation track.

The sensors detect differences in reflected infrared light between the black tape and the surrounding surface. Their readings are processed by the ESP32 to determine the robot's position relative to the track.

The navigation logic uses these readings to adjust motor movement and keep the robot aligned with the predefined path.

**Main functions:**
- Black-line detection.
- Track alignment.
- Direction correction.
- Navigation checkpoint detection.

### 5.2 RFID Reader – RC522

The RC522 RFID module provides the identification mechanism for the attendance subsystem.

It communicates with the ESP32 through the SPI interface. When an RFID card is presented, the reader retrieves its unique identifier (UID), which can be checked against registered users.

**Main functions:**
- RFID card detection.
- UID reading.
- User identification.
- Attendance event initiation.

### 5.3 Ultrasonic Sensor – HC-SR04

The HC-SR04 measures the distance between the robot and nearby objects using ultrasonic sound waves.

It contains two transducers: one transmits the ultrasonic pulse, while the other receives its reflection.

Distance is calculated using:

```text
Distance = (Time × Speed of Sound) / 2
```

The sensor provides distance information that can be used to detect obstacles and initiate appropriate stopping or movement decisions.

### 5.4 PIR Motion Sensor

The Passive Infrared (PIR) sensor detects movement by identifying changes in infrared radiation within its detection area.

It adds another layer of environmental awareness to the robot.

**Main functions:**
- Movement detection.
- Motion-triggered events.
- Human-presence-related automation.

### 5.5 DHT11 Temperature and Humidity Sensor

The DHT11 collects basic environmental information.

It measures:
- Ambient temperature.
- Relative humidity.

These readings can be used for environmental monitoring and displayed through the monitoring interface.

### 5.6 ESP32-CAM

The ESP32-CAM provides image-capturing functionality for the attendance subsystem.

It is intended to work alongside the RFID reader. When an attendance event is initiated, the camera captures a photograph that can be uploaded through Wi-Fi.

The photograph URL can then be associated with the corresponding attendance record.

### 5.7 OLED Display

The OLED display acts as the robot's local information interface.

It can communicate system status and instructions, including:

- SHRAM MITRA welcome screen.
- RFID scanning instructions.
- Attendance verification status.
- Navigation information.
- Sensor readings.

### 5.8 Buzzer

The buzzer is intended to provide audio notifications during important system events.

Possible notifications include RFID detection, successful identification and error alerts.

---

## 6. Mobility and Motor Control

SHRAM MITRA uses a four-wheel-drive configuration with four DC geared motors.

An L298N dual H-bridge motor driver is used to control the motors. The ESP32 sends the required direction and control signals to the driver.

### Mechanical Layout

```text
                 FRONT
          ___________________
         |                   |
         |   M1         M2   |
         |    O         O    |
         |                   |
         |                   |
         |    O         O    |
         |   M3         M4   |
         |___________________|

                  REAR
```

### Movement Control

| Movement | Motor Operation |
|---|---|
| Forward | All four wheels rotate in the forward direction |
| Reverse | All four wheels rotate in the reverse direction |
| Left | Differential motor control |
| Right | Differential motor control |
| Stop | Motor outputs are disabled |

During development, motor polarity and wheel orientation required calibration to ensure that all four wheels produced the same effective direction of movement.

The final GPIO configuration should be documented using the verified circuit connections.

---

## 7. Operating Workflow

The intended operating sequence combines autonomous navigation with RFID-based attendance.

```text
                  SYSTEM START
                       |
                       v
               INITIALIZE ESP32
                       |
                       v
              INITIALIZE SENSORS
                       |
                       v
               BEGIN NAVIGATION
                       |
                       v
              FOLLOW BLACK TRACK
                       |
                       v
             DETECT RFID CHECKPOINT
                       |
                       v
                 STOP ROBOT
                       |
                       v
                SCAN RFID CARD
                       |
                       v
                READ CARD UID
                       |
                       v
           INITIATE ATTENDANCE EVENT
                       |
                       v
              CAPTURE PHOTOGRAPH
                       |
                       v
             PROCESS ATTENDANCE DATA
                       |
                       v
              CLOUD DATA UPLOAD
                       |
                       v
              UPDATE DASHBOARD
                       |
                       v
              RESUME NAVIGATION
                       |
                       v
                CONTINUE CYCLE
```

The robot is designed to operate continuously through this sequence, subject to successful sensor readings and attendance verification.

---

## 8. RFID-Based Attendance Management

One of the primary applications of SHRAM MITRA is automated attendance recording.

The RFID reader identifies a user through their registered card UID. Once an attendance event is initiated, the camera subsystem is intended to capture a photograph for additional visual documentation.

### Proposed Attendance Process

1. The robot reaches the designated attendance checkpoint.
2. The robot stops and waits for an RFID scan.
3. The RC522 reads the RFID card UID.
4. The system initiates user identification.
5. The ESP32-CAM captures a photograph.
6. Attendance information is prepared for cloud storage.
7. The photograph is uploaded to Google Drive.
8. The attendance record is updated in Google Sheets.
9. The dashboard displays the available attendance information.
10. The robot resumes its navigation.

### Proposed Attendance Data Structure

| Field | Description |
|---|---|
| User ID | Registered user identifier |
| RFID UID | Unique card identifier |
| Date | Attendance date |
| Time | Attendance timestamp |
| Status | Attendance or verification status |
| Image URL | Photograph reference |

---

## 9. IoT and Cloud Integration

Wireless connectivity is intended to extend the robot's functionality beyond its onboard hardware.

### Google Sheets

Google Sheets is planned as the attendance data management platform.

It can maintain user identification, attendance timestamps, verification status and photograph references.

### Google Drive

Google Drive is intended to store photographs captured by the ESP32-CAM.

The corresponding image links can be associated with attendance records.

### Monitoring Dashboard

A centralized dashboard is planned to make the collected information accessible through a convenient interface.

Proposed dashboard features include:

- Attendance records.
- User identification details.
- Date and time information.
- Photograph references.
- Attendance statistics.
- Environmental sensor readings.
- Robot operating status.

---

## 10. Software and Technologies

| Technology | Purpose |
|---|---|
| Arduino IDE | Firmware development |
| Embedded C/C++ | Robot programming |
| ESP32 Platform | Hardware control |
| SPI | RFID communication |
| I2C | OLED communication |
| Wi-Fi | Wireless connectivity |
| Google Sheets | Attendance data management |
| Google Drive | Photograph storage |
| Web Technologies | Dashboard development |

### Required Libraries

Depending on the modules being used, the project requires:

- MFRC522
- SPI
- Wire
- Adafruit GFX
- Adafruit SSD1306
- DHT Sensor Library
- ESP32 Camera Support

---

## 11. Repository Structure

The repository is organized into separate modules to make development, testing and maintenance easier.

```text
SHRAM-MITRA/
|
|-- README.md
|
|-- code/
|   |
|   |-- main_robot/
|   |   |-- main_robot.ino
|   |
|   |-- motor_control/
|   |   |-- motor_test.ino
|   |
|   |-- line_following/
|   |   |-- ir_sensor_test.ino
|   |
|   |-- rfid/
|   |   |-- rfid_test.ino
|   |
|   |-- ultrasonic/
|   |   |-- ultrasonic_test.ino
|   |
|   |-- pir/
|   |   |-- pir_test.ino
|   |
|   |-- environmental_monitoring/
|       |-- dht11_test.ino
|
|-- camera/
|   |-- esp32_cam.ino
|
|-- dashboard/
|   |-- README.md
|
|-- circuit_diagrams/
|   |-- pin_connections.png
|   |-- circuit_diagram.png
|
|-- documentation/
|   |-- architecture.md
|   |-- working_principle.md
|
|-- images/
|   |-- prototype_photos/
|
|-- LICENSE
```

---

## 12. Installation and Setup

### Prerequisites

- Arduino IDE.
- ESP32 board package.
- Boodskap ESP32 development board.
- Required hardware modules.
- USB data cable.
- Suitable power supply.
- Wi-Fi connectivity for cloud features.

### Setup Instructions

**Step 1 – Install Arduino IDE**

Download and install Arduino IDE:

https://www.arduino.cc/en/software

**Step 2 – Configure the ESP32 Board**

Install the ESP32 board package through the Arduino IDE Boards Manager and select the appropriate development board.

**Step 3 – Install Dependencies**

Install the required libraries for RFID, OLED, DHT11 and other connected modules.

**Step 4 – Connect Hardware**

Connect the motors, motor driver, sensors, display and other modules according to the verified circuit diagram.

**Step 5 – Configure GPIO Pins**

Set the GPIO assignments according to the actual hardware connections.

**Step 6 – Upload Firmware**

Open the main robot program and upload it to the ESP32.

**Step 7 – Perform Individual Tests**

Test each subsystem independently before integrating the complete robot.

**Step 8 – Run System Integration**

Test autonomous movement, checkpoint detection, RFID identification and the intended cloud workflow together.

---

## 13. Testing and Validation

Each module is tested independently to identify hardware or software issues before complete system integration.

| Module | Testing Objective |
|---|---|
| DC Motors | Verify direction and movement of all four wheels |
| L298N | Verify motor driver operation |
| IR Array | Verify line detection |
| RC522 | Verify RFID initialization and card reading |
| Ultrasonic | Verify distance measurement |
| PIR | Verify motion detection |
| DHT11 | Verify environmental readings |
| OLED | Verify display communication |
| Buzzer | Verify audio output |
| ESP32-CAM | Verify photograph capture |
| Cloud Integration | Verify data transfer and storage |

Final validation should include navigation reliability, checkpoint accuracy, attendance workflow completion and stable operation of all integrated modules.

---

## 14. Development Status

The project is being developed and tested in individual stages.

| Feature | Status |
|---|---|
| ESP32 controller setup | In use |
| Four-wheel motor control | Under calibration |
| L298N integration | Under testing |
| IR line-following subsystem | Requires integrated validation |
| RFID reader | Under troubleshooting |
| Buzzer | Under troubleshooting |
| Ultrasonic sensing | Hardware identified |
| PIR motion sensing | Hardware identified |
| DHT11 monitoring | Hardware identified |
| OLED interface | Display design prepared |
| ESP32-CAM integration | Planned integration |
| Google Sheets integration | Planned integration |
| Google Drive integration | Planned integration |
| Live dashboard | Planned integration |

---

## 15. Future Improvements

SHRAM MITRA is designed with a modular architecture, making it possible to extend its capabilities over time.

Potential improvements include:

- AI-based facial recognition.
- More advanced autonomous navigation.
- Intelligent obstacle avoidance.
- Voice-controlled interaction.
- GPS-based location tracking.
- Mobile application integration.
- Advanced cloud analytics.
- Automated route optimization.
- Servo-operated robotic mechanisms.
- Improved battery management.
- More advanced user authentication.

---

## 16. Project Information

| Attribute | Details |
|---|---|
| Project Name | SHRAM MITRA |
| Full Name | Smart Autonomous Assistance Robot |
| Project Category | Robotics and IoT |
| Controller | Boodskap ESP32 |
| Programming Language | Embedded C/C++ |
| Navigation | Infrared Sensor Array |
| Identification | RFID |
| Communication | Wi-Fi |
| Motor Control | L298N |
| Development Platform | Arduino IDE |

---

## Closing Note

SHRAM MITRA is an exploration of how robotics, embedded systems and connected technologies can work together to solve practical problems.

From controlling four independent motors to integrating RFID identification and environmental sensors, each subsystem contributes to the development of a larger intelligent platform.

The project is being developed through hardware experimentation, individual module testing and progressive system integration.

**SHRAM MITRA — Engineering Intelligence into Motion.**
