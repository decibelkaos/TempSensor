# TempSensor - The Sensing End

This is the transmitter component of the TempSensor project - the strong, silent type that does all the actual work. It reads temperature and humidity data from an AHT10 sensor and transmits it wirelessly to the receiver module. Think of it as the introvert of the pair.

## Hardware Connections

| AHT10 Pin | ESP32 Pin |
|-----------|-----------|
| VIN | 3.3V |
| GND | GND |
| SCL | GPIO 13 |
| SDA | GPIO 12 |

## How It Works

1. The ESP32 communicates with the AHT10 sensor using I2C protocol (they speak in beeps and boops)
2. Temperature and humidity readings are taken at regular intervals (it asks "how hot is it?" approximately 3,600 times per hour)
3. Data is transmitted wirelessly using ESP-NOW protocol (faster than shouting the temperature across the room)
4. Low power optimizations are implemented to extend battery life (because we're environmentally conscious... or just hate changing batteries)

## Flashing Instructions

1. Visit the [ESP Web Flasher Tool](https://espressif.github.io/esptool-js/)
2. Connect your ESP32 module to your computer via USB
3. Set the following options:
   - **Baudrate**: 115200
   - **Flash Address**: 0x0 (IMPORTANT)
4. Click "Connect" and select your device
5. Upload the `TempSensor.bin` file located in the Binaries folder

## Customization Options

The code can be modified to:
- Change the sampling rate
- Adjust calibration settings
- Add additional sensors
- Implement different wireless protocols

## Troubleshooting

- **No data being transmitted**: Ensure proper wiring and that the sensor is functioning correctly (the classic "is it plugged in?" solution)
- **Incorrect readings**: Verify sensor calibration and placement away from heat sources (including your gaming PC or that overworked coffee maker)
- **Connection issues**: Check that both modules are powered and within range (ESP-NOW has impressive range, but can't quite reach Mars)
- **Sensor reports impossible values**: Either you've discovered a climate anomaly worthy of scientific investigation, or something's wrong with your wiring

For more detailed information, refer to the main project [README](../README.md).