/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "FastLedInclude.h"
#include <vector>
#include <map>

const CRGBPalette16 firePalettes[] = {
	// This first palette is the basic 'black body radiation' colors,
	// which run from black to red to bright yellow to white.
	HeatColors_p,
	// First, a gradient from black to red to yellow to white -- similar to HeatColors_p
	CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White),
	// Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
	CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White),
	// Third, here's a simpler, three-step gradient, from black to red to white
	CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::White),
	// Fourth, here's a warmer gradient, from black to yellow
	CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Yellow),
	// Fifth, here's a second warmer gradient, from black to blue
	CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Blue),
	// Sixth, here's a third warmer gradient, from darkred to yellow
	CRGBPalette16(CRGB::DarkRed, CRGB::Red, CRGB::Orange, CRGB::Yellow)};

const std::map<String, std::vector<CRGB>> ColorPalettes = {
	{"Pop", {CRGB(0xAAFF00), CRGB(0xFFAA00), CRGB(0xFF00AA), CRGB(0xAA00FF), CRGB(0x00AAFF)}},
	{"DarkRainbox", {CRGB(0x482344), CRGB(0x2B5166), CRGB(0x429867), CRGB(0xFAB243), CRGB(0xE02130)}},
	{"Pastel1", {CRGB(0x3FB8AF), CRGB(0x7FC7AF), CRGB(0xDAD8A7), CRGB(0xFF9E9D), CRGB(0xFF3D7F)}},
	{"Pastel2", {CRGB(0x93DFB8), CRGB(0xFFC8BA), CRGB(0xE3AAD6), CRGB(0xB5D8EB), CRGB(0xFFBDD8)}},
	{"Pinkish1", {CRGB(0xCD0869), CRGB(0xF24685), CRGB(0xEC509D), CRGB(0xF487AF), CRGB(0xF8BED3)}},
	{"Pinkish2", {CRGB(0xEB2A73), CRGB(0xFF619A), CRGB(0xFF94BB), CRGB(0xFFC7DB), CRGB(0xFFE8F0)}},
	{"Girlish1", {CRGB(0x009999), CRGB(0x8E0039), CRGB(0xDB0058), CRGB(0xFF7400), CRGB(0xFFC700)}},
	{"NightAndDay1", {CRGB(0x029DAF), CRGB(0xE5D599), CRGB(0xFFC219), CRGB(0xF07C19), CRGB(0xE32551)}},
	{"NightAndDay2", {CRGB(0x75386D), CRGB(0x418592), CRGB(0x2DC0A7), CRGB(0x7EE368), CRGB(0xCEFD47)}},
	{"Unicorn1", {CRGB(0xEEF6D0), CRGB(0xBF7DB7), CRGB(0x40F9E8), CRGB(0x49C1BB), CRGB(0x85038C)}},
	{"Unicorn2", {CRGB(0xA697E8), CRGB(0xD2ABEA), CRGB(0xFFBFEB), CRGB(0xFFDFDC), CRGB(0xFFFFCC)}},
	{"Nature1", {CRGB(0xF6C918), CRGB(0xADAF34), CRGB(0x76B86E), CRGB(0x09A590), CRGB(0x3F2542)}},
	{"Nature2", {CRGB(0xBF2A23), CRGB(0xA6AD3C), CRGB(0xF0CE4E), CRGB(0xCF872E), CRGB(0x8A211D)}},
	{"Nature3", {CRGB(0xFCDEBB), CRGB(0xFAC589), CRGB(0xD94709), CRGB(0xA80022), CRGB(0x8C001C)}},
	{"Ocean1", {CRGB(0x69D2E7), CRGB(0xA7DBD8), CRGB(0xE0E4CC), CRGB(0xF38630), CRGB(0xFA6900)}},
	{"Ocean2", {CRGB(0xCFF09E), CRGB(0xA8DBA8), CRGB(0x79BD9A), CRGB(0x3B8686), CRGB(0x0B486B)}},
	{"Ocean3", {CRGB(0xF2E8C4), CRGB(0x98D9B6), CRGB(0x3EC9A7), CRGB(0x2B879E), CRGB(0x616668)}},
	{"Castle1", {CRGB(0x996C97), CRGB(0x8D8EA3), CRGB(0x99A8AD), CRGB(0xEFBE9E), CRGB(0xF6DFA9)}},
	{"Castle2", {CRGB(0xF98367), CRGB(0x9BB1C6), CRGB(0xE4E2E3), CRGB(0xCDBFB4), CRGB(0x506678)}},
	{"Fresh", {CRGB(0xF35C9F), CRGB(0xF3F3F3), CRGB(0x22ADC2), CRGB(0x736F66)}},
	{"CozyFire1", {CRGB(0x330A04), CRGB(0x6C1305), CRGB(0xB32900), CRGB(0xDE8531), CRGB(0xEFAC41)}},
	{"CozyFire2", {CRGB(0x992C61), CRGB(0xFF244A), CRGB(0xFF631B), CRGB(0xFFBF1E), CRGB(0xFFF3CF)}},
	{"GrecianFire", {CRGB(0x604860), CRGB(0x784860), CRGB(0xA86060), CRGB(0xC07860), CRGB(0xF0A848)}}};

