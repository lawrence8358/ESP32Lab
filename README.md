# 部落格文章 - ESP32 系列


### [[ESP32 系列] 初探 ESP32](https://lawrencetech.blogspot.com/2024/03/esp32-esp32.html) 
> * ESP32 基本介紹
> * Arduino IDE 開發工具
> * 第一個小程式
> * 物件導向的寫法，```Lab1_HelloWorld```


---
### Lab2_LCD，LCD 控制
> * 使用前須先安裝 `LiquidCrystal_I2C library`，可使用 packages 內的壓縮檔，解壓縮進行安裝，安裝步驟可參考【[How to Use I2C LCD with ESP32 on Arduino IDE](https://randomnerdtutorials.com/esp32-esp8266-i2c-lcd-arduino-ide/)】、【[搭配LCD 1602 I2C顯示模組](https://shop.mirotek.com.tw/iot/esp32-start-9/)】這兩個網站的教學。
> * ESP32-S，LCD 需使用 I2C 通訊，5V 電壓
>     + SCL → GPIO 22 (右上 3)
>     + SDA → GPIO 21 (右上 6)


---
### Lab3_WiFi，連接 Wifi
> * 使用前需先到 WiFiController.h 設定底下的參數 
>     + `WIFI_SSID`：WIFI SSID
>     + `WIFI_PASSWORD`：WIFI 密碼
>     + `WIFI_GPIO_GREEN_LED`：WIFI 連線 "成功燈號" 的輸出腳位
>     + `WIFI_GPIO_RED_LED`：WIFI 連線 "失敗燈號" 的輸出腳位 


---
### Lab4_DHT11，讀取溫濕度感測器
> * 使用前必須先安裝 DHT sensor library by Adafruit v1.4.6 版，可透過 DHT11 關鍵字搜尋。
> * 使用前需先到 TempHumController.h 設定底下的參數  
>     + `DHT_GPIO`：溫溼度的輸入腳位


---
### License
The MIT license