#include <FastLED.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include "config.h"  // Include local configuration file

#define LED_PIN     5
#define NUM_LEDS    256
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define MATRIX_WIDTH  16
#define MATRIX_HEIGHT 16

// API Configuration
const char* apiEndpoint = "/api/arduinotest";
const unsigned long API_CHECK_INTERVAL = 5000;  // Check every 5 seconds
unsigned long lastApiCheck = 0;

// Animation state
uint8_t currentMode = 0;
uint8_t targetBrightness = 128;
uint8_t currentSpeed = 50;
bool hasValidData = false;
float spiralAngle = 0;

CRGB leds[NUM_LEDS];
const size_t JSON_BUFFER_SIZE = 48;  // Small fixed size for our known JSON structure

// Simple XY mapping for bottom-up serpentine
uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t actual_y = 15 - y;
  if(actual_y % 2 == 0) {
    return (actual_y * 16) + x;
  } else {
    return (actual_y * 16) + (15 - x);
  }
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  
  // Wait for Serial connection
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("\n\nLED Matrix API Control Starting...");
  Serial.println("-------------------------------");
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Wait for WiFi connection before proceeding
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    drawXmark();
    FastLED.show();
    delay(500);
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("\nStarting main loop...");
  Serial.println("Will check API every 5 seconds");
  Serial.println("-------------------------------");
}

void loop() {
  FastLED.clear();
  
  int wifiStatus = WiFi.status();
  static int lastWifiStatus = -1;
  static bool lastHasValidData = false;
  
  // Print WiFi status changes
  if (wifiStatus != lastWifiStatus) {
    Serial.print("\nWiFi Status Changed to: ");
    switch (wifiStatus) {
      case WL_CONNECTED:
        Serial.print("CONNECTED - IP: ");
        Serial.println(WiFi.localIP());
        break;
      case WL_DISCONNECTED:
        Serial.println("DISCONNECTED - Will retry connection");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("CONNECTION FAILED - Check credentials");
        break;
      case WL_CONNECTION_LOST:
        Serial.println("CONNECTION LOST - Will attempt recovery");
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("SSID NOT FOUND - Check network name");
        break;
      default:
        Serial.print("OTHER (");
        Serial.print(wifiStatus);
        Serial.println(")");
    }
    lastWifiStatus = wifiStatus;
  }
  
  if (wifiStatus != WL_CONNECTED) {
    if (lastWifiStatus != wifiStatus) {
      Serial.println("Waiting for WiFi connection...");
    }
    drawXmark();
    hasValidData = false;
  } else {
    // Check API when connected
    if (millis() - lastApiCheck >= API_CHECK_INTERVAL) {
      Serial.println("\nChecking API...");
      checkApi();
    }
    
    if (hasValidData) {
      // Only print animation status when it changes
      if (!lastHasValidData || millis() - lastApiCheck < 100) {
        Serial.print("Running animation mode: ");
        Serial.print(currentMode);
        Serial.print(" (Brightness: ");
        Serial.print(targetBrightness);
        Serial.print(", Speed: ");
        Serial.print(currentSpeed);
        Serial.println(")");
      }
      
      // Run the appropriate animation based on mode
      switch (currentMode) {
        case 1:
          drawPulsingColors();
          break;
        case 2:
          drawSpiral();
          break;
        default:
          if (!lastHasValidData) {
            Serial.println("Invalid mode received from API");
          }
          drawXmark();
          break;
      }
    } else {
      if (lastHasValidData) {
        Serial.println("API data became invalid");
      }
      drawXmark();
    }
  }
  
  lastHasValidData = hasValidData;
  FastLED.show();
  delay(20);
}

