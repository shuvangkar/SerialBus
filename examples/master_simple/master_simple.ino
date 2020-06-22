#include <SoftwareSerial.h>
#include "SerialBus.h"

#define RS485_RX 4
#define RS485_TX 3
#define RS485_DIR 5

SoftwareSerial mySerial(RS485_RX, RS485_TX);
Serialbus master(mySerial, 0);

byte payload[10];
byte rcvPacket[10];
byte rcvPacket2[10];
void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  master.setDirectionPin(RS485_DIR);
  Serial.println(F("System RESET"));
  delay(2000);
}

void loop()
{
  Serial.println("---------------------");
  master.query(1,11,rcvPacket);
  master.printBuffer(rcvPacket,sizeof(rcvPacket));
  delay(1000);
  Serial.println("---------------------");
  master.query(2,11,rcvPacket2);
  delay(3000);
}
