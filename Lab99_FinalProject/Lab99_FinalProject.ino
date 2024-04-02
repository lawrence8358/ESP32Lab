#include "WiFiController.h"
#include "MqttController.h"
#include "TempHumController.h"
#include "OlcdController.h"
#include "TouchController.h"
#include "RSA.h"

const int GIPO_0_BTN = 0; 
const String TemperatureKey = "Temperature=";
const String HumidityKey = "Humidity=";

bool _rsaEnable = true;
bool _trunOff = false;
bool _isTouched = false;
bool _isSendMqtt = false;

unsigned long _lastSendMQttTime = 0;    // 上次發送 MQTT 訊息的時間
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
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  if (digitalRead(GIPO_0_BTN) == LOW)
  {
    _trunOff = true;
    MQTT.close();
    WIFI.close();
  }

  _isTouched = Touch.isTouched();
  _isSendMqtt = isNeedSendMqtt();
 
  // TODO: 效能問題，後續改多執行序優化
  if(_isSendMqtt) 
  {
    TempHum.readData(tempHumCallback);
  }

  if (_trunOff == false && _isSendMqtt)
  {
    WIFI.reconnect();

    if (WIFI.connected())
      MQTT.connect();

    MQTT.loop();
  } 
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

  // 未加密的溫度和濕度
  value = TemperatureKey + String(temperature, 2) + "\n" + HumidityKey + String(humidity, 2);
 
  // 如果啟用 RSA 加密，則將溫度和濕度加密
  if(_rsaEnable == true)
  {  
    uint64_t cipher_temp = rsa.Encrypt(uint64_t(temperature * 100));
    uint64_t cipher_humid = rsa.Encrypt(uint64_t(humidity * 100));
    value = TemperatureKey + String(cipher_temp) + "\n" + HumidityKey + String(cipher_humid);
  } 

  if (!WIFI.connected())
    displayOLCD(value.c_str(), false);
  else if (_isSendMqtt) 
  {
    //Serial.println("MQTT.publish : " + value);
    MQTT.publish(value.c_str()); 
  }
}

// 顯示訊息到 OLED
void displayOLCD(String message, bool isOnline)
{
  if (_isTouched)
  {
    // 取得 Temperature 和 Humidity 的值 
    // message 的格式範例，Temperature=25.00\nHumidity=50.00  
    int index = message.indexOf("\n");
    String temp = message.substring(TemperatureKey.length(), index);
    String humid = message.substring(index + HumidityKey.length() + 1);
  
    // 如果啟用 RSA 加密，則解密溫度和濕度
    if(_rsaEnable == true)
    {
      uint64_t cipher_temp = temp.toInt();
      uint64_t cipher_humid = humid.toInt();
      temp = String(rsa.Decrypt(cipher_temp) / 100.0, 2);
      humid = String(rsa.Decrypt(cipher_humid) / 100.0, 2);
    }

    String tempstr = "Temp: " + temp + " C";
    String humidstr = "Humid: " + humid + " %";  
    String status = isOnline ? "Online" : "Offline";
   
    OLCD.print(status, tempstr, humidstr);
  }
  else
  {
    OLCD.clear();
  }
}