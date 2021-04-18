#include <Arduino.h>

#include <utils/DataTransmiter.h>
#include "utils/IHandler.h"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Debug.h>

#define SSID "Gnorberg_MSK"
#define PASS "Vfzccjgh11$"
#define TOKEN "LEBsLiSAb_vptx1ZWnTTgxd7yr-wC79Z"
#define TIMEZONE 3

I2CHandler<MASTER> twi;

WiFiUDP ntpUdp;
NTPClient timeClient(ntpUdp, "europe.pool.ntp.org");

Scheduler *pumpSch[4], lightSch(&timeClient), wateringSch(&timeClient);
SoilTemperatureSensor *soilTemp[4];
SoilMoistureSensor *soilMois[4];
AirHumiditySensor airHudm(&twi);
AirTemperatureSensor airTemp(&twi);
Manual *manual;

Handler *pumpHandler[4], lightHandler, wateringHandler, jalouseHandler, fanHandler;
HeatHandler heatHandler;

BlynkTimer timerBl;
BlynkTimer syncTimer;
WidgetLED led(V37);

uint32_t timer = 0;

void pushData();
void sync();
float getSoilTempAvg();

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50);
  twi.begin(D1, D2, 0x12, 0x13);
  timeClient.begin();

  WiFi.mode(WIFI_STA);
  
  Blynk.begin(TOKEN, SSID, PASS);
  timerBl.setInterval(10000L, pushData);
  syncTimer.setInterval(30000l, sync);

  Serial.println();
  Serial.println(WiFi.localIP());

   manual = new Manual();

  for(int i = 0; i < 4; i++)
  {
    soilTemp[i] = new SoilTemperatureSensor(&twi);
    soilMois[i] = new SoilMoistureSensor(&twi);
    pumpSch[i] = new Scheduler(&timeClient);
    pumpHandler[i] = new Handler();

    soilTemp[i]->begin(i);
    soilTemp[i]->setNormDataVal(25.0);

    soilMois[i]->begin(i);
    soilMois[i]->setNormDataVal(50);

    pumpSch[i]->begin(i);
    pumpSch[i]->setTimezone(TIMEZONE);

    pumpHandler[i]->begin(i, &twi);
    pumpHandler[i]->setMode(manual);
  }

  airHudm.begin(4);
  airTemp.begin(4);

  lightSch.begin(5);
  lightSch.setTimezone(TIMEZONE);
  lightHandler.begin(5, &twi);
  lightHandler.setMode(manual);

  wateringSch.begin(8);
  wateringSch.setTimezone(TIMEZONE);
  wateringHandler.begin(8, &twi);
  wateringHandler.setMode(manual);

  fanHandler.begin(7, &twi);
  fanHandler.setMode(manual);

  jalouseHandler.begin(9, &twi);
  jalouseHandler.setMode(manual);

  heatHandler.begin(6, &twi);
  heatHandler.setMode(manual);
}

void loop() 
{
  Blynk.run();
  timerBl.run();

  for(int i = 0; i < 4; i++)
  {
    pumpHandler[i]->handle();
  }

  lightHandler.handle();
  fanHandler.handle();
  jalouseHandler.handle();
  heatHandler.handle();
  wateringHandler.handle();
}

float getSoilTempAvg()
{
  float temp = 0;
  for(int i = 0; i < 4; i++)
  {
    temp += soilTemp[i]->getData();
  }
  return temp/4;
}

void pushData()
{

  Blynk.virtualWrite(V1, soilTemp[0]->getData());
  Blynk.virtualWrite(V2, soilMois[0]->getData());

  Blynk.virtualWrite(V3, soilTemp[1]->getData());
  Blynk.virtualWrite(V4, soilMois[1]->getData());

  Blynk.virtualWrite(V5, soilTemp[2]->getData());
  Blynk.virtualWrite(V6, soilMois[2]->getData());

  Blynk.virtualWrite(V7, soilTemp[3]->getData());
  Blynk.virtualWrite(V8, soilMois[3]->getData());

  Blynk.virtualWrite(V9, airTemp.getData());
  Blynk.virtualWrite(V10, airHudm.getData());

  if(lightHandler.getState() == 0)
  {
    led.off();
  }
  else led.on();

  Blynk.virtualWrite(V38, getSoilTempAvg());
}

void sync()
{
  Blynk.syncAll();
}

BLYNK_WRITE(V11)
{
  pumpHandler[0]->manualEnable(param.asInt());
  DEBUG(String("Manual 0") + String(param.asInt()))
}

