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

/*
JsonObject &parseArgs(String args)
{
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	//JsonObject& jsonArgs =
	return jsonBuffer.parseObject(argsbuf);
}
*/

int initStripInternal(int ledCount, bool doStart = false, bool playDemo = true)
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

	pixelCount = ledCount;
	initialized = true;

	if (playDemo)
	{
		for (int dot = 0; dot < ledCount; dot++)
		{
			leds[dot] = CHSV(random8(), 255, 255);
			FastLED.show();
			delay(10);
		}
		delay(500);
		for (int fade = 0; fade < 20; fade++)
		{
			fadeToBlackBy(leds, ledCount, 20);
			FastLED.show();
			delay(50);
		}
		FastLED.clear(true);
		FastLED.show();
	}

	int offset = 4;
	neoGroups.clear();
	// Group 0: all LEDs
	addGroupInternal("All LEDs", 0, pixelCount, offset);
	/*
	// Right Wing
	addGroupInternal("Right Wing", 0, 9);
	// Small Room
	addGroupInternal("Small Room", 10, 16);
	// Gate
	addGroupInternal("Gate", 17, 26);
	// Main Hall
	addGroupInternal("Main Hall", 32, 47);
	*/

	return doStart ? startStrip("") : pixelCount;
}

int initStrip(String args)
{
	//JsonObject &jsonArgs = parseArgs(args);
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	JsonObject &jsonArgs = jsonBuffer.parseObject(argsbuf);

	if (!jsonArgs.success())
		return -1; // Invalid JSon arguments
	int ledCount = (jsonArgs.containsKey("ledC")) ? jsonArgs["ledC"] : 0;
	return initStripInternal(ledCount);
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
		neoGroup->Stop(true);
	}

	FastLED.clear(true);
	FastLED.show();

	return 0;
}

int addGroupInternal(String grpId, int ledFirst, int ledCount, int ledOffset)
{
	if ((ledFirst >= pixelCount) ||
		(ledCount <= 0) ||
		(ledFirst + ledCount) > pixelCount)
		return -((((3 * 1000) + ledFirst) * 1000) + ledCount); // Invalid parameter

	NeoGroup *newGroup = new NeoGroup(grpId, ledFirst, ledCount, ledOffset);
	neoGroups.push_back(newGroup);
	return neoGroups.size();
}

int addGroup(String args)
{
	//JsonObject &jsonArgs = parseArgs(args);
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	JsonObject &jsonArgs = jsonBuffer.parseObject(argsbuf);

	if (!jsonArgs.success())
		return -1; // Invalid JSon arguments
	String grpId = (jsonArgs.containsKey("id")) ? jsonArgs["id"] : "";
	//int ledFirst = (jsonArgs.containsKey("ledF")) ? jsonArgs["ledF"] : -1;
	//int ledCount = (jsonArgs.containsKey("ledC")) ? jsonArgs["ledC"] : -1;
	int ledFirst = jsonArgs.at("ledF");
	int ledCount = jsonArgs.at("ledC");
	int ledOffset = (jsonArgs.containsKey("ledO")) ? jsonArgs["ledO"] : 0;
	return addGroupInternal(grpId, ledFirst, ledCount, ledOffset);
}

int startGroup(String args)
{
	//JsonObject &jsonArgs = parseArgs(args);
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	JsonObject &jsonArgs = jsonBuffer.parseObject(argsbuf);

	if (!jsonArgs.success())
		if (!jsonArgs.success())
			return -1;
	int grpNr = (jsonArgs.containsKey("grp")) ? jsonArgs["grp"] : -1;
	if (grpNr < 0 || grpNr >= neoGroups.size())
	{
		return -((((2 * 1000) + grpNr) * 1000) + neoGroups.size()); // Invalid parameter
	}
	NeoGroup *neoGroup = neoGroups.at(grpNr);
	neoGroup->Start();
	return 0;
}

int stopGroup(String args)
{
	//JsonObject &jsonArgs = parseArgs(args);
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	JsonObject &jsonArgs = jsonBuffer.parseObject(argsbuf);

	if (!jsonArgs.success())
		if (!jsonArgs.success())
			return -1;
	int grpNr = (jsonArgs.containsKey("grp")) ? jsonArgs["grp"] : -1;
	if (grpNr < 0 || grpNr >= neoGroups.size())
	{
		return -((((2 * 1000) + grpNr) * 1000) + neoGroups.size()); // Invalid parameter
	}
	bool stopNow = (jsonArgs.containsKey("now")) ? jsonArgs["now"] : false;
	NeoGroup *neoGroup = neoGroups.at(grpNr);
	neoGroup->Stop(stopNow);
	return 0;
}

