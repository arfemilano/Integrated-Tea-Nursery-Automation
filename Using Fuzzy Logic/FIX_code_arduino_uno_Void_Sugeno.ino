//Libraries
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <stdlib.h>
#include "fis_header.h"
char array1[] = "K.M.T.E.";  // CHANGE THIS AS PER YOUR NEED 
char array2[] = "Air Temp   = ";     // CHANGE THIS AS PER YOUR NEED 
char array3[] = "Air Humid  = ";   // CHANGE THIS AS PER YOUR NEED
char array4[] = "Soil Humid = ";     // CHANGE THIS AS PER YOUR 

LiquidCrystal_I2C lcd(0x27, 20, 4); // CHANGE THE 0X27 ADDRESS TO YOUR SCREEN ADDRESS IF NEEDED
SoftwareSerial espSerial(0,1);//RX,TX

//Constants
#define DHT1PIN 8 //KANAN
//#define DHT2PIN 9 //GAKEPAKE
#define DHT3PIN 10//KIRI
//#define DHT4PIN 11//GAKEPAKE
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht1(DHT1PIN, DHTTYPE); //KANAN
//DHT dht2(DHT2PIN, DHTTYPE); //GAKEPAKE
DHT dht3(DHT3PIN, DHTTYPE); //KIRI
//DHT dht4(DHT4PIN, DHTTYPE); //GAKEPAKE

//Variables
const int relay = 2;
const int relay2 = 3;
int sensorPin1 = A0;//KANAN
int sensorPin2 = A1;//KANAN
int sensorPin3 = A2;//KIRI
int sensorPin4 = A3;//KIRI
int nilai1;
int nilai2;
int nilai3;
int nilai4;
float hum1;
float temp1;
float hum3;
float temp3;
String str;
String kondisirelay;
String kondisirelay2;

//================================FUZZY SUGENO====================================
// Number of inputs to the fuzzy inference system
const int fis_gcI = 3;
// Number of outputs to the fuzzy inference system
const int fis_gcO = 2;
// Number of rules to the fuzzy inference system
const int fis_gcR = 27;
FIS_TYPE g_fisInput[fis_gcI];
FIS_TYPE g_fisOutput[fis_gcO];

