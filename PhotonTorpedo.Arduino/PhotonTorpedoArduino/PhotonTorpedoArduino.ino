/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/
#define INCLUDE_XMAS_DEMO
#define INCLUDE_WIFI_MGR
//#define INCLUDE_PHOTON

#include "FastLedInclude.h"
#include "ColorPalettes.h"
#include <Arduino.h>
//#include <ArduinoSTL.h>
#include <vector>
//#include <map>
#include "ArduinoJson.h"
//#include "SparkJson.h"
#include "NeoGroup.cpp"

#ifdef INCLUDE_WIFI_MGR
#include <ESP8266WiFi.h>	  //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>		  //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>	  //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#endif

//SYSTEM_MODE(AUTOMATIC);

const int ConfigureAPTimeout = 30;
const int AutoChangeInterval = 15;

//int ByteReceived;

// Helper macro
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// Static size
//struct CRGB leds[PIXEL_COUNT];
// Dynamic size:
struct CRGB *leds = NULL;
int pixelCount = PIXEL_COUNT;
bool initialized = false;
bool started = false;
#ifdef INCLUDE_XMAS_DEMO
unsigned long lastUpdate = 0;
unsigned long updateInterval = AutoChangeInterval * 1000;
int currFxNr = 0;
int currColNr = 0;
#endif

//std::vector<NeoGroup *> neoGroups;
std::vector<NeoGroup> neoGroups;
int groupCount = 0;

int globalBrightness = 64;

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

#ifdef INCLUDE_WIFI_MGR
void InitWifi()
{
	//WiFiManager
	WiFiManager wifiManager;
	//wifiManager.resetSettings();
	//wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
	//fetches ssid and pass from eeprom and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "AutoConnectAP" and goes into a blocking loop awaiting configuration
	Serial.print("WiFi Manager trying to connect (you have ");
	Serial.print(ConfigureAPTimeout);
	Serial.println(" seconds for configuration)...");
	wifiManager.setConfigPortalTimeout(ConfigureAPTimeout);
	wifiManager.autoConnect("XmasTreeAP");
	//or use this for auto generated name ESP + ChipID
	//wifiManager.autoConnect();
	//if you get here you have connected to the WiFi
	Serial.println("Connected to WiFi...yay!!!");
}
#endif

int initStrip(int ledCount, bool doStart = false, bool playDemo = true)
{
	if (initialized)
	{
		return doStart ? startStrip() : pixelCount;
	}

	Serial.println("Allocating memory for LED strip data.");
	leds = (struct CRGB *)malloc(ledCount * sizeof(struct CRGB));
	Serial.println("Assigning LEDs to FastLED.");
	FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN>(leds, ledCount);
	//FastLED.setMaxPowerInVoltsAndMilliamps(5,3000);
	FastLED.setBrightness(globalBrightness);
	FastLED.setDither(0);
	FastLED.clear(true);
	FastLED.show();

	pixelCount = ledCount;
	initialized = true;

	if (playDemo)
	{
		Serial.println("Playing little demo effect.");
		for (int dot = 0; dot < ledCount; dot++)
		{
			leds[dot] = CHSV(random8(), 255, 255);
			FastLED.show();
			delay(10);
		}
		delay(500);
	}
#if defined(INCLUDE_WIFI_MGR) && defined(INCLUDE_XMAS_DEMO)
	InitWifi();
#endif
	if (playDemo)
	{
		Serial.println("Fading away demo effect.");
		for (int fade = 0; fade < 20; fade++)
		{
			fadeToBlackBy(leds, ledCount, 20);
			FastLED.show();
			delay(50);
		}
		Serial.println("Clearing LEDs.");
		FastLED.clear(true);
		FastLED.show();
	}

	Serial.println("Adding group 0 for all LEDs.");
	int offset = 4;
	neoGroups.clear();
	// Group 0: all LEDs
	addGroup("All LEDs", 0, pixelCount, offset);

	// Right Wing
	//addGroup("Right Wing", 0, 9);
	// Small Room
	//addGroup("Small Room", 10, 16);
	// Gate
	//addGroup("Gate", 17, 26);
	// Main Hall
	//addGroup("Main Hall", 32, 47);

	return doStart ? startStrip() : pixelCount;
}

