# ArduinoAI LED Art Installations

A collection of Arduino-based LED art installations using AI-assisted development.

## Projects

### Garage Door Display
- **Location**: Garage door windows
- **Hardware**: WS2812B LED strips (300 LEDs per window)
- **Animations**:
  - Fire effect with dynamic color palette
  - Cloud/mist effect with smooth transitions
  - Vertical stripes with customizable colors

### 16x16 LED Matrix Display
- **Location**: Wall-mounted display
- **Hardware**: 16x16 WS2812B LED Matrix (256 LEDs)
- **Features**:
  - WiFi-enabled status display
  - API-controlled animations
  - Real-time animation updates
- **Animations**:
  - Pulsing colors with smooth transitions
  - Dynamic spiral patterns
  - Wave effects
  - Cloud/mist patterns
  - Status indicators (checkmark, X mark)
- **WiFi Status**:
  - Corner LEDs indicate connection status
  - Green: Connected
  - Blinking Red: Connecting/Checking
  - Solid Red: Disconnected

## API Integration

### Matrix Controller API
- **Endpoint**: `/api-T/matrix/config`
- **Method**: GET
- **Response Format**:
```json
{
  "mode": 1-4,
  "brightness": 0-255,
  "speed": 0-100
}
```
- **Animation Modes**:
  1. Pulsing Colors
  2. Spiral
  3. Wave
  4. Cloud Mist

### Configuration
- Uses `config.h` for sensitive settings
- Template provided in `config.h.template`
- Required settings:
  - WiFi SSID
  - WiFi Password
  - API Host

## Setup Instructions

1. Clone the repository
2. Copy `config.h.template` to `config.h` in the project directory
3. Update WiFi and API credentials in `config.h`
4. Upload sketch to Arduino MKR1000 or compatible board
5. Connect LED strip/matrix to specified data pin

## Dependencies

- FastLED Library
- WiFi101 Library
- ArduinoJson Library

## Hardware Requirements

### Garage Door Display
- Arduino Mega or compatible
- WS2812B LED strips
- 5V Power Supply (10A recommended)

### LED Matrix Display
- Arduino MKR1000 WiFi
- 16x16 WS2812B Matrix
- 5V Power Supply (5A recommended)

## Development Notes

- Uses AI-assisted development for pattern generation
- Implements non-blocking WiFi and API operations
- Maintains animation continuity during network operations
- Includes error handling and connection recovery
- Status feedback through corner LEDs

## Contributing

Feel free to submit issues and enhancement requests!

## Resources
- [Arduino MKR 1000 Documentation](https://docs.arduino.cc/hardware/mkr-1000-wifi)
- [FastLED Library](https://fastled.io/)
- [AI Coding Assistant Resources](https://cursor.sh/)
- [LED Art Installation Guides](https://learn.adafruit.com/led-art)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

Support my work on [Patreon](https://www.patreon.com/c/makeratl)
