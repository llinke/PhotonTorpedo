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

#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256 // Default is 128
#include <BlynkSimpleEsp8266.h>

// Device 'XmasTree FBS43':
char blynkAuth[] = "4abfe0577ae745aca3d5d5d9f37911b7";
// Device 'XmasTree EFB':
// char blynkAuth[] = "7b8bd1934e8b4cf284dfa96471effc3f";
// Device 'XmasTree BNL':
// char blynkAuth[] = "a43bbc5bd9f04c8cb8b00dbe4add9da8";
// Device 'XmasTree SB':
// char blynkAuth[] = "fd6d6a43b6b4474aa708261839a736a9";
// Device 'XmasTree JL':
// char blynkAuth[] = "a652e6db9a0d435986feeb6866896e85";

//SYSTEM_MODE(AUTOMATIC);

const int ConfigureAPTimeout = 10;
const int AutoChangeIntervalFx = 60;
const int AutoChangeIntervalCol = 20;

//int ByteReceived;

// Helper macro
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// Static size
//struct CRGB leds[PIXEL_COUNT];
// Dynamic size:
struct CRGB *leds = NULL;
int pixelCount = PIXEL_COUNT;
bool ledsInitialized = false;
bool ledsStarted = false;
#ifdef INCLUDE_XMAS_DEMO
unsigned long lastUpdateFx = 0;
unsigned long updateIntervalFx = AutoChangeIntervalFx * 1000;
unsigned long lastUpdateCol = 0;
unsigned long updateIntervalCol = AutoChangeIntervalCol * 1000;
const int maxFxNr = 5;
const int startUpFxNr = 2;
int currFxNr = 0;
const int maxColNr = 5;
const int startUpColNr = 1;
int currColNr = 0;
int currFps = 25;
int currGlitter = 32; //48;
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
bool InitWifi(bool useWifiCfgTimeout = true, bool forceReconnect = false)
{
	Serial.println("WIFI ------------------------------------------------------");
	if (!forceReconnect && WiFi.status() == WL_CONNECTED)
	{
		Serial.println("WiFi: already connected...");
		return true; // Is already connected...
	}

	if (ledsInitialized)
	{
		FastLED.clear(true);
		fill_solid(leds, pixelCount, CRGB::Blue);
		FastLED.show();
	}
	delay(2500);
	//WiFiManager
	WiFiManager wifiManager;
	//wifiManager.resetSettings();
	//wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
	//fetches ssid and pass from eeprom and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "AutoConnectAP" and goes into a blocking loop awaiting configuration
	Serial.println("WiFi Manager trying to connect...");
	if (useWifiCfgTimeout)
	{
		Serial.print("You have ");
		Serial.print(ConfigureAPTimeout);
		Serial.println(" seconds for configuration if required.");
		wifiManager.setConfigPortalTimeout(ConfigureAPTimeout);
	}
	bool connected = wifiManager.autoConnect("XmasTreeAP");
	//or use this for auto generated name ESP + ChipID
	//wifiManager.autoConnect();
	//if you get here you have connected to the WiFi
	if (ledsInitialized)
	{
		fill_solid(leds, pixelCount, connected ? CRGB::Green : CRGB::Red);
		FastLED.show();
	}
	if (connected)
		Serial.println("Wifi is connected...yay!!!");
	else
		Serial.println("!!! WIFI NOT CONNECTED !!!");
	delay(5000);

	return connected;
}

void InitBlynk()
{
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.println("Blynk: authenticating");
	Blynk.config(blynkAuth);
	Blynk.connect();

	Serial.println("Blynk: assigning dropdown 'FX'");
	BlynkParamAllocated fxItems(128);
	fxItems.add("Zufällig");
	fxItems.add("Dynamisch");
	fxItems.add("Plasma");
	fxItems.add("Welle");
	fxItems.add("Konfetti");
	fxItems.add("Farbwechsel");
	Blynk.setProperty(V2, "labels", fxItems);

	Serial.println("Blynk: assigning dropdown 'Colors'");
	BlynkParamAllocated colItems(128);
	colItems.add("Zufällig");
	colItems.add("Farbschema 1");
	colItems.add("Farbschema 2");
	colItems.add("Farbschema 3");
	colItems.add("Farbschema 4");
	colItems.add("Farbschema 5");
	Blynk.setProperty(V4, "labels", colItems);

	SendStatusToBlynk();
}
void SendStatusToBlynk()
{
	Serial.println("Blynk: sending current status");
	Blynk.virtualWrite(V0, ledsStarted);
	Blynk.virtualWrite(V2, currFxNr + 1);
	Blynk.virtualWrite(V4, currColNr + 1);
	Blynk.virtualWrite(V5, currGlitter);
	Blynk.virtualWrite(V6, globalBrightness);
	Blynk.virtualWrite(V7, currFps);
}
#endif

