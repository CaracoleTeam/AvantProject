#include <SoftwareSerial.h>

SoftwareSerial COM(9,10);

void setup(){
  COM.begin(9600);
}

void loop(){
  COM.write('1');
  delay(1000);
}
