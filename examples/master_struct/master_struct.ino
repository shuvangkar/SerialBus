#include <SoftwareSerial.h>
#include "SerialBus.h"

#define RS485_RX 4
#define RS485_TX 3
#define RS485_DIR 5

SoftwareSerial mySerial(RS485_RX, RS485_TX);
Serialbus master(mySerial);

typedef struct sensor_t 
{
  int index;
  float temp;
  float humidity;
};
sensor_t sensor;
sensor_t sensor2;
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
  byte len = master.query(1,12,&sensor);
  printSensor();
  delay(1000);
  Serial.println("---------------------");
  len = master.query(2,12,&sensor2);
  printSensor2();
  delay(1000);
}
void printSensor()
{
  Serial.println(sensor.index);
  Serial.println(sensor.temp);
  Serial.println(sensor.humidity);
}
void printSensor2()
{
  
}

