#pragma once
#define G1_SOILPIN  A7
#define G1_PUMPPIN  8
#define G1_ID 1

#define G2_SOILPIN  A6
#define G2_PUMPPIN  9
#define G2_ID 2

#define G3_SOILPIN  A3
#define G3_PUMPPIN  10
#define G3_ID 3

#define G4_SOILPIN  A2
#define G4_PUMPPIN  11
#define G4_ID 4



#include <DallasTemperature.h>

struct Settings
{
    byte soilpin;
    byte pumppin;
    byte groupid;
};

class GroupPlant
{
private:
    String _exception = "";
    uint16_t _requestPeriod = 15000;
    uint16_t _calibrationAirValue = 1024;
    uint16_t _calibrationWaterValue = 0;
    uint32_t _soilTempRequestTimer = 0;
    uint8_t _groupId = 0; 
    uint8_t _pumpPin = 0;
    uint8_t _soilPin = 0;
    bool _pumpState = 0;
    bool _addrNotFound = 0;
    float _soilTemp = 0;
    DeviceAddress *_tempSensorAddr;
    DallasTemperature * _tempSensor;

public:
    GroupPlant(DallasTemperature * tempSensor, DeviceAddress *tempSensorAddr,uint8_t soilPin, uint8_t pumpPin, uint8_t groupId);
    GroupPlant(DallasTemperature * tempSensor, DeviceAddress *tempSensorAddr,Settings * settings);
    GroupPlant();
    void begin();
    uint16_t calibrateMoistureAir();
    uint16_t calibrateMoistureWater();
    void turnPump(bool pumpState);
    void clearException();
    bool getPumpState();
    uint8_t getGroupId();
    uint16_t getSoilMoisture();
    float getSoilTemp();        
    void requestSoilTemp();
    void setRequestPeriod(uint16_t requestPeriod);
    String getExceptions();
    ~GroupPlant();
};

GroupPlant::GroupPlant(DallasTemperature *tempSensor, DeviceAddress *tempSensorAddr, uint8_t soilPin, uint8_t pumpPin, uint8_t groupId)
{
  _tempSensor = tempSensor;
  _tempSensorAddr = tempSensorAddr;
  _soilPin = soilPin;
  _pumpPin = pumpPin;
  _groupId = groupId;
  _exception = String(_groupId);
}

GroupPlant::GroupPlant(DallasTemperature *tempSensor, DeviceAddress *tempSensorAddr,Settings * settings)
{
  _tempSensor = tempSensor;
  _tempSensorAddr = tempSensorAddr;
  _soilPin = settings->soilpin;
  _pumpPin = settings->pumppin;
  _groupId = settings->groupid;
  _exception = String(_groupId);
}

void GroupPlant::begin()
{

  pinMode(_soilPin, INPUT);
  pinMode(_pumpPin, OUTPUT);
  digitalWrite(_pumpPin, LOW);
  _tempSensor->begin();
  if (!_tempSensor->getAddress(*_tempSensorAddr, _groupId)){
    _addrNotFound = 1;
    _exception += "Addr not found";
  } 
  else
  {
    _exception += "Addr found: ";
    _addrNotFound = 0;
    for(byte i = 0; i < 8; i++)
    {
      _exception += String(byte(*_tempSensorAddr[i]));
    }    
  }
}

uint16_t GroupPlant::calibrateMoistureAir()
{
  _calibrationAirValue = 0;
  for (byte i = 0; i < 10; i++)
  {
    _calibrationAirValue += analogRead(_soilPin);
  }
  _calibrationAirValue /= 10;
  _exception += "Calibrations value Air ";
  _exception +=String(_calibrationAirValue) ;
  _exception += "  ";
  return _calibrationAirValue;
}

uint16_t GroupPlant::calibrateMoistureWater()
{
  _calibrationWaterValue = 0;
  for (byte i = 0; i < 10; i++)
  {
    _calibrationWaterValue += analogRead(_soilPin);
  }
  _calibrationWaterValue /= 10;
  _exception += "Calibrations value Water ";
  _exception +=String(_calibrationWaterValue);
  _exception += "  ";
  return _calibrationWaterValue;
}

void GroupPlant::clearException()
{
  _exception = String(_groupId);
}

void GroupPlant::turnPump(bool pumpState)
{
  digitalWrite(_pumpPin, pumpState);
  _pumpState = pumpState;
  
}

uint8_t GroupPlant::getGroupId()
{
  return _groupId;
}

uint16_t GroupPlant::getSoilMoisture()
{
  uint16_t moisture = map(analogRead(_soilPin),_calibrationAirValue, _calibrationWaterValue, 0, 100);
  static uint16_t moisture_l;
  //if (moisture >= 0 && moisture <= 100)  return moisture;
  //else return moisture_l;  
  return ((moisture >= 0 && moisture <= 100) ? moisture : moisture_l);
  moisture_l = moisture;
}
float GroupPlant::getSoilTemp()
{
  return _soilTemp;
}

void GroupPlant::requestSoilTemp()
{
  if(millis() - _soilTempRequestTimer > _requestPeriod && _addrNotFound == 0)
  {
    _soilTempRequestTimer = millis();
     _tempSensor->requestTemperaturesByAddress((uint8_t*)_tempSensorAddr);
    float soilTemp = (float)_tempSensor->getTempC((uint8_t*)_tempSensorAddr);
    
    
    if (soilTemp != DEVICE_DISCONNECTED_C)   _soilTemp = soilTemp;
    else
    {
      _exception += "Device disconected";
    }
  }
}

void GroupPlant::setRequestPeriod(uint16_t requestPeriod)
{
  _requestPeriod = requestPeriod;
}

String GroupPlant::getExceptions()
{
  return _exception;
}

bool GroupPlant::getPumpState()
{
  return _pumpState;
}

GroupPlant::~GroupPlant()
{
  delete &_pumpPin;
  delete &_soilPin;
  delete &_exception;
  delete &_calibrationAirValue;
  delete &_groupId;
  delete &_tempSensorAddr;
  delete &_tempSensor;
}