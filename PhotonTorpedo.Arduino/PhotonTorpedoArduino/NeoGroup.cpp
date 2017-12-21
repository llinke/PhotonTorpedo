/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "FastLedInclude.h"
#include <Arduino.h>
//#include <ArduinoSTL.h>
#include <vector>
//#include <map>

enum pattern
{
	NOFX,
	STATIC,
	FADE,
	WAVE,
	COLWAVE,
	RAINBOW,
	CONFETTI,
	FIRE
};
enum direction
{
	FORWARD,
	REVERSE,
	//	OUTWARD,
	//	IN0WARD
};
enum mirror
{
	MIRROR0,
	MIRROR1,
	MIRROR2
};

class NeoGroup
{
#define FIRE_COOLING 55
#define FIRE_SPARKING 120
#define FADEOUT_DURATION 1000
#define FADEOUT_STEPS 20

	CRGB *LedFirst;
	int LedOffset = 0;

	uint8_t fxFps;
	unsigned long lastUpdate;
	uint16_t fxStep;
	uint16_t totalFxSteps;
	direction fxDirection;
	int fxFadeOut = 0;
	int fxAmountGlitter;
	uint16_t fxLength;
	mirror fxMirror = MIRROR0;

	//std::vector<CRGB> currentColors = {};
	std::vector<CRGB> currentColors;
	CRGBPalette16 colorPalette;
	uint8_t firePaletteNr = 6;

	void (NeoGroup::*effectFunc)();

  public:
	String GroupID;
	int LedCount;
	bool Active;

	NeoGroup(String groupID, int ledFirst, int ledCount, int ledOffset = 0)
	{
		GroupID = groupID;
		Active = false;
		fxStep = 0;
		LedFirst = &leds[ledFirst];
		LedCount = ledCount;
		LedOffset = ledOffset;
		totalFxSteps = LedCount;
	}

	uint16_t ConfigureEffect(
		pattern pattern,
		uint16_t length = 0,
		int amountglitter = 0,
		uint8_t fps = 50,
		direction direction = FORWARD,
		mirror mirror = MIRROR0)
	{
		Serial.println("Stopping effect execution.");
		Stop();

		Serial.println("Configuring effect parameters.");
		ChangeFps(fps);
		fxStep = 0;
		fxDirection = direction;
		ChangeGlitter(amountglitter);
		fxMirror = mirror;
		fxLength = 256;
		totalFxSteps = 256;

		if (pattern == STATIC)
		{
			Serial.print("Setting FX 'Static' for group '");
			Serial.print(GroupID);
			Serial.println("'.");
			effectFunc = &NeoGroup::FxStatic;
			totalFxSteps = 1;
		}
		if (pattern == FADE)
		{
			Serial.print("Setting FX 'Fade' for group '");
			Serial.print(GroupID);
			Serial.println("'.");
			effectFunc = &NeoGroup::FxFade;
		}
		if (pattern == WAVE)
		{
			Serial.print("Setting FX 'Wave' for group '");
			Serial.print(GroupID);
			Serial.println("'.");
			effectFunc = &NeoGroup::FxWave;
			fxLength = (length == 0 ? (LedCount * 2) : length);
		}
		if (pattern == COLWAVE)
		{
			Serial.print("Setting FX 'ColorWave' for group '");
			Serial.print(GroupID);
			Serial.println("'.");
			effectFunc = &NeoGroup::FxColorWaves;
			fxLength = (length == 0 ? (LedCount * 2) : length);
		}
		if (pattern == RAINBOW)
		{
			Serial.print("Setting FX 'Rainbow' for group '");
			Serial.print(GroupID);
			Serial.println("'.");
			effectFunc = &NeoGroup::FxRainbow;
			fxLength = (length == 0 ? (LedCount * 2) : length);
		}
		if (pattern == CONFETTI)
		{
			Serial.print("Setting FX 'Confetti' for group '");
			Serial.print(GroupID);
			Serial.println("'.");
			effectFunc = &NeoGroup::FxConfetti;
			fxAmountGlitter = 0;
		}
		if (pattern == FIRE)
		{
			Serial.print("Setting FX 'Fire' for group '");
			Serial.print(GroupID);
			Serial.println("'.");
			fill_solid(LedFirst, LedCount, 0x000000);
			effectFunc = &NeoGroup::FxFire;
			fxAmountGlitter = 0;
		}
		return totalFxSteps;
	}

