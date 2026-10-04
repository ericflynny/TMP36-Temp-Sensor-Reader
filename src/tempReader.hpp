#pragma once
#include <Arduino.h>


class TempReader
{
    public:
        // Configured for TMP36 on Analog Pin A0 with 5.0V reference by default
        explicit TempReader(uint8_t sensorPin = A0, float vRef = 5.0f);

        // Hardware initialization and sensor stabilization
        void init();

        // Read and update cached temperature values
        void process();

        // Live measurement methods
        float readTemperatureC();
        float readTemperatureF();
        float readVoltage();
        int readRawAdc();

        // Cached value getters (updated by process())
        float getTemperatureC() const { return _lastTempC; }
        float getTemperatureF() const { return _lastTempF; }
        float getVoltage() const      { return _lastVoltage; }
        int getRawAdc() const         { return _lastAdc; }
        bool isStable() const         { return _isStable; }

        // Sensor pin configuration getter
        uint8_t getSensorPin() const  { return _sensorPin; }

    private:
        // Arduino LED Pin
        static constexpr uint8_t LedPin {LED_BUILTIN};

        // Hardware functions
        void setLed(const bool State) { digitalWrite(LedPin, State ? HIGH : LOW); }

        // TMP36 characteristics: 500 mV offset at 0 °C, 10 mV / °C scale factor
        static constexpr float VoltageOffset {0.5f};          // 500 mV at 0 °C
        static constexpr float MillivoltsPerDegree {0.010f};  // 10 mV / °C (0.010 V / °C)

        // Conversions
        float voltageToC(float voltage) const {return (voltage - VoltageOffset) / MillivoltsPerDegree;}
        float cToF(float tempC) const {return (tempC * 1.8f) + 32.0f;}

        uint8_t _sensorPin;
        float _vRef;

        float _lastTempC;
        float _lastTempF;
        float _lastVoltage;
        int _lastAdc;
        bool _isStable;
};
