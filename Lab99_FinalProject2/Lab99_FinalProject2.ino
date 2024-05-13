#include "WiFiController.h"
#include "MqttController.h"
#include "TempHumController.h"
#include "OlcdController.h" 
#include "Elgamal.h" 

String _sendData = "";   
TaskHandle_t SendMessageTaskHandle;
unsigned long _getInterval = 1000; // 要取得訊號 & 更新 UI 的間隔時間，單位為毫秒
unsigned long _sendInterval = 3000; // 發送 MQTT 訊息的間隔時間，單位為毫秒

void setup()
{
  Serial.begin(115200);

  OLCD.init();
  OLCD.print("Wifi", "init...");
  
  TempHum.init();

  WIFI.connect();

  MQTT.init(); 

  xTaskCreatePinnedToCore(
    SendMessageCore,      // 要執行的 Function
    "SendMessageTask",      // 自行定義的任務名稱
    10000,                  // 所需堆疊空間（常用10000）
    NULL,                   // 輸入值
    0,                      // 優先序：0 代表最低，數字越高越優先
    &SendMessageTaskHandle, // 對應的任務 handle變數
    0                       // 指定執行核心編號
  );       
}

void loop()
{
  GetDataAndUpdateUiCore();
}

// 負責發送訊號
void SendMessageCore(void *pvParameters) 
{
  for (;;)
  {
    vTaskDelay(_sendInterval / portTICK_PERIOD_MS);
 
    if(_sendData != "") 
    { 
      Serial.println("MQTT.publish : " + _sendData);
      MQTT.publish(_sendData.c_str()); 
 
      _sendData = ""; // 清除發送資料
    }  

    WIFI.reconnect();

    if (WIFI.connected())
      MQTT.connect();

    MQTT.loop();
  }
} 
 
// 負責取得訊號和更新 UI
void GetDataAndUpdateUiCore() 
{
  vTaskDelay(_getInterval / portTICK_PERIOD_MS); 
  TempHum.readData(tempHumCallback); 
} 

// 溫濕度回調函數
void tempHumCallback(float humidity, float temperature)
{
  // 顯示 UI 
  displayOLCD(humidity, temperature);

  // 如果啟用 Elgamal 加密，則將溫度和濕度加密
  uint64_t Y = elgamal.GenY(23);  // TODO: 改讀取 Server 端發送
  uint64_t x = 5;

  uint64_t* cipher_humid_array = elgamal.Encrypt(int(humidity * 100) + 0.5, Y, x); 
  uint64_t* cipher_temp_array = elgamal.Encrypt(int(temperature * 100) + 0.5, Y, x);  
  uint64_t K = cipher_humid_array[0];  
  uint64_t cipher_humid_c = cipher_humid_array[1];   
  uint64_t cipher_temp_c = cipher_temp_array[1];  
  
  // 釋放記憶體
  free(cipher_humid_array); 
  free(cipher_temp_array);

  // 要發送的加密資料
  _sendData = "{\"Temperature\":" + String(cipher_temp_c) + ",\"Humidity\":" + String(cipher_humid_c) + ",\"K\":" + String(K) + "}"; 
}

// 顯示訊息到 OLED
void displayOLCD(float humidity, float temperature)
{
  String tempstr = "Temp: " + String(temperature) + " C";
  String humidstr = "Humid: " + String(humidity) + " %";  
  String status = WIFI.connected() ? "Online" : "Offline";
  
  OLCD.print(status, tempstr, humidstr);
}