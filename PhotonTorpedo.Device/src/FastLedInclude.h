//#define FASTLED_ESP8266_RAW_PIN_ORDER
//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define PIXEL_PIN D2
#define PIXEL_COUNT 32
//#define PIXEL_COUNT 48
#define PIXEL_TYPE NEOPIXEL

// Static size:
//extern struct CRGB leds[];
// Dynamic size:
extern struct CRGB *leds;
