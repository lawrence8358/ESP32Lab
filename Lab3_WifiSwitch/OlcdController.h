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
    * 只更新第一行文字
    * @param firstLine 第一行文字
    */
    void printLine1(String firstLine);
    
    /**
    * 只更新第二行文字
    * @param secondLine 第二行文字
    */
    void printLine2(String secondLine);

    /** 清除 OLCD */
    void clear();

  private:
    Adafruit_SSD1306 display;
    bool OLEDStatus;
    String currentLine1;
    String currentLine2;
};
 
extern OlcdController OLCD;

#endif