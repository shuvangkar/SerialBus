#include <SoftwareSerial.h>
#include "SerialBus.h"

void runSlaveSM(byte funCode);

#define RS485_RX 4
#define RS485_TX 3
#define RS485_DIR 5

#define SLAVE_ID 1

byte opcode;

SoftwareSerial mySerial(RS485_RX, RS485_TX);
Serialbus slave(mySerial, SLAVE_ID);

byte dataArr[5];
void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
//  slave.setDirectionPin(RS485_DIR);
  Serial.print(F("Own Addr: ")); Serial.println(slave._slaveId);
}

void loop()
{


  opcode = slave.getOpcode();
  runSlaveSM(opcode);
  //.println(mySerial.available());
  //delay(1000);
}
void runSlaveSM(byte funCode)
{
  if (funCode > 0)
  {
    Serial.println("-------------------------");
//    Serial.print(F("Function Code: ")); Serial.println(funCode);
    switch (opcode)
    {
      case 10:
        break;
      case 11:
        populatePayload();
        slave.reply(dataArr, sizeof(dataArr));
        break;
      case 12:
        slave.reply(dataArr, sizeof(dataArr));
        break;
      default:
        break;
    }

  }

}

void populatePayload()
{
  for (int i = 0; i < sizeof(dataArr); i++)
  {
    dataArr[i] = random(0, 30);
  }
}

