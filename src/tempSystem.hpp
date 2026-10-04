#pragma once
#include <Arduino.h>
#include "timer.hpp"
#include "tempReader.hpp"

enum class OutputFormat : uint8_t
{
    Csv = 0,    // Pure CSV format (Time_s,Sample,Temp_C,Temp_F,Voltage_V,Raw_ADC)
    Text,       // Human-readable formatted text
    Plotter,    // Arduino Serial Plotter (Temp_C:xx,Temp_F:yy,Voltage:zz)
    Graph,      // Live ASCII bar graph over Serial Monitor
    Teleplot    // Teleplot extension format (>var:value)
};

class TempSystem
{
    public:
        // Encapsulates Timer1 and TempReader with a default 10-second interval
        explicit TempSystem(unsigned long timerIntervalMs = TenSecondsMs, uint8_t tempPin = A0, OutputFormat defaultFormat = OutputFormat::Csv);

        // System initialization
        void init();

        // Main system executive loop (called in setup(), controls the execution loop)
        void process();

        // Transmission actions
        void transmitTemperature();

        // Format selection
        void setOutputFormat(OutputFormat format) { _outputFormat = format; }
        OutputFormat getOutputFormat() const { return _outputFormat; }

        // Print CSV column headers
        void printCsvHeader();

    private:
        // Timing constants
        static constexpr unsigned long OneSecondMs {1000};
        static constexpr unsigned long TenSecondsMs {10000};

        // Status LED
        static constexpr uint8_t LedPin {LED_BUILTIN};
        void setLed(const bool state) { digitalWrite(LedPin, state ? HIGH : LOW); }

        // Live text/ASCII graph rendering
        void printLiveGraph(float tempC, float tempF, unsigned long timeSec);

        // Serial command processing
        void handleSerialCommands();
        void printHelp();

        Timer1 _timer;
        TempReader _tempReader;
        bool _running;
        unsigned long _sampleCount;
        OutputFormat _outputFormat;
};
