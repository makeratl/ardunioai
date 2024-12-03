#include <FastLED.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include "config.h"

// LED Matrix configuration
#define LED_PIN     6
#define NUM_LEDS    256
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define MATRIX_WIDTH  16
#define MATRIX_HEIGHT 16

CRGB leds[NUM_LEDS];

// API Configuration
const unsigned long API_CHECK_INTERVAL = 15000;  // Check every 15 seconds
const unsigned long WIFI_ATTEMPT_INTERVAL = 500;  // Time between connection attempts
unsigned long lastApiCheck = 0;
unsigned long lastWiFiAttempt = 0;
int wifiAttempts = 0;
const int MAX_WIFI_ATTEMPTS = 10;
bool wifiResetInProgress = false;

// Animation state
struct AnimationState {
  uint8_t mode = 0;
  uint8_t brightness = BRIGHTNESS;
  uint8_t speed = 50;
  bool hasValidData = false;
  float spiralAngle = 0;
  uint8_t hue = 0;
  bool isChecking = false;
} state;

// XY mapping function for the LED matrix
uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t actual_y = 15 - y;  // Flip Y coordinate (0 is bottom)
  if(actual_y % 2 == 0) {
    return (actual_y * 16) + x;
  } else {
    return (actual_y * 16) + (15 - x);
  }
}

// Function declarations
void startWiFiReset();
void handleWiFiConnection();
bool checkApiStatus();
void drawSpinningPattern(CRGB color);
void drawCheckmark(CRGB color);
void drawXmark(CRGB color);
void drawPulsingColors();
void drawSpiral();
void drawWave();
void drawCloudMist();
void showWiFiStatus();

void setup() {
  delay(2000);
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("\nMatrix Controller Starting...");
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(state.brightness);
  
  // Initial WiFi connection
  startWiFiReset();  // Start the WiFi connection process
  
  // Set initial animation state
  state.mode = 1;  // Default to pulsing colors
  state.brightness = BRIGHTNESS;
  state.speed = 50;
  state.hasValidData = true;  // Allow default animation to run
  FastLED.setBrightness(state.brightness);
}

void loop() {
  EVERY_N_MILLISECONDS(20) {  // 50fps update rate
    FastLED.clear();
    
    // Run current animation regardless of connection status
    if (state.hasValidData) {
      switch (state.mode) {
        case 1: drawPulsingColors(); break;
        case 2: drawSpiral(); break;
        case 3: drawWave(); break;
        case 4: drawCloudMist(); break;
        default: drawPulsingColors(); break;
      }
    } else {
      drawPulsingColors();
    }
    
    // Handle WiFi connection state machine
    handleWiFiConnection();
    
    // Show WiFi status in corners
    showWiFiStatus();
    
    // Check API periodically without interrupting animation
    static int failedChecks = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - lastApiCheck >= API_CHECK_INTERVAL) {
      lastApiCheck = currentMillis;
      
      if (WiFi.status() == WL_CONNECTED) {
        state.isChecking = true;
        if (!checkApiStatus()) {
          failedChecks++;
          if (failedChecks >= 3) {  // After 3 failed checks, reset WiFi
            failedChecks = 0;
            startWiFiReset();
          }
        } else {
          failedChecks = 0;  // Reset counter on successful check
        }
      } else if (!wifiResetInProgress) {
        startWiFiReset();
      }
    }
    
    FastLED.show();
  }
}

void startWiFiReset() {
  if (!wifiResetInProgress) {
    Serial.println("\nStarting WiFi reset...");
    wifiResetInProgress = true;
    wifiAttempts = 0;
    WiFi.end();
    state.isChecking = true;
  }
}

