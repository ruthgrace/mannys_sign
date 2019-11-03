#include <FastLED.h>

FASTLED_USING_NAMESPACE

#include <time.h>
#include <sys/time.h>

#include <WiFi.h>
#include "time.h"

const char* ssid       = "Noisebridge";
const char* password   = "";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -28800;
const int   daylightOffset_sec = 3600;

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

#define DATA_PIN__S2 19
#define DATA_PIN__S3 14 // GPIO21
#define DATA_PIN__S4 2  // S4, hours digits, GPIO2 / ADC12 / TOUCH2
#define DATA_PIN__S5 22 // S5, hours label, GPIO22
#define DATA_PIN__S6 4  // S6, mins digits, GPIO4 / ADC12 / TOUCH0
#define DATA_PIN__S9 17 // GPIO17
#define DATA_PIN__S7 0 // S7, mins label, GPIO18 / VSPI SCK
#define DATA_PIN__S8 16
#define DATA_PIN__S1 5
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define NUM_LEDS__S2 300
#define NUM_LEDS__S4 200
#define NUM_LEDS__S6 200
#define NUM_LEDS__S8 200

#define NUM_LEDS__LABEL 50

#define NUM_LEDS__S9 50
#define NUM_LEDS__S7 50
#define NUM_LEDS__S5 50
#define NUM_LEDS__S3 50
#define NUM_LEDS__S1 300

CRGB ledsDays[NUM_LEDS__S2];
CRGB ledsHours[NUM_LEDS__S4]; // Need to calibrate
CRGB ledsMinutes[NUM_LEDS__S6]; // Need to calibrate?
CRGB ledsSeconds[NUM_LEDS__S8];
CRGB ledsMinsLabel[NUM_LEDS__S7];
CRGB ledsHoursLabel[NUM_LEDS__S5];
CRGB ledsDaysLabel[NUM_LEDS__S3];
CRGB ledsSecsLabel[NUM_LEDS__S9];
CRGB ledsTitle[NUM_LEDS__S1];

#define BRIGHTNESS         128
#define FRAMES_PER_SECOND  120

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

struct DigitLayout daysDigit1 = {
  { 0, 8 },
  { 48, 58 }, // left top
  { 40, 49 }, // left bottom
  { 10, 19 }, // right top
  { 19, 28 }, // right bottom
  { 30, 37 }, // bottom
  { 68, 75 }  // center
};

struct DigitLayout daysDigit2 = {
  { 95, 103 },
  { 105, 114 }, // left top
  { 113, 123 }, // left bottom
  { 143, 153 }, // right top
  { 135, 144 }, // right bottom
  { 125, 132 }, // bottom
  { 79, 86 }
};

struct DigitLayout daysDigit3 = {
  { 156, 163 }, // top
  { 204, 214 }, // left top
  { 196, 206 }, // left bottom
  { 166, 175 }, // right top
  { 175, 184 }, // right bottom
  { 186, 194 }, // bottom
  { 223, 231 }  // center
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
  { 106, 116 }, // left top
  { 115, 124 }, // left bottom
  { 146, 155 }, // right top
  { 137, 147 }, // right bottom
  { 127, 134 }, // bottom
  { 80, 87 }    // center
};

struct DigitLayout minsDigit1 = {
  { 0, 7 },   // top
  { 49, 58 }, // left top
  { 40, 50 }, // left bottom
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
  { 0, 8 },
  { 48, 58 }, // left top
  { 40, 49 }, // left bottom
  { 10, 19 }, // right top
  { 18, 29 }, // right bottom
  { 28, 39 },
  { 68, 78 }
};

struct DigitLayout secsDigit2 = {
  { 91, 105 },
  { 106, 116 }, // left top
  { 115, 124 }, // left bottom
  { 151, 160 }, // right top
  { 139, 152 }, // right bottom
  { 125, 138 },
  { 80, 89 }
};

// 0 = top
// 1 = leftTop
// 2 = leftBottom
// 3 = rightTop
// 4 = rightBottom
// 5 = bottom
// 6 = center

