/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "FastLedInclude.h"
#include "NeoGroup.cpp"
#include <vector>

SYSTEM_MODE(AUTOMATIC);

// Static size
//struct CRGB leds[PIXEL_COUNT];
// Dynamic size:
struct CRGB *leds = NULL;

NeoGroup *neoGroup1;
NeoGroup *neoGroup2;

bool started = false;
int pixelCount = PIXEL_COUNT;

void setup()
{
	Particle.function("initStrip", initStrip);
	Particle.function("stopStrip", stopStrip);

	Particle.function("setStatic", setStatic);
	Particle.function("setRainbow", setRainbow);
	Particle.function("setWipe", setWipe);
	Particle.function("setFade", setFade);

	Particle.function("setStatic2", setStatic2);
	Particle.function("setRainbow2", setRainbow2);
	Particle.function("setWipe2", setWipe2);
	Particle.function("setFade2", setFade2);

	Particle.variable("countLeds", pixelCount);
	Particle.variable("isStarted", started);
}

int initStrip(String args)
{
	leds = (struct CRGB *) malloc(PIXEL_COUNT * sizeof(struct CRGB));
	FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN>(leds, PIXEL_COUNT);

	//FastLED.setMaxPowerInVoltsAndMilliamps(5,3000);
	FastLED.setBrightness(64);
	FastLED.clear(true);
	FastLED.show();

	// TEST
	for(int dot = 0; dot < PIXEL_COUNT; dot++)
	{
			leds[dot] =  CHSV(random8(),255,255);
      FastLED.show();
      delay(10);
  }
	delay(1000);
	FastLED.clear(true);
	FastLED.show();

	neoGroup1 = new NeoGroup(0, 0, (PIXEL_COUNT / 2) - 1);
	neoGroup2 = new NeoGroup(1, (PIXEL_COUNT / 2), PIXEL_COUNT - 1);

	pixelCount = PIXEL_COUNT;
	started = true;
	return pixelCount;
}

int stopStrip(String args)
{
	started = false;

	neoGroup1->Stop();
	neoGroup2->Stop();

	FastLED.clear(true);
	FastLED.show();

	return 0;
}

int setRainbow(String args)
{
	std::vector<uint32_t> colors = {};
	neoGroup1->Stop();
	uint16_t result = neoGroup1->ConfigureEffect(RAINBOW, colors, 10, FORWARD);
	neoGroup1->Start();
	return result;
}

int setWipe(String args)
{
	std::vector<uint32_t> colors = { 0xffff0000, 0xff7f7f00, 0xff00ff00, 0xff007f7f, 0xff0000ff, 0xff7f007f};
	neoGroup1->Stop();
	uint16_t result = neoGroup1->ConfigureEffect(WIPE, colors, 25, FORWARD);
	neoGroup1->Start();
	return result;
}

int setFade(String args)
{
	std::vector<uint32_t> colors = { 0xffff0000, 0xff00ff00, 0x00000000, 0xff0000ff, 0xff00ff00, 0x00000000};
	neoGroup1->Stop();
	uint16_t result = neoGroup1->ConfigureEffect(FADE, colors, 10, FORWARD);
	neoGroup1->Start();
	return result;
}

int setStatic(String args)
{
	std::vector<uint32_t> colors = { 0xff007f7f, 0xff7f007f, 0xff7f7f00};
	neoGroup1->Stop();
	uint16_t result = neoGroup1->ConfigureEffect(STATIC, colors, 10, FORWARD);
	neoGroup1->Start();
	return result;
}

int setRainbow2(String args)
{
	std::vector<uint32_t> colors = {};
	neoGroup2->Stop();
	uint16_t result = neoGroup2->ConfigureEffect(RAINBOW, colors, 10, FORWARD);
	neoGroup2->Start();
	return result;
}

int setWipe2(String args)
{
	std::vector<uint32_t> colors = { 0xffff0000, 0xff7f7f00, 0xff00ff00, 0xff007f7f, 0xff0000ff, 0xff7f007f};
	neoGroup2->Stop();
	uint16_t result = neoGroup2->ConfigureEffect(WIPE, colors, 25, FORWARD);
	neoGroup2->Start();
	return result;
}

int setFade2(String args)
{
	std::vector<uint32_t> colors = { 0xffff0000, 0xff00ff00, 0x00000000, 0xff0000ff, 0xff00ff00, 0x00000000};
	neoGroup2->Stop();
	uint16_t result = neoGroup2->ConfigureEffect(FADE, colors, 10, FORWARD);
	neoGroup2->Start();
	return result;
}

int setStatic2(String args)
{
	std::vector<uint32_t> colors = { 0xff007f7f, 0xff7f007f, 0xff7f7f00};
	neoGroup2->Stop();
	uint16_t result = neoGroup2->ConfigureEffect(STATIC, colors, 10, FORWARD);
	neoGroup2->Start();
	return result;
}

// Main loop
void loop()
{
	if (!started)
		return;

	bool autoShow = false;
	neoGroup1->Update(autoShow);
	neoGroup2->Update(autoShow);
	if (!autoShow)
	{
		FastLED.show();
	}
	pixelCount = PIXEL_COUNT;
}