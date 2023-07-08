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

//  #include <LiquidCrystal_I2C.h>      // подключаем библу
//LiquidCrystal_I2C lcd(0x27, 16, 2); // адрес, столбцов, строк
// термистор на пине А1 - А3
// сопротивление резистора 10к
// тепловой коэффициент 3950
GyverNTC therm1(0, 10000, 3950);  // морозилка
//test
GyverNTC therm2(1, 10000, 3950);  //  холодилка
GyverNTC therm3(2, 10000, 3950);  //  испарилка (испарение)
//unsigned int val = 0;
boolean isEmagnUp = true;
boolean comp = false;
boolean kn = false;
unsigned long timing, timing1;
int t1 = 0;
int t2 = 0;
int t3 = 0;
int knPin = 2;
int prPin = 4;
int comprPin = 8;  // Компрессор
int fanPin = 6;    // Вентилятор
int inPin = 5;     //
int ledPin = 7;    //  Эл. магнит
int DoorOld = 0;
int DoorNew = 0;
float tmor;
float thol;
float tisp;
//unsigned long timing; // Переменная для хранения точки отсчета
static uint8_t tog = 0;
static uint32_t oldtime = millis();
float thermProv () {
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
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

  /* lcd.home();  
  lcd.print(therm1.getTempAverage());
  lcd.print(',');
  lcd.println(therm2.getTempAverage());
  lcd.print(',');
  lcd.println(therm3.getTempAverage());*/

  delay(500);

  tmor = therm1.getTempAverage();
  thol = therm2.getTempAverage();
  tisp = therm3.getTempAverage();

  // Вывод на экран монитора
  Serial.print("Mor: ");
  Serial.print(tmor);
  Serial.print(" Hol: ");
  Serial.print(thol);
  Serial.print(" Isp: ");
  Serial.print(tisp);
  Serial.println(";");
  //Serial.println(digitalRead(5));

  if (digitalRead(comprPin) == LOW) {
    comp = true;
    Serial.println(" comp == true ");
  }  //Проверка работы компрессора

 /* if (comp = true && tmor > -10) {
    Serial.println(" comprPin VKL ");
    digitalWrite(comprPin, HIGH);  //Включить компрессор
  }*/
  ProvTisp();  // начать проверку температуры испарителя

  //ждать 5-8 минут
  //раз в 30 секунд проверять температуру
  //при понижении температуры Эл.магнит не включается
  //при повышении Эл. магнит включится
   /* if (tmor > -12 && comprPin == HIGH) {
    Serial.println(" comprPin == HIGH ");
  }

             digitalWrite(ledPin, LOW);
     */

  //------------------------------------------------------

  // }
  //if (tmor > - 12) { digitalWrite (fanPin, HIGH);}
  //-------------------------Здесь Миллис 5 минут----------------------------//
  delay(1000);
  if (tmor > -15) {
    Serial.println(" comprPin == HIGH FAN");  //comprPin == HIGH &&&& fanPin == LOW
    if ((millis() - oldtime) > 300000) {
      oldtime = millis();
      tog = ~tog;  //invert
      if (tog) digitalWrite(fanPin, HIGH);
      else digitalWrite(fanPin, LOW);
    }  //else fanPin = LOW;
  }
  if (tmor < -18) {
    digitalWrite(fanPin, LOW);
  }
  //-----------------Включение----------------------
  if (thol < 3 && tisp < -24) {  // if (isEmagnUp == false){eMagn();}
    //digitalWrite(ledPin, HIGH);
    //digitalWrite(ledPin, HIGH); 
    klapan();
    pauseKl();
/* 
 if (isEmagnUp == false) { while (isEmagnUp == true){ klapan();
    pauseKl();}
 }
*/
   // klapan();
  //  pauseKl();
   if (isEmagnUp == true && tisp < - 10) {Serial.println ("vkluchit' .............."/*продолжу завтра...*/".."); int t4 = 0;
    digitalWrite(fanPin, LOW);
delay(5000);
    while (tmor > -20) {
        digitalWrite(fanPin, HIGH);

        delay(1500);
        Serial.println(" VKL -24 Cikl ");

        delay(500);
        therm3.getTempAverage();
        tisp = therm3.getTempAverage();
        if (tisp > 0) {
          Serial.println(" VKL -24 Cikl END Dosrochno !!! ");
          digitalWrite(fanPin, LOW);
          break; 
          }
      }
       digitalWrite(fanPin, LOW);
        Serial.println(" VKL -24 Cikl END ");
        Serial.println(tmor);
        digitalWrite(comprPin, LOW);   
        Serial.println(" Compressor OTKL. !!! ");

        delay(900000);
      //Serial.println(" Prodoljilos bez otkluchenija !!! ");
   } 
    //----------------------------Проверка?---------------------------------------//
    // digitalRead(8);// boolean isr(kn);
    // if( digitalRead(8) == HIGH) {Serial.println(" FLAG VVERH "); } //
    // if(kn == true) {}
    // else { return; }
    //------------------------------------------------------
    // delay(15);
    // digitalWrite(ledPin, LOW);
    /* if(t1 == 1){
             Serial.println("Srabotalo - "); 
             delay(480000);
             }else { Serial.println("Ne srabotalo - ");delay (5000); return;}*/
    // delay(480000);
    // if(thol > 2 && tisp >= -20 && comprPin == HIGH) {
    // if(tmor > -22) { digitalWrite(fanPin, HIGH);
    //}
    // if(tmor < -24) { digitalWrite(fanPin,LOW);comp = false;
    // }
   // pauseKl();
    //delay(480000);
  }

  //}
  //if(thol < 5 && tisp > 2 ){}
  if (thol > 6 && tisp > 8) {
    klapan();
    Serial.print (" isEmagnUp 'thol > 6' ==  "); 
    Serial.println (isEmagnUp );
    ProvTisp(); 
    if (isEmagnUp == false )  { digitalWrite(comprPin, HIGH);
Serial.print (" comprPin, HIGH  ");     
    isr();
    if (t1 == 1) { Serial.println(" FLAG VNIZ "); }
    /*  if(t1 == 1){
             Serial.println("Srabotalo + "); 
             delay(480000);
             }else { Serial.println("Ne srabotalo + ");delay (5000); return;}*/
    // delay(480000);
    Serial.println("Srabotalo ++");
    } else klapan();
    //delay(480000);
  }
  //if(comp == comp) { digitalWrite(comprPin, LOW);
  // delay(900000);
  //}
  /*void test (float thol) {
    if (t1 <= 5) { // -1
    if (t2 <= -1) {
      isEmagnUp = false;
    }
    refrigirator();
  } else {
    isEmagnUp = true;
    comp = true;
  }*/
  delay(100);
}
//------------------------Проверка испарителя длинная-----------------//
void ProvTisp() {
  t2 = tisp;
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
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
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно ----------------
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
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
  //float toStrl (tmor, thol, tisp){
}
//------------------------------------------------------//
void klapan() {
  digitalWrite(ledPin, HIGH);
  delay(25);
  digitalWrite(ledPin, LOW);  //digitalWrite(7, LOW);
}
//--------------------------------------------------------------
void pauseKl() {
  Serial.println("PauseKl Start ");
  therm3.getTempAverage();
  tisp = therm3.getTempAverage(); 
  t2=tisp;
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println("PauseKl END ");
  Serial.println(tisp);
  if (t2< tisp) {isEmagnUp = true;
  } else isEmagnUp = false;
  delay(100);
}
//--------------------------------------------------------------------------//
/*void eMagn() {
   isEmagnUp = isEmagnUp ;
    delay (1000);
    klapan();
  Serial.println("eMagn Start ");
  therm3.getTempAverage();
  tisp = therm3.getTempAverage(); 
  t2=tisp;
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println(tisp);
  delay(30000);  // подрегулировать нужно -------------------!
  therm3.getTempAverage();
  tisp = therm3.getTempAverage();
  Serial.println("eMagn END ");
  Serial.println(tisp);
  if (t2< tisp) {isEmagnUp = true;
  } else isEmagnUp = false;
  delay(100);
}           
             delay (180000); // подрегулировать нужно -------------------!
             therm3.getTempAverage();tisp = therm3.getTempAverage();
             delay (100);
             if (t2 < tisp) {
              Serial.print(" elMagnUp 1 "); Serial.println(tisp); 
             }
             t3=tisp;
             delay (180000); // подрегулировать нужно --------------------!
             therm3.getTempAverage();tisp = therm3.getTempAverage();
             delay (100);
             if (t3 && t2 < tisp) { 
              Serial.print(" OH YES ! MagnUP "); Serial.println(tisp); t3=tisp; isEmagnUp = !isEmagnUp;
             } else return 241;

 }*/
//-----------------Включение Магнитного клапана -------------------------//
void vklElKl () {
  thermProv ();
  float ispt = tisp;
  thermProv ();
  delay (30000);
  if (ispt > tisp && tmor > 6 )  {
  klapan();
  }
  
  
}

/*     void compressor (vkl)(
//включение компрессора

//проверка температуры на уменьшение/увеличение

}*/
/*     void elmagnit (elmag)(
//включение компрессора
//проверка температуры на уменьшение/увеличение
 }
/*void refrigirator() {
    if (t2 >= -6) {
      isEmagnUp = false;
      comp = true;
    } else if (t2 <= -6 && t2 >= -12){
      isEmagnUp = false;
      comp = true;
    } else {
      isEmagnUp = true;
      comp = false;
    }*/

//}
