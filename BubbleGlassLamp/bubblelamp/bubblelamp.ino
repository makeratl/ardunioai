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

// Animation parameters
#define SPEED 60
#define COLOR_SPEED 1
#define WAVE_SPEED 1
#define BLUR_AMOUNT 128
#define CENTER_COLOR_VARIATION 96
#define EDGE_COLOR_VARIATION 32
#define MIN_BRIGHTNESS_EDGES 180
#define MIN_BRIGHTNESS_CENTER 100

// Bottom spiral dance parameters
#define DANCE_SPEED 2
#define DANCE_SCALE 3
#define SPARKLE_CHANCE 20

uint16_t timeOffset = 0;
uint8_t hue = 0;
uint16_t danceOffset = 0;

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t pos;
    uint8_t wave1, wave2, wave3;
    uint8_t hueOffset;
    uint8_t minBrightness;
    
    if(i < BOTTOM_SPIRAL) {
      // Enhanced bottom spiral animation
      pos = map(i, 0, BOTTOM_SPIRAL, 0, 255);
      
      // Create multiple overlapping waves for more dynamic movement
      wave1 = sin8(pos/2 + timeOffset/2 + danceOffset);
      wave2 = sin8(pos/3 - timeOffset + danceOffset/2);
      wave3 = cos8(pos/4 + danceOffset/3);
      
      // Add rotational movement
      uint8_t rotationOffset = sin8(pos + danceOffset/2);
      
      // Create "dancing" effect with additional wave patterns
      uint8_t dance1 = sin8(pos * DANCE_SCALE + danceOffset);
      uint8_t dance2 = cos8((255 - pos) * DANCE_SCALE + danceOffset/2);
      
      // Combine waves with dance patterns
      wave1 = qadd8(wave1, dance1);
      wave2 = qadd8(wave2, dance2);
      wave3 = qadd8(wave3, rotationOffset);
      
      hueOffset = map(wave2, 0, 255, 0, EDGE_COLOR_VARIATION);
      minBrightness = MIN_BRIGHTNESS_EDGES;
      
      // Occasional subtle brightness boost for sparkle
      if(random8() < SPARKLE_CHANCE) {
        minBrightness = qadd8(minBrightness, 20);
      }
    }
    else if(i < MIDDLE_CYLINDER_END) {
      // Middle cylinder (unchanged)
      uint8_t x = (i - MIDDLE_CYLINDER_START) % 32;
      uint8_t y = (i - MIDDLE_CYLINDER_START) / 32;
      
      wave1 = sin8(x * 3 + timeOffset/2 + y * 4);
      wave2 = sin8(y * 8 - timeOffset + x * 2);
      wave3 = cos8((x + y * 6) + timeOffset/3);
      
      hueOffset = map(wave2, 0, 255, 0, CENTER_COLOR_VARIATION);
      hueOffset += map(wave3, 0, 255, 0, CENTER_COLOR_VARIATION/2);
      minBrightness = MIN_BRIGHTNESS_CENTER;
    }
    else {
      // Top spiral (unchanged)
      pos = map(i, TOP_SECTION_START, TOP_SECTION_END, 255, 0);
      wave1 = sin8(pos/3 - timeOffset/2);
      wave2 = sin8(pos/4 + timeOffset);
      wave3 = cos8(pos/5 - timeOffset/3);
      hueOffset = map(wave2, 0, 255, 0, EDGE_COLOR_VARIATION);
      minBrightness = MIN_BRIGHTNESS_EDGES;
    }
    
    // Combine waves with brightness floor
    uint8_t waveSum = scale8(wave1 + wave2 + wave3, 75);
    uint8_t brightness = qadd8(waveSum, minBrightness);
    
    // Create color with high brightness for edges
    CHSV pixelColor = CHSV(
      hue + hueOffset + (i / 12),
      200 - (wave3 / 8),
      brightness
    );
    
    leds[i] = pixelColor;
  }
  
  // Double blur pass for smoothness
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT);
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT/2);
  
  EVERY_N_MILLISECONDS(SPEED) {
    timeOffset += WAVE_SPEED;
    danceOffset += DANCE_SPEED;  // Separate movement for dance effect
  }
  
  EVERY_N_MILLISECONDS(150) {
    hue += COLOR_SPEED;
  }
  
  FastLED.show();
}
