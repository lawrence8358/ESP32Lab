#ifndef LawrenceLib_LcdController_H
#define LawrenceLib_LcdController_H
 
#include <Arduino.h> 
#include <LiquidCrystal_I2C.h>
#endif
  
class LcdController 
{
  public: 
    /** 建構子 */
    LcdController();  

    /** 初始化 LCD */
    void init(); 

    /**
    * 顯示兩行文字
    * @param firstLine 第一行文字
    * @param secondLine 第二行文字
    */
    void print(String firstLine, String secondLine);

    /** 清除 LCD */
    void clear();

  private: 
    /** LCD 控制器 */
    LiquidCrystal_I2C _lcd;  
};
 
extern LcdController LCD;