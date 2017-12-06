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

// Define some NeoPatterns for the lightstrip as well as some completion routines
Adafruit_NeoPixel lightstrip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
NeoPatterns *neostrip;
bool started = false;
int pixelCount = PIXEL_COUNT;

// Initialize everything and prepare to start
void setup()
{
	Particle.function("initStrip",initStrip);
	Particle.function("stopStrip",stopStrip);
	Particle.function("setRainbow",setRainbow);
	Particle.function("setWipe",setWipe);
	Particle.function("setFade",setFade);

	Particle.variable("countLeds", pixelCount);
	Particle.variable("isStarted", started);
}

int initStrip(String args)
{
	neostrip = new NeoPatterns((&lightstrip));
	lightstrip.setBrightness(64);
	lightstrip.begin();
	lightstrip.clear();
	lightstrip.show();
	started = true;
	return lightstrip.numPixels();
}

int stopStrip(String args)
{
	started = false;
	lightstrip.clear();
	lightstrip.show();
	return 0;
}

int setRainbow(String args)
{
	neostrip->RainbowCycle(10);
	return 0;
}

int setWipe(String args)
{
	neostrip->ColorWipe((uint32_t)0x0000ff00,(uint32_t)0x00007f7f, 50);
	return 0;
}

int setFade(String args)
{
	neostrip->Fade((uint32_t)0x000000ff, (uint32_t)0x00ff0000, (uint16_t)64, 10);
	return 0;
}

// Main loop
void loop()
{
	if (!started) return;

	neostrip->Update();
	pixelCount = lightstrip.numPixels();
}
