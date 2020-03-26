#ifndef _ARDUINI_485_
#define _ARDUINI_485_

#include "Arduino.h"

typedef enum errorCode_t 
{
  NO_ERROR = 0,
  PACKET_OVERFLOW = 1,
  
};

class Serialbus
{
  public:
    Serialbus(Stream &port, uint8_t slaveId);
    void setDirectionPin(byte Pin);
    void printbusBytes();
    uint8_t getFunctionCode();// return function Code if address matched |clear buffer if address not matches
    uint8_t availableBytes();
    
    template <typename T_port>
    void begin(T_port* port, long baudRate);
    
    void response(byte *dataPtr,byte  dataLen);
    byte getPayload(byte *dataPtr, byte SlaveID);
    byte query(byte slaveId,byte FunCode,byte *rcvPtr);

    byte *dataPtr;

    uint8_t _slaveId;
    byte payloadLength;

     void _transmitBuffer(byte *ptr,byte size);
     void _testRx();
     void _printBuffer(byte *ptr,byte size);
  private:
    Stream *serialPort;
    uint8_t _dirPin;
    bool _bufReadOnce = false;
    byte _FunctionCode;


    void _clearBuffer();
    
   
};

template <typename T_port>
void begin(T_port* port, long baudRate)
{
  port->begin(baudRate);
}

#endif
