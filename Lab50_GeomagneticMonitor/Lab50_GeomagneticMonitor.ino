/**
 * Lab80_GeoMagMonitor - 地磁監測器
 * 
 * 功能說明：
 * 1. WiFi AP/STA 模式切換，並在 OLED 上顯示狀態
 * 2. BMM150 地磁感測器數據持續顯示在 OLED 第二列
 * 3. 網路連線成功後，透過 MQTT 發送地磁數據（不加密）
 * 4. 使用多執行緒（FreeRTOS）分離各個功能
 * 
 * CPU 核心分配：
 * - Core 0: WiFi 連線管理、MQTT 發送
 * - Core 1: BMM150 數據讀取、OLED 顯示更新
 */

#include "WiFiController.h"
#include "OlcdController.h"
#include "Bmm150Controller.h"
#include "MqttController.h"

// ============== 設定參數 ==============
// BMM150 重試延遲 (毫秒)
const uint32_t BMM150_RETRY_DELAY_MS = 500;
// BMM150 讀取間隔 (毫秒)
const uint32_t BMM150_READ_INTERVAL_MS = 1000;
// MQTT 發送間隔 (毫秒)
const uint32_t MQTT_SEND_INTERVAL_MS = 5000;

// ============== 全域變數 ==============
// 執行緒 Handle
TaskHandle_t TaskHandle_WiFi;
TaskHandle_t TaskHandle_BMM150;

// 共享資料（使用 volatile 確保執行緒安全）
volatile bool wifiConnected = false;
volatile bool mqttReady = false;

// 地磁數據緩衝區（用於 MQTT 發送）
String magDataBuffer = "";
SemaphoreHandle_t magDataMutex;

// ============== WiFi 資訊更新回調 ==============
void onWiFiInfoUpdate()
{
    String line1 = "Mode: " + WiFiCtrl.getMode() + "\nSSID: " + WiFiCtrl.getSSID() + "\nIP: " + WiFiCtrl.getIP();
    OLCD.printLine1(line1);
}

// ============== MQTT 接收回調 ==============
void mqttReceiveCallback(String topic, String message) 
{
    Serial.print("MQTT Callback : ");
    Serial.println(message); 
}

// ============== WiFi & MQTT 執行緒 (Core 0) ==============
void WiFiMqttThread(void *pvParameters) 
{
    Serial.print("[WiFi Thread] 啟動於核心: ");
    Serial.println(xPortGetCoreID());

    // 初始化 MQTT
    MQTT.init();
    MQTT.receive(mqttReceiveCallback);

    // 嘗試連接 WiFi（此函式會阻塞直到連線成功或進入 AP 模式）
    if (WiFiCtrl.connectWiFi())
    {
        Serial.println("[WiFi Thread] WiFi 連線成功");
        wifiConnected = true;
    }

    // 主迴圈（當 WiFi 連線成功後處理 MQTT）
    for (;;)
    {
        // 檢查 WiFi 連線狀態
        if (WiFiCtrl.isConnected())
        {
            wifiConnected = true;

            // 連接 MQTT
            if (!MQTT.connected())
            {
                Serial.println("[WiFi Thread] 連接 MQTT...");
                MQTT.connect();
                mqttReady = true;
                Serial.println("[WiFi Thread] MQTT 連線成功");
            }

            // 發送地磁數據
            if (MQTT.connected())
            {
                // 取得地磁數據（使用 mutex 保護）
                if (xSemaphoreTake(magDataMutex, portMAX_DELAY) == pdTRUE)
                {
                    if (magDataBuffer.length() > 0)
                    {
                        // 建立 JSON 格式的訊息
                        sBmm150MagData_t data = BMM150.readMagData();
                        float degree = BMM150.readCompassDegree();
                        
                        String jsonPayload = "{";
                        jsonPayload += "\"x\":" + String(data.x, 2) + ",";
                        jsonPayload += "\"y\":" + String(data.y, 2) + ",";
                        jsonPayload += "\"z\":" + String(data.z, 2) + ",";
                        jsonPayload += "\"degree\":" + String(degree, 2);
                        jsonPayload += "}";
                        
                        Serial.print("[WiFi Thread] 發送 MQTT: ");
                        Serial.println(jsonPayload);
                        MQTT.publish(jsonPayload);
                    }
                    xSemaphoreGive(magDataMutex);
                }

                // MQTT 維持連線
                MQTT.loop();
            }
        }
        else
        {
            wifiConnected = false;
            mqttReady = false;
        }

        vTaskDelay(MQTT_SEND_INTERVAL_MS / portTICK_PERIOD_MS);
    }
}

