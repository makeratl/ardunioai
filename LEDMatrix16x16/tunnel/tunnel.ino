#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    256  // 16x16 matrix
#define BRIGHTNESS  20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define MATRIX_WIDTH  16
#define MATRIX_HEIGHT 16
#define CENTER_X      8
#define CENTER_Y      8

CRGB leds[NUM_LEDS];

// Tunnel parameters
#define TUNNEL_SPEED     4     // Speed of tunnel movement
#define COLOR_SPEED      2     // Speed of color changes
#define NUM_RINGS        4     // Fewer rings for smoother gradient
#define ANGLE_SPEED      3     // Faster rotation

uint8_t hue = 0;
uint16_t timeOffset = 0;

// Convert x,y coordinate to LED index
uint16_t XY(uint8_t x, uint8_t y) {
  // Zigzag layout
  uint16_t i;
  if(y & 0x01) {  // Odd rows run backwards
    uint8_t reverseX = (MATRIX_WIDTH - 1) - x;
    i = (y * MATRIX_WIDTH) + reverseX;
  } else {        // Even rows run forwards
    i = (y * MATRIX_WIDTH) + x;
  }
  return i;
}

// Calculate distance from center point with floating point for smoother gradients
float fdistance(uint8_t x, uint8_t y) {
  float dx = CENTER_X - x;
  float dy = CENTER_Y - y;
  return sqrt(dx * dx + dy * dy);
}

// Calculate angle from center point
uint8_t angle(uint8_t x, uint8_t y) {
  int16_t dx = x - CENTER_X;
  int16_t dy = y - CENTER_Y;
  return atan2(dy, dx) * 128 / PI;
}

void setup() {
  delay(2000);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  // Update time and color
  timeOffset += TUNNEL_SPEED;
  hue += COLOR_SPEED;
  
  // Draw each pixel
  for(uint8_t x = 0; x < MATRIX_WIDTH; x++) {
    for(uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
      float dist = fdistance(x, y);
      uint8_t ang = angle(x, y);
      
      // Create smooth whirlpool effect
      uint8_t tunnelValue = beatsin8(10, 0, 255, 0, dist * 32);
      
      // Add spinning motion
      uint8_t spinOffset = scale8(ang, beatsin8(5, 100, 255));
      tunnelValue += spinOffset;
      
      // Create vibrant color palette
      uint8_t baseHue = hue + tunnelValue;
      uint8_t sat = beatsin8(5, 195, 255, 0, dist * 32);
      
      // Brightness falls off toward edges
      uint8_t bri = 255 - (dist * 18);
      bri = qadd8(bri, tunnelValue / 4);
      
      // Add temporal variation
      bri = scale8(bri, beatsin8(7, 192, 255));
      
      // Blend between two colors for more vibrancy
      CRGB color1 = CHSV(baseHue, sat, bri);
      CRGB color2 = CHSV(baseHue + 64, sat, bri);
      uint8_t blend = sin8(tunnelValue);
      CRGB blendedColor = blend_CRGB(color1, color2, blend);
      
      // Add extra sparkle to bright areas
      if(bri > 180 && random8() < 32) {
        blendedColor += CRGB(32, 32, 32);
      }
      
      leds[XY(x, y)] = blendedColor;
    }
  }
  
  // Add flowing highlights
  for(uint8_t i = 0; i < 3; i++) {
    uint8_t pos = beatsin8(10 + i, 0, MATRIX_WIDTH - 1, i * 1000);
    uint8_t pos2 = beatsin8(12 + i, 0, MATRIX_HEIGHT - 1, i * 1500);
    leds[XY(pos, pos2)] += CRGB(64, 64, 64);
  }
  
  // Simple blur effect
  uint8_t blurAmount = 32;
  for(uint8_t i = 1; i < MATRIX_WIDTH-1; i++) {
    for(uint8_t j = 1; j < MATRIX_HEIGHT-1; j++) {
      CRGB avgColor = CRGB(0, 0, 0);
      
      // Average with neighboring pixels
      avgColor += leds[XY(i-1, j)];
      avgColor += leds[XY(i+1, j)];
      avgColor += leds[XY(i, j-1)];
      avgColor += leds[XY(i, j+1)];
      
      avgColor /= 4;
      
      // Blend with original pixel
      CRGB& pixel = leds[XY(i, j)];
      pixel = blend(pixel, avgColor, blurAmount);
    }
  }
  
  FastLED.show();
  delay(10);
}

// Helper function to blend two CRGB colors
CRGB blend_CRGB(CRGB color1, CRGB color2, uint8_t blend) {
  CRGB result;
  result.r = blend8(color1.r, color2.r, blend);
  result.g = blend8(color1.g, color2.g, blend);
  result.b = blend8(color1.b, color2.b, blend);
  return result;
} 