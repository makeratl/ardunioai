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
#define LINE_WIDTH 15
#define FADE_RATE 150
#define SPEED 20

// Background effect parameters
uint8_t hue = 0;
uint16_t timeOffset = 0;
static int position = 0;
static bool direction = true;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void drawBackground() {
  // Create psychedelic background pattern
  for(int window = 0; window < 2; window++) {
    int windowStart = window * LEDS_PER_WINDOW;
    
    for(int i = 0; i < WINDOW_WIDTH; i++) {
      // Create flowing patterns using multiple sine waves
      uint8_t wave1 = sin8(i * 3 + timeOffset);
      uint8_t wave2 = sin8(i * 5 - timeOffset * 2);
      uint8_t wave3 = cos8(i * 4 + timeOffset / 2);
      
      // Combine waves for color selection
      uint8_t colorIndex = (wave1 + wave2 + wave3) / 3;
      
      // Create background color with reduced brightness
      CRGB bgColor = CHSV(colorIndex + hue, 255, 100); // Lower brightness (100 instead of 255)
      
      // Bottom bar
      leds[windowStart + i] = bgColor;
      
      // Top bar (mirrored)
      int topLed = windowStart + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - i - 1);
      leds[topLed] = bgColor;
    }
  }
  
  // Update background animation
  timeOffset++;
  hue++;
}

void loop() {
  // Draw psychedelic background first
  drawBackground();
  
  // First window (right to left)
  for(int i = -LINE_WIDTH/2 - 2; i <= LINE_WIDTH/2 + 2; i++) {  // Wider loop for shadow
    int pos = WINDOW_WIDTH - position + i - 1;  // Start from right
    
    if(pos >= 0 && pos < WINDOW_WIDTH) {
      // Bottom bar
      int bottomLed = pos;
      uint8_t brightness = sin8(map(abs(i), 0, LINE_WIDTH/2, 255, 0));
      
      // Create shadow effect
      if(abs(i) > LINE_WIDTH/2) {
        // Outer shadow
        leds[bottomLed] = blend(leds[bottomLed], CRGB(0, 0, 0), 200);
      } else {
        // Blue line with alpha blending
        leds[bottomLed] = blend(leds[bottomLed], CRGB(0, 0, brightness), 200);
        if(i == 0) leds[bottomLed] = blend(leds[bottomLed], CRGB(20, 20, 50), 200);
      }
      
      // Top bar (mirrored)
      int topLed = WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - pos - 1);
      if(abs(i) > LINE_WIDTH/2) {
        // Outer shadow
        leds[topLed] = blend(leds[topLed], CRGB(0, 0, 0), 200);
      } else {
        leds[topLed] = blend(leds[topLed], CRGB(0, 0, brightness), 200);
        if(i == 0) leds[topLed] = blend(leds[topLed], CRGB(20, 20, 50), 200);
      }
    }
  }
  
  // Second window (left to right)
  for(int i = -LINE_WIDTH/2 - 2; i <= LINE_WIDTH/2 + 2; i++) {  // Wider loop for shadow
    int pos = position + i;  // Start from left
    
    if(pos >= 0 && pos < WINDOW_WIDTH) {
      // Bottom bar
      int bottomLed = LEDS_PER_WINDOW + pos;
      uint8_t brightness = sin8(map(abs(i), 0, LINE_WIDTH/2, 255, 0));
      
      // Create shadow effect
      if(abs(i) > LINE_WIDTH/2) {
        // Outer shadow
        leds[bottomLed] = blend(leds[bottomLed], CRGB(0, 0, 0), 200);
      } else {
        leds[bottomLed] = blend(leds[bottomLed], CRGB(0, 0, brightness), 200);
        if(i == 0) leds[bottomLed] = blend(leds[bottomLed], CRGB(20, 20, 50), 200);
      }
      
      // Top bar (mirrored)
      int topLed = LEDS_PER_WINDOW + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - pos - 1);
      if(abs(i) > LINE_WIDTH/2) {
        // Outer shadow
        leds[topLed] = blend(leds[topLed], CRGB(0, 0, 0), 200);
      } else {
        leds[topLed] = blend(leds[topLed], CRGB(0, 0, brightness), 200);
        if(i == 0) leds[topLed] = blend(leds[topLed], CRGB(20, 20, 50), 200);
      }
    }
  }
  
  // Update position for blue lines
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