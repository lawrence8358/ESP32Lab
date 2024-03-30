#ifndef LawrenceLib_MqttController_H
#define LawrenceLib_MqttController_H

// MQTT 連線設定
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883

// MQTT 連線 ID
#define MQTT_ID "Lawrence"
// MQTT 預設主題
#define MQTT_DEFAULT_TOPIC "primeeagle/esp32" 

// MQTT 訊號發送的 LED Pin（預設為 -1，表示不使用 LED 顯示， 2 使用內建藍色 LED） 
#define MQTT_GPIO_PUBLISH_LED 2

#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
  
typedef void (*MqtCallback)(String topic, String message);

class MqttController 
{
  public: 
    /** 建構子 */
    MqttController(); 
 
    /** 初始化 MQTT */
    void init();

    /** 連接 MQTT */
    void connect();

    /** 發佈訊息 */
    void publish(const char* payload);

    /** 接收訊息 */
    void receive(MqtCallback callback);

    /** 定期呼叫以便和伺服器保持連線，以便處理接收到的訊息 */
    void loop();
    
    /** 關閉 MQTT 連線 */
    void close();
 
    /** 檢查 MQTT 連線狀態 */
    bool connected();

  private:    
    /** 使用 WiFiClient 執行 MQTT */
    WiFiClient _espClient;         

    /** 使用 WiFiClient 執行 MQTT */
    PubSubClient _client;  
  
    /** 訂閱主題 
     * @param topic 主題
     * @param message 訊息
     * @param length 長度
    */
    static void _callback(char* topic, byte* message, unsigned int length);
};

extern MqttController MQTT;

#endif