#include "WiFiController.h" 
#include "MqttController.h"
#include "TempHumController.h" 
#include "OlcdController.h"  
 
int GIPO_0_BTN = 0;
bool trunOff = false;

void setup() 
{
  Serial.begin(115200);    

  OLCD.init();
  OLCD.print("ESP32", "init..."); 

  TempHum.init(); 

  WIFI.connect();  

  MQTT.init();
  MQTT.receive(mqttReciveCallback);
}

void loop() 
{ 
  vTaskDelay(2000 / portTICK_PERIOD_MS); 
  
  if(trunOff == true) return;

  if (digitalRead(GIPO_0_BTN) == LOW) 
  { 
    trunOff = true;
    MQTT.close();
    WIFI.close(); 
  }
  
  TempHum.readData(tempHumCallback);

  WIFI.reconnect();

  if(WIFI.connected()) MQTT.connect();  

  MQTT.loop();  
}

void mqttReciveCallback(String topic, String message) 
{
  displayOLCD(message, true);
}

void tempHumCallback(float humidity, float temperature) 
{
  String value;
  String temperatureString = String(temperature, 2);
  String humidityString = String(humidity, 2);
  value = "Temperature: " + temperatureString + " Humidity: " + humidityString;
 
  if(!WIFI.connected()) 
    displayOLCD(value.c_str(), false);
  else 
    MQTT.publish(value.c_str());  
}

void displayOLCD(String message, bool isOnline) 
{
  // 分割 Temperature 和 Humidity 子字串
  String tempstr = "Temp: " + message.substring(13, 18) + " C";
  String humidstr = "Humid: " + message.substring(29, 34) + " %";
  String status = isOnline ? "Online" : "Offline";

  OLCD.print(status, tempstr, humidstr);  
}