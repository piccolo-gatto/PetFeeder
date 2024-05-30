#include "lib/GyverStepper.h"
#include "lib/EncButton.h"
#include <EEPROM.h>
#include <math.h>

#define EE_RESET 12
#define BTN_PIN 4
#define LED_PIN 7


GStepper<STEPPER4WIRE> stepper(1024, 10, 12, 11, 13);
EncButton<EB_TICK, BTN_PIN> btn;

int feedAmount = 1000;


void setup()
{


  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  //stepper.setSpeed(5000);
  //stepper.setMaxSpeed(5000);
  //stepper.setAcceleration(5000);

}

void loop()
{

  btn.tick();

  if (btn.click()){
    digitalWrite(LED_PIN, HIGH);
    feed();
    digitalWrite(LED_PIN, LOW);
  }

  if (btn.hold()) {                // NEED FIX
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
    //EEPROM.put(1, feedAmount);
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
  for (int i=0; i< feedAmount; i++){
    Serial.println(i);
    anti_jam();
  }
}

void anti_jam() {
  if (!stepper.tick()) {
    static bool dir;
    dir = !dir;
    stepper.setTarget(dir ? -12: 19, RELATIVE);
  }
}