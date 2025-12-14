#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "WiFiController.h"

const char *ESP32_WIFI_CONF = "ESP32-WIFI";

// 輸出日誌訊息
void WiFiController::printLog(String msg)
{
    Serial.print(msg);
    if (this -> logCallback != 0)
        this -> logCallback(msg);
}

// 嘗試連接指定的 WiFi 網路
bool WiFiController::attemptWiFiConnection(const String& ssid, const String& passwd, String& wifiIp)
{
    WiFi.begin(ssid.c_str(), passwd.c_str());
    this -> printlnLog("connecting [" + ssid + "] ");
    
    int timeout = 30;
    while (WiFi.status() != WL_CONNECTED && timeout-- > 0)
    {
        this -> printLog(".");
        delay(1000);
    }
    this -> printlnLog();
    
    if (timeout > 0)
    {
        this -> printlnLog("connected.");
        wifiIp = WiFi.localIP().toString();
        this -> printlnLog("IP=" + wifiIp);
        return true;
    }
    else
    {
        this -> printlnLog(ssid + " not connected");
        return false;
    }
}

// 處理設定網頁的主迴圈
void WiFiController::handleConfigWebLoop(String& phase, String& message, String& ssid, String& passwd, String& wifiIp, bool& reboot, bool& reset)
{
    while (true)
    {
        // 處理連線請求
        if (phase == "CONNECTING")
        {
            if (attemptWiFiConnection(ssid, passwd, wifiIp))
            {
                phase = "CONNECTED";
                message = "[" + ssid + "]連線成功<br />IP " + wifiIp;
                
                // 更新為 STA 模式的資訊
                this -> currentMode = "STA";
                this -> currentSSID = ssid;
                this -> currentIP = wifiIp;
                
                // 呼叫資訊更新回調
                if (this -> infoCallback != 0)
                    this -> infoCallback();
            }
            else
            {
                phase = "FAILED";
                message = "[" + ssid + "]連線失敗<br />請重新輸入";
            }
        }
        
        // 處理重新啟動請求
        if (reboot)
        {
            phase = "REBOOT";
            message = "ESP 正在重新啟動...<br />稍後請切換網路改用<br /><a href=http://" + wifiIp + ">http://" + wifiIp + "</a> 操作";
            delay(5000);
            ESP.restart();
        }
        
        // 處理還原預設值請求
        if (reset)
        {
            phase = "RESET";
            message = "正在還原預設值並重新啟動...<br />請稍後重新連線至 ESP-XXXXXX";
            delay(3000);
            clearWiFiConfig();
        }
        
        delay(200);
    }
}

// 設定 Web 伺服器路由（共用函數）
void WiFiController::setupWebServer(AsyncWebServer* server, String& ssid, String& passwd, String& phase, String& message, bool& reboot, bool& reset)
{
    // 儲存當前已連線的 WiFi 資訊（用於 STA 模式時自動填入）
    String savedSsid = this->currentSSID;
    String savedPasswd = WiFi.psk();
    
    // 設定首頁路由
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request -> send_P(200, "text/html", wifi_config_html);
    }); 
    
    // 設定 POST 路由處理表單提交
    server->on("/", HTTP_POST, [&ssid, &passwd, &phase, &message, &reboot, &reset](AsyncWebServerRequest *request) {
        String action = (*request -> getParam("action", true)).value();
        if (action == "重新啟動") {
            reboot = true;
        }
        else if (action == "還原預設值") {
            reset = true;
        }
        else {
            ssid = (*request -> getParam("ssid", true)).value();
            passwd = (*request -> getParam("passwd", true)).value();
            ssid.trim();
            passwd.trim();
            if (ssid.length() > 0 && passwd.length() > 0) {
                message = "正在測試 [" + ssid + "] 無線網路...";
                phase = "CONNECTING";
            }
        }
        request -> send(200, "text/plain", "OK");
    });
    
    // 設定狀態查詢路由（加入已儲存的 WiFi 資訊）
    server->on("/status", HTTP_GET, [this, &phase, &message, savedSsid, savedPasswd](AsyncWebServerRequest *request) {
        String json = "{ \"p\":\"" + phase + "\", \"m\":\"" + message + "\", \"mode\":\"" + this->currentMode + "\"";
        // 在 STA 模式時，將已連線的 WiFi 資訊傳給前端
        if (this->currentMode == "STA") {
            json += ", \"savedSsid\":\"" + savedSsid + "\", \"savedPasswd\":\"" + savedPasswd + "\"";
        }
        // 加入 AP 模式的預設 IP（用於還原預設值後導向）
        json += ", \"apIP\":\"192.168.4.1\"";
        json += " }";
        request -> send(200, "application/json", json);
    });
}

