#include "WiFiController.h"
#include "MqttController.h"
#include "TempHumController.h"
#include "OlcdController.h" 
#include "Elgamal.h"  

String _sendData = "";   
TaskHandle_t SendMessageTaskHandle;
unsigned long _getInterval = 1000; // 要取得訊號 & 更新 UI 的間隔時間，單位為毫秒
unsigned long _sendInterval = 3000; // 發送 MQTT 訊息的間隔時間，單位為毫秒

Elgamal* elgamal = nullptr;

// 接收端的 Elgamal 參數，預設值為 0，實際值要從 Server 端取得
uint64_t Server_Elgamal_G = 0; // 公開選定的數(任意)
uint64_t Server_Elgamal_P = 0; // 大質數
uint64_t Server_Elgamal_Y = 0; // 公鑰 Y

void setup()
{
  Serial.begin(115200);

  OLCD.init();
  OLCD.print("Wifi", "init...");
  
  TempHum.init();

  WIFI.connect();

  MQTT.init(); 
  MQTT.receive(mqttReciveCallback);

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

// 處理 MQTT 回傳的公開金鑰 
void mqttReciveCallback(String topic, String message)
{
  // 已完成初始化，就不需要再處理了
  if (elgamal != nullptr)
  {
    return;
  }

  int gIndex = message.indexOf("g=");
  if (gIndex == 0)
  {
    // if (elgamal != nullptr) delete elgamal;

    int pIndex = message.indexOf("p=");
    int YIndex = message.indexOf("Y=");
 
    Server_Elgamal_G = atol(message.substring(2, pIndex - 1).c_str()); 
    Server_Elgamal_P = atol(message.substring(pIndex + 2, YIndex - 1).c_str()); 
    Server_Elgamal_Y = atol(message.substring(YIndex + 2).c_str());
  
    Serial.printf("Server 端的 Elgamal G : ");
    Serial.println(Server_Elgamal_G);
    Serial.printf("Server 端的 Elgamal P : ");
    Serial.println(Server_Elgamal_P);
    Serial.printf("Server 端的 Elgamal Y : ");
    Serial.println(Server_Elgamal_Y);

    elgamal = new Elgamal(Server_Elgamal_P, Server_Elgamal_G);
  }
}

// 溫濕度回調函數
void tempHumCallback(float humidity, float temperature)
{
  // 顯示 UI 
  displayOLCD(humidity, temperature);

  if (elgamal == nullptr) 
  {
    Serial.println("尚未取的 Server 端的 Elgamal 參數");
    return;
  } 

  uint64_t r = rand() % 100;  // 每次加密隨機產生的數 0~99，這邊的 r 要小於 P 的整數  
  uint64_t* cipher_humid_array = elgamal->Encrypt(int(humidity * 100) + 0.5, Server_Elgamal_Y, r); 
  uint64_t* cipher_temp_array = elgamal->Encrypt(int(temperature * 100) + 0.5, Server_Elgamal_Y, r);  

  uint64_t X = cipher_humid_array[0];  
  uint64_t cipher_humid_c = cipher_humid_array[1];   
  uint64_t cipher_temp_c = cipher_temp_array[1];  
  
  // 釋放記憶體
  free(cipher_humid_array); 
  free(cipher_temp_array);

  // 要發送的加密資料
  _sendData = "{\"Temperature\":" + String(cipher_temp_c) + ",\"Humidity\":" + String(cipher_humid_c) + ",\"X\":" + String(X) + "}"; 
}

// 顯示訊息到 OLED
void displayOLCD(float humidity, float temperature)
{
  String tempstr = "Temp: " + String(temperature) + " C";
  String humidstr = "Humid: " + String(humidity) + " %";  
  String status = WIFI.connected() ? "Online" : "Offline";
  
  OLCD.print(status, tempstr, humidstr);
}