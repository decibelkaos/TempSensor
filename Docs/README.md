# Wiring Diagram for TempSensor

## Overview

This project requires two ESP32 modules with specific components connected to each:
1. **Transmitter Module**: ESP32 + AHT10 Temperature/Humidity Sensor
2. **Receiver Module**: ESP32 + OLED Display

## Required Materials

- 2× ESP32-S3 Development Boards
- 1× AHT10 Temperature/Humidity Sensor
- 1× I2C OLED Display
- Jumper wires
- Soldering equipment

## Transmitter Module Wiring

The AHT10 sensor connects to the ESP32 using I2C protocol.

| AHT10 Pin | ESP32 Pin | Wire Color (Suggested) |
|-----------|-----------|------------------------|
| VIN | 3.3V | Red |
| GND | GND | Black |
| SCL | GPIO 13 | Yellow |
| SDA | GPIO 12 | Blue |

### Visual Diagram - Transmitter

```
ESP32-S3                AHT10
+--------+              +-------+
|        |              |       |
| 3.3V o-|------------->o VIN   |
|        |              |       |
| GND  o-|------------->o GND   |
|        |              |       |
| GPIO13o-|------------->o SCL   |
|        |              |       |
| GPIO12o-|------------->o SDA   |
|        |              |       |
+--------+              +-------+
```

## Receiver Module Wiring

The OLED Display connects to the ESP32 using I2C protocol.

| OLED Pin | ESP32 Pin | Wire Color (Suggested) |
|----------|-----------|------------------------|
| VIN | 3.3V | Red |
| GND | GND | Black |
| SCL | GPIO 13 | Yellow |
| SDA | GPIO 12 | Blue |

### Visual Diagram - Receiver

```
ESP32-S3                OLED Display
+--------+              +-----------+
|        |              |           |
| 3.3V o-|------------->o VIN       |
|        |              |           |
| GND  o-|------------->o GND       |
|        |              |           |
| GPIO13o-|------------->o SCL       |
|        |              |           |
| GPIO12o-|------------->o SDA       |
|        |              |           |
+--------+              +-----------+
```

## Soldering Tips

1. **Headers**: Solder the headers to the ESP32 modules, AHT10 sensor, and OLED display
   - Use a soldering iron at approximately 350°C (662°F)
   - Apply a small amount of solder to the tip first (tinning)
   - Heat the pin and pad together for 2-3 seconds before applying solder
   - Allow each joint to cool before moving to the next

2. **Alternative Connection**: If you prefer not to solder:
   - Female-to-female jumper wires can be used to connect the components
   - Breadboards can provide temporary connections for testing

3. **Checking Connections**: 
   - Visually inspect all solder joints for shininess and good contact
   - Use a multimeter to check for continuity between connected points
   - Gently tug on wires to ensure they are secure

## Common Wiring Issues

1. **Cold Solder Joints**: Appear dull and gray rather than shiny - reheat and add a small amount of solder
2. **Shorts**: Accidentally connecting adjacent pins - check with a magnifying glass
3. **Poor Contact**: Ensure header pins are fully inserted into the boards before soldering
4. **Wire Length**: Keep wires short but with enough slack to avoid strain

## Notes

- Both modules use the same GPIO pins (12 and 13) for I2C communication
- The ESP32-S3 has built-in pull-up resistors, so external resistors are not required
- Position the AHT10 sensor away from heat sources (including the ESP32 itself) for accurate readings