void checkApi() {
  WiFiClient client;
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  
  Serial.print("Connecting to API host: ");
  Serial.println(apiHost);
  
  if (client.connect(apiHost, 80)) {
    Serial.println("Connected to API host");
    
    // Make HTTP request
    Serial.print("Requesting: ");
    Serial.println(apiEndpoint);
    
    client.print("GET ");
    client.print(apiEndpoint);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(apiHost);
    client.println("Connection: close");
    client.println();
    
    Serial.println("Waiting for response...");
    
    // Skip HTTP headers
    while (client.available()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
      Serial.print("Header: ");
      Serial.println(line);
    }
    
    // Parse JSON response
    DeserializationError error = deserializeJson(doc, client);
    client.stop();
    
    if (!error) {
      Serial.println("JSON parsed successfully");
      currentMode = doc["mode"].as<int>();
      targetBrightness = doc["brightness"].as<int>();
      currentSpeed = doc["speed"].as<int>();
      
      Serial.print("Received - Mode: ");
      Serial.print(currentMode);
      Serial.print(", Brightness: ");
      Serial.print(targetBrightness);
      Serial.print(", Speed: ");
      Serial.println(currentSpeed);
      
      hasValidData = true;
      FastLED.setBrightness(targetBrightness);
    } else {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      hasValidData = false;
    }
  } else {
    Serial.println("Failed to connect to API host");
    hasValidData = false;
  }
  
  lastApiCheck = millis();
}

void drawPulsingColors() {
  static uint8_t hue = 0;
  uint8_t bright = beatsin8(currentSpeed / 10, 100, 255);
  
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      leds[XY(x, y)] = CHSV(hue + (x + y) * 4, 255, bright);
    }
  }
  
  EVERY_N_MILLISECONDS(50) {
    hue++;
  }
}

void drawSpiral() {
  float centerX = MATRIX_WIDTH / 2.0f - 0.5f;
  float centerY = MATRIX_HEIGHT / 2.0f - 0.5f;
  
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      float dx = x - centerX;
      float dy = y - centerY;
      float distance = sqrt(dx * dx + dy * dy);
      float angle = atan2(dy, dx);
      
      uint8_t hue = (uint8_t)((angle + spiralAngle) * 128/PI + distance * 8);
      uint8_t bright = beatsin8(currentSpeed / 8, 128, 255, 0, distance * 32);
      
      leds[XY(x, y)] = CHSV(hue, 255, bright);
    }
  }
  
  spiralAngle += (float)currentSpeed / 1000.0f;
}

void drawXmark() {
  // Use full grid for border
  uint8_t startPos = 0;
  uint8_t endPos = 15;
  
  // Draw centered X (thicker lines)
  for(int8_t offset = -1; offset <= 1; offset++) {
    // Main diagonal (bottom-left to top-right)
    for(int8_t i = 2; i < 14; i++) {
      leds[XY(i, i + offset)] = CRGB::Red;
    }
    
    // Counter diagonal (top-left to bottom-right)
    for(int8_t i = 2; i < 14; i++) {
      leds[XY(i, 15 - i + offset)] = CRGB::Red;
    }
  }
  
  // Animated border
  uint8_t borderHue = beatsin8(30, 0, 32);  // Red range
  uint8_t borderBright = beatsin8(40, 128, 255);
  
  // Draw border
  for(uint8_t i = startPos; i <= endPos; i++) {
    // Top and bottom edges
    leds[XY(i, startPos)] = CHSV(borderHue, 255, borderBright);
    leds[XY(i, endPos)] = CHSV(borderHue, 255, borderBright);
    
    // Left and right edges
    leds[XY(startPos, i)] = CHSV(borderHue, 255, borderBright);
    leds[XY(endPos, i)] = CHSV(borderHue, 255, borderBright);
  }
  
  // Corner highlights
  CRGB cornerColor = CHSV(borderHue, 192, 255);
  leds[XY(startPos, startPos)] = cornerColor;
  leds[XY(endPos, startPos)] = cornerColor;
  leds[XY(startPos, endPos)] = cornerColor;
  leds[XY(endPos, endPos)] = cornerColor;
  
  // Optional sparkle effect
  if(random8() < 50) {
    uint8_t sparklePos = random8(12) + 2;
    if(random8() < 128) {
      leds[XY(sparklePos, sparklePos)] = CRGB::White;
    } else {
      leds[XY(sparklePos, 15 - sparklePos)] = CRGB::White;
    }
  }
}
  