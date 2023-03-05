const int SW_pin = 2;
const int X_pin = 0;
const int Y_pin = 1;
//const int relay = 4;
const byte pin[4] = {3,4,5,6};

  unsigned long values[4]{ 2000, 5000, 2000, 8000 };
  unsigned long start[4]{ 0, 0, 0, 0 };
  unsigned long end_t[4]{ 0, 0, 0, 0 };
  bool rele[4]{ 0, 0, 0, 0 };
  int j = 0;
  
  bool fload = true;
  unsigned long scikal = 40000;
  unsigned long fcikal = scikal;
  int count = 0;
  unsigned long temp = 0;

  int input = 2;
  unsigned long timer = 1000;
  int cycle_cnt = 0;
  
void setup() {
  pinMode(pin[0] , OUTPUT);
  pinMode(pin[1] , OUTPUT);
  pinMode(pin[2] , OUTPUT);
  pinMode(pin[3] , OUTPUT);

  Serial.begin(9600);
}

void loop() {
timer = millis();
////////////////////////////////////////////////////////////
//        Serial.print(timer);
//        Serial.println("  Taimer  ");
//Serial.println(cycle_cnt);
////////////////////////////////////////////////////////////
    if (timer > scikal)
    {
      if (fload == true)
      {
///////////////////////////
cycle_cnt++;
//////////////////////////        
        fload = false;
        temp = scikal;
// Пресмятане и задаване стойност за край на всяко реле
        while (j < 4) {
          for (int l = 0; l < input; l++)
          {
            if (j < input) {
              end_t[j] = scikal + values[j];
            }
            else {
              end_t[j] = end_t[j - input] + values[j];
            }
            j++;
          }
        }
// Пресмятане и задаване стойност за старта на всяко реле
        for (int i = 0; i < 4; i++) {
          start[i] = end_t[i] - values[i];
        }
      }

      if (fload == false)
      {
        for (int i = 0; i < 4; i++) {
// Проверява таймера дали е в диапазона старт/край
          if (timer >= start[i] && timer < end_t[i])
          {
            rele[i] = 1;
            digitalWrite(pin[i] , rele[i]);
          }
// Проверява дали таймера е достигнал стойността за край
          if (timer > end_t[i] && rele[i] == 1) {
            rele[i] = 0;
            digitalWrite(pin[i] , rele[i]);
            count++;
          }
        }
///////////////////////////////////////////////////////////////////////
        Serial.print("\n");
        Serial.println("  СТАРТ  ");
        for (int l = 0; l < 4; l++)
        {
          Serial.print(start[l]);
          Serial.print(" ");
        }

        Serial.print("\n");
        Serial.println("  КРАЙ  ");
        for (int l = 0; l < 4; l++)
        {
          Serial.print(end_t[l]);
          Serial.print(" ");
        }

        Serial.print("\n");
        Serial.println("  VALUES  ");
        for (int l = 0; l < 4; l++)
        {
          Serial.print(values[l]);
          Serial.print(" ");
        }

        Serial.print("\n");
        Serial.println("  РЕЛЕТА  ");
        for (int l = 0; l < 4; l++)
        {
          Serial.print(rele[l]);
          Serial.print(" ");
        }

/////////////////////////////////////////////////////////////////////// 
// Зануляване на стройките и подготовка за следващ цикъл
        if (count == 4)
        {
          for (int l = 0; l < 4; l++)
          {
            start[l] = 0;
            end_t[l] = 0;
            rele[l] = 0;
          }
          
          j = 0;
          count = 0;
          fload = true;
          scikal = temp + fcikal;
        }

      }

    }

}
