#ifndef JALOUSE_H
#define JALOUSE_H

#include <Arduino.h>
#include "GyverStepper.h"

enum Jalouse_State
{
    OPEN,
    CLOSE
};

class Jalouse
{
    private:
    int _rotations = 4;
    GStepper <STEPPER4WIRE> *_jalStepper;
    Jalouse_State _jalState;

    public:
    Jalouse(int pin1, int pin2, int pin3, int pin4);   
    void open();
    void begin(); 
    void close();
    void handle();
    void setRotations(int rots);
    Jalouse_State getState();

};

#endif