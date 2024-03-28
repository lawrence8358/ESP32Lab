#include "WiFiController.h" 

// 建構子
WiFiController::WiFiController()
{    
  // 檢查綠色 LED 連線成功狀態的腳位是否有設定
  if (WIFI_GPIO_GREEN_LED != -1)  
    pinMode(WIFI_GPIO_GREEN_LED, OUTPUT); 

  // 檢查紅色 LED 連線失敗狀態的腳位是否有設定
  if (WIFI_GPIO_RED_LED != -1)  
    pinMode(WIFI_GPIO_RED_LED, OUTPUT); 
    
  setWiFiLEDStatus(false);
}

// 連接 WiFi
void WiFiController::connectToWiFi(const char* ssid, const char* password) 
{
  if (ssid == NULL || password == NULL) 
  {
    Serial.println("[WiFi] 錯誤：未提供 SSID 或密碼！");
    return;
  }

  Serial.println();
  Serial.print("[WiFi] 連線至 ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // 會嘗試大約 10 秒 (20x 500ms)
  int tryDelay = 500;
  int numberOfTries = 20; 

  // 等待 WiFi 連線狀態
  while (true) 
  {
      switch(WiFi.status()) 
      {
          case WL_NO_SSID_AVAIL:
              Serial.println("[WiFi] 未找到 SSID");
              setWiFiLEDStatus(false);
              break;
          case WL_CONNECT_FAILED:
              Serial.println("[WiFi] 失敗 - 未連線至 WiFi！");
              setWiFiLEDStatus(false);
              return;
              break;
          case WL_CONNECTION_LOST:
              Serial.println("[WiFi] 連線已中斷");
              setWiFiLEDStatus(false);
              break;
          case WL_SCAN_COMPLETED:
              Serial.println("[WiFi] 掃描已完成");
              setWiFiLEDStatus(false);
              break;
          case WL_DISCONNECTED:
              Serial.println("[WiFi] WiFi 連線已中斷");
              setWiFiLEDStatus(false);
              break;
          case WL_CONNECTED:
              Serial.println("[WiFi] WiFi 連線成功！");
              Serial.print("[WiFi] IP 位址：");
              Serial.println(WiFi.localIP());
              Serial.print("[WiFi] 信號強度：");
              Serial.println(WiFi.RSSI());
              setWiFiLEDStatus(true);
              return;
          default:
              Serial.print("[WiFi] WiFi 連線狀態：");
              Serial.println(WiFi.status());
              setWiFiLEDStatus(true);
              break;
      }
      delay(tryDelay);
      
      if(numberOfTries <= 0)
      {
          Serial.print("[WiFi] 連線至 WiFi 失敗！");
          // 使用斷開連線功能強制停止連線嘗試
          WiFi.disconnect();
          setWiFiLEDStatus(false);
          return;
      } 
      else 
      {
          numberOfTries--;
      }
  }
}

// 關閉 WiFi 連線 
void WiFiController::closeWiFi() 
{
  Serial.println("[WiFi] 斷開與 WiFi 的連線！");
  if(WiFi.disconnect(true, false) == false) return;

  Serial.println("[WiFi] 已與 WiFi 斷開連線！");
 
  setWiFiLEDStatus(false);
}

void WiFiController::setWiFiLEDStatus(bool greenOn)
{
  if(WIFI_GPIO_GREEN_LED != -1)  
    digitalWrite(WIFI_GPIO_GREEN_LED, greenOn ? HIGH : LOW); 

  if(WIFI_GPIO_RED_LED != -1)  
    digitalWrite(WIFI_GPIO_RED_LED, !greenOn ? HIGH : LOW);  
}

WiFiController WIFI;