// Support functions for Fuzzy Inference System
// Trapezoidal Member Function
FIS_TYPE fis_trapmf(FIS_TYPE x, FIS_TYPE* p)
{
    FIS_TYPE a = p[0], b = p[1], c = p[2], d = p[3];
    FIS_TYPE t1 = ((x <= c) ? 1 : ((d < x) ? 0 : ((c != d) ? ((d - x) / (d - c)) : 0)));
    FIS_TYPE t2 = ((b <= x) ? 1 : ((x < a) ? 0 : ((a != b) ? ((x - a) / (b - a)) : 0)));
    return (FIS_TYPE) min(t1, t2);
}
// Triangular Member Function
FIS_TYPE fis_trimf(FIS_TYPE x, FIS_TYPE* p)
{
    FIS_TYPE a = p[0], b = p[1], c = p[2];
    FIS_TYPE t1 = (x - a) / (b - a);
    FIS_TYPE t2 = (c - x) / (c - b);
    if ((a == b) && (b == c)) return (FIS_TYPE) (x == a);
    if (a == b) return (FIS_TYPE) (t2*(b <= x)*(x <= c));
    if (b == c) return (FIS_TYPE) (t1*(a <= x)*(x <= b));
    t1 = min(t1, t2);
    return (FIS_TYPE) max(t1, 0);
}
FIS_TYPE fis_prod(FIS_TYPE a, FIS_TYPE b)
{
    return (a * b);
}
FIS_TYPE fis_probor(FIS_TYPE a, FIS_TYPE b)
{
    return (a + b - (a * b));
}
FIS_TYPE fis_sum(FIS_TYPE a, FIS_TYPE b)
{
    return (a + b);
}
FIS_TYPE fis_array_operation(FIS_TYPE *array, int size, _FIS_ARR_OP pfnOp)
{
    int i;
    FIS_TYPE ret = 0;
    if (size == 0) return ret;
    if (size == 1) return array[0];
    ret = array[0];
    for (i = 1; i < size; i++)
    {
        ret = (*pfnOp)(ret, array[i]);
    }
    return ret;
}
// Data for Fuzzy Inference System 
// Pointers to the implementations of member functions
_FIS_MF fis_gMF[] =
{
    fis_trapmf, fis_trimf
};
// Count of member function for each Input
int fis_gIMFCount[] = { 3, 3, 3 };
// Count of member function for each Output 
int fis_gOMFCount[] = { 2, 2 };
// Coefficients for the Input Member Functions
FIS_TYPE fis_gMFI0Coeff1[] = { 23, 26, 40, 40 };
FIS_TYPE fis_gMFI0Coeff2[] = { 10, 19, 30 };
FIS_TYPE fis_gMFI0Coeff3[] = { 0, 0, 10, 18 };
FIS_TYPE* fis_gMFI0Coeff[] = { fis_gMFI0Coeff1, fis_gMFI0Coeff2, fis_gMFI0Coeff3 };
FIS_TYPE fis_gMFI1Coeff1[] = { 0, 0, 30, 45 };
FIS_TYPE fis_gMFI1Coeff2[] = { 65, 75, 100, 100 };
FIS_TYPE fis_gMFI1Coeff3[] = { 30, 40, 64, 78.5 };
FIS_TYPE* fis_gMFI1Coeff[] = { fis_gMFI1Coeff1, fis_gMFI1Coeff2, fis_gMFI1Coeff3 };
FIS_TYPE fis_gMFI2Coeff1[] = { 0, 0, 15, 25 };
FIS_TYPE fis_gMFI2Coeff2[] = { 27, 40, 100, 100 };
FIS_TYPE fis_gMFI2Coeff3[] = { 15, 27, 40 };
FIS_TYPE* fis_gMFI2Coeff[] = { fis_gMFI2Coeff1, fis_gMFI2Coeff2, fis_gMFI2Coeff3 };
FIS_TYPE** fis_gMFICoeff[] = { fis_gMFI0Coeff, fis_gMFI1Coeff, fis_gMFI2Coeff };
// Coefficients for the Output Member Functions
FIS_TYPE fis_gMFO0Coeff1[] = { 0, 0, 0, 0 };
FIS_TYPE fis_gMFO0Coeff2[] = { 0, 0, 0, 1 };
FIS_TYPE* fis_gMFO0Coeff[] = { fis_gMFO0Coeff1, fis_gMFO0Coeff2 };
FIS_TYPE fis_gMFO1Coeff1[] = { 0, 0, 0, 0 };
FIS_TYPE fis_gMFO1Coeff2[] = { 0, 0, 0, 1 };
FIS_TYPE* fis_gMFO1Coeff[] = { fis_gMFO1Coeff1, fis_gMFO1Coeff2 };
FIS_TYPE** fis_gMFOCoeff[] = { fis_gMFO0Coeff, fis_gMFO1Coeff };
// Input membership function set
int fis_gMFI0[] = { 0, 1, 0 };
int fis_gMFI1[] = { 0, 0, 0 };
int fis_gMFI2[] = { 0, 0, 1 };
int* fis_gMFI[] = { fis_gMFI0, fis_gMFI1, fis_gMFI2};
// Output membership function set
int* fis_gMFO[] = {};
// Rule Weights
FIS_TYPE fis_gRWeight[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
// Rule Type
int fis_gRType[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
// Rule Inputs
int fis_gRI0[] = { 1, 1, 1 };
int fis_gRI1[] = { 1, 1, 2 };
int fis_gRI2[] = { 1, 1, 3 };
int fis_gRI3[] = { 1, 2, 1 };
int fis_gRI4[] = { 1, 2, 2 };
int fis_gRI5[] = { 1, 2, 3 };
int fis_gRI6[] = { 1, 3, 1 };
int fis_gRI7[] = { 1, 3, 2 };
int fis_gRI8[] = { 1, 3, 3 };
int fis_gRI9[] = { 2, 1, 1 };
int fis_gRI10[] = { 2, 1, 2 };
int fis_gRI11[] = { 2, 1, 3 };
int fis_gRI12[] = { 2, 2, 1 };
int fis_gRI13[] = { 2, 2, 2 };
int fis_gRI14[] = { 2, 2, 3 };
int fis_gRI15[] = { 2, 3, 1 };
int fis_gRI16[] = { 2, 3, 2 };
int fis_gRI17[] = { 2, 3, 3 };
int fis_gRI18[] = { 3, 1, 1 };
int fis_gRI19[] = { 3, 1, 2 };
int fis_gRI20[] = { 3, 1, 3 };
int fis_gRI21[] = { 3, 2, 1 };
int fis_gRI22[] = { 3, 2, 2 };
int fis_gRI23[] = { 3, 2, 3 };
int fis_gRI24[] = { 3, 3, 1 };
int fis_gRI25[] = { 3, 3, 2 };
int fis_gRI26[] = { 3, 3, 3 };
int* fis_gRI[] = { fis_gRI0, fis_gRI1, fis_gRI2, fis_gRI3, fis_gRI4, fis_gRI5, fis_gRI6, fis_gRI7, fis_gRI8, fis_gRI9, fis_gRI10, fis_gRI11, fis_gRI12, fis_gRI13, fis_gRI14, fis_gRI15, fis_gRI16, fis_gRI17, fis_gRI18, fis_gRI19, fis_gRI20, fis_gRI21, fis_gRI22, fis_gRI23, fis_gRI24, fis_gRI25, fis_gRI26 };
// Rule Outputs
int fis_gRO0[] = { 2, 2 };
int fis_gRO1[] = { 2, 1 };
int fis_gRO2[] = { 2, 2 };
int fis_gRO3[] = { 2, 2 };
int fis_gRO4[] = { 2, 1 };
int fis_gRO5[] = { 2, 2 };
int fis_gRO6[] = { 2, 2 };
int fis_gRO7[] = { 2, 1 };
int fis_gRO8[] = { 2, 2 };
int fis_gRO9[] = { 2, 2 };
int fis_gRO10[] = { 2, 1 };
int fis_gRO11[] = { 2, 2 };
int fis_gRO12[] = { 1, 1 };
int fis_gRO13[] = { 1, 1 };
int fis_gRO14[] = { 1, 1 };
int fis_gRO15[] = { 2, 2 };
int fis_gRO16[] = { 2, 1 };
int fis_gRO17[] = { 2, 2 };
int fis_gRO18[] = { 2, 2 };
int fis_gRO19[] = { 2, 1 };
int fis_gRO20[] = { 2, 2 };
int fis_gRO21[] = { 1, 1 };
int fis_gRO22[] = { 1, 1 };
int fis_gRO23[] = { 1, 1 };
int fis_gRO24[] = { 2, 2 };
int fis_gRO25[] = { 2, 1 };
int fis_gRO26[] = { 2, 2 };
int* fis_gRO[] = { fis_gRO0, fis_gRO1, fis_gRO2, fis_gRO3, fis_gRO4, fis_gRO5, fis_gRO6, fis_gRO7, fis_gRO8, fis_gRO9, fis_gRO10, fis_gRO11, fis_gRO12, fis_gRO13, fis_gRO14, fis_gRO15, fis_gRO16, fis_gRO17, fis_gRO18, fis_gRO19, fis_gRO20, fis_gRO21, fis_gRO22, fis_gRO23, fis_gRO24, fis_gRO25, fis_gRO26 };
// Input range Min
FIS_TYPE fis_gIMin[] = { 0, 0, 0 };
// Input range Max
FIS_TYPE fis_gIMax[] = { 40, 100, 100 };
// Output range Min
FIS_TYPE fis_gOMin[] = { 0, 0 };
// Output range Max
FIS_TYPE fis_gOMax[] = { 1, 1 };
// Data dependent support functions for Fuzzy Inference System
// None for Sugeno
//================================FUZZY SUGENO====================================

void setup()
{
  Serial.begin(115200);
  espSerial.begin(115200);
  dht1.begin();
//  dht2.begin();
  dht3.begin();
//  dht4.begin();
  lcd.init();
  lcd.backlight();
  pinMode(relay,OUTPUT); 
  pinMode(relay2,OUTPUT);
  delay(100);
}

void loop(){
//  gapakeserial();
//  gapakeserial();
  pakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
//  gapakeserial();
  
}

void gapakeserial(){
  delay(200);
  //Read data and store it to variables hum and temp
    int nilai1 = analogRead(sensorPin1);//KANAN
    int nilai2 = analogRead(sensorPin2);//KANAN
    int nilai3 = analogRead(sensorPin3);//KIRI
    int nilai4 = analogRead(sensorPin4);//KIRI
    hum1 = dht1.readHumidity();//KANAN                                 
    hum3 = dht3.readHumidity();//KIRI
    temp1= dht1.readTemperature();//KANAN
    temp3= dht3.readTemperature();//KIRI
    float airtemp = (temp1 + temp3)/2;
    float airhumid = (hum1 + hum3)/2;
    float soilhumid = (nilai1 + nilai2 + nilai3 + nilai4)/4;
    
    float temp1persen = map(temp1, 18.5, 25.3, 17, 22);
    if(temp1persen<0){temp1persen=0;}else if(temp1persen>100){temp1persen=100;}
    float temp3persen = map(temp3, 18.3, 24.5, 17, 22);
    if(temp3persen<0){temp3persen=0;}else if(temp3persen>100){temp3persen=100;}
    float hum1persen = map(hum1, 92, 96.7, 91.5, 99);
    if(hum1persen<0){hum1persen=0;}else if(hum1persen>100){hum1persen=100;}
    float hum3persen = map(hum3, 99.1, 91.8, 91.5, 99);//ini masih belum valid
    if(hum3persen<0){hum3persen=0;}else if(hum3persen>100){hum3persen=100;}

    float nilai1persen = map(nilai1, 958, 560, 0, 100);//958-476(ini nilai aslinya)
    if(nilai1persen<0){nilai1persen=0;}else if(nilai1persen>100){nilai1persen=100;}
    float nilai2persen = map(nilai2, 958, 560, 0, 100);//958-476(ini nilai aslinya)
    if(nilai2persen<0){nilai2persen=0;}else if(nilai2persen>100){nilai2persen=100;}
    float nilai3persen = map(nilai3, 953, 560, 0, 100);
    if(nilai3persen<0){nilai3persen=0;}else if(nilai3persen>100){nilai3persen=100;}
    float nilai4persen = map(nilai4, 953, 560, 0, 100);//983-476(ini nilai aslinya)
    if(nilai4persen<0){nilai4persen=0;}else if(nilai4persen>100){nilai4persen=100;}

    float soilhumidkiripersen = (nilai3persen + nilai4persen)/2;
    float soilhumidkananpersen = (nilai1persen + nilai2persen)/2;
    float soilhumidpersen = (nilai1persen + nilai2persen + nilai3persen + nilai4persen)/4;
    float airtemppersen = (temp1persen+temp3persen)/2;  //+temp2persen+temp4persen
    float airhumidpersen = (hum1persen+hum3persen)/2; //+hum2persen+hum4persen

    g_fisInput[0] = airtemppersen;
    g_fisInput[1] = airhumidpersen;
    g_fisInput[2] = soilhumidpersen;
//    g_fisInput[0] = 20;
//    g_fisInput[1] = 89;
//    g_fisInput[2] = 30;
    g_fisOutput[0] = 0;
    g_fisOutput[1] = 0;
    fis_evaluate();
  
//Jika LOW maka ON, jika HIGH maka akan OFF  => ini khusus buat relay1(active low), kalo relay2 fungsinya normal    
    if(g_fisOutput[0]>=0.5 && g_fisOutput[1]>=0.5){
      digitalWrite(relay,LOW);
      kondisirelay = "Aktif";
      digitalWrite(relay2,HIGH);
      kondisirelay2= "Aktif";
    }
    else if(g_fisOutput[0]>=0.5 && g_fisOutput[1]<0.5){
      digitalWrite(relay,LOW);
      kondisirelay= "Aktif";
      digitalWrite(relay2,LOW);
      kondisirelay2= "Mati";
    }
    else if(g_fisOutput[0]<0.5 && g_fisOutput[1]>=0.5){
      digitalWrite(relay,HIGH);
      kondisirelay= "Mati";
      digitalWrite(relay2,HIGH);
      kondisirelay2= "Aktif";
    }
    else if(g_fisOutput[0]<0.5 && g_fisOutput[1]<0.5){
      digitalWrite(relay,HIGH);
      kondisirelay= "Mati";
      digitalWrite(relay2,LOW);
      kondisirelay2= "Mati";
    }

      //BUAT TESTING
//      digitalWrite(relay,LOW);
//      digitalWrite(relay2,LOW);
//      kondisirelay = "Aktif";
//      kondisirelay2= "Mati";
      
//INI TAMPILAN LCD DATANYA
    lcd.setCursor(6,0);
    lcd.print(array1);
    lcd.setCursor(0,1);
    lcd.print(array2);
    lcd.print(airtemppersen,1);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0,2);
    lcd.print(array3);
    lcd.print(airhumidpersen,1);
    lcd.print("%");
    lcd.setCursor(0,3);
    lcd.print(array4);
    lcd.print(soilhumidpersen,1);
    lcd.print("%");
  
    delay(2000); //Delay 2 sec.
    lcd.clear();
}

void pakeserial()
{
  delay(200);
    int nilai1 = analogRead(sensorPin1);//KANAN
    int nilai2 = analogRead(sensorPin2);//KANAN
    int nilai3 = analogRead(sensorPin3);//KIRI
    int nilai4 = analogRead(sensorPin4);//KIRI
    hum1 = dht1.readHumidity();//KANAN                                 
    hum3 = dht3.readHumidity();//KIRI
    temp1= dht1.readTemperature();//KANAN
    temp3= dht3.readTemperature();//KIRI
    float airtemp = (temp1 + temp3)/2;
    float airhumid = (hum1 + hum3)/2;
    float soilhumid = (nilai1 + nilai2 + nilai3 + nilai4)/4;
    
    float temp1persen = map(temp1, 18.5, 25.3, 17, 22);
    if(temp1persen<0){temp1persen=0;}else if(temp1persen>100){temp1persen=100;}
    float temp3persen = map(temp3, 18.3, 24.5, 17, 22);
    if(temp3persen<0){temp3persen=0;}else if(temp3persen>100){temp3persen=100;}
    float hum1persen = map(hum1, 92, 96.7, 91.5, 99);
    if(hum1persen<0){hum1persen=0;}else if(hum1persen>100){hum1persen=100;}
    float hum3persen = map(hum3, 99.1, 91.8, 91.5, 99);//ini masih belum valid
    if(hum3persen<0){hum3persen=0;}else if(hum3persen>100){hum3persen=100;}

    float nilai1persen = map(nilai1, 958, 560, 0, 100);//958-476(ini nilai aslinya)
    if(nilai1persen<0){nilai1persen=0;}else if(nilai1persen>100){nilai1persen=100;}
    float nilai2persen = map(nilai2, 958, 560, 0, 100);//958-476(ini nilai aslinya)
    if(nilai2persen<0){nilai2persen=0;}else if(nilai2persen>100){nilai2persen=100;}
    float nilai3persen = map(nilai3, 953, 560, 0, 100);
    if(nilai3persen<0){nilai3persen=0;}else if(nilai3persen>100){nilai3persen=100;}
    float nilai4persen = map(nilai4, 953, 560, 0, 100);//983-476(ini nilai aslinya)
    if(nilai4persen<0){nilai4persen=0;}else if(nilai4persen>100){nilai4persen=100;}

    float soilhumidkiripersen = (nilai3persen + nilai4persen)/2;
    float soilhumidkananpersen = (nilai1persen + nilai2persen)/2;
    float soilhumidpersen = (nilai1persen + nilai2persen + nilai3persen + nilai4persen)/4;
    float airtemppersen = (temp1persen+temp3persen)/2;  //+temp2persen+temp4persen
    float airhumidpersen = (hum1persen+hum3persen)/2; //+hum2persen+hum4persen

    g_fisInput[0] = airtemppersen;
    g_fisInput[1] = airhumidpersen;
    g_fisInput[2] = soilhumidpersen;
//    g_fisInput[0] = 20;
//    g_fisInput[1] = 89;
//    g_fisInput[2] = 30;
    g_fisOutput[0] = 0;
    g_fisOutput[1] = 0;
    fis_evaluate();
  
//Jika LOW maka ON, jika HIGH maka akan OFF  => ini khusus buat relay1(active low), kalo relay2 fungsinya normal    
    if(g_fisOutput[0]>=0.5 && g_fisOutput[1]>=0.5){
      digitalWrite(relay,LOW);
      kondisirelay = "Aktif";
      digitalWrite(relay2,HIGH);
      kondisirelay2= "Aktif";
    }
    else if(g_fisOutput[0]>=0.5 && g_fisOutput[1]<0.5){
      digitalWrite(relay,LOW);
      kondisirelay= "Aktif";
      digitalWrite(relay2,LOW);
      kondisirelay2= "Mati";
    }
    else if(g_fisOutput[0]<0.5 && g_fisOutput[1]>=0.5){
      digitalWrite(relay,HIGH);
      kondisirelay= "Mati";
      digitalWrite(relay2,HIGH);
      kondisirelay2= "Aktif";
    }
    else if(g_fisOutput[0]<0.5 && g_fisOutput[1]<0.5){
      digitalWrite(relay,HIGH);
      kondisirelay= "Mati";
      digitalWrite(relay2,LOW);
      kondisirelay2= "Mati";
    }
      //BUAT TESTING
//      digitalWrite(relay,LOW);
//      digitalWrite(relay2,LOW);
//      kondisirelay = "Aktif";
//      kondisirelay2= "Mati";
    
//    str=String("#")+String(soilhumidpersen)+String(",")+String(airhumidpersen)+String(",")+String(airtemppersen)+String(",")+String(kondisirelay)+String(",")+String(kondisirelay2)+String(",")+String(g_fisOutput[0])+String(",")+String(g_fisOutput[1])+String("#")+String("\n");
//    Serial.println(str);

    Serial.print("#");Serial.print(soilhumidpersen);Serial.print(",");Serial.print(airhumidpersen);Serial.print(",");Serial.print(airtemppersen);Serial.print(",");
    Serial.print(kondisirelay);Serial.print(",");Serial.print(kondisirelay2);Serial.print(",");Serial.print(g_fisOutput[0]);Serial.print(",");Serial.print(g_fisOutput[1]);Serial.print("#");//POKOK SAMPE SINI
    Serial.print(soilhumidkiripersen);Serial.print(",");Serial.print(soilhumidkananpersen);Serial.print(",");
    Serial.print(hum3persen);Serial.print(",");Serial.print(hum1persen);Serial.print(",");
    Serial.print(temp3persen);Serial.print(",");Serial.print(temp1persen);Serial.print("#");
    Serial.print(hum3);Serial.print(",");Serial.print(hum1);Serial.print(",");//INI YANG REAL, BUKAN PERSENTASE
    Serial.print(temp3);Serial.print(",");Serial.print(temp1);Serial.println("#");//INI YANG REAL, BUKAN PERSENTASE
//    Serial.println("###########################");
//    Serial.print(soilhumidpersen);Serial.print("||");Serial.print(soilhumid);Serial.print("||");Serial.print(nilai1);Serial.print(",");Serial.print(nilai2);Serial.print(",");Serial.print(nilai3);Serial.print(",");Serial.println(nilai4);
//    Serial.print(airhumidpersen);Serial.print("||");Serial.print(airhumid);Serial.print("||");Serial.print(hum1);Serial.print(",");Serial.println(hum3);
//    Serial.print(airtemppersen);Serial.print("||");Serial.print(airtemp);Serial.print("||");Serial.print(temp1);Serial.print(",");Serial.println(temp3);
//    Serial.println("###########################");
    
    Serial.flush();

//INI TAMPILAN LCD DATANYA
    lcd.setCursor(6,0);
    lcd.print(array1);
    lcd.setCursor(0,1);
    lcd.print(array2);
    lcd.print(airtemppersen,1);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0,2);
    lcd.print(array3);
    lcd.print(airhumidpersen,1);
    lcd.print("%");
    lcd.setCursor(0,3);
    lcd.print(array4);
    lcd.print(soilhumidpersen,1);
    lcd.print("%");
  
    delay(2000); //Delay 2 sec.
    lcd.clear();
}

