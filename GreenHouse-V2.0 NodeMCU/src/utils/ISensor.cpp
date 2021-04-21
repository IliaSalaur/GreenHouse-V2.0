#include "ISensor.h"
#define REQUEST_TIMER 500
#define BORDER 3.0

ISensor::ISensor(){}

SoilTemperatureSensor::SoilTemperatureSensor(I2CHandler<MASTER> *twi)
{
    _twi = twi;
    _requestPeriod = REQUEST_TIMER;
    _requestTimer = millis() + REQUEST_TIMER;
    _enabled = 0;
    _data = 0.0;
}

void SoilTemperatureSensor::begin(uint8_t id)
{
    _id = id;
}

float SoilTemperatureSensor::getData()
{
    if(millis() - _requestTimer >= _requestPeriod)
    {
        _requestTimer = millis();
        _twi->sendCommand(_id, 0, 0);

        //SEND -READ;
        _data = _twi->getData();
    }
    return _data;
}

bool SoilTemperatureSensor::isEnabled()
{
    Serial.println(this->getData());
    if(this->getData() >= (_normVal + BORDER))
    {
        _enabled = 0;
    }

    else if(this->getData() < (_normVal - BORDER))
    {
        _enabled = 1;
    }
    else Serial.println();
    return _enabled;    
}

void SoilTemperatureSensor::setNormDataVal(float val)
{
    _normVal = val;
}

Modes SoilTemperatureSensor::getMode()
{
    return _mode;
}


/*----------------------------------------------------------------------------------------------*/


SoilMoistureSensor::SoilMoistureSensor(I2CHandler<MASTER> *twi)
{
    _twi = twi;
    _requestPeriod = REQUEST_TIMER;
    _requestTimer = millis() + REQUEST_TIMER;
    _enabled = 0;
    _data = 0.0;
}

void SoilMoistureSensor::begin(uint8_t id)
{
    _id = id;
}

float SoilMoistureSensor::getData()
{
    if(millis() - _requestTimer >= _requestPeriod)
    {
        _requestTimer = millis();
        _twi->sendCommand(_id, 1, 0);

        //SEND -READ;
        _data = _twi->getData();
    }
    return _data;
}

bool SoilMoistureSensor::isEnabled()
{
    Serial.println(this->getData());
    if(this->getData() >= (_normVal + BORDER))
    {
        _enabled = 0;
    }

    else if(this->getData() < (_normVal - BORDER))
    {
        _enabled = 1;
    }
    else Serial.println();
    return _enabled;    
}

void SoilMoistureSensor::setNormDataVal(float val)
{
    _normVal = val;
}

Modes SoilMoistureSensor::getMode()
{
    return _mode;
}

/*----------------------------------------------------------------------------------------------*/


AirHumiditySensor::AirHumiditySensor(I2CHandler<MASTER> *twi)
{
    _twi = twi;
    _requestPeriod = REQUEST_TIMER;
    _requestTimer = millis() + REQUEST_TIMER;
    _enabled = 0;
    _data = 0.0;
}

void AirHumiditySensor::begin(uint8_t id)
{
    _id = id;
}

float AirHumiditySensor::getData()
{
    if(millis() - _requestTimer >= _requestPeriod)
    {
        _requestTimer = millis();
        _twi->sendCommand(_id, 1, 0);

        //SEND -READ;
        _data = _twi->getData();
    }
    return _data;
}

bool AirHumiditySensor::isEnabled()
{
    Serial.println(this->getData());
    if(this->getData() >= (_normVal + BORDER))
    {
        _enabled = 0;
    }

    else if(this->getData() < (_normVal - BORDER))
    {
        _enabled = 1;
    }
    else Serial.println();
    return _enabled;    
}

void AirHumiditySensor::setNormDataVal(float val)
{
    _normVal = val;
}

Modes AirHumiditySensor::getMode()
{
    return _mode;
}


/*----------------------------------------------------------------------------------------------*/


AirTemperatureSensor::AirTemperatureSensor(I2CHandler<MASTER> *twi)
{
    _twi = twi;
    _requestPeriod = REQUEST_TIMER;
    _requestTimer = millis() + REQUEST_TIMER;
    _enabled = 0;
    _data = 0.0;
}

void AirTemperatureSensor::begin(uint8_t id)
{
    _id = id;
}

float AirTemperatureSensor::getData()
{
    if(millis() - _requestTimer >= _requestPeriod)
    {
        _requestTimer = millis();
        _twi->sendCommand(_id, 0, 0);

        //SEND -READ;
        _data = _twi->getData();
    }
    return _data;
}

bool AirTemperatureSensor::isEnabled()
{
    Serial.println(this->getData());
    if(this->getData() >= (_normVal + BORDER))
    {
        _enabled = 0;
    }

    else if(this->getData() < (_normVal - BORDER))
    {
        _enabled = 1;
    }
    else Serial.println();
    return _enabled;    
}

void AirTemperatureSensor::setNormDataVal(float val)
{
    _normVal = val;
}

Modes AirTemperatureSensor::getMode()
{
    return _mode;
}


/*----------------------------------------------------------------------------------------------*/


Manual::Manual()
{
    _enabled = 0;
    _id = 255;
    _requestPeriod = REQUEST_TIMER;
    _requestTimer = 0;
}

bool Manual::isEnabled()
{
    return false;
}

Modes Manual::getMode()
{
    return _mode;
}

void Manual::begin(uint8_t id){}
float Manual::getData(){return 0.0;}
void Manual::setNormDataVal(float val){}

/*----------------------------------------------------------------------------------------------*/


Scheduler::Scheduler(NTPClient * timeClient)
{
    _requestPeriod = REQUEST_TIMER;
    _requestTimer = 0;
    _enabled = 0;
    _timeClient = timeClient;
}

void Scheduler::begin(uint8_t id)
{
    _id = id;
    _timeClient->update();
}

void Scheduler::setSchedule(Schedule sch)
{
    _sch = sch;
    //DEBUG(String(_sch.hourStart) + String(_sch.minuteStart) + String("  ->  ") + String(_sch.hourStop) + String(_sch.minuteStop))
}

Time Scheduler::getTime()
{
    if(millis() - _requestTimer > _requestPeriod)
    {
        _requestTimer = millis();
        _timeClient->update();
        _time.dayIndex = (_timeClient->getDay() == 0) ? 7:_timeClient->getDay();
        _time.hour = _timeClient->getHours();
        _time.minute = _timeClient->getMinutes();        
    }
    return _time;
}

bool Scheduler::isEnabled()
{
    this->getTime();
    if(/*_sch.days[_time.dayIndex - 1] == 1 && */(_time.hour * 60 + _time.minute) >= (_sch.hourStart * 60 + _sch.minuteStart) && (_time.hour * 60 + _time.minute) < (_sch.hourStop * 60 + _sch.minuteStop))
    {
        return 1;
    }
    else return 0;
}
/*
21:01 start sch  1261
21:02 time  sch 1262
21:04 stop sch 1264

*/

void Scheduler::setTimezone(int gmtChange)
{
    _timeZone = gmtChange;
    _timeClient->setTimeOffset(_timeZone * 3600);
}

Modes Scheduler::getMode()
{

    return _mode;
}

float Scheduler::getData()
{
    return 0;
}

void Scheduler::setNormDataVal(float val) {}