// ============== BMM150 & OLED 執行緒 (Core 1) ==============
void BMM150Thread(void *pvParameters) 
{
    Serial.print("[BMM150 Thread] 啟動於核心: ");
    Serial.println(xPortGetCoreID());

    // 初始化 BMM150
    if (!BMM150.init(BMM150_PRESETMODE_HIGHACCURACY, BMM150_DATA_RATE_30HZ, 3, BMM150_RETRY_DELAY_MS))
    {
        Serial.println("[BMM150 Thread] BMM150 初始化失敗!");
        OLCD.printLine2("BMM150 Error!");
        
        // 初始化失敗後持續顯示錯誤
        for (;;)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    Serial.println("[BMM150 Thread] BMM150 初始化成功");

    // 主迴圈
    for (;;)
    {
        // 讀取地磁數據
        String magDataStr = BMM150.getMagDataString();
        
        // 更新 OLED 第二列
        OLCD.printLine2(magDataStr);

        // 更新共享緩衝區（使用 mutex 保護）
        if (xSemaphoreTake(magDataMutex, portMAX_DELAY) == pdTRUE)
        {
            magDataBuffer = magDataStr;
            xSemaphoreGive(magDataMutex);
        }

        // Serial 輸出（除錯用）
        sBmm150MagData_t data = BMM150.readMagData();
        float degree = BMM150.readCompassDegree();
        Serial.print("[BMM150] X:");
        Serial.print(data.x);
        Serial.print(" Y:");
        Serial.print(data.y);
        Serial.print(" Z:");
        Serial.print(data.z);
        Serial.print(" Degree:");
        Serial.println(degree);

        vTaskDelay(BMM150_READ_INTERVAL_MS / portTICK_PERIOD_MS);
    }
}

// ============== Setup ==============
void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("=================================");
    Serial.println("Lab80_GeoMagMonitor Starting...");
    Serial.println("=================================");

    // 初始化 OLED
    OLCD.init();
    OLCD.print("GeoMag Monitor", "Initializing...");

    // 建立 Mutex
    magDataMutex = xSemaphoreCreateMutex();

    // 設定 WiFi 資訊更新回調
    WiFiCtrl.infoCallback = onWiFiInfoUpdate;

    // 建立 BMM150 執行緒（Core 1）
    xTaskCreatePinnedToCore(
        BMM150Thread,       // 執行函式
        "BMM150Task",       // 任務名稱
        10000,              // 堆疊大小
        NULL,               // 參數
        1,                  // 優先權
        &TaskHandle_BMM150, // Task Handle
        1                   // 執行核心 (Core 1)
    );

    // 建立 WiFi & MQTT 執行緒（Core 0）
    xTaskCreatePinnedToCore(
        WiFiMqttThread,     // 執行函式
        "WiFiMqttTask",     // 任務名稱
        10000,              // 堆疊大小
        NULL,               // 參數
        1,                  // 優先權
        &TaskHandle_WiFi,   // Task Handle
        0                   // 執行核心 (Core 0)
    );

    Serial.println("All tasks created successfully!");
}

// ============== Loop ==============
void loop()
{
    // 主迴圈保持空閒，所有工作由 FreeRTOS 執行緒處理
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
