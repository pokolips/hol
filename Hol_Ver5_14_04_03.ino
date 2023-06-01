// пример выводит температуру в COM порт
#define rts1
#define rt0
#include <Wire.h>
#include <GyverNTC.h>
// #include <iarduino_DHT.h> // Библиотека для работы с датчиком DHT11/DHT22
//iarduino_DHT sensor(3); // Датчик подключен к 3 пину Arduino

#include <SPI.h>;       // Библиотека для работы с шиной SPI
#include <nRF24L01.h>;  // Файл конфигурации для библиотеки RF24
#include <RF24.h>;      // Библиотека для работы с модулем NRF24L01
//_____________________________Заменить____________________________
// термистор на пине А1 - А3
// сопротивление резистора 10к
// тепловой коэффициент 3950
GyverNTC therm1(0, 10000, 3950);  // морозилка
GyverNTC therm2(1, 10000, 3950);  //  холодилка
GyverNTC therm3(2, 10000, 3950);  //  испарилка (испарение)
//.._____________________________________________________________________..//
//unsigned int val = 0;
boolean isEmagnUp = true;
boolean isCheckingPass = true;  //Проверка /Старые/00000 ?????
boolean comp = false;
boolean kn = false;
boolean isFan = false;
unsigned long timing, timing1;
int t1 = 0;
int t2 = 0;
int t3 = 0;
int knPin = 2;
int prPin = 4;
int comprPin = 5;  // Компрессор
int fanPin = 6;    // Вентилятор
int inPin = 8;     //
int ledPin = 7;    //  Эл. магнит
int DoorOld = 0;
int DoorNew = 0;
float tmor;
float thol;
float tisp;
float tMin = -12;
float tIspMin = -23;
float tIspMax = 8;
float tMorMin = -15;
float tMorMax = -12;
float tHolMin = 3;
float tHolMax = 6;
//unsigned long timing; // Переменная для хранения точки отсчета
static uint8_t tog = 0;
static uint32_t oldtime = millis();

float thermProv() {
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
}
float KlapHol() {
  digitalWrite(ledPin, HIGH);
  delay(1);
  digitalWrite(ledPin, LOW);
}

