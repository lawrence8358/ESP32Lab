int WIFI_GPIO_GREEN_LED = 17; 
int WIFI_GPIO_RED_LED = 16;
int GPIO_SHOCK = 32; 
int alarmCount = 10; // 震動達幾次跳紅色警示燈

int preShockValue = 0;
int shockCount = 0;

void setup() 
{
  Serial.begin(115200);   

  // 設定 GPIO Pin 腳位
  pinMode(WIFI_GPIO_GREEN_LED, OUTPUT);  
  pinMode(WIFI_GPIO_RED_LED, OUTPUT);  
	pinMode(GPIO_SHOCK, INPUT);  

  // 清除預設值
  digitalWrite(WIFI_GPIO_GREEN_LED, LOW);  
  digitalWrite(WIFI_GPIO_RED_LED, LOW);   
}

void loop() 
{  
  int shockValue = digitalRead(GPIO_SHOCK);
 
  if(shockValue == HIGH)     
  {
    digitalWrite(WIFI_GPIO_GREEN_LED, LOW);   
    digitalWrite(WIFI_GPIO_RED_LED, HIGH);  
  }
  else   
  {
    digitalWrite(WIFI_GPIO_GREEN_LED, HIGH); 
    digitalWrite(WIFI_GPIO_RED_LED, LOW);    
  } 

  delay(100);
}
 
