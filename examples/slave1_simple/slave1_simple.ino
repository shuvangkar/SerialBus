#include <SoftwareSerial.h>
#include "SerialBus.h"

void runSlaveSM(byte funCode);

#define RS485_RX D6
#define RS485_TX D5
#define RS485_DIR D4

#define SLAVE_ID 1

uint8_t state;
byte functionCode;

SoftwareSerial mySerial(RS485_RX, RS485_TX);
Serialbus slave(mySerial, SLAVE_ID);

byte dataArr[5] = {4, 6, 8, 10, 12};
byte rcvArray[5];
void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  slave.setDirectionPin(RS485_DIR);
  Serial.print(F("Own Addr: "));Serial.println(slave._slaveId);
  state = 0;
}

void loop()
{
  
  functionCode = slave.getFunctionCode();
  runSlaveSM(functionCode);
  //.println(mySerial.available());
  //delay(1000);
}
void runSlaveSM(byte funCode)
{
  if (funCode > 0)
  {
    Serial.println("-------------------------");
    Serial.print(F("Function Code: "));Serial.println(funCode);
    switch (functionCode)
    {
      case 10:
//        slave.readData(rcvArray);
        break;
      case 11:
        slave.response(dataArr,sizeof(dataArr));
        break;
        case 12:
          slave.response(dataArr,sizeof(dataArr));
        break;
      default:
      functionCode = 0;
    }
    
  }

}

