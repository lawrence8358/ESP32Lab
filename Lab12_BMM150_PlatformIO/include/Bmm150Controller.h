#ifndef LawrenceLib_Bmm150Controller_H
#define LawrenceLib_Bmm150Controller_H

#include <Arduino.h>
#include <DFRobot_BMM150.h>

class Bmm150Controller {
 public:
  explicit Bmm150Controller(uint8_t address = I2C_ADDRESS_4);

  bool init(
    uint8_t presetMode = BMM150_PRESETMODE_HIGHACCURACY,
    uint8_t dataRate = BMM150_DATA_RATE_10HZ,
    uint8_t maxRetries = 3,
    uint32_t retryDelayMs = 1000
  );

  bool isReady() const { return initialized; }

  sBmm150MagData_t readMagData();

  float readCompassDegree();

 private:
  DFRobot_BMM150_I2C bmm150;
  bool initialized;
};

extern Bmm150Controller BMM150;

#endif
