#include "WiFiController.h" 
#include "MqttController.h"
#include "TempHumController.h" 
#include "OlcdController.h"  
#include "TouchController.h"  
 
const int GIPO_0_BTN = 0;
bool _trunOff = false; 
bool _isTouched = false;
bool _isSendMqtt = false;
unsigned long _lastSendMQttTime = 0; // 上次發送 MQTT 訊息的時間
unsigned long _sendMqttInterval = 3000; // 發送 MQTT 訊息的間隔時間，單位為毫秒

void setup() 
{
  Serial.begin(115200);    
 
  OLCD.init();
  OLCD.print("ESP32", "init..."); 

  Touch.init(); 

  TempHum.init();  

  WIFI.connect();  

  MQTT.init();
  MQTT.receive(mqttReciveCallback);
}

void loop() 
{ 
  vTaskDelay(500 / portTICK_PERIOD_MS); 

  if (digitalRead(GIPO_0_BTN) == LOW) 
  { 
    _trunOff = true;
    MQTT.close();
    WIFI.close(); 
  } 

  _isTouched = Touch.isTouched();
  _isSendMqtt = isNeedSendMqtt();
 
  if (_trunOff == false && _isSendMqtt) 
  {   
    WIFI.reconnect();

    if(WIFI.connected()) MQTT.connect();  

    MQTT.loop();   
  }

  TempHum.readData(tempHumCallback); 
}

// 檢查是否需要發送 MQTT 訊息 
bool isNeedSendMqtt() 
{
  unsigned long currentMillis = millis();
  if (currentMillis - _lastSendMQttTime >= _sendMqttInterval) 
  {   
    _lastSendMQttTime = currentMillis;  
    return true;
  }

  return false;
}

// MQTT 訊息接收回調函數
void mqttReciveCallback(String topic, String message) 
{ 
  displayOLCD(message, true);
}

// 溫濕度回調函數
void tempHumCallback(float humidity, float temperature) 
{ 
  String value;
  String temperatureString = String(temperature, 2);
  String humidityString = String(humidity, 2);
  value = "Temperature: " + temperatureString + " Humidity: " + humidityString;
 
  if(!WIFI.connected()) 
    displayOLCD(value.c_str(), false);
  else if (_isSendMqtt) 
    MQTT.publish(value.c_str());   
}

// 顯示訊息到 OLED
void displayOLCD(String message, bool isOnline) 
{   
  if (_isTouched) 
  {
    // 分割 Temperature 和 Humidity 子字串
    String tempstr = "Temp: " + message.substring(13, 18) + " C";
    String humidstr = "Humid: " + message.substring(29, 34) + " %";
    String status = isOnline ? "Online" : "Offline";

    OLCD.print(status, tempstr, humidstr);  
  }
  else 
  {
    OLCD.clear();  
  }  
}