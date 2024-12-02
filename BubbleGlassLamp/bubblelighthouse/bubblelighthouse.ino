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

// Lighthouse parameters
#define ROTATION_SPEED 1
#define BEAM_WIDTH 8
#define GLOW_SPEED 2
#define PATTERN_SPEED 1
#define BLUR_AMOUNT 180
#define SPARKLE_CHANCE 20

// Color definitions
#define BASE_HUE_BLUE 160
#define BASE_HUE_PURPLE 192
#define BASE_HUE_RED 0
#define YELLOW_SPARKLE_HUE 32

// Additional pattern parameters
#define PATTERN_SCALE 2
#define COLOR_BLEND_SPEED 3
#define PATTERN_COMPLEXITY 3

// Add these new pattern definitions
#define PATTERN_SIZE 8      // Size of X and O patterns (8x8)
#define NUM_PATTERNS 4      // Number of patterns across cylinder (32/8)
#define PATTERN_CHANGE_SPEED 1

// Add these rhythm parameters
#define PULSE_SPEED 8       // Speed of the central pulse
#define PULSE_MIN 60        // Minimum brightness of pulse
#define PULSE_MAX 255       // Maximum brightness of pulse
#define CENTER_CONTRAST 128  // How different the center color is

uint16_t rotationOffset = 0;
uint8_t glowPhase = 0;

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  // Calculate beam position
  uint8_t beamPos = (rotationOffset / 8) % 32;  // Position around cylinder
  uint8_t beamGlow = sin8(glowPhase);  // Breathing effect
  
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness;
    uint8_t hue;
    uint8_t sat = 255;  // Full saturation for rich colors
    
    if(i < BOTTOM_SPIRAL) {
      // Bottom spiral - deep blue base with purple waves
      uint8_t pos = map(i, 0, BOTTOM_SPIRAL, 0, 255);
      
      // Alternate between blue and purple
      hue = lerp8by8(BASE_HUE_BLUE, BASE_HUE_PURPLE, 
                     sin8(pos * 2 + rotationOffset));
      
      // Add wave patterns
      uint8_t wave = sin8(pos * 3 + rotationOffset);
      brightness = map(wave, 0, 255, 40, 150);  // Keep it moody
      
      // Yellow sparkles
      if(random8() < SPARKLE_CHANCE/2) {
        hue = YELLOW_SPARKLE_HUE;
        brightness = 200;
        sat = 180;  // Slightly desaturated for golden look
      }
      
    } else if(i < MIDDLE_CYLINDER_END) {
      // Middle cylinder with pulsing center band
      uint8_t x = (i - MIDDLE_CYLINDER_START) % 32;
      uint8_t y = (i - MIDDLE_CYLINDER_START) / 32;
      
      // Calculate base pattern and color
      uint8_t patternBlock = x / 8;
      uint8_t localX = x % 8;
      uint8_t localY = y;
      
      // Calculate base color phase
      uint8_t colorPhase = rotationOffset + (patternBlock * 64);
      uint8_t colorBlend = sin8(colorPhase * COLOR_BLEND_SPEED);
      
      // Base color selection
      if(colorPhase % 768 < 256) {
        hue = lerp8by8(BASE_HUE_BLUE, BASE_HUE_PURPLE, colorBlend);
      } else if(colorPhase % 768 < 512) {
        hue = lerp8by8(BASE_HUE_PURPLE, BASE_HUE_RED, colorBlend);
      } else {
        hue = lerp8by8(BASE_HUE_RED, BASE_HUE_BLUE, colorBlend);
      }
      
      // Create pulsing effect for center bands (y = 3,4)
      uint8_t pulseVal = beatsin8(PULSE_SPEED, PULSE_MIN, PULSE_MAX, 0, 0);
      
      // Check if we're in the center bands
      if(y == 3 || y == 4) {
        // Contrast color for center (offset by 128 in color wheel)
        hue += CENTER_CONTRAST;
        
        // Enhanced brightness for pulse
        brightness = pulseVal;
        
        // Make center more vibrant
        sat = 255;
        
        // Add sparkle during peak pulse
        if(pulseVal > 200 && random8() < SPARKLE_CHANCE) {
          brightness = 255;
          sat = 180;
        }
      } else {
        // Pattern logic for outer rows
        uint8_t patternType = ((patternBlock + (rotationOffset / 64)) % 2);
        uint8_t patternMult = 0;
        
        if(patternType == 0) {
          // X pattern
          if(abs(localX - localY) <= 1 || abs(localX - (7-localY)) <= 1) {
            patternMult = 255;
          }
        } else {
          // O pattern
          uint8_t centerX = 3;
          uint8_t centerY = 3;
          uint8_t distX = abs(localX - centerX);
          uint8_t distY = abs(localY - centerY);
          float dist = sqrt(distX * distX + distY * distY);
          
          if(dist >= 2.5 && dist <= 3.5) {
            patternMult = 255;
          }
        }
        
        // Set brightness based on pattern
        brightness = scale8(patternMult, 160); // Slightly dimmer outer rows
        
        // Add base glow to non-pattern areas
        if(brightness < 40) brightness = 40;
        
        // Subtle pulse influence on outer rows
        brightness = scale8(brightness, map(pulseVal, PULSE_MIN, PULSE_MAX, 180, 255));
      }
      
      // Add subtle sparkles
      if(random8() < SPARKLE_CHANCE/4) {
        brightness = qadd8(brightness, 30);
        sat = qsub8(sat, 30);
      }
    } else {
      // Top spiral - intense beam source
      uint8_t pos = map(i, TOP_SECTION_START, TOP_SECTION_END, 0, 255);
      
      // Pulsing red/yellow crown
      hue = lerp8by8(BASE_HUE_RED, YELLOW_SPARKLE_HUE, sin8(rotationOffset * 2));
      brightness = map(sin8(pos * 2 + rotationOffset), 0, 255, 150, 255);
      sat = 200;  // Slightly desaturated for intensity
      
      // Extra sparkles
      if(random8() < SPARKLE_CHANCE) {
        brightness = 255;
        sat = 150;  // More white
      }
    }
    
    // Set final LED color
    leds[i] = CHSV(hue, sat, brightness);
  }
  
  // Apply blur for smoothness
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT);
  
  // Update rotation and glow
  EVERY_N_MILLISECONDS(20) {
    rotationOffset += ROTATION_SPEED;
    glowPhase += GLOW_SPEED;
  }
  
  FastLED.show();
}