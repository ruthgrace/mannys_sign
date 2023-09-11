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
  for (int i = 300; i >= 0; i--) {
    leds[i] = CRGB ( 255, 255, 255);
    FastLED.show();
    delay(40);
 }
}