int initStrip(int ledCount, bool doStart = false, bool playDemo = true)
{
	if (ledsInitialized)
	{
		return doStart ? startStrip() : pixelCount;
	}

	Serial.println("LEDStrip --------------------------------------------------");
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
	ledsInitialized = true;

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
	if (InitWifi())
		InitBlynk();
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

	Serial.println("Adding group 0 spanning all LEDs.");
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
	if (!ledsInitialized)
		return -1;

	ledsStarted = true;
	return pixelCount;
}

int stopStrip()
{
	ledsStarted = false;

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

	//int fxGlitter = amountglitter <= 0 ? neoGroup->GetGlitter() : amountglitter;
	int fxGlitter = amountglitter <= 0 ? currGlitter : amountglitter;
	//uint8_t fxFps = fps <= 0 ? neoGroup->GetFps() : fps;
	uint8_t fxFps = fps <= 0 ? currFps : fps;

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
	if (fxName == "dynwave")
		fxPattern = pattern::DYNAMICWAVE;
	if (fxName == "noise")
		fxPattern = pattern::NOISE;
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
	Particle.variable("isStarted", ledsStarted);
}
#endif

#ifdef INCLUDE_XMAS_DEMO
void SetXmasEffect(int grpNr, int fxNr, bool startFx = false)
{
	Serial.println("XMAS-FX ---------------------------------------------------");
	Serial.println("XmasFx: Configuring LED effect");

	if (fxNr == 0)
	{
		Serial.println("XmasFx: Choosing random effect.");
		SetXmasEffect(grpNr, random8(1, maxFxNr), startFx);
		return;
	}

	String fxPatternName = "";
	pattern fxPattern = pattern::STATIC;
	uint16_t fxLength = 255;
	int fxGlitter = currGlitter;
	uint8_t fxFps = currFps;
	mirror fxMirror = MIRROR0;

	switch (fxNr)
	{
	case 1: // Dynamic Wave
		fxPatternName = "Dynamic Wave";
		fxPattern = pattern::DYNAMICWAVE;
		fxMirror = mirror::MIRROR1; // mirror::MIRROR0;
		break;
	case 2: // Noise
		fxPatternName = "Noise";
		fxPattern = pattern::NOISE;
		fxMirror = mirror::MIRROR1; // mirror::MIRROR0;
		break;
	case 3: // Wave
		fxPatternName = "Wave";
		fxPattern = pattern::WAVE;
		fxLength = (pixelCount * 1.5); // 48;
		fxMirror = mirror::MIRROR2;
		break;
	case 4: // confetti
		fxPatternName = "Confetti";
		fxPattern = pattern::CONFETTI;
		fxGlitter = 0;
		break;
	case 5: // Fade
		fxPatternName = "Fade";
		fxPattern = pattern::FADE;
		break;
	default:
		fxPatternName = "Static";
		fxPattern = pattern::STATIC;
		fxMirror = mirror::MIRROR0;
		break;
	}
	Serial.print("XmasFx: Changing effect to '");
	Serial.print(fxPatternName);
	Serial.println("'");
	setEffect(
		grpNr,
		fxPattern,
		fxLength,
		fxGlitter,
		fxFps,
		direction::FORWARD,
		fxMirror);
	if (startFx)
		startGroup(grpNr);
}

void SetXmasColors(int grpNr, int colNr)
{
	Serial.println("XMAS-COL --------------------------------------------------");
	Serial.println("XmasCol: Configuring LED colors");

	if (colNr == 0)
	{
		Serial.println("XmasCol: Choosing random color palette.");
		SetXmasColors(grpNr, random8(1, maxColNr));
		return;
	}

	String palKey = "";
	if (colNr == 1)
	{
		palKey = "Christmas5";
	}
	if (colNr == 2)
	{
		palKey = "Christmas6";
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
		palKey = "Christmas7";
	}
	Serial.print("XmasCol: Changing color palette to '");
	Serial.print(palKey);
	Serial.println("'");
	if (ColorPalettes.find(palKey) != ColorPalettes.end())
	{
		std::vector<CRGB> colors = ColorPalettes.find(palKey)->second;
		setColors(grpNr, colors, true, true);
	}
}
#endif

void NextXmasEffect(int nextFx = -1)
{
	if (nextFx < 0)
	{
		currFxNr++;
	}
	else
	{
		currFxNr = nextFx;
	}
	if (currFxNr > maxFxNr)
		currFxNr = 0;
	Serial.print("CONTROL: Button 'FX' pressed, changing effect number to: ");
	Serial.println(currFxNr);
	SetXmasEffect(0, currFxNr, true);
}

