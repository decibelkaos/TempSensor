# TempSensor Binary Files

This folder contains the pre-compiled binary files for flashing directly to your ESP32 modules.

## Available Binaries

- **TempSensor.bin** - For the transmitter module (the one with the AHT10 sensor)
- **TempSensor-RCV.bin** - For the receiver module (the one with the OLED display)

## Flashing Instructions

1. Visit the [ESP Web Flasher Tool](https://espressif.github.io/esptool-js/)
2. Connect your ESP32 module to your computer via USB
3. Set the following options:
   - **Baudrate**: 115200 (recommended for reliability)
   - **Flash Address**: 0x0 (IMPORTANT - incorrect address may cause boot loops)
4. Click "Connect" and select your device from the popup
5. Upload the appropriate binary file:
   - For the transmitter module with AHT10 sensor: `TempSensor.bin`
   - For the receiver module with OLED display: `TempSensor-RCV.bin`

## Important Notes

- Always ensure the Flash Address is set to 0x0
- If you encounter boot loops after flashing:
  1. Unplug the ESP32
  2. Hold down the BOOT button
  3. Plug the device back in while continuing to hold the button
  4. Try flashing again

## Binary File Details

### TempSensor.bin (Transmitter)
- Target: ESP32-S3
- Functionality: Reads temperature and humidity data from AHT10 sensor and transmits wirelessly

### TempSensor-RCV.bin (Receiver)
- Target: ESP32-S3
- Functionality: Receives temperature and humidity data and displays on OLED screen
- Additional feature: Creates WiFi access point for configuration

## Versioning

These binaries are version 1.0.0 of the TempSensor project. 

Future updates may include:
- Power optimization improvements
- Additional sensor support
- Enhanced display options

Check the main repository for the latest binary files.