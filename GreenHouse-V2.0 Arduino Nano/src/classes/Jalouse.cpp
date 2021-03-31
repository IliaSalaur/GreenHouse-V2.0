#include "Jalouse.h"
Jalouse::Jalouse(int pin1, int pin2, int pin3, int pin4)
{
    _jalStepper = new GStepper<STEPPER4WIRE>(1024, pin1, pin2, pin3, pin4);
}

void Jalouse::begin()
{
  _jalStepper->setRunMode(FOLLOW_POS);
  _jalStepper->setMaxSpeed(400);
  _jalStepper->setAcceleration(1024); 
}

void Jalouse::open()
{
    _jalStepper->setTarget(0);
    _jalState = OPEN;
}

void Jalouse::close()
{
    _jalStepper->setTarget(1024 * _rotations);
    _jalState = CLOSE;
}

void Jalouse::setRotations(int rots)
{
    _rotations = rots;
}

Jalouse_State Jalouse::getState()
{
    return _jalState;
}

void Jalouse::handle()
{
    _jalStepper->tick();
}