//uint8_t gVisualizeRegions[] = { 6, 0, 1, 2, 5, 4, 3 }; // center top left bottom right
uint8_t gVisualizeRegions[] = { 5, 4, 3 }; // center top left bottom right
//uint8_t gVisualizeRegions[] = { 0, 3, 4, 5, 2, 1, 6 }; // top right bottom left
uint8_t gVisualizeRegionIndex = 0;
uint8_t gVisualizeRegion = gVisualizeRegions[0];

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

struct DigitDisplay {
  struct DigitLayout layout;
  CRGB *leds;
};

const struct DigitDisplay days1 = {
  daysDigit1,
  ledsDays
};

const struct DigitDisplay days2 = {
  daysDigit2,
  ledsDays
};

const struct DigitDisplay days3 = {
  daysDigit3,
  ledsDays
};

const struct DigitDisplay hours1 = {
  hoursDigit1,
  ledsHours
};

const struct DigitDisplay hours2 = {
  hoursDigit2,
  ledsHours
};

const struct DigitDisplay mins1 = {
  minsDigit1,
  ledsMinutes
};

const struct DigitDisplay mins2 = {
  minsDigit2,
  ledsMinutes
};

const struct DigitDisplay secs1 = {
  secsDigit1,
  ledsSeconds
};

const struct DigitDisplay secs2 = {
  secsDigit2,
  ledsSeconds
};

//#include "RTCZero.h"
//
//RTCZero rtc;

void setupRtc() {
  //  rtc.begin();
  byte seconds, minutes, hours;
  byte days, months, years;
  int thour, tminute, tsecond;
  int tmonth, tday, tyear;
  char s_month[5];
  static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  sscanf(__DATE__, "%s %d %d", s_month, &tday, &tyear);
  sscanf(__TIME__, "%d:%d:%d", &thour, &tminute, &tsecond);
  tmonth = (strstr(month_names, s_month) - month_names) / 3;
  years = tyear - 2000;
  months =  tmonth + 1;
  days = tday;
  hours = thour;
  minutes = tminute;
  seconds = tsecond;

  Serial.print("tmonth: ");
  Serial.println(tmonth);
  Serial.print("years: ");
  Serial.println(years);
  Serial.print("months: ");
  Serial.println(months);
  Serial.print("days: ");
  Serial.println(days);
  Serial.print("hours: ");
  Serial.println(hours);
  Serial.print("minutes: ");
  Serial.println(minutes);
  Serial.print("seconds: ");
  Serial.println(seconds);
  Serial.println("---");

  Serial.print("before ");
  time_t now;

  time(&now);
  Serial.println(now);

  //  setTime(hours,minutes,seconds,days,months,tyear);

  Serial.print("after ");
  time(&now);
  Serial.println(now);
  //  rtc.setTime(hours, minutes, seconds);
  //  rtc.setDate(days, months, years);
}

