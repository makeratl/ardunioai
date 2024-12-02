#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    600
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// Keep the strong variation in colors but reduce the number of transitions
CRGB waterColors[] = {
  CRGB(0, 30, 80),     // Deep blue
  CRGB(0, 60, 120),    // Medium blue
  CRGB(0, 100, 160),   // Light blue
  CRGB(0, 140, 200),   // Bright blue
  CRGB(0, 180, 220)    // Cyan blue
};

// Create smooth transition arrays
uint8_t wave1Values[NUM_LEDS];
uint8_t wave2Values[NUM_LEDS];
uint8_t wave3Values[NUM_LEDS];

void loop() {
  static uint16_t offset = 0;
  static uint16_t offset2 = 0;
  
  // Update wave values with smooth transitions
  for(int i = 0; i < NUM_LEDS; i++) {
    // Smoother wave calculations with different scales
    wave1Values[i] = sin8(((i * 10) + offset) / 2);     // Longer, slower waves
    wave2Values[i] = sin8(((i * 5) + offset2) / 2);     // Very slow background waves
    wave3Values[i] = sin8(((i * 15) - offset) / 2);     // Medium waves
    
    // Combine waves with gentle weighting
    uint16_t combinedWave = ((uint16_t)wave1Values[i] * 3 + 
                            (uint16_t)wave2Values[i] * 2 + 
                            (uint16_t)wave3Values[i]) / 6;
    
    // Smooth color selection
    uint8_t colorIndex = map(combinedWave, 0, 255, 0, sizeof(waterColors)/sizeof(waterColors[0]) - 1);
    
    // Apply color
    leds[i] = waterColors[colorIndex];
    
    // Smooth brightness variation
    uint8_t brightness = 170 + (sin8(combinedWave/2 + offset/8) / 4);
    leds[i].nscale8(brightness);
    
    // Gentle highlights only on wave peaks
    if(combinedWave > 200) {
      leds[i] += CRGB(20, 20, 20);
    }
  }
  
  // Slower, consistent movement
  offset += 2;
  offset2 += 1;
  
  FastLED.show();
  delay(30);  // Slightly slower for smoother transition
}

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  // Initialize wave values
  for(int i = 0; i < NUM_LEDS; i++) {
    wave1Values[i] = 0;
    wave2Values[i] = 0;
    wave3Values[i] = 0;
  }
  
  FastLED.clear();
  FastLED.show();
}