void NextXmasColor(int nextCol = -1)
{
	if (nextCol < 0)
	{
		currColNr++;
	}
	else
	{
		currColNr = nextCol;
	}
	if (currColNr > maxColNr)
		currColNr = 0;
	Serial.print("CONTROL: Button 'Colors' pressed, changing color number to: ");
	Serial.println(currColNr);
	SetXmasColors(0, currColNr);
}

/*
BLYNK_READ(V0)
{
	Blynk.virtualWrite(V0, ledsStarted);
}
BLYNK_READ(V2)
{
	Blynk.virtualWrite(V2, currFxNr + 1);
}
BLYNK_READ(V4)
{
	Blynk.virtualWrite(V4, currColNr + 1);
}
BLYNK_READ(V5)
{
	Blynk.virtualWrite(V5, currGlitter);
}
BLYNK_READ(V6)
{
	Blynk.virtualWrite(V6, globalBrightness);
}
BLYNK_READ(V0)
{
	Blynk.virtualWrite(V7, currFps);
}
*/
BLYNK_WRITE(V0) // Button "Power"
{
	int pinValue = param.asInt();
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.print("Blynk-Button 'POWER' pressed: ");
	Serial.println(pinValue);
	if (pinValue == 1)
	{
		if (!ledsStarted)
		{
			startStrip();
			startGroup(0);
		}
	}
	else
	{
		if (ledsStarted)
		{
			stopGroup(0);
			stopStrip();
		}
	}
}

BLYNK_WRITE(V1) // Button "FX"
{
	int pinValue = param.asInt();
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.print("Blynk-Button 'FX' pressed: ");
	Serial.println(pinValue);
	if (pinValue == 1)
	{
		NextXmasEffect();
		Blynk.virtualWrite(V2, currFxNr + 1);
	}
}

BLYNK_WRITE(V2) // DropDown "FX"
{
	int pinValue = param.asInt() - 1;
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.print("Blynk-DropDown 'FX' selected: ");
	Serial.println(pinValue);
	NextXmasEffect(pinValue);
}

BLYNK_WRITE(V3) // Button "Color"
{
	int pinValue = param.asInt();
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.print("Blynk-Button 'Colors' pressed: ");
	Serial.println(pinValue);
	if (pinValue == 1)
	{
		NextXmasColor();
		Blynk.virtualWrite(V4, currColNr + 1);
	}
}

BLYNK_WRITE(V4) // DropDown "Colors"
{
	int pinValue = param.asInt() - 1;
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.print("Blynk-DropDown 'Colors' selected: ");
	Serial.println(pinValue);
	NextXmasColor(pinValue);
}

BLYNK_WRITE(V5) // Slider "Glitter"
{
	int pinValue = param.asInt();
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.print("Blynk-Slider 'Glitter' selected: ");
	Serial.println(pinValue);
	currGlitter = pinValue;
	NeoGroup *neoGroup = &(neoGroups.at(0));
	neoGroup->ChangeGlitter(currGlitter);
}

BLYNK_WRITE(V6) // Slider "Brightness"
{
	int pinValue = param.asInt();
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.print("Blynk-Slider 'Brightness' selected: ");
	Serial.println(pinValue);
	globalBrightness = pinValue;
	FastLED.setBrightness(pinValue);
	FastLED.setDither(0);
}

BLYNK_WRITE(V7) // Slider "Speed"
{
	int pinValue = param.asInt();
	Serial.println("BLYNK -----------------------------------------------------");
	Serial.print("Blynk-Slider 'Speed' selected: ");
	Serial.println(pinValue);
	currFps = pinValue;
	NeoGroup *neoGroup = &(neoGroups.at(0));
	neoGroup->ChangeFps(currFps);
}
BLYNK_APP_CONNECTED()
{
	SendStatusToBlynk();
	//Blynk.syncAll();
}

