#include <FastLED.h>

// this code is for debugging the definition of where digit segments start and end.
// it flashes in red color for one second each:
// * top two vertical segments of all digits
// * bottom two vertical segments of all digits
// * middle three horizontal segments of all digits

// top power supply
#define S1 19   // Seconds counter
#define S2 18   // Minutes counter
#define S3 5    // Hours label
#define S4 17   // Days label
// middle power supply
#define S5 16   // Seconds label
#define S6 4    // Hours counter
#define S7 14   // ELECTION COUNTDOWN label
// bottom power supply
#define S8 15   // Days counter
#define S9 13   // Minutes label

#define NUM_LEDS__LARGE 300 // # LEDs for title display and days counter (3 digits)
#define NUM_LEDS__COUNTER 200 // Assume all counters are 200 leds
#define NUM_LEDS__LABEL 50 // Assume all labels are 50 leds
#define BRIGHTNESS    192   //Global brightness cap
#define FRAMES_PER_SECOND  120  //Frame rate. Needs to be high for things like fade to work well

//Constructor for combining declared variable with size a.k.a. "CRGBArray< SIZE > Class Template"
CRGB DATA_S1[NUM_LEDS__COUNTER];
CRGB DATA_S2[NUM_LEDS__COUNTER];
CRGB DATA_S3[NUM_LEDS__LABEL];
CRGB DATA_S4[NUM_LEDS__LABEL];
CRGB DATA_S5[NUM_LEDS__LABEL];
CRGB DATA_S6[NUM_LEDS__COUNTER];
CRGB DATA_S7[NUM_LEDS__LARGE];
CRGB DATA_S8[NUM_LEDS__LARGE];
CRGB DATA_S9[NUM_LEDS__LABEL];

CRGB RED = CRGB(0, 255, 0);
CRGB BLACK = CRGB(0, 0, 0);

// Definition of one segment in a digit
struct DigitRegion {
  uint8_t start;
  uint8_t end;
};

// Definition of how a 7 segment digit is laid out
struct DigitLayout {
  struct DigitRegion top;
  struct DigitRegion leftTop;
  struct DigitRegion leftBottom;
  struct DigitRegion rightTop;
  struct DigitRegion rightBottom;
  struct DigitRegion bottom;
  struct DigitRegion center;
};

struct DigitLayout daysDigit1 = {
  { 0, 8 },   // top
  { 47, 57 }, // left top
  { 39, 48 }, // left bottom
  { 9, 18 }, // right top
  { 18, 27 }, // right bottom
  { 29, 36 }, // bottom
  { 67, 74 }  // center
};

struct DigitLayout daysDigit2 = {
  { 94, 102 },  // top
  { 104, 113 }, // left top
  { 112, 121 }, // left bottom
  { 142, 152 }, // right top
  { 134, 143 }, // right bottom
  { 124, 131 }, // bottom
  { 78, 85 }    // center
};

struct DigitLayout daysDigit3 = {
  { 155, 162 }, // top
  { 204, 213 }, // left top
  { 195, 204 }, // left bottom
  { 165, 174 }, // right top
  { 174, 183 }, // right bottom
  { 185, 193 }, // bottom
  { 222, 230 }  // center
};

struct DigitLayout hoursDigit1 = {
  { 0, 7 },   // top
  { 48, 58 }, // left top
  { 40, 49 }, // left bottom
  { 10, 20 }, // right top
  { 19, 28 }, // right bottom
  { 30, 37 }, // bottom
  { 68, 75 }  // center
};

struct DigitLayout hoursDigit2 = {
  { 96, 103 },  // top
  { 106, 115 }, // left top
  { 115, 124 }, // left bottom
  { 146, 155 }, // right top
  { 137, 147 }, // right bottom
  { 127, 134 }, // bottom
  { 80, 87 }    // center
};

struct DigitLayout minsDigit1 = {
  { 0, 7 },   // top
  { 49, 58 }, // left top
  { 40, 49 }, // left bottom
  { 10, 19 }, // right top
  { 19, 28 }, // right bottom
  { 30, 38 }, // bottom
  { 68, 76 }  // center
};

