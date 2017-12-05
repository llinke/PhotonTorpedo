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

// Define some NeoPatterns for the stick as well as some completion routines
NeoPatterns Stick = NeoPatterns(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE, &StickComplete);
//NeoPatterns Stick = new NeoPatterns();
bool started = false;

// Initialize everything and prepare to start
void setup()
{
  Particle.function("initStrip",initStrip);
  Particle.function("setRainbow",setRainbow);
  //Particle.function("setScanner",setScanner);
  Particle.function("setFade",setFade);
}

int initStrip(String args)
{
  //Stick = NeoPatterns(pixelCount, PIXEL_PIN, PIXEL_TYPE, &StickComplete);
  // Initialize all the pixelStrips
  Stick.begin();
  started = true;
  return 0;
}

int setRainbow(String args)
{
  Stick.RainbowCycle(3);
  return 0;
}

int setScanner(String args)
{
  Stick.Scanner((uint32_t)0x00204080, 3);
  return 0;
}

int setFade(String args)
{
  Stick.Fade((uint32_t)0x00204080, (uint32_t)0x00080402, (uint16_t)8, 3);
  return 0;
}

// Main loop
void loop()
{
  if (!started) return;

  Stick.Update();
}

//------------------------------------------------------------
//Completion Routines - get called on completion of a pattern
//------------------------------------------------------------
void StickComplete()
{
    Stick.Color1 = Stick.Wheel(random(255));
}