void setupWifi() {
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println(" CONNECTED");
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup() {
  delay(3000); // 3 second delay for recovery

  Serial.begin(9600);

  setupWifi();
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  pinMode(DATA_PIN__S2, OUTPUT);
  pinMode(DATA_PIN__S3, OUTPUT);
  pinMode(DATA_PIN__S4, OUTPUT);
  pinMode(DATA_PIN__S5, OUTPUT);
  pinMode(DATA_PIN__S6, OUTPUT);
  pinMode(DATA_PIN__S9, OUTPUT);
  pinMode(DATA_PIN__S7, OUTPUT);
  pinMode(DATA_PIN__S8, OUTPUT);

  FastLED.addLeds<LED_TYPE, DATA_PIN__S2, COLOR_ORDER>(ledsDays, NUM_LEDS__S2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S4, COLOR_ORDER>(ledsHours, NUM_LEDS__S4).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S6, COLOR_ORDER>(ledsMinutes, NUM_LEDS__S6).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S8, COLOR_ORDER>(ledsSeconds, NUM_LEDS__S8).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S7, COLOR_ORDER>(ledsMinsLabel, NUM_LEDS__S7).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S5, COLOR_ORDER>(ledsHoursLabel, NUM_LEDS__S5).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S9, COLOR_ORDER>(ledsSecsLabel, NUM_LEDS__S9).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S3, COLOR_ORDER>(ledsDaysLabel, NUM_LEDS__S3).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S1, COLOR_ORDER>(ledsTitle, NUM_LEDS__S1).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  int core = xPortGetCoreID();
  Serial.print("Main code running on core ");
  Serial.println(core);

  Serial.print("Compiled at ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  // -- Create the FastLED show task
  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2, &FastLEDshowTaskHandle, FASTLED_SHOW_CORE);
}

void fill_region(struct DigitRegion &region, const CRGB &color) {
  fill_solid(ledsHours + region.start, region.end - region.start, color);
}

void paint_region(struct CRGB *leds, const struct DigitRegion &region, const CRGB &color = CRGB::Red) {
  fill_solid(leds + region.start, region.end - region.start, color);
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

void drawDigit(const struct DigitDisplay &disp, time_t digit) {
  switch (digit) {
    case 0:
      draw0(disp);
      break;
    case 1:
      draw1(disp);
      break;
    case 2:
      draw2(disp);
      break;
    case 3:
      draw3(disp);
      break;
    case 4:
      draw4(disp);
      break;
    case 5:
      draw5(disp);
      break;
    case 6:
      draw6(disp);
      break;
    case 7:
      draw7(disp);
      break;
    case 8:
      draw8(disp);
      break;
    case 9:
      draw9(disp);
      break;
    default:
      Serial.println("WTF? Digit wasn't 0-9");
  }
}

uint8_t gHue = 0;

void visualize_region(struct CRGB *leds, const struct DigitRegion &region) {
  struct CRGB *ledsRegion = leds + region.start;
  uint16_t numLeds = region.end - region.start;

  fill_solid(ledsRegion, numLeds, CRGB::Red);

  ledsRegion[0] = CRGB::Blue;
  ledsRegion[1] = CRGB::Green;

  ledsRegion[numLeds - 1] = CRGB::Yellow;
  ledsRegion[numLeds - 2] = CRGB::DeepPink;
}

void visualize_regions(const struct DigitDisplay &disp) {
  if (gVisualizeRegion == 0) {
    // top
    const struct DigitRegion region = disp.layout.top;
    // Serial.println("Visualizing top");
    // Serial.print("Starts at ");
    // Serial.print(region.start);
    // Serial.println(" (blue)");
    // Serial.print("End is ");
    // Serial.print(region.end);
    // Serial.println(" (yellow)");

    visualize_region(disp.leds, region);
    return;
  }
  if (gVisualizeRegion == 1) {
    // leftTop
    const struct DigitRegion region = disp.layout.leftTop;
    // Serial.println("Visualizing leftTop");
    // Serial.print("Starts at ");
    // Serial.print(region.start);
    // Serial.println(" (blue)");
    // Serial.print("End is ");
    // Serial.print(region.end);
    // Serial.println(" (yellow)");

    visualize_region(disp.leds, region);
    return;
  }
  if (gVisualizeRegion == 2) {
    // leftBottom
    const struct DigitRegion region = disp.layout.leftBottom;
    // Serial.println("Visualizing leftBottom");
    // Serial.print("Starts at ");
    // Serial.print(region.start);
    // Serial.println(" (blue)");
    // Serial.print("End is ");
    // Serial.print(region.end);
    // Serial.println(" (yellow)");

    visualize_region(disp.leds, region);
    return;
  }
  if (gVisualizeRegion == 3) {
    // rightTop
    const struct DigitRegion region = disp.layout.rightTop;
    // Serial.println("Visualizing rightTop");
    // Serial.print("Starts at ");
    // Serial.print(region.start);
    // Serial.println(" (blue)");
    // Serial.print("End is ");
    // Serial.print(region.end);
    // Serial.println(" (yellow)");

    visualize_region(disp.leds, region);
    return;
  }
  if (gVisualizeRegion == 4) {
    // rightBotto
    const struct DigitRegion region = disp.layout.rightBottom;
    // Serial.println("Visualizing rightBottom");
    // Serial.print("Starts at ");
    // Serial.print(region.start);
    // Serial.println(" (blue)");
    // Serial.print("End is ");
    // Serial.print(region.end);
    // Serial.println(" (yellow)");

    visualize_region(disp.leds, region);
    return;
  }
  if (gVisualizeRegion == 5) {
    // bottom
    const struct DigitRegion region = disp.layout.bottom;
    // Serial.println("Visualizing bottom");
    // Serial.print("Starts at ");
    // Serial.print(region.start);
    // Serial.println(" (blue)");
    // Serial.print("End is ");
    // Serial.print(region.end);
    // Serial.println(" (yellow)");

    visualize_region(disp.leds, region);
    return;
  }
  if (gVisualizeRegion == 6) {
    // center
    const struct DigitRegion region = disp.layout.center;
    // Serial.println("Visualizing center");
    // Serial.print("Starts at ");
    // Serial.print(region.start);
    // Serial.println(" (blue)");
    // Serial.print("End is ");
    // Serial.print(region.end);
    // Serial.println(" (yellow)");

    visualize_region(disp.leds, region);
    return;
  }
}

void fill_label(struct CRGB *labelLeds) {
  fill_solid(labelLeds, NUM_LEDS__LABEL, CRGB::Blue);
}

static struct DigitDisplay visualized = mins2;

// End date/time: 8pm Nov 3 2020

struct tm election_date = {
  0,
  0,
  20,
  3,
  10,
  120,
  2,
  306,
  0
};

uint16_t daysBetween(struct tm &date1, struct tm &date2) {
  time_t x = mktime(&date1);
  time_t y = mktime(&date2);

  return (uint16_t)difftime(y, x) / (60 * 60 * 24);
}

int gLastSec = 0;

void loop() {
  //  FastLED.clear();
  fadeToBlackBy(ledsSeconds, NUM_LEDS__S8, 20);
//  fadeToBlackBy(ledsMinsLabel, NUM_LEDS__S7, 100);
//  fadeToBlackBy(ledsHoursLabel, NUM_LEDS__S5, 100);
//  fadeToBlackBy(ledsDaysLabel, NUM_LEDS__S3, 100);
//  fadeToBlackBy(ledsSecsLabel, NUM_LEDS__S9, 100);
//  fadeToBlackBy(ledsTitle, NUM_LEDS__S1, 100);

  // When the minute is about to change, fade out all the other counters
  if (gLastSec == 0) {
    fadeToBlackBy(ledsDays, NUM_LEDS__S2, 20);
    fadeToBlackBy(ledsHours, NUM_LEDS__S4, 20);
    fadeToBlackBy(ledsMinutes, NUM_LEDS__S6, 20);
  }

  //  printLocalTime();

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  if (gLastSec == timeinfo.tm_sec) {
    FastLEDshowESP32();
    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);
    return;
  }

  gLastSec = timeinfo.tm_sec;

  time_t seconds = timeinfo.tm_sec;

  //  uint16_t daysDiff = daysBetween(timeinfo, election_date);

  time_t nowTime = mktime(&timeinfo);
  time_t electionTime = mktime(&election_date);

  double diff = difftime(electionTime, nowTime);
  unsigned long daysDiff = (unsigned long)diff / 60 / 60 / 24;

  drawDigit(days1, daysDiff / 100);
  drawDigit(days2, daysDiff % 100 / 10);
  drawDigit(days3, daysDiff % 100 % 10);

  uint8_t hoursOnly = ((unsigned long)(diff) / 60 / 60) % 24;

  drawDigit(hours1, hoursOnly / 10);
  drawDigit(hours2, hoursOnly % 10);

  Serial.print("diff: ");
  Serial.println(diff);

  uint8_t minsOnly = ((unsigned long)(diff) / 60) % 60;

  Serial.print("mins only: ");
  Serial.println(minsOnly);

  drawDigit(mins1, minsOnly / 10);
  drawDigit(mins2, minsOnly % 10);

  uint8_t secsOnly = (unsigned long)(diff) % 60;

  drawDigit(secs1, secsOnly / 10);
  drawDigit(secs2, secsOnly % 10);

  //
  //    fill_solid(ledsMinsLabel, NUM_LEDS__S9, CRGB::Green);
  fill_label(ledsMinsLabel);
  fill_label(ledsSecsLabel);
  fill_label(ledsHoursLabel);
  fill_label(ledsDaysLabel);

  fill_solid(ledsTitle, NUM_LEDS__S1, CRGB::Blue);
//  fill_rainbow(ledsTitle, NUM_LEDS__S1, gHue);
  //  fill_solid(ledsMinsLabel, NUM_LEDS__S7, CRGB::DeepPink);
  //  fill_solid(ledsSecsLabel, NUM_LEDS__S9, CRGB::Green);
  //  fill_solid(ledsHoursLabel, NUM_LEDS__S5, CRGB::Gold);
  //  fill_solid(ledsDaysLabel, NUM_LEDS__S3, CRGB::Purple);

  // send the 'leds' array out to the actual LED strip
  FastLEDshowESP32();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_MILLISECONDS(2) {
    gHue++;
  }

  EVERY_N_SECONDS(8) {
    gVisualizeRegionIndex++;
    if (gVisualizeRegionIndex >= ARRAY_SIZE(gVisualizeRegions)) gVisualizeRegionIndex = 0;
    gVisualizeRegion = gVisualizeRegions[gVisualizeRegionIndex];


    if (gVisualizeRegion == 0) {
      // top
      const struct DigitRegion region = visualized.layout.top;
      Serial.println("---");
      Serial.println("Visualizing top");
      Serial.print("Starts at ");
      Serial.print(region.start);
      Serial.println(" (blue)");
      Serial.print("End is ");
      Serial.print(region.end);
      Serial.println(" (yellow)");
    }
    if (gVisualizeRegion == 1) {
      // leftTop
      const struct DigitRegion region = visualized.layout.leftTop;
      Serial.println("---");
      Serial.println("Visualizing leftTop");
      Serial.print("Starts at ");
      Serial.print(region.start);
      Serial.println(" (blue)");
      Serial.print("End is ");
      Serial.print(region.end);
      Serial.println(" (yellow)");
    }
    if (gVisualizeRegion == 2) {
      // leftBottom
      const struct DigitRegion region = visualized.layout.leftBottom;
      Serial.println("---");
      Serial.println("Visualizing leftBottom");
      Serial.print("Starts at ");
      Serial.print(region.start);
      Serial.println(" (blue)");
      Serial.print("End is ");
      Serial.print(region.end);
      Serial.println(" (yellow)");
    }
    if (gVisualizeRegion == 3) {
      // rightTop
      const struct DigitRegion region = visualized.layout.rightTop;
      Serial.println("---");
      Serial.println("Visualizing rightTop");
      Serial.print("Starts at ");
      Serial.print(region.start);
      Serial.println(" (blue)");
      Serial.print("End is ");
      Serial.print(region.end);
      Serial.println(" (yellow)");
    }
    if (gVisualizeRegion == 4) {
      // rightBotto
      const struct DigitRegion region = visualized.layout.rightBottom;
      Serial.println("---");
      Serial.println("Visualizing rightBottom");
      Serial.print("Starts at ");
      Serial.print(region.start);
      Serial.println(" (blue)");
      Serial.print("End is ");
      Serial.print(region.end);
      Serial.println(" (yellow)");
    }
    if (gVisualizeRegion == 5) {
      // bottom
      const struct DigitRegion region = visualized.layout.bottom;
      Serial.println("---");
      Serial.println("Visualizing bottom");
      Serial.print("Starts at ");
      Serial.print(region.start);
      Serial.println(" (blue)");
      Serial.print("End is ");
      Serial.print(region.end);
      Serial.println(" (yellow)");
    }
    if (gVisualizeRegion == 6) {
      // center
      const struct DigitRegion region = visualized.layout.center;
      Serial.println("---");
      Serial.println("Visualizing center");
      Serial.print("Starts at ");
      Serial.print(region.start);
      Serial.println(" (blue)");
      Serial.print("End is ");
      Serial.print(region.end);
      Serial.println(" (yellow)");
    }
  }
}
