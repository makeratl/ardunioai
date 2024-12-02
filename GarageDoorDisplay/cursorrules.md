# GarageDoorDisplay LED Configuration

## Hardware Configuration
- **LED Type:** WS2812B Addressable LEDs
- **Data Pin:** 6
- **Total LEDs:** 600
- **Color Order:** GRB
- **Default Brightness:** 128 (50%)

## Physical Structure
The display consists of two identical windows, each with:

1. **Window Dimensions**
   - Width: 130 LEDs
   - Height: 20 LEDs
   - LEDs per Window: 300
   - Top Offset: 20 LEDs between bars

2. **Layout Per Window**
   - Bottom Bar: 130 LEDs (0-129)
   - Top Bar: 130 LEDs (150-279)
   - Mirror Effect: Top bar animations are typically mirrored from bottom

## Animation Capabilities
The display supports multiple animation modes:

1. **Cylon** (`cylon.ino`)
   - Classic Battlestar Galactica scanner effect
   - Moving red light with white core
   - Synchronized mirrored movement in top/bottom bars
   - Smooth fade trails

2. **Engine** (`engine.ino`)
   - Energy core simulation with pulsing effects
   - Blue-white core with energy ripples
   - Deep red background patterns
   - Distance-based edge fading

3. **Fire** (`fire.ino`)
   - Realistic fire simulation using heat mapping
   - Custom fire color palette
   - Enhanced bottom edge effects
   - Random spark generation

4. **Funk** (`funk.ino`)
   - Smooth color flow patterns
   - Complementary color schemes
   - Gentle flowing overlay effects
   - Custom color palette transitions

5. **Matrix** (`matrix.ino`)
   - Digital rain effect
   - Multiple simultaneous streams
   - Variable speed drops
   - Brightness trails

6. **Wave** (`wave.ino`)
   - Simple wave patterns
   - Contrasting colors (red/blue)
   - Phase-shifted animations
   - Minimum brightness threshold

7. **WaveSpect** (`wavespect.ino`)
   - Advanced wave patterns with color transitions
   - Complementary color pairs
   - Smooth color transitions
   - Configurable wave parameters

8. **Water** (`water.ino`)
   - Realistic water simulation
   - Multiple overlapping wave patterns
   - Custom water color palette
   - Smooth transitions and highlights

## Development Guidelines
1. **Window Synchronization**
   - Each animation must handle both windows (0-299 and 300-599)
   - Maintain symmetry between windows
   - Consider mirroring effects between top and bottom bars

2. **Performance Considerations**
   - Use efficient LED updates
   - Implement appropriate delays
   - Consider memory usage for arrays and patterns
   - Use FastLED's built-in functions for color manipulation

3. **Pattern Development**
   - Use defined constants for window dimensions
   - Handle transitions between bars
   - Implement proper fading and blending
   - Consider mirrored animations for top bars

4. **Animation Guidelines**
   - Maintain consistent brightness levels
   - Use smooth transitions
   - Consider the physical layout when designing patterns
   - Implement proper timing controls

## Future Development
- Add sound reactivity
- Implement weather-based animations
- Create holiday-themed patterns
- Add wireless control capabilities
- Develop interactive modes
- Implement synchronized patterns across multiple displays 