/**
 * Lab13_GeomagneticMonitor
 * 
 * 地磁監測器 - 使用 BMM150 感測器收集三軸地磁數據
 * 定期輸出原始數據、計算值、變化量和統計值
 * 
 * 硬體連接:
 *   ESP32 GPIO21 (SDA) -> BMM150 SDA
 *   ESP32 GPIO22 (SCL) -> BMM150 SCL
 *   ESP32 3.3V         -> BMM150 VCC
 *   ESP32 GND          -> BMM150 GND
 */

#include <DFRobot_BMM150.h>

// ============ 配置參數 ============
#define SAMPLE_INTERVAL_MS    100     // 採樣間隔 (毫秒)，10Hz
#define PRINT_INTERVAL_MS     1000    // 輸出間隔 (毫秒)
#define STATS_WINDOW_SIZE     100     // 統計視窗大小 (樣本數)
#define BMM150_I2C_ADDR       I2C_ADDRESS_4  // 0x13

// ============ BMM150 感測器 ============
DFRobot_BMM150_I2C bmm150(&Wire, BMM150_I2C_ADDR);

// ============ 數據結構 ============
struct MagneticData {
    float x;              // X軸 (μT)
    float y;              // Y軸 (μT)
    float z;              // Z軸 (μT)
    float total;          // 總磁場強度
    float horizontal;     // 水平磁場強度
    float compass;        // 羅盤角度 (度)
    float inclination;    // 磁傾角 (度)
};

struct DeltaData {
    float dx;             // X軸變化量
    float dy;             // Y軸變化量
    float dz;             // Z軸變化量
    float dTotal;         // 總變化量
    float rate;           // 變化速率 (μT/秒)
};

struct StatsData {
    float avgX, avgY, avgZ;
    float stdX, stdY, stdZ;
    float variance;
    int sampleCount;
};

// ============ 全域變數 ============
MagneticData currentData;
MagneticData previousData;
DeltaData deltaData;
StatsData statsData;

// 滑動視窗緩衝區
float bufferX[STATS_WINDOW_SIZE];
float bufferY[STATS_WINDOW_SIZE];
float bufferZ[STATS_WINDOW_SIZE];
int bufferIndex = 0;
int bufferCount = 0;

// 計時器
unsigned long lastSampleTime = 0;
unsigned long lastPrintTime = 0;
unsigned long sampleIndex = 0;

// ============ Setup ============
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println();
    Serial.println("========================================");
    Serial.println("   Lab13 - 地磁監測器 (BMM150)");
    Serial.println("========================================");
    Serial.println();
    
    // 初始化 BMM150
    Serial.print("初始化 BMM150...");
    
    int retryCount = 0;
    const int maxRetries = 10;
    
    while (bmm150.begin() != 0) {
        retryCount++;
        Serial.print(".");
        if (retryCount >= maxRetries) {
            Serial.println(" 失敗!");
            Serial.println("請檢查接線: SDA->GPIO21, SCL->GPIO22");
            while (true) delay(1000);
        }
        delay(500);
    }
    Serial.println(" 成功!");
    
    // 設定感測器模式
    bmm150.setOperationMode(BMM150_POWERMODE_NORMAL);
    bmm150.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY);
    bmm150.setRate(BMM150_DATA_RATE_30HZ);
    bmm150.setMeasurementXYZ();
    
    Serial.println();
    Serial.println("配置:");
    Serial.printf("  採樣間隔: %d ms (%d Hz)\n", SAMPLE_INTERVAL_MS, 1000/SAMPLE_INTERVAL_MS);
    Serial.printf("  輸出間隔: %d ms\n", PRINT_INTERVAL_MS);
    Serial.printf("  統計視窗: %d 樣本\n", STATS_WINDOW_SIZE);
    Serial.println();
    Serial.println("開始監測...");
    Serial.println("========================================");
    Serial.println();
    
    // 初始化緩衝區
    memset(bufferX, 0, sizeof(bufferX));
    memset(bufferY, 0, sizeof(bufferY));
    memset(bufferZ, 0, sizeof(bufferZ));
}

// ============ Loop ============
void loop() {
    unsigned long now = millis();
    
    // 採樣
    if (now - lastSampleTime >= SAMPLE_INTERVAL_MS) {
        lastSampleTime = now;
        sampleIndex++;
        
        // 保存前一筆數據
        previousData = currentData;
        
        // 讀取新數據
        readMagneticData();
        
        // 計算衍生數據
        calculateDerivedData();
        
        // 計算變化量
        calculateDelta();
        
        // 更新統計
        updateStatistics();
    }
    
    // 輸出
    if (now - lastPrintTime >= PRINT_INTERVAL_MS) {
        lastPrintTime = now;
        printData();
    }
}

// ============ 讀取感測器數據 ============
void readMagneticData() {
    sBmm150MagData_t magData = bmm150.getGeomagneticData();
    
    currentData.x = magData.x;
    currentData.y = magData.y;
    currentData.z = magData.z;
    currentData.compass = bmm150.getCompassDegree();
}

