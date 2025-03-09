# TempSensor - The Display End

This is the receiver component of the TempSensor project - the show-off end of the operation. It receives temperature and humidity data from the transmitter module and displays it on a tiny but surprisingly visible OLED screen. It also hosts a web server for customizing display settings because we all need more things to configure in our lives.

## Hardware Connections

| OLED Pin | ESP32 Pin |
|----------|-----------|
| VIN | 3.3V |
| GND | GND |
| SCL | GPIO 13 |
| SDA | GPIO 12 |

## How It Works

1. The ESP32 initializes the OLED display using I2C communication (magic smoke stays inside)
2. It sets up as an ESP-NOW receiver to get data from the transmitter (like a digital mailbox)
3. Creates a WiFi access point for configuration (look for it in your WiFi list, it's the one screaming for attention)
4. Hosts a surprisingly decent web server at 192.168.4.1 (no, you can't mine Bitcoin on it)
5. Displays received environmental data and updates in real-time (or what passes for real-time in microcontroller terms)
6. Stores user preferences in non-volatile memory (it remembers your choices even after a power nap)

## Flashing Instructions

1. Visit the [ESP Web Flasher Tool](https://espressif.github.io/esptool-js/)
2. Connect your ESP32 module to your computer via USB
3. Set the following options:
   - **Baudrate**: 115200
   - **Flash Address**: 0x0 (IMPORTANT)
4. Click "Connect" and select your device
5. Upload the `TempSensor-RCV.bin` file located in the Binaries folder

## Web Interface (Yes, it has one!)

The web interface is contained in the `Webpage.h` file, which is essentially HTML, CSS, and JavaScript crammed into a C++ header file because... embedded programming.

After powering on the receiver:
1. Connect to the WiFi access point it creates (no password - security is for the weak!)
2. Navigate to 192.168.4.1 in a web browser (any browser will do, except IE6)
3. Marvel at the interface that was created with blood, sweat, and Stack Overflow
4. Customize display settings:
   - Temperature units (°C/°F - choose wisely, this will define your character)
   - Display brightness (from "barely visible" to "mini sun")
   - Update frequency (how frequently you need to be reminded of the weather)
   - Alert thresholds (at what point should you panic about the temperature)

## Troubleshooting

- **Blank display**: Check wiring and power supply
- **No data received**: Ensure transmitter is powered and within range
- **Cannot connect to web interface**: Verify you're connected to the correct WiFi network
- **Settings not saving**: Internal memory may need to be reset (instructions in code comments)

For more detailed information, refer to the main project [README](../README.md).