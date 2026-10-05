#include "tempSystem.hpp"

TempSystem::TempSystem(unsigned long timerIntervalMs, uint8_t tempPin)
    : _timer(timerIntervalMs),
      _tempReader(tempPin),
      _sampleCount(0)
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
    Serial.println(F("# TMP36 Temperature Monitoring System"));

    // Initialize temperature sensor & wait for stable readings
    Serial.print(F("# Initializing TMP36 sensor on pin A0... "));
    _tempReader.init();

    // Initialize Timer1 with the configured duration
    Serial.print(F("# Configuring Timer1 CTC interrupt for "));
    Serial.print(_timer.getDuration());
    Serial.println(F(" ms intervals..."));
    _timer.init();

    // Print CSV column headers
    Serial.println(F("Time_s,Sample,Temp_C,Temp_F,Voltage_V,Raw_ADC"));

    // Transmit baseline reading
    transmitTemperature();
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
    
    Serial.flush();

    // Visible 50ms LED pulse to confirm transmission
    delay(50);
    setLed(false);
}

void TempSystem::process()
{
    while (true)
    {
        // Check if Timer1 periodic interrupt triggered
        if (_timer.checkAndClear())
        {
            transmitTemperature();
        }
    }
}
