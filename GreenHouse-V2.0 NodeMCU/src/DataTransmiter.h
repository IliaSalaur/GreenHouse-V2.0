#pragma once

#ifndef DATATRANSMITER_H
#define DATATRANSMITER_H
#include <Arduino.h>
#include <Wire.h>
#include <Debug.h>
#define REQUEST_TIMEOUT 15


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

class TWI_Master
{
	public:

    static DataZipper<MASTER> *_zipper;
    static int sAddr;
    static void init(int SDA_pin, int SCL_pin,int mAddr)
    {
        Wire.begin(SDA_pin, SCL_pin, mAddr);
    }

    static void send()
    {
        Wire.beginTransmission(sAddr);
        Wire.write(_zipper->_getCommandToSend());
        Wire.endTransmission();
        delay(30);
    }

    static void read()
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
};

class TWI_Slave
{
    static DataZipper<SLAVE> *_unzipper;
    static int mAddr;

    static void receiveEvent(size_t howMany)
    {
        (void)howMany;
        _unzipper->_setCommandReceived(Wire.read());
    }

    static void requestEvent()
    {
        for(byte i = 0; i < 4; i++)
        {
            Wire.write(_unzipper->_getRequestedData().rawData[i]);
        }
    }

    static void init(int sAddr)
    {
        Wire.begin(sAddr);
        Wire.onReceive(receiveEvent);
        Wire.onRequest(requestEvent);
    }

};

template<TwiMode _twiMode>
class I2CHandler
{

};
/*
template<>
class I2CHandler<SLAVE>
{
private:
	uint8_t _command;
	
	static I2CHandler<SLAVE> * pSingletonInstance;

	static void requestEvent()
	{

	}

	static void receiveEvent()
	{

	}
public:



};

I2CHandler<SLAVE> * I2CHandler<SLAVE>::pSingletonInstance = 0;
*/

template<>
class I2CHandler<MASTER>
{
private:
	int _sAddr;
	uint32_t _requestTimeoutTimer = REQUEST_TIMEOUT; 
	RequestedData request;
public:

	I2CHandler()
	{
		_sAddr = 0;
		request.fdata = 0.0;		
	}

	void begin(int SDA_pin, int SCL_pin,int mAddr, int sAddr)
	{
        Wire.begin(SDA_pin, SCL_pin, mAddr);
		_sAddr = sAddr;
    }

	void sendCommand(uint8_t id, uint8_t action, uint8_t argument)
	{
		byte enc = 0b10000000 | (id << 3) | (action << 1) | argument;
		Wire.beginTransmission(_sAddr);
        Wire.write(enc);
        Wire.endTransmission();
        delay(1);
	}

	float getData(bool gettingState = 0)
	{	
		uint32_t requestTimer = millis();
		Wire.requestFrom(_sAddr, 4);
		while(millis() - requestTimer < 700)
		{
			if(Wire.available() > 0)
			{				
				for(byte i = 0; i < 4; i++)
				{
					request.rawData[i] = Wire.read();
				}         
				break;
			}
		}
		delay(1);
		float data = request.fdata;
		request.fdata = 0.0;
		return data;
	}
};
#endif