// 初始化設定網頁伺服器（連線失敗時使用，會啟動 AP 模式）
void WiFiController::initConfigWeb()
{
    // 取得晶片 ID
#ifdef ESP32
    uint64_t macAddress = ESP.getEfuseMac();
    uint64_t macAddressTrunc = macAddress << 40;
    uint32_t chipID = macAddressTrunc >> 40;
#else
    auto chipID = ESP.getChipId();
#endif
    
    // 設定 WiFi AP+STA 模式
    auto apSsid = String("ESP-") + String(chipID, HEX);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(apSsid.c_str());
    IPAddress apIp = WiFi.softAPIP();
    
    // 記錄 AP 模式的資訊
    this -> currentMode = "AP";
    this -> currentSSID = apSsid;
    this -> currentIP = apIp.toString();
    
    // 呼叫資訊更新回調
    if (this -> infoCallback != 0)
        this -> infoCallback();
    
    // 輸出 AP 模式資訊
    this -> printlnLog("AP Mode <" + apSsid + ">");
    String ipStr = apIp.toString();
    this -> printlnLog("IP=" + ipStr);
    this -> printlnLog();
    
    // 建立網頁伺服器
    AsyncWebServer* server = new AsyncWebServer(80);
     
    // 初始化狀態變數
    String ssid(""), passwd(""), phase("WAIT"), message("請設定 SSID 與密碼");
    bool reboot = false;
    bool reset = false;
    String wifiIp;

    // 設定伺服器路由
    setupWebServer(server, ssid, passwd, phase, message, reboot, reset);
     
    // 啟動伺服器
    server->begin();
    this -> printlnLog("WiFi config web on");
    this -> printlnLog("http://" + ipStr);

    // 進入主迴圈處理使用者請求
    handleConfigWebLoop(phase, message, ssid, passwd, wifiIp, reboot, reset);
}

// 啟動 Web 伺服器（連線成功後使用，保持 STA 模式）
void WiFiController::startWebServer()
{
    this -> printlnLog("Starting web server in STA mode...");
    
    // 建立網頁伺服器
    AsyncWebServer* server = new AsyncWebServer(80);
     
    // 初始化狀態變數（預先填入當前連線資訊）
    String ssid(this->currentSSID), passwd(WiFi.psk()), phase("CONNECTED"), message("已連線至 WiFi");
    bool reboot = false;
    bool reset = false;
    String wifiIp = this -> currentIP;

    // 設定伺服器路由
    setupWebServer(server, ssid, passwd, phase, message, reboot, reset);
     
    // 啟動伺服器
    server->begin();
    this -> printlnLog("Web server started");
    this -> printlnLog("http://" + this->currentIP);

    // 進入主迴圈處理使用者請求
    handleConfigWebLoop(phase, message, ssid, passwd, wifiIp, reboot, reset);
}

// 嘗試連接已儲存的 WiFi
bool WiFiController::tryConnect()
{
    // 取得已儲存的 WiFi 資訊
    String ssid = WiFi.SSID();
    String pwd = WiFi.psk();
#ifdef ESP32
    ssid = ESP32_WIFI_CONF;
#endif

    this -> printlnLog();
    
    // 連線超時設定為 10 秒
    int timeoutCount = 100;
    bool connected = false;
    
    // 檢查是否有有效的 WiFi 設定
    if (ssid == ESP32_WIFI_CONF || (ssid != "" && pwd != ""))
    {
        WiFi.mode(WIFI_STA);
#ifdef ESP32
        this -> printlnLog(String("Connecting stored SSID "));
        WiFi.begin();
#else
        this -> printLog(String("Conneting ") + ssid + " ");
        WiFi.begin(ssid.c_str(), pwd.c_str());
#endif
        // 等待連線
        while (WiFi.status() != WL_CONNECTED && timeoutCount > 0)
        {
            this -> printLog(".");
            delay(100);
            timeoutCount--;
        }
        this -> printlnLog();
        connected = timeoutCount > 0;
    }
    
    // 根據連線結果進行處理
    if (!connected)
    {
        this -> initConfigWeb();
        return false;
    }
    else
    {
        this -> printlnLog();
        this -> printlnLog("AP connected.");
        this -> printlnLog();
        
        // 取得實際連接的 SSID（避免顯示 ESP32_WIFI_CONF）
        String actualSSID = WiFi.SSID();
        this -> printlnLog("SSID => " + actualSSID);
        this -> printLog("IP => ");
        this -> printlnLog(WiFi.localIP().toString());
        this -> printlnLog();
        
        // 記錄 STA 模式的資訊
        this -> currentMode = "STA";
        this -> currentSSID = actualSSID;
        this -> currentIP = WiFi.localIP().toString();
        
        // 呼叫資訊更新回調
        if (this -> infoCallback != 0)
            this -> infoCallback();
        
        // 啟動 web server 以便管理（保持 STA 模式）
        this -> startWebServer();
        
        return true;
    }
}

// 連接 WiFi
bool WiFiController::connectWiFi()
{
    delay(2000);
    this -> printlnLog("WiFi Connecting...");
    return tryConnect();
}

// 清除 WiFi 設定並重新啟動
void WiFiController::clearWiFiConfig()
{
#ifdef ESP32
    WiFi.disconnect(true, true);
#else
    WiFi.disconnect(true);
#endif
    delay(1000);
    ESP.restart();
}

WiFiController WiFiCtrl;