BLYNK_WRITE(V12)
{
 if(param.asInt() == 0)
  {
    pumpHandler[0]->setMode(manual);
    DEBUG(String("Mode manual 0"));
  }
  else
  {
    DEBUG(String("Mode mois 0"));
    pumpHandler[0]->setMode(soilMois[0]);
  } 
}

BLYNK_WRITE(V13)
{
  Schedule sch;
  TimeInputParam t(param);
  if(t.hasStartTime() && t.hasStopTime())
  {
      for(int i = 0; i <= 7; i++)
      {
          if(t.isWeekdaySelected(i) == 1) sch.days[i - 1] = 1;
          else sch.days[i - 1] = 0;
      }
      sch.minuteStart = t.getStartMinute();
      sch.hourStart = t.getStartHour();
      sch.minuteStop = t.getStopMinute();
      sch.hourStop = t.getStopHour();
      pumpSch[0]->setSchedule(sch);
      pumpHandler[0]->setMode(pumpSch[0]);
  }
  for(int i = 0; i < 7; i++)
  {
    DEBUG(sch.days[i])
  }
  DEBUG(sch.hourStart)
  DEBUG(sch.minuteStart)
  DEBUG(sch.hourStop)
  DEBUG(sch.minuteStop)
}


BLYNK_WRITE(V14)
{
  pumpHandler[1]->manualEnable(param.asInt());
  DEBUG(String("Manual 1") + String(param.asInt()))
}

BLYNK_WRITE(V15)
{
  if(param.asInt() == 0)
  {
    pumpHandler[1]->setMode(manual);
    DEBUG(String("Mode manual 1"));
  }
  else
  {
    DEBUG(String("Mode mois 1"));
    pumpHandler[1]->setMode(soilMois[1]);
  } 
}

BLYNK_WRITE(V16)
{
  Schedule sch;
  TimeInputParam t(param);
  if(t.hasStartTime() && t.hasStopTime())
  {
      for(int i = 0; i <= 7; i++)
      {
          if(t.isWeekdaySelected(i) == 1) sch.days[i - 1] = 1;
          else sch.days[i - 1] = 0;
      }
      sch.minuteStart = t.getStartMinute();
      sch.hourStart = t.getStartHour();
      sch.minuteStop = t.getStopMinute();
      sch.hourStop = t.getStopHour();
      pumpSch[1]->setSchedule(sch);
      pumpHandler[1]->setMode(pumpSch[1]);
  }
  for(int i = 0; i < 7; i++)
  {
    DEBUG(sch.days[i])
  }
  DEBUG(sch.hourStart)
  DEBUG(sch.minuteStart)
  DEBUG(sch.hourStop)
  DEBUG(sch.minuteStop)
}


BLYNK_WRITE(V17)
{
  pumpHandler[2]->manualEnable(param.asInt());
  DEBUG(String("Manual 2") + String(param.asInt()))
}

BLYNK_WRITE(V18)
{
  if(param.asInt() == 0)
  {
    pumpHandler[2]->setMode(manual);
    DEBUG(String("Mode manual 2"));
  }
  else
  {
    DEBUG(String("Mode mois 2"));
    pumpHandler[2]->setMode(soilMois[2]);
  } 
}

BLYNK_WRITE(V19)
{
  Schedule sch;
  TimeInputParam t(param);
  if(t.hasStartTime() && t.hasStopTime())
  {
      for(int i = 0; i <= 7; i++)
      {
          if(t.isWeekdaySelected(i) == 1) sch.days[i - 1] = 1;
          else sch.days[i - 1] = 0;
      }
      sch.minuteStart = t.getStartMinute();
      sch.hourStart = t.getStartHour();
      sch.minuteStop = t.getStopMinute();
      sch.hourStop = t.getStopHour();
      pumpSch[2]->setSchedule(sch);
      pumpHandler[2]->setMode(pumpSch[2]);
  }
  for(int i = 0; i < 7; i++)
  {
    DEBUG(sch.days[i])
  }
  DEBUG(sch.hourStart)
  DEBUG(sch.minuteStart)
  DEBUG(sch.hourStop)
  DEBUG(sch.minuteStop)
}


BLYNK_WRITE(V20)
{
  pumpHandler[3]->manualEnable(param.asInt());
  DEBUG(String("Manual 3") + String(param.asInt()))
}

BLYNK_WRITE(V21)
{
  if(param.asInt() == 0)
  {
    pumpHandler[3]->setMode(manual);
    DEBUG(String("Mode manual 3"));
  }
  else
  {
    DEBUG(String("Mode mois 3"));
    pumpHandler[3]->setMode(soilMois[3]);
  } 
}

