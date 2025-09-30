#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"

// WiFi credentials
const char* ssid = "Sanchit-5G";
const char* password = "Lukaluka12";

// Flask server URL (your PC's IP + Flask port)
const char* serverName = "http://192.168.29.77:5000/";

// Arduino communication pin
#define ARDUINO_COMM_PIN 13

// Variables for sensor ID detection
volatile int pulseCount = 0;
int currentSensorId = 0;
unsigned long lastPulseTime = 0;
bool captureReady = false;

// AI Thinker ESP32-CAM pin configuration
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

// Interrupt handler for counting pulses from Arduino
void IRAM_ATTR countPulse() {
  pulseCount++;
  lastPulseTime = millis();
}

void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Set frame size and quality
  config.frame_size = FRAMESIZE_QVGA;  // 320x240
  config.jpeg_quality = 12;            // 0 = best, 63 = worst
  config.fb_count = 1;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x. Restarting...\n", err);
    delay(2000);
    ESP.restart();
  }

  Serial.println("Camera initialized successfully");
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Setup Arduino communication pin
  pinMode(ARDUINO_COMM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ARDUINO_COMM_PIN), countPulse, RISING);
  
  connectWiFi();
  startCamera();
}

void loop() {
  // Check if pulse sequence is complete (500ms timeout after last pulse)
  if(pulseCount > 0 && (millis() - lastPulseTime) > 500) {
    currentSensorId = pulseCount;
    pulseCount = 0;
    captureReady = true;
    Serial.println("Sensor ID received: " + String(currentSensorId));
  }
  
  // Only capture and send if we received a sensor trigger
  if(captureReady) {
    captureAndSendImage();
    captureReady = false;
  }
  
  delay(100); // Small delay to prevent excessive looping
}

void captureAndSendImage() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return;
  }

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("Captured image is not JPEG format!");
    esp_camera_fb_return(fb);
    return;
  }

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "image/jpeg");
  http.addHeader("X-Sensor-ID", "SENSOR_" + String(currentSensorId));

  Serial.println("Sending image from sensor " + String(currentSensorId));
  int httpResponseCode = http.POST(fb->buf, fb->len);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println("Response:");
    Serial.println(response);
  } else {
    Serial.print("HTTP POST failed: ");
    Serial.println(http.errorToString(httpResponseCode).c_str());
  }

  http.end();
  esp_camera_fb_return(fb);
}