#include <Arduino.h>
#include <DataTransmiter.h>
#include <IHandler.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
I2CHandler<MASTER> twi;

Scheduler *pumpSch[4];
SoilTemperatureSensor *soilTemp[4];

SoilMoistureSensor *soilMois[4];
Manual manual;

Handler handler = Handler();

WiFiUDP ntpUdp;
NTPClient timeClient(ntpUdp, "europe.pool.ntp.org");
Schedule sch0;

uint32_t timer = 0;

void twiRequestTest();
void handlerTest();

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50);
  twi.begin(D1, D2, 0x12, 0x13);
  timeClient.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin("leonid1", "sl820710");

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(400);
  }
  Serial.println();

  Serial.println(WiFi.localIP());

  for(int i = 0; i < 4; i++)
  {
    soilTemp[i] = new SoilTemperatureSensor(&twi);
    soilMois[i] = new SoilMoistureSensor(&twi);
    pumpSch[i] = new Scheduler(&timeClient);

    soilTemp[i]->begin(i);
    soilTemp[i]->setNormDataVal(25.0);

    soilMois[i]->begin(i);
    soilMois[i]->setNormDataVal(50);

    pumpSch[i]->begin(i);
    pumpSch[i]->setTimezone(3);
  }

  handler.begin(0, &twi);
  handler.setMode(soilTemp[0]);

  for(int i = 0; i < 7; i++)
  {
    sch0.days[i] = 1;
  }

  sch0.hourStart = 21;
  sch0.minuteStart = 0;
  sch0.hourStop = 21;
  sch0.minuteStop = 10;
}

void loop() 
{
  //twiRequestTest();
  handlerTest();
  
}

void schTest()
{
  //pumpSch[0]->setSchedule(sch0);
  //Serial.println(String(pumpSch[0]->getTime().hour) + String(pumpSch[0]->getTime().minute));

  if(pumpSch[0]->isEnabled() == 1)
  {
    Serial.println(String(pumpSch[0]->getTime().hour) + String(pumpSch[0]->getTime().minute) + String("  enabled!"));
  }
}

void twiRequestTest()
{
  Serial.println(soilTemp[0]->getData());
  if(soilTemp[0]->isEnabled())
  {
    Serial.println("enabled");
  }
}

void handlerTest()
{
  if(millis() - timer > 300)
  {
    timer = millis();
    handler.handle();
  }
}