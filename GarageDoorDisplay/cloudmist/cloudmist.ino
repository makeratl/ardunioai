#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    600
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Window configuration
#define WINDOW_WIDTH 130
#define WINDOW_HEIGHT 20
#define TOP_OFFSET 20
#define LEDS_PER_WINDOW 300

CRGB leds[NUM_LEDS];

// Mist parameters
#define MIST_SPEED 2
#define BLEND_SPEED 4
#define NUM_LAYERS 3
#define CLOUD_DENSITY 20

// Cloud movement tracking
uint16_t timeOffsets[NUM_LAYERS];
uint8_t layerHues[NUM_LAYERS];
uint8_t targetHues[NUM_LAYERS];
uint8_t mistDensity[WINDOW_WIDTH];

void setup() {
  delay(2000);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  
  // Initialize layers with different colors and offsets
  for(int i = 0; i < NUM_LAYERS; i++) {
    timeOffsets[i] = random16();
    layerHues[i] = random8();
    targetHues[i] = random8();
  }
}

void loop() {
  EVERY_N_MILLISECONDS(50) {
    // Update layer colors
    for(int i = 0; i < NUM_LAYERS; i++) {
      if(abs(layerHues[i] - targetHues[i]) < BLEND_SPEED) {
        targetHues[i] = random8();
      }
      if(layerHues[i] < targetHues[i]) {
        layerHues[i] += BLEND_SPEED;
      } else if(layerHues[i] > targetHues[i]) {
        layerHues[i] -= BLEND_SPEED;
      }
      timeOffsets[i] += MIST_SPEED * (i + 1);  // Different speeds for each layer
    }
  }

  // For each window
  for(int window = 0; window < 2; window++) {
    int windowStart = window * LEDS_PER_WINDOW;
    
    // Calculate mist density for each column
    for(int x = 0; x < WINDOW_WIDTH; x++) {
      uint8_t density = 0;
      
      // Combine multiple noise layers
      for(int layer = 0; layer < NUM_LAYERS; layer++) {
        uint16_t xoffset = timeOffsets[layer] + (x * 10);
        uint8_t noise = inoise8(xoffset, layer * 1000);
        density += noise / NUM_LAYERS;
      }
      mistDensity[x] = density;
    }

    // Render the mist effect
    for(int x = 0; x < WINDOW_WIDTH; x++) {
      // Create base color from blended layers
      CRGB mistColor = CRGB::Black;
      
      for(int layer = 0; layer < NUM_LAYERS; layer++) {
        CRGB layerColor = CHSV(layerHues[layer], 180, mistDensity[x]);
        mistColor += layerColor.nscale8(255 / NUM_LAYERS);
      }
      
      // Add some sparkle for light catching effects
      if(random8() < CLOUD_DENSITY) {
        mistColor += CRGB(random8(20), random8(20), random8(20));
      }

      // Bottom bar
      for(int y = 0; y < WINDOW_HEIGHT; y++) {
        uint8_t brightness = quadwave8(mistDensity[x] + y * 4);
        CRGB pixelColor = mistColor;
        pixelColor.nscale8(brightness);
        leds[windowStart + x + (y * WINDOW_WIDTH)] = pixelColor;
      }

      // Top bar with slight variation
      for(int y = 0; y < WINDOW_HEIGHT; y++) {
        uint8_t brightness = quadwave8(mistDensity[x] + y * 4 + 128);
        CRGB pixelColor = mistColor;
        pixelColor.nscale8(brightness);
        leds[windowStart + x + ((y + TOP_OFFSET) * WINDOW_WIDTH)] = pixelColor;
      }
    }
  }

  // Add subtle white highlights
  EVERY_N_MILLISECONDS(100) {
    for(int i = 0; i < 5; i++) {
      int pos = random16(NUM_LEDS);
      leds[pos] += CRGB(20, 20, 20);
    }
  }

  FastLED.show();
  delay(20);
} 