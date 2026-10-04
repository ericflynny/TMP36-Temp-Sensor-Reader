#include <Arduino.h>
#include "tempSystem.hpp"


int main(void)
{
    init(); // Required Arduino hardware initialization (timers, ADC, etc.)

    // Setup serial port and wait for enumeration
    Serial.begin(115200);
    while (!Serial){ ; }
    delay(500);

    // Setup and run temperature system
    TempSystem tempSystem;
    tempSystem.init();
    tempSystem.process();

    Serial.println("System halted. Press Reset button on board to restart.");

    return 0;
}
