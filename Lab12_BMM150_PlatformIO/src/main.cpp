#include <Arduino.h>
#include "Bmm150Controller.h"

// I2C 位址選擇：CS/SDO 腳位高低對應下列組合
// I2C_ADDRESS_1 0x10  (CS:0 SDO:0)
// I2C_ADDRESS_2 0x11  (CS:0 SDO:1)
// I2C_ADDRESS_3 0x12  (CS:1 SDO:0)
// I2C_ADDRESS_4 0x13  (CS:1 SDO:1) 預設位址

const uint32_t BMM150_RETRY_DELAY_MS = 500;
const uint32_t LOOP_DELAY_MS = 1000;

void showXYZ();

void setup()
{
  Serial.begin(115200);
  delay(1000);

  if (!BMM150.init(BMM150_PRESETMODE_HIGHACCURACY, BMM150_DATA_RATE_30HZ, 3, BMM150_RETRY_DELAY_MS))
  {
    Serial.println("bmm150 init failed, Please try again!");
    while (true) delay(1000);
  }
  
  Serial.println("bmm150 init success!");
}

void loop()
{
  showXYZ();
}

void showXYZ() {
  sBmm150MagData_t magData = BMM150.readMagData();
  Serial.print("mag x = "); Serial.print(magData.x); Serial.print(" uT");
  Serial.print(", xx = "); Serial.print(magData.xx); Serial.println(" uT");

  Serial.print("mag y = "); Serial.print(magData.y); Serial.print(" uT");
  Serial.print(", yy = "); Serial.print(magData.yy); Serial.println(" ");

  Serial.print("mag z = "); Serial.print(magData.z); Serial.print(" uT");
  Serial.print(", zz = "); Serial.print(magData.zz); Serial.println(" uT");

  float compassDegree = BMM150.readCompassDegree();
  Serial.print("指向方向相對北的逆時針角度:");
  Serial.println(compassDegree);
  Serial.println("--------------------------------");
  delay(LOOP_DELAY_MS);
}