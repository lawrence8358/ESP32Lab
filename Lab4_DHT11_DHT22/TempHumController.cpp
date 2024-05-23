#include "TempHumController.h" 

TempHumController::TempHumController(int pin)
{
  dht.setup(pin, DHTesp::DHT_TYPE);
}

void TempHumController::init() 
{ 
}

void TempHumController::readData(DhtCallback callback) 
{
  TempAndHumidity data = dht.getTempAndHumidity();

  if (dht.getStatus() != 0) 
  {
    Serial.println("DHT sensor error status: " + String(dht.getStatusString()));
    return;
  } 

  // Not a Number 
  if (isnan(data.humidity) || isnan(data.temperature)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } 

  callback(data.humidity, data.temperature); 
}

TempHumController TempHum(DHT_GPIO);