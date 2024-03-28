//
// 需告要使用 HelloWorld 類別的標頭檔文件
#include "HelloWorld.h" 

// 這是 HelloWorld 類別建構子的實作部分
// 當建立 HelloWorld 物件時，會將一個字串作為預設名字傳遞進來，然後將它儲存在私有成員 _defaultName
HelloWorld::HelloWorld(const char* defaultName)
{  
  _defaultName = defaultName;
}

// 這是 HelloWorld 類別中 sayHello 函式的實作部分
void HelloWorld::sayHello(const char* name) 
{ 
  Serial.print("Hello, "); 
  Serial.println(name != NULL ? name : _defaultName); 
} 

// 這是在全域範圍內創建了一個 HelloWorld 類別的物件，名為 hello
// 並使用 "Lawrence" 作為預設名字來初始化
// 在程式的其它地方就直接使用 hello 這個物件了
HelloWorld Hello("Lawrence");
