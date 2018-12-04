#include <AltSoftSerial.h>
#include <Wire.h>
#include <SoftwareSerial.h>



typedef struct List_Item {
  float acc;
  int accTime;
} ListItem;





float getDistanceFromList(float x[], float y[], int n) {
  float result = 0;
  for (int i = 0; i < n - 1; i++)
  {
    Serial.print(x[i]);
    Serial.print("---");
    Serial.print(y[i]);
    Serial.print("\n");
    float alfa = (y[i + 1] - y[i]) / ((x[i + 1]) - (x[i]));

    float beta = ( (y[i] * x[i + 1]) - (y[i + 1] * x[i])  ) / (x[i + 1] - x[i]);

    float gama = ((alfa / 2) * (x[i] * x[i])) + beta * x[i];


    result += ( ((alfa / 6) * (x[i + 1] * x[i + 1] * x[i + 1])) + (beta / 2) * (x[i + 1] * x[i + 1]) - (gama * x[i + 1])) - ( ((alfa / 6) * (x[i] * x[i] * x[i])) + (beta / 2) * (x[i] * x[i]) - (gama * x[i]));


  }
  return result / 2;

}


SoftwareSerial COM(9, 10);

//Variables
int acc_error = 0;                       //We use this variable to only calculate once the Acc data error
float rad_to_deg = 180 / 3.141592654;    //This value is for pasing from radians to degrees values
float Acc_rawX, Acc_rawY, Acc_rawZ;    //Here we store the raw data read
float Acc_angle_x, Acc_angle_y;          //Here we store the angle value obtained with Acc data
float Acc_angle_error_x, Acc_angle_error_y; //Here we store the initial Acc data error

int passada = 0;




void setup() {
  Wire.begin();                           //begin the wire comunication
  COM.begin(9600);
  Wire.beginTransmission(0x68);           //begin, Send the slave adress (in this case 68)
  Wire.write(0x6B);                       //make the reset (place a 0 into the 6B register)
  Wire.write(0x00);
  Wire.endTransmission(true);             //end the transmission

  Wire.beginTransmission(0x68);           //Start communication with the address found during search.
  Wire.write(0x1C);                       //We want to write to the ACCEL_CONFIG register
  Wire.write(0x10);                       //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);

  Serial.begin(9600);                     //Remember to set this same baud rate to the serial monitor



  /*Here we calculate the acc data error before we start the loop
     I make the mean of 200 values, that should be enough*/
  if (acc_error == 0)
  {
    for (int a = 0; a < 200; a++)
    {
      Wire.beginTransmission(0x68);
      Wire.write(0x3B);                       //Ask for the 0x3B register- correspond to AcX
      Wire.endTransmission(false);
      Wire.requestFrom(0x68, 6, true);

      Acc_rawX = (Wire.read() << 8 | Wire.read()) / 4096.0 ; //each value needs two registres
      Acc_rawY = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
      Acc_rawZ = (Wire.read() << 8 | Wire.read()) / 4096.0 ;


      /*---X---*/
      Acc_angle_error_x = Acc_angle_error_x + ((atan((Acc_rawY) / sqrt(pow((Acc_rawX), 2) + pow((Acc_rawZ), 2))) * rad_to_deg));
      /*---Y---*/
      Acc_angle_error_y = Acc_angle_error_y + ((atan(-1 * (Acc_rawX) / sqrt(pow((Acc_rawY), 2) + pow((Acc_rawZ), 2))) * rad_to_deg));

      if (a == 199)
      {
        Acc_angle_error_x = Acc_angle_error_x / 200;
        Acc_angle_error_y = Acc_angle_error_y / 200;
        acc_error = 1;
      }
    }
  }//end of error calculation
}//end of setup void




int steps = 0 ;

long int    startTemp = 0;
float currentStart = 0;

ListItem accData[15];

int j = 0;
float dados[20];
float tempo[20];



int parada = 0;


