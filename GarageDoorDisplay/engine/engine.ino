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
#define WINDOW_CENTER (WINDOW_WIDTH / 2)

// Effect parameters
#define LINE_WIDTH 15
#define FADE_RATE 150
#define SPEED 4

// Background effect parameters
uint8_t hue = 0;
uint16_t timeOffset = 0;
static int position = 0;

// Modify center definition to be the middle of entire display
#define DISPLAY_CENTER (NUM_LEDS / 2)
#define MAX_DISTANCE (WINDOW_WIDTH)  // Maximum distance from center

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void drawBackground() {
  // Create smooth plasma-like background pattern
  for(int window = 0; window < 2; window++) {
    int windowStart = window * LEDS_PER_WINDOW;
    
    for(int i = 0; i < WINDOW_WIDTH; i++) {
      // Create multiple overlapping waves for plasma effect
      uint8_t wave1 = sin8(i * 3 + timeOffset);
      uint8_t wave2 = sin8((i * 5 + timeOffset * 2) / 2);
      uint8_t wave3 = sin8((i * 2 - timeOffset * 3) / 3);
      uint8_t wave4 = cos8(i * 4 + timeOffset / 2);
      
      // Combine waves with different weights for more organic look
      uint16_t combined = (wave1 * 2 + wave2 * 3 + wave3 * 2 + wave4) / 8;
      
      // Map to a deeper red range with subtle variations
      uint8_t redBase = map(combined, 0, 255, 30, 90);
      uint8_t redShade = map(sin8(i * 2 + timeOffset), 0, 255, redBase-10, redBase+10);
      
      // Create plasma-like deep red color with subtle shadows
      CRGB bgColor = CRGB(
        redShade,
        redShade/12 + sin8(timeOffset + i * 3) / 32,  // Subtle variation in green
        redShade/15 + cos8(timeOffset - i * 2) / 32   // Subtle variation in blue
      );
      
      // Bottom bar
      leds[windowStart + i] = bgColor;
      
      // Top bar (mirrored)
      int topLed = windowStart + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - i - 1);
      leds[topLed] = bgColor;
    }
  }
  
  // Smooth background animation
  timeOffset += 3;  // Adjusted for smoother flow
}

