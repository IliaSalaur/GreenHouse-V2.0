#include <Arduino.h>
#include "classes/GroupPlants.h"

#define LIGHT_PIN A0
#define HEAT_PIN A1
#define FAN_PIN A2
#define WATER_PIN 12

#include <DHT.h>
#define DHT_TYPE DHT11
#define DHT_PIN 2
DHT dht(DHT_PIN, DHT_TYPE);

#include "classes/Jalouse.h"
#define STEPPER_PIN_1 4
#define STEPPER_PIN_2 5
#define STEPPER_PIN_3 6
#define STEPPER_PIN_4 7
#define ROTATION 2
Jalouse jalouse(STEPPER_PIN_4, STEPPER_PIN_2, STEPPER_PIN_3, STEPPER_PIN_1);

#include "classes/DataTransmiter.h"
#define ONEWIRE_BUS_PIN 13
OneWire oneWire(ONEWIRE_BUS_PIN);
DallasTemperature tempSoil(&oneWire);
DeviceAddress groupsAddr[4];
GroupPlant *groups[4];
Settings settings[4];
//const int soilpins[4] = {G1_SOILPIN, G2_SOILPIN, G3_SOILPIN, G4_SOILPIN};
//const int pumppins[4] = {G1_PUMPPIN, G2_PUMPPIN, G3_PUMPPIN, G4_PUMPPIN};
//const int id[4] = {1, 2, 3, 4};

DataZipper<SLAVE> unzipper;
uint32_t timer = 0;

void setupGroups();
void printEx();
void setupOthers();
void commandHandler(Command _comm);

float hudmAir = 0.0;
float tempAir = 0.0;

int dir = -1;

bool hudmifierState = 0;
bool lightState = 0;
bool heatState = 0;
bool fanState = 0;
bool jalouseDown = 0; // off - open , down 0 - open

void setup() {

  Serial.begin(115200);
  Serial.setTimeout(50);

  setupGroups();
  setupOthers();

  dht.begin();

  TWI_Slave::init(0x13);
  TWI_Slave::_unzipper = &unzipper;
  TWI_Slave::mAddr = 0x12;
  unzipper.sendData(14.57);

}

void loop() { 
  Serial.println("loop");
  commandHandler(unzipper.getCommand());
  jalouse.handle();

  for(byte i = 0; i < 4; i++)
  {
    groups[i]->requestSoilTemp();
  }
}

void setupGroups()
{
  settings[0].groupid = 0;
  settings[0].pumppin = G1_PUMPPIN;
  settings[0].soilpin = G1_SOILPIN;

  settings[1].groupid = 1;
  settings[1].pumppin = G2_PUMPPIN;
  settings[1].soilpin = G2_SOILPIN;

  settings[2].groupid = 2;
  settings[2].pumppin = G3_PUMPPIN;
  settings[2].soilpin = G3_SOILPIN;

  settings[3].groupid = 3;
  settings[3].pumppin = G4_PUMPPIN;
  settings[3].soilpin = G4_SOILPIN;

  for(byte i = 0; i < 4; i++)
  {
    groups[i] = new GroupPlant(&tempSoil, &groupsAddr[i], &settings[i]);
    groups[i]->begin();
    Serial.println(groups[i]->getExceptions());
    groups[i]->turnPump(0);
  }
  #ifdef CALIBRATION_MODE
  bool calibrated = 0;
  Serial.println("Calibration mode: ");
  while (calibrated == 0)
  {
    if (Serial.available())
    {
      switch (Serial.read())
      {
        case 'a':
          for (byte i = 0; i < 4; i ++ )
          {
            groups[i]->calibrateMoistureAir();
            Serial.println(groups[i]->getExceptions());
          }
          break;

        case 'w':
          for (byte i = 0; i < 4; i ++ )
          {
            groups[i]->calibrateMoistureWater();
            Serial.println(groups[i]->getExceptions());
          }
          break;

        case 'd':
          calibrated = 1;
          break;
      }
    }
  }
#endif
}

void setupOthers()
{
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(WATER_PIN, OUTPUT);

  for(byte i = 12; i < 16; i++)
  {
    digitalWrite(i, LOW);
  }
}

void commandHandler(Command _comm)
{
  if(_comm.encCommand != 0)
  {
    if(_comm.encCommand != 0)
    {
      Serial.println(_comm.id);
      Serial.println(_comm.action);
      Serial.println(_comm.argument);
      Serial.println(_comm.encCommand);
    }
    
    switch(_comm.action)
    {
      case 0b0:
        if(_comm.id < 4)
        {
          unzipper.sendData(groups[_comm.id]->getSoilTemp());
        }
        else if(_comm.id == 4)
        {
          float t = dht.readTemperature();
          if (!isnan(t)) unzipper.sendData(t);
        }
        break;

      case 0b1:
          if(_comm.id < 4)
          {
            unzipper.sendData(groups[_comm.id]->getSoilMoisture());
          }
          else if(_comm.id == 4)
          {
            float f = dht.readHumidity();
            if (!isnan(f)) unzipper.sendData(f);
          }
        break;

      case 0b10:
        switch (_comm.id)
        {
        case 5: unzipper.sendData(digitalRead(LIGHT_PIN)); break;
        case 6: unzipper.sendData(digitalRead(HEAT_PIN)); break;
        case 7: unzipper.sendData(digitalRead(FAN_PIN)); break;
        case 8: unzipper.sendData(digitalRead(WATER_PIN)); break;
        case 9:
          if(jalouse.getState() == CLOSE) unzipper.sendData(0.0);
          else unzipper.sendData(1.0);
          break;
          
        default:
          if(_comm.id < 4) unzipper.sendData(groups[_comm.id]->getPumpState());
          break;
        }
        break;  

      case 0b11:
        switch(_comm.id)
        {
        case 5: digitalWrite(LIGHT_PIN,_comm.argument); break;
        case 6: digitalWrite(HEAT_PIN,_comm.argument); break;
        case 7: digitalWrite(FAN_PIN,_comm.argument); break;
        case 8: digitalWrite(WATER_PIN,_comm.argument); break;
        case 9:
          if(_comm.argument == 1) jalouse.open();
          else jalouse.close();
          break;
        }
        break;
    }
  }
}
