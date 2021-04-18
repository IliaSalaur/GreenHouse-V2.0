#include "IHandler.h"
#define SYNC 10000
#include <Debug.h>

IHandler::IHandler(){}


/*----------------------------------------------------------------------------------------------*/


Handler::Handler()
{
    _id = 0;
    _sensorState = 0;
    _enabled = 0;
    _syncTimer = -SYNC;
}

void Handler::begin(uint8_t id, I2CHandler<MASTER> *twi)
{
    _id = id;
    _twi = twi;
}

void Handler::setMode(ISensor * sensor)
{
    _sensor = sensor;
}

void Handler::handle()
{
    //sync deleted
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

bool Handler::getState()
{
    return _enabled;
}

Modes Handler::getMode()
{
    return _sensor->getMode();
}

void Handler::manualEnable(bool enable)
{
    _enabled = enable;
    _twi->sendCommand(_id, 3, enable);
}


/*----------------------------------------------------------------------------------------------*/


HeatHandler::HeatHandler()
{
    _id = 0;
    _sensorState = 0;
    _enabled = 0;
    _syncTimer = -SYNC;
}

void HeatHandler::begin(uint8_t id, I2CHandler<MASTER> *twi)
{
    _id = id;
    _twi = twi;
}

void HeatHandler::setMode(ISensor * sensor[4])
{
    for(int i = 0; i < 4; i++)
    {
        _sensors[i] = sensor[i];
    }
}

void HeatHandler::setMode(ISensor *sensor)
{
    for(int i =0; i < 4; i++)
    {
        _sensors[i] = sensor;
    }
}

void HeatHandler::handle()
{
    //sync deleted
    bool sensorState = 0;
    for(int i = 0; i < 4; i++)
    {
        sensorState |= _sensors[i]->isEnabled();
    }
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

bool HeatHandler::getState()
{
    return _enabled;
}

Modes HeatHandler::getMode()
{
    return _sensor->getMode();
}

void HeatHandler::manualEnable(bool enable)
{
    _enabled = enable;
    _twi->sendCommand(_id, 3, enable);
}
