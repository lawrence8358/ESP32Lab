#ifndef LawrenceLib_Bmm150Controller_H
#define LawrenceLib_Bmm150Controller_H

#include <Arduino.h>
#include <DFRobot_BMM150.h>

/**
 * BMM150 磁力計封裝，提供初始化、讀取磁場數值與方位角。
 */
class Bmm150Controller {
 public:
  /**
   * 以指定 I2C 位址建立控制器，預設 CS=1、SDO=1 對應 I2C_ADDRESS_4。
   */
  explicit Bmm150Controller(uint8_t address = I2C_ADDRESS_4);

  /**
   * 初始化感測器並套用基本設定。
   * @param presetMode BMM150_PRESETMODE_* 預設模式
   * @param dataRate   BMM150_DATA_RATE_* 資料率
   * @param maxRetries 失敗重試次數 (>=0)
   * @param retryDelayMs Delay between retries in milliseconds (default 1000)
   * @return 初始化成功回傳 true
   */
  bool init(
    uint8_t presetMode = BMM150_PRESETMODE_HIGHACCURACY,
    uint8_t dataRate = BMM150_DATA_RATE_10HZ,
    uint8_t maxRetries = 3,
    uint32_t retryDelayMs = 1000
  );

  /** 感測器是否初始化成功。 */
  bool isReady() const { return initialized; }

  /** 讀取原始磁場數據 (uT)。 */
  sBmm150MagData_t readMagData();

  /** 取得方位角 (度)，未初始化時回傳 NaN。 */
  float readCompassDegree();

  /** 取得格式化的磁場數據字串 */
  String getMagDataString();

 private:
  DFRobot_BMM150_I2C bmm150;
  bool initialized;
};

extern Bmm150Controller BMM150;

#endif
