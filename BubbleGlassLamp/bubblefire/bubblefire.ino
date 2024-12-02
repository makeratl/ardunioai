#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    497
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Define sections
#define BOTTOM_SPIRAL 181
#define MIDDLE_CYLINDER_START 181
#define MIDDLE_CYLINDER_END 437
#define TOP_SECTION_START 437
#define TOP_SECTION_END 497

// Fire and spark parameters
#define FLAME_SPEED 4
#define SPARK_CHANCE 20
#define SPARK_FADE 10
#define FLAME_HEIGHT 8
#define BLUR_AMOUNT 150

uint16_t timeOffset = 0;

CRGB leds[NUM_LEDS];

// Warmer, more intense fire palette
DEFINE_GRADIENT_PALETTE(hotFireGradient) {
    0,     0,  0,  0,      // black
   32,    120, 0,  0,      // deeper red
   90,    200, 0,  0,      // intense red
  150,    255, 20, 0,      // red-orange
  200,    255, 60, 0,      // warm orange
  255,    255, 100, 0      // bright orange
};
CRGBPalette16 firePalette = hotFireGradient;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  fadeToBlackBy(leds, NUM_LEDS, SPARK_FADE);
  
  // Update the fire effect for middle cylinder
  for(int x = 0; x < 32; x++) {  // Around the cylinder
    for(int y = 0; y < FLAME_HEIGHT; y++) {  // Up the cylinder
      int index = (y * 32) + x;
      
      // Create cylindrical flame effect
      uint8_t angle = (x * 256) / 32;  // Convert x to angle (0-255)
      
      // Base flame movement
      uint8_t flameBase = sin8(angle + timeOffset);
      
      // Rotating flame effect
      uint8_t rotatingFlame = sin8(angle * 2 + timeOffset * 2) + 
                             cos8(angle * 3 - timeOffset);
      
      // Vertical flame movement
      uint8_t verticalWave = sin8((y * 32) + timeOffset * 3);
      
      // Height-based intensity (hotter at bottom)
      uint8_t heightFactor = 255 - ((y * 255) / FLAME_HEIGHT);
      
      // Combine all effects
      uint8_t brightness = heightFactor;
      brightness = qadd8(brightness, flameBase / 2);
      brightness = qadd8(brightness, rotatingFlame / 3);
      brightness = qadd8(brightness, verticalWave / 4);
      
      // Add more intensity to the bottom
      if(y < 3) {
        brightness = qadd8(brightness, 50);
      }
      
      // Map to fire palette
      int ledIndex = MIDDLE_CYLINDER_START + index;
      leds[ledIndex] = ColorFromPalette(firePalette, brightness);
      
      // Add random hot spots (more frequent at bottom)
      if(random8() < (50 - (y * 6))) {
        // Warmer colors for the hot spots
        if(y < 2) {
          // Brighter, whiter hot spots at base
          leds[ledIndex] += CRGB(random8(100, 255), random8(30, 60), 0);
        } else {
          // Regular orange-red hot spots
          leds[ledIndex] += CRGB(random8(80), random8(20), 0);
        }
      }
    }
  }
  
  // Enhanced spark effect for top and bottom
  if(random8() < SPARK_CHANCE) {
    // Bottom spark with ember trail
    int sparkPos = random16(BOTTOM_SPIRAL);
    // Intense orange-red spark
    leds[sparkPos] = CRGB(255, random8(40, 80), 0);
    // Add ember trail
    if(sparkPos > 2) {
      leds[sparkPos-1] = CRGB(180, 30, 0);
      leds[sparkPos-2] = CRGB(100, 10, 0);
    }
  }
  
  if(random8() < SPARK_CHANCE) {
    // Top spark with ember trail
    int sparkPos = random16(TOP_SECTION_START, TOP_SECTION_END);
    leds[sparkPos] = CRGB(255, random8(40, 80), 0);
    // Add ember trail
    if(sparkPos < (TOP_SECTION_END - 2)) {
      leds[sparkPos+1] = CRGB(180, 30, 0);
      leds[sparkPos+2] = CRGB(100, 10, 0);
    }
  }
  
  // Double blur pass for extra smoothness
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT);
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT/2);
  
  EVERY_N_MILLISECONDS(20) {
    timeOffset += FLAME_SPEED;
  }
  
  FastLED.show();
}