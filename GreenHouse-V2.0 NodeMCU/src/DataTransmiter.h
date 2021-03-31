#pragma once

#ifndef DATATRANSMITER_H
#define DATATRANSMITER_H
#include <Arduino.h>
#include <Wire.h>

#define DEBUG(x, y) Serial.println(String(x) + String(y));

enum TwiMode
{
	SLAVE, 
	MASTER
};

struct Command
{
	int id;
	int action;
	int argument;
	uint8_t encCommand;
};



union RequestedData
{
	uint8_t rawData[4];
    float fdata;
};

template<TwiMode _TWIMODE>
class DataZipper
{
	
};

template<>
class DataZipper<MASTER>
{
	private:
	uint8_t enc;
	float _convertedData;
	RequestedData _response;
	
	public:
	DataZipper()
	{
		enc = 0;
		_convertedData = 0;
	};
	
	void sendCommand(uint8_t id, uint8_t action, uint8_t argument)
	{
		enc = 0b10000000 | (id << 3) | (action << 1) | argument;
	}
	
	float getData()
	{
		float data = _response.fdata;
		_response.fdata = 0.0;
        return data;
	}
	
	uint8_t _getCommandToSend()
	{
		return enc;
	}
	
	void _setRequestedData(RequestedData response)
	{
		_response = response;
	}

};

template<>
class DataZipper<SLAVE>
{
	private:
	float _dataToSend;
	uint8_t _command;
	
	public:
	DataZipper()
	{
		_dataToSend = 0;
		_command = 0;
	}
	
	void sendData(float dataToSend)
	{
		_dataToSend = dataToSend;
	}
	
	Command getCommand()
	{
		Command decryptedCommand;
        decryptedCommand.id = _command >> 3;
        decryptedCommand.action = (_command & 0b00000110) >> 1;
        decryptedCommand.argument = (_command & 0b00000001);
        _command = 0;
		return decryptedCommand;
	}
	
	void _setCommandReceived(uint8_t commandReceived)
	{
		_command = commandReceived;
	}
	
	RequestedData _getRequestedData()
	{
		RequestedData request;
		request.fdata = _dataToSend;
        return request;
	}
};

namespace TWI_Master
{
    DataZipper<MASTER> *_zipper;
    int sAddr;
    void init(int SDA_pin, int SCL_pin,int mAddr)
    {
        Wire.begin(SDA_pin, SCL_pin, mAddr);
    }

    void send()
    {
        Wire.beginTransmission(sAddr);
        Wire.write(_zipper->_getCommandToSend());
        Wire.endTransmission();
        delay(10);
    }

    void read()
    {

		uint32_t requestTimer = millis();
		RequestedData request;
        Wire.requestFrom(sAddr, 4);
		while(millis() - requestTimer < 700)
		{
			if(Wire.available() > 0)
			{				
				for(byte i = 0; i < 4; i++)
				{
					request.rawData[i] = Wire.read();
				}         
				_zipper->_setRequestedData(request);
				break;
			}
		}
		Serial.print("Data: ");
		Serial.println(request.fdata);
    }
}

namespace TWI_Slave
{
    DataZipper<SLAVE> *_unzipper;
    int mAddr;

    void receiveEvent(size_t howMany)
    {
        (void)howMany;
        _unzipper->_setCommandReceived(Wire.read());
    }

    void requestEvent()
    {
        for(byte i = 0; i < 4; i++)
        {
            Wire.write(_unzipper->_getRequestedData().rawData[i]);
        }
    }

    void init(int sAddr)
    {
        Wire.begin(sAddr);
        Wire.onReceive(receiveEvent);
        Wire.onRequest(requestEvent);
    }

}

#endif