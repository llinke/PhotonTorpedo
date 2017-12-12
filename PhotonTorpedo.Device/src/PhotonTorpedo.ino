/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "FastLedInclude.h"
#include "NeoGroup.cpp"
#include "SparkJson.h"
#include <vector>

SYSTEM_MODE(AUTOMATIC);

// Static size
//struct CRGB leds[PIXEL_COUNT];
// Dynamic size:
struct CRGB *leds = NULL;

//NeoGroup *neoGroup1;
//NeoGroup *neoGroup2;
std::vector<NeoGroup*> neoGroups;

bool initialized = false;
bool started = false;
int pixelCount = PIXEL_COUNT;
int groupCount = 0;

void setup()
{
	Particle.function("initStrip", initStrip);
	Particle.function("stopStrip", stopStrip);

	Particle.function("setStatic", setStatic);
	Particle.function("setRainbow", setRainbow);
	Particle.function("setWipe", setWipe);
	Particle.function("setFade", setFade);

	Particle.variable("countLeds", pixelCount);
	Particle.variable("countGroups", groupCount);
	Particle.variable("isStarted", started);
}

JsonObject& parseArgs(String args)
{
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	//JsonObject& jsonArgs =
	return jsonBuffer.parseObject(argsbuf);
}

int initStrip(String args)
{
	if (initialized)
	{
		started = true;
		return pixelCount;
	}

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

	neoGroups.clear();
	// Group 1
	NeoGroup *neoGroup1 = new NeoGroup(0, 0, (PIXEL_COUNT / 2) - 1);
	neoGroups.push_back(neoGroup1);
	//neoGroups.push_back(NeoGroup(0, 0, (PIXEL_COUNT / 2) - 1));
	// Group 2
	NeoGroup *neoGroup2 = new NeoGroup(1, (PIXEL_COUNT / 2), PIXEL_COUNT - 1);
	neoGroups.push_back(neoGroup2);
	//neoGroups.push_back(NeoGroup(1, (PIXEL_COUNT / 2), PIXEL_COUNT - 1));

	pixelCount = PIXEL_COUNT;
	started = true;
	initialized = true;
	return pixelCount;
}

int stopStrip(String args)
{
	started = false;

	for(int i=0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = neoGroups.at(i);
		neoGroup->Stop();
	}

	FastLED.clear(true);
	FastLED.show();

	return 0;
}

int setRainbow(String args)
{
	JsonObject& jsonArgs = parseArgs(args);
	if (!jsonArgs.success()) { return -1; }
	int grpId = -1;
	if (jsonArgs.containsKey("grp")) { grpId = jsonArgs["grp"]; }
	if (grpId < 0 || grpId >= neoGroups.size()) { return -2; }

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<uint32_t> colors = {};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(RAINBOW, colors, 10, FORWARD);
	neoGroup->Start();
	return result;
}

int setWipe(String args)
{
	JsonObject& jsonArgs = parseArgs(args);
	if (!jsonArgs.success()) { return -1; }
	int grpId = -1;
	if (jsonArgs.containsKey("grp")) { grpId = jsonArgs["grp"]; }
	if (grpId < 0 || grpId >= neoGroups.size()) { return -2; }

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<uint32_t> colors = { 0xffff0000, 0xff7f7f00, 0xff00ff00, 0xff007f7f, 0xff0000ff, 0xff7f007f};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(WIPE, colors, 25, FORWARD);
	neoGroup->Start();
	return result;
}

int setFade(String args)
{
	JsonObject& jsonArgs = parseArgs(args);
	if (!jsonArgs.success()) { return -1; }
	int grpId = -1;
	if (jsonArgs.containsKey("grp")) { grpId = jsonArgs["grp"]; }
	if (grpId < 0 || grpId >= neoGroups.size()) { return -2; }

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<uint32_t> colors = { 0xffff0000, 0xff00ff00, 0x00000000, 0xff0000ff, 0xff00ff00, 0x00000000};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(FADE, colors, 10, FORWARD);
	neoGroup->Start();
	return result;
}

int setStatic(String args)
{
	JsonObject& jsonArgs = parseArgs(args);
	if (!jsonArgs.success()) { return -1; }
	int grpId = -1;
	if (jsonArgs.containsKey("grp")) { grpId = jsonArgs["grp"]; }
	if (grpId < 0 || grpId >= neoGroups.size()) { return -2; }

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<uint32_t> colors = { 0xff007f7f, 0xff7f007f, 0xff7f7f00};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(STATIC, colors, 10, FORWARD);
	neoGroup->Start();
	//return neoGroup->LedLast - neoGroup->LedFirst + 1;
	return result;
}

// Main loop
void loop()
{
	if (!started)
		return;

	bool autoShow = false;
	int count = 0;
	for(int i=0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = neoGroups.at(i);
		count += neoGroup->LedCount;
		neoGroup->Update(autoShow);
	}
	if (!autoShow)
	{
		FastLED.show();
	}
	pixelCount = count; //PIXEL_COUNT;
	groupCount = neoGroups.size();
}