	uint16_t ConfigureColors(
		std::vector<CRGB> colors,
		bool clearFirst = true,
		bool generatePalette = true)
	{
		Serial.print("Configuring colors for group '");
		Serial.print(GroupID);
		Serial.println("'.");
		if (clearFirst)
		{
			Serial.println("Clearing old list of colors.");
			currentColors.clear();
		}

		Serial.println("Adding CRGB colors to internal list.");
		for (CRGB color : colors)
		{
			currentColors.push_back(color);
		}

		if (generatePalette)
		{
			if (colors.size() != 0)
			{
				Serial.println("Generating color palette from colors.");
				colorPalette = GenerateRGBPalette(currentColors);
			}
			else
			{
				Serial.println("No colors, using empty list.");
				//colorPalette = NULL;
				colorPalette = CRGBPalette16();
			}
		}
		return currentColors.size();
	}

	void ChangeFps(uint8_t fps)
	{
		fxFps = fps;
	}

	uint8_t GetFps()
	{
		return fxFps;
	}

	void ChangeGlitter(int amountglitter)
	{
		fxAmountGlitter = amountglitter;
	}

	int GetGlitter()
	{
		return fxAmountGlitter;
	}

	void Start()
	{
		Serial.print("Starting group '");
		Serial.print(GroupID);
		Serial.println("'.");
		Active = true;
		lastUpdate = 0;
	}

	void Stop(bool stopNow = false)
	{
		Serial.print("Stopping group '");
		Serial.print(GroupID);
		Serial.println("'.");
		Active = false;
		fxFadeOut = (stopNow) ? 0 : FADEOUT_STEPS * 2;
		if (stopNow)
		{
			fill_solid(LedFirst, LedCount, 0x000000);
		}
	}

