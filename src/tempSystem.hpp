#pragma once
#include <Arduino.h>
#include "timer.hpp"
#include "tempReader.hpp"

class TempSystem
{
    public:
        // Encapsulates Timer1 and TempReader with a default 10-second interval
        explicit TempSystem(unsigned long timerIntervalMs = TenSecondsMs, uint8_t tempPin = A0);

        // System initialization
        void init();

        // Main system executive loop (called in setup(), controls the execution loop)
        void process();

        // Transmission actions
        void transmitTemperature();

    private:
        // Timing constants
        static constexpr unsigned long OneSecondMs {1000};
        static constexpr unsigned long TenSecondsMs {10000};

        // Status LED
        static constexpr uint8_t LedPin {LED_BUILTIN};
        void setLed(const bool state) { digitalWrite(LedPin, state ? HIGH : LOW); }

        Timer1 _timer;
        TempReader _tempReader;
        unsigned long _sampleCount;
};
