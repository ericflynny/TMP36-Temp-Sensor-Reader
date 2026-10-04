// This class implements a timer interrupt of dynamic length
#include "timer.hpp"


// Define static member variables
volatile bool Timer1::_needsHandling = false;
volatile unsigned long Timer1::_tickCount = 0;
volatile unsigned long Timer1::_subTicks = 0;
unsigned long Timer1::_targetSubTicks = 1;

Timer1::Timer1(unsigned long durationMs)
    : _durationMs(durationMs > 0 ? durationMs : 10000UL)
{
    _needsHandling = false;
}

Timer1::Timer1(int durationMs)
    : _durationMs(durationMs > 0 ? static_cast<unsigned long>(durationMs) : 10000UL)
{
    _needsHandling = false;
}

void Timer1::init()
{
    setupTimer1();
}

void Timer1::setDuration(unsigned long durationMs)
{
    _durationMs = durationMs > 0 ? durationMs : 1000UL;
    setupTimer1();
}

void Timer1::setDurationSeconds(unsigned int seconds)
{
    setDuration(static_cast<unsigned long>(seconds) * 1000UL);
}

// Writes values to Timer 1's configuration registers to setup timing dynamically
void Timer1::setupTimer1()
{
    // Disable global interrupts while configuring
    cli();

    // Setup registers
    TCCR1A = 0;  // Normal port operation, CTC mode (WGM11=0, WGM10=0)
    TCCR1B = 0;  // Clear control register B
    TCNT1  = 0;  // Reset counter register

    _tickCount = 0;
    _subTicks = 0;
    _needsHandling = false;

    // Dynamically set prescaler and OCR1A register based off duration
    // Clock = 16 MHz
    // Prescaler 64:   250,000 Hz (250 counts/ms)  -> Max interval: 65536 / 250 = 262 ms
    // Prescaler 256:   62,500 Hz (62.5 counts/ms) -> Max interval: 65536 / 62.5 = 1048 ms
    // Prescaler 1024:  15,625 Hz (15.625 counts/ms)-> Max interval: 65536 / 15.625 = 4194 ms
    if (_durationMs <= 260)
    {
        // Prescaler 64: counts = (16 MHz / 64) * (durationMs / 1000) = 250 * durationMs
        // Example: for 10ms -> 250 * 10 - 1 = 2499
        OCR1A = static_cast<uint16_t>((250UL * _durationMs) - 1);
        TCCR1B |= (1 << WGM12);              // CTC mode (Clear Timer on Compare match)
        TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler = 64
        _targetSubTicks = 1;
    }
    else if (_durationMs <= 1000)
    {
        // Prescaler 256: counts = (16 MHz / 256) * (durationMs / 1000) = (625 * durationMs) / 10
        // Example: for 1000ms -> 62500 - 1 = 62499
        OCR1A = static_cast<uint16_t>(((625UL * _durationMs) / 10UL) - 1);
        TCCR1B |= (1 << WGM12);              // CTC mode
        TCCR1B |= (1 << CS12);               // Prescaler = 256
        _targetSubTicks = 1;
    }
    else if (_durationMs <= 4000)
    {
        // Prescaler 1024: counts = (16 MHz / 1024) * (durationMs / 1000) = (15625 * durationMs) / 1000
        // Example: for 4000ms -> 62500 - 1 = 62499
        OCR1A = static_cast<uint16_t>(((15625UL * _durationMs) / 1000UL) - 1);
        TCCR1B |= (1 << WGM12);              // CTC mode
        TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler = 1024
        _targetSubTicks = 1;
    }
    else
    {
        // Duration exceeds maximum 16-bit hardware compare interval (~4.194 seconds).
        // Configure hardware timer for 1-second base tick (Prescaler 256, OCR1A = 62499),
        // and count sub-ticks in software until the requested duration is reached.
        OCR1A = 62499;
        TCCR1B |= (1 << WGM12);              // CTC mode
        TCCR1B |= (1 << CS12);               // Prescaler = 256 (1000 ms per tick)
        _targetSubTicks = (_durationMs + 500UL) / 1000UL;
    }

    TIFR1  |= (1 << OCF1A);  // Clear any pending Timer 1 compare-A interrupt flag
    TIMSK1 |= (1 << OCIE1A); // Enable Timer 1 compare-A interrupt

    // Re-enable global interrupts
    sei();
}

void Timer1::isrHandler()
{
    _tickCount++;
    _subTicks++;
    if (_subTicks >= _targetSubTicks)
    {
        _needsHandling = true; // Sets flag for handling outside of ISR
        _subTicks = 0;
    }
}

void Timer1::clearHandlingFlag()
{
    cli();
    _needsHandling = false;
    sei();
}

bool Timer1::checkAndClear()
{
    if (_needsHandling)
    {
        cli();
        _needsHandling = false;
        sei();
        return true;
    }
    return false;
}

void Timer1::resetTickCount()
{
    cli();
    _tickCount = 0;
    sei();
}

ISR(TIMER1_COMPA_vect)
{
    Timer1::isrHandler();
}
