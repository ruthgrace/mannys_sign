#include <FastLED.h>

FASTLED_USING_NAMESPACE
#define FASTLED_SHOW_CORE 0

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

#define NUM_LEDS__STRIP1 300
CRGB ledsHours[NUM_LEDS__STRIP1];

#define BRIGHTNESS          200
#define FRAMES_PER_SECOND  20

struct DigitRegion {
  uint8_t start;
  uint8_t end;
};

struct DigitDisplay {
  struct DigitRegion top;
  struct DigitRegion leftTop;
  struct DigitRegion leftBottom;
  struct DigitRegion rightTop;
  struct DigitRegion rightBottom;
  struct DigitRegion bottom;
  struct DigitRegion center;
};

struct DigitDisplay digit1 = {
  { 0, 8 },
  { 48, 58 }, // left top
  { 40, 49 }, // left bottom
  { 10, 19 }, // right top
  { 18, 29 }, // right bottom
  { 28, 39 },
  { 68, 78 }
};

struct DigitDisplay digit2 = {
  { 91, 105 },
  { 106, 116 }, // left top
  { 115, 124 }, // left bottom
  { 151, 160 }, // right top
  { 139, 152 }, // right bottom
  { 125, 138 },
  { 80, 89 }
};

struct DigitBoard {
  struct DigitDisplay digit1;
  struct DigitDisplay digit2;
  CRGB *leds;
};

const struct DigitBoard hours = {
  digit1,
  digit2,
  ledsHours
};

void setup() {
  delay(3000); // 3 second delay for recovery

  Serial.begin(9600);

  FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(ledsHours, NUM_LEDS__STRIP1).setCorrection(TypicalLEDStrip);
  //  FastLED.addLeds<LED_TYPE, 0, COLOR_ORDER>(leds[1], NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //  FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(leds[2], NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //  FastLED.addLeds<LED_TYPE, 16, COLOR_ORDER>(leds[3], NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  //  FastLED.addLeds<LED_TYPE, 17, COLOR_ORDER>(leds[4], NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

//void drawA(struct DigitBoard &board) {
//  fill_region(
//}

void fill_region(struct DigitRegion &region, const CRGB &color) {
  fill_solid(ledsHours + region.start, region.end - region.start, color);
}

void loop() {
  static uint8_t region = 1;
  fill_solid(ledsHours, NUM_LEDS__STRIP1, CRGB::Black);

  if (region == 0) {
    fill_region(digit1.top, CRGB::Red);
    fill_region(digit2.top, CRGB::Red);
  } else if (region == 1) {
    fill_region(digit1.leftTop, CRGB::Orange);
    fill_region(digit2.leftTop, CRGB::Orange);
  } else if (region == 2) {
    fill_region(digit1.leftBottom, CRGB::Yellow);
    fill_region(digit2.leftBottom, CRGB::Yellow);
  } else if (region == 3) {
    fill_region(digit1.rightTop, CRGB::Green);
    fill_region(digit2.rightTop, CRGB::Green);
  } else if (region == 4) {
    fill_region(digit1.rightBottom, CRGB::Blue);
    fill_region(digit2.rightBottom, CRGB::Blue);
  } else if (region == 5) {
    fill_region(digit1.bottom, CRGB::Indigo);
    fill_region(digit2.bottom, CRGB::Indigo);
  } else if (region == 6) {
    fill_region(digit1.center, CRGB::Violet);
    fill_region(digit2.center, CRGB::Violet);
  }

  static uint8_t nLeds = 79;
  //  Serial.println(nLeds);

//  fill_solid(ledsHours + 79, nLeds - 79, CRGB::Red);

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_SECONDS(2) {
    region++;
    if (region > 4) {
      region = 1;
    }

    //    nLeds++;
    //    if (nLeds >= NUM_LEDS__STRIP1) {
    //      nLeds = 79;
    //    }
  }
}
