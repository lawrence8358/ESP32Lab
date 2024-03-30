#ifndef LawrenceLib_OlcdController_H
#define LawrenceLib_OlcdController_H

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#include <Arduino.h> 
#include <Adafruit_SSD1306.h> 
  
class OlcdController  
{
  public: 
    /** 建構子 */
    OlcdController();  

    /** 初始化 LCD */
    void init(); 

    /**
    * 顯示兩行文字
    * @param firstLine 第一行文字
    * @param secondLine 第二行文字
    */
    void print(String firstLine, String secondLine);

    /**
    * 顯示兩行文字
    * @param firstLine 第一行文字
    * @param secondLine 第二行文字
    * @param thirdLine 第三行文字
    */
    void print(String firstLine, String secondLine, String thirdLine);

    /** 清除 OLCD */
    void clear();

  private:
    Adafruit_SSD1306 display;
    bool OLEDStatus;
};
 
extern OlcdController OLCD;

#endif