void loop() {
  // Draw deep red background first
  drawBackground();
  
  // Calculate positions for energy bars expanding from center
  for(int i = -LINE_WIDTH/2 - 2; i <= LINE_WIDTH/2 + 2; i++) {
    // For box 1 (right box, 0-300) - moving right from center
    int rightBoxPos = WINDOW_WIDTH - position + i - 1;
    // For box 2 (left box, 301-600) - moving left from center
    int leftBoxPos = position + i;
    
    // Process right box (0-300)
    if(rightBoxPos >= 0 && rightBoxPos < WINDOW_WIDTH) {
      // Calculate brightness with reduced pulse effect
      uint8_t baseBrightness = sin8(map(abs(i), 0, LINE_WIDTH/2, 255, 0));
      uint8_t pulseBrightness = sin8(timeOffset * 3);
      // Modified brightness calculation to maintain higher minimum brightness
      uint8_t finalBrightness = map((baseBrightness * pulseBrightness) >> 8, 0, 255, 180, 255);
      
      // Calculate brightness with distance from center effect
      uint8_t distanceFromCenter = abs(position - WINDOW_CENTER);
      uint8_t edgeFade = map(distanceFromCenter, 0, WINDOW_WIDTH/2, 255, 0); // Fade as it reaches edges
      
      // Bottom bar
      int bottomLed = rightBoxPos;  // First window starts at 0
      if(abs(i) > LINE_WIDTH/2) {
        leds[bottomLed] = blend(leds[bottomLed], CRGB(0, 0, 0), 200);
      } else {
        CRGB energyColor;
        if(abs(i) < 2) {
          // White-blue core that fades near edges
          energyColor = CRGB(
            (200 * edgeFade) >> 8,
            (220 * edgeFade) >> 8,
            (255 * edgeFade) >> 8
          );
        } else {
          // Blue energy that fades near edges
          energyColor = CRGB(
            0,
            (180 * edgeFade) >> 8,
            (255 * edgeFade) >> 8
          );
        }
        leds[bottomLed] = blend(leds[bottomLed], energyColor, 250);
      }
      
      // Top bar (mirrored)
      int topLed = WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - rightBoxPos - 1);
      if(abs(i) > LINE_WIDTH/2) {
        leds[topLed] = blend(leds[topLed], CRGB(0, 0, 0), 200);
      } else {
        CRGB energyColor;
        if(abs(i) < 2) {
          // White-blue core that fades near edges
          energyColor = CRGB(
            (200 * edgeFade) >> 8,
            (220 * edgeFade) >> 8,
            (255 * edgeFade) >> 8
          );
        } else {
          // Blue energy that fades near edges
          energyColor = CRGB(
            0,
            (180 * edgeFade) >> 8,
            (255 * edgeFade) >> 8
          );
        }
        leds[topLed] = blend(leds[topLed], energyColor, 250);
      }
    }
    
    // Process left box (301-600)
    if(leftBoxPos >= 0 && leftBoxPos < WINDOW_WIDTH) {
      // Calculate brightness with reduced pulse effect
      uint8_t baseBrightness = sin8(map(abs(i), 0, LINE_WIDTH/2, 255, 0));
      uint8_t pulseBrightness = sin8(timeOffset * 3);
      // Modified brightness calculation to maintain higher minimum brightness
      uint8_t finalBrightness = map((baseBrightness * pulseBrightness) >> 8, 0, 255, 180, 255);
      
      // Calculate brightness with distance from center effect
      uint8_t distanceFromCenter = abs(position - WINDOW_CENTER);
      uint8_t edgeFade = map(distanceFromCenter, 0, WINDOW_WIDTH/2, 255, 0); // Fade as it reaches edges
      
      // Bottom bar
      int bottomLed = LEDS_PER_WINDOW + leftBoxPos;  // Second window starts at 300
      if(abs(i) > LINE_WIDTH/2) {
        leds[bottomLed] = blend(leds[bottomLed], CRGB(0, 0, 0), 200);
      } else {
        CRGB energyColor;
        if(abs(i) < 2) {
          // White-blue core that fades near edges
          energyColor = CRGB(
            (200 * edgeFade) >> 8,
            (220 * edgeFade) >> 8,
            (255 * edgeFade) >> 8
          );
        } else {
          // Blue energy that fades near edges
          energyColor = CRGB(
            0,
            (180 * edgeFade) >> 8,
            (255 * edgeFade) >> 8
          );
        }
        leds[bottomLed] = blend(leds[bottomLed], energyColor, 250);
      }
      
      // Top bar (mirrored)
      int topLed = LEDS_PER_WINDOW + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - leftBoxPos - 1);
      if(abs(i) > LINE_WIDTH/2) {
        leds[topLed] = blend(leds[topLed], CRGB(0, 0, 0), 200);
      } else {
        CRGB energyColor;
        if(abs(i) < 2) {
          // White-blue core that fades near edges
          energyColor = CRGB(
            (200 * edgeFade) >> 8,
            (220 * edgeFade) >> 8,
            (255 * edgeFade) >> 8
          );
        } else {
          // Blue energy that fades near edges
          energyColor = CRGB(
            0,
            (180 * edgeFade) >> 8,
            (255 * edgeFade) >> 8
          );
        }
        leds[topLed] = blend(leds[topLed], energyColor, 250);
      }
    }
  }
  
  // Update position for energy bars - faster outward movement
  position += 2;  // Move 2 pixels at a time for faster motion
  if(position >= WINDOW_WIDTH) {
    position = 0;  // Reset to center
  }
  
  FastLED.show();
  delay(SPEED);
}