#include "SerialBus.h"

#define SERIAL_BUS_DEBUG
#ifdef SERIAL_BUS_DEBUG
#define debugPrint(...) Serial.print(__VA_ARGS__)
#define debugPrintln(...) Serial.println(__VA_ARGS__)
#else
#define debugPrint(...)   __VA_ARGS__
#define debugPrintln(...)  __VA_ARGS__
#endif

#define MAX_RX_BUFFER (byte)64
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

void Serialbus::_testRx()
{
  receiveMode();
  while(!serialPort -> available());
  printbusBytes();
}

uint8_t Serialbus::availableBytes()
{
  byte length = serialPort -> peek();
  //debugPrint(F("peek : "));debugPrint(length);
  byte rcvBytes = serialPort ->available();
  //debugPrint(F(" | Actual : "));debugPrintln(rcvBytes);
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

byte Serialbus::query(byte slaveId,byte FunCode,byte *rcvPtr)
{
  byte ctrlByte[5];
  ctrlByte[0] = sizeof(ctrlByte);
  ctrlByte[1] = slaveId;
  ctrlByte[2] = FunCode;
  ctrlByte[3] = 0;
  ctrlByte[4] = 0; //Control bytes
  //_printBuffer(ctrlByte,sizeof(ctrlByte));
  this -> _transmitBuffer(ctrlByte,sizeof(ctrlByte));
  debugPrint(F("Query Request ID : "));debugPrintln(slaveId);
  uint16_t timeout = 5000;
  byte rcvLen = 0;
  do
  {
    rcvLen = this -> availableBytes();
    //rcvLen = serialPort -> available();
    //debugPrint(F("Available : "));debugPrintln(rcvLen);
    if(rcvLen > 0)
    {
      //debugPrint(F("Available : "));debugPrintln(rcvLen);
      rcvLen = getPayload(rcvPtr,slaveId);
      //this->printbusBytes();
      //return rcvLen;
      this -> _clearBuffer();
    }
    delay(1);
  }while(--timeout && !rcvLen);
  return rcvLen;
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
        this->payloadLength = packetLen;
        _bufReadOnce = true;
      }
      else
      {
        //This is not me. clear buffer
        debugPrintln(F("Address Not Matched"));
        this->payloadLength = 0;
        this ->_clearBuffer();
      }
    }
    
  }
  return _FunctionCode;
}
void Serialbus::response(byte *dataPtr,byte  dataLen)
{
  debugPrint(F("Responding : "));debugPrintln(_FunctionCode);
  uint8_t buffer[MAX_RX_BUFFER];
  byte packetLen = dataLen+4;
  buffer[0] = packetLen;
  buffer[1] = _slaveId;
  buffer[2] = _FunctionCode;
  buffer[3] = 0;
  memcpy(buffer+4,dataPtr,dataLen);
  _printBuffer(buffer,packetLen);
  this -> _transmitBuffer(buffer,packetLen);
  this -> _clearBuffer();

  // byte ctrlByte[4];
  // ctrlByte[0] = dataLen+ sizeof(ctrlByte);
  // ctrlByte[1] = _slaveId;
  // ctrlByte[2] = _FunctionCode;
  // ctrlByte[3] = 0;
  // _printBuffer(ctrlByte,sizeof(ctrlByte));
  // this -> _transmitBuffer(ctrlByte,sizeof(ctrlByte));
  // _printBuffer(dataPtr,dataLen);
  // this -> _transmitBuffer(dataPtr,dataLen);
  // this -> _clearBuffer();

/*
  uint8_t buffer[MAX_RX_BUFFER];
  buffer[0] = slaveId;
  buffer[1] = _funCode;
  buffer[2] = dataLen;// If calculate crc add 2 bytes more here
  memcpy(buffer+3,dataPtr,dataLen);
  printBuffer(buffer,dataLen+3);
  this -> _transmitBuffer(buffer,dataLen+3);
  this ->clearBuffer();//clear buffer for accidental byte ramaining
  */
}
byte  Serialbus::getPayload(byte *dataPtr, byte SlaveID)
{
  //Checksum here, then proceed to the 

  byte ctrlByte[4];
  byte temp;
  for(byte i = 0; i<4; i++)
  {
    /*
    byte 0->Packet Length
    byte 1->Slave Address
    byte 2->Function Code
    byte 3-> Control Code
    */
    ctrlByte[i] = serialPort -> read();
  }

  if(ctrlByte[1] == SlaveID)
  {
    byte  packetLen = ctrlByte[0] - 4;
    for (byte i = 0; i < packetLen; ++i)
    {
      *(dataPtr+i) = serialPort -> read();
    }

    return packetLen;
  }
  else
  {
    return 0;
  }

}

void Serialbus::_clearBuffer()
{
  //byte len = serialPort->available();
  _bufReadOnce = false;
  _FunctionCode = 0;
  payloadLength = 0;
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

