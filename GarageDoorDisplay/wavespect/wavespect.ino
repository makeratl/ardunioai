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

// Wave parameters
#define WAVE_LENGTH 40
#define WAVE_SPEED 3
#define MIN_BRIGHTNESS 64
#define WAVE_RANGE (255 - MIN_BRIGHTNESS)
#define COLOR_CHANGE_SPEED 1  // How fast colors transition
#define COLOR_HOLD_TIME 500   // How long to hold each color pair

uint16_t timeOffset = 0;
uint8_t currentHue = 0;       // Current base hue
uint8_t targetHue = 0;        // Target hue to transition to
uint16_t colorTimer = 0;      // Timer for color changes
bool transitioning = false;    // Are we currently transitioning colors?

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  
  // Initialize first random color
  currentHue = random8();
  targetHue = currentHue;
}

void loop() {
  // Handle color transitions
  colorTimer++;
  if (!transitioning && colorTimer >= COLOR_HOLD_TIME) {
    // Time to pick a new target color
    targetHue = random8();
    transitioning = true;
    colorTimer = 0;
  }
  
  // Gradually transition to target hue
  if (transitioning) {
    if (abs(currentHue - targetHue) > COLOR_CHANGE_SPEED) {
      if (currentHue < targetHue) {
        currentHue += COLOR_CHANGE_SPEED;
      } else {
        currentHue -= COLOR_CHANGE_SPEED;
      }
    } else {
      currentHue = targetHue;
      transitioning = false;
      colorTimer = 0;
    }
  }
  
  // Calculate complementary hue
  uint8_t complementHue = currentHue + 128; // Opposite on color wheel
  
  // Convert hues to RGB colors
  CRGB baseColor = CHSV(currentHue, 255, 255);
  CRGB complementColor = CHSV(complementHue, 255, 255);
  
  // For each window
  for(int window = 0; window < 2; window++) {
    int windowStart = window * LEDS_PER_WINDOW;
    
    for(int i = 0; i < WINDOW_WIDTH; i++) {
      // Calculate sine wave brightness with minimum level
      uint8_t bottomWave = sin8(i * (256/WAVE_LENGTH) + timeOffset);
      uint8_t topWave = sin8(i * (256/WAVE_LENGTH) + timeOffset + 128);
      
      // Map the sine wave to our desired brightness range
      uint8_t bottomBrightness = map(bottomWave, 0, 255, MIN_BRIGHTNESS, 255);
      uint8_t topBrightness = map(topWave, 0, 255, MIN_BRIGHTNESS, 255);
      
      // Bottom row with varying brightness
      CRGB bottomColorScaled = baseColor;
      bottomColorScaled.nscale8(bottomBrightness);
      leds[windowStart + i] = bottomColorScaled;
      
      // Top row with varying brightness
      CRGB topColorScaled = complementColor;
      topColorScaled.nscale8(topBrightness);
      int topLed = windowStart + WINDOW_WIDTH + TOP_OFFSET + (WINDOW_WIDTH - i - 1);
      leds[topLed] = topColorScaled;
    }
  }
  
  timeOffset += WAVE_SPEED;
  FastLED.show();
  delay(20);
}