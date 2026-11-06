/*
  SHRAM MITRA — ESP32-CAM firmware

  Assumes the common AI-Thinker ESP32-CAM module.
  If your camera board is different, replace the camera pin mapping.

  Libraries:
    - ESP32 Arduino core (includes esp_camera.h)
    - WiFi
    - WebServer
    - HTTPClient
    - ArduinoJson (Benoit Blanchon)

  Main ESP32 calls:
    http://CAMERA_IP/capture?uid=STUDENT_UID

  The camera captures a JPEG and POSTs it to Google Apps Script.
*/

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <mbedtls/base64.h>

// ---------- Wi-Fi ----------
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Paste the deployed Google Apps Script Web App URL ending in /exec.
const char* APPS_SCRIPT_URL = "PASTE_YOUR_APPS_SCRIPT_EXEC_URL_HERE";

// Must exactly match API_KEY in Code.gs.
const char* API_KEY = "CHANGE_THIS_TO_A_LONG_RANDOM_SECRET";

WebServer server(80);

// ---------- AI-Thinker ESP32-CAM pin map ----------
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

bool cameraReady = false;

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 15;
    config.fb_count = 1;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }
  return true;
}

String base64Encode(const uint8_t* input, size_t length) {
  size_t outputLength = 0;
  size_t bufferSize = 4 * ((length + 2) / 3) + 1;
  unsigned char* output = (unsigned char*) malloc(bufferSize);
  if (!output) return "";

  int result = mbedtls_base64_encode(output, bufferSize, &outputLength, input, length);
  if (result != 0) {
    free(output);
    return "";
  }

  String encoded((char*)output);
  free(output);
  return encoded;
}

bool postImageToAppsScript(const String& uid, camera_fb_t* frame, String& responseText) {
  if (String(APPS_SCRIPT_URL).indexOf("/exec") < 0) {
    responseText = "Apps Script URL is not configured (expected /exec URL).";
    return false;
  }

  String encoded = base64Encode(frame->buf, frame->len);
  if (encoded.length() == 0) {
    responseText = "Base64 encoding failed; check available memory.";
    return false;
  }

  StaticJsonDocument<512> doc;
  doc["apiKey"] = API_KEY;
  doc["uid"] = uid;
  doc["mimeType"] = "image/jpeg";
  doc["photoBase64"] = encoded;

  String body;
  // Large JSON payload is allocated in heap. VGA may exceed available memory;
  // reduce frame size to QVGA if uploads fail due to memory or request limits.
  serializeJson(doc, body);
  encoded = "";

  WiFiClientSecure client;
  client.setInsecure(); // Prototype only. Use certificate validation for production.

  HTTPClient https;
  https.setTimeout(45000);
  if (!https.begin(client, APPS_SCRIPT_URL)) {
    responseText = "Could not start HTTPS connection.";
    return false;
  }
  https.addHeader("Content-Type", "application/json");
  int status = https.POST(body);
  responseText = https.getString();
  https.end();
  body = "";

  Serial.printf("Apps Script HTTP status: %d\n", status);
  Serial.println(responseText);
  return status >= 200 && status < 300 && responseText.indexOf("\"success\":true") >= 0;
}

void handleCapture() {
  String uid = server.arg("uid");
  if (uid.length() == 0) uid = "UNKNOWN";

  if (!cameraReady) {
    server.send(500, "text/plain", "Camera is not initialized.");
    return;
  }
  if (WiFi.status() != WL_CONNECTED) {
    server.send(503, "text/plain", "Camera is not connected to Wi-Fi.");
    return;
  }

  camera_fb_t* frame = esp_camera_fb_get();
  if (!frame) {
    server.send(500, "text/plain", "Camera capture failed.");
    return;
  }

  String responseText;
  bool ok = postImageToAppsScript(uid, frame, responseText);
  esp_camera_fb_return(frame);

  if (ok) {
    server.send(200, "application/json", responseText);
  } else {
    server.send(502, "application/json", responseText);
  }
}

void handleRoot() {
  String message = "SHRAM MITRA ESP32-CAM is online\n";
  message += "Use /capture?uid=TEST001 to capture and upload.\n";
  message += "IP: " + WiFi.localIP().toString();
  server.send(200, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nSHRAM MITRA camera starting...");

  cameraReady = initCamera();
  if (!cameraReady) {
    Serial.println("Camera unavailable. Check board model and pin mapping.");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 25000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Camera IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection failed.");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/capture", HTTP_GET, handleCapture);
  server.begin();
  Serial.println("HTTP camera server started.");
}

void loop() {
  server.handleClient();
}
