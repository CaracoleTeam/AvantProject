#include <SoftwareSerial.h>

SoftwareSerial COM(9,10);


void setup(){
  COM.begin(9600);
}

void loop(){

  int t = 3;
  int  l = 5;
  
  COM.write("t");
  COM.write(t);
  COM.write("l");
  COM.write(l);
  delay(1000);
}
