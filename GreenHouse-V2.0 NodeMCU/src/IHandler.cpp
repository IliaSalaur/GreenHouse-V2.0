#include <IHandler.h>
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