// ============ 計算衍生數據 ============
void calculateDerivedData() {
    // 總磁場強度 = √(X² + Y² + Z²)
    currentData.total = sqrt(
        currentData.x * currentData.x + 
        currentData.y * currentData.y + 
        currentData.z * currentData.z
    );
    
    // 水平磁場強度 = √(X² + Y²)
    currentData.horizontal = sqrt(
        currentData.x * currentData.x + 
        currentData.y * currentData.y
    );
    
    // 磁傾角 = atan(Z / 水平分量)
    if (currentData.horizontal > 0.01) {
        currentData.inclination = atan2(currentData.z, currentData.horizontal) * 180.0 / PI;
    } else {
        currentData.inclination = (currentData.z > 0) ? 90.0 : -90.0;
    }
}

// ============ 計算變化量 ============
void calculateDelta() {
    if (sampleIndex <= 1) {
        deltaData.dx = 0;
        deltaData.dy = 0;
        deltaData.dz = 0;
        deltaData.dTotal = 0;
        deltaData.rate = 0;
        return;
    }
    
    deltaData.dx = currentData.x - previousData.x;
    deltaData.dy = currentData.y - previousData.y;
    deltaData.dz = currentData.z - previousData.z;
    
    // 變化量的向量長度
    deltaData.dTotal = sqrt(
        deltaData.dx * deltaData.dx + 
        deltaData.dy * deltaData.dy + 
        deltaData.dz * deltaData.dz
    );
    
    // 變化速率 (μT/秒)
    deltaData.rate = deltaData.dTotal / (SAMPLE_INTERVAL_MS / 1000.0);
}

// ============ 更新統計數據 ============
void updateStatistics() {
    // 加入緩衝區
    bufferX[bufferIndex] = currentData.x;
    bufferY[bufferIndex] = currentData.y;
    bufferZ[bufferIndex] = currentData.z;
    
    bufferIndex = (bufferIndex + 1) % STATS_WINDOW_SIZE;
    if (bufferCount < STATS_WINDOW_SIZE) {
        bufferCount++;
    }
    
    statsData.sampleCount = bufferCount;
    
    // 計算平均值
    float sumX = 0, sumY = 0, sumZ = 0;
    for (int i = 0; i < bufferCount; i++) {
        sumX += bufferX[i];
        sumY += bufferY[i];
        sumZ += bufferZ[i];
    }
    statsData.avgX = sumX / bufferCount;
    statsData.avgY = sumY / bufferCount;
    statsData.avgZ = sumZ / bufferCount;
    
    // 計算標準差
    float varX = 0, varY = 0, varZ = 0;
    for (int i = 0; i < bufferCount; i++) {
        varX += (bufferX[i] - statsData.avgX) * (bufferX[i] - statsData.avgX);
        varY += (bufferY[i] - statsData.avgY) * (bufferY[i] - statsData.avgY);
        varZ += (bufferZ[i] - statsData.avgZ) * (bufferZ[i] - statsData.avgZ);
    }
    statsData.stdX = sqrt(varX / bufferCount);
    statsData.stdY = sqrt(varY / bufferCount);
    statsData.stdZ = sqrt(varZ / bufferCount);
    
    // 總變異數
    statsData.variance = (varX + varY + varZ) / (3.0 * bufferCount);
}

// ============ 輸出數據 ============
void printData() {
    Serial.println("----------------------------------------");
    Serial.printf("樣本 #%lu (時間: %lu ms)\n", sampleIndex, millis());
    Serial.println();
    
    // 原始數據
    Serial.println("[原始數據]");
    Serial.printf("  X: %8.2f μT\n", currentData.x);
    Serial.printf("  Y: %8.2f μT\n", currentData.y);
    Serial.printf("  Z: %8.2f μT\n", currentData.z);
    Serial.println();
    
    // 計算值
    Serial.println("[計算值]");
    Serial.printf("  總磁場:     %8.2f μT\n", currentData.total);
    Serial.printf("  水平磁場:   %8.2f μT\n", currentData.horizontal);
    Serial.printf("  羅盤角度:   %8.2f °\n", currentData.compass);
    Serial.printf("  磁傾角:     %8.2f °\n", currentData.inclination);
    Serial.println();
    
    // 變化量
    Serial.println("[變化量]");
    Serial.printf("  ΔX: %+8.3f μT\n", deltaData.dx);
    Serial.printf("  ΔY: %+8.3f μT\n", deltaData.dy);
    Serial.printf("  ΔZ: %+8.3f μT\n", deltaData.dz);
    Serial.printf("  ΔTotal: %8.3f μT\n", deltaData.dTotal);
    Serial.printf("  變化速率: %8.3f μT/s\n", deltaData.rate);
    Serial.println();
    
    // 統計值
    Serial.printf("[統計值] (視窗: %d/%d)\n", statsData.sampleCount, STATS_WINDOW_SIZE);
    Serial.printf("  平均 X: %8.2f μT  標準差: %6.3f\n", statsData.avgX, statsData.stdX);
    Serial.printf("  平均 Y: %8.2f μT  標準差: %6.3f\n", statsData.avgY, statsData.stdY);
    Serial.printf("  平均 Z: %8.2f μT  標準差: %6.3f\n", statsData.avgZ, statsData.stdZ);
    Serial.printf("  變異數: %8.4f\n", statsData.variance);
    Serial.println();
}