int startStrip()
{
	if (!initialized)
		return -1;

	started = true;
	return pixelCount;
}

int stopStrip()
{
	started = false;

	for (int i = 0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = &(neoGroups.at(i));
		neoGroup->Stop(true);
	}

	FastLED.clear(true);
	FastLED.show();

	return 0;
}

int addGroup(String grpId, int ledFirst, int ledCount, int ledOffset)
{
	if ((ledFirst >= pixelCount) ||
		(ledCount <= 0) ||
		(ledFirst + ledCount) > pixelCount)
		return -((((3 * 1000) + ledFirst) * 1000) + ledCount); // Invalid parameter

	// V1: new NeoGroup
	//NeoGroup *newGroup = new NeoGroup(grpId, ledFirst, ledCount, ledOffset);
	//neoGroups.push_back(newGroup);
	// V2: NeoGroup w/o new
	NeoGroup newGroup = NeoGroup(grpId, ledFirst, ledCount, ledOffset);
	neoGroups.push_back(newGroup);
	return neoGroups.size();
}

int startGroup(int grpNr)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Start();
	return 0;
}

int stopGroup(int grpNr, bool stopNow = false)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Stop(stopNow);
	return 0;
}

int setEffect(
	int grpNr,
	pattern pattern,
	uint16_t length = 0,
	int amountglitter = -1,
	uint8_t fps = 0,
	direction direction = FORWARD,
	mirror mirror = MIRROR0)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	neoGroup->Stop();

	int fxGlitter = amountglitter <= 0 ? neoGroup->GetGlitter() : amountglitter;
	uint8_t fxFps = fps <= 0 ? neoGroup->GetFps() : fps;

	uint16_t result = neoGroup->ConfigureEffect(pattern, length, fxGlitter, fxFps, direction, mirror);
	//neoGroup->Start();
	return result;
}

int setColors(
	int grpNr,
	std::vector<CRGB> colors,
	bool clearFirst = true,
	bool generatePalette = true)
{
	NeoGroup *neoGroup = &(neoGroups.at(grpNr));
	//neoGroup->Stop();
	uint16_t result = neoGroup->ConfigureColors(colors, clearFirst, generatePalette);
	//neoGroup->Start();
	return result;
}

#ifdef INCLUDE_PHOTON
int initStripParticle(String args)
{
	//JsonObject &jsonArgs = parseArgs(args);
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	JsonObject &jsonArgs = jsonBuffer.parseObject(argsbuf);

	if (!jsonArgs.success())
		return -1; // Invalid JSon arguments
	int ledCount = (jsonArgs.containsKey("ledC")) ? jsonArgs["ledC"] : 0;
	return initStrip(ledCount);
}

int startStripParticle(String args)
{
	return startStrip();
}

int stopStripParticle(String args)
{
	return stopStrip();
}

int addGroupParticle(String args)
{
	//JsonObject &jsonArgs = parseArgs(args);
	StaticJsonBuffer<200> jsonBuffer;
	char argsbuf[args.length() + 1];
	args.toCharArray(argsbuf, args.length() + 1);
	JsonObject &jsonArgs = jsonBuffer.parseObject(argsbuf);

	if (!jsonArgs.success())
		return -1; // Invalid JSon arguments
	String grpId = (jsonArgs.containsKey("id")) ? jsonArgs["id"].as<String>() : "";
	//int ledFirst = (jsonArgs.containsKey("ledF")) ? jsonArgs["ledF"] : -1;
	//int ledCount = (jsonArgs.containsKey("ledC")) ? jsonArgs["ledC"] : -1;
	int ledFirst = jsonArgs["ledF"];
	int ledCount = jsonArgs["ledC"];
	int ledOffset = (jsonArgs.containsKey("ledO")) ? jsonArgs["ledO"] : 0;
	return addGroup(grpId, ledFirst, ledCount, ledOffset);
}

