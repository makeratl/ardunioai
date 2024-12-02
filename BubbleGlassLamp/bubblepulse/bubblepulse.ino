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

// Pulse parameters
#define PULSE_DURATION 3000    // Total cycle time 3 seconds
#define DARK_DURATION 1000     // Dark period 1 second
#define IGNITE_DURATION 2300   // How long top/bottom sections stay lit
#define PULSE_SPEED 2        
#define MAX_DISTANCE 255     
#define BLUR_AMOUNT 150      

// Color definitions
#define CORE_HUE 160        // Blue core
#define PULSE_HUE 140       // Cyan pulse
#define PULSE_SATURATION 240

uint32_t lastPulseTime = 0;
uint16_t pulseProgress = 0;

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  uint32_t currentTime = millis();
  uint32_t timeInSequence = currentTime % PULSE_DURATION;
  bool isDarkPhase = timeInSequence > (PULSE_DURATION - DARK_DURATION);
  
  // Calculate different phases for center and edges
  uint8_t pulseRadius = 0;
  uint8_t ignitePhase = 0;
  
  if (!isDarkPhase) {
    pulseRadius = map(timeInSequence, 0, PULSE_DURATION - DARK_DURATION, 0, MAX_DISTANCE);
    // Calculate ignite phase (0-255) for top/bottom sections
    ignitePhase = map(timeInSequence, 0, IGNITE_DURATION, 255, 0);
    ignitePhase = min(255, ignitePhase); // Clamp to 255
  }
  
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness;
    uint8_t hue;
    uint8_t sat = PULSE_SATURATION;
    
    if(isDarkPhase) {
      brightness = 10;
      hue = CORE_HUE;
    } else {
      if(i < BOTTOM_SPIRAL) {
        // Bottom spiral - ignite and fade
        uint8_t distFromStart = map(i, 0, BOTTOM_SPIRAL, MAX_DISTANCE, 0);
        
        if(timeInSequence < IGNITE_DURATION) {
          // During ignite phase
          brightness = scale8(ignitePhase, 255 - (distFromStart / 2));
          // Color shift during fade
          hue = lerp8by8(PULSE_HUE, CORE_HUE, 255 - ignitePhase);
        } else {
          // Wait for next pulse
          brightness = 20;
          hue = CORE_HUE;
        }
        
      } else if(i < MIDDLE_CYLINDER_END) {
        // Middle cylinder - maintain original pulse behavior
        uint8_t x = (i - MIDDLE_CYLINDER_START) % 32;
        uint8_t y = (i - MIDDLE_CYLINDER_START) / 32;
        
        uint8_t centerX = 16;
        uint8_t centerY = 4;
        uint8_t distX = abs(x - centerX);
        uint8_t distY = abs(y - centerY) * 4;
        uint8_t distance = sqrt16((distX * distX) + (distY * distY));
        
        int16_t brightnessDrop = abs(distance - pulseRadius);
        brightness = max(0, 255 - brightnessDrop * 2);
        hue = lerp8by8(CORE_HUE, PULSE_HUE, brightness);
        
      } else {
        // Top spiral - ignite and fade (mirror of bottom)
        uint8_t distFromEnd = map(i, TOP_SECTION_START, TOP_SECTION_END, 0, MAX_DISTANCE);
        
        if(timeInSequence < IGNITE_DURATION) {
          // During ignite phase
          brightness = scale8(ignitePhase, 255 - (distFromEnd / 2));
          // Color shift during fade
          hue = lerp8by8(PULSE_HUE, CORE_HUE, 255 - ignitePhase);
        } else {
          // Wait for next pulse
          brightness = 20;
          hue = CORE_HUE;
        }
      }
      
      // Enhanced white core for middle section only
      if(i >= MIDDLE_CYLINDER_START && i < MIDDLE_CYLINDER_END && brightness > 200) {
        sat = qsub8(sat, map(brightness, 200, 255, 0, 200));
      }
    }
    
    // Set final LED color
    leds[i] = CHSV(hue, sat, brightness);
  }
  
  // Apply blur for smoothness
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT);
  
  FastLED.show();
}