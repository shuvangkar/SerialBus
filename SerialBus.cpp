#include "SerialBus.h"

#define SERIAL_BUS_DEBUG
#ifdef SERIAL_BUS_DEBUG
#define debugPrint(...) Serial.print(__VA_ARGS__)
#define debugPrintln(...) Serial.println(__VA_ARGS__)
#else
#define debugPrint(...)   __VA_ARGS__
#define debugPrintln(...)  __VA_ARGS__
#endif


#define QUERY_TIMEOUT (uint32_t)2000

#define sendMode()  digitalWrite(_dirPin,HIGH) //Transmit
#define receiveMode() digitalWrite(_dirPin,LOW) //Receive

Serialbus::Serialbus(Stream &port, uint8_t slaveId)
{
  this ->serialPort = &port;
  this ->_slaveId = slaveId;
}

void Serialbus::setDirectionPin(byte Pin)
{
  this ->_dirPin = Pin;
  pinMode(_dirPin,OUTPUT);
  receiveMode();
}

void Serialbus::_transmitBuffer(byte *ptr,byte size)
{
  //High/low MAX485 pins for transmit
  sendMode();
  //digitalWrite(_dirPin,HIGH);
  serialPort ->write(ptr,size);
  receiveMode();
}

uint8_t Serialbus::availableBytes()
{
  byte length = serialPort -> peek();
  byte rcvBytes = serialPort ->available();
  if(rcvBytes>=length)
  {
    return length;
  }
  else
  {
    return 0; 
  }
  //return length;
}

uint8_t Serialbus::getFunctionCode()
{
  byte len =   this -> availableBytes();
  if(len>0)
  {
    //check if master inquires this slave
    if(_bufReadOnce == false)
    {
      byte packetLen = serialPort -> read();
      byte id = serialPort ->read();
      if(id == _slaveId)
      {
        debugPrintln(F("Address Matched"));
        this -> _FunctionCode = serialPort ->read();
        this->payloadLength = packetLen-3;
        _bufReadOnce = true;
      }
      else
      {
        //This is not me. clear buffer
        debugPrintln(F("Address Not Matched"));
        this->payloadLength = 0;
        this->payloadLength = 0;
        this ->_clearBuffer();
      }
    }
    
  }
  return _FunctionCode;
}

void Serialbus::_clearBuffer()
{
  //byte len = serialPort->available();
  while (serialPort->available())
  {
    serialPort->read();
  }
  debugPrintln(F("buffer Cleared"));
}

void Serialbus::printbusBytes()
{
  byte len = serialPort->available();
  if (len > 0)
  {
    for (byte i = 0; i < len; i++)
    {
      byte data = serialPort->read();
      Serial.print(data); Serial.print("  ");
    }
    Serial.println();
  }
}
void Serialbus::_printBuffer(byte *ptr,byte size)
{
  for(byte i = 0; i<size; i++)
  {
    debugPrint(*(ptr+i));debugPrint(" ");
  }
  debugPrintln();
}