int startGroupParticle(String args)
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
	return startGroup(grpNr);
}

int stopGroupParticle(String args)
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
	bool stopNow = (jsonArgs.containsKey("now")) ? jsonArgs["now"] : false;
	return stopGroup(grpNr, stopNow);
}
int setEffectParticle(String args)
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
	String fxName = (jsonArgs.containsKey("fx")) ? jsonArgs["fx"].as<String>() : "";
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
	String parmDir = (jsonArgs.containsKey("dir")) ? jsonArgs["dir"].as<String>() : "f";
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
	return setEffect(grpNr, fxPattern, fxLength, fxGlitter, fxFps, fxDir, fxMirror);
}
int setColorsParticle(String args)
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

	String palKey = "";
	bool clearFirst = true;
	bool genPalette = true;
	if (jsonArgs.containsKey("pal"))
	{
		//String palKey = jsonArgs["pal"].as<String>();
		String palKey = (jsonArgs.containsKey("pal")) ? jsonArgs["pal"].as<String>() : "";
		if (ColorPalettes.find(palKey) == ColorPalettes.end())
		{
			return grpNr; //-2;
		}
		std::vector<CRGB> colors = ColorPalettes.find(palKey)->second;
		return setColors(grpNr, colors, clearFirst, genPalette);
	}
	else
	{
		bool clearFirst = (jsonArgs.containsKey("clr")) ? jsonArgs["clr"] : true;
		bool genPalette = (jsonArgs.containsKey("gen")) ? jsonArgs["gen"] : true;
		std::vector<CRGB> colors = {};
		//std::vector<CRGB> colors;
		JsonArray &colorsArray = jsonArgs["cols"];
		if (colorsArray != JsonArray::invalid())
		{
			for (int c = 0; c < colorsArray.size(); c++)
			{
				uint32_t colVal = colorsArray[c];
				colors.push_back(CRGB(colVal));
			}
		}
		return setColors(grpNr, colors, clearFirst, genPalette);
	}
}
void RegisterPhotonFunctions()
{
	// Register cloud methods
	// Methods for LED strip
	Particle.function("initStrip", initStripParticle);
	Particle.function("startStrip", startStripParticle);
	Particle.function("stopStrip", stopStripParticle);

	// Methods for groups
	Particle.function("addGroup", addGroupParticle);
	Particle.function("startGroup", startGroupParticle);
	Particle.function("stopGroup", stopGroupParticle);

	// Methods for group effects
	Particle.function("setEffect", setEffectParticle);
	Particle.function("setColors", setColorsParticle);

	// Register cloud variables
	Particle.variable("countLeds", pixelCount);
	Particle.variable("countGroups", groupCount);
	Particle.variable("isStarted", started);
}
#endif

#ifdef INCLUDE_XMAS_DEMO
void SetXmasEffect(int grpNr, int fxNr)
{
	Serial.println("Configuring LED effect");

	String fxPatternName = "";
	pattern fxPattern = pattern::STATIC;
	uint16_t fxLength = 255;
	int fxGlitter = 48;
	uint8_t fxFps = 25;
	mirror fxMirror = MIRROR0;

	if (fxNr == 1)
	{
		fxPatternName = "Wave";
		fxPattern = pattern::WAVE;
		fxLength = 48;
		fxMirror = mirror::MIRROR2;
	}
	if (fxNr == 2)
	{
		fxPatternName = "Confetti";
		fxPattern = pattern::CONFETTI;
	}
	if (fxNr == 3)
	{
		fxPatternName = "Fade";
		fxPattern = pattern::FADE;
	}
	setEffect(
		grpNr,
		fxPattern,
		fxLength,
		fxGlitter,
		fxFps,
		direction::FORWARD,
		fxMirror);
}

