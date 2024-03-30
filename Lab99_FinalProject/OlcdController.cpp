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
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 16);
  display.println(firstLine);

  display.setCursor(0, 40);
  display.print(secondLine);

  display.display();
}

void OlcdController::clear() 
{
  if (!OLEDStatus) return;
  
  display.clearDisplay(); 
  display.display();
}

OlcdController OLCD;