#include "TouchController.h" 

void setup() 
{
  Serial.begin(115200);     
  Touch.init(); 
}

void loop() 
{ 
  vTaskDelay(200 / portTICK_PERIOD_MS); // 暫停 0.2 秒
 
  bool isTouched = Touch.isTouched();
  Serial.println(isTouched); 
}
 
