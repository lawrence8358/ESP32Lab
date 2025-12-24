#include "Bmm150Controller.h"

Bmm150Controller::Bmm150Controller(uint8_t address)
    : bmm150(&Wire, address), initialized(false) {}

bool Bmm150Controller::init(uint8_t presetMode, uint8_t dataRate, uint8_t maxRetries, uint32_t retryDelayMs) {
  // ESP32 預設 I2C 引腳: SDA=21, SCL=22
  Wire.begin();

  uint8_t attempts = 0;
  // bmm150.begin() 返回 0 表示成功,非 0 表示失敗
  while (bmm150.begin()) {
    attempts++;
    Serial.print("bmm150 初始化嘗試 ");
    Serial.print(attempts);
    Serial.print(" 次, 等待 ");
    Serial.print(retryDelayMs);
    Serial.println(" ms 後重試...");
    if (attempts > maxRetries) {
      Serial.println("bmm150 初始化失敗");
      initialized = false;
      return false;
    }
    delay(retryDelayMs);
  }

  Serial.println("bmm150 初始化成功!");
  bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);
  bmm150.setPresetMode(presetMode);
  bmm150.setRate(dataRate);
  bmm150.setMeasurementXYZ();
  initialized = true;
  return true;
}

sBmm150MagData_t Bmm150Controller::readMagData() {
  if (!initialized) {
    sBmm150MagData_t empty = {0};
    return empty;
  }
  return bmm150.getGeomagneticData();
}

float Bmm150Controller::readCompassDegree() {
  if (!initialized) return NAN;
  return bmm150.getCompassDegree();
}

String Bmm150Controller::getMagDataString() {
  if (!initialized) return "BMM150 N/A";
  
  sBmm150MagData_t data = readMagData();
  float degree = readCompassDegree();
  
  // 格式縮短以適應 OLED 顯示 (整數顯示節省空間)
  String result = "X:" + String((int)data.x) + 
                  " Y:" + String((int)data.y) + 
                  " Z:" + String((int)data.z) +
                  "\nD:" + String(degree, 1) + " deg";
  return result;
}

Bmm150Controller BMM150;