BLYNK_WRITE(V22)
{
  Schedule sch;
  TimeInputParam t(param);
  if(t.hasStartTime() && t.hasStopTime())
  {
      for(int i = 0; i <= 7; i++)
      {
          if(t.isWeekdaySelected(i) == 1) sch.days[i - 1] = 1;
          else sch.days[i - 1] = 0;
      }
      sch.minuteStart = t.getStartMinute();
      sch.hourStart = t.getStartHour();
      sch.minuteStop = t.getStopMinute();
      sch.hourStop = t.getStopHour();
      pumpSch[3]->setSchedule(sch);
      pumpHandler[3]->setMode(pumpSch[3]);
  }
  for(int i = 0; i < 7; i++)
  {
    DEBUG(sch.days[i])
  }
  DEBUG(sch.hourStart)
  DEBUG(sch.minuteStart)
  DEBUG(sch.hourStop)
  DEBUG(sch.minuteStop)
}

BLYNK_WRITE(V23)      //normval
{
  for(int i = 0; i < 4; i++)
  {
    soilMois[i]->setNormDataVal(param.asFloat());
  }
}

BLYNK_WRITE(V25)
{
  for(int i = 0; i < 4; i++)
  {
    soilTemp[i]->setNormDataVal(param.asFloat());
  }
  DEBUG(String("Heat norm-val ") + String(param.asFloat()))
}

BLYNK_WRITE(V26)
{
  if(param.asInt() == 0)
  {
    heatHandler.setMode(manual);
    DEBUG(String("Mode manual heat"));
  }
  else
  {
    DEBUG(String("Mode temp heat"));
    heatHandler.setMode(*soilTemp);
  } 
}

BLYNK_WRITE(V27)
{
  heatHandler.manualEnable(param.asInt());
  DEBUG(String("Manual heat") + String(param.asInt()))
}

BLYNK_WRITE(V28)
{
  fanHandler.manualEnable(param.asInt());
  DEBUG(String("Manual fan") + String(param.asInt()))
}

BLYNK_WRITE(V31)
{
  jalouseHandler.manualEnable(1);
  DEBUG(String("Manual jalouse") + String(1))
}

BLYNK_WRITE(V32)
{
  jalouseHandler.manualEnable(0);
  DEBUG(String("Manual jalouse") + String(0))
}

BLYNK_WRITE(V33)
{
  wateringHandler.manualEnable(param.asInt());
  DEBUG(String("Manual watering") + String(param.asInt()))
}

BLYNK_WRITE(V34)
{
  Schedule sch;
  TimeInputParam t(param);
  if(t.hasStartTime() && t.hasStopTime())
  {
    for(int i = 0; i <= 7; i++)
    {
        if(t.isWeekdaySelected(i) == 1) sch.days[i - 1] = 1;
        else sch.days[i - 1] = 0;
    }
    sch.minuteStart = t.getStartMinute();
    sch.hourStart = t.getStartHour();
    sch.minuteStop = t.getStopMinute();
    sch.hourStop = t.getStopHour();
    wateringSch.setSchedule(sch);
    wateringHandler.setMode(&wateringSch);
  }
  for(int i = 0; i < 7; i++)
  {
    DEBUG(sch.days[i])
  }
  DEBUG(sch.hourStart)
  DEBUG(sch.minuteStart)
  DEBUG(sch.hourStop)
  DEBUG(sch.minuteStop)
}

BLYNK_WRITE(V35)
{
  lightHandler.manualEnable(param.asInt());
  DEBUG(String("Manual watering") + String(param.asInt()))
}

BLYNK_WRITE(V36)
{
  Schedule sch;
  TimeInputParam t(param);
  if(t.hasStartTime() && t.hasStopTime())
  {
    for(int i = 0; i <= 7; i++)
    {
        if(t.isWeekdaySelected(i) == 1) sch.days[i - 1] = 1;
        else sch.days[i - 1] = 0;
    }
    sch.minuteStart = t.getStartMinute();
    sch.hourStart = t.getStartHour();
    sch.minuteStop = t.getStopMinute();
    sch.hourStop = t.getStopHour();
    lightSch.setSchedule(sch);
    lightHandler.setMode(&lightSch);
    for(int i = 0; i < 7; i++)
    {
      DEBUG(sch.days[i])
    }
    DEBUG(sch.hourStart)
    DEBUG(sch.minuteStart)
    DEBUG(sch.hourStop)
    DEBUG(sch.minuteStop)
  }
  else 
  {
    lightHandler.setMode(manual);
    DEBUG("Light no schedule");
  }
}

BLYNK_CONNECTED()
{
  Blynk.syncAll();
}
