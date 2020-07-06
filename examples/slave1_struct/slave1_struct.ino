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

typedef struct sensor_t 
{
  int index;
  float temp;
  float humidity;
};

sensor_t sensor;

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
        break;
      case 12:
          populatePayload();
          slave.reply(&sensor, sizeof(sensor_t));
        break;
      default:
        break;
    }

  }

}

void populatePayload()
{
  sensor.index = random(1,30);
  sensor.temp = random(25,40);
  sensor.humidity = random(50,70);

  Serial.println(sensor.index);
  Serial.println(sensor.temp);
  Serial.println(sensor.humidity);
}

