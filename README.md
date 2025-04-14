This library provides an Arduino interface for the Cirrus Logic CS5490 single-phase power/energy monitoring IC. The CS5490 is designed for power/energy measurement in AC power meters and other power monitoring applications.


## Features

- Access to all CS5490 registers across all pages
- Automatic fixed-point/floating-point conversion
- Read electrical parameters:
  - Voltage RMS
  - Current RMS
  - Active Power
  - Reactive Power
  - Apparent Power
  - Power Factor
  - Line Frequency
  - Temperature
- Calibration functionality
- Debug mode for troubleshooting

## Installation

### Using the Arduino Library Manager

1. Open Arduino IDE
2. Go to Sketch > Include Library > Manage Libraries...
3. Search for "CS5490"
4. Click Install

### Manual Installation

1. Download the repository as a ZIP file
2. In Arduino IDE: Sketch > Include Library > Add .ZIP Library...
3. Select the downloaded ZIP file

### PlatformIO

Add this to your platformio.ini file:

```ini
lib_deps =
  CS5490
```

## Hardware Connection

The CS5490 communicates via UART. Connect the following pins:

| CS5490 Pin | Arduino/ESP32 Pin |
|------------|------------------|
| RX         | TX (Serial TX)   |
| TX         | RX (Serial RX)   |
| RESET      | Any digital pin (optional) |
| VDD        | 3.3V             |
| GND        | GND              |

**Note:** The CS5490 operates at 3.3V logic levels. Use a level shifter when connecting to 5V Arduino boards.

## Basic Usage

```cpp
#include <Arduino.h>
#include "CS5490.h"

// Create an instance of CS5490 using Serial1 UART
CS5490 energyMeter(Serial1);

void setup() {
    Serial.begin(115200);  // For debug output
    
    // Initialize CS5490 at 19200 baud, enable debug output
    energyMeter.begin(19200, true);
    
    // Ensure the chip is awake
    energyMeter.sendInstruction(CS5490::WAKEUP);
    
    // Start continuous conversion mode
    energyMeter.sendInstruction(CS5490::CONT_CONV);
}

void loop() {
    // Read measurements
    float voltage_rms = energyMeter.readRegister(CS5490::V_RMS);
    float current_rms = energyMeter.readRegister(CS5490::I_RMS);
    float power_avg = energyMeter.readRegister(CS5490::P_AVG);
    float power_factor = energyMeter.readRegister(CS5490::PF);
    float epsilon = energyMeter.readRegister(CS5490::EPSILON);
    float line_freq_hz = epsilon * 4000.0f;  // Epsilon = lineFreq / 4000
    
    // Print measurements
    Serial.print("Voltage: "); Serial.print(voltage_rms, 2); Serial.println(" V");
    Serial.print("Current: "); Serial.print(current_rms, 3); Serial.println(" A");
    Serial.print("Power: "); Serial.print(power_avg, 2); Serial.println(" W");
    Serial.print("Power Factor: "); Serial.println(power_factor, 3);
    Serial.print("Frequency: "); Serial.print(line_freq_hz, 2); Serial.println(" Hz");
    
    delay(1000);
}
```

## API Reference

### Constructor

```cpp
CS5490(HardwareSerial &uart, int resetPin = -1);
```

- `uart`: Hardware serial port for communication with the CS5490
- `resetPin`: Optional GPIO pin connected to the CS5490 reset pin

### Initialization

```cpp
bool begin(long baudRate = 600, bool debugEnable = false);
```

- `baudRate`: Communication baud rate (default 600 bps)
- `debugEnable`: Enable debug output (default false)

### Register Access

```cpp
float readRegister(uint32_t reg);
void writeRegister(uint32_t reg, float value);
uint32_t readRawRegister(uint32_t reg);
void writeRawRegister(uint32_t reg, uint32_t rawValue);
```

### Instruction Commands

```cpp
void sendInstruction(uint8_t instrCode);
```

Common instructions:
- `CS5490::SOFT_RESET`: Perform software reset
- `CS5490::STANDBY`: Enter standby mode
- `CS5490::WAKEUP`: Wake up from standby
- `CS5490::SINGLE_CONV`: Perform a single conversion
- `CS5490::CONT_CONV`: Enter continuous conversion mode
- `CS5490::HALT_CONV`: Halt conversions

### Utility Methods

```cpp
bool softReset();
void setDebug(bool enable);
bool lastReadSuccess() const;
```

## Common Registers

The library defines constants for all CS5490 registers. Key measurement registers include:

| Register | Description | Units/Notes |
|----------|-------------|-------------|
| `CS5490::V_RMS` | Voltage RMS | Volts |
| `CS5490::I_RMS` | Current RMS | Amperes |
| `CS5490::P_AVG` | Active power | Watts |
| `CS5490::Q_AVG` | Reactive power | VAR |
| `CS5490::S` | Apparent power | VA |
| `CS5490::PF` | Power factor | -1.0 to 1.0 |
| `CS5490::TEMP` | Chip temperature | °C |
| `CS5490::EPSILON` | Line frequency | LineFreq = Epsilon * 4000 |

## Calibration

The CS5490 requires calibration for accurate measurements. The library includes support for the calibration instructions:

```cpp
// Examples of calibration instructions
energyMeter.sendInstruction(CS5490::I_AC_CAL);    // Current AC offset calibration
energyMeter.sendInstruction(CS5490::V_AC_CAL);    // Voltage AC offset calibration
energyMeter.sendInstruction(CS5490::IV_AC_CAL);   // Current and Voltage AC offset calibration
energyMeter.sendInstruction(CS5490::I_GAIN_CAL);  // Current gain calibration
energyMeter.sendInstruction(CS5490::V_GAIN_CAL);  // Voltage gain calibration
```

Refer to the CS5490 datasheet for detailed calibration procedures.

## Troubleshooting

If you're having communication issues:

1. Enable debug mode: `energyMeter.begin(19200, true);`
2. Check your wiring and voltage levels
3. Try lower baud rates initially (600 is the default after reset)
4. Check that your hardware serial port is correctly defined
5. Verify the CS5490 has proper power supply

## License

This library is released under the MIT License. See LICENSE file for details.

## Credits

Developed by Myself
Ported from [Tisham Dhar's cs5490_micropython] (https://github.com/whatnick/cs5490_micropython.git)

## References

- [CS5490 Datasheet](https://www.cirrus.com/products/cs5490/)
- [Cirrus Logic Website](https://www.cirrus.com/)
