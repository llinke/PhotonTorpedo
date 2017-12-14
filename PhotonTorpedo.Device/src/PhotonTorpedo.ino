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
std::vector<NeoGroup *> neoGroups;

bool initialized = false;
bool started = false;
int pixelCount = PIXEL_COUNT;
int groupCount = 0;

void setup()
{
	Particle.function("initStrip", initStrip);
	Particle.function("stopStrip", stopStrip);

	Particle.function("setStatic", setStatic);
	Particle.function("setStatic2", setStatic2);
	Particle.function("setFade", setFade);
	Particle.function("setFade2", setFade2);
	Particle.function("setFade3", setFade3);
	Particle.function("setRainbow", setRainbow);
	Particle.function("setRainbow2", setRainbow2);
	Particle.function("setConfetti", setConfetti);
	Particle.function("setFire", setFire);

	Particle.variable("countLeds", pixelCount);
	Particle.variable("countGroups", groupCount);
	Particle.variable("isStarted", started);
}

JsonObject &parseArgs(String args)
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

	leds = (struct CRGB *)malloc(PIXEL_COUNT * sizeof(struct CRGB));
	FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN>(leds, PIXEL_COUNT);
	//FastLED.setMaxPowerInVoltsAndMilliamps(5,3000);
	FastLED.setBrightness(64);
	FastLED.clear(true);
	FastLED.show();

	// TEST
	for (int dot = 0; dot < PIXEL_COUNT; dot++)
	{
		leds[dot] = CHSV(random8(), 255, 255);
		FastLED.show();
		delay(10);
	}
	delay(500);
	for (int dot = 0; dot < 20; dot++)
	{
		fadeToBlackBy(leds, PIXEL_COUNT, 20);
		FastLED.show();
		delay(50);
	}
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

	/*
	// Right Wing
	NeoGroup *ngRoom1 = new NeoGroup(1, 0, 9);
	neoGroups.push_back(ngRoom1);
	// Small Room
	NeoGroup *ngRoom2 = new NeoGroup(2, 10, 16);
	neoGroups.push_back(ngRoom2);
	// Gate
	NeoGroup *ngRoom3 = new NeoGroup(3, 17, 26);
	neoGroups.push_back(ngRoom3);
	// Main Hall
	NeoGroup *ngRoom4 = new NeoGroup(4, 32, 47);
	neoGroups.push_back(ngRoom4);
*/

	pixelCount = PIXEL_COUNT;
	started = true;
	initialized = true;
	return pixelCount;
}

int stopStrip(String args)
{
	started = false;

	for (int i = 0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = neoGroups.at(i);
		neoGroup->Stop();
	}

	FastLED.clear(true);
	FastLED.show();

	return 0;
}

int setStatic(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors = {CRGB(0x007f7f)};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(STATIC, colors, 50, FORWARD, false);
	neoGroup->Start();
	//return neoGroup->LedLast - neoGroup->LedFirst + 1;
	return result;
}

int setStatic2(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors = {CRGB(0x007f7f)};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(STATIC, colors, 50, FORWARD, true);
	neoGroup->Start();
	//return neoGroup->LedLast - neoGroup->LedFirst + 1;
	return result;
}

int setFade(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors =
		{
			CRGB(0x0000ff),
			CRGB(0x00ff00),
			CRGB(0xff0000)};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(FADE, colors, 50, FORWARD);
	neoGroup->Start();
	return result;
}

int setFade2(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors =
		{
			CRGB(0x0000ff),
			CRGB(0xffffff),
			CRGB(0x00ff00),
			CRGB(0xffffff),
			CRGB(0xff0000),
			CRGB(0xffffff)};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(FADE, colors, 50, FORWARD);
	neoGroup->Start();
	return result;
}

int setFade3(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors =
		{
			CRGB(0x0000ff),
			CRGB(0x00ff00),
			CRGB(0xff0000)};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(FADE, colors, 50, FORWARD, true);
	neoGroup->Start();
	return result;
}

int setRainbow(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors = {};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(RAINBOW, colors, 50, FORWARD, false);
	neoGroup->Start();
	return result;
}

int setRainbow2(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors = {};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(RAINBOW, colors, 50, FORWARD, true);
	neoGroup->Start();
	return result;
}

int setConfetti(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors = {};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(CONFETTI, colors, 25, FORWARD);
	neoGroup->Start();
	return result;
}

int setFire(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
	{
		return -1;
	}
	int grpId = -1;
	if (jsonArgs.containsKey("grp"))
	{
		grpId = jsonArgs["grp"];
	}
	if (grpId < 0 || grpId >= neoGroups.size())
	{
		return -2;
	}

	NeoGroup *neoGroup = neoGroups.at(grpId);

	std::vector<CRGB> colors = {};
	neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureEffect(FIRE, colors, 50, FORWARD);
	neoGroup->Start();
	return result;
}

// Main loop
void loop()
{
	if (!started)
		return;

	int count = 0;
	for (int i = 0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = neoGroups.at(i);
		count += neoGroup->LedCount;
		neoGroup->Update();
	}
	FastLED.show();
	pixelCount = count; //PIXEL_COUNT;
	groupCount = neoGroups.size();
}
