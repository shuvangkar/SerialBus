#include <SoftwareSerial.h>
#include "SerialBus.h"

#define RS485_RX 4
#define RS485_TX 3
#define RS485_DIR 5

SoftwareSerial mySerial(RS485_RX, RS485_TX);
Serialbus master(mySerial);

char str[64];
char str2[64];
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
  byte len = master.query(1,13,str);
  Serial.println(str);
  delay(1000);
  Serial.println("---------------------");
  len = master.query(2,13,str2);
  Serial.println(str2);
  delay(1000);
}
void printSensor()
{
  
}
void printSensor2()
{
  
}

