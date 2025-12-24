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

void OlcdController::clear() 
{
  if (!OLEDStatus) return;
  
  display.clearDisplay(); 
  display.display();
}

OlcdController OLCD;
