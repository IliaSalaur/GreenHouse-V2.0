#pragma once
#ifndef IHANDLER_H
#define IHANDLER_H

#include <Arduino.h>
#include <ISensor.h>

class IHandler
{
protected:
    uint8_t _id;
    uint32_t _syncTimer;
    bool _sensorState;
    bool _enabled;
    ISensor * _sensor;
    I2CHandler<MASTER> * _twi;

public:
    IHandler();
    virtual void begin(uint8_t id, I2CHandler<MASTER> *twi) = 0;
    virtual void setMode(ISensor * sensor) = 0;
    virtual void handle() = 0;
    virtual bool getState() = 0;
    virtual void manualEnable(bool enable) = 0;
    virtual Modes getMode() = 0;
};

class PumpHandler : IHandler
{
private:


public:
    PumpHandler();
    void begin(uint8_t id, I2CHandler<MASTER> *twi) override;
    void setMode(ISensor *sensor) override;
    void handle() override;
    bool getState() override;
    void manualEnable(bool enable) override;
    Modes getMode() override;

};

class ClimateHandler : IHandler
{
private:


public:
    ClimateHandler();
    void begin(uint8_t id, I2CHandler<MASTER> *twi) override;
    void setMode(ISensor *sensor) override;
    void handle() override;
    bool getState() override;
    Modes getMode() override;
};

#endif