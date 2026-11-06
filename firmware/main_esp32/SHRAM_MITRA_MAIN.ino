/*
  SHRAM MITRA — Main ESP32 firmware baseline

  Board: Boodskap IoT HV1 (ESP32-based; select the exact board profile)
  Functions:
    - 5-channel IR line following
    - Stop at a broad transverse checkpoint marker
    - Wait for RC522 RFID scan
    - Ask ESP32-CAM to capture/upload an attendance image
    - DHT11 / PIR / ultrasonic / OLED status hooks

  Libraries:
    - MFRC522 by GithubCommunity
    - Adafruit GFX Library
    - Adafruit SSD1306
    - DHT sensor library by Adafruit

  IMPORTANT:
    Read ../../docs/HARDWARE_AND_PINOUT.md before connecting power.
    Some pin assignments below are proposed, not verified.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------- Wi-Fi ----------
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ESP32-CAM address printed in its Serial Monitor.
// Example only; replace with the actual address.
const char* CAMERA_CAPTURE_URL = "http://192.168.1.150/capture";

// ---------- Motor driver GPIOs (provided by user) ----------
#define MOTOR_IN1 32
#define MOTOR_IN2 33
#define MOTOR_IN3 26
#define MOTOR_IN4 14

// Conventional H-bridge direction mapping.
// CALIBRATE actual wheel grouping and polarity before floor operation.
const bool REVERSE_LEFT_MOTOR = false;
const bool REVERSE_RIGHT_MOTOR = false;

// ---------- Five-channel IR array (provided by user) ----------
const uint8_t IR_PINS[5] = {34, 35, 36, 39, 13};
// Set to LOW if the sensor output is LOW over black tape.
// Verify this with Serial Monitor before autonomous operation.
#define IR_BLACK_LEVEL LOW

// ---------- RC522 (proposed; wire to match) ----------
#define RFID_SS_PIN 5
#define RFID_RST_PIN 4
#define RFID_SCK_PIN 18
#define RFID_MISO_PIN 19
#define RFID_MOSI_PIN 23
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

// ---------- OLED (proposed I2C wiring) ----------
#define OLED_SDA 21
#define OLED_SCL 22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oledReady = false;

// ---------- DHT11 / PIR / ultrasonic (proposed wiring) ----------
#define DHT_PIN 27
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

#define PIR_PIN 25
#define TRIG_PIN 16
#define ECHO_PIN 17

// Buzzer GPIO is not confirmed. Keep disabled until identified.
#define BUZZER_PIN -1

// ---------- Tuning ----------
const int BASE_SPEED_UNUSED = 0; // No PWM is configured in this baseline.
const unsigned long CHECKPOINT_CONFIRM_MS = 250;
const unsigned long RFID_RETRY_DELAY_MS = 100;
const float OBSTACLE_STOP_CM = 18.0;

// ---------- State ----------
enum RobotState { FOLLOW_LINE, WAIT_FOR_RFID, OBSTACLE_STOP, CAMERA_REQUEST };
RobotState state = FOLLOW_LINE;
unsigned long checkpointSince = 0;
String lastProcessedUID = "";
unsigned long lastScanMs = 0;

// ---------- Motor helpers ----------
void setMotorPair(uint8_t pinA, uint8_t pinB, bool forward, bool reversePolarity) {
  bool direction = forward ^ reversePolarity;
  digitalWrite(pinA, direction ? HIGH : LOW);
  digitalWrite(pinB, direction ? LOW : HIGH);
}

void stopMotors() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
}

void driveForward() {
  setMotorPair(MOTOR_IN1, MOTOR_IN2, true, REVERSE_LEFT_MOTOR);
  setMotorPair(MOTOR_IN3, MOTOR_IN4, true, REVERSE_RIGHT_MOTOR);
}

void turnLeft() {
  // Differential steering: left side stop/reverse, right side forward.
  setMotorPair(MOTOR_IN1, MOTOR_IN2, false, REVERSE_LEFT_MOTOR);
  setMotorPair(MOTOR_IN3, MOTOR_IN4, true, REVERSE_RIGHT_MOTOR);
}

void turnRight() {
  setMotorPair(MOTOR_IN1, MOTOR_IN2, true, REVERSE_LEFT_MOTOR);
  setMotorPair(MOTOR_IN3, MOTOR_IN4, false, REVERSE_RIGHT_MOTOR);
}

// ---------- Sensors ----------
bool isBlack(uint8_t pin) {
  return digitalRead(pin) == IR_BLACK_LEVEL;
}

int readLinePosition() {
  // Weighted position: -2 (far left) to +2 (far right).
  const int weights[5] = {-2, -1, 0, 1, 2};
  int total = 0;
  int count = 0;
  for (int i = 0; i < 5; i++) {
    if (isBlack(IR_PINS[i])) {
      total += weights[i];
      count++;
    }
  }
  if (count == 0) return 999; // no line detected
  return total / count;
}

bool checkpointPattern() {
  // Basic wide-bar heuristic: all five sensors see black.
  // Adjust after observing actual sensor pattern at the checkpoint.
  for (int i = 0; i < 5; i++) {
    if (!isBlack(IR_PINS[i])) return false;
  }
  return true;
}

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return -1.0;
  return duration * 0.0343 / 2.0;
}

void updateDisplay(const String& line1, const String& line2 = "") {
  if (!oledReady) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("SHRAM MITRA");
  display.println("----------------");
  display.println(line1);
  display.println(line2);
  display.display();
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection timed out; RFID and line logic can still be tested.");
  }
}

bool requestCameraCapture(const String& uid) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Camera request skipped: Wi-Fi not connected.");
    return false;
  }

  HTTPClient http;
  String url = String(CAMERA_CAPTURE_URL) + "?uid=" + uid;
  http.setTimeout(30000);
  if (!http.begin(url)) {
    Serial.println("Could not start HTTP request to camera.");
    return false;
  }

  int code = http.GET();
  String response = http.getString();
  http.end();

  Serial.printf("Camera HTTP status: %d\n", code);
  Serial.println(response);
  return code == 200;
}

String readCardUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

void handleRFID() {
  if (millis() - lastScanMs < RFID_RETRY_DELAY_MS) return;
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  String uid = readCardUID();
  lastScanMs = millis();
  Serial.print("RFID UID: ");
  Serial.println(uid);

  if (uid == lastProcessedUID) {
    Serial.println("Same card already processed; remove and scan again for a new event.");
  } else {
    state = CAMERA_REQUEST;
    updateDisplay("RFID accepted", uid);
    bool uploaded = requestCameraCapture(uid);
    if (uploaded) {
      lastProcessedUID = uid;
      Serial.println("Camera endpoint returned HTTP 200.");
      updateDisplay("Attendance sent", uid);
    } else {
      Serial.println("Capture/upload failed. Robot remains stopped for retry.");
      updateDisplay("Upload failed", "Scan again / check Wi-Fi");
    }
    state = WAIT_FOR_RFID;
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nSHRAM MITRA main controller starting...");

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  stopMotors();

  for (int i = 0; i < 5; i++) pinMode(IR_PINS[i], INPUT);

  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  if (BUZZER_PIN >= 0) pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin(OLED_SDA, OLED_SCL);
  oledReady = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!oledReady) Serial.println("OLED not detected at 0x3C; check address/wiring.");

  dht.begin();

  SPI.begin(RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN, RFID_SS_PIN);
  rfid.PCD_Init();
  delay(50);
  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.printf("RC522 VersionReg: 0x%02X\n", version);

  connectWiFi();
  updateDisplay("System ready", "Following line");
}

void loop() {
  float distance = readDistanceCM();
  if (distance > 0 && distance < OBSTACLE_STOP_CM && state != WAIT_FOR_RFID) {
    stopMotors();
    state = OBSTACLE_STOP;
    updateDisplay("Obstacle detected", String(distance, 1) + " cm");
    delay(80);
    return;
  }

  if (state == OBSTACLE_STOP) {
    if (distance < 0 || distance >= OBSTACLE_STOP_CM) {
      state = FOLLOW_LINE;
      updateDisplay("Obstacle clear", "Following line");
    } else {
      stopMotors();
      delay(80);
      return;
    }
  }

  if (state == WAIT_FOR_RFID) {
    stopMotors();
    updateDisplay("Checkpoint", "Scan RFID card");
    handleRFID();

    static unsigned long lastEnvRead = 0;
    if (millis() - lastEnvRead > 3000) {
      lastEnvRead = millis();
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      Serial.printf("DHT11 temperature: %.1f C, humidity: %.1f %% | PIR: %d\n",
                    t, h, digitalRead(PIR_PIN));
    }
    delay(20);
    return;
  }

  if (state == CAMERA_REQUEST) {
    stopMotors();
    delay(20);
    return;
  }

  bool atCheckpoint = checkpointPattern();
  if (atCheckpoint) {
    if (checkpointSince == 0) checkpointSince = millis();
    if (millis() - checkpointSince >= CHECKPOINT_CONFIRM_MS) {
      stopMotors();
      state = WAIT_FOR_RFID;
      checkpointSince = 0;
      updateDisplay("Checkpoint", "Scan RFID card");
      return;
    }
  } else {
    checkpointSince = 0;
  }

  int position = readLinePosition();
  if (position == 999) {
    stopMotors();
    updateDisplay("Line not found", "Stopped safely");
  } else if (position == 0) {
    driveForward();
    updateDisplay("Line centered", "Moving");
  } else if (position < 0) {
    turnLeft();
    updateDisplay("Correcting", "Left");
  } else {
    turnRight();
    updateDisplay("Correcting", "Right");
  }

  delay(20);
}
