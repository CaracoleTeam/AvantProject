#include <SoftwareSerial.h>


typedef struct passo{
    float stepTime;
    float stepLenght;
}Passo;

SoftwareSerial COM(10,11);


void setup(){
  COM.begin(9600);  
  Serial.begin(9600);
}

void loop(){
  
  
  if(COM.available()>0){
    Passo passoEsquerdo;
    switch((char)COM.read()){
      case 't':
        passoEsquerdo.stepTime =COM.read();
        break;
      case 'l':
        passoEsquerdo.stepLenght = COM.read();
        break;
        
    }
    Serial.println(passoEsquerdo.stepLenght);
    
  }
  
  
  
}
