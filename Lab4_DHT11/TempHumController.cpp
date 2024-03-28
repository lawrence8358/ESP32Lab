#include "TempHumController.h" 

TempHumController::TempHumController(int pin) : dht(pin, DHT11) 
{ 
}

void TempHumController::init() 
{
  dht.begin();
}

void TempHumController::readData(DhtCallback callback) 
{
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Not a Number 
  if(isnan(humidity) || isnan(temperature))
  { 
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  callback(humidity, temperature);
}

TempHumController TempHum(DHT_GPIO);