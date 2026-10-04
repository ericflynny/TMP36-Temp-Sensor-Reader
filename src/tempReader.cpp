#include "tempReader.hpp"


TempReader::TempReader(uint8_t sensorPin, float vRef)
    : _sensorPin(sensorPin),
      _vRef(vRef),
      _lastTempC(0.0f),
      _lastTempF(0.0f),
      _lastVoltage(0.0f),
      _lastAdc(0),
      _isStable(false)
{
}

void TempReader::init()
{
    // Configure sensor analog pin and onboard status LED
    pinMode(_sensorPin, INPUT);
    pinMode(LedPin, OUTPUT);
    setLed(false);

    // Initial discard read to settle ADC multiplexer
    analogRead(_sensorPin);
    delay(20);

    // Take readings until temperature (voltage) stabilizes
    constexpr uint8_t RequiredStableSamples = 5;
    constexpr int MaxAllowedAdcDifference   = 4; // ~19.5 mV (approx 1.95 °C) stability tolerance
    constexpr uint8_t MaxAttempts           = 30;
    int previousReading = -1;
    uint8_t stableCount = 0;
    for (uint8_t attempt = 0; attempt < MaxAttempts; ++attempt)
    {
        int currentReading = readRawAdc();

        if (previousReading >= 0 && abs(currentReading - previousReading) <= MaxAllowedAdcDifference)
        {
            stableCount++;
            if (stableCount >= RequiredStableSamples)
            {
                _isStable = true;
                break;
            }
        }
        else
        {
            stableCount = 0;
        }

        previousReading = currentReading;
        delay(50);
    }

    // Capture baseline reading
    process();

    // Blink LED twice to signal sensor initialized and stable
    setLed(true);
    delay(80);
    setLed(false);
    delay(80);
    setLed(true);
    delay(80);
    setLed(false);
}

int TempReader::readRawAdc()
{
    // Discard initial read when sampling to allow sample & hold capacitor settling
    analogRead(_sensorPin);
    delayMicroseconds(400);

    // Average 16 samples to filter high-frequency analog noise
    unsigned long sum = 0;
    constexpr uint8_t sampleCount = 16;
    for (uint8_t i = 0; i < sampleCount; ++i)
    {
        sum += analogRead(_sensorPin);
        delayMicroseconds(200);
    }

    _lastAdc = static_cast<int>((sum + (sampleCount / 2)) / sampleCount);
    return _lastAdc;
}

float TempReader::readVoltage()
{
    int raw = readRawAdc();
    // 10-bit ADC has 1024 discrete levels (0 to 1023)
    _lastVoltage = (static_cast<float>(raw) * _vRef) / 1024.0f;
    return _lastVoltage;
}

float TempReader::readTemperatureC()
{
    float v = readVoltage();
    _lastTempC = voltageToC(v);
    _lastTempF = cToF(_lastTempC);
    return _lastTempC;
}

float TempReader::readTemperatureF()
{
    readTemperatureC();
    return _lastTempF;
}

void TempReader::process()
{
    readTemperatureC();
}
