#include <SoftwareSerial.h>
#include "SerialBus.h"

#define RS485_RX 4
#define RS485_TX 3
#define RS485_DIR 5

SoftwareSerial mySerial(RS485_RX, RS485_TX);
Serialbus master(mySerial);

byte dataArr[32];
byte dataArr2[16];
void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
//  master.setDirectionPin(RS485_DIR);
  Serial.println(F("System RESET"));
  delay(2000);
}

void loop()
{
  Serial.println("---------------------");
  byte len = master.query(1,11,dataArr);
  master.printBuffer(dataArr,len);
  delay(1000);
  Serial.println("---------------------");
  master.query(2,11,dataArr2);
  delay(1000);
}
