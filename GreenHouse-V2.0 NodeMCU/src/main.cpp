#include <Arduino.h>
#include <DataTransmiter.h>
DataZipper<MASTER> zipper;

uint32_t timer = 0;
Command comm;

void setup() {

  Serial.begin(115200);
  Serial.setTimeout(50);
  TWI_Master::init(4, 5, 0x12);
  TWI_Master::_zipper = &zipper;
  TWI_Master::sAddr = 0x13;
}

void loop() {
  if(Serial.available() > 0)
  {
    char c = Serial.read();
    
    switch(c)
    {
      case 'i':
        zipper.sendCommand(9, 3, 0);
        TWI_Master::send();
        break;

      case 'a':
        zipper.sendCommand(9, 3, 1);
        TWI_Master::send();
        break;

      case 'g':
        zipper.sendCommand(0, 1, 0);
        TWI_Master::send();
        break;

      case 's':
        zipper.sendCommand(0, 0, 0);
        TWI_Master::send();
        break;
    }
    

    TWI_Master::read();
    Serial.println(zipper.getData());
  }
}