float morthermProv() {
  therm1.getTempAverage();
  tmor = therm1.getTempAverage();
}
float holthermProv() {
  therm2.getTempAverage();
  thol = therm2.getTempAverage();
}
void goProvMor() {  //?????????
  Serial.println("goProvMor Pusk");
  morthermProv();
  int rt = tmor;
  delay(30000);
  for (int v = 0; v = 7; v++) {
    Serial.print("for (int v = 0; v = 7; v++) ");
    Serial.println(tmor);
    while (tmor < rt) {
      KlapHol();
      Serial.println("goProvMor ");
      morthermProv();
      Serial.print("goProvMor = ");
      Serial.println(tmor);
      //int rt = tmor;
      delay(30000);
      if (tmor > rt + 2) {
        Serial.print("goProvMor break = ");
        Serial.println(tmor);
        break;
      }
      break;
    }
  }
}
void ventFanGo() {  //--Сделано 12.04.23
  Serial.print("ventFanGo()  ");
  digitalRead(fanPin);
  Serial.println(isFan);
  delay(100);
  if (digitalRead(fanPin) == HIGH) {
    Serial.println("ventFanGo(fanPin == HIGH)");
    if (isFan == 1) {}
    if (isFan == 0) { digitalWrite(fanPin, LOW); }
  }
  if (digitalRead(fanPin) == LOW) {
    Serial.println("ventFanGo(fanPin == LOW)");
    if (isFan == 1) { digitalWrite(fanPin, HIGH); }
    if (isFan == 0) {}
  }
}
void setup() {
  // lcd.init();           // инициализация
  // lcd.backlight();      // включить подсветку
  Serial.begin(9600);
  //pinMode( 4 , OUTPUT);
  pinMode(prPin, OUTPUT);     // 10
  pinMode(comprPin, OUTPUT);  // Kompressor
                              //  pinMode(comprPin, LOW);
  pinMode(fanPin, OUTPUT);    // 6 FEN
                              // digitalWrite(fanPin, LOW);
  pinMode(ledPin, OUTPUT);    // ElMagn
  //pinMode(knPin, INPUT);
  pinMode(inPin, INPUT);
  digitalWrite(knPin, LOW);
  digitalWrite(inPin, HIGH);
  delay(10000);
  // прерывание на D2 (UNO/NANO)
  attachInterrupt(1, agp, RISING);
  attachInterrupt(0, isr, FALLING);
  delay(90000);
}
volatile uint32_t debounce;
void isr() {
  // оставим 100 мс таймаут на гашение дребезга
  // CHANGE не предоставляет состояние пина,
  // придётся узнать его при помощи digitalRead
  if (millis() - debounce >= 1 && digitalRead(2)) {
    debounce = millis();
    if (knPin == HIGH) {
      kn = true;
    } else {
      kn = false;
    }
    // ваш код по прерыванию по высокому сигналу
  }
}
volatile uint32_t deboce;
int agp() {
  // оставим 100 мс таймаут на гашение дребезга
  // CHANGE не предоставляет состояние пина,
  // придётся узнать его при помощи digitalRead
  if (millis() - deboce >= 1 && digitalRead(3)) {
    debounce = millis();
    if (knPin == HIGH) {
      t1 = !t1;
    } else {
      t1 = 0;
    }
    // ваш код по прерыванию по высокому сигналу
  }
}
void loop() {

  delay(100);
  //_______________Заменить_____________________________
  tmor = therm1.getTempAverage();
  thol = therm2.getTempAverage();
  tisp = therm3.getTempAverage();
  // ___________________________________________________,
  // Вывод на экран монитора
  Serial.print("Mor: ");
  Serial.print(tmor);
  Serial.print(" Hol: ");
  Serial.print(thol);
  Serial.print(" Isp: ");
  Serial.print(tisp);
  Serial.println(";");
  // ______________________________________________________
  Serial.println(" Serial.println Ver5.14.04.23;");
  delay(100);
  if (tmor > tMorMax) {
    isFan = true;
    ventFanGo();
    digitalRead(fanPin);
    if (fanPin == HIGH) {
      Serial.println("6, HIGH;");
    } else Serial.println("6, LOW;");
  }

  if (digitalRead(comprPin) == LOW) {
    comp = true;
    Serial.println(" comp == true ");
  }  //Проверка работы компрессора

  tmorMinFen();
  if (!isCheckingPass) {
    ProvTisp();  // начать проверку температуры испарителя
  }
  //-------------------------Здесь Миллис 5 минут----------------------------//
  delay(1000);
  /* if (tmor > -15) {
    Serial.println(" comprPin == HIGH FAN");  //comprPin == HIGH &&&& fanPin == LOW
    if ((millis() - oldtime) > 300000) {
      oldtime = millis();
      tog = ~tog;  //invert
      if (tog) digitalWrite(fanPin, HIGH);
      else digitalWrite(fanPin, LOW);
    }  //else fanPin = LOW;
  }*/
  if (tmor < tMorMin) {
    (isFan = false);
    ventFanGo();
    digitalRead(fanPin);
    if (digitalRead(fanPin == HIGH)) {
      Serial.println("6, LOW; -15");
    } else {
      Serial.println("6, HIGH; -15");
      digitalWrite(6, LOW);
    }
  }
  //**********************Отключение Холодильника*************************//
  if (thol < tHolMin && tisp < tIspMin) {  // if (isEmagnUp == false){eMagn();}
                                           // goProvMor();
                                           // KlapHol();
    klapan();
    pauseKl();

    if (isEmagnUp == false) {
      while (isEmagnUp == true) {
        klapan();
        pauseKl();
      }
    }
    if (isEmagnUp == true && tisp < 0) {
      isFan = true;
      Serial.println("vkluchit' .............." /*продолжу завтра...*/ "..");
      int t4 = 0;
      // digitalWrite(6, LOW);
      delay(10000);
      while (tmor > -18) {
        digitalRead(fanPin);
        ventFanGo();
        delay(60000);
        Serial.println(" VKL -24 Cikl ");
        delay(500);
        morthermProv();
        thermProv();
        if (tisp > -10 && digitalRead(comprPin == HIGH)) {
          Serial.println(" Compressor Otkl tisp = -10 ");
          digitalWrite(comprPin, LOW);
        }
        if (tisp > 3) {
          Serial.println(" VKL -24 Cikl END Dosrochno !!! ");
          digitalWrite(fanPin, LOW);
          break;
        }
      }
      Serial.println(" Prodoljilos bez otkluchenija !!! ");
      isFan = false;
      digitalRead(fanPin);
      ventFanGo();
      delay(10000);
      Serial.println(" Otkluchenie ");
      Serial.println(tmor);
      if (digitalRead(comprPin == LOW)) {
        Serial.println(" Compressor OTKL. !!! ");
      } else digitalWrite(comprPin, LOW);
      Serial.println(" Compressor OTKL. !!! ");

      delay(600000);
    }
  }

  //*******************Включение Холодильника*******************//
  if (thol > tHolMax && tisp > tIspMax) {
    if (comp == true) {
      Serial.print(comp);
      Serial.println(" = comp  ");
      delay(100);
      digitalWrite(comprPin, HIGH);
      Serial.print(" comprPin, HIGH  ");
    }
    delay(5000);
    klapan();
    Serial.print(" isEmagnUp 'thol > 6' ==  ");
    Serial.println(isEmagnUp);
    ProvTisp();
    if (isEmagnUp == true) {
      while (isEmagnUp == false) {
        klapan();
        pauseKl();
      }
    }
    if (isEmagnUp == false) {

      isr();
      if (t1 == 1) { Serial.println(" FLAG VNIZ "); }
      Serial.println("Srabotalo ++");
    } else klapan();
  }
  delay(100);
}
//------------------------Проверка испарителя длинная-----------------//
/*for(i=0,i<=5,i++ ) {
thermProv();
  Serial.println(tisp);
  delay(30000);}*/
