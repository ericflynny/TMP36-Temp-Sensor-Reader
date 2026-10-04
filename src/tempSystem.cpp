#include "tempSystem.hpp"


TempSystem::TempSystem(unsigned long timerIntervalMs, uint8_t tempPin, OutputFormat defaultFormat)
    : _timer(timerIntervalMs),
      _tempReader(tempPin),
      _running(false),
      _sampleCount(0),
      _outputFormat(defaultFormat)
{
}

void TempSystem::init()
{
    // Allow USB-Serial connection to stabilize
    delay(1000);

    // Setup onboard LED
    pinMode(LedPin, OUTPUT);
    setLed(false);

    Serial.flush();

    // Startup banner (commented with '#' so CSV parsers ignore it)
    Serial.println(F("# ========================================================"));
    Serial.println(F("# TMP36 Temperature Monitoring System"));
    Serial.println(F("# EN.605.715 - Software Development for Real-Time Embedded Systems"));
    Serial.println(F("# Commands: 'csv', 'plot', 'graph', 'text', 'read', 'help', 'exit'"));
    Serial.println(F("# ========================================================"));

    // Initialize temperature sensor & wait for stable readings
    Serial.print(F("# Initializing TMP36 sensor on pin A0... "));
    _tempReader.init();
    Serial.println(F("Done."));

    // Initialize Timer1 with the configured duration
    Serial.print(F("# Configuring Timer1 CTC interrupt for "));
    Serial.print(_timer.getDuration());
    Serial.println(F(" ms intervals... Done."));
    _timer.init();

    // If starting in CSV mode, output the header
    if (_outputFormat == OutputFormat::Csv)
    {
        printCsvHeader();
    }
    else
    {
        Serial.println(F("\nBaseline temperature:"));
    }

    // Transmit baseline reading
    transmitTemperature();

    if (_outputFormat != OutputFormat::Csv)
    {
        Serial.println(F("\nMonitoring active. Commands: 'read', 'csv', 'plot', 'graph', 'text', 'help', 'exit'\n"));
    }
}

void TempSystem::printCsvHeader()
{
    Serial.println(F("Time_s,Sample,Temp_C,Temp_F,Voltage_V,Raw_ADC"));
}

void TempSystem::printLiveGraph(float tempC, float tempF, unsigned long timeSec)
{
    // Visual gauge spanning 15.0 °C to 35.0 °C (typical room / ambient range)
    constexpr float MinTempC = 15.0f;
    constexpr float MaxTempC = 35.0f;
    constexpr int BarWidth   = 28;

    float clamped = constrain(tempC, MinTempC, MaxTempC);
    int barLen = static_cast<int>(((clamped - MinTempC) / (MaxTempC - MinTempC)) * BarWidth);
    if (barLen < 1) barLen = 1;

    Serial.print(F("["));
    if (timeSec < 10) Serial.print(F("   "));
    else if (timeSec < 100) Serial.print(F("  "));
    else if (timeSec < 1000) Serial.print(F(" "));
    Serial.print(timeSec);
    Serial.print(F("s] "));

    if (tempC < 10.0f) Serial.print(F(" "));
    Serial.print(tempC, 2);
    Serial.print(F(" °C ("));
    if (tempF < 100.0f) Serial.print(F(" "));
    Serial.print(tempF, 2);
    Serial.print(F(" °F) [15°C |"));

    for (int i = 0; i < BarWidth; ++i)
    {
        if (i < barLen - 1)
        {
            Serial.print(F("="));
        }
        else if (i == barLen - 1)
        {
            Serial.print(F(">"));
        }
        else
        {
            Serial.print(F(" "));
        }
    }
    Serial.println(F("| 35°C]"));
}

