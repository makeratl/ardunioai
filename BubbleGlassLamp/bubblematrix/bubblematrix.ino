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

// Matrix parameters
#define NUM_DROPS 12          // Number of simultaneous drops
#define DROP_SPEED 80         // Lower = faster
#define TRAIL_LENGTH 6        // Length of trailing fade
#define RANDOM_SEED 1337      // For predictable randomness
#define BLUR_AMOUNT 50        // Subtle blur for glow effect

// Color definitions
#define MATRIX_GREEN 96       // Base green hue
#define MATRIX_SATURATION 255 // Full saturation
#define GLOW_BRIGHTNESS 255   // Leading drop brightness
#define MIN_BRIGHTNESS 30     // Minimum trail brightness

// Drop structure
struct Drop {
  int16_t position;    // Current LED position
  uint8_t speed;       // Individual drop speed
  uint8_t brightness;  // Individual brightness variation
};

Drop drops[NUM_DROPS];
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  
  // Initialize drops with random positions
  for(int i = 0; i < NUM_DROPS; i++) {
    drops[i].position = random16(NUM_LEDS);
    drops[i].speed = random8(40, 120);  // Variable speed
    drops[i].brightness = random8(180, 255);
  }
}

void loop() {
  // Fade all LEDs
  fadeToBlackBy(leds, NUM_LEDS, 40);
  
  EVERY_N_MILLISECONDS(DROP_SPEED) {
    // Update each drop
    for(int i = 0; i < NUM_DROPS; i++) {
      // Previous position for trail
      int16_t prevPos = drops[i].position;
      
      // Move drop down
      drops[i].position++;
      
      // Handle wrapping based on section
      if(prevPos < BOTTOM_SPIRAL) {
        if(drops[i].position >= BOTTOM_SPIRAL) {
          drops[i].position = 0;  // Restart at bottom spiral start
        }
      } else if(prevPos < MIDDLE_CYLINDER_END) {
        // Handle cylinder wrapping
        if((prevPos - MIDDLE_CYLINDER_START) % 32 == 31) {
          drops[i].position = prevPos - 31;  // Wrap around cylinder
        }
        if(drops[i].position >= MIDDLE_CYLINDER_END) {
          drops[i].position = MIDDLE_CYLINDER_START;  // Restart at cylinder top
        }
      } else {
        if(drops[i].position >= TOP_SECTION_END) {
          drops[i].position = TOP_SECTION_START;  // Restart at top spiral start
        }
      }
      
      // Draw bright leading drop
      leds[drops[i].position] = CHSV(
        MATRIX_GREEN + random8(10),  // Slight hue variation
        MATRIX_SATURATION,
        drops[i].brightness
      );
      
      // Draw trailing fade
      for(int j = 1; j <= TRAIL_LENGTH; j++) {
        int16_t trailPos = prevPos - j;
        
        // Check section boundaries for trail
        bool validTrail = true;
        if(prevPos < BOTTOM_SPIRAL) {
          validTrail = trailPos >= 0;
        } else if(prevPos < MIDDLE_CYLINDER_END) {
          validTrail = (trailPos >= MIDDLE_CYLINDER_START) && 
                      ((prevPos - MIDDLE_CYLINDER_START) / 32 == 
                       (trailPos - MIDDLE_CYLINDER_START) / 32);
        } else {
          validTrail = trailPos >= TOP_SECTION_START;
        }
        
        if(validTrail) {
          uint8_t trailBrightness = drops[i].brightness / (j + 1);
          trailBrightness = max(trailBrightness, MIN_BRIGHTNESS);
          
          leds[trailPos] = CHSV(
            MATRIX_GREEN + random8(5),  // Subtle hue variation
            MATRIX_SATURATION,
            trailBrightness
          );
        }
      }
      
      // Random brightness flutter
      if(random8() < 30) {
        drops[i].brightness = random8(180, 255);
      }
    }
  }
  
  // Apply blur for glow effect
  blur1d(leds, NUM_LEDS, BLUR_AMOUNT);
  
  FastLED.show();
}