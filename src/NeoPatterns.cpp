/*
* Project PhotonTorpedo
* Description:
* Author:
* Date:
*/

#include "neopixel.h"

// Pattern types supported:
enum pattern
{
	NONE,
	RAINBOW_CYCLE,
	COLOR_WIPE,
	FADE
};
// Patern directions supported:
enum direction
{
	FORWARD,
	REVERSE
};

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns
{
	Adafruit_NeoPixel *strip;

  public:
	// Member Variables:
	pattern ActivePattern;	// which pattern is running
	direction Direction;	  // direction to run the pattern
	unsigned long Interval;   // milliseconds between updates
	unsigned long lastUpdate; // last update of position
	uint32_t Color1, Color2;  // What colors are in use
	uint16_t TotalSteps;	  // total number of steps in the pattern
	uint16_t Index;			  // current step within the pattern

	// Constructor - calls base-class constructor to initialize strip
	NeoPatterns(Adafruit_NeoPixel *mainstrip)
	{
		strip = mainstrip;
		//strip->clear();
	}

	// Update the pattern
	void Update()
	{
		if ((millis() - lastUpdate) > Interval) // time to update
		{
			lastUpdate = millis();
			switch (ActivePattern)
			{
			case RAINBOW_CYCLE:
				RainbowCycleUpdate();
				break;
			case COLOR_WIPE:
				ColorWipeUpdate();
				break;
			case FADE:
				FadeUpdate();
				break;
			default:
				break;
			}
		}
	}

	// Increment the Index and reset at the end
	void Increment(bool invert = false)
	{
		if (Direction == FORWARD)
		{
			Index++;
			if (Index >= TotalSteps)
			{
				if (invert)
				{
					Index = TotalSteps - 1;
					Direction = REVERSE;
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
			if (Index <= 0)
			{
				if (invert)
				{
					Index = 0;
					Direction = FORWARD;
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

	// Initialize for a RainbowCycle
	void RainbowCycle(uint8_t interval, direction dir = FORWARD)
	{
		ActivePattern = RAINBOW_CYCLE;
		Interval = interval;
		TotalSteps = 255;
		Index = 0;
		Direction = dir;
	}

	// Update the Rainbow Cycle Pattern
	void RainbowCycleUpdate()
	{
		for (int i = 0; i < strip->numPixels(); i++)
		{
			strip->setPixelColor(i, Wheel(((i * 256 / strip->numPixels()) + Index) & 255));
		}
		strip->show();
		Increment();
	}

	// Initialize for a ColorWipe
	void ColorWipe(uint32_t color, uint32_t color2, uint8_t interval, direction dir = FORWARD)
	{
		ActivePattern = COLOR_WIPE;
		Interval = interval;
		TotalSteps = strip->numPixels();
		Color1 = color;
		Color2 = color2;
		Index = 0;
		Direction = dir;
	}

	// Update the Color Wipe Pattern
	void ColorWipeUpdate()
	{
		if (Direction == FORWARD)
		{
			strip->setPixelColor(Index, Color1);
		}
		else
		{
			strip->setPixelColor(Index, Color2);
		}
		strip->show();
		Increment(true);
	}

	// Initialize for a Fade
	void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
	{
		ActivePattern = FADE;
		Interval = interval;
		TotalSteps = steps;
		Color1 = color1;
		Color2 = color2;
		Index = 0;
		Direction = dir;
	}

	// Update the Fade Pattern
	void FadeUpdate()
	{
		// Calculate linear interpolation between Color1 and Color2
		// Optimise order of operations to minimize truncation error
		uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
		uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
		uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

		uint32_t newColor = strip->Color(red, green, blue);
		for (int i = 0; i < strip->numPixels(); i++)
		{
			strip->setPixelColor(i, newColor);
		}
		strip->show();
		Increment(true);
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
	uint32_t Wheel(byte WheelPos)
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
