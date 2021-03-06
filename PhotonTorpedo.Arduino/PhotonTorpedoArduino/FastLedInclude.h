#define FASTLED_ESP8266_RAW_PIN_ORDER
//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER

#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ESP8266_DMA
#include <FastLED.h>
FASTLED_USING_NAMESPACE

//#define PIXEL_PIN D2
#define PIXEL_PIN D1

#define PIXEL_COUNT 32
#define PIXEL_OFFSET 4
/*
#define PIXEL_COUNT 21
#define PIXEL_OFFSET 3
*/

#define PIXEL_TYPE NEOPIXEL
#define BUTTON_PIN_1 D5
#define BUTTON_PIN_2 D6

// Static size:
//extern struct CRGB leds[];
// Dynamic size:
extern struct CRGB *leds;