int setEffect(String args)
{
	//JsonObject &jsonArgs = parseArgs(args);
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	JsonObject &jsonArgs = jsonBuffer.parseObject(argsbuf);

	if (!jsonArgs.success())
		if (!jsonArgs.success())
			return -1;
	int grpNr = (jsonArgs.containsKey("grp")) ? jsonArgs["grp"] : -1;
	if (grpNr < 0 || grpNr >= neoGroups.size())
	{
		return -((((2 * 1000) + grpNr) * 1000) + neoGroups.size()); // Invalid parameter
	}
	NeoGroup *neoGroup = neoGroups.at(grpNr);
	neoGroup->Stop();
	String fxName = (jsonArgs.containsKey("fx")) ? jsonArgs["fx"] : "";
	pattern fxPattern = pattern::NOFX;
	if (fxName == "static")
		fxPattern = pattern::STATIC;
	if (fxName == "fade")
		fxPattern = pattern::FADE;
	if (fxName == "wave")
		fxPattern = pattern::WAVE;
	if (fxName == "rainbow")
		fxPattern = pattern::RAINBOW;
	if (fxName == "confetti")
		fxPattern = pattern::CONFETTI;
	if (fxName == "fire")
		fxPattern = pattern::FIRE;
	int fxLength = (jsonArgs.containsKey("len")) ? jsonArgs["len"] : 0;
	int fxGlitter = (jsonArgs.containsKey("glt")) ? jsonArgs["glt"] : 0;
	int fxFps = (jsonArgs.containsKey("fps")) ? jsonArgs["fps"] : 50;
	String parmDir = (jsonArgs.containsKey("dir")) ? jsonArgs["dir"] : "f";
	direction fxDir = direction::FORWARD;
	if (parmDir == "f")
		fxDir = direction::FORWARD;
	if (parmDir == "r")
		fxDir = direction::REVERSE;
	int parmMirror = (jsonArgs.containsKey("mirr")) ? jsonArgs["mirr"] : 0;
	mirror fxMirror = mirror::MIRROR0;
	if (parmMirror == 1)
		fxMirror = mirror::MIRROR1;
	if (parmMirror == 2)
		fxMirror = mirror::MIRROR2;
		uint16_t result = neoGroup->ConfigureEffect(fxPattern, fxLength, fxGlitter, fxFps, fxDir, fxMirror);
	//neoGroup->Start();
	return result;
}

int setColors(String args)
{
	//JsonObject &jsonArgs = parseArgs(args);
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	JsonObject &jsonArgs = jsonBuffer.parseObject(argsbuf);

	if (!jsonArgs.success())
		return -1;
	int grpNr = (jsonArgs.containsKey("grp")) ? jsonArgs["grp"] : -1;
	if (grpNr < 0 || grpNr >= neoGroups.size())
	{
		return -((((2 * 1000) + grpNr) * 1000) + neoGroups.size()); // Invalid parameter
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
			return grpNr; //-2;
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

void setup()
{
	// Register cloud methods
	// Methods for LED strip
	Particle.function("initStrip", initStrip);
	Particle.function("startStrip", startStrip);
	Particle.function("stopStrip", stopStrip);

	// Methods for groups
	Particle.function("addGroup", addGroup);
	Particle.function("startGroup", startGroup);
	Particle.function("stopGroup", stopGroup);

	// Methods for group effects
	Particle.function("setEffect", setEffect);
	Particle.function("setColors", setColors);

	// Register cloud variables
	Particle.variable("countLeds", pixelCount);
	Particle.variable("countGroups", groupCount);
	Particle.variable("isStarted", started);

	bool runXmasDemo = true;
	// TEST: Christmas Effects
	if (runXmasDemo)
	{
		initStripInternal(32, true, false);
		String startArgs = "";
		startStrip(startArgs);

		/*
		NeoGroup *neoGroup = neoGroups.at(0);
		neoGroup->ConfigureEffect(pattern::FADE, true, 25, direction::FORWARD);
		String palKey = "Christmas2";
		std::vector<CRGB> colors = ColorPalettes.find(palKey)->second;
		neoGroup->ConfigureColors(colors, true, true);
		neoGroup->Start();
		*/
		NeoGroup *neoGroup = neoGroups.at(0);
		/*
		uint16_t ConfigureEffect(
			pattern pattern,
			uint16_t length = 0,
			bool amountglitter = 0,
			uint8_t fps = 50,
			direction direction = FORWARD,
			mirror mirror = MIRROR0)
		*/
		neoGroup->ConfigureEffect(pattern::WAVE, 32, 128, 25, direction::FORWARD, mirror::MIRROR2);
		String palKey = "Christmas4";
		std::vector<CRGB> colors = ColorPalettes.find(palKey)->second;
		neoGroup->ConfigureColors(colors, true, true);
		neoGroup->Start();
	}
}

// Main loop
void loop()
{
	if (!started)
		return;

	int count = 0;
	bool isActiveMainGrp = false;
	for (int i = 0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = neoGroups.at(i);
		count += neoGroup->LedCount;
		neoGroup->Update();

		if (i == 0)
			isActiveMainGrp = neoGroup->Active;
		if (i > 0 && isActiveMainGrp)
			break; // Don't update other groups if main group (all LEDs) is active
	}
	FastLED.show();
	//pixelCount = count; //PIXEL_COUNT;
	groupCount = neoGroups.size() - 1; // Don't count main group (all LEDs)
}
