# Serial Transmission of Temperature

Eric Flynn, Fall 2026

EN.605.715: Software Development For Real-Time Embedded Systems, Johns Hopkins University

## About
The following project will:
1. Read analog voltage from a TMP36 temperature sensor connected to pin A0
2. Configure Timer1 in CTC mode to trigger an interrupt periodically every 10 seconds
3. Convert the sensor voltage to temperature in degrees Celsius and Fahrenheit
4. Transmit the temperature data natively over the Serial Monitor in CSV format at 115200 baud
5. Flash the onboard Arduino LED (Pin 13) with each transmission
6. Support dual-compilation using either standard Arduino IDE (`setup`/`loop`) or VS Code PlatformIO (standard C++ `main`)

## Hardware Connections
- **TMP36 Left Pin (Vs)** -> Arduino 5V
- **TMP36 Middle Pin (Vout)** -> Arduino A0
- **TMP36 Right Pin (GND)** -> Arduino GND

## Required Parts
1. Arduino (tested using a Mega2560 R3). If you do not have access to an Arduino board you can simulate the functionality here: https://wokwi.com
2. TMP36 Temperature Sensor

## Steps to Compile and Program Arduino

This project is structured to support **both** VS Code (PlatformIO) and the standard Arduino IDE.

### Option A: VS Code with PlatformIO (Recommended)
1. Open this project folder in VS Code
2. Connect the Arduino to your host PC via USB
3. Click the PlatformIO **Upload** button (the right arrow `→` on the bottom blue status bar)

### Option B: Standard Arduino IDE
1. Open `Transmit Temperature.ino` in the Arduino IDE. (The IDE will automatically pull in the core files from the `src/` folder).
2. Connect the Arduino to your host PC
3. At the top of the Arduino IDE, select the connected Arduino board and serial port
4. File -> Upload (This will compile and upload the firmware to the Arduino)

## Python Automated Logger & HTML Graph (For Experiments)
Run the Python logger script on your host machine to automatically read the serial port, log data directly to a `.csv` file, and render a live-updating web graph. This is perfect for capturing the thermal step response experiment (e.g., placing the Arduino in a refrigerator).

```bash
# Log live data from Arduino to temperature_data.csv and generate temperature_graph.html
python3 tempLogger.py

# Test / demo without hardware connected:
python3 tempLogger.py --demo
```
- Open `temperature_graph.html` in Safari or Chrome to view an interactive graph. It refreshes automatically.
- All data points are saved in `temperature_data.csv`.

*Note: Make sure to close any Serial Monitors before running the script so the script can access the port.*

## Acknowledgments
*Note: The Antigravity AI coding assistant (Google) was utilized to help refactor and document portions of this project.*
