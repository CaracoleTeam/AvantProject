#include <SoftwareSerial.h>


SoftwareSerial COM(10,11);
char value;

void setup(){
  COM.begin(9600);  
  Serial.begin(9600);
}

void loop(){
  if(COM.available()>0){
    value = (char) COM.read();
  }
  Serial.println(value);
  
}
