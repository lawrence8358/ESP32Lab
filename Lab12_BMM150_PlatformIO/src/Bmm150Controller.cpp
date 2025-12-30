#include "Bmm150Controller.h"

Bmm150Controller::Bmm150Controller(uint8_t address)
    : bmm150(&Wire, address), initialized(false) {}

bool Bmm150Controller::init(uint8_t presetMode, uint8_t dataRate, uint8_t maxRetries, uint32_t retryDelayMs) {
  Wire.begin();

  uint8_t attempts = 0;
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

Bmm150Controller BMM150;
