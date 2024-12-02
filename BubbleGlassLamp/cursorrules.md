# BubbleGlassLamp LED Configuration

## Hardware Configuration
- **LED Type:** WS2812B Addressable LEDs
- **Data Pin:** 6
- **Total LEDs:** 497
- **Color Order:** GRB
- **Default Brightness:** 128 (50%)

## Physical Structure
The lamp is divided into three distinct sections:

1. **Bottom Spiral** (LEDs 0-180)
   - Contains 181 LEDs
   - Arranged in a spiral pattern
   - Primary zone for upward animations
   - Used for intake effects and base patterns

2. **Middle Cylinder** (LEDs 181-436)
   - Contains 256 LEDs
   - Arranged in a 32x8 grid pattern
   - 32 LEDs per revolution
   - 8 rows in height
   - Main display area for patterns and animations

3. **Top Section** (LEDs 437-496)
   - Contains 60 LEDs
   - Forms the crown/top spiral
   - Used for special effects and pattern completions

## Animation Capabilities
The lamp supports multiple animation modes:

1. **Bubble Ocean** (`bubbleocean.ino`)
   - Simulates underwater bubble effects
   - Features rising bubbles with depth-aware brightness
   - Includes wave and ambient glow effects

2. **Lighthouse** (`bubblelighthouse.ino`)
   - Creates rotating beam patterns
   - Features X and O patterns in middle section
   - Includes pulsing center bands

3. **Basic Bubble** (`bubblelamp.ino`)
   - Provides smooth wave animations
   - Features dancing patterns in bottom spiral
   - Includes sparkle effects

4. **Engine Effect** (`bubbleengine.ino`)
   - Simulates energy core pulsing
   - Creates starburst patterns
   - Features energy ripple effects

5. **Matrix Rain** (`bubblematrix.ino`)
   - Digital rain effect with trailing drops
   - Section-aware drop movement
   - Custom drop speeds and brightness

6. **Pulse Mode** (`bubblepulse.ino`)
   - Creates synchronized pulse patterns
   - Features ignition sequences
   - Includes white core effects

7. **Rainbow Flow** (`bubblerainbow.ino`)
   - Generates flowing rainbow patterns
   - Features color bands and transitions
   - Includes palette shifting

8. **Fire Effect** (`bubblefire.ino`)
   - Simulates realistic fire patterns
   - Uses custom color palette
   - Features spark effects

## Development Guidelines
1. **Section Awareness**
   - All animations must handle section transitions properly
   - Bottom spiral requires continuous flow patterns
   - Middle cylinder needs proper 32-LED wraparound handling
   - Top section should complement the overall effect

2. **Performance Considerations**
   - Use `blur1d()` for smooth transitions
   - Implement `EVERY_N_MILLISECONDS()` for timing
   - Keep brightness levels balanced across sections

3. **Pattern Development**
   - Use defined constants for section boundaries
   - Implement proper fading and blending
   - Consider section-specific effects
   - Maintain visual continuity between sections

4. **Memory Usage**
   - Use appropriate data structures for patterns
   - Consider PROGMEM for palettes
   - Optimize animation calculations

## Future Development
- Add new animation patterns
- Implement interactive modes
- Create synchronized multi-lamp effects
- Develop sound-reactive capabilities
- Add wireless control features 