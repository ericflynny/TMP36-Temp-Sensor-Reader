# Serial Transmission of Temperature

Eric Flynn, September 27th, 2026

EN.605.715: Software Development For Real-Time Embedded Systems, Johns Hopkins University

## About
The following project will:
1. Read analog voltage from a TMP36 temperature sensor connected to pin A0
2. Configure Timer1 in CTC mode to trigger an interrupt periodically every 10 seconds
3. Convert the sensor voltage to temperature in degrees Celsius and Fahrenheit
4. Transmit the temperature data over the Serial Monitor at 115200 baud
5. Flash the onboard Arduino LED (Pin 13) with each transmission
6. Allow the user to enter commands via the Serial Monitor (`read`, `temp`, `csv`, `plot`, `text`, `exit`)
7. Log temperature data to a CSV file (`temperature_data.csv`) and display an interactive temperature-over-time graph

## Hardware Connections
- **TMP36 Left Pin (Vs)** -> Arduino 5V
- **TMP36 Middle Pin (Vout)** -> Arduino A0
- **TMP36 Right Pin (GND)** -> Arduino GND

## Required Parts
1. Arduino (tested using a Mega2560 R3). If you do not have access to an Arduino board you can simulate the functionality here: https://wokwi.com
2. TMP36 Temperature Sensor
3. Arduino IDE

## Steps to Compile and Program Arduino
1. Open `transmitTemp.ino` in the Arduino IDE
2. Connect the Arduino to your host PC
3. At the top of the Arduino IDE, select the connected Arduino board and serial port
4. File -> Upload (This will compile and upload the firmware to the Arduino)
5. Open the Serial Monitor and set the baud rate to **115200 baud**

## Viewing the Live Graph & CSV Logging

### Method 1: Python Logger & Graph Generator (`tempLogger.py`)
Run the Python logger script on your host machine to record data directly to CSV and render an interactive graph:
```bash
# Log live data from Arduino to temperature_data.csv and generate temperature_graph.html
python3 tempLogger.py

# Test / demo without hardware connected:
python3 tempLogger.py --demo
```
- Open `temperature_graph.html` in Safari or Chrome to view an interactive graph of temperature over time.
- All data points are saved in `temperature_data.csv`.

### Method 2: Arduino IDE Built-in Serial Plotter
1. In the Serial Monitor, type `plot` and press Enter (switches output to Plotter format).
2. Go to **Tools -> Serial Plotter** (or press `Cmd + Shift + L`).
3. Set the baud rate to **115200 baud** to see a live real-time graph drawn in the Arduino IDE.

### Method 3: Direct CSV Mode in Serial Monitor
In the Serial Monitor, type `csv` and press Enter. The Arduino will output pure comma-separated values (`Time_s,Sample,Temp_C,Temp_F,Voltage_V,Raw_ADC`) ready to copy directly into Excel or a `.csv` file.
