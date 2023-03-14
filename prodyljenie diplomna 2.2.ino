#include <virtuabotixRTC.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int SW_pin = 2;
const int X_pin = 0;
const int Y_pin = 1;
byte btnValue = 0;
bool programing = 0;

const byte pin[8] = { 6, 7, 8, 9, 10, 11, 12, 13 };
virtuabotixRTC myRTC(3, 4, 5);

byte startHour = 19;
byte startMinutes = 59;
byte days = 3;

byte nextWorkingMonth = 3;
byte nextWorkingDay = 13;
int input = 8;

byte values[]{ 1, 5, 13, 7, 9, 16, 5, 1, startHour, startMinutes, days, nextWorkingMonth, nextWorkingDay, input };
byte start_hour[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
byte start_minutes[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
byte end_hour[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
byte end_minutes[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
bool rele[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
bool worked[8]{ 0, 0, 0, 0, 0, 0, 0, 0 };
bool workingFlag = 0;
int j = 0;

bool fload = true;

int count = 0;
byte tempHour = 0;
byte tempMinutes = 0;

int cycle_cnt = 0;

void setup() {

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  pinMode(pin[0], OUTPUT);
  pinMode(pin[1], OUTPUT);
  pinMode(pin[2], OUTPUT);
  pinMode(pin[3], OUTPUT);

  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  Serial.begin(9600);


  myRTC.setDS1302Time(59, 58, 19, 6, 11, 3, 2023);
  myRTC.updateTime();

  if (nextWorkingMonth <= myRTC.month && nextWorkingDay < myRTC.dayofmonth) {
    calcNextCycle();
  }
}

void loop() {

  myRTC.updateTime();
  changeVals();
  // Serial.println();
  // Serial.println();

  if ((myRTC.month == nextWorkingMonth) && (myRTC.dayofmonth == nextWorkingDay) && (myRTC.hours >= startHour) && (myRTC.minutes >= startMinutes) || workingFlag == 1) {
    if (fload == true) {
      ///////////////////////////
      Serial.println("vleze v cikyla");
      cycle_cnt++;
      //////////////////////////
      fload = false;
      workingFlag = 1;
      tempHour = myRTC.hours;
      tempMinutes = myRTC.minutes;
      int minutes = 0;
      byte hours = 0;
      // Пресмятане и задаване стойност за край на всяко реле
      while (j < 8) {  // ДА СЕ ИЗМИСЛИ ПРОВЕРКА АКО МИНАВА В ДРУГО ДЕНОНОЩИЕ
        for (int l = 0; l < input; l++) {
          if (j < input) {

            minutes = tempMinutes + values[j];

            if (minutes < 60) {

              end_minutes[j] = minutes;
              end_hour[j] = tempHour;

            } else if (minutes >= 60 && minutes < 120) {

              end_minutes[j] = minutes - 60;
              hours = 1;
              end_hour[j] = tempHour + 1;

            } else if (minutes >= 120) {

              end_minutes[j] = minutes - 120;

              hours = 2;
              end_hour[j] = tempHour + 2;
            }

          } else {
            int tminutes = 0;
            byte thours = 0;
            tminutes = end_minutes[j - input] + values[j];

            if (tminutes < 60) {
              end_minutes[j] = tminutes;
              thours = hours;
            } else if (tminutes >= 60 && tminutes < 120) {
              end_minutes[j] = tminutes - 60;
              thours = 1 + hours;
            } else if (tminutes >= 120) {
              end_minutes[j] = tminutes - 120;
              thours = 2 + hours;
            }

            // NE PIPAI !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            Serial.println(end_minutes[j + 1]);  // MISTERIQTA ZASHTO KATO GO NQMA NE RABOTI
                                                 // NE PIPAI !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //delay(200);
            end_hour[j] = thours + tempHour;
          }
          j++;
        }
      }

      // Пресмятане и задаване стойност за старта на всяко реле
      for (int i = 0; i < 8; i++) {

        int minutes = 0;
        byte shours = 0;
        minutes = end_minutes[i] - values[i];

        if (minutes >= 0) {
          start_minutes[i] = minutes;
          start_hour[i] = end_hour[i];
        } else if (minutes < 0 && minutes > -60) {
          start_minutes[i] = minutes + 60;
          start_hour[i] = end_hour[i] - 1;
        } else if (minutes <= -120) {
          start_minutes[i] = minutes + 120;
          start_hour[i] = end_hour[i] - 2;
        }
      }
    }

    if (fload == false) {

      for (int i = 0; i < 8; i++) {

        if ((myRTC.hours >= start_hour[i] && myRTC.minutes >= start_minutes[i]) && worked[i] == 0) {

          // Serial.println("vlkliuchva rele");
          rele[i] = 1;
          digitalWrite(pin[i], rele[i]);
        }
        // Проверява дали таймера е достигнал стойността за край
        if (myRTC.hours >= end_hour[i] && myRTC.minutes >= end_minutes[i] && rele[i] == 1 && worked[i] == 0) {
          rele[i] = 0;
          worked[i] = 1;
          digitalWrite(pin[i], rele[i]);
          count++;
          Serial.println(count);
        }
      }

      // Зануляване на стройките и подготовка за следващ цикъл
      if (count == 8) {
        for (int l = 0; l < 8; l++) {
          start_hour[l] = 0;
          start_minutes[l] = 0;
          end_hour[l] = 0;
          end_minutes[l] = 0;
          rele[l] = 0;
          worked[l] = 0;
        }

        j = 0;
        count = 0;
        fload = true;
        calcNextCycle();
        workingFlag = 0;
      }
    }
  }
}
///////////////////////////////////////////////////
//            func
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// смяна на стойности
///////////////////////////////////////////////////
void changeVals() {

  if (digitalRead(SW_pin) == 1 && programing == 0) {  // btn not presed, da se dobavi flag
    homeScreen();
    delay(1000);
  }
  if (digitalRead(SW_pin) == 0 || programing == 1) {  // btn presed
    if (programing == 0) {
      programing = 1;
      lcd.setCursor(0, 0);
      lcd.print("Programing ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(1000);
    }
    if (analogRead(Y_pin) > 100) {
      btnValue++;
      delay(200);
    }
    if (analogRead(Y_pin) < 1000) {
      btnValue--;
      delay(200);
    }
    if (btnValue <= 1) {
      btnValue = 13;
    }
    if (btnValue >= 14) {
      btnValue = 2;
    }
    if (btnValue == 2) {
      changeZoneParams(btnValue);
    }
    if (btnValue == 3) {
      changeZoneParams(btnValue);
    }
    if (btnValue == 4) {
      changeZoneParams(btnValue);
    }
    if (btnValue == 5) {
      changeZoneParams(btnValue);
    }
    if (btnValue == 6) {
      changeZoneParams(btnValue);
    }
    if (btnValue == 7) {
      changeZoneParams(btnValue);
    }
    if (btnValue == 8) {
      changeZoneParams(btnValue);
    }
    if (btnValue == 9) {
      changeZoneParams(btnValue);
    }
    if (btnValue == 10) {
      lcd.setCursor(0, 0);
      lcd.print("Vreme Povtorenie");

      if (analogRead(X_pin) > 1000) {
        if (days > 1) {
          days -= 1;
        }
        //delay(200);
      }
      if (analogRead(X_pin) < 100) {
        if (days < 10) {
          days += 1;
        }
        //delay(200);
      }
      lcd.setCursor(0, 1);
      lcd.print("     dni - ");
      lcd.print(days);
      lcd.print("    ");
      delay(200);
    }
    if (btnValue == 11) {
      lcd.setCursor(0, 0);
      lcd.print("ednovremenno    ");
      if (analogRead(X_pin) > 1000) {

        if (input < 8) {
          input++;
        }
      }
      if (analogRead(X_pin) < 100) {
        if (input > 1) {
          input--;
        }
        //delay(200);
      }

      lcd.setCursor(0, 1);
      lcd.print("broi zoni - ");
      lcd.print(input);
      lcd.print("  ");
      delay(200);
    }
    if (btnValue == 12) {
      lcd.setCursor(0, 0);
      lcd.print("pres to EXIT    ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
    if (btnValue == 12 && digitalRead(SW_pin) == 0) {

      lcd.setCursor(0, 0);
      lcd.print("EXIT Programing");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(1000);
      programing = 0;
    }
    if (btnValue == 13) {
      lcd.setCursor(0, 0);
      lcd.print("pres to manual  ");
      lcd.setCursor(0, 1);
      lcd.print("START CYCLE     ");
    }
    if (btnValue == 13 && digitalRead(SW_pin) == 0) {

      lcd.setCursor(0, 0);
      lcd.print(" START WATERING ");
      lcd.setCursor(0, 1);
      lcd.print("      CYCLE     ");
      delay(1000);
      workingFlag = 1;
      programing = 0;
    }
  }
}

/////////////////////////////////////////////////
// HomeScreen
/////////////////////////////////////////////////
void homeScreen() {

  lcd.setCursor(0, 0);
  lcd.print("  Date / Time:  ");
  lcd.setCursor(0, 1);          //|
  lcd.print(myRTC.dayofmonth);  //|
  lcd.print("/");               //|
  lcd.print(myRTC.month);       //|
  lcd.print("/");               //|
  lcd.print(myRTC.dayofweek);   //|
  lcd.print("  ");              //|
  lcd.print(myRTC.hours);       //|
  lcd.print(":");               //|
  lcd.print(myRTC.minutes);     //|
  lcd.print(":");               //|
  lcd.println(myRTC.seconds);
  lcd.print("  ");
}

void calcNextCycle() {
  if ((myRTC.month == 1 || myRTC.month == 3 || myRTC.month == 5 || myRTC.month == 7 || myRTC.month == 8 || myRTC.month == 10 || myRTC.month == 12)
      && (myRTC.dayofmonth + days <= 31)) {
    nextWorkingMonth = myRTC.month;
    nextWorkingDay = myRTC.dayofmonth + days;
    Serial.println(" v if za - jan march may july august october december");
  } else if ((myRTC.month == 1 || myRTC.month == 3 || myRTC.month == 5 || myRTC.month == 7 || myRTC.month == 8 || myRTC.month == 10 || myRTC.month == 12)
             && (myRTC.dayofmonth + days > 31)) {
    nextWorkingMonth = myRTC.month + 1;
    nextWorkingDay = (myRTC.dayofmonth + days) - 31;

  } else if ((myRTC.month == 2) && (myRTC.dayofmonth + days) <= 28) {
    nextWorkingMonth = myRTC.month;
    nextWorkingDay = myRTC.dayofmonth + days;

  } else if ((myRTC.month == 2) && (myRTC.dayofmonth + days) > 28) {
    nextWorkingMonth = myRTC.month + 1;
    nextWorkingDay = (myRTC.dayofmonth + days) - 28;

  } else if ((myRTC.month == 4 || myRTC.month == 6 || myRTC.month == 9 || myRTC.month == 11)
             && (myRTC.dayofmonth + days <= 30)) {
    nextWorkingMonth = myRTC.month;
    nextWorkingDay = myRTC.dayofmonth + days;

  } else if ((myRTC.month == 4 || myRTC.month == 6 || myRTC.month == 9 || myRTC.month == 11)
             && (myRTC.dayofmonth + days > 30)) {
    nextWorkingMonth = myRTC.month + 1;
    nextWorkingDay = (myRTC.dayofmonth + days) - 30;
  }
}
void r() {
  Serial.print("\n");
  Serial.println("  END HOUR  ");
  for (int l = 0; l < 8; l++) {
    Serial.print(end_hour[l]);
    Serial.print(" ");
  }

  Serial.print("\n");
  Serial.println("  END MINUTES  ");
  for (int l = 0; l < 8; l++) {
    Serial.print(end_minutes[l]);
    Serial.print(" ");
  }
  Serial.print("\n");
  Serial.println("  START HOUR  ");
  for (int l = 0; l < 8; l++) {
    Serial.print(start_hour[l]);
    Serial.print(" ");
  }
  Serial.print("\n");
  Serial.println("  START MINUTES ");
  for (int l = 0; l < 8; l++) {
    Serial.print(start_minutes[l]);
    Serial.print(" ");
  }

  Serial.print("\n");
  Serial.println("  VALUES  ");
  for (int l = 0; l < 8; l++) {
    Serial.print(values[l]);
    Serial.print(" ");
  }

  Serial.print("\n");
  Serial.println("  RELAYS  ");
  for (int l = 0; l < 8; l++) {
    Serial.print(rele[l]);
    Serial.print(" ");
  }
}
void changeZoneParams(byte btnVal) {
  Serial.println(btnVal);
  lcd.setCursor(0, 0);
  lcd.print("ZONA ");
  lcd.print(btnVal - 1);
  lcd.print("          ");
  if (analogRead(X_pin) > 1000) {
    if (values[btnVal - 2] > 1) {
      values[btnVal - 2] -= 1;
    }
    // delay(200);
  }
  if (analogRead(X_pin) < 100) {
    if (values[btnVal - 2] < 120) {
      values[btnVal - 2] += 1;
    }
    //delay(200);
  }

  lcd.setCursor(0, 1);
  lcd.print("vreme rabota ");
  lcd.print(values[btnVal - 2]);
  Serial.println(values[btnVal - 2]);
  lcd.print("  ");
  delay(200);
}