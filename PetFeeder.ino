#include "lib/GyverStepper.h"
#include "lib/EncButton.h"
#include <EEPROM.h>

#define EE_RESET 12
#define BTN_PIN 4
#define LED_PIN 7	


GStepper<STEPPER4WIRE> stepper(100, 10, 12, 11, 13);
EncButton<EB_TICK, BTN_PIN> btn;

int feedAmount = 1000;


void setup()
{


  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  stepper.setSpeed(10000);
  stepper.setMaxSpeed(10000);
  stepper.setAcceleration(10000);

}

void loop()
{

  btn.tick();

  if (btn.click()){
    digitalWrite(LED_PIN, HIGH);
    feed();
    digitalWrite(LED_PIN, LOW);
  }

  // if (btn.hold()) {                // NEED FIX
  //   digitalWrite(LED_PIN, HIGH);
  //   int newAmount = 0;
  //   while (btn.isHold()) {
  //     btn.tick();
  //     anti_jam();
  //     newAmount++;
  //   }
  //   feedAmount = newAmount;
  //   EEPROM.put(1, feedAmount);
  //   digitalWrite(LED_PIN, LOW);
  // }
}

void feed() {
  for (int i=0; i< feedAmount; i++){
    Serial.println(i);
    anti_jam();
  }
}

void anti_jam() {
  if (!stepper.tick()) {
    static bool dir;
    dir = !dir;
    stepper.setTarget(dir ? -1: 19, RELATIVE);
  }
}
