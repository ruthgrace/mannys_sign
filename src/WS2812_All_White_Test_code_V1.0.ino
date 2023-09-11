#include <FastLED.h>

#define LEDPIN     5     //Pin from MCU to WS2812 Data
#define NUMOFLEDS  300   //num of LEDs to light up total

CRGB leds[NUMOFLEDS];

void setup() {

  FastLED.addLeds<WS2812, LEDPIN, GRB>(leds, NUMOFLEDS);
  //leds[300] = CRGB ( 255, 255, 255);  //doesnt work for some reason - i gotta read docum
  //FastLED.show();
}

void loop() {
  // bright white 5 seconds
  for (int i = 0; i <= 600; i++) {
    leds[i] = CRGB ( 255, 255, 255);
    FastLED.show();
    delay(8);
  }
  // fade off to red 1 second
  for (int i = 0; i <= 120; i++) {
    leds[i] = CRGB ( 2*i, 0, 0);
    FastLED.show();
    delay(8);
  }
  // fade red to off 1 second
  for (int i = 120; i >= 0; i--) {
    leds[i] = CRGB ( 2*i, 0, 0);
    FastLED.show();
    delay(8);
  }
  // fade off to green 1 second
  for (int i = 0; i <= 120; i++) {
    leds[i] = CRGB (0, 2*i, 0);
    FastLED.show();
    delay(8);
  }
  // fade green to off 1 second
  for (int i = 120; i >= 0; i--) {
    leds[i] = CRGB (0, 2*i, 0);
    FastLED.show();
    delay(8);
  }
  // fade off to blue 1 second
  for (int i = 0; i <= 120; i++) {
    leds[i] = CRGB (0, 0, 2*i);
    FastLED.show();
    delay(8);
  }
  // fade blue to off 1 second
  for (int i = 120; i >= 0; i--) {
    leds[i] = CRGB (0, 0, 2*i);
    FastLED.show();
    delay(8);
  }
  // fade off to white 1 second
  for (int i = 0; i <= 120; i++) {
    leds[i] = CRGB (2*i, 2*i, 2*i);
    FastLED.show();
    delay(8);
  }
  // fade white to off 1 second
  for (int i = 120; i >= 0; i--) {
    leds[i] = CRGB (2*i, 2*i, 2*i);
    FastLED.show();
    delay(8);
  }
  // fade off to purple 1 second
  for (int i = 0; i <= 120; i++) {
    leds[i] = CRGB (2*i, 0, 2*i);
    FastLED.show();
    delay(8);
  }
  // fade purple to off 1 second
  for (int i = 120; i >= 0; i--) {
    leds[i] = CRGB (2*i, 0, 2*i);
    FastLED.show();
    delay(8);
  }
  // fade off to orange 1 second
  for (int i = 0; i <= 120; i++) {
    leds[i] = CRGB (2*i, 2*i, 0);
    FastLED.show();
    delay(8);
  }
  // fade orange to off 1 second
  for (int i = 120; i >= 0; i--) {
    leds[i] = CRGB (2*i, 2*i, 0);
    FastLED.show();
    delay(8);
  }
  // fade off to cyan 1 second
  for (int i = 0; i <= 120; i++) {
    leds[i] = CRGB (0, 2*i, 2*i);
    FastLED.show();
    delay(8);
  }
  // fade cyan to off 1 second
  for (int i = 120; i >= 0; i--) {
    leds[i] = CRGB (0, 2*i, 2*i);
    FastLED.show();
    delay(8);
  }
}
