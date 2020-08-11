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
  digitalWrite(_dirPin,LOW); //Receive mode
  // _rcvMode();
  // receiveMode();
}

void Serialbus::setDirFunctions(funPtr_t sendFun, funPtr_t rcvFun)
{
  _sendFun = sendFun;
  _rcvFun = rcvFun;
  _rcvFun();
}

// void Serialbus::_sendMode()
// {
//   Serial.println(F("<PIN_SEND_MODE>"));
//   digitalWrite(_dirPin,HIGH);
// }
// void Serialbus::_rcvMode()
// {
//   Serial.println(F("<PIN_RCV_MODE>"));
//   digitalWrite(_dirPin,LOW);
// }
/*************Low Level  Methods************************/
void Serialbus::_transmitBuffer(void *ptr,byte length)
{
  if(_sendFun)
  {
    // Serial.println(F("<SEND_MODE>"));
    _sendFun();
  }
  else
  {
    if(_dirPin !=255)
    {
      // Serial.println(F("<PIN_SEND_MODE>"));
      digitalWrite(_dirPin,HIGH);
    }
  }

  serialPort ->write((byte*)ptr,length);

  if(_rcvFun)
  {
    // Serial.println(F("<RCV_MODE>"));
    _rcvFun();
  }
  else
  {
    if(_dirPin !=255)
    {
      // Serial.println(F("<PIN_RCV_MODE>"));
      digitalWrite(_dirPin,LOW);
    }
  }
}

byte *Serialbus::_readBuffer(byte *buf, byte len)
{
  byte *ptr = buf;
  for(byte i = 0; i<len ; i++)
  {
    ptr[i] = serialPort->read();
  }
  return buf;
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
  
  if(rcvBytes>=length)
  {
    return length;
  }
  else
  {
    return 0; 
  }
}

int8_t Serialbus::_polling()
{
  if(serialPort -> available())
  {
    byte len = serialPort -> read();
    // Serial.print(F("Len:"));Serial.println(len);
    if(len == 5)
    {
      return 1;
      //printbusBytes();
    }
    else
    {
      _clearBuffer();
      // return 0;
    }
  }
  return 0;
}

uint8_t Serialbus::getOpcode()
{
  if(_polling()>0)
  {
    // printbusBytes();
    byte buf[4];
    for(byte i = 0; i< 4; i++)
    {
      buf[i] = _timed_read();
    }

    // printBuffer(buf,sizeof(buf));
    _clearBuffer();
    if(buf[0] == _slaveId)
    {
      Serial.println(F("Match"));
      _FunctionCode = buf[1];
      return buf[1];
    }
    else
    {
      Serial.println(F("Not Match"));
    }
  }
  return 0;
}

int Serialbus::_timed_read()
{
  int c;
  _start_millis = millis();
  long current_millis;
  do
  {
    c = serialPort -> read();
    if (c > 0 )
    {
      return c;
    }
    current_millis = millis();
  } while ((current_millis - _start_millis) < 10);
  return -1;
}

// byte *Serialbus::_read_bytes(byte *buf)
// {
//   byte *p = buf;
//   int b = _timed_read();
//   while(b>=0)
//   {
//     *p = (byte)b;
//     p++;
//     b = _timed_read();
//   }
//   return buf;
// }

void Serialbus::_clearBuffer()
{
  while (serialPort->available())
  {
    _timed_read();
    // serialPort->read();
  }
  // debugPrintln(F("buffer Cleared"));
  // debugPrintln(F("<>"));
}

/************************Common Methods***************************/
void Serialbus::reply(void *payload,byte length)
{
  debugPrint(F("Responding : "));debugPrint(_slaveId);debugPrint('|');debugPrintln(_FunctionCode);

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

byte  Serialbus::getPayload(void *dataPtr, byte SlaveID)
{
  //Checksum here, then proceed to the 

  byte ctrlByte[4];
  byte temp;
  _readBuffer(ctrlByte, sizeof(ctrlByte));

  if(ctrlByte[1] == SlaveID)
  {
    //have to match opcode also
    byte  packetLen = ctrlByte[0] - 4;
    _readBuffer((byte*)dataPtr,packetLen);

    return packetLen;
  }
  else
  {
    return 0;
  }

}

/********************Master Device Methods***********************/

byte Serialbus::query(byte slaveId,byte FunCode,void *rcvPtr)
{
  //this -> _clearBuffer();
  byte ctrlByte[5]; //query control byte is 5 bytes.

  // ctrlByte[0] = slaveId;
  // ctrlByte[1] = sizeof(ctrlByte);
  // ctrlByte[2] = FunCode;
  // ctrlByte[3] = 0;
  // ctrlByte[4] = 0; 

  ctrlByte[0] = sizeof(ctrlByte);
  ctrlByte[1] = slaveId;
  ctrlByte[2] = FunCode;
  ctrlByte[3] = 0;
  ctrlByte[4] = 0; //Control bytes

  //printBuffer(ctrlByte,sizeof(ctrlByte));
  this -> _transmitBuffer(ctrlByte,sizeof(ctrlByte));
  debugPrint(F("Query Request ID : "));debugPrintln(slaveId);
  long ms1 = millis();
  byte rcvLen = 0;
  do
  {
    rcvLen = this -> _available();
    //rcvLen = serialPort -> available();
    // debugPrint(F("Available : "));debugPrintln(rcvLen);
    if(rcvLen > 0)
    {
      //debugPrint(F("Available : "));debugPrintln(rcvLen);
      rcvLen = getPayload(rcvPtr,slaveId);
      if(rcvLen>0)
      {
        break;
      }
      //this->printbusBytes();
      // return rcvLen;
     
    }
    // delay(1);
  }while((millis()-ms1<3000));
  this -> _clearBuffer();
  return rcvLen;
}



/**********************Debug Methods******************/
void Serialbus::printbusBytes()
{
  while(serialPort->available())
  {
    byte data = (byte)_timed_read();
    Serial.print(data); Serial.print("  ");
  }
  Serial.println();
}
void Serialbus::printBuffer(void *ptr,byte length)
{
  for(byte i = 0; i<length; i++)
  {
    debugPrint(*(byte*)(ptr+i));debugPrint(" ");
  }
  debugPrintln();
}




  // if(len>0)
  // {
  //   //check if master inquires this slave
  //   if(_bufReadOnce == false)
  //   {
  //     byte packetLen = serialPort -> read();
  //     byte id = serialPort ->read();
  //     if(id == _slaveId)
  //     {
  //       debugPrintln(F("Address Matched"));
  //       this -> _FunctionCode = serialPort ->read();
  //       this->payloadLength = packetLen;
  //       _bufReadOnce = true;
  //     }
  //     else
  //     {
  //       //This is not me. clear buffer
  //       debugPrintln(F("Address Not Matched"));
  //       this->payloadLength = 0;
  //       this ->_clearBuffer();
  //     }
  //   }
    
  // }
  // return _FunctionCode;