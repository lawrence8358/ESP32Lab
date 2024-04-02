#ifndef LawrenceLib_TouchController_H
#define LawrenceLib_TouchController_H

#define TTP223_GPIO 33 

#include <Arduino.h> 

class TouchController 
{
  public:
    /** 建構子 */
    TouchController();
 
    /** 初始化 
     * @param initTouched 是否初始化為觸碰狀態
    */
    void init(bool initTouched = true);
 
    /** 是否被觸碰 */
    bool isTouched(); 

  private: 
    // 前一次觸碰的時間
    unsigned long _previousMillis = 0;
    
    // 觸碰持續返回 true 的間隔時間，預設為 30 秒
    unsigned long _interval = 30000;
};
 
extern TouchController Touch;

#endif