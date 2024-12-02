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

// Rainbow flow parameters
#define FLOW_SPEED 2
#define COLOR_SPREAD 3      // How spread out the colors are
#define BLUR_AMOUNT 128     // Smoothing amount
#define MIN_BRIGHTNESS 180  // Keep it bright
#define SATURATION 240      // Rich colors

// [Previous definitions remain the same, but add these new parameters]
#define COLOR_BANDS 8       // Increased number of color cycles
#define BAND_SHARPNESS 3    // Sharper color transitions
#define FLOW_VARIATION 4     // More color variation
#define PALETTE_SPEED 3      // How fast the colors shift

uint16_t flowOffset = 0;

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  // Update all LEDs
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness;
    uint8_t hue;
    uint8_t sat = SATURATION;
    
    if(i < BOTTOM_SPIRAL) {
      // Bottom spiral - tighter color bands
      uint8_t pos = map(i, 0, BOTTOM_SPIRAL, 0, 255);
      
      // Create more frequent color changes
      hue = (pos * COLOR_BANDS + flowOffset) * BAND_SHARPNESS;
      
      // Add complex wave patterns
      uint8_t wave1 = sin8(pos * 3 + flowOffset * 2);
      uint8_t wave2 = cos8(pos * 4 - flowOffset * 3);
      uint8_t wave3 = sin8(pos * 2 + flowOffset);
      brightness = map((wave1 + wave2 + wave3) / 3, 0, 255, MIN_BRIGHTNESS, 255);
      
    } else if(i < MIDDLE_CYLINDER_END) {
      // Middle cylinder - horizontal rainbow bands with more variation
      uint8_t x = (i - MIDDLE_CYLINDER_START) % 32;
      uint8_t y = (i - MIDDLE_CYLINDER_START) / 32;
      
      // Create more frequent color bands around cylinder
      uint16_t baseHue = (x * COLOR_BANDS * 8) + (flowOffset * FLOW_VARIATION);
      
      // Add multiple color influences
      uint8_t verticalEffect = sin8(y * 32 + flowOffset);
      uint8_t diagonalEffect = sin8((x + y * 4) * 8 + flowOffset * 2);
      hue = baseHue + scale8(verticalEffect, 32) + scale8(diagonalEffect, 16);
      
      // Create dynamic wave patterns
      uint8_t wave1 = sin8(x * 12 + flowOffset * 2);
      uint8_t wave2 = cos8(x * 8 - flowOffset);
      uint8_t wave3 = sin8(y * 16 + flowOffset * 3);
      uint8_t wave4 = cos8((x + y * 2) * 8 - flowOffset * 2);
      
      // Combine waves for complex brightness
      brightness = map((wave1 + wave2 + wave3 + wave4) / 4, 
                      0, 255, MIN_BRIGHTNESS, 255);
      
      // Enhanced center band with color variation
      if(y > 2 && y < 6) {
        brightness = qadd8(brightness, 30);
        hue += sin8(flowOffset * 2) / 8;  // Subtle hue shift in center
        sat = qsub8(sat, 20);
      }
      
      // Dynamic sparkles with color variation
      if(random8() < 15) {
        brightness = qadd8(brightness, 45);
        hue += random8(32);  // Random hue shift for sparkles
        sat = qsub8(sat, 50);
      }
      
    } else {
      // Top spiral - matching bottom but with phase shift
      uint8_t pos = map(i, TOP_SECTION_START, TOP_SECTION_END, 0, 255);
      
      // Create color bands with phase shift
      hue = (pos * COLOR_BANDS + flowOffset + 128) * BAND_SHARPNESS;  // 128 = half phase
      
      // Complex wave patterns
      uint8_t wave1 = sin8(pos * 3 + flowOffset * 2);
      uint8_t wave2 = cos8(pos * 4 - flowOffset * 3);
      uint8_t wave3 = sin8(pos * 2 + flowOffset);
      brightness = map((wave1 + wave2 + wave3) / 3, 0, 255, MIN_BRIGHTNESS, 255);
    }
    
    // Additional color variation based on time
    hue += sin8(flowOffset * PALETTE_SPEED) / 4;
    
    // Set final LED color
    leds[i] = CHSV(hue, sat, brightness);
  }
  
  // Multiple blur passes for smooth transitions while maintaining definition
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT);
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT/2);
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT/4);
  
  EVERY_N_MILLISECONDS(20) {
    flowOffset += FLOW_SPEED;
  }
  
  FastLED.show();
}