struct DigitLayout minsDigit2 = {
  { 96, 104 },  // top
  { 106, 115 }, // left top
  { 115, 125 }, // left bottom
  { 146, 155 }, // right top
  { 137, 146 }, // right bottom
  { 126, 134 }, // bottom
  { 80, 87 }    // center
};

struct DigitLayout secsDigit1 = {
  { 0, 8 },   // top
  { 48, 58 }, // left top
  { 39, 49 }, // left bottom
  { 10, 19 }, // right top
  { 19, 29 }, // right bottom
  { 28, 37 }, // bottom
  { 68, 78 }  // center
};

struct DigitLayout secsDigit2 = {
  { 91, 105 },   // top
  { 106, 115 }, // left top
  { 115, 124 }, // left bottom
  { 149, 159 }, // right top
  { 138, 149 }, // right bottom
  { 127, 136 }, // bottom
  { 79, 89 }    // center
};

void color_top_vertical_segments(struct CRGB *data_pin, DigitLayout digit_layout) {
  fill_solid(data_pin + digit_layout.leftTop.start, digit_layout.leftTop.end - digit_layout.leftTop.start, RED);
  fill_solid(data_pin + digit_layout.rightTop.start, digit_layout.rightTop.end - digit_layout.rightTop.start, RED);
}

void color_bottom_vertical_segments(struct CRGB *data_pin, DigitLayout digit_layout) {
  fill_solid(data_pin + digit_layout.leftBottom.start, digit_layout.leftBottom.end - digit_layout.leftBottom.start, RED);
  fill_solid(data_pin + digit_layout.rightBottom.start, digit_layout.rightBottom.end - digit_layout.rightBottom.start, RED);
}

void color_horizontal_segments(struct CRGB *data_pin, DigitLayout digit_layout) {
  fill_solid(data_pin + digit_layout.top.start, digit_layout.top.end - digit_layout.top.start, RED);
  fill_solid(data_pin + digit_layout.bottom.start, digit_layout.bottom.end - digit_layout.bottom.start, RED);
  fill_solid(data_pin + digit_layout.center.start, digit_layout.center.end - digit_layout.center.start, RED);
}

void color_all_black(struct CRGB *data_pin, DigitLayout digit_layout) {
  fill_solid(data_pin + digit_layout.leftTop.start, digit_layout.leftTop.end - digit_layout.leftTop.start, BLACK);
  fill_solid(data_pin + digit_layout.rightTop.start, digit_layout.rightTop.end - digit_layout.rightTop.start, BLACK);
  fill_solid(data_pin + digit_layout.leftBottom.start, digit_layout.leftBottom.end - digit_layout.leftBottom.start, BLACK);
  fill_solid(data_pin + digit_layout.rightBottom.start, digit_layout.rightBottom.end - digit_layout.rightBottom.start, BLACK);
  fill_solid(data_pin + digit_layout.top.start, digit_layout.top.end - digit_layout.top.start, BLACK);
  fill_solid(data_pin + digit_layout.bottom.start, digit_layout.bottom.end - digit_layout.bottom.start, BLACK);
  fill_solid(data_pin + digit_layout.center.start, digit_layout.center.end - digit_layout.center.start, BLACK);
}

