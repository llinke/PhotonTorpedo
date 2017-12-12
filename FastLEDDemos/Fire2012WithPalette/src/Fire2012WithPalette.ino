#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define LED_PIN     2
//#define COLOR_ORDER GRB
#define CHIPSET     NEOPIXEL
#define NUM_LEDS    30

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];

CRGBPalette16 gPal;
CRGBPalette16 gPals[] = {
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
uint8_t gPalNr = 0;

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  gPal = gPals[4];
}

void loop()
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random8());

  // Fourth, the most sophisticated: this one sets up a new palette every
  // time through the loop, based on a hue that changes every time.
  // The palette is a gradient from black, to a dark color based on the hue,
  // to a light color based on the hue, to white.
  //
  //   static uint8_t hue = 0;
  //   hue++;
  //   CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
  //   CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
  //   gPal = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);


  Fire2012WithPalette(); // run simulation frame, using palette colors

  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  //EVERY_N_SECONDS( 15 ) { nextPalette(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPalette()
{
  // add one to the current pattern number, and wrap around at the end
  gPalNr = (gPalNr + 1) % ARRAY_SIZE( gPals);
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
////
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      //CRGB color = ColorFromPalette( gPals[gPalNr], colorindex);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int dir = (j % 2);
      int pixelnumber = dir == 0
        ? (NUM_LEDS / 2) + (j / 2) + 1
        : (NUM_LEDS / 2) - (j / 2);
      leds[pixelnumber] = color;
    }
}
