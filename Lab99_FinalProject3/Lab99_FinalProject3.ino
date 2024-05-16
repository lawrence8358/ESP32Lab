#include "WiFiController.h"
#include "MqttController.h"
#include "OlcdController.h" 
#include "Elgamal.h"  

String _sendData = "";   
TaskHandle_t SendMessageTaskHandle;
unsigned long _getInterval = 500; // 已符合發送條件要等待的間隔時間，單位為毫秒
unsigned long _sendInterval = 3000; // 發送 MQTT 訊息的間隔時間，單位為毫秒

Elgamal* elgamal = nullptr;

// 接收端的 Elgamal 參數，預設值為 0，實際值要從 Server 端取得
uint64_t Server_Elgamal_G = 0; // 公開選定的數(任意)
uint64_t Server_Elgamal_P = 0; // 大質數
uint64_t Server_Elgamal_Y = 0; // 公鑰 Y

int GPIO_SHOCK = 32; 
int alarmCount = 1; // 震動達幾次才發送訊號
int preShockValue = 0;
int shockCount = 0;

void setup()
{
  Serial.begin(115200);

  pinMode(GPIO_SHOCK, INPUT);  

  OLCD.init();
  OLCD.print("Wifi", "init...");
  
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
    
    shockCount = 0;
  }
} 
 
// 負責取得訊號和更新 UI
void GetDataAndUpdateUiCore() 
{  
  displayOLCD();

  if (elgamal == nullptr) 
  { 
    vTaskDelay(100 / portTICK_PERIOD_MS); 
    return;
  } 

  int shockValue = digitalRead(GPIO_SHOCK);

  // 連續震動判斷，必須是不同震動才累加次數，若前一次都是也是低電位，視為相同震動
  if(shockValue == LOW && shockValue != preShockValue)  
    shockCount += 1; 

  preShockValue = shockValue;  
  displayOLCD();

  if(shockCount >= alarmCount)
  { 
    uint64_t r = rand() % 100;  // 每次加密隨機產生的數 0~99，這邊的 r 要小於 P 的整數   
    uint64_t* cipher_chipId_array = elgamal->Encrypt(getChipId(), Server_Elgamal_Y, r);  

    uint64_t X = cipher_chipId_array[0];   
    uint64_t cipher_chipId_c = cipher_chipId_array[1];  
    
    // 釋放記憶體 
    free(cipher_chipId_array);

    // 要發送的加密資料
    _sendData = "{";
    _sendData += "\"Type\":101,";
    _sendData += "\"ChipId\":" + String(cipher_chipId_c) + ","; 
    _sendData += "\"X\":" + String(X);
    _sendData += "}"; 

    vTaskDelay(_getInterval / portTICK_PERIOD_MS); 
    shockCount = 0;
  } 
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
 
// 顯示訊息到 OLED
void displayOLCD()
{  
  String status = WIFI.connected() ? "Online" : "Offline";
  String serverKey = (elgamal == nullptr) ? "Init key exchange": "Key exchange done";
  String count = "Count : " + String(shockCount);
  
  OLCD.print(status, serverKey, count);
}

uint32_t getChipId() 
{
  uint32_t chipId = 0;
  for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}