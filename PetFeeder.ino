#include "lib/GyverStepper.h"
#include "lib/EncButton.h"


#include <EEPROM.h> // работа с памятью
#define EE_RESET 15
#include <Ds1302.h>
Ds1302 rtc(D8, D6, D7);


#define BTN_PIN D4
#define LED_PIN D5


GStepper<STEPPER4WIRE> stepper(2048, D0, D2, D1, D3);
EncButton<EB_TICK, BTN_PIN> btn;

int feedAmount = 14000;

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
        Serial.println("RTC is halted. Setting time...");

        Ds1302::DateTime dt = {
            .year = 24,
            .month = Ds1302::MONTH_JUN,
            .day = 2,
            .hour = 22,
            .minute = 5,
            .second = 10,
            .dow = Ds1302::DOW_SUN
        };

        rtc.setDateTime(&dt);
    }
  }
  EEPROM.get(1, feedAmount);

  stepper.setSpeed(10000);
  //stepper.setMaxSpeed(10000);
  //stepper.setAcceleration(10000);
  stepper.setRunMode(KEEP_SPEED);

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
      anti_jam();
      newAmount++;
      Serial.println(newAmount);
    }
    //Serial.println(newAmount);
    feedAmount = newAmount;
    EEPROM.put(1, feedAmount);
    EEPROM.commit();
    digitalWrite(LED_PIN, LOW);
  }
}

void feed() {
  // int tmp_fa = feedAmount;
  // while (tmp_fa>0){
  //   Serial.println(tmp_fa);
  //   anti_jam();
  //   tmp_fa--;
  // }
  
  digitalWrite(LED_PIN, HIGH);
  for (int i=0; i< feedAmount; i++){
    Serial.println(i);
    anti_jam();
  }
  digitalWrite(LED_PIN, LOW);
}

void anti_jam() {
  if (!stepper.tick()) {
    static bool dir;
    dir = !dir;
    stepper.setTarget(dir ? -7: 12, RELATIVE);
  }
}