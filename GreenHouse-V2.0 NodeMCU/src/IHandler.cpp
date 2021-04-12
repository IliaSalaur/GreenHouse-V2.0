#include <IHandler.h>
#define SYNC 10000

IHandler::IHandler(){}

PumpHandler::PumpHandler()
{
    _id = 0;
    _sensorState = 0;
    _enabled = 0;
    _syncTimer = -SYNC;
}

void PumpHandler::begin(uint8_t id, I2CHandler<MASTER> *twi)
{
    _id = id;
    _twi = twi;
}

void PumpHandler::setMode(ISensor * sensor)
{
    _sensor = sensor;
}

void PumpHandler::handle()
{
    if(millis() - _syncTimer > SYNC)
    {
        _syncTimer = millis();
        _twi->sendCommand(_id, 0b10, 0);
        if(_enabled != _twi->getState())    _twi->sendCommand(_id, 3, _enabled);
    }
    bool sensorState = _sensor->isEnabled();
    if(sensorState == 1 && _sensorState == 0)
    {
        _twi->sendCommand(_id, 3, 1);
        _sensorState = sensorState;
        _enabled = 1;
    }
    

    else if(sensorState == 0 && _sensorState == 1)
    {
        _twi->sendCommand(_id, 3, 0);
        _sensorState = sensorState;
        _enabled = 0;
    }

    //else _sensorState = 0;

}

bool PumpHandler::getState()
{
    return _enabled;
}

Modes PumpHandler::getMode()
{
    return _sensor->getMode();
}

void PumpHandler::manualEnable(bool enable)
{
    _enabled = enable;
    _twi->sendCommand(_id, 3, enable);
}


/*----------------------------------------------------------------------------------------------*/


ClimateHandler::ClimateHandler()
{

}

void ClimateHandler::begin(uint8_t id, I2CHandler<MASTER> *twi)
{

}

void ClimateHandler::setMode(ISensor *sensor)
{

} 

void ClimateHandler::handle()
{

}

bool ClimateHandler::getState()
{

}

Modes ClimateHandler::getMode()
{

}

