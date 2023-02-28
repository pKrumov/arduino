#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>

#define JOYSTICK_X A0
#define JOYSTICK_Y A1
#define JOYSTICK_SW 2

#define ZONE_1 3
#define ZONE_2 4
#define ZONE_3 5
#define ZONE_4 6
#define ZONE_5 7
#define ZONE_6 8
#define ZONE_7 9
#define ZONE_8 10

#define BUTTON_PIN 11

#define WORKING_TIMES_FILE "working_times.txt"

const int NUM_ZONES = 8;
const int PIN_ZONES[] = {ZONE_1, ZONE_2, ZONE_3, ZONE_4, ZONE_5, ZONE_6, ZONE_7, ZONE_8};

RTC_DS1307 rtc;
File working_times;

LiquidCrystal_I2C lcd(0x27, 16, 2);

int selected_zones = 1;
int working_times_arr[NUM_ZONES] = {0};

void setup() {
  // initialize LCD
  lcd.init();
  lcd.backlight();

  // initialize RTC
  Wire.begin();
  rtc.begin();
  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // initialize SD card
  if (!SD.begin(4)) {
    lcd.print("SD card failed");
    return;
  }

  // load working times from file
  load_working_times();

  // initialize zones as output pins
  for (int i = 0; i < NUM_ZONES; i++) {
    pinMode(PIN_ZONES[i], OUTPUT);
  }

  // initialize joystick and button pins
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(JOYSTICK_SW, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lcd.clear();
 
