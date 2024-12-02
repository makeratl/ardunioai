#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    497
#define BRIGHTNESS  200
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Define sections
#define BOTTOM_SPIRAL 181
#define MIDDLE_CYLINDER_START 181
#define MIDDLE_CYLINDER_END 437
#define TOP_SECTION_START 437
#define TOP_SECTION_END 497

// Ocean parameters - adjusted for ultra-smooth feel
#define NUM_BUBBLES 3        // Even fewer bubbles
#define BUBBLE_SPEED 200     // Much slower bubbles
#define WAVE_SPEED 1         // Very slow water movement
#define GLOW_SPEED 1         // Very slow ambient changes
#define BLUR_AMOUNT 250      // Maximum blur for smoothness

// Color definitions - adjusted for surface light effect
#define DEEP_BLUE 170        // Deep blue base
#define SURFACE_BLUE 155     // Lighter blue for top
#define WATER_SATURATION 180 // Base saturation
#define SURFACE_SATURATION 140 // Less saturated at top for sunlit effect
#define BRIGHTNESS_BASE 40   // Deep water brightness
#define SURFACE_BRIGHTNESS 180 // Bright surface water
#define BRIGHTNESS_VARIANCE 20 // Reduced variance for smoother effect

// Bubble structure
struct Bubble {
  int16_t position;
  uint8_t size;
  uint8_t brightness;
  uint8_t speed;
};

Bubble bubbles[NUM_BUBBLES];
CRGB leds[NUM_LEDS];

uint16_t waveOffset = 0;
uint8_t glowPhase = 0;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  
  // Initialize bubbles
  for(int i = 0; i < NUM_BUBBLES; i++) {
    bubbles[i].position = random16(BOTTOM_SPIRAL);  // Start in bottom section
    bubbles[i].size = random8(2, 5);               // Random bubble size
    bubbles[i].brightness = random8(180, 255);     // Random brightness
    bubbles[i].speed = random8(1, 4);             // Random rise speed
  }
}

void loop() {
  fadeToBlackBy(leds, NUM_LEDS, 10);
  
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness;
    uint8_t hue = DEEP_BLUE;
    uint8_t sat = WATER_SATURATION;
    
    // Calculate smooth gradient position
    float gradientPos;
    if(i < BOTTOM_SPIRAL) {
      gradientPos = (float)i / BOTTOM_SPIRAL;
      // Deep water settings
      brightness = BRIGHTNESS_BASE + (sin8(millis() / 50 + gradientPos * 255) * BRIGHTNESS_VARIANCE / 255);
      
    } else if(i < MIDDLE_CYLINDER_END) {
      gradientPos = (float)(i - MIDDLE_CYLINDER_START) / (MIDDLE_CYLINDER_END - MIDDLE_CYLINDER_START);
      // Mid-water transition
      brightness = map(gradientPos, 0, 1, BRIGHTNESS_BASE + 20, BRIGHTNESS_BASE + 60);
      brightness += (sin8(millis() / 50 + gradientPos * 255) * BRIGHTNESS_VARIANCE / 255);
      
    } else {
      // Surface water effect
      gradientPos = (float)(i - TOP_SECTION_START) / (TOP_SECTION_END - TOP_SECTION_START);
      
      // Transition to surface colors
      hue = lerp8by8(DEEP_BLUE, SURFACE_BLUE, gradientPos * 255);
      sat = lerp8by8(WATER_SATURATION, SURFACE_SATURATION, gradientPos * 255);
      
      // Bright surface water with subtle movement
      brightness = lerp8by8(BRIGHTNESS_BASE + 60, SURFACE_BRIGHTNESS, gradientPos * 255);
      brightness += (sin8(millis() / 40 + gradientPos * 255) * BRIGHTNESS_VARIANCE / 255);
      
      // Add shimmer to the very top
      if(i > TOP_SECTION_END - 20) {
        uint8_t shimmer = sin8(millis() / 30 + random8());
        brightness += (shimmer * (i - (TOP_SECTION_END - 20)) / 20);
      }
    }
    
    // Add very subtle ambient glow
    float glowVal = sin8(glowPhase + (gradientPos * 128)) / 255.0;
    brightness += glowVal * 3;  // Even more subtle glow variation
    
    // Set final color
    leds[i] = CHSV(hue, sat, brightness);
  }
  
  // Update and draw bubbles with much softer effect
  EVERY_N_MILLISECONDS(BUBBLE_SPEED) {
    for(int i = 0; i < NUM_BUBBLES; i++) {
      int16_t prevPos = bubbles[i].position;
      bubbles[i].position += bubbles[i].speed;
      
      // Handle section transitions and wrapping
      if(prevPos < BOTTOM_SPIRAL) {
        if(bubbles[i].position >= BOTTOM_SPIRAL) {
          bubbles[i].position = MIDDLE_CYLINDER_START + random8(32);
        }
      } else if(prevPos < MIDDLE_CYLINDER_END) {
        if((prevPos - MIDDLE_CYLINDER_START) % 32 == 31) {
          bubbles[i].position = prevPos + 1 - 32;  // Wrap around cylinder
        }
        if(bubbles[i].position >= MIDDLE_CYLINDER_END) {
          bubbles[i].position = TOP_SECTION_START;
        }
      } else {
        if(bubbles[i].position >= TOP_SECTION_END) {
          bubbles[i].position = random16(BOTTOM_SPIRAL/2);  // Reset to bottom
          bubbles[i].brightness = random8(180, 255);
          bubbles[i].speed = random8(1, 4);
        }
      }
      
      // Adjust bubble visibility based on depth
      uint8_t baseBrightness = bubbles[i].brightness;
      if(bubbles[i].position >= TOP_SECTION_START) {
        // Make bubbles more visible in bright water
        baseBrightness = scale8(baseBrightness, 180);
      }
      
      // Draw bubble with depth-aware brightness
      for(int j = -(bubbles[i].size * 2); j <= (bubbles[i].size * 2); j++) {
        int16_t glowPos = bubbles[i].position + j;
        if(glowPos >= 0 && glowPos < NUM_LEDS) {
          float distance = abs(j) / (float)(bubbles[i].size * 2);
          uint8_t glowBrightness = cubicwave8(constrain((1.0 - distance) * 255, 0, 255));
          glowBrightness = scale8(glowBrightness, baseBrightness / 4);
          
          // Fixed bubble color definition with its own hue and saturation
          CRGB bubbleColor = CHSV(
            DEEP_BLUE - 5,    // Slightly shifted from base water color
            WATER_SATURATION - 40,  // Less saturated than water
            glowBrightness
          );
          leds[glowPos] += bubbleColor;
        }
      }
    }
  }
  
  // Very slow wave and glow movement
  EVERY_N_MILLISECONDS(100) {  // Even slower update rate
    waveOffset += WAVE_SPEED;
    glowPhase += GLOW_SPEED;
  }
  
  // Multiple blur passes for smooth effect
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT);
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT/2);
  
  FastLED.show();
}