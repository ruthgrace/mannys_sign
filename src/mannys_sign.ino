#include <FastLED.h>

FASTLED_USING_NAMESPACE

#include <time.h>
#include <sys/time.h>

#include <WiFi.h>
#include "time.h"

const char* ssid       = "breggrug";
const char* password   = "imbreggstatic";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -28800; // -0800
const int   daylightOffset_sec = 3600;

// top power supply
#define DATA_PIN__S1 19   // Seconds counter
#define DATA_PIN__S2 18   // Minutes counter
#define DATA_PIN__S3 5    // Hours label
#define DATA_PIN__S4 17   // Days label
// middle power supply
#define DATA_PIN__S5 16   // Seconds label
#define DATA_PIN__S6 4    // Hours counter
#define DATA_PIN__S7 14   // ELECTION COUNTDOWN label
// bottom power supply
#define DATA_PIN__S8 15   // Days counter
#define DATA_PIN__S9 13   // Minutes label


#define S1  19
#define S2  18
#define S3  5
#define S4  17
#define S5  16
#define S6  4
#define S7  14
#define S8  15
#define S9  13

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define NUM_LEDS__LARGE 300 // # LEDs for title display and days counter (3 digits)
#define NUM_LEDS__COUNTER 200 // Assume all counters are 200 leds
#define NUM_LEDS__LABEL 50 // Assume all labels are 50 leds

CRGB ledsDays[NUM_LEDS__LARGE];
CRGB ledsHours[NUM_LEDS__COUNTER];
CRGB ledsMinutes[NUM_LEDS__COUNTER];
CRGB ledsSeconds[NUM_LEDS__COUNTER];
CRGB ledsMinsLabel[NUM_LEDS__LABEL];
CRGB ledsHoursLabel[NUM_LEDS__LABEL];
CRGB ledsDaysLabel[NUM_LEDS__LABEL];
CRGB ledsSecsLabel[NUM_LEDS__LABEL];
CRGB ledsTitle[NUM_LEDS__LARGE];

char stringBuffer [128]; // for printing out countdown to serial monitor

// Global brightness cap
#define BRIGHTNESS         192

// Frame rate. Needs to be high for things like fade to work well
#define FRAMES_PER_SECOND  120

/* ESP32 specific stuff to avoid flickering. Don't touch this */

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

/* Ok, you can touch the code again beyond here  */

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

// We need to connect to the WiFi so we can get the current time
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

void printLocalTime() {
  struct tm now;
  if (!getLocalTime(&now)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&now, "%A, %B %d %Y %H:%M:%S");
}

void setup() {
  delay(3000); // 3 second delay for recovery

  Serial.begin(9600);

  // Connect to wifi, set time, then disconnect
  setupWifi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  pinMode(DATA_PIN__S1, OUTPUT);
  pinMode(DATA_PIN__S2, OUTPUT);
  pinMode(DATA_PIN__S3, OUTPUT);
  pinMode(DATA_PIN__S4, OUTPUT);
  pinMode(DATA_PIN__S5, OUTPUT);
  pinMode(DATA_PIN__S6, OUTPUT);
  pinMode(DATA_PIN__S7, OUTPUT);
  pinMode(DATA_PIN__S8, OUTPUT);
  pinMode(DATA_PIN__S9, OUTPUT);

  FastLED.addLeds<LED_TYPE, DATA_PIN__S1, COLOR_ORDER>(ledsSeconds, NUM_LEDS__COUNTER).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S2, COLOR_ORDER>(ledsMinutes, NUM_LEDS__COUNTER).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S3, COLOR_ORDER>(ledsHoursLabel, NUM_LEDS__LABEL).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S4, COLOR_ORDER>(ledsDaysLabel, NUM_LEDS__LABEL).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S5, COLOR_ORDER>(ledsSecsLabel, NUM_LEDS__LABEL).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S6, COLOR_ORDER>(ledsHours, NUM_LEDS__COUNTER).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S7, COLOR_ORDER>(ledsTitle, NUM_LEDS__LARGE).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S8, COLOR_ORDER>(ledsDays, NUM_LEDS__LARGE).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN__S9, COLOR_ORDER>(ledsMinsLabel, NUM_LEDS__LABEL).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

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

void fill_label(struct CRGB *labelLeds) {
  fill_solid(labelLeds, NUM_LEDS__LABEL, CRGB::Blue);
}

//test code for 12:05am Tues Dec 19 2024
/* struct tm election_date = {
  0,
  5,
  0,
  19,
  11,
  123,
  2,
  352,
  0
}; */

// End date/time: 8pm November 5 2024
// See: http://www.cplusplus.com/reference/ctime/tm/
 struct tm election_date = {
  0,
  0,
  20,
  5,
  10,
  124,
  2,
  309,
  0
};

int gLastSec = 0;

void loop() {
  // We always fade the seconds display since it is changing every second
  fadeToBlackBy(ledsSeconds, NUM_LEDS__COUNTER, 20);

  // When the minute is about to change, fade out all the other counters
  if (gLastSec == 0) {
    fadeToBlackBy(ledsDays, NUM_LEDS__LARGE, 20);
    fadeToBlackBy(ledsHours, NUM_LEDS__COUNTER, 20);
    fadeToBlackBy(ledsMinutes, NUM_LEDS__COUNTER, 20);
  }

  fill_label(ledsMinsLabel);
  fill_label(ledsSecsLabel);
  fill_label(ledsHoursLabel);
  fill_label(ledsDaysLabel);

//  uint8_t highlightPosition = scale8(beat8(60), NUM_LEDS__LABEL / 2);
//  ledsMinsLabel[highlightPosition] = CRGB::White;
//  ledsMinsLabel[highlightPosition * 2] = CRGB::White;

  struct tm now;
  if (!getLocalTime(&now)) {
    Serial.println("Failed to obtain time");
    return;
  }

  if (gLastSec == now.tm_sec) {
    FastLEDshowESP32();
    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);
    return;
  }

  gLastSec = now.tm_sec;

  time_t seconds = now.tm_sec;

  time_t nowTime = mktime(&now);
  time_t electionTime = mktime(&election_date);

  double diff = difftime(electionTime, nowTime);
  unsigned long daysDiff = (unsigned long)diff / 60 / 60 / 24;
  uint8_t hoursOnly = ((unsigned long)(diff) / 60 / 60) % 24;
  uint8_t minsOnly = ((unsigned long)(diff) / 60) % 60;
  uint8_t secsOnly = (unsigned long)(diff) % 60;
  sprintf (stringBuffer, "Countdown: %3d days, %2d hours, %2d minutes, and %2d seconds.\r\n", daysDiff, hoursOnly, minsOnly, secsOnly);
  Serial.println(stringBuffer);

  drawDigit(days1, daysDiff / 100);
  drawDigit(days2, daysDiff % 100 / 10);
  drawDigit(days3, daysDiff % 100 % 10);

  drawDigit(hours1, hoursOnly / 10);
  drawDigit(hours2, hoursOnly % 10);

  drawDigit(mins1, minsOnly / 10);
  drawDigit(mins2, minsOnly % 10);

  drawDigit(secs1, secsOnly / 10);
  drawDigit(secs2, secsOnly % 10);

  fill_solid(ledsTitle, NUM_LEDS__LARGE, CRGB::Blue);

  // send the 'leds' array out to the actual LED strip
  FastLEDshowESP32();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_MILLISECONDS(2) {
    gHue++;
  }
}