	bool Update()
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
				return true; // LEDs updated
			}
			return false; // LEDs not updated
		}

		if (!Active)
		{
			return false; // LEDs not updated
		}

		int updateInterval = (1000 / fxFps);
		if ((millis() - lastUpdate) > updateInterval)
		{
			lastUpdate = millis();
			if (effectFunc != NULL)
			{
				(this->*effectFunc)();

				if (fxAmountGlitter > 0)
				{
					FxGlitter(fxAmountGlitter);
				}
			}
			return true; // LEDs updated
		}

		return false; // LEDs not updated
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

	void SetPixel(int pos, CRGB newcolor, mirror mirror = MIRROR0, bool blend = false)
	{
		if (mirror == MIRROR1) // set even/odd as mirror values
		{
			int newPos = pos / 2;
			if ((pos % 2) == 0)
			{
				if (blend)
					nblend(LedFirst[CapLedPosition(newPos + LedOffset)], newcolor, 128);
				else
					LedFirst[CapLedPosition(newPos + LedOffset)] = newcolor;
			}
			else
			{
				int mirrorPos = LedCount - newPos - 1;
				if (blend)
					nblend(LedFirst[CapLedPosition(mirrorPos + LedOffset)], newcolor, 128);
				else
					LedFirst[CapLedPosition(mirrorPos + LedOffset)] = newcolor;
			}
			return;
		}

		if (mirror == MIRROR2) // mirror each second value
		{
			int newPos = pos / 2;
			if (blend)
				nblend(LedFirst[CapLedPosition(newPos + LedOffset)], newcolor, 128);
			else
				LedFirst[CapLedPosition(newPos + LedOffset)] = newcolor;
			int mirrorPos = LedCount - newPos - 1;
			if (blend)
				nblend(LedFirst[CapLedPosition(mirrorPos + LedOffset)], newcolor, 128);
			else
				LedFirst[CapLedPosition(mirrorPos + LedOffset)] = newcolor;
			return;
		}

		// else
		if (blend)
			nblend(LedFirst[CapLedPosition(pos + LedOffset)], newcolor, 128);
		else
			LedFirst[CapLedPosition(pos + LedOffset)] = newcolor;
	}

	int CapLedPosition(int pos)
	{
		int newPos = pos;
		while (newPos >= LedCount)
			newPos -= LedCount;
		while (newPos < 0)
			newPos += LedCount;
		return newPos;
	}

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

	void FxWave()
	{
		uint8_t waveLength = fxLength;
		uint8_t deltaWave = waveLength >= LedCount ? waveLength / LedCount : 1;
		//fill_rainbow(LedFirst, LedCount, fxStep, deltaHue);

		CRGB rgb;
		uint8_t wavePos = fxStep;
		for (int i = 0; i < LedCount; i++)
		{
			rgb = ColorFromPalette(colorPalette, wavePos);
			SetPixel((LedCount - i - 1), rgb, fxMirror);
			wavePos += deltaWave;
		}

		NextFxStep();
	}

	void FxRainbow()
	{
		uint8_t rainBowLength = fxLength;
		uint8_t deltaHue = rainBowLength >= LedCount ? rainBowLength / LedCount : 1;
		//fill_rainbow(LedFirst, LedCount, fxStep, deltaHue);

		CHSV hsv;
		hsv.hue = fxStep;
		hsv.sat = 240;
		hsv.val = 255;
		for (int i = 0; i < LedCount; i++)
		{
			//LedFirst[i] = hsv;
			SetPixel((LedCount - i - 1), hsv, fxMirror);
			hsv.hue += deltaHue;
		}

		NextFxStep();
	}

	void FxGlitter(uint8_t chanceOfGlitter = 80)
	{
		if (random8() < chanceOfGlitter)
		{
			LedFirst[random16(LedCount)] += CRGB::White;
			//SetPixel(random16(LedCount), CRGB::White, MIRROR0, true);
		}
	}

	void FxConfetti()
	{
		fadeToBlackBy(LedFirst, LedCount, 10);
		int pos = random16(LedCount);
		//LedFirst[pos] += CHSV(fxStep + random8(64), 200, 255);
		LedFirst[pos] += ColorFromPalette(colorPalette, fxStep + random8(64));
		//SetPixel(pos, ColorFromPalette(colorPalette, fxStep + random8(64)), MIRROR0, true);
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
			int y = random8(LedCount / 8);
			heat[y] = qadd8(heat[y], random8(160, 255));
		}

		// Step 4.  Map from heat cells to LED colors
		for (int j = 0; j < LedCount; j++)
		{
			// Scale the heat value from 0-255 down to 0-240
			// for best results with color palettes.
			byte colorindex = scale8(heat[j], 240);
			CRGB color = ColorFromPalette(colorPalette, colorindex);
			SetPixel(j, color, fxMirror);
		}
		NextFxStep();
	}

	void FxColorWaves()
	{
		static uint16_t sPseudotime = 0;
		static uint16_t sLastMillis = 0;
		static uint16_t sHue16 = 0;

		uint8_t sat8 = beatsin88(87, 220, 250);
		uint8_t brightdepth = beatsin88(341, 96, 224);
		uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
		uint8_t msmultiplier = beatsin88(147, 23, 60);

		uint16_t hue16 = sHue16; //gHue * 256;
		uint16_t hueinc16 = beatsin88(113, 300, 1500);

		uint16_t ms = millis();
		uint16_t deltams = ms - sLastMillis;
		sLastMillis = ms;
		sPseudotime += deltams * msmultiplier;
		sHue16 += deltams * beatsin88(400, 5, 9);
		uint16_t brightnesstheta16 = sPseudotime;

		for (uint16_t i = 0; i < LedCount; i++)
		{
			hue16 += hueinc16;
			uint8_t hue8 = hue16 / 256;
			uint16_t h16_128 = hue16 >> 7;
			if (h16_128 & 0x100)
			{
				hue8 = 255 - (h16_128 >> 1);
			}
			else
			{
				hue8 = h16_128 >> 1;
			}

			brightnesstheta16 += brightnessthetainc16;
			uint16_t b16 = sin16(brightnesstheta16) + 32768;

			uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
			uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
			bri8 += (255 - brightdepth);

			uint8_t index = hue8;
			//index = triwave8( index);
			index = scale8(index, 240);

			CRGB newcolor = ColorFromPalette(colorPalette, index, bri8);
			uint16_t pixelnumber = i;
			pixelnumber = (LedCount - 1) - pixelnumber;
			//nblend(ledarray[pixelnumber], newcolor, 128);
			SetPixel(pixelnumber, newcolor, fxMirror, true);
		}
	}

	static CRGBPalette16 GenerateRGBPalette(std::vector<CRGB> colors)
	{
		CRGB nc[16];
		int colCount = colors.size();
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
