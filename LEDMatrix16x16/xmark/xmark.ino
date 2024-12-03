#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    256  // 16x16 matrix
#define BRIGHTNESS  20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define MATRIX_WIDTH  16
#define MATRIX_HEIGHT 16

CRGB leds[NUM_LEDS];

// Simple XY mapping for bottom-up serpentine
uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t actual_y = 15 - y;  // Flip Y coordinate (0 is bottom)
  if(actual_y % 2 == 0) {
    // Even rows (from bottom) go left to right
    return (actual_y * 16) + x;
  } else {
    // Odd rows go right to left
    return (actual_y * 16) + (15 - x);
  }
}

void setup() {
  delay(2000);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  FastLED.clear();
  
  // Use full grid for border
  uint8_t startPos = 0;     // Start at edge
  uint8_t endPos = 15;      // End at edge
  
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
  
  // Animated square border
  uint8_t borderHue = beatsin8(30, 0, 32);  // Red range
  uint8_t borderBright = beatsin8(40, 128, 255);  // Pulsing brightness
  
  // Draw the square border at grid edges
  for(uint8_t i = startPos; i <= endPos; i++) {
    // Top and bottom edges
    leds[XY(i, startPos)] = CHSV(borderHue, 255, borderBright);
    leds[XY(i, endPos)] = CHSV(borderHue, 255, borderBright);
    
    // Left and right edges
    leds[XY(startPos, i)] = CHSV(borderHue, 255, borderBright);
    leds[XY(endPos, i)] = CHSV(borderHue, 255, borderBright);
  }
  
  // Add corner highlights
  CRGB cornerColor = CHSV(borderHue, 192, 255);  // Brighter corners
  leds[XY(startPos, startPos)] = cornerColor;  // Top-left
  leds[XY(endPos, startPos)] = cornerColor;    // Top-right
  leds[XY(startPos, endPos)] = cornerColor;    // Bottom-left
  leds[XY(endPos, endPos)] = cornerColor;      // Bottom-right
  
  // Optional: Add sparkle effect to the X
  if(random8() < 50) {  // 20% chance each frame
    uint8_t sparklePos = random8(12);
    sparklePos += 2;  // Offset from edges
    if(random8() < 128) {  // 50% chance for each diagonal
      leds[XY(sparklePos, sparklePos)] = CRGB::White;  // Main diagonal
    } else {
      leds[XY(sparklePos, 15 - sparklePos)] = CRGB::White;  // Counter diagonal
    }
  }
  
  FastLED.show();
  delay(20);
} 