int FATOR_DE_CALIBRACAO = 200, INDEX_CALIB = 0;
float totalOfAcc = 0;
float media;
bool calibrado = false;
void loop() {




  //////////////////////////////////////Acc read/////////////////////////////////////


  Wire.beginTransmission(0x68);     //begin, Send the slave adress (in this case 68)
  Wire.write(0x3B);                 //Ask for the 0x3B register- correspond to AcX
  Wire.endTransmission(false);      //keep the transmission and next
  Wire.requestFrom(0x68, 6, true);  //We ask for next 6 registers starting withj the 3B
  /*We have asked for the 0x3B register. The IMU will send a brust of register.
    The amount of register to read is specify in the requestFrom function.
    In this case we request 6 registers. Each value of acceleration is made out of
    two 8bits registers, low values and high values. For that we request the 6 of them
    and just make then sum of each pair. For that we shift to the left the high values
    register (<<) and make an or (|) operation to add the low values.
    If we read the datasheet, for a range of+-8g, we have to divide the raw values by 4096*/
  Acc_rawX = (Wire.read() << 8 | Wire.read()) / 4096.0 ; //each value needs two registres
  Acc_rawY = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
  Acc_rawZ = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
  /*Now in order to obtain the Acc angles we use euler formula with acceleration values
    after that we substract the error value found before*/
  /*---X---*/
  Acc_angle_x = (atan((Acc_rawY) / sqrt(pow((Acc_rawX), 2) + pow((Acc_rawZ), 2))) * rad_to_deg) - Acc_angle_error_x;
  /*---Y---*/
  Acc_angle_y = (atan(-1 * (Acc_rawX) / sqrt(pow((Acc_rawY), 2) + pow((Acc_rawZ), 2))) * rad_to_deg) - Acc_angle_error_y;
  /*Uncoment the rest of the serial prines
    I only print the Y raw acceleration value */
  //Serial.print("AccX raw: ");
  //Serial.print(Acc_rawX);
  //Serial.print("   |   ");
  //Serial.print("AccY raw: ");
  //Serial.print(Acc_rawY);
  //Serial.print("   |   ");
  //Serial.print("AccZ raw: ");


  //CALIBRAÇÂO DO SENSOR


  if (calibrado == false) {
    if (INDEX_CALIB == FATOR_DE_CALIBRACAO) {
      media = totalOfAcc / FATOR_DE_CALIBRACAO;
      dados[0] = media;
      tempo[0] = 0.0;
      Serial.println(media);
      calibrado = true;
    } else {
      totalOfAcc += Acc_rawX;
      INDEX_CALIB++;
    }

  }

  long int    lastDebounceTime = millis();

  passada = lastDebounceTime - startTemp;

  for (int i = 14; i > 0; i--) {
    accData[i] = accData[i - 1];
  }
  ListItem atual;
  atual.acc = Acc_rawX;
  atual.accTime = passada;
  accData[0] = atual;


  if ((accData[7].acc > accData[0].acc + 0.25 and accData[7].acc  > accData[14].acc + 0.25) or accData[7].acc < accData[0].acc - 0.25 and accData[7].acc  < accData[14].acc - 0.25) {
    if (j == 0 ) {
      currentStart = passada;
    }else {
      if(j== 8){
        dados[j] = media;
      }else{
        dados[j] = accData[7].acc;
      }
      
      tempo[j] =  (accData[7].accTime - currentStart)/100;
    }
    j++;
  }

  
  
   if (j == 8) {

    
        Serial.println("PASSO");
        float distancia = getDistanceFromList(dados, tempo, j+1);
        Serial.print("Distancia: ");
        Serial.print(distancia);
        Serial.print("\n");

        float finalPassada = passada/100;
        Serial.println(finalPassada);
        
        
        String stringBuffer = (String)"p"+(String)"E"+(String)"t"+finalPassada+(String)"l"+distancia ;
        char bufToSend[stringBuffer.length()+1];
        stringBuffer.toCharArray(bufToSend,stringBuffer.length()+1);

        
        int dataLength = COM.write(bufToSend);
        COM.write("\n");
        startTemp = lastDebounceTime;
        delay(500);
        j = 0;
      }

//  Serial.print(passada);
//  Serial.print("  --  ");
//  Serial.print(Acc_rawX);
//  Serial.print("\n");


  //    for(int i = 0;i<14;i++){
  //      Serial.print(accData[i].acc);
  //      Serial.print("  --  ");
  //
  //    }
  //    Serial.print("\n");
  //    Serial.println("---------------------------------------------------------------------------");
  //
  //    for(int i = 0;i<10;i++){
  //      Serial.print(dados[i]);
  //      Serial.print("  --  ");
  //
  //    }
  //    Serial.print("\n");
  //    delay(1000);



  //    if(accData[14].acc < -0.8){
  //      Serial.println("DEU PASSO");
  //
  //      float distancia = getDistanceFromList(dados,tempo,j);
  //      j = 0;
  //      currentStart = 0;
  //      Serial.println(distancia);
  //
  //
  //      if(distancia >0){
  //        steps++;
  //        passada = lastDebounceTime - startTemp;
  //        startTemp = lastDebounceTime;
  //        int finalPassada= passada/10/10;
  //
  //
  //        Serial.print("Tempo: ");
  //        Serial.print(passada);
  //        Serial.print("Passos: ");
  //        Serial.print(steps);
  //        Serial.print(" Passada a ser enviada: ");
  //        Serial.print(finalPassada);
  //        Serial.print("\n");
  //
  //
  //        String stringBuffer = (String)"p"+(String)"E"+(String)"t"+finalPassada+(String)"l"+distancia ;
  //        char bufToSend[stringBuffer.length()+1];
  //        stringBuffer.toCharArray(bufToSend,stringBuffer.length()+1);
  //        Serial.println(bufToSend);
  //        int dataLength = COM.write(bufToSend);
  //        COM.write("\n");
  //      }
  //
  //
  //      delay(350);
  //      delay(250);
  //    }






  //if((Acc_rawX >0.9) ){
  //  steps++;
  //  passada = lastDebounceTime - startTemp;
  //  startTemp = lastDebounceTime;
  //  int finalPassada= passada/10/10;
  //
  //
  //  Serial.print("Tempo: ");
  //  Serial.print(passada);
  //  Serial.print("Passos: ");
  //  Serial.print(steps);
  //   Serial.print(" Passada a ser enviada: ");
  //   Serial.print(finalPassada);
  //   Serial.print("\n");
  //
  //  float distance = getDistanceFromList(listOfAcc);
  //  String stringBuffer = (String)"p"+(String)"E"+(String)"t"+finalPassada+(String)"l"+distance ;
  //  char bufToSend[stringBuffer.length()+1];
  //  stringBuffer.toCharArray(bufToSend,stringBuffer.length()+1);
  //  Serial.println(bufToSend);
  //  int dataLength = COM.write(bufToSend);
  //  COM.write("\n");
  //
  //  i =0;
  //  delay(350);
  //}


  //

  //Serial.print("   |   ");
  //Serial.print("AccX angle: ");

  //Serial.print("   |   ");
  //Serial.print("AccY angle: ");
  //Serial.println(Acc_rawZ);
  //Serial.println(" ");




}
