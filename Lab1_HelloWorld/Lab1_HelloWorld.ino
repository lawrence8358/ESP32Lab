#include "HelloWorld.h" 

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);   

  // 方法一，直接使用全域 Hello 物件，會顯示預設名稱 Lawrence
  Hello.sayHello();

  // 方法二，自己 new HelloWorld 物件，會顯示自己宣告預設名稱 Shen
  HelloWorld Hello2("Shen");
  Hello2.sayHello();

  // 方法三，呼叫傳入參數，使用傳入的參數名稱 Lawrence Shen
  Hello.sayHello("Lawrence Shen");
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
