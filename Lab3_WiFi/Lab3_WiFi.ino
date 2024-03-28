#include "WiFiController.h" 

// GPIO Port 定義
int GIPO_0_BTN = 0;
  
void setup() 
{
  Serial.begin(115200);    

  WIFI.connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
}

void loop() 
{
  vTaskDelay(200 / portTICK_PERIOD_MS); // 暫停 0.2 秒
  
  // 若偵測到按下 GPIO 0 按鈕，則關閉 WiFi 連線
  if (digitalRead(GIPO_0_BTN) == LOW) 
    WIFI.closeWiFi(); 
}
