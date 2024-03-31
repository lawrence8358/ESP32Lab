#include "TouchController.h" 

TouchController::TouchController() { }

void TouchController::init(bool initTouched) 
{
  pinMode(TTP223_GPIO, INPUT); 

  if(initTouched) _previousMillis = millis();
}

bool TouchController::isTouched() 
{
  unsigned long currentTime = millis();

  bool isTouched = digitalRead(TTP223_GPIO) == 1 ? true : false;
  if (isTouched) _previousMillis = currentTime;

  return (currentTime - _previousMillis < _interval) ? true : false;
}

TouchController Touch;