enum pattern
{
	NONE,
	STATIC,
	RAINBOW,
	CONFETTI,
	FADE,
	FIRE
};
enum direction
{
	FORWARD,
	OUTWARD,
	REVERSE,
	INWARD
};

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoGroup
{
#define FIRE_COOLING 55
#define FIRE_SPARKING 120
	uint8_t firePaletteNr = 4;

	int fadeLength = 64;
	void (NeoGroup::*effectFunc)();
	CRGB *LedFirst;
	unsigned long lastUpdate;

  public:
	int GroupID;
	int LedCount;

	CRGBPalette16 Colors;
	bool AddGlitter;
	uint16_t TotalSteps;
	uint16_t Index;
	direction Direction;

	unsigned long Interval;
	bool Active;

	// Constructor - calls base-class constructor to initialize strip
	NeoGroup(int groupID, int ledFirst, int ledLast)
	{
		GroupID = groupID;
		Active = false;
		Index = 0;
		LedFirst = &leds[ledFirst];
		LedCount = (ledLast - ledFirst) + 1;
		TotalSteps = LedCount;
	}

	uint16_t ConfigureEffect(pattern pattern, std::vector<CRGB> colors, uint8_t fps, direction dir = FORWARD, bool addglitter = false)
	{
		Interval = (1000 / fps);
		Index = 0;
		Direction = dir;
		AddGlitter = false;
		if (colors.size() != 0)
		{
			Colors = GenerateRGBPalette(colors);
		}
		else
		{
			Colors = NULL;
		}
		TotalSteps = 256;

		if (pattern == RAINBOW)
		{
			effectFunc = &NeoGroup::Rainbow;
			AddGlitter = addglitter;
		}
		if (pattern == CONFETTI)
		{
			effectFunc = &NeoGroup::Confetti;
		}
		if (pattern == FADE)
		{
			effectFunc = &NeoGroup::Fade;
			AddGlitter = addglitter;
		}
		if (pattern == FIRE)
		{
			fill_solid(LedFirst, LedCount, 0x000000);
			Colors = firePalettes[firePaletteNr];
			effectFunc = &NeoGroup::Fire;
		}
		if (pattern == STATIC)
		{
			effectFunc = &NeoGroup::Static;
			AddGlitter = addglitter;
			TotalSteps = 1;
		}
		return TotalSteps;
	}

	void Start()
	{
		Active = true;
	}

	void Stop()
	{
		Active = false;
	}

	// Update the pattern
	void Update()
	{
		if (!Active)
			return;

		if ((millis() - lastUpdate) > Interval)
		{
			lastUpdate = millis();
			if (effectFunc != NULL)
			{
				(this->*effectFunc)();
				if (AddGlitter)
				{
					Glitter();
				}
			}
		}
	}

	void Increment(bool invert = false)
	{
		if (Direction == FORWARD)
		{
			Index++;
			if (Index >= TotalSteps)
			{
				if (invert)
				{
					Reverse();
				}
				else
				{
					Index = 0;
				}
			}
		}
		else // Direction == REVERSE
		{
			--Index;
			if (Index < 0)
			{
				if (invert)
				{
					Reverse();
				}
				else
				{
					Index = TotalSteps - 1;
				}
			}
		}
	}

	// Reverse pattern direction
	void Reverse()
	{
		if (Direction == FORWARD)
		{
			Direction = REVERSE;
			Index = TotalSteps - 1;
		}
		else
		{
			Direction = FORWARD;
			Index = 0;
		}
	}

	// Update the Static Pattern
	void Static()
	{
		CRGB newColor = ColorFromPalette(Colors, 0);
		fill_solid(LedFirst, LedCount, newColor);
		//Stop();
	}

	void Fade()
	{
		CRGB newColor = ColorFromPalette(Colors, Index);
		fill_solid(LedFirst, LedCount, newColor);
		Increment();
	}

	void Rainbow()
	{
		uint8_t rainBowLength = 64;
		uint8_t deltaHue = rainBowLength > LedCount ? rainBowLength / LedCount : 1;
		fill_rainbow(LedFirst, LedCount, Index, deltaHue);
		Increment();
	}

	void Glitter(uint8_t chanceOfGlitter = 80)
	{
		if (random8() < chanceOfGlitter)
		{
			LedFirst[random16(LedCount)] += CRGB::White;
		}
	}

	void Confetti()
	{
		fadeToBlackBy(LedFirst, LedCount, 10);
		int pos = random16(LedCount);
		LedFirst[pos] += CHSV(Index + random8(64), 200, 255);
		Increment();
	}

	void Fire()
	{
		// Array of temperature readings at each simulation cell
		byte heat[LedCount];

		// Step 1.  Cool down every cell a little
		for (int i = 0; i < LedCount; i++)
		{
			heat[i] = qsub8(heat[i], random8(0, ((FIRE_COOLING * 10) / LedCount) + 2));
		}

		// Step 2.  Heat from each cell drifts 'up' and diffuses a little
		for (int k = LedCount - 1; k >= 2; k--)
		{
			heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
		}

		// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
		if (random8() < FIRE_SPARKING)
		{
			int y = random8(7);
			heat[y] = qadd8(heat[y], random8(160, 255));
		}

		// Step 4.  Map from heat cells to LED colors
		for (int j = 0; j < LedCount; j++)
		{
			// Scale the heat value from 0-255 down to 0-240
			// for best results with color palettes.
			byte colorindex = scale8(heat[j], 240);
			CRGB color = ColorFromPalette(Colors, colorindex);
			int dir = (j % 2);
			int pixelnumber = dir == 0
								  ? (LedCount / 2) + (j / 2) + 1
								  : (LedCount / 2) - (j / 2);
			LedFirst[pixelnumber] = color;
		}
		Increment();
	}

	CRGBPalette16 GenerateRGBPalette(std::vector<CRGB> colors)
	{
		CRGB nc[16];
		int colCount = colors.size();
		/*
		for(int c = 0; c < 16; c++)
		{
			int modVal = (c * colCount) % 16;
			int newColIdx1 = (c * colCount) >> 4; // same as DIV 16
			int newColIdx2 = newColIdx1;
			if (newColIdx2 >= colCount) newColIdx2 = 0;
			CRGB col1 = colors[newColIdx1];
			CRGB col2 = colors[newColIdx2];
			nc[c] = blend(col1, col2, modVal << 4); // same as MUL 16
		}
		*/
		for (int c = 0; c < colCount; c++)
		{
			uint16_t trgtIdx1 = ((c * 16) / colCount);
			uint16_t trgtIdx2 = (((c + 1) * 16) / colCount) - 1;
			if (trgtIdx2 > 15)
				trgtIdx2 = 15;
			if (trgtIdx2 < 0)
				trgtIdx2 = 0;
			CRGB col1 = colors[c];
			CRGB col2 = (c + 1 < colCount) ? colors[c + 1] : colors[0];
			fill_gradient_RGB(&(nc[0]), trgtIdx1, col1, trgtIdx2, col2);
		}
		return CRGBPalette16(
			nc[0], nc[1], nc[2], nc[3], nc[4], nc[5], nc[6], nc[7],
			nc[8], nc[9], nc[10], nc[11], nc[12], nc[13], nc[14], nc[15]);
	}
};
