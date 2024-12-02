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

// Matrix effect parameters
#define NUM_STREAMS 15     // Number of simultaneous falling streams
#define MIN_SPEED 2        // Minimum speed of drops
#define MAX_SPEED 5        // Maximum speed of drops

// RainDrop structure to track each falling light
struct RainDrop {
  int position;           // Current position
  int speed;             // How fast this drop falls
  uint8_t brightness;    // Brightness of the head
  int length;            // Length of the trail
};

RainDrop drops[NUM_STREAMS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  
  // Initialize drops at random positions
  for(int i = 0; i < NUM_STREAMS; i++) {
    initDrop(drops[i]);
  }
}

void initDrop(RainDrop &drop) {
  drop.position = random16(WINDOW_WIDTH);
  drop.speed = random8(MIN_SPEED, MAX_SPEED);
  drop.brightness = random8(180, 255);
  drop.length = random8(3, 8);
}

void loop() {
  // Fade all LEDs slightly
  fadeToBlackBy(leds, NUM_LEDS, 40);
  
  // Process both windows
  for(int window = 0; window < 2; window++) {
    int windowStart = window * LEDS_PER_WINDOW;
    
    // Update each drop
    for(int i = 0; i < NUM_STREAMS; i++) {
      RainDrop &drop = drops[i];
      
      // Calculate vertical positions for the drop
      for(int j = 0; j < drop.length; j++) {
        int yPos = (drop.position - j) % WINDOW_HEIGHT;
        if(yPos < 0) yPos += WINDOW_HEIGHT;
        
        // Calculate LED positions for both bottom and top sections
        int bottomLed = windowStart + drop.position;
        int topLed = windowStart + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - drop.position - 1);
        
        // Set colors with fade trail
        uint8_t trailBrightness = drop.brightness * (drop.length - j) / drop.length;
        
        // Matrix green with slight variation
        CRGB color = CRGB(0, trailBrightness, 0);
        if(j == 0) {  // Head of drop is brighter and whiter
          color += CRGB(trailBrightness/3, trailBrightness/2, trailBrightness/3);
        }
        
        // Apply to both bottom and top (mirrored)
        if(drop.position >= 0 && drop.position < WINDOW_WIDTH) {
          leds[bottomLed] = color;
          leds[topLed] = color;
        }
      }
      
      // Move drop down
      if(random8() < drop.speed) {
        drop.position++;
        if(drop.position >= WINDOW_WIDTH) {
          initDrop(drop);  // Reset drop at random position
        }
      }
      
      // Randomly adjust brightness
      if(random8() < 50) {
        drop.brightness = qadd8(drop.brightness, random8(20) - 10);
      }
    }
  }
  
  FastLED.show();
  delay(30);
}