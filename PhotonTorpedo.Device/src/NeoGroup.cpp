/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "FastLedInclude.h"
#include <vector>
#include <map>

enum pattern
{
	NONE,
	STATIC,
	FADE,
	RAINBOW,
	CONFETTI,
	FIRE
};
enum direction
{
	FORWARD,
	REVERSE,
	//	OUTWARD,
	//	INWARD
};

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoGroup
{
#define FIRE_COOLING 55
#define FIRE_SPARKING 120
#define FADEOUT_DURATION 1000
#define FADEOUT_STEPS 20

	CRGB *LedFirst;

	unsigned long updateInterval;
	unsigned long lastUpdate;
	uint16_t fxStep;
	uint16_t totalFxSteps;
	direction fxDirection;
	int fxFadeOut = 0;
	bool fxAddGlitter;

	std::vector<CRGB> currentColors = {};
	CRGBPalette16 colorPalette;
	uint8_t firePaletteNr = 6;

	void (NeoGroup::*effectFunc)();

  public:
	String GroupID;
	int LedCount;
	bool Active;

	// Constructor - calls base-class constructor to initialize strip
	NeoGroup(String groupID, int ledFirst, int ledCount)
	{
		GroupID = groupID;
		Active = false;
		fxStep = 0;
		LedFirst = &leds[ledFirst];
		LedCount = ledCount;
		totalFxSteps = LedCount;
	}

	uint16_t ConfigureEffect(
		pattern pattern,
		bool addglitter = false,
		uint8_t fps = 50,
		direction direction = FORWARD)
	{
		Stop();

		updateInterval = (1000 / fps);
		fxStep = 0;
		fxDirection = direction;
		fxAddGlitter = addglitter;
		totalFxSteps = 256;

		if (pattern == STATIC)
		{
			effectFunc = &NeoGroup::FxStatic;
			totalFxSteps = 1;
		}
		if (pattern == FADE)
		{
			effectFunc = &NeoGroup::FxFade;
		}
		if (pattern == RAINBOW)
		{
			effectFunc = &NeoGroup::FxRainbow;
		}
		if (pattern == CONFETTI)
		{
			effectFunc = &NeoGroup::FxConfetti;
			fxAddGlitter = false;
		}
		if (pattern == FIRE)
		{
			fill_solid(LedFirst, LedCount, 0x000000);
			effectFunc = &NeoGroup::FxFire;
			fxAddGlitter = false;
		}
		return totalFxSteps;
	}

	uint16_t ConfigureColors(
		std::vector<CRGB> colors,
		bool clearFirst = true,
		bool generatePalette = true)
	{
		if (clearFirst)
		{
			currentColors.clear();
		}

		for (CRGB color : colors)
		{
			currentColors.push_back(color);
		}

		if (generatePalette)
		{
			if (colors.size() != 0)
			{
				colorPalette = GenerateRGBPalette(currentColors);
			}
			else
			{
				colorPalette = NULL;
			}
		}
		return currentColors.size();
	}

	void Start()
	{
		Active = true;
		lastUpdate = 0;
	}

	void Stop(bool stopNow = false)
	{
		Active = false;
		fxFadeOut = (stopNow) ? 0 : FADEOUT_STEPS;
	}

	// Update the pattern
	void Update()
	{
		if (fxFadeOut > 0)
		{
			if ((millis() - lastUpdate) > (FADEOUT_DURATION / FADEOUT_STEPS))
			{
				lastUpdate = millis();
				fadeToBlackBy(LedFirst, LedCount, FADEOUT_STEPS);
				fxFadeOut--;
				if (fxFadeOut == 0)
				{
					fill_solid(LedFirst, LedCount, 0x000000);
				}
			}
			return;
		}

		if (!Active)
			return;

		if ((millis() - lastUpdate) > updateInterval)
		{
			lastUpdate = millis();
			if (effectFunc != NULL)
			{
				(this->*effectFunc)();

				if (fxAddGlitter)
				{
					FxGlitter();
				}
			}
		}
	}

	void NextFxStep(bool invert = false)
	{
		if (fxDirection == FORWARD)
		{
			fxStep++;
			if (fxStep >= totalFxSteps)
			{
				if (invert)
				{
					ReverseFxDirection();
				}
				else
				{
					fxStep = 0;
				}
			}
		}
		else // fxDirection == REVERSE
		{
			--fxStep;
			if (fxStep < 0)
			{
				if (invert)
				{
					ReverseFxDirection();
				}
				else
				{
					fxStep = totalFxSteps - 1;
				}
			}
		}
	}

	// Reverse pattern direction
	void ReverseFxDirection()
	{
		if (fxDirection == FORWARD)
		{
			fxDirection = REVERSE;
			fxStep = totalFxSteps - 1;
		}
		else
		{
			fxDirection = FORWARD;
			fxStep = 0;
		}
	}

	// Update the Static Pattern
	void FxStatic()
	{
		CRGB newColor = ColorFromPalette(colorPalette, 0);
		fill_solid(LedFirst, LedCount, newColor);
		//Stop();
	}

	void FxFade()
	{
		CRGB newColor = ColorFromPalette(colorPalette, fxStep);
		fill_solid(LedFirst, LedCount, newColor);
		NextFxStep();
	}

	void FxRainbow()
	{
		uint8_t rainBowLength = 64;
		uint8_t deltaHue = rainBowLength > LedCount ? rainBowLength / LedCount : 1;
		fill_rainbow(LedFirst, LedCount, fxStep, deltaHue);
		NextFxStep();
	}

	void FxGlitter(uint8_t chanceOfGlitter = 80)
	{
		if (random8() < chanceOfGlitter)
		{
			LedFirst[random16(LedCount)] += CRGB::White;
		}
	}

	void FxConfetti()
	{
		fadeToBlackBy(LedFirst, LedCount, 10);
		int pos = random16(LedCount);
		LedFirst[pos] += CHSV(fxStep + random8(64), 200, 255);
		NextFxStep();
	}

	void FxFire()
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
			CRGB color = ColorFromPalette(colorPalette, colorindex);
			int dir = (j % 2);
			int pixelnumber = dir == 0
								  ? (LedCount / 2) + (j / 2) + 1
								  : (LedCount / 2) - (j / 2);
			LedFirst[pixelnumber] = color;
		}
		NextFxStep();
	}

	static CRGBPalette16 GenerateRGBPalette(std::vector<CRGB> colors)
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
