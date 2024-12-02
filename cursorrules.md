# Arduino AI Development Guidelines

## Project Overview
This repository contains AI-assisted Arduino projects focusing on LED installations and IoT implementations. The primary development platform is the Arduino MKR 1000 WiFi, chosen for its robust wireless capabilities and compact form factor.

## Hardware Specifications

### Primary Development Board
- **Device:** Arduino MKR 1000 WiFi
- **Processor:** SAMD21 Cortex-M0+ 32bit low power ARM MCU
- **Clock Speed:** 48 MHz
- **Operating Voltage:** 3.3V
- **Flash Memory:** 256KB
- **SRAM:** 32KB
- **WiFi:** Built-in Nina W102 module

### Common Components
- WS2812B Addressable LEDs
- Various sensors (temperature, motion, light)
- Power supplies (3.3V and 5V compatible)
- Level shifters (when needed for LED control)

## Development Roles

### AI Assistant Responsibilities
1. **Code Development**
   - Write and optimize Arduino sketches
   - Implement WiFi and network functionality
   - Design LED animation patterns
   - Create sensor integration logic
   - Handle memory management and optimization

2. **Technical Guidance**
   - Provide pin configuration advice
   - Suggest optimal power management solutions
   - Recommend appropriate libraries
   - Debug code issues
   - Optimize performance

3. **Documentation**
   - Generate detailed code comments
   - Create setup instructions
   - Document API endpoints
   - Maintain configuration guides

### Human Collaborator Responsibilities
1. **Hardware Setup**
   - Physical installation of components
   - Wiring and connections
   - Power supply setup
   - Sensor placement

2. **Testing & Feedback**
   - Run test sequences
   - Report performance issues
   - Validate sensor readings
   - Verify LED patterns
   - Test WiFi connectivity

## Project Structure
1. **LED Installations**
   - BubbleGlassLamp/
   - GarageDoorDisplay/
   - [Future Projects]/

2. **Common Libraries**
   - FastLED
   - WiFiNINA
   - ArduinoJson
   - [Project-specific libraries]

## Development Guidelines

### Code Standards
1. **Memory Management**
   - Use PROGMEM for constant data
   - Optimize variable types for size
   - Implement efficient data structures
   - Monitor stack and heap usage

2. **WiFi Implementation**
   - Implement reconnection strategies
   - Use secure connections (SSL/TLS)
   - Handle network failures gracefully
   - Cache data when appropriate

3. **LED Control**
   - Use FastLED's efficient methods
   - Implement power limitations
   - Consider refresh rates
   - Handle multiple LED strips

4. **Sensor Integration**
   - Implement proper sampling rates
   - Filter noisy data
   - Handle sensor failures
   - Calibrate as needed

### Communication Protocol
1. **AI-Human Interaction**
   - Human reports physical setup completion
   - AI provides test sequences
   - Human reports test results
   - AI adjusts code based on feedback
   - Iterate until optimal performance

2. **Documentation Requirements**
   - Clear pin assignments
   - Power requirements
   - Setup procedures
   - Troubleshooting guides

## Best Practices

### Power Management
- Calculate power requirements
- Implement current limiting
- Monitor voltage levels
- Handle brownout conditions

### WiFi Connectivity
- Implement robust connection handling
- Use static IP when appropriate
- Monitor signal strength
- Handle network transitions

### LED Operations
- Calculate power draw
- Implement brightness limits
- Use efficient patterns
- Handle timing issues

### Error Handling
- Implement watchdog timers
- Log critical errors
- Provide status indicators
- Auto-recovery procedures

## Future Development
1. **Planned Features**
   - Web-based control interface
   - Mobile app integration
   - Advanced sensor fusion
   - Machine learning integration

2. **Optimization Goals**
   - Reduce power consumption
   - Improve network reliability
   - Enhance animation smoothness
   - Expand sensor capabilities

## Testing Procedures
1. **Initial Setup**
   - Power system verification
   - LED functionality check
   - WiFi connection test
   - Sensor calibration

2. **Performance Validation**
   - Memory usage monitoring
   - Frame rate verification
   - Network latency testing
   - Power consumption measurement

## Support and Maintenance
- Regular code updates
- Library compatibility checks
- Security patches
- Performance optimization
- Documentation updates 