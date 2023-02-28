#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>

RTC_DS1307 rtc;

LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int chipSelect = 4;

File workingTimesFile;

int zoneCount = 8;
bool zonesEnabled[8] = {true, true, true, true, true, true, true, true};
int wateringTimes[8] = {5, 5, 5, 5, 5, 5, 5, 5};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {}
  pinMode(2, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  if (!rtc.begin()) {
    Serial.println("RTC initialization failed!");
    while (1);
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();
  lcd.setCursor(0, 0);
  lcd.print(now.toString("YYYY/MM/DD"));
  lcd.setCursor(0, 1);
  lcd.print(now.toString("hh:mm:ss"));

  // TODO: Add code to read and process the joystick input
}

void writeWorkingTimes() {
  // TODO: Add code to write the working times to the SD card
}

void readWorkingTimes() {
  // TODO: Add code to read the working times from the SD card
}

void enableZone(int zone) {
  zonesEnabled[zone] = true;
}

void disableZone(int zone) {
  zonesEnabled[zone] = false;
}

void setWateringTime(int zone, int minutes) {
  wateringTimes[zone] = minutes;
}
