/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "neopixel.h"
#include <vector>

enum pattern
{
	NONE,
	STATIC,
	RAINBOW,
	WIPE,
	FADE
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
	Adafruit_NeoPixel *strip;
	int fadeLength = 64;

  public:
		int GroupID;
		uint16_t LedFirst;
		uint16_t LedLast;
		uint16_t LedCount;
		pattern ActivePattern;
		direction Direction;
		std::vector<uint32_t> Colors;
		uint16_t TotalSteps;
		uint16_t Index;
		bool Active;
		unsigned long Interval;
		unsigned long lastUpdate;

	// Constructor - calls base-class constructor to initialize strip
	NeoGroup(Adafruit_NeoPixel *mainstrip, int groupID, uint16_t ledFirst, uint16_t ledLast)
	{
		GroupID = groupID;
		Active = false;
		Index = 0;
		LedFirst = ledFirst;
		LedLast = ledLast;
		LedCount = (ledLast - ledFirst) + 1;
		TotalSteps = LedCount;

		strip = mainstrip;
		//strip->clear();
	}

	uint16_t ConfigureEffect(pattern pattern, std::vector<uint32_t> colors, uint8_t interval, direction dir = FORWARD)
	{
		if (pattern == RAINBOW)
		{
			ActivePattern = RAINBOW;
			Interval = interval;
			Colors.clear();
			Index = 0;
			TotalSteps = 256;
			Direction = dir;
		}
		if (pattern == WIPE)
		{
			ActivePattern = WIPE;
			Interval = interval;
			Colors.clear();
			for(uint32_t color : colors)
			{
				Colors.push_back(color);
			}
			Index = 0;
			TotalSteps = LedCount * Colors.size();
			Direction = dir;
		}
		if (pattern == FADE)
		{
			ActivePattern = FADE;
			Interval = interval;
			Colors.clear();
			for(uint32_t color : colors)
			{
				Colors.push_back(color);
			}
			Index = 0;
			TotalSteps = fadeLength * Colors.size();
			Direction = dir;
		}
		if (pattern == STATIC)
		{
			ActivePattern = STATIC;
			Interval = interval;
			Colors.clear();
			for(uint32_t color : colors)
			{
				Colors.push_back(color);
			}
			Index = 0;
			TotalSteps = 1;
			Direction = dir;
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
	void Update(bool autoShow = true)
	{
		if (!Active) return;

		if ((millis() - lastUpdate) > Interval)
		{
			lastUpdate = millis();

			switch (ActivePattern)
			{
				case STATIC:
					StaticUpdate(autoShow);
					break;
				case RAINBOW:
					RainbowUpdate(autoShow);
					break;
				case WIPE:
					WipeUpdate(autoShow);
					break;
				case FADE:
					FadeUpdate(autoShow);
					break;
				default:
					break;
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

	// Update the Rainbow Cycle Pattern
	void RainbowUpdate(bool autoShow = true)
	{
		for (int i = LedFirst; i <= LedLast; i++)
		{
			int i2 = i - LedFirst;
			strip->setPixelColor(i, GetRainbowColor(((i2 * 256 / LedCount) + (255 - Index)) & 255));
		}
		if (autoShow)
		{
			strip->show();
		}
		Increment();
	}

	// Update the Color Wipe Pattern
	void WipeUpdate(bool autoShow = true)
	{
		int colNr = (int)abs(Index / LedCount);
		uint16_t indexNew = Index - (colNr * LedCount);
		strip->setPixelColor(LedFirst + indexNew, Colors.at(colNr));
		if (autoShow)
		{
			strip->show();
		}
		Increment();
	}

	// Update the Fade Pattern
	void FadeUpdate(bool autoShow = true)
	{
		int colNr1 = (int)abs(Index / fadeLength);
		int colNr2 = Direction == FORWARD ? colNr1 + 1 : colNr1 - 1;
		if (colNr2 >= Colors.size())
		{
			colNr2 = 0;
		}
		if (colNr2 < 0)
		{
			colNr2 = (Colors.size() - 1);
		}

		uint32_t color1 = Colors.at(colNr1);
		uint32_t color2 = Colors.at(colNr2);
		uint16_t indexNew = Index - (colNr1 * fadeLength);

		// Calculate linear interpolation between color1 and color2
		// Optimise order of operations to minimize truncation error
		uint8_t red = ((Red(color1) * (fadeLength - indexNew)) + (Red(color2) * indexNew)) / fadeLength;
		uint8_t green = ((Green(color1) * (fadeLength - indexNew)) + (Green(color2) * indexNew)) / fadeLength;
		uint8_t blue = ((Blue(color1) * (fadeLength - indexNew)) + (Blue(color2) * indexNew)) / fadeLength;
		uint32_t newColor = strip->Color(red, green, blue);

		for (int i = LedFirst; i <= LedLast; i++)
		{
			strip->setPixelColor(i, newColor);
		}
		if (autoShow)
		{
			strip->show();
		}
		Increment();
	}

	// Update the Static Pattern
	void StaticUpdate(bool autoShow = true)
	{
		uint32_t newColor = Colors.at(0);
		for (int i = LedFirst; i <= LedLast; i++)
		{
			strip->setPixelColor(i, newColor);
		}
		if (autoShow)
		{
			strip->show();
		}
		Stop();
	}

	// Returns the Red component of a 32-bit color
	uint8_t Red(uint32_t color)
	{
		return (color >> 16) & 0xFF;
	}

	// Returns the Green component of a 32-bit color
	uint8_t Green(uint32_t color)
	{
		return (color >> 8) & 0xFF;
	}

	// Returns the Blue component of a 32-bit color
	uint8_t Blue(uint32_t color)
	{
		return color & 0xFF;
	}

	// Input a value 0 to 255 to get a color value.
	// The colours are a transition r - g - b - back to r.
	uint32_t GetRainbowColor(byte WheelPos)
	{
		WheelPos = 255 - WheelPos;
		if (WheelPos < 85)
		{
			return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
		}
		else if (WheelPos < 170)
		{
			WheelPos -= 85;
			return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
		}
		else
		{
			WheelPos -= 170;
			return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
		}
	}
};
