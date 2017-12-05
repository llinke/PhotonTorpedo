/*
 * Project PhotonTorpedo
 * Description:
 * Author:
 * Date:
 */

#include "neopixel.h"
#include "NeoPatterns.cpp"

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 32
#define PIXEL_TYPE WS2812B

void StickComplete();

// Define some NeoPatterns for the two rings and the stick
//  as well as some completion routines
NeoPatterns Stick = NeoPatterns(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE, &StickComplete);

// Initialize everything and prepare to start
void setup()
{
    //Serial.begin(115200);

    //pinMode(8, INPUT_PULLUP);
    //pinMode(9, INPUT_PULLUP);

    // Initialize all the pixelStrips
    Stick.begin();

    // Kick off a pattern
    Stick.RainbowCycle(3);
}

// Main loop
void loop()
{
    Stick.Update();
}

//------------------------------------------------------------
//Completion Routines - get called on completion of a pattern
//------------------------------------------------------------
void StickComplete()
{
    // Random color change for next scan
    Stick.Color1 = Stick.Wheel(random(255));
}
