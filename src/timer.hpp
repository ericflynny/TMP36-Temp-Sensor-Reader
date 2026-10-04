#pragma once
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>


class Timer1
{
    public:
        // Constructors accepting duration in milliseconds
        explicit Timer1(unsigned long durationMs = 10000); // default: 10000 ms = 10 s
        explicit Timer1(int durationMs);

        // Hardware initialization
        void init();

        // Dynamically calculate prescaler and OCR1A register, then configure Timer 1
        void setupTimer1();

        // Flag and interrupt handling
        bool needsHandling() const { return _needsHandling; }
        void clearHandlingFlag();
        bool checkAndClear();

        // Duration getters and setters
        unsigned long getDuration() const { return _durationMs; }
        void setDuration(unsigned long durationMs);
        void setDurationSeconds(unsigned int seconds);

        // Tick count statistics
        unsigned long getTickCount() const { return _tickCount; }
        void resetTickCount();

        // ISR handler invoked by TIMER1_COMPA_vect
        static void isrHandler();

    private:
        unsigned long _durationMs;

        // Shared static state accessed in ISR
        static volatile bool _needsHandling;
        static volatile unsigned long _tickCount;
        static volatile unsigned long _subTicks;
        static unsigned long _targetSubTicks;
};