void setup()
{
#ifdef INCLUDE_PHOTON
	RegisterPhotonFunctions();
#endif

	Serial.begin(115200);

	pinMode(BUTTON_PIN_1, INPUT_PULLUP);
	pinMode(BUTTON_PIN_2, INPUT_PULLUP);

#if defined(INCLUDE_WIFI_MGR) && !defined(INCLUDE_XMAS_DEMO)
	if (InitWifi())
		InitBlynk();
#endif

#ifdef INCLUDE_XMAS_DEMO
	// TEST: Christmas Effects
	Serial.println("BOOT/SETUP ------------------------------------------------");
	Serial.println("Setup: Setting up Xmas Tree for Arduino");
	Serial.println("Setup: Initializing LED strip");
	initStrip(32, true, true);
	Serial.print("Setup: Amount of LEDs: ");
	Serial.println(pixelCount);

	Serial.println("Setup: Starting LED strip");
	startStrip();

	/*
	Serial.println("Setup: Configuring LED effect");
	pattern fxPattern = pattern::WAVE;
	int fxLength = 48;
	int fxGlitter = 48;
	int fxFps = 25;
	mirror fxMirror = mirror::MIRROR2;
	NeoGroup *neoGroup = &(neoGroups.at(0));
	neoGroup->ConfigureEffect(fxPattern, fxLength, fxGlitter, fxFps, direction::FORWARD, fxMirror);

	Serial.println("Setup: Configuring LED colors");
	String palKey = "Christmas5";
	if (ColorPalettes.find(palKey) != ColorPalettes.end())
	{
		std::vector<CRGB> colors = ColorPalettes.find(palKey)->second;
		neoGroup->ConfigureColors(colors, true, true);
	}

	Serial.println("Setup: Starting LED effect");
	neoGroup->Start();
*/

	SetXmasEffect(0, startUpFxNr);
	SetXmasColors(0, startUpColNr);
	startGroup(0);
#else
	Serial.println("Setup: Xmas Tree not active");
#endif
}

// Main loop
void loop()
{
	static bool button1Pressed = false;
	static bool button2Pressed = false;
	static bool bothButtonsPressed = false;
	//Serial.println("LOOP ------------------------------------------------------");
	bool btn1Pressed = !digitalRead(BUTTON_PIN_1);
	bool btn2Pressed = !digitalRead(BUTTON_PIN_2);
	if (bothButtonsPressed &&
		(!btn1Pressed | !btn2Pressed) /*&&
		WiFi.status() != WL_CONNECTED*/) // Both buttons pressed
	{
		Serial.println("Loop: both buttons pressed, entering WiFi-setup.");
		if (InitWifi(false, true))
			InitBlynk();
	}
	else
	{
		if (button1Pressed && !btn1Pressed) // Button was released
		{
			Serial.println("Loop: button 'FX' pressed and released.");
			NextXmasEffect();
		}
		if (button2Pressed && !btn2Pressed) // Button was released
		{
			Serial.println("Loop: button 'colors' pressed and releases.");
			NextXmasColor();
		}
	}
	button1Pressed = btn1Pressed;
	button2Pressed = btn2Pressed;
	bothButtonsPressed = btn1Pressed & btn2Pressed;

	if (!ledsStarted)
	{
		//Serial.println("Loop: LEDs not started, leaving loop.");
		return;
	}

#ifdef INCLUDE_XMAS_DEMO
	bool fxChanged = false;
	if ((millis() - lastUpdateFx) > updateIntervalFx)
	{
		lastUpdateFx = millis();
		if (currFxNr == 0)
		{
			Serial.println("Loop: time to randomly change Xmas FX.");
			SetXmasEffect(0, 0);
			fxChanged = true;
		}
	}
	if ((millis() - lastUpdateCol) > updateIntervalCol)
	{
		lastUpdateCol = millis();
		if (currColNr == 0)
		{
			Serial.println("Loop: time to randomly change Xmas Colors.");
			SetXmasColors(0, 0);
		}
	}
	if (fxChanged)
	{
		startGroup(0);
	}
#endif

	bool isActiveMainGrp = false;
	bool ledsUpdated = false;
	for (int i = 0; i < neoGroups.size(); i++)
	{
		NeoGroup *neoGroup = &(neoGroups.at(i));
		if (neoGroup->LedCount <= pixelCount)
		{
			//Serial.print("Loop: Updating group ");
			//Serial.println(i);
			ledsUpdated |= neoGroup->Update();
		}

		if (i == 0)
			isActiveMainGrp = neoGroup->Active;
		if (i > 0 && isActiveMainGrp)
			break; // Don't update other groups if main group (all LEDs) is active
	}
	if (ledsUpdated)
	{
		//Serial.println("Loop: Refreshing LEDs.");
		FastLED.show();
	}
	else
	{
#ifdef INCLUDE_WIFI_MGR
		static int lastBlynkUpdate = 0;
		if ((millis() - lastBlynkUpdate) > 50)
		{
			lastBlynkUpdate = millis();
			//Serial.println("Loop: Blynk.Run()");
			Blynk.run();
		}
#endif
	}
	//pixelCount = count; //PIXEL_COUNT;
	groupCount = neoGroups.size() - 1; // Don't count main group (all LEDs)
}
