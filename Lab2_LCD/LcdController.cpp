#include "LcdController.h" 

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
 
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LcdController::LcdController() : _lcd(0x27, lcdColumns, lcdRows) { 
} 
 
void LcdController::init() 
{
  // initialize LCD
  _lcd.init();
  
  // turn on LCD backlight                      
  _lcd.backlight(); 
}
 
void LcdController::print(String firstLine, String secondLine) 
{ 
  _lcd.setCursor(0,0);
  _lcd.print(firstLine);
  
  _lcd.setCursor(0,1);
  _lcd.print(secondLine);
}

void LcdController::clear()
{
  // clear LCD
  _lcd.clear();

  // turn off LCD backlight
  _lcd.noBacklight();
}

LcdController LCD;