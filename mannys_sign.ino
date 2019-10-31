#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    5
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

//#define NUM_STRIPS 5
//#define NUM_LEDS_PER_STRIP 300
//#define NUM_LEDS__STRIP1 162
#define NUM_LEDS__STRIP1 300
CRGB leds1[NUM_LEDS__STRIP1];

#define BRIGHTNESS          200
#define FRAMES_PER_SECOND  20

struct DigitRegion {
  uint8_t start;
  uint8_t end;
};

struct DigitDisplay {
  struct DigitRegion top;
  struct DigitRegion left;
  struct DigitRegion right;
  struct DigitRegion bottom;
  struct DigitRegion center;
};

struct DigitDisplay digit1 = {
  { 0, 8 },
  { 40, 58 },
  { 10, 28 },
  { 28, 39 },
  { 68, 78 }
};

void setup() {
  delay(3000); // 3 second delay for recovery

  Serial.begin(9600);

  FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(leds1, NUM_LEDS__STRIP1).setCorrection(TypicalLEDStrip);
//  FastLED.addLeds<LED_TYPE, 0, COLOR_ORDER>(leds[1], NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
//  FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(leds[2], NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
//  FastLED.addLeds<LED_TYPE, 16, COLOR_ORDER>(leds[3], NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
//  FastLED.addLeds<LED_TYPE, 17, COLOR_ORDER>(leds[4], NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void fill_region(struct DigitRegion &region, const CRGB &color) {
  fill_solid(leds1 + region.start, region.end - region.start, color);
}

void loop() {
  static uint8_t region = 0;
  fill_solid(leds1, NUM_LEDS__STRIP1, CRGB::Black);

  if (region == 0) {
    fill_region(digit1.top, CRGB::Red);  
  } else if (region == 1) {
    fill_region(digit1.left, CRGB::Green);
  } else if (region == 2) {
    fill_region(digit1.right, CRGB::Blue);
  } else if (region == 3) {
    fill_region(digit1.bottom, CRGB::Yellow);
  } else if (region == 4) {
    fill_region(digit1.center, CRGB::Pink);
  }

//  fill_solid(leds1 + r.start, r.end - r.start, CRGB::Red);

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_SECONDS(3) {
    region++;
    if (region > 4) {
      region = 0;
    }
  }
}
