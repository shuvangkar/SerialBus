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


/*************Setup Methods****************************/
Serialbus::Serialbus(Stream &port, uint8_t slaveId)
{
  this ->serialPort = &port;
  this ->_slaveId = slaveId;
}

void Serialbus::setDirectionPin(byte Pin)
{
  this -> _dirPin = Pin;
  pinMode(_dirPin,OUTPUT);
  receiveMode();
}
/*************Low Level  Methods************************/
void Serialbus::_transmitBuffer(void *ptr,byte length)
{
  //High/low MAX485 pins for transmit
  sendMode();
  //digitalWrite(_dirPin,HIGH);
  serialPort ->write((byte*)ptr,length);
  receiveMode();
}

void Serialbus::_testRx()
{
  receiveMode();
  while(!serialPort -> available());
  printbusBytes();
}

uint8_t Serialbus::_available()
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

/************************Common Methods***************************/
void Serialbus::reply(void *payload,byte length)
{
  debugPrint(F("Responding : "));debugPrintln(_FunctionCode);
  // uint8_t buffer[MAX_RX_BUFFER];
  // byte packetLen = length+4;
  // buffer[0] = packetLen;
  // buffer[1] = _slaveId;
  // buffer[2] = _FunctionCode;
  // buffer[3] = 0;              //Control bytes
  // memcpy(buffer+4,payload,length);
  // printBuffer(buffer,packetLen);
  // this -> _transmitBuffer(buffer,packetLen);
  // this -> _clearBuffer();

  uint8_t payloadInfo[4];
  payloadInfo[0] = sizeof(payloadInfo)+length;
  payloadInfo[1] = _slaveId;
  payloadInfo[2] = _FunctionCode; //_FunctionCode store the last received function code
  payloadInfo[3] = 0;
  _transmitBuffer(payloadInfo,sizeof(payloadInfo));
  _transmitBuffer(payload,length);
  // _slaveId = 0;
  _FunctionCode = 0;
  printBuffer(payload,length);
  _clearBuffer();

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

/********************Master Device Methods***********************/

byte Serialbus::query(byte slaveId,byte FunCode,byte *rcvPtr)
{
  //this -> _clearBuffer();
  byte ctrlByte[5]; //query control byte is 5 bytes. 
  ctrlByte[0] = sizeof(ctrlByte);
  ctrlByte[1] = slaveId;
  ctrlByte[2] = FunCode;
  ctrlByte[3] = 0;
  ctrlByte[4] = 0; //Control bytes
  //printBuffer(ctrlByte,sizeof(ctrlByte));
  this -> _transmitBuffer(ctrlByte,sizeof(ctrlByte));
  debugPrint(F("Query Request ID : "));debugPrintln(slaveId);
  uint16_t timeout = 5000;
  byte rcvLen = 0;
  do
  {
    rcvLen = this -> _available();
    //rcvLen = serialPort -> available();
    //debugPrint(F("Available : "));debugPrintln(rcvLen);
    if(rcvLen > 0)
    {
      //debugPrint(F("Available : "));debugPrintln(rcvLen);
      rcvLen = getPayload(rcvPtr,slaveId);
      //this->printbusBytes();
      //return rcvLen;
     
    }
    delay(1);
  }while(--timeout && !rcvLen);
  this -> _clearBuffer();
  return rcvLen;
}

/*******************Slave Methods******************************/
uint8_t Serialbus::getFunctionCode()
{
  byte len =   this -> _available();
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



/**********************Debug Methods******************/
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
void Serialbus::printBuffer(void *ptr,byte length)
{
  for(byte i = 0; i<length; i++)
  {
    debugPrint(*(byte*)(ptr+i));debugPrint(" ");
  }
  debugPrintln();
}

