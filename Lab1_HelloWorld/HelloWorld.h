// 底下這兩行這是一個預處理器指令，用於確保在同一個程式碼中這個頭文件不會被多次引用
// 如果 LawrenceLib_HelloWorld_H 這個變數尚未被定義，則執行以下程式碼，否則會跳過
#ifndef LawrenceLib_HelloWorld_H
#define LawrenceLib_HelloWorld_H
 
#include <Arduino.h> // 包含 Arduino 的頭文件，這是為了使用 Arduino 平台的庫函式
#endif

// 這是一個類別的定義，名為 HelloWorld
class HelloWorld 
{ 
  // Public 宣告，在這這個範圍下的成員會公開給外部呼叫時使用
  public:
    // 建構子，用來初始化 HelloWorld 類別的物件，並包含一個字串作為參數，用來設置預設的名字
    HelloWorld(const char* defaultName);
    
    // 這是一個公開的函式，用來向使用者打招呼
    // 它接受一個字串作為參數，如果未提供參數，則使用預設的名字打招呼
    void sayHello(const char* name = NULL); 

  // Private 宣告，在這這個範圍下的成員僅提供內部使用
  private:
    // 這是一個私有的成員變數，用來存儲預設的名字
    const char* _defaultName; 
};

// 這行聲明了一個 HelloWorld 類別的外部實例，名為 hello
// 這樣可以在其他文件中使用這個實例，就是類似底層 Serial.println 的用法
extern HelloWorld Hello;

