#include "IRremote.h"
//https://robotclass.ru/tutorials/arduino-ir-remote-control/
IRrecv irrecv(2);  // указываем вывод, к которому подключен приемник

decode_results results;
const int Relay = 6;
int stop = 0;
int RelayState = LOW;             // этой переменной устанавливаем состояние светодиода
long previousMillis = 0;        // храним время последнего переключения светодиода
boolean milState = false;
long interval = 1000;           // интервал между включение/выключением светодиода (1 секунда)

void setup() {
  pinMode(Relay, OUTPUT);
  irrecv.enableIRIn();  // запускаем прием
}
void loop() {
  if (irrecv.decode(&results)) {  // если данные пришли
    switch (results.value) {
      case 0x100C:
        digitalWrite(Relay, HIGH);
        break;
      case 0x180C:
        digitalWrite(Relay, LOW);
        break;
case 0x1810  :
stop = !stop;
Blink();
break;
case 0x1010:
stop = stop;
Blink();
break;
case 0x1811:
pauseMil();
break;
case 0x1011:
pauseMil();
break;
}
irrecv.resume(); // принимаем следующую команду
}
}
void Blink() {
  if (digitalRead (Relay== HIGH)) {
    digitalWrite (Relay, LOW);
   // delay (5000);
  }
  while (int n = stop){
 digitalWrite( Relay, HIGH );
delay(3000);


if (milState == !milState) {
pauseMil();
}
if (stop != stop) {n==stop;
 break;
} 
digitalWrite( Relay, LOW );
delay(3000); 

 }
}
void pauseMil() {
 unsigned long currentMillis = millis();
  
  //проверяем не прошел ли нужный интервал, если прошел то
  if(currentMillis - previousMillis > interval) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis; 
 
    // если светодиод не горит, то зажигаем, и наоборот
    if (RelayState == LOW)
      RelayState = HIGH;
    else
      RelayState = LOW;
 
    // устанавливаем состояния выхода, чтобы включить или выключить светодиод
    digitalWrite(Relay, RelayState);
    milState = milState;
      return 66;
  }

}