void handleWiFiConnection() {
  if (!wifiResetInProgress) return;
  
  unsigned long currentMillis = millis();
  if (currentMillis - lastWiFiAttempt >= WIFI_ATTEMPT_INTERVAL) {
    lastWiFiAttempt = currentMillis;
    
    if (wifiAttempts == 0) {
      Serial.print("\nConnecting to WiFi: ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      wifiResetInProgress = false;
      state.isChecking = false;
      
      if (!state.hasValidData) {
        state.mode = 1;
        state.brightness = BRIGHTNESS;
        state.speed = 50;
        state.hasValidData = true;
        FastLED.setBrightness(state.brightness);
      }
      return;
    }
    
    wifiAttempts++;
    Serial.print(".");
    
    if (wifiAttempts >= MAX_WIFI_ATTEMPTS) {
      Serial.println("\nConnection failed, will retry later");
      wifiResetInProgress = false;
      state.isChecking = false;
      return;
    }
    
    // Additional reset attempts at specific intervals
    if (wifiAttempts == 5) {
      Serial.println("\nRetrying with reset...");
      WiFi.end();
      delay(100);  // Short delay is still needed for WiFi hardware
      WiFi.begin(ssid, password);
    }
  }
}

bool checkApiStatus() {
  WiFiClient client;
  bool connected = false;
  int retryCount = 0;
  const int MAX_RETRIES = 2;
  
  Serial.println("\nChecking for updates...");
  
  while (!connected && retryCount < MAX_RETRIES) {
    if (!client.connected()) {
      Serial.print("Connecting to ");
      Serial.println(apiHost);
      if (!client.connect(apiHost, 80)) {
        Serial.println("Connection failed");
        retryCount++;
        if (retryCount < MAX_RETRIES) {
          delay(100);
        }
        continue;
      }
    }
    
    String request = String("GET /api-T/matrix/config HTTP/1.1\r\n") +
                    "Host: " + apiHost + "\r\n" +
                    "Connection: close\r\n\r\n";
    
    Serial.println("Sending request...");
    client.print(request);
    
    // Process response with minimal delays
    unsigned long timeout = millis();
    bool receivedResponse = false;
    
    while (!receivedResponse && (millis() - timeout < 2000)) {
      if (client.available()) {
        receivedResponse = true;
        String jsonResponse = "";
        bool headersEnded = false;
        
        while (client.connected() || client.available()) {
          if (client.available()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") {
              headersEnded = true;
              continue;
            }
            if (headersEnded) {
              jsonResponse += line;
            }
          }
        }
        
        jsonResponse.trim();
        
        if (jsonResponse.length() > 0) {
          Serial.print("Received: ");
          Serial.println(jsonResponse);
          
          StaticJsonDocument<200> doc;
          DeserializationError error = deserializeJson(doc, jsonResponse);
          
          if (!error) {
            int newMode = doc["mode"] | state.mode;
            if (newMode != state.mode) {
              Serial.print("New mode received: ");
              Serial.println(newMode);
            }
            state.mode = newMode;
            state.brightness = doc["brightness"] | state.brightness;
            state.speed = doc["speed"] | state.speed;
            state.hasValidData = true;
            FastLED.setBrightness(state.brightness);
            connected = true;
          } else {
            Serial.print("JSON parse error: ");
            Serial.println(error.c_str());
          }
        } else {
          Serial.println("Empty response received");
        }
      }
      EVERY_N_MILLISECONDS(20) {
        FastLED.show();  // Keep animations running during wait
      }
    }
    
    if (!receivedResponse) {
      Serial.println("Response timeout");
    }
    
    client.stop();
    
    if (!connected) {
      retryCount++;
      if (retryCount < MAX_RETRIES) {
        Serial.print("Retry ");
        Serial.println(retryCount);
        delay(100);
      }
    }
  }
  
  if (!connected) {
    Serial.println("Failed to get updates");
  }
  
  state.isChecking = false;
  return connected;
}

void showStartupAnimation() {
  for(int i = 0; i < 3; i++) {  // Spin 3 times
    for(int j = 0; j < 16; j++) {
      FastLED.clear();
      drawSpinningPattern(CRGB::Yellow);
      FastLED.show();
      delay(50);
    }
  }
}

void drawSpinningPattern(CRGB color) {
  static uint8_t pos = 0;
  // Draw spinning line
  for(uint8_t i = 0; i < 8; i++) {
    uint8_t x = 7 + cos8(pos + i * 32) / 32;
    uint8_t y = 7 + sin8(pos + i * 32) / 32;
    leds[XY(x, y)] = color;
    leds[XY(x+1, y)] = color;
    leds[XY(x, y+1)] = color;
    leds[XY(x+1, y+1)] = color;
  }
  pos += 8;  // Speed of spin
}

void drawCheckmark(CRGB color) {
  // Draw checkmark
  for(int8_t offset = -1; offset <= 1; offset++) {
    // Short part
    for(int8_t i = 0; i < 3; i++) {
      uint8_t x = 4 + i;
      uint8_t y = 8 + i;
      leds[XY(x, y + offset)] = color;
    }
    
    // Long part
    for(int8_t i = 0; i < 5; i++) {
      uint8_t x = 7 + i;
      uint8_t y = 10 - i;
      leds[XY(x, y + offset)] = color;
    }
  }
}

