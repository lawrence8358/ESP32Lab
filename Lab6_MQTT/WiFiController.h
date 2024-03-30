#ifndef LawrenceLib_WiFiController_H
#define LawrenceLib_WiFiController_H

// 連接實際 WIFI 的帳號密碼
// 本版本寫死帳號密碼很爛，但目的是為了當作試驗範例，後續會希望改成外部輸入版本
#define WIFI_SSID "PrimeEagle"
#define WIFI_PASSWORD "0123456789"
 
// GreenPin 網路連線成功的 LED Pin（預設為 -1，表示不使用 LED 顯示）
// RedPin 網路連線失敗的 LED Pin（預設為 -1，表示不使用 LED 顯示）
#define WIFI_GPIO_GREEN_LED 17
#define WIFI_GPIO_RED_LED  16

#include <Arduino.h> 
#include <WiFi.h>
  
class WiFiController 
{
  public:
    /** 建構子 */
    WiFiController(); 

    /** 連接 WiFi */
    void connect();
  
    /** 關閉 WiFi 連線 */
    void close();
    
    /** 檢查 WiFi 連線狀態 */
    bool connected();

  private:
    /** 設定 LED 燈號狀態 */ 
    void setWiFiLEDStatus(bool greenOn);
};
 
extern WiFiController WIFI;

#endif