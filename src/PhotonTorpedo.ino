/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "neopixel.h"
//#include "NeoPatterns.cpp"
#include "NeoGroup.cpp"
#include <vector>

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 32
#define PIXEL_TYPE WS2812B

// Define some NeoPatterns for the lightstrip as well as some completion routines
Adafruit_NeoPixel lightstrip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
//NeoPatterns *neostrip;
NeoGroup *neoGroup;
bool started = false;
int pixelCount = PIXEL_COUNT;

// Initialize everything and prepare to start
void setup()
{
	Particle.function("initStrip", initStrip);
	Particle.function("stopStrip", stopStrip);

	Particle.function("setStatic", setStatic);
	Particle.function("setRainbow", setRainbow);
	Particle.function("setWipe", setWipe);
	Particle.function("setFade", setFade);

	Particle.variable("countLeds", pixelCount);
	Particle.variable("isStarted", started);
}

int initStrip(String args)
{
	lightstrip.setBrightness(64);
	lightstrip.begin();
	lightstrip.clear();
	lightstrip.show();
	pixelCount = lightstrip.numPixels();
	//neostrip = new NeoPatterns((&lightstrip));
	neoGroup = new NeoGroup((&lightstrip), 0, 0, pixelCount - 1);
	//neoGroup = new NeoGroup((&lightstrip), 0, 0 + 4, pixelCount - 1 - 4);
	started = true;
	return pixelCount;
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
	//neostrip->RainbowCycle(10);
	std::vector<uint32_t> colors = {};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(RAINBOW, colors, 10, FORWARD);
	neoGroup->Start();
	return result;
}

int setWipe(String args)
{
	//neostrip->ColorWipe((uint32_t)0x0000ff00, (uint32_t)0x00007f7f, 50);
	std::vector<uint32_t> colors = { 0xffff0000, 0xff7f7f00, 0xff00ff00, 0xff007f7f, 0xff0000ff, 0xff7f007f};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(WIPE, colors, 25, FORWARD);
	neoGroup->Start();
	return result;
}

int setFade(String args)
{
	//neostrip->Fade((uint32_t)0x000000ff, (uint32_t)0x00ff0000, (uint16_t)64, 10);
	std::vector<uint32_t> colors = { 0xffff0000, 0xff00ff00, 0x00000000, 0xff0000ff, 0xff00ff00, 0x00000000};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(FADE, colors, 10, FORWARD);
	neoGroup->Start();
	return result;
}

int setStatic(String args)
{
	//neostrip->Fade((uint32_t)0x000000ff, (uint32_t)0x00ff0000, (uint16_t)64, 10);
	std::vector<uint32_t> colors = { 0xff007f7f, 0xff7f007f, 0xff7f7f00};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(STATIC, colors, 10, FORWARD);
	neoGroup->Start();
	return result;
}

// Main loop
void loop()
{
	if (!started)
		return;

	//neostrip->Update();
	neoGroup->Update();
	pixelCount = lightstrip.numPixels();
}
