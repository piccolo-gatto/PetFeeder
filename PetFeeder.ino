#include <ezButton.h>

ezButton button(4);


#include <EEPROM.h>

#include <CustomStepper.h>            // Подключаем библиотеку CustomStepper
CustomStepper stepper(13, 12, 11, 10);  // Указываем пины, к которым подключен драйвер шагового двигателя
//#define STEPS_FRW 2        // шаги вперёд
//#define STEPS_BKW 1       // шаги назад

#define BTN_PIN 4           // кнопка
#define LED_PIN 7
#include "EncButton.h"
EncButton<EB_TICK, BTN_PIN> btn;

int feedAmount = 100;



boolean rotate_dir = false;


void setup()
{


  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  stepper.setRPM(17);                 // Устанавливаем количество оборотов в минуту
  stepper.setSPR(4000);       // Устанавливаем колочество шагов на полный оборот. Максимальное значение 4075.7728395
}

void loop()
{
  button.loop();


  //btn.tick();
  // if (stepper.isDone() &&  btn.click())  
  // {
  //   stepper.setDirection(CCW);        // Устанавливает направление вращения (по часовой)
  //   stepper.rotate(2);                // Устанавливает вращение на заданное количество оборотов
  //   //rotate1 = true;
  // }
  // if (stepper.isDone() && rotate1 == true && rotatedeg == false)
  // {
  //   stepper.setDirection(CW);         // Устанавливает направление вращения (против часовой)
  //   stepper.rotateDegrees(90);        // Поворачивает вал на заданное кол-во градусов
  //   rotatedeg = true;
  // }

  // if (btn.click()){
  //   digitalWrite(LED_PIN, HIGH);
  //   feed();
  //   digitalWrite(LED_PIN, LOW);
  // }    

  // if (btn.hold()) {
  //   digitalWrite(LED_PIN, HIGH);
  //   int newAmount = 0;
  //   while (btn.isHold()) {
  //     btn.tick();
  //     anti_jam();
  //     newAmount++;
  //   }
  //   stepper.setDirection(STOP);
  //   feedAmount = newAmount;
  //   EEPROM.put(1, feedAmount);
  //   digitalWrite(LED_PIN, LOW);
  // }

  if(button.isPressed()){
    //Serial.println("The button is pressed");
    feed();    
  }       
  //anti_jam();    
}

void feed() {
  for (int i=0; i< 5000; i++){
    anti_jam();
  }
}

void anti_jam() {
  if (stepper.isDone() && rotate_dir == false){
    stepper.setDirection(CCW); 
    //stepper.rotate(2);
    //stepper.rotate();
    stepper.rotateDegrees(25);
    rotate_dir = true;
  }
  if (stepper.isDone() && rotate_dir == true){
    stepper.setDirection(CW);   
    //stepper.rotate(1);
    stepper.rotateDegrees(70);
    //stepper.setDirection(STOP);
    rotate_dir = false;
  }
  stepper.run();
}