void drawXmark(CRGB color) {
  // Use full grid for border
  uint8_t startPos = 0;
  uint8_t endPos = 15;
  
  // Draw centered X (thicker lines)
  for(int8_t offset = -1; offset <= 1; offset++) {
    // Main diagonal
    for(int8_t i = 2; i < 14; i++) {
      leds[XY(i, i + offset)] = color;
    }
    
    // Counter diagonal
    for(int8_t i = 2; i < 14; i++) {
      leds[XY(i, 15 - i + offset)] = color;
    }
  }
  
  // Animated border
  uint8_t borderHue = beatsin8(30, 0, 32);  // Red range
  uint8_t borderBright = beatsin8(40, 128, 255);
  
  // Draw border
  for(uint8_t i = startPos; i <= endPos; i++) {
    leds[XY(i, startPos)] = CHSV(borderHue, 255, borderBright);
    leds[XY(i, endPos)] = CHSV(borderHue, 255, borderBright);
    leds[XY(startPos, i)] = CHSV(borderHue, 255, borderBright);
    leds[XY(endPos, i)] = CHSV(borderHue, 255, borderBright);
  }
  
  // Corner highlights
  CRGB cornerColor = CHSV(borderHue, 192, 255);
  leds[XY(startPos, startPos)] = cornerColor;
  leds[XY(endPos, startPos)] = cornerColor;
  leds[XY(startPos, endPos)] = cornerColor;
  leds[XY(endPos, endPos)] = cornerColor;
}

void drawPulsingColors() {
  uint8_t bright = beatsin8(state.speed / 10, 100, 255);
  
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      leds[XY(x, y)] = CHSV(state.hue + (x + y) * 4, 255, bright);
    }
  }
  
  EVERY_N_MILLISECONDS(20) {
    state.hue++;
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
      
      uint8_t hue = (uint8_t)((angle + state.spiralAngle) * 128/PI + distance * 8);
      uint8_t bright = beatsin8(state.speed / 8, 128, 255, 0, distance * 32);
      
      leds[XY(x, y)] = CHSV(hue, 255, bright);
    }
  }
  
  EVERY_N_MILLISECONDS(20) {
    state.spiralAngle += (float)state.speed / 1000.0f;
  }
}

void drawWave() {
  uint8_t t = millis() / (101 - state.speed);
  
  for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
    uint8_t y = beatsin8(state.speed / 2, 0, MATRIX_HEIGHT - 1, 0, x * 8);
    leds[XY(x, y)] = CHSV(state.hue + x * 8, 255, 255);
    
    // Add trailing effect
    for(int8_t i = 1; i < 4; i++) {
      if(y + i < MATRIX_HEIGHT) {
        leds[XY(x, y + i)] = CHSV(state.hue + x * 8, 255, 255 - (64 * i));
      }
      if(y - i >= 0) {
        leds[XY(x, y - i)] = CHSV(state.hue + x * 8, 255, 255 - (64 * i));
      }
    }
  }
  
  EVERY_N_MILLISECONDS(50) {
    state.hue++;
  }
}

// Cloud mist effect variables
static uint8_t noise[16][16];  // Fixed size array for 16x16 matrix
static uint32_t ioffset = 0;

void drawCloudMist() {
  // Update noise pattern
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      noise[x][y] = inoise8(x * 60, y * 60, ioffset);
    }
  }
  ioffset += 1000;  // Adjust speed of noise movement

  // Map noise to LED colors
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      uint8_t data = noise[x][y];
      uint8_t hue = map(data, 0, 255, 160, 220);  // Blue to purple range
      leds[XY(x, y)] = CHSV(hue, 200, data);
    }
  }
}

void showWiFiStatus() {
  CRGB cornerColor;
  bool shouldBlink = false;
  
  if (state.isChecking) {
    // Blinking red during checks
    cornerColor = CRGB::Red;
    shouldBlink = true;
  } else if (WiFi.status() == WL_CONNECTED) {
    // Solid green when connected
    cornerColor = CRGB::Green;
    shouldBlink = false;
  } else {
    // Blinking red when disconnected
    cornerColor = CRGB::Red;
    shouldBlink = true;
  }
  
  // If should blink, use millis to create blink effect
  if (shouldBlink && (millis() % 1000) < 500) {
    cornerColor = CRGB::Black;
  }
  
  // Set corner LEDs
  leds[XY(0, 0)] = cornerColor;         // Bottom left
  leds[XY(15, 0)] = cornerColor;        // Bottom right
  leds[XY(0, 15)] = cornerColor;        // Top left
  leds[XY(15, 15)] = cornerColor;       // Top right
}