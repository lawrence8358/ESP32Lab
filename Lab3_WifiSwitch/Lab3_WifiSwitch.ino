#include "WiFiController.h"
#include "OlcdController.h"   

// 更新 WiFi 資訊
void onWiFiInfoUpdate()
{
    String line1 = "Mode: " + WiFiCtrl.getMode() + "\nSSID: " + WiFiCtrl.getSSID() + "\nIP: " + WiFiCtrl.getIP();
    OLCD.printLine1(line1);
    OLCD.printLine2("");
}
 

void setup()
{
    Serial.begin(115200);

    OLCD.init();
    OLCD.print("Welcome to", "WiFi Switch!");
    
    // 設定 WiFi 資訊更新
    WiFiCtrl.infoCallback = onWiFiInfoUpdate;

    // 嘗試連上無線網路，若成功傳回 true 繼續作業；若失敗則啟用 AP 模式讓使用者連上來設定網路
    if (WiFiCtrl.connectWiFi())
    {
        Serial.print("Connected to WiFi SSID: ");
        Serial.println(WiFi.SSID());

        // TODO: 在此加入其他連線成功後的程式碼
    }
}

void loop()
{ 
}
