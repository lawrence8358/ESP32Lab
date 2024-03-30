#include "WiFiController.h" 
#include "MqttController.h"

// GPIO Port 定義
int GIPO_0_BTN = 0;

void setup() 
{
  Serial.begin(115200);    

  WIFI.connect();  
  
  MQTT.init();
  MQTT.receive(mqttReciveCallback);
}

void loop() 
{ 
  vTaskDelay(2000 / portTICK_PERIOD_MS); // 暫停 2 秒
  
  // 若偵測到按下 GPIO 0 按鈕，則關閉 WiFi & MQTT 連線
  if (digitalRead(GIPO_0_BTN) == LOW) 
  { 
    MQTT.close();
    WIFI.close(); 
  }
 
  if(WIFI.connected()) MQTT.connect();
  MQTT.publish("Hello MQTT, I am ESP32."); 

  // 若要接收訊息，必須定期呼叫以便和伺服器保持連線
  // 若不須接收訊息，則可以註解底下這行
  MQTT.loop(); 
}

void mqttReciveCallback(String topic, String message) 
{
  Serial.print("MQTT Callback : ");
  Serial.println(message); 
}
