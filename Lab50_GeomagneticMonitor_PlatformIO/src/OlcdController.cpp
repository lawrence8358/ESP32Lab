#include "OlcdController.h" 

OlcdController::OlcdController() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), OLEDStatus(true) 
{
}

void OlcdController::init() 
{ 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    OLEDStatus = false;
  }
}

void OlcdController::print(String firstLine, String secondLine) 
{
  if (!OLEDStatus) return;
  
  // 儲存當前內容
  currentLine1 = firstLine;
  currentLine2 = secondLine;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println(firstLine);

  display.setTextSize(1, 2);
  display.setCursor(0, 30);
  display.print(secondLine);

  display.display();
}

void OlcdController::printLine1(String firstLine) 
{
  if (!OLEDStatus) return;
  
  // 更新第一行，保持第二行
  currentLine1 = firstLine;
  print(currentLine1, currentLine2);
}

void OlcdController::printLine2(String secondLine) 
{
  if (!OLEDStatus) return;
  
  // 更新第二行，保持第一行
  currentLine2 = secondLine;
  print(currentLine1, currentLine2);
}

void OlcdController::printGeomagneticData(String line1, float magX, float magY, float magZ, float degree) 
{
  if (!OLEDStatus) return;
  
  currentLine1 = line1;
  
  display.clearDisplay();
  
  // 第一行：WiFi/MQTT 狀態（小字）
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  
  // 第二行：X 和 Y 磁場數據（左側）
  display.setCursor(0, 35);
  display.print("X:");
  display.print(magX, 2);
  display.print(" Y:");
  display.print(magY, 2);
  
  // 第三行：Z 磁場數據（左側）
  display.setCursor(0, 45);
  display.print("Z:");
  display.print(magZ, 2);
  
  // 第四行：角度顯示（左側）
  display.setCursor(0, 55);
  display.print("Deg:");
  display.print(degree, 1);
  display.write(0xF8);  // CP437 度數符號
  
  // 右側：繪製指北針（圓形 + 箭頭）
  int compassX = 112;  // 指北針中心 X
  int compassY = 50;   // 指北針中心 Y
  int radius = 10;     // 圓形半徑
  
  // 繪製外圈
  display.drawCircle(compassX, compassY, radius, SSD1306_WHITE);
  
  // 繪製刻度線（東西南北四個方向）
  // 北（上）
  display.drawLine(compassX, compassY - radius, compassX, compassY - radius - 3, SSD1306_WHITE);
  // 東（右）
  display.drawLine(compassX + radius, compassY, compassX + radius + 3, compassY, SSD1306_WHITE);
  // 南（下）
  display.drawLine(compassX, compassY + radius, compassX, compassY + radius + 3, SSD1306_WHITE);
  // 西（左）
  display.drawLine(compassX - radius, compassY, compassX - radius - 3, compassY, SSD1306_WHITE);
  
  // 計算指北針方向
  // degree = 0° 表示設備指向北，90° 指向東，180° 指向南，270° 指向西
  // 箭頭永遠指向北方（相對於設備）
  // 在螢幕座標系（Y向下為正）：90° 是上方，0° 是右方，270° 是下方，180° 是左方
  // 當設備指向北(0°)，箭頭指向上方(90°)
  // 當設備指向東(90°)，箭頭指向左方(0°/360°)
  // 公式: screen_angle = 90° + degree
  float radian = (90.0 + degree) * PI / 180.0;
  
  // 繪製三角形箭頭（指向北方）
  int arrowLength = (int)(radius * 0.75);
  int arrowWidth = 3;
  
  // 箭頭頂點（指向北方）
  int tipX = compassX + (int)(arrowLength * cos(radian));
  int tipY = compassY + (int)(arrowLength * sin(radian));
  
  // 箭頭底部左右兩點
  float baseAngle = radian + PI;  // 反方向
  int baseLeftX = compassX + (int)((arrowLength * 0.4) * cos(baseAngle) + arrowWidth * cos(baseAngle - PI/2));
  int baseLeftY = compassY + (int)((arrowLength * 0.4) * sin(baseAngle) + arrowWidth * sin(baseAngle - PI/2));
  int baseRightX = compassX + (int)((arrowLength * 0.4) * cos(baseAngle) + arrowWidth * cos(baseAngle + PI/2));
  int baseRightY = compassY + (int)((arrowLength * 0.4) * sin(baseAngle) + arrowWidth * sin(baseAngle + PI/2));
  
  // 繪製箭頭三角形（用線條勾勒）
  display.drawLine(tipX, tipY, baseLeftX, baseLeftY, SSD1306_WHITE);
  display.drawLine(tipX, tipY, baseRightX, baseRightY, SSD1306_WHITE);
  display.drawLine(baseLeftX, baseLeftY, baseRightX, baseRightY, SSD1306_WHITE);
  
  // 填充箭頭（繪製多條線使其看起來實心）
  for (int i = 0; i <= 10; i++) {
    float t = i / 10.0;
    int fillX1 = tipX + (int)((baseLeftX - tipX) * t);
    int fillY1 = tipY + (int)((baseLeftY - tipY) * t);
    int fillX2 = tipX + (int)((baseRightX - tipX) * t);
    int fillY2 = tipY + (int)((baseRightY - tipY) * t);
    display.drawLine(fillX1, fillY1, fillX2, fillY2, SSD1306_WHITE);
  }
  
  display.display();
}

void OlcdController::clear() 
{
  if (!OLEDStatus) return;
  
  display.clearDisplay(); 
  display.display();
}

OlcdController OLCD;
