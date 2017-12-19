#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define DATA_PIN 3
#define PIXEL_PIN 3
#define PIXEL_COUNT 32
#define PIXEL_TYPE NEOPIXEL

// Static size:
//extern struct CRGB leds[];
// Dynamic size:
extern struct CRGB *leds;
