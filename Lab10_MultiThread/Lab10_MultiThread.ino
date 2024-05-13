TaskHandle_t TaskHandle1;
String _sendData = "";

void setup() 
{
	Serial.begin(115200);  

  xTaskCreatePinnedToCore(
    SendMessageThread,  // 要執行的 Function
    "Task1",            // 自行定義的任務名稱
    10000,              // 所需堆疊空間（常用10000）
    NULL,               // 輸入值
    0,                  // 優先序：0 代表最低，數字越高越優先
    &TaskHandle1,       // 對應的任務 handle變數
    0                   // 指定執行核心編號
  );       
}

void loop() 
{ 
  GetDataThread();
}
 
// 負責取得數據的執行緒
void GetDataThread() 
{
  Serial.print("我是負責 '取得' 感應器資料的執行緒，核心編號：");
  Serial.println(xPortGetCoreID());    
  delay(5000); // 模擬 5 秒取一次
}

// 負責發送訊號的執行緒
void SendMessageThread(void *pvParameters) 
{
  for (;;)
  {
    if(_sendData != "") 
    { 
      Serial.print("我是負責 '發送' 訊號的執行緒，核心編號：");
      Serial.print(xPortGetCoreID()); 
      Serial.print("，發送資料："); 
      Serial.println(_sendData); 
      _sendData = ""; // 清除發送資料
    }
    else 
    { 
      Serial.print("我是負責 '發送' 訊號的執行緒，核心編號：");
      Serial.print(xPortGetCoreID()); 
      Serial.println("，這次沒有資料要發送"); 
    }
    
    delay(1000);
  }
}

