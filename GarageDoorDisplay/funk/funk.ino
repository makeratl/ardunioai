#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    600
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// Window parameters
#define WINDOW_HEIGHT 20
#define WINDOW_WIDTH 130
#define LEDS_PER_WINDOW 300
#define TOP_OFFSET 20

// Smoother color palette with fewer transitions
DEFINE_GRADIENT_PALETTE( flow_gp ) {
    0,     0,  0,  255,  // Deep Blue
   64,    0,  255, 255,  // Cyan
   128,   0,  255, 0,    // Green
   192,   255, 0,  255,  // Purple
   255,   0,  0,  255    // Back to Deep Blue
};

CRGBPalette16 flowPalette = flow_gp;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  static uint16_t timeOffset = 0;
  static uint8_t hue = 0;
  
  // Process both windows
  for(int window = 0; window < 2; window++) {
    int windowStart = window * LEDS_PER_WINDOW;
    
    // Create flowing patterns for bottom and top bars
    for(int i = 0; i < WINDOW_WIDTH; i++) {
      // Slower, gentler wave patterns
      uint8_t wave1 = sin8(i * 3 + timeOffset);
      uint8_t wave2 = sin8(i * 2 - timeOffset / 2);
      uint8_t wave3 = cos8(i * 4 + timeOffset / 3);
      
      // Smooth wave combination
      uint8_t colorIndex = (wave1 * 2 + wave2 + wave3) / 4;
      
      // Bottom bar with smooth color transition
      CRGB color = ColorFromPalette(flowPalette, colorIndex + hue);
      leds[windowStart + i] = color;
      
      // Top bar (mirrored with offset)
      int topIndex = windowStart + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - i - 1);
      // Complementary pattern for top
      uint8_t topColorIndex = (wave2 * 2 + wave3) / 3;
      color = ColorFromPalette(flowPalette, topColorIndex + hue + 128); // Offset hue for complementary color
      leds[topIndex] = color;
    }
    
    // Add gentle flowing overlay
    for(int i = 0; i < WINDOW_WIDTH; i++) {
      uint8_t overlay = sin8((i * 2 + timeOffset) / 4);
      // Bottom enhancement
      leds[windowStart + i].fadeToBlackBy(overlay / 8);
      
      // Top enhancement
      int topIndex = windowStart + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - i - 1);
      leds[topIndex].fadeToBlackBy(overlay / 8);
    }
  }
  
  // Slower time and color progression
  timeOffset += 1;  // Reduced speed
  hue += 1;         // Slower color cycling
  
  FastLED.show();
  delay(30);  // Slightly longer delay for smoother movement
}