void TempSystem::transmitTemperature()
{
    // Pulse status LED during transmission
    setLed(true);

    _tempReader.process();
    _sampleCount++;

    float tempC = _tempReader.getTemperatureC();
    float tempF = _tempReader.getTemperatureF();
    float volts = _tempReader.getVoltage();
    int rawAdc  = _tempReader.getRawAdc();
    unsigned long timeSec = (_sampleCount * (_timer.getDuration() / 1000UL));

    switch (_outputFormat)
    {
        case OutputFormat::Csv:
        {
            // Pure CSV format: Time_s,Sample,Temp_C,Temp_F,Voltage_V,Raw_ADC
            Serial.print(timeSec);
            Serial.print(F(","));
            Serial.print(_sampleCount);
            Serial.print(F(","));
            Serial.print(tempC, 2);
            Serial.print(F(","));
            Serial.print(tempF, 2);
            Serial.print(F(","));
            Serial.print(volts, 3);
            Serial.print(F(","));
            Serial.println(rawAdc);
            break;
        }
        case OutputFormat::Plotter:
        {
            // Arduino Serial Plotter format (Tools -> Serial Plotter)
            Serial.print(F("Temp_C:"));
            Serial.print(tempC, 2);
            Serial.print(F(",Temp_F:"));
            Serial.print(tempF, 2);
            Serial.print(F(",Voltage:"));
            Serial.println(volts, 3);
            break;
        }
        case OutputFormat::Teleplot:
        {
            // Teleplot extension format (>var:value)
            Serial.print(F(">Temp_C:"));
            Serial.println(tempC, 2);
            Serial.print(F(">Temp_F:"));
            Serial.println(tempF, 2);
            Serial.print(F(">Voltage:"));
            Serial.println(volts, 3);
            break;
        }
        case OutputFormat::Graph:
        {
            // Live ASCII/bar graph
            printLiveGraph(tempC, tempF, timeSec);
            break;
        }
        case OutputFormat::Text:
        default:
        {
            // Standard human-readable format
            Serial.print(F("[Sample #"));
            Serial.print(_sampleCount);
            Serial.print(F(" | Time: "));
            Serial.print(timeSec);
            Serial.print(F("s] Temp: "));
            Serial.print(tempC, 2);
            Serial.print(F(" °C  |  "));
            Serial.print(tempF, 2);
            Serial.print(F(" °F  |  Voltage: "));
            Serial.print(volts, 3);
            Serial.print(F(" V (ADC: "));
            Serial.print(rawAdc);
            Serial.println(F(")"));
            break;
        }
    }
    Serial.flush();

    // Visible 50ms LED pulse to confirm transmission
    delay(50);
    setLed(false);
}

void TempSystem::process()
{
    _running = true;

    while (_running)
    {
        // 1. Check if Timer1 periodic interrupt triggered
        if (_timer.checkAndClear())
        {
            transmitTemperature();
        }

        // 2. Check for Serial commands from the user (non-blocking)
        handleSerialCommands();
    }
}

void TempSystem::printHelp()
{
    Serial.println(F("\n--- Available Commands ---"));
    Serial.println(F("  csv    : Switch output to comma-separated values (CSV)"));
    Serial.println(F("  graph  : Switch output to live ASCII bar graph"));
    Serial.println(F("  plot   : Switch output to Arduino Serial Plotter format"));
    Serial.println(F("  text   : Switch output to human-readable text"));
    Serial.println(F("  teleplot: Switch output to Teleplot extension format"));
    Serial.println(F("  read   : Immediately sample and transmit current temperature"));
    Serial.println(F("  header : Re-print the CSV column header"));
    Serial.println(F("  help   : Display this help menu"));
    Serial.println(F("  exit   : Stop the periodic monitoring loop\n"));
}

void TempSystem::handleSerialCommands()
{
    if (Serial.available() > 0)
    {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        cmd.toLowerCase();
        if (cmd.length() == 0)
        {
            return;
        }

        if (cmd == "exit" || cmd == "quit")
        {
            Serial.println(F("\nExit command received. Halting system..."));
            _running = false;
        }
        else if (cmd == "read" || cmd == "temp")
        {
            if (_outputFormat != OutputFormat::Csv && _outputFormat != OutputFormat::Plotter)
            {
                Serial.println(F("[Manual Sample]"));
            }
            transmitTemperature();
        }
        else if (cmd == "csv")
        {
            _outputFormat = OutputFormat::Csv;
            printCsvHeader();
        }
        else if (cmd == "plot")
        {
            _outputFormat = OutputFormat::Plotter;
            Serial.println(F("# Switched to Arduino Serial Plotter format."));
        }
        else if (cmd == "graph")
        {
            _outputFormat = OutputFormat::Graph;
            Serial.println(F("# Switched to live ASCII graph format (15°C - 35°C scale)."));
        }
        else if (cmd == "text")
        {
            _outputFormat = OutputFormat::Text;
            Serial.println(F("Switched to standard text format."));
        }
        else if (cmd == "teleplot")
        {
            _outputFormat = OutputFormat::Teleplot;
            Serial.println(F("# Switched to Teleplot extension format."));
        }
        else if (cmd == "header")
        {
            printCsvHeader();
        }
        else if (cmd == "help")
        {
            printHelp();
        }
        else
        {
            Serial.print(F("Unknown command: "));
            Serial.print(cmd);
            Serial.println(F(". Type 'help' for command list."));
        }
    }
}