void ProvTisp() {
  thermProv();
  t2 = tisp;
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.print("PrvTisp END ");
  Serial.println(tisp);
  delay(100);
  if (t2 < tisp) {
    Serial.print("t2 tisp < ");
    Serial.println(tisp);
  } else Serial.println("t2 tisp !< ");
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  delay(100);
  t3 = tisp;
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно ----------------
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  //-----------------добавить проверку вентилятора?------------------------!
  if (tmor > -10 && tisp < -25 && fanPin == HIGH) {
    klapan();
    delay(180000);
    klapan();
  }
  Serial.print("PrvTisp 2 END ");
  Serial.println(tisp);
  delay(100);
  Serial.print("PrvTisp 2 ");
  Serial.println(tisp);
  // delay (180000); // подрегулировать нужно --------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  if (t3 < tisp) {
    Serial.print("t3 tisp < ");
    Serial.println(tisp);
  } else Serial.println("t3 tisp !< ");
  delay(100);
  if (t3 && t2 < tisp) {
    Serial.print(" OH YES ! MagnUP true - Tepleet ! ");
    Serial.println(tisp);
    t3 = tisp;
    isEmagnUp = true;
    Serial.println("t3 & t2 tisp < ");
  } else {
    Serial.print(" OH NO ! MagnDown false - Holodaet ! ");
    t2 = tisp;
    t3 = tisp;
    isEmagnUp = false;
    Serial.println("t3 & t2 tisp !< ");
  }
  isCheckingPass = true;
  //float toStrl (tmor, thol, tisp){
}
//------------------------------------------------------//
void klapan() {
  digitalWrite(ledPin, HIGH);
  delay(1);
  digitalWrite(ledPin, LOW);  //digitalWrite(7, LOW);
}
//--------------------------------------------------------------
void pauseKl() {  //проверено 12.03.23
  Serial.println("PauseKl Start ");
  thermProv();
  t2 = tisp;
  delay(30000); 
// ------Включить цикл for ---------------------------------!
for(int i=0; i <=5; i++ ) {
thermProv();
  Serial.println(tisp);
  delay(30000);
  }
 /* therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000); 
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();*/
  Serial.println("PauseKl END ");
  Serial.println(tisp);
  if (t2 < tisp) {
    isEmagnUp = true;
  } else isEmagnUp = false;
  delay(100);
}
//-----------------Включение Магнитного клапана -------------------------//

void vklElKl() {  //-- Нужно пересмотреть (12.04.23)
  thermProv();
  float ispt = tisp;
  //thermProv();
  delay(30000);
  thermProv();
  if (ispt > tisp) {
    if (ispt > tisp && thol > 6) {
      klapan();
    }
    return vklElKl();
  }
  if (ispt < tisp) {
    klapan();
  }
}
void tmorMinFen() {
  float tMin = tMin + 2;
  // сюда вставить проверку  морозилки (12.04.23) Выполнено ниже
  morthermProv();
  if (tmor < -5) {
    if (comprPin == HIGH) {
      // если  isEmagnUp == false тогда isEmagnUp == false
      float tm = tisp;
      thermProv();
      delay(30000);
      if (tm < tisp) {
        while (tisp < tMin) {
          //  проверка испарителя
          Serial.println("tmorMinFen() while");
          thermProv();
          delay(10000);
        }
        digitalWrite(fanPin, HIGH);
      } else {
        tMin = tMin;
        return tmorMinFen();
      }
    }
    if (comprPin == LOW) {
      digitalWrite(comprPin, HIGH);
      delay(50);
      ProvTisp();

      tMin = tMin;
      return tmorMinFen();
    }
    digitalWrite(fanPin, LOW);
  }
}
void cikllMagn() { //продумать 14.04.23
  Serial.println(" cikllMagn ");
  float tMin = tMin + 4;
  // проверку температуры морозилки сюда
  morthermProv();
  while (tmor > tMin) {
    digitalWrite(fanPin, HIGH);
    delay(1500);
    therm1.getTempAverage();
    tmor = therm1.getTempAverage();
    delay(500);
  }
  digitalWrite(fanPin, LOW);
}
//проверка температуры морозилки на файле ....DS18
