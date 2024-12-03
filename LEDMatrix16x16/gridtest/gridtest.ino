#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    256  // 16x16 matrix
#define BRIGHTNESS  20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define MATRIX_WIDTH  16
#define MATRIX_HEIGHT 16

CRGB leds[NUM_LEDS];

// Current test pattern
uint8_t currentPattern = 0;
uint32_t patternTimer = 0;

void setup() {
  delay(2000);  // Power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  
  Serial.begin(115200);
  Serial.println("Grid Test Starting");
}

void loop() {
  FastLED.clear();
  
  // Change pattern every 5 seconds
  if(millis() - patternTimer > 5000) {
    currentPattern = (currentPattern + 1) % 5;
    patternTimer = millis();
    Serial.print("Showing pattern: ");
    Serial.println(currentPattern);
  }
  
  switch(currentPattern) {
    case 0:
      // Single moving pixel
      drawMovingPixel();
      break;
    case 1:
      // Row by row
      drawRows();
      break;
    case 2:
      // Column by column
      drawColumns();
      break;
    case 3:
      // Borders
      drawBorders();
      break;
    case 4:
      // Quadrants
      drawQuadrants();
      break;
  }
  
  FastLED.show();
  delay(50);
}

void drawMovingPixel() {
  static uint16_t pos = 0;
  pos = (pos + 1) % NUM_LEDS;
  leds[pos] = CRGB::Red;
}

void drawRows() {
  static uint8_t currentRow = 0;
  currentRow = ((millis() / 500) % MATRIX_HEIGHT);
  
  for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
    leds[currentRow * MATRIX_WIDTH + x] = CRGB::Blue;
  }
}

void drawColumns() {
  static uint8_t currentCol = 0;
  currentCol = ((millis() / 500) % MATRIX_WIDTH);
  
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    leds[y * MATRIX_WIDTH + currentCol] = CRGB::Green;
  }
}

void drawBorders() {
  // Top row
  for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
    leds[x] = CRGB::Red;
  }
  
  // Bottom row
  for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
    leds[(MATRIX_HEIGHT-1) * MATRIX_WIDTH + x] = CRGB::Blue;
  }
  
  // Left column
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    leds[y * MATRIX_WIDTH] = CRGB::Green;
  }
  
  // Right column
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    leds[y * MATRIX_WIDTH + (MATRIX_WIDTH-1)] = CRGB::Yellow;
  }
}

void drawQuadrants() {
  for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
    for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
      if(y < MATRIX_HEIGHT/2) {
        if(x < MATRIX_WIDTH/2) {
          leds[y * MATRIX_WIDTH + x] = CRGB::Red;     // Top-left
        } else {
          leds[y * MATRIX_WIDTH + x] = CRGB::Green;   // Top-right
        }
      } else {
        if(x < MATRIX_WIDTH/2) {
          leds[y * MATRIX_WIDTH + x] = CRGB::Blue;    // Bottom-left
        } else {
          leds[y * MATRIX_WIDTH + x] = CRGB::Yellow;  // Bottom-right
        }
      }
    }
  }
} 