#ifndef LawrenceLib_TempHumController_H
#define LawrenceLib_TempHumController_H

#define DHT_GPIO 32 

// 根據你的溫濕度感測器的類型調整此參數，預設使用 DHT11
#define DHT_TYPE DHT11 
// #define DHT_TYPE DHT22 

#include <Arduino.h> 
#include "DHTesp.h"
    
/** 溫濕度感測器 callback 函式 */
typedef void (*DhtCallback)(float humidity, float temperature);

class TempHumController 
{
  public:
    /** 建構子 */
    TempHumController(int pin);

    /** 初始化溫濕度感測器 */
    void init();

    /**
     * 讀取溫濕度資料
     * @param callback 回呼函式
     */
    void readData(DhtCallback callback);

  private:
    /** 溫濕度感測器 */
    DHTesp dht;
};
 
extern TempHumController TempHum;

#endif