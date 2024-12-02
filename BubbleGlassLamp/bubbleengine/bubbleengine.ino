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

// Engine effect parameters
#define CORE_PULSE_SPEED 3
#define ENERGY_WAVE_SPEED 2
#define BLUR_AMOUNT 128
#define PULSE_MIN_BRIGHTNESS 100
#define PULSE_MAX_BRIGHTNESS 255
#define ENERGY_RIPPLE_CHANCE 40

// Starburst parameters
#define STARBURST_CHANCE 40    // Chance of new starburst
#define STARBURST_FADE 20      // How quickly bursts fade
#define STARBURST_RAYS 8       // Number of rays in burst
#define MAX_STARBURSTS 3       // Maximum concurrent bursts

// Structure to track active starbursts
struct Starburst {
  uint8_t x;          // X position
  uint8_t y;          // Y position
  uint8_t life;       // Lifetime of burst
  uint8_t intensity;  // Initial brightness
};

Starburst starbursts[MAX_STARBURSTS];

// Color definitions
#define CORE_HUE 160        // Blue core
#define ENERGY_HUE 140      // Cyan energy
#define PULSE_SATURATION 240

uint16_t timeOffset = 0;
uint8_t pulsePhase = 0;

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  // Calculate core pulse
  uint8_t corePulse = beatsin8(CORE_PULSE_SPEED, PULSE_MIN_BRIGHTNESS, PULSE_MAX_BRIGHTNESS, 0, 0);
  
  // Update all LEDs
  for(int i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness;
    uint8_t saturation = PULSE_SATURATION;
    uint8_t hue = CORE_HUE;
    
    if(i < BOTTOM_SPIRAL) {
      // Bottom spiral - energy intake effect
      uint8_t pos = map(i, 0, BOTTOM_SPIRAL, 0, 255);
      uint8_t wave = sin8(pos + timeOffset * 2);
      
      // Create inward flowing effect
      brightness = scale8(wave, 128) + scale8(corePulse, 128);
      
      // Add occasional energy ripples
      if(random8() < ENERGY_RIPPLE_CHANCE) {
        brightness = qadd8(brightness, random8(40));
        saturation = saturation - 40;  // Whiter energy ripples
      }
      
    } else if(i < MIDDLE_CYLINDER_END) {
      // Middle cylinder - core reactor effect with starbursts
      uint8_t x = (i - MIDDLE_CYLINDER_START) % 32;
      uint8_t y = (i - MIDDLE_CYLINDER_START) / 32;
      
      // Base core brightness from pulse
      brightness = corePulse;
      
      // Calculate starburst contributions
      uint8_t burstBrightness = 0;
      
      for(int b = 0; b < MAX_STARBURSTS; b++) {
        if(starbursts[b].life > 0) {
          // Calculate distance from burst center
          int8_t dx = x - starbursts[b].x;
          int8_t dy = y - starbursts[b].y;
          
          // Create ray effect
          uint8_t angle = atan2(dy, dx) * 256 / (2 * PI);
          uint8_t rayEffect = sin8(angle * STARBURST_RAYS);
          
          // Calculate distance and fade
          uint8_t distance = sqrt16((dx * dx) + (dy * dy));
          uint8_t fadeWithDistance = map(distance, 0, 8, starbursts[b].life, 0);
          
          // Add ray brightness
          if(fadeWithDistance > 0) {
            uint8_t rayBrightness = scale8(rayEffect, fadeWithDistance);
            burstBrightness = qadd8(burstBrightness, rayBrightness);
          }
          
          // Update starburst life
          if(b == 0) {  // Only decrease once per frame
            starbursts[b].life = starbursts[b].life > STARBURST_FADE ? 
                                starbursts[b].life - STARBURST_FADE : 0;
          }
        }
      }
      
      // Add starburst brightness to base
      brightness = qadd8(brightness, burstBrightness);
      
      // Chance to create new starburst
      if(random8() < STARBURST_CHANCE) {
        for(int b = 0; b < MAX_STARBURSTS; b++) {
          if(starbursts[b].life == 0) {
            starbursts[b].x = random8(32);  // Random x position
            starbursts[b].y = random8(8);   // Random y position
            starbursts[b].life = 255;       // Full life
            starbursts[b].intensity = random8(128, 255);  // Random intensity
            break;
          }
        }
      }
      
      // Adjust color for bursts
      if(burstBrightness > 0) {
        // Make brighter areas whiter
        saturation = qsub8(saturation, scale8(burstBrightness, 128));
      }
      
    } else {
      // Top spiral - enhanced energy exhaust effect with consistent flashes
      uint8_t pos = map(i, TOP_SECTION_START, TOP_SECTION_END, 0, 255);
      
      // Base wave pattern
      uint8_t wave = sin8(pos - timeOffset * 3);
      
      // Create consistent flash pattern
      uint8_t flashPattern = sin8(timeOffset * 4);  // Faster flash cycle
      uint8_t flashIntensity = map(flashPattern, 0, 255, 60, 180);
      
      // Combine effects for brightness
      brightness = scale8(wave, 64);                // Reduced base wave effect
      brightness = qadd8(brightness, flashIntensity);  // Add flash
      brightness = qadd8(brightness, scale8(corePulse, 64));  // Add some core pulse
      
      // Make color slightly different for exhaust
      hue = ENERGY_HUE;
      
      // Add bright white highlights during peak flash
      if(flashPattern > 200) {
        brightness = qadd8(brightness, 60);  // Extra brightness boost
        saturation = qsub8(saturation, 80);  // More white
      }
      
      // Add random energy sparks
      if(random8() < ENERGY_RIPPLE_CHANCE) {
        brightness = qadd8(brightness, random8(40, 80));
        saturation = qsub8(saturation, 40);  // Whiter sparks
      }
      
      // Ensure minimum brightness
      brightness = qadd8(brightness, 40);  // Keep it visible
    }
    
    // Set final LED color
    leds[i] = CHSV(hue, saturation, brightness);
  }
  
  // Apply blur for smoothness
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT);
  
  // Update timing
  EVERY_N_MILLISECONDS(20) {
    timeOffset += ENERGY_WAVE_SPEED;
  }
  
  FastLED.show();
}