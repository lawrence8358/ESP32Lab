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
    
  _setLEDStatus(); 
}

// 連接 WiFi
void WiFiController::connect() 
{ 
  if (WIFI_SSID == NULL || WIFI_PASSWORD == NULL) 
  {
    Serial.println("[WiFi] 錯誤：未提供 SSID 或密碼！");
    return;
  }

  Serial.println();
  Serial.print("[WiFi] 連線至 ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // 每次等待 1000ms
  int tryDelay = 1000; 

  // 等待 WiFi 連線狀態
  while (true) 
  {
    _printStatus();

    switch(WiFi.status()) 
    { 
      case WL_CONNECTED:
      case WL_CONNECT_FAILED: 
        return;  
      default: 
        break;
    } 

    delay(tryDelay); 
  }
}

// 關閉 WiFi 連線 
void WiFiController::close() 
{
  Serial.println("[WiFi] 斷開與 WiFi 的連線！");
  if(WiFi.disconnect(true, false) == false) return;

  Serial.println("[WiFi] 已與 WiFi 斷開連線！");
 
  _setLEDStatus();
}

// 檢查 WiFi 連線狀態
bool WiFiController::connected()
{
  bool isConnected = WiFi.status() == WL_CONNECTED ? true : false;
  
  if(WIFI_GPIO_GREEN_LED != -1)  
    digitalWrite(WIFI_GPIO_GREEN_LED, isConnected ? HIGH : LOW); 

  if(WIFI_GPIO_RED_LED != -1)  
    digitalWrite(WIFI_GPIO_RED_LED, !isConnected ? HIGH : LOW);  

  return isConnected;
}

void WiFiController::reconnect()
{ 
  unsigned long currentMillis = millis();

  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - _previousMillis >= _interval)) 
  { 
    Serial.println("Reconnecting to WiFi...");

    WiFi.disconnect();
    WiFi.reconnect();
    
    delay(_interval); 
    _printStatus(); 

    _previousMillis = currentMillis;
  }
}

void WiFiController::_setLEDStatus()
{
  connected();
}

void WiFiController::_printStatus()
{
  switch(WiFi.status()) 
  {
    case WL_IDLE_STATUS: 
      Serial.println("[WiFi] 嘗試連接中"); 
      break;
    case WL_NO_SSID_AVAIL:
      Serial.println("[WiFi] 未找到 SSID"); 
      break;
    case WL_CONNECT_FAILED:
      Serial.println("[WiFi] 失敗 - 未連線至 WiFi！");  
      break;
    case WL_CONNECTION_LOST:
      Serial.println("[WiFi] 連線已中斷"); 
      break;
    case WL_SCAN_COMPLETED:
      Serial.println("[WiFi] 掃描已完成"); 
      break;
    case WL_DISCONNECTED:
      Serial.println("[WiFi] WiFi 連線已中斷"); 
      break;
    case WL_CONNECTED:
      Serial.println("[WiFi] WiFi 連線成功！");
      Serial.print("[WiFi] IP 位址：");
      Serial.println(WiFi.localIP());
      Serial.print("[WiFi] 信號強度：");
      Serial.println(WiFi.RSSI()); 
      break;
    default:
      Serial.print("[WiFi] WiFi 連線狀態：");
      Serial.println(WiFi.status()); 
      break;
  }

  _setLEDStatus();
}

WiFiController WIFI;