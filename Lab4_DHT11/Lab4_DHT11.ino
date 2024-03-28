#include "TempHumController.h" 

void setup() 
{
  Serial.begin(115200);    

  TempHum.init(); 
}

void loop() 
{ 
  vTaskDelay(2000 / portTICK_PERIOD_MS); // 暫停 2 秒
 
  TempHum.readData(myCallback);
}

void myCallback(float humidity, float temperature) 
{
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%, Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
}
