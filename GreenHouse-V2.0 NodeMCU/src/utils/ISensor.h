#ifndef ISENSOR_H
#define ISENSOR_H
#include <Arduino.h>
#include "DataTransmiter.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define DEBUG_

#ifdef DEBUG_
#define DEBUG(x) Serial.println(x);
#else 
#define DEBUG(x)
#endif

enum Modes
{
    SCHEDULE,
    AUTO,
    MANUAL
};

struct Schedule
{
    int minuteStart;
    int hourStart;
    int minuteStop;
    int hourStop;
    bool days[7];
};

struct Time
{
    int hour;
    int minute;
    int dayIndex;
};

class ISensor
{
protected:
    bool _enabled;
    uint8_t _id;
    uint32_t _requestTimer;
    uint32_t _requestPeriod;
public:
    ISensor();
    virtual void begin(uint8_t id);

    virtual void setNormDataVal(float val) = 0;
    virtual bool isEnabled() = 0;
    virtual float getData() = 0;
    virtual Modes getMode() = 0;
};

class SoilTemperatureSensor : public ISensor
{
private:
    float _normVal;
    float _data;
    I2CHandler<MASTER> *_twi;
    const Modes _mode = AUTO;
public:
    SoilTemperatureSensor(I2CHandler<MASTER> *twi);
    void begin(uint8_t id) override;
    void setNormDataVal(float val) override;
    bool isEnabled() override;
    float getData() override;
    Modes getMode() override;
};

class SoilMoistureSensor : public ISensor
{
private:
    float _normVal;
    float _data;
    I2CHandler<MASTER>* _twi;
    const Modes _mode = AUTO;
public:
    SoilMoistureSensor(I2CHandler<MASTER> *twi);
    void begin(uint8_t id) override;
    void setNormDataVal(float val) override;
    bool isEnabled() override;
    float getData() override;
    Modes getMode() override;
};

class AirHumiditySensor : public ISensor
{
private:
    float _normVal;
    float _data;
    I2CHandler<MASTER>* _twi;
    const Modes _mode = AUTO;
public:
    AirHumiditySensor(I2CHandler<MASTER> *twi);
    void begin(uint8_t id) override;
    void setNormDataVal(float val) override;
    bool isEnabled() override;
    float getData() override;
    Modes getMode() override;
};

class AirTemperatureSensor : public ISensor
{
private:
    float _normVal;
    float _data;
    I2CHandler<MASTER>* _twi;
    const Modes _mode = AUTO;
public:
    AirTemperatureSensor(I2CHandler<MASTER> *twi);
    void begin(uint8_t id) override;
    void setNormDataVal(float val) override;
    bool isEnabled() override;
    float getData() override;
    Modes getMode() override;
};

class Manual : public  ISensor
{
private:
    const Modes _mode = MANUAL;
public:
    Manual();
    void begin(uint8_t id) override;
    bool isEnabled() override;
    float getData() override;
    void setNormDataVal(float val) override;
    Modes getMode() override;
};

class Scheduler : public ISensor
{
private:
    Schedule _sch;
    int _timeZone = 0;
    Time _time;
    NTPClient *_timeClient;
    const Modes _mode = SCHEDULE;

public:
    Scheduler(NTPClient * timeClient);
    void begin(uint8_t id) override;
    bool isEnabled() override;
    Modes getMode() override;
    Time getTime();
    void setSchedule(Schedule sch);
    void setTimezone(int gmtChange);

    float getData() override;
    void setNormDataVal(float val) override;
};

#endif