#include <CustomStepper.h>            // Подключаем библиотеку CustomStepper
CustomStepper stepper(13, 12, 11, 10);  // Указываем пины, к которым подключен драйвер шагового двигателя
boolean rotate1 = false;              // Переменная для смены режимов
boolean rotatedeg = false;            // Переменная для смены режимов
boolean crotate = false;              // Переменная для смены режимов                

void setup()
{
  stepper.setRPM(12);                 // Устанавливаем количество оборотов в минуту
  stepper.setSPR(4075.7728395);       // Устанавливаем колочество шагов на полный оборот. Максимальное значение 4075.7728395
}

void loop()
{
  if (stepper.isDone() &&  rotate1 == false)  
  {
    stepper.setDirection(CCW);        // Устанавливает направление вращения (по часовой)
    stepper.rotate(2);                // Устанавливает вращение на заданное количество оборотов
    rotate1 = true;
  }
  if (stepper.isDone() && rotate1 == true && rotatedeg == false)
  {
    stepper.setDirection(CW);         // Устанавливает направление вращения (против часовой)
    stepper.rotateDegrees(90);        // Поворачивает вал на заданное кол-во градусов
    rotatedeg = true;
  }
  if (stepper.isDone() && rotatedeg == true && crotate == false)
  {
    stepper.setDirection(CCW);        // Устанавливает направление вращения (по часовой)
    stepper.rotate();                 // Будет вращать пока не получит команду о смене направления или STOP
    crotate = true;
  }
  stepper.run();                      // Этот метод обязателен в блоке loop. Он инициирует работу двигателя, когда это необходимо
}
