#include "src/tempSystem.hpp"

#ifndef PIO_BUILD
void setup()
{
    // Setup serial port and wait for enumeration
    Serial.begin(115200);
    while (!Serial){ ; }
    delay(500);

    // Setup and run temperature system
    TempSystem tempSystem;
    tempSystem.init();
    tempSystem.process();

    Serial.println("System halted. Press Reset button on board to restart.");
}

void loop() { } // tempSystem.process() controls loop
#endif
