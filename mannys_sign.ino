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

/* ESP32 specific stuff to avoid flickering?? */

// -- The core to run FastLED.show()
#define FASTLED_SHOW_CORE 0

// -- Task handles for use in the notifications
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;

/** show() for ESP32
    Call this function instead of FastLED.show(). It signals core 0 to issue a show,
    then waits for a notification that it is done.
*/
void FastLEDshowESP32()
{
  if (userTaskHandle == 0) {
    // -- Store the handle of the current task, so that the show task can
    //    notify it when it's done
    userTaskHandle = xTaskGetCurrentTaskHandle();

    // -- Trigger the show task
    xTaskNotifyGive(FastLEDshowTaskHandle);

    // -- Wait to be notified that it's done
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 200 );
    ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    userTaskHandle = 0;
  }
}

/** show Task
    This function runs on core 0 and just waits for requests to call FastLED.show()
*/
void FastLEDshowTask(void *pvParameters)
{
  // -- Run forever...
  for (;;) {
    // -- Wait for the trigger
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // -- Do the show (synchronously)
    FastLED.show();

    // -- Notify the calling task
    xTaskNotifyGive(userTaskHandle);
  }
}

struct DigitRegion {
  uint8_t start;
  uint8_t end;
};

struct DigitLayout {
  struct DigitRegion top;
  struct DigitRegion leftTop;
  struct DigitRegion leftBottom;
  struct DigitRegion rightTop;
  struct DigitRegion rightBottom;
  struct DigitRegion bottom;
  struct DigitRegion center;
};

struct DigitLayout digit1 = {
  { 0, 8 },
  { 48, 58 }, // left top
  { 40, 49 }, // left bottom
  { 10, 19 }, // right top
  { 18, 29 }, // right bottom
  { 28, 39 },
  { 68, 78 }
};

struct DigitLayout digit2 = {
  { 91, 105 },
  { 106, 116 }, // left top
  { 115, 124 }, // left bottom
  { 151, 160 }, // right top
  { 139, 152 }, // right bottom
  { 125, 138 },
  { 80, 89 }
};

struct DigitDisplay {
  struct DigitLayout layout;
  CRGB *leds;
};

const struct DigitDisplay hours1 = {
  digit1,
  ledsHours
};

const struct DigitDisplay hours2 = {
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

  int core = xPortGetCoreID();
  Serial.print("Main code running on core ");
  Serial.println(core);

  // -- Create the FastLED show task
  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2, &FastLEDshowTaskHandle, FASTLED_SHOW_CORE);
}

void fill_region(struct DigitRegion &region, const CRGB &color) {
  fill_solid(ledsHours + region.start, region.end - region.start, color);
}

void paint_region(struct CRGB *leds, const struct DigitRegion &region) {
  fill_solid(leds + region.start, region.end - region.start, CRGB::Red);
}

void paintRight(const struct DigitDisplay &disp) {
  paint_region(disp.leds, disp.layout.rightTop);
  paint_region(disp.leds, disp.layout.rightBottom);
}

void paintLeft(const struct DigitDisplay &disp) {
  paint_region(disp.leds, disp.layout.leftTop);
  paint_region(disp.leds, disp.layout.leftBottom);
}

void draw1(const struct DigitDisplay &disp) {
  paintRight(disp);
}

void draw2(const struct DigitDisplay &disp) {
  paint_region(disp.leds, disp.layout.top);
  paint_region(disp.leds, disp.layout.center);
  paint_region(disp.leds, disp.layout.bottom);

  paint_region(disp.leds, disp.layout.rightTop);
  paint_region(disp.leds, disp.layout.leftBottom);
}

void draw3(const struct DigitDisplay &disp) {
  paint_region(disp.leds, disp.layout.top);
  paint_region(disp.leds, disp.layout.center);
  paint_region(disp.leds, disp.layout.bottom);
  paintRight(disp);
}

void draw4(const struct DigitDisplay &disp) {
  paint_region(disp.leds, disp.layout.center);
  paint_region(disp.leds, disp.layout.leftTop);
  paintRight(disp);
}

void draw5(const struct DigitDisplay &disp) {
  paint_region(disp.leds, disp.layout.top);
  paint_region(disp.leds, disp.layout.center);
  paint_region(disp.leds, disp.layout.bottom);
  paint_region(disp.leds, disp.layout.leftTop);
  paint_region(disp.leds, disp.layout.rightBottom);
}

void draw6(const struct DigitDisplay &disp) {
  paint_region(disp.leds, disp.layout.top);
  paint_region(disp.leds, disp.layout.center);
  paint_region(disp.leds, disp.layout.bottom);
  paint_region(disp.leds, disp.layout.rightBottom);
  paintLeft(disp);
}

void draw7(const struct DigitDisplay &disp) {
  paint_region(disp.leds, disp.layout.top);
  paintRight(disp);
}

void draw8(const struct DigitDisplay &disp) {
  paintRight(disp);
  paintLeft(disp);

  paint_region(disp.leds, disp.layout.top);
  paint_region(disp.leds, disp.layout.center);
  paint_region(disp.leds, disp.layout.bottom);
}

void draw9(const struct DigitDisplay &disp) {
  paintRight(disp);

  paint_region(disp.leds, disp.layout.leftTop);
  paint_region(disp.leds, disp.layout.top);
  paint_region(disp.leds, disp.layout.center);
}

void draw0(const struct DigitDisplay &disp) {
  paintRight(disp);
  paintLeft(disp);
  paint_region(disp.leds, disp.layout.top);
  paint_region(disp.leds, disp.layout.bottom);
}

void loop() {
  draw7(hours1);
  draw0(hours2);
//  draw9(hours1);

  // send the 'leds' array out to the actual LED strip
  FastLEDshowESP32();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}
