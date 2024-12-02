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

// Wave parameters
#define WAVE_LENGTH 40    // Length of one complete wave
#define WAVE_SPEED 3      // Speed of wave motion
#define MIN_BRIGHTNESS 64 // Minimum brightness (never fully dim)
#define WAVE_RANGE (255 - MIN_BRIGHTNESS)  // Range of brightness variation
uint16_t timeOffset = 0;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  // For each window
  for(int window = 0; window < 2; window++) {
    int windowStart = window * LEDS_PER_WINDOW;
    
    for(int i = 0; i < WINDOW_WIDTH; i++) {
      // Calculate sine wave brightness with minimum level
      uint8_t bottomWave = sin8(i * (256/WAVE_LENGTH) + timeOffset);
      uint8_t topWave = sin8(i * (256/WAVE_LENGTH) + timeOffset + 128); // 180 degrees out of phase
      
      // Map the sine wave to our desired brightness range
      uint8_t bottomBrightness = map(bottomWave, 0, 255, MIN_BRIGHTNESS, 255);
      uint8_t topBrightness = map(topWave, 0, 255, MIN_BRIGHTNESS, 255);
      
      // Bottom row - Blue with varying brightness but never fully dim
      CRGB bottomColor = CRGB(0, 0, 255);  // Solid blue
      bottomColor.nscale8(bottomBrightness);
      leds[windowStart + i] = bottomColor;
      
      // Top row - Red with varying brightness but never fully dim
      CRGB topColor = CRGB(255, 0, 0);     // Solid red
      topColor.nscale8(topBrightness);
      int topLed = windowStart + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - i - 1);
      leds[topLed] = topColor;
    }
  }
  
  timeOffset += WAVE_SPEED;
  FastLED.show();
  delay(20);
}