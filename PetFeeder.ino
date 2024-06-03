#include "lib/GyverStepper.h"
#include "lib/EncButton.h"


#include <EEPROM.h> // работа с памятью
#define EE_RESET 1
#include <Ds1302.h>
Ds1302 rtc(27, 14, 12);


#define BTN_PIN 33
#define LED_PIN 32


GStepper<STEPPER4WIRE> stepper(2048, 4, 2, 0, 15);
EncButton<EB_TICK, BTN_PIN> btn;

int feedAmount = 1000;
bool dir = true;

const byte feedTime[][2] = {
  {7, 0},       // часы, минуты. НЕ НАЧИНАТЬ ЧИСЛО С НУЛЯ
  {12, 0},
  {17, 0},
  {22, 11},
};

void setup()
{
  Serial.begin(9600);

  rtc.init();

  EEPROM.begin(4096);
  if (EEPROM.read(0) != EE_RESET) {   // первый запуск
    EEPROM.write(0, EE_RESET);
    EEPROM.put(1, feedAmount);
    EEPROM.commit();
    if (rtc.isHalted())
    {
        //Serial.println("RTC is halted. Setting time...");

        Ds1302::DateTime dt = {
            .year = 24,
            .month = Ds1302::MONTH_JUN,
            .day = 3,
            .hour = 16,
            .minute = 43,
            .second = 0,
            .dow = Ds1302::DOW_MON
        };

        rtc.setDateTime(&dt);
    }
  }
  EEPROM.get(1, feedAmount);

  stepper.setSpeed(10000);
  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(10000);
  //stepper.setRunMode(KEEP_SPEED);

  pinMode(LED_PIN, OUTPUT);
}

void loop()
{
  static uint32_t tmr = 0;
  if (millis() - tmr > 500) {           // два раза в секунду
    static byte prevMin = 0;
    tmr = millis();
    //DateTime now = rtc.getTime();
    Ds1302::DateTime now;
    rtc.getDateTime(&now);
    if (prevMin != now.minute) {
      prevMin = now.minute;
      for (byte i = 0; i < sizeof(feedTime) / 2; i++)    // для всего расписания
        if (feedTime[i][0] == now.hour && feedTime[i][1] == now.minute){
          feed();
        } 
    }
  }

  btn.tick();

  if (btn.click()){
    feed();
  }

  if (btn.hold()) {               
    digitalWrite(LED_PIN, HIGH);
    int newAmount = 0;
    while (btn.isHold()) {
      btn.tick();
      delay(1);
      anti_jam();
      newAmount++;
      //Serial.println(newAmount);
    }
    Serial.println(feedAmount);
    feedAmount = newAmount;
    Serial.println(feedAmount);
    EEPROM.put(1, feedAmount);
    EEPROM.commit();
    digitalWrite(LED_PIN, LOW);
  }
}

void feed() {

  digitalWrite(LED_PIN, HIGH);
  int tmp_fa = feedAmount;
  while (tmp_fa>0){
    //Serial.println(tmp_fa);
    //btn.tick();
    delay(1);
    anti_jam();
    tmp_fa--;
  }
  digitalWrite(LED_PIN, LOW);

  
  // digitalWrite(LED_PIN, HIGH);
  // for (int i=0; i< feedAmount; i++){
  //   Serial.println(i);
  //   anti_jam();
  // }
  // digitalWrite(LED_PIN, LOW);
}

void anti_jam() {
  if (!stepper.tick()) {
    dir = !dir;
    //stepper.setSpeed(1500);//dir ? -0 : 1500);
    stepper.setTarget(dir ? -12 : 19, RELATIVE);
  }
}