void setup() {

  //Set outputs, although the FastLED library already seems to do this, so maybe redundant.  Doesnt hurt to add it.
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S4, OUTPUT);
  pinMode(S5, OUTPUT);
  pinMode(S6, OUTPUT);
  pinMode(S7, OUTPUT);
  pinMode(S8, OUTPUT);
  pinMode(S9, OUTPUT);
  
  FastLED.addLeds<WS2812, S1, RGB>(DATA_S1, NUM_LEDS__COUNTER);
  FastLED.addLeds<WS2812, S2, RGB>(DATA_S2, NUM_LEDS__COUNTER);
  FastLED.addLeds<WS2812, S3, RGB>(DATA_S3, NUM_LEDS__LABEL);
  FastLED.addLeds<WS2812, S4, RGB>(DATA_S4, NUM_LEDS__LABEL);
  FastLED.addLeds<WS2812, S5, RGB>(DATA_S5, NUM_LEDS__LABEL);
  FastLED.addLeds<WS2812, S6, RGB>(DATA_S6, NUM_LEDS__COUNTER);
  FastLED.addLeds<WS2812, S7, RGB>(DATA_S7, NUM_LEDS__LARGE);
  FastLED.addLeds<WS2812, S8, RGB>(DATA_S8, NUM_LEDS__LARGE);
  FastLED.addLeds<WS2812, S9, RGB>(DATA_S9, NUM_LEDS__LABEL);

  //set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  
  color_top_vertical_segments(DATA_S1, secsDigit1);
  color_top_vertical_segments(DATA_S1, secsDigit2);
  color_top_vertical_segments(DATA_S2, minsDigit1);
  color_top_vertical_segments(DATA_S2, minsDigit2);
  color_top_vertical_segments(DATA_S6, hoursDigit1);
  color_top_vertical_segments(DATA_S6, hoursDigit2);
  color_top_vertical_segments(DATA_S8, daysDigit1);
  color_top_vertical_segments(DATA_S8, daysDigit2);
  color_top_vertical_segments(DATA_S8, daysDigit3);
  FastLED.show();
  delay(1000);
  
  color_all_black(DATA_S1, secsDigit1);
  color_all_black(DATA_S1, secsDigit2);
  color_all_black(DATA_S2, minsDigit1);
  color_all_black(DATA_S2, minsDigit2);
  color_all_black(DATA_S6, hoursDigit1);
  color_all_black(DATA_S6, hoursDigit2);
  color_all_black(DATA_S8, daysDigit1);
  color_all_black(DATA_S8, daysDigit2);
  color_all_black(DATA_S8, daysDigit3);

  color_bottom_vertical_segments(DATA_S1, secsDigit1);
  color_bottom_vertical_segments(DATA_S1, secsDigit2);
  color_bottom_vertical_segments(DATA_S2, minsDigit1);
  color_bottom_vertical_segments(DATA_S2, minsDigit2);
  color_bottom_vertical_segments(DATA_S6, hoursDigit1);
  color_bottom_vertical_segments(DATA_S6, hoursDigit2);
  color_bottom_vertical_segments(DATA_S8, daysDigit1);
  color_bottom_vertical_segments(DATA_S8, daysDigit2);
  color_bottom_vertical_segments(DATA_S8, daysDigit3);
  FastLED.show();
  delay(1000);
  
  color_all_black(DATA_S1, secsDigit1);
  color_all_black(DATA_S1, secsDigit2);
  color_all_black(DATA_S2, minsDigit1);
  color_all_black(DATA_S2, minsDigit2);
  color_all_black(DATA_S6, hoursDigit1);
  color_all_black(DATA_S6, hoursDigit2);
  color_all_black(DATA_S8, daysDigit1);
  color_all_black(DATA_S8, daysDigit2);
  color_all_black(DATA_S8, daysDigit3);
  
  color_horizontal_segments(DATA_S1, secsDigit1);
  color_horizontal_segments(DATA_S1, secsDigit2);
  color_horizontal_segments(DATA_S2, minsDigit1);
  color_horizontal_segments(DATA_S2, minsDigit2);
  color_horizontal_segments(DATA_S6, hoursDigit1);
  color_horizontal_segments(DATA_S6, hoursDigit2);
  color_horizontal_segments(DATA_S8, daysDigit1);
  color_horizontal_segments(DATA_S8, daysDigit2);
  color_horizontal_segments(DATA_S8, daysDigit3);
  FastLED.show();
  delay(1000);
  
  color_all_black(DATA_S1, secsDigit1);
  color_all_black(DATA_S1, secsDigit2);
  color_all_black(DATA_S2, minsDigit1);
  color_all_black(DATA_S2, minsDigit2);
  color_all_black(DATA_S6, hoursDigit1);
  color_all_black(DATA_S6, hoursDigit2);
  color_all_black(DATA_S8, daysDigit1);
  color_all_black(DATA_S8, daysDigit2);
  color_all_black(DATA_S8, daysDigit3);
}
