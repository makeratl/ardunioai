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

// Effect parameters
#define CYLON_WIDTH 15
#define FADE_RATE   150
#define SPEED       20

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  static int position = 0;
  static bool direction = true;  // true = right, false = left
  
  // Fade existing LEDs
  fadeToBlackBy(leds, NUM_LEDS, FADE_RATE);
  
  // Process both windows
  for(int window = 0; window < 2; window++) {
    int windowStart = window * LEDS_PER_WINDOW;
    
    // Create the moving light effect
    for(int i = -CYLON_WIDTH/2; i <= CYLON_WIDTH/2; i++) {
      int pos = position + i;
      
      if(pos >= 0 && pos < WINDOW_WIDTH) {
        // Bottom bar - moving left to right
        int bottomLed = windowStart + pos;
        uint8_t brightness = sin8(map(abs(i), 0, CYLON_WIDTH/2, 255, 0));
        leds[bottomLed] = CRGB(brightness, 0, 0);
        if(i == 0) leds[bottomLed] += CRGB(50, 50, 50);
        
        // Top bar - moving right to left (mirrored with offset)
        // Adjusted calculation to align with bottom
        int topLed = windowStart + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - pos - 1);
        leds[topLed] = CRGB(brightness, 0, 0);
        if(i == 0) leds[topLed] += CRGB(50, 50, 50);
      }
    }
  }
  
  // Update position
  if(direction) {
    position++;
    if(position >= WINDOW_WIDTH) {
      direction = false;
      position = WINDOW_WIDTH - 1;
    }
  } else {
    position--;
    if(position < 0) {
      direction = true;
      position = 0;
    }
  }
  
  FastLED.show();
  delay(SPEED);
}