void SetXmasColors(int grpNr, int colNr)
{
	Serial.println("Configuring LED colors");
	String palKey = "";
	if (colNr == 1)
	{
		palKey = "Christmas5";
	}
	if (colNr == 2)
	{
		palKey = "Ocean1";
	}
	if (colNr == 3)
	{
		palKey = "Pop";
	}
	if (colNr == 4)
	{
		palKey = "NightAndDay2";
	}
	if (colNr == 5)
	{
		palKey = "CozyFire1";
	}
	Serial.print("Changing color palette to '");
	Serial.print(palKey);
	Serial.println("'");
	if (ColorPalettes.find(palKey) != ColorPalettes.end())
	{
		std::vector<CRGB> colors = ColorPalettes.find(palKey)->second;
		setColors(grpNr, colors, true, true);
	}
}
#endif

void setup()
{
#ifdef INCLUDE_PHOTON
	RegisterPhotonFunctions();
#endif

	Serial.begin(115200);

#if defined(INCLUDE_WIFI_MGR) && !defined(INCLUDE_XMAS_DEMO)
	InitWifi();
#endif

#ifdef INCLUDE_XMAS_DEMO
	// TEST: Christmas Effects
	Serial.println("Setting up Xmas Tree for Arduino");
	Serial.println("Initializing LED strip");
	initStrip(32, true, true);
	Serial.print("LEDs: ");
	Serial.print(pixelCount);
	Serial.println("");

	Serial.println("Starting LED strip");
	startStrip();

	/*
	Serial.println("Configuring LED effect");
	pattern fxPattern = pattern::WAVE;
	int fxLength = 48;
	int fxGlitter = 48;
	int fxFps = 25;
	mirror fxMirror = mirror::MIRROR2;
	NeoGroup *neoGroup = &(neoGroups.at(0));
	neoGroup->ConfigureEffect(fxPattern, fxLength, fxGlitter, fxFps, direction::FORWARD, fxMirror);

	Serial.println("Configuring LED colors");
	String palKey = "Christmas5";
	if (ColorPalettes.find(palKey) != ColorPalettes.end())
	{
		std::vector<CRGB> colors = ColorPalettes.find(palKey)->second;
		neoGroup->ConfigureColors(colors, true, true);
	}

	Serial.println("Starting LED effect");
	neoGroup->Start();
*/

	SetXmasEffect(0, random8(1, 3));
	SetXmasColors(0, random8(1, 5));
	startGroup(0);
#else
	Serial.println("Xmas Tree not active");
#endif
}

// Main loop
void loop()
{
	if (!started)
		return;

#ifdef INCLUDE_XMAS_DEMO
	if ((millis() - lastUpdate) > updateInterval)
	{
		lastUpdate = millis();
		if (currFxNr == 0)
		{
			SetXmasEffect(0, random8(1, 3));
			if (currColNr == 0)
			{
				SetXmasColors(0, random8(1, 5));
			}
			startGroup(0);
		}
	}
#endif

	bool isActiveMainGrp = false;
	bool ledsUpdated = false;
	for (int i = 0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = &(neoGroups.at(i));
		if (neoGroup->LedCount <= pixelCount)
		{
			ledsUpdated |= neoGroup->Update();
		}

		if (i == 0)
			isActiveMainGrp = neoGroup->Active;
		if (i > 0 && isActiveMainGrp)
			break; // Don't update other groups if main group (all LEDs) is active
	}
	if (ledsUpdated)
	{
		//Serial.print("Refreshing LEDs.");
		FastLED.show();
	}
	//pixelCount = count; //PIXEL_COUNT;
	groupCount = neoGroups.size() - 1; // Don't count main group (all LEDs)
}
