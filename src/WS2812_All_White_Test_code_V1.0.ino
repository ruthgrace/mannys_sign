#include <FastLED.h>

  //for fixing any bootstrap pin issues on startup for GPIO 13, 15 (unused right now)
//PIN_FUNC_SELECT(GPIO_PIN_REG_13, PIN_FUNC_GPIO);

#define S1  19
#define S2  18
#define S3  5
#define S4  17
#define S5  16
#define S6  4
#define S7  14
#define S8  15
#define S9  13

#define NUMOFLEDS  300   //num of LEDs to light up total.  Some panels are only 50, but by keeping this number large you dont have to worry about it for testing
#define BRIGHTNESS    192   //Global brightness cap
#define FRAMES_PER_SECOND  120  //Frame rate. Needs to be high for things like fade to work well

//Constructor for combining declared variable with size a.k.a. "CRGBArray< SIZE > Class Template"
CRGB DATA_S1[NUMOFLEDS];
CRGB DATA_S2[NUMOFLEDS];
CRGB DATA_S3[NUMOFLEDS];
CRGB DATA_S4[NUMOFLEDS];
CRGB DATA_S5[NUMOFLEDS];
CRGB DATA_S6[NUMOFLEDS];
CRGB DATA_S7[NUMOFLEDS];
CRGB DATA_S8[NUMOFLEDS];
CRGB DATA_S9[NUMOFLEDS];

int i = 0;


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
  
  FastLED.addLeds<WS2812, S1, RGB>(DATA_S1, NUMOFLEDS);
  FastLED.addLeds<WS2812, S2, RGB>(DATA_S2, NUMOFLEDS);
  FastLED.addLeds<WS2812, S3, RGB>(DATA_S3, NUMOFLEDS);
  FastLED.addLeds<WS2812, S4, RGB>(DATA_S4, NUMOFLEDS);
  FastLED.addLeds<WS2812, S5, RGB>(DATA_S5, NUMOFLEDS);
  FastLED.addLeds<WS2812, S6, RGB>(DATA_S6, NUMOFLEDS);
  FastLED.addLeds<WS2812, S7, RGB>(DATA_S7, NUMOFLEDS);
  FastLED.addLeds<WS2812, S8, RGB>(DATA_S8, NUMOFLEDS);
  FastLED.addLeds<WS2812, S9, RGB>(DATA_S9, NUMOFLEDS);

  //set master brightness control
  //FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  //Fade on bright blue 
  for (; i <= NUMOFLEDS; i++) {
    DATA_S1[i] = CRGB(10, 10, 255);
    DATA_S2[i] = CRGB(10, 10, 255);
    DATA_S3[i] = CRGB(10, 10, 255);
    DATA_S4[i] = CRGB(10, 10, 255);
    DATA_S5[i] = CRGB(10, 10, 255);
    DATA_S6[i] = CRGB(10, 10, 255);
    DATA_S7[i] = CRGB(10, 10, 255);
    DATA_S8[i] = CRGB(10, 10, 255);
    DATA_S9[i] = CRGB(10, 10, 255);
    FastLED.show();
    delay(5);
  }
}