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

    template <typename T_port>
    void begin(T_port* port, long baudRate);
    
    void reply(void *payload,byte length);
    byte getPayload(byte *dataPtr, byte SlaveID);


    uint8_t getFunctionCode();
    byte query(byte slaveId,byte FunCode,byte *rcvPtr);

    void printbusBytes();
    void printBuffer(void *ptr,byte length);

    //byte *dataPtr;
    uint8_t _slaveId;
    byte payloadLength;
  private:
    Stream *serialPort;
   
    uint8_t _dirPin;
    bool _bufReadOnce = false;
    byte _FunctionCode;

    uint8_t _available();
    void _transmitBuffer(void *ptr,byte length);
    void _testRx();
    void _clearBuffer();
    
   
};

template <typename T_port>
void begin(T_port* port, long baudRate)
{
  port->begin(baudRate);
}

#endif
