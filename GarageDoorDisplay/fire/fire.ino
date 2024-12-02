#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    600
#define BRIGHTNESS  128
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// Window dimensions from previous mapping
#define WINDOW_HEIGHT 20
#define LEDS_PER_WINDOW 300

// Fire color palette
DEFINE_GRADIENT_PALETTE(fireColors_gp) {
    0,     0,  0,  0,      // black
   32,   255,  0,  0,      // red
  128,   255, 80,  0,      // orange
  200,   255,130,  0,      // bright orange
  255,   255,200, 30       // yellow
};

CRGBPalette16 firePalette = fireColors_gp;

// Heat array for fire simulation
uint8_t heat[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  
  // Initialize heat array
  memset(heat, 0, NUM_LEDS);
}

void loop() {
  // Update each window's fire effect
  fireWindow(0, LEDS_PER_WINDOW);                  // First window
  fireWindow(LEDS_PER_WINDOW, LEDS_PER_WINDOW);   // Second window
  
  FastLED.show();
  delay(15);  // Adjust speed of fire
}

void fireWindow(int startPos, int length) {
  // Cool down every cell a little
  for(int i = startPos; i < startPos + length; i++) {
    heat[i] = qsub8(heat[i], random8(0, 2));
  }
  
  // Heat from each cell drifts up and diffuses
  for(int k = startPos + length - 1; k >= startPos + 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  
  // Randomly ignite new sparks at the bottom
  if(random8() < 120) {
    int y = startPos;
    heat[y] = qadd8(heat[y], random8(160, 255));
  }
  
  // Convert heat to LED colors
  for(int j = startPos; j < startPos + length; j++) {
    // Scale heat value to LED brightness
    uint8_t colorindex = scale8(heat[j], 240);
    
    // Increase intensity for bottom LEDs
    if(j < startPos + WINDOW_HEIGHT || j > (startPos + length - WINDOW_HEIGHT)) {
      colorindex = qadd8(colorindex, 30);
    }
    
    // Map heat to color
    CRGB color = ColorFromPalette(firePalette, colorindex);
    
    // Add extra yellow/white for intense heat
    if(heat[j] > 200) {
      color += CRGB(50, 20, 0);
    }
    
    leds[j] = color;
  }
  
  // Add random flickering to the vertical sides
  for(int i = 0; i < WINDOW_HEIGHT; i++) {
    if(random8() < 60) {
      int rightSide = startPos + length - i - 1;
      int leftSide = startPos + i;
      leds[rightSide] += CRGB(random8(50), random8(20), 0);
      leds[leftSide] += CRGB(random8(50), random8(20), 0);
    }
  }
}