// Fuzzy Inference System 
void fis_evaluate()
{
    FIS_TYPE fuzzyInput0[] = { 0, 0, 0 };
    FIS_TYPE fuzzyInput1[] = { 0, 0, 0 };
    FIS_TYPE fuzzyInput2[] = { 0, 0, 0 };
    FIS_TYPE* fuzzyInput[fis_gcI] = { fuzzyInput0, fuzzyInput1, fuzzyInput2, };
    FIS_TYPE fuzzyOutput0[] = { 0, 0 };
    FIS_TYPE fuzzyOutput1[] = { 0, 0 };
    FIS_TYPE* fuzzyOutput[fis_gcO] = { fuzzyOutput0, fuzzyOutput1, };
    FIS_TYPE fuzzyRules[fis_gcR] = { 0 };
    FIS_TYPE fuzzyFires[fis_gcR] = { 0 };
    FIS_TYPE* fuzzyRuleSet[] = { fuzzyRules, fuzzyFires };
    FIS_TYPE sW = 0;

    // Transforming input to fuzzy Input
    int i, j, r, o;
    for (i = 0; i < fis_gcI; ++i)
    {
        for (j = 0; j < fis_gIMFCount[i]; ++j)
        {
            fuzzyInput[i][j] =
                (fis_gMF[fis_gMFI[i][j]])(g_fisInput[i], fis_gMFICoeff[i][j]);
        }
    }

    int index = 0;
    for (r = 0; r < fis_gcR; ++r)
    {
        if (fis_gRType[r] == 1)
        {
            fuzzyFires[r] = 1;
            for (i = 0; i < fis_gcI; ++i)
            {
                index = fis_gRI[r][i];
                if (index > 0)
                    fuzzyFires[r] = fis_prod(fuzzyFires[r], fuzzyInput[i][index - 1]);
                else if (index < 0)
                    fuzzyFires[r] = fis_prod(fuzzyFires[r], 1 - fuzzyInput[i][-index - 1]);
                else
                    fuzzyFires[r] = fis_prod(fuzzyFires[r], 1);
            }
        }
        else
        {
            fuzzyFires[r] = 0;
            for (i = 0; i < fis_gcI; ++i)
            {
                index = fis_gRI[r][i];
                if (index > 0)
                    fuzzyFires[r] = fis_probor(fuzzyFires[r], fuzzyInput[i][index - 1]);
                else if (index < 0)
                    fuzzyFires[r] = fis_probor(fuzzyFires[r], 1 - fuzzyInput[i][-index - 1]);
                else
                    fuzzyFires[r] = fis_probor(fuzzyFires[r], 0);
            }
        }

        fuzzyFires[r] = fis_gRWeight[r] * fuzzyFires[r];
        sW += fuzzyFires[r];
    }

    if (sW == 0)
    {
        for (o = 0; o < fis_gcO; ++o)
        {
            g_fisOutput[o] = ((fis_gOMax[o] + fis_gOMin[o]) / 2);
        }
    }
    else
    {
        for (o = 0; o < fis_gcO; ++o)
        {
            FIS_TYPE sWI = 0.0;
            for (j = 0; j < fis_gOMFCount[o]; ++j)
            {
                fuzzyOutput[o][j] = fis_gMFOCoeff[o][j][fis_gcI];
                for (i = 0; i < fis_gcI; ++i)
                {
                    fuzzyOutput[o][j] += g_fisInput[i] * fis_gMFOCoeff[o][j][i];
                }
            }

            for (r = 0; r < fis_gcR; ++r)
            {
                index = fis_gRO[r][o] - 1;
                sWI += fuzzyFires[r] * fuzzyOutput[o][index];
            }

            g_fisOutput[o] = sWI / sW;
        }
    }
}
