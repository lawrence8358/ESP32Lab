#include "MqttController.h"
  
MqtCallback _callbackEvent;

MqttController::MqttController() : _client(_espClient) 
{
  // 檢查MQTT 訊號發送的 LED 連線成功狀態的腳位是否有設定
  if (MQTT_GPIO_PUBLISH_LED != -1)  
    pinMode(MQTT_GPIO_PUBLISH_LED, OUTPUT); 
}

void MqttController::init()
{
  _client.setServer(MQTT_SERVER, MQTT_PORT); 
}

void MqttController::connect() 
{ 
  // 檢查連線狀態
  while(!_client.connected()) 
  { 
    // 訂閱 channel
    if (_client.connect(MQTT_ID))
      _client.subscribe(MQTT_DEFAULT_TOPIC); 
  }
}

void MqttController::publish(const char* payload)
{ 
  bool wifiConntected = WiFi.status() == WL_CONNECTED;
  if(!wifiConntected || !connected()) return;

  // Serial.print("publish : ");
  // Serial.println(payload);

  if(MQTT_GPIO_PUBLISH_LED != -1)  
    digitalWrite(MQTT_GPIO_PUBLISH_LED, HIGH); 

  _client.publish(MQTT_DEFAULT_TOPIC, payload); 
  _client.loop();
 
  vTaskDelay(200 / portTICK_PERIOD_MS); // 暫停 0.2 秒
  if(MQTT_GPIO_PUBLISH_LED != -1)  
    digitalWrite(MQTT_GPIO_PUBLISH_LED, LOW); 
}  

void MqttController::receive(MqtCallback callback)
{
  _callbackEvent = callback;
  _client.setCallback(_callback);
}

void MqttController::loop()
{
  if(!connected()) return;
  _client.loop();
}
  
void MqttController::close()
{
  _client.disconnect();
}

bool MqttController::connected()
{
  return _client.connected();
}
 
void MqttController::_callback(char* topic, byte* message, unsigned int length)
{
  String messageTemp;

  // 封包內的資料
  for (int i = 0; i < length; i++) 
  {
    messageTemp += (char)message[i];
  } 

  _callbackEvent(String(topic), messageTemp);
}

MqttController MQTT;
 