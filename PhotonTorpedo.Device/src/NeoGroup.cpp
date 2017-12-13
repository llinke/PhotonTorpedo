/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "FastLedInclude.h"
#include <vector>

const CRGBPalette16 firePalettes[] = {
	// This first palette is the basic 'black body radiation' colors,
	// which run from black to red to bright yellow to white.
	HeatColors_p,
	// First, a gradient from black to red to yellow to white -- similar to HeatColors_p
	CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White),
	// Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
	CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White),
	// Third, here's a simpler, three-step gradient, from black to red to white
	CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White),
	// Fourth, here's a warmer gradient, from black to yellow
	CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Yellow),
	// Fifth, here's a second warmer gradient, from black to blue
	CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Blue)
};

enum pattern
{
	NONE,
	STATIC,
	RAINBOW,
	RAINBOW2,
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
	#define FIRE_COOLING  55
	#define FIRE_SPARKING 120
	uint8_t firePaletteNr = 4;

	int fadeLength = 64;
	void (NeoGroup::*effectFunc)();
	CRGB *LedFirst;
	unsigned long lastUpdate;

  public:
		int GroupID;
		int LedCount;
		direction Direction;
		CRGBPalette16 Colors;
		uint16_t TotalSteps;
		uint16_t Index;
		bool Active;
		unsigned long Interval;

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

	uint16_t ConfigureEffect(pattern pattern, std::vector<CRGB> colors, uint8_t fps, direction dir = FORWARD)
	{
		Interval = (1000 / fps);
		Index = 0;
		Direction = dir;
		if (colors.size() != 0)
		{
			Colors = GeneratePalette(colors);
		}
		else
		{
			Colors = NULL;
		}
		TotalSteps = 256;

		if (pattern == RAINBOW)
		{
			effectFunc = &NeoGroup::Rainbow;
		}
		if (pattern == RAINBOW2)
		{
			effectFunc = &NeoGroup::RainbowWithGlitter;
		}
		if (pattern == CONFETTI)
		{
			effectFunc = &NeoGroup::Confetti;
		}
		if (pattern == FADE)
		{
			effectFunc = &NeoGroup::Fade;
		}
		if (pattern == FIRE)
		{
			Colors = firePalettes[firePaletteNr];
			effectFunc = &NeoGroup::Fire;
		}
		if (pattern == STATIC)
		{
			effectFunc = &NeoGroup::Static;
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
		if (!Active) return;

		if ((millis() - lastUpdate) > Interval)
		{
			lastUpdate = millis();
			if (effectFunc != NULL)
			{
				(this->*effectFunc)();
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

	void Rainbow()
	{
		fill_rainbow(LedFirst, LedCount, Index, 7);
		Increment();
	}

	void RainbowWithGlitter()
	{
	  Rainbow();

		uint8_t chanceOfGlitter = 80;
	  if(random8() < chanceOfGlitter)
		{
	    LedFirst[random16(LedCount)] += CRGB::White;
	  }
	}

	void Confetti()
	{
	  fadeToBlackBy(LedFirst,LedCount,10);
	  int pos = random16(LedCount);
	  LedFirst[pos] += CHSV(Index + random8(64), 200, 255);
		Increment();
	}

	void Fade()
	{
		CRGB newColor = ColorFromPalette(Colors, Index);
		fill_solid(LedFirst, LedCount, newColor);
		Increment();
	}

	void Fire()
	{
		// Array of temperature readings at each simulation cell
	  byte heat[LedCount];

	  // Step 1.  Cool down every cell a little
    for(int i = 0; i < LedCount; i++) {
      heat[i] = qsub8(heat[i],  random8(0, ((FIRE_COOLING * 10) / LedCount) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for(int k= LedCount - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if(random8() < FIRE_SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8(heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for(int j = 0; j < LedCount; j++) {
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

	// Update the Static Pattern
	void Static()
	{
		CRGB newColor = ColorFromPalette(Colors, 0);
		fill_solid(LedFirst, LedCount, newColor);
		Stop();
	}

	CRGBPalette16 GeneratePalette(std::vector<CRGB> colors)
	{
		CRGB nc[16];
		int colCount = colors.size();
		for(int c = 0; c < 16; c++)
		{
			int modVal = (c * (colCount + 1)) % 16;
			if (modVal == 0)
			{
				nc[c] = colors[(c * (colCount + 1)) >> 4];
			}
			else
			{
				int newColIdx1 = (c * (colCount + 1)) >> 4;
				int newColIdx2 = modVal <= 8 ? newColIdx1 : newColIdx1 + 1;
				if (newColIdx2 >= colCount) newColIdx2 = 0;
				CRGB col1 = colors[newColIdx1];
				CRGB col2 = colors[newColIdx2];
				nc[c] = blend(col1, col2, modVal / 16);
			}
		}
		return CRGBPalette16(
			nc[0],nc[1],nc[2],nc[3],nc[4],nc[5],nc[6],nc[7],
			nc[8],nc[9],nc[10],nc[11],nc[12],nc[13],nc[14],nc[15]);
	}
};
