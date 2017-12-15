/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "FastLedInclude.h"
#include "ColorPalettes.h"
#include "NeoGroup.cpp"
#include "SparkJson.h"
#include <vector>

SYSTEM_MODE(AUTOMATIC);

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

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
	Particle.function("startStrip", startStrip);
	Particle.function("stopStrip", stopStrip);

	Particle.function("addGroup", stopStrip);

	Particle.function("setEffect", setEffect);
	Particle.function("setColors", setColors);

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

int initStrip(int ledCount, bool doStart = true, bool playDemo = true)
{
	if (initialized)
	{
		return doStart ? startStrip("") : pixelCount;
	}

	leds = (struct CRGB *)malloc(ledCount * sizeof(struct CRGB));
	FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN>(leds, ledCount);
	//FastLED.setMaxPowerInVoltsAndMilliamps(5,3000);
	FastLED.setBrightness(64);
	FastLED.clear(true);
	FastLED.show();

	if (playDemo)
	{
		for (int dot = 0; dot < ledCount; dot++)
		{
			leds[dot] = CHSV(random8(), 255, 255);
			FastLED.show();
			delay(10);
		}
		delay(500);
		for (int dot = 0; dot < 20; dot++)
		{
			fadeToBlackBy(leds, ledCount, 20);
			FastLED.show();
			delay(50);
		}
		FastLED.clear(true);
		FastLED.show();
	}

	neoGroups.clear();
	// Group 0: all LEDs
	addGroup("All LEDs", 0, pixelCount);
	/*
	// Right Wing
	addGroup("Right Wing", 0, 9);
	// Small Room
	addGroup("Small Room", 10, 16);
	// Gate
	addGroup("Gate", 17, 26);
	// Main Hall
	addGroup("Main Hall", 32, 47);
	*/

	pixelCount = ledCount;
	initialized = true;
	return doStart ? startStrip("") : pixelCount;
}

int initStrip(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
		return -1; // Invalid JSon arguments
	int ledCount = (jsonArgs.containsKey("ledC")) ? jsonArgs["ledC"] : 0;
	return initStrip(ledCount);
}

int startStrip(String args)
{
	if (!initialized)
		return -1;

	started = true;
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

int addGroup(String grpId, int ledFirst, int ledCount)
{
	if ((ledFirst >= pixelCount) ||
		(ledCount <= 0) ||
		(ledFirst + ledCount) >= pixelCount)
		return -2; // Invalid parameter

	NeoGroup *newGroup = new NeoGroup(grpId, ledFirst, ledCount);
	neoGroups.push_back(newGroup);
	return neoGroups.size();
}

int addGroup(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
		return -1; // Invalid JSon arguments
	String grpId = (jsonArgs.containsKey("id")) ? jsonArgs["id"] : "";
	int ledFirst = (jsonArgs.containsKey("ledF")) ? jsonArgs["ledF"] : 0;
	int ledCount = (jsonArgs.containsKey("ledC")) ? jsonArgs["ledC"] : 0;
	return addGroup(grpId, ledFirst, ledCount);
}

int setEffect(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
		if (!jsonArgs.success())
			return -1;
	int grpNr = (jsonArgs.containsKey("grp")) ? jsonArgs["grp"] : -1;
	if (grpNr < 0 || grpNr >= neoGroups.size())
	{
		return -2;
	}
	NeoGroup *neoGroup = neoGroups.at(grpNr);
	neoGroup->Stop();
	String fxName = (jsonArgs.containsKey("fx")) ? jsonArgs["fx"] : "";
	pattern fxPattern = pattern::NONE;
	if (fxName == "static")
		fxPattern = pattern::STATIC;
	if (fxName == "rainbow")
		fxPattern = pattern::RAINBOW;
	if (fxName == "confetti")
		fxPattern = pattern::CONFETTI;
	if (fxName == "fade")
		fxPattern = pattern::FADE;
	if (fxName == "fire")
		fxPattern = pattern::FIRE;
	bool fxGlitter = (jsonArgs.containsKey("glt")) ? jsonArgs["glt"] : false;
	int fxFps = (jsonArgs.containsKey("fps")) ? jsonArgs["fps"] : 50;
	String parmDir = (jsonArgs.containsKey("dir")) ? jsonArgs["dir"] : "f";
	direction fxDir = direction::FORWARD;
	if (parmDir == "f")
		fxDir = direction::FORWARD;
	if (parmDir == "r")
		fxDir = direction::REVERSE;
	/*
	uint16_t ConfigureEffect(
		pattern pattern,
		bool addglitter = false,
		uint8_t fps = 50,
		direction direction = FORWARD)
	*/
	uint16_t result = neoGroup->ConfigureEffect(fxPattern, fxGlitter, fxFps, fxDir);
	neoGroup->Start();
	return result;
}

int setColors(String args)
{
	JsonObject &jsonArgs = parseArgs(args);
	if (!jsonArgs.success())
		if (!jsonArgs.success())
			return -1;
	int grpNr = (jsonArgs.containsKey("grp")) ? jsonArgs["grp"] : -1;
	if (grpNr < 0 || grpNr >= neoGroups.size())
	{
		return -2;
	}
	NeoGroup *neoGroup = neoGroups.at(grpNr);
	//neoGroup->Stop();

	String palKey = "";
	bool clearFirst = true;
	bool genPalette = true;
	if (jsonArgs.containsKey("pal"))
	{
		//String palKey = jsonArgs["pal"].as<String>();
		String palKey = (jsonArgs.containsKey("pal")) ? jsonArgs["pal"] : "";
		if (ColorPalettes.find(palKey) == ColorPalettes.end())
		{
			return -2;
		}
		std::vector<CRGB> colors = ColorPalettes.find(palKey)->second;

		uint16_t result = neoGroup->ConfigureColors(colors, clearFirst, genPalette);
		//neoGroup->Start();
		return result;
	}
	else
	{
		bool clearFirst = (jsonArgs.containsKey("clr")) ? jsonArgs["clr"] : true;
		bool genPalette = (jsonArgs.containsKey("gen")) ? jsonArgs["gen"] : true;
		std::vector<CRGB> colors = {};
		JsonArray &colorsArray = jsonArgs["cols"];
		if (colorsArray != JsonArray::invalid())
		{
			for (int c = 0; c < colorsArray.size(); c++)
			{
				uint32_t colVal = colorsArray[c];
				colors.push_back(CRGB(colVal));
			}
		}
		uint16_t result = neoGroup->ConfigureColors(colors, clearFirst, genPalette);
		//neoGroup->Start();
		return result;
	}
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
