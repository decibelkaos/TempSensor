# TempSensor: Yet Another ESP32 Temperature Monitor

A wireless temperature and humidity monitoring system using two ESP32 modules - one as a transmitter with an AHT10 sensor and another as a receiver with an OLED display. Because apparently, the world needed yet another way to be told it's hot outside.



## Features

- **Wireless Monitoring**: Real-time temperature and humidity data transmitted wirelessly (yes, like magic!)
- **Customizable Display**: Web interface to customize display settings, because one size never fits all
- **Simple Setup**: Minimal wiring and straightforward flashing process - even your cat could do it (though please don't let them try)
- **No Configuration**: Devices automatically pair when powered on, saving you precious minutes of life that you'll probably waste on social media anyway
- **Persistent Settings**: Customized display settings are retained after power cycles, because re-entering preferences is so 2010

## Required Materials

| Component | Quantity | Link | Approximate Cost |
|-----------|----------|------|------------------|
| ESP32-S3 Development Board | 2 | [Amazon Link](https://a.co/d/dDLfVQh) | $18.99 (3-pack) |
| AHT10 Temperature/Humidity Sensor | 1 | [Amazon Link](https://a.co/d/9607TRc) | $14.99 (10-pack) |
| OLED Display (I2C) | 1 | [Amazon Link](https://a.co/d/d1oBQb6) | $21.69 (10-pack) |
| Jumper Wires | As needed | - | - |
| Soldering Equipment | - | - | - |

**Note**: Any ESP32-S3 development board should work as long as the GPIO pins are compatible.

## Skill Level

**Level 3** DIY project - Requires basic soldering skills and familiarity with flashing microcontrollers. Not your first rodeo, but we're not asking you to build a quantum computer either.

## Wiring Instructions

### Transmitter Module (with AHT10 sensor)

| AHT10 Pin | ESP32 Pin |
|-----------|-----------|
| VIN | 3.3V |
| GND | GND |
| SCL | GPIO 13 |
| SDA | GPIO 12 |

### Receiver Module (with OLED display)

| OLED Pin | ESP32 Pin |
|----------|-----------|
| VIN | 3.3V |
| GND | GND |
| SCL | GPIO 13 |
| SDA | GPIO 12 |

## Installation Instructions

### Flashing the Firmware

1. Visit the [ESP Web Flasher Tool](https://espressif.github.io/esptool-js/)
2. Connect your ESP32 module to your computer via USB
3. Set the following options:
   - **Baudrate**: 115200 (recommended for reliability)
   - **Flash Address**: 0x0 (IMPORTANT - incorrect address may cause boot loops)
4. Click "Connect" and select your device from the popup
5. For the Transmitter module:
   - Upload the `TempSensor.bin` file
6. For the Receiver module:
   - Upload the `TempSensor-RCV.bin` file

### Troubleshooting Flash Issues

If your device enters a boot loop (the ESP32 equivalent of having an existential crisis):
1. Unplug the ESP32
2. Hold down the BOOT button (gently, it's sensitive)
3. Plug the device back in while continuing to hold the button
4. Try flashing again while whispering encouraging words to your device

## Usage

1. Power both modules (via USB or external power supply) - yes, electricity is required
2. The devices will automatically connect to each other (they're quite friendly that way)
3. The transmitter will begin sending temperature and humidity data at blinding speeds (well, technically at the speed of radio waves)
4. The receiver will display the data on its OLED screen with all the excitement of a digital thermometer

### Customizing the Display

1. Check your available WiFi networks
2. Connect to the access point broadcasted by the receiver module (no password required)
3. Open a web browser and navigate to `192.168.4.1`
4. Adjust display settings as desired - changes apply instantly and are saved permanently

## Source Code

The project consists of two separate Arduino sketches (because one massive sketch would be chaos):

- **Transmitter**: [TempSensor.ino](./Transmitter/TempSensor.ino) - The one that does the sensing
- **Receiver**: [TempSensor-RCV.ino](./Receiver/TempSensor-RCV.ino) - The one that does the displaying
- **Web Interface**: [Webpage.h](./Receiver/Webpage.h) - The surprisingly decent-looking web interface

## Future Improvements

- Add battery power support (because who doesn't love the thrill of wondering when batteries will die?)
- Implement sleep modes for power efficiency (ESP32s love naps as much as we do)
- Add multiple sensor support (for the data hoarders among us)
- Create customizable alerts for temperature/humidity thresholds (so you can be notified exactly HOW uncomfortable you are)
- Machine learning integration (just kidding, not everything needs AI)

## License

This project is released under the MIT License. See the LICENSE file for details.

## Contributing

Contributions welcome! Please feel free to submit a Pull Request.

## Acknowledgements

- Thanks to the ESP32 and Arduino communities for their excellent documentation
- Special thanks to all open-source libraries used in this project