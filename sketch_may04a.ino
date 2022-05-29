#include "DHT.h"
#include <Servo.h> // include servo library to use its related functions
#include <SoftwareSerial.h>
#include <stdlib.h>
SoftwareSerial sim808(10, 11); // RX, TX


unsigned long lastActionTime = 0;

char buffer[80];
byte pos = 0;
double latitude=9;
double longitude;

int contentLength = 0;


#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
bool hasBeenPrinted = false;

#define Servo_PWM 6 // A descriptive name for D6 pin of Arduino to provide PWM signal
Servo MG995_Servo;  // Define an instance of of Servo with the name of "MG995_Servo"


#include "MQ135.h"
///The load resistance on the board
#define RLOAD 102.0
///Calibration resistence at atmospheric CO2 level
#define RZERO 10000.99
///Atmospheric CO2 level for calibration purposes
#define ATMOCO2 397.13
int sensorIn = A0;

const byte ledPin1 = 13;
const byte ledPin2 = 12;
const byte sensorPin1 = 3;
const byte sensorPin2 = 2;

int counter=0;
int state1 = LOW;
int state2 = LOW;
int lastst1=LOW;
int lastst2=LOW;
int state1count=0;
int state2count=0;
int ok=0;


void setup(){
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  Serial.begin(9600);
  MG995_Servo.attach(Servo_PWM);  // Connect D6 of Arduino with PWM signal pin of servo motor
  dht.begin();


  sim808.begin(9600);
}

void nr_oameni(){
   state1=digitalRead(sensorPin1);
  state2=digitalRead(sensorPin2);
  
  //digitalWrite(ledPin1,state1);
  //digitalWrite(ledPin2,state2);

  if(state1!=lastst1 && state1==HIGH){
    	state1count++;    
    	//digitalWrite(ledPin1,state1);    
  }
  if(state2!=lastst2 && state2==HIGH){
    	state2count++;
    	//digitalWrite(ledPin2,state2);   
  }
  
  if(state1count==(state2count+1)){
  	ok=1;
   // Serial.print("OK pentru + este:");
    Serial.println(ok);
  }
  else state1count=state2count;
  
  if(state2count==(state1count+1)){
   	ok=1;
  //  Serial.print("OK pentru - este:");
    Serial.println(ok);
  }
  else state1count=state2count;
  
  if(ok==1 && lastst2==LOW &&state2==HIGH && state1==HIGH){
    counter++;
    ok=0;
    delay(300);
    Serial.print("////////Counter oameni:");
    Serial.println(counter);
  }
  else if(ok==1 && lastst1==LOW && state2==HIGH && state1==HIGH){
    if(counter>0){counter--; 
                  Serial.print("////////Counter oameni:");}
    else Serial.println("Bus empty");
    
    Serial.println(counter);
    
    ok=0;
    delay(300);
  }
  Serial.print("Counter senzor 1:");
    Serial.println(state1count);
  Serial.print("Counter senzor 2:");
    Serial.println(state2count);
  Serial.print("////////Counter oameni:");
    Serial.println(counter);
  lastst1=state1;
  lastst2=state2;
  Serial.println("////////////////////////////////");
}



  float h;
  float t;

void DHT_Servo(){
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hic ;//= dht.computeHeatIndex(t, h, false);
  if(hasBeenPrinted==false)
  {
    Serial.print(F("  Humidity: "));
    Serial.print(h);
    Serial.print(F("%\n  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(F("\n  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
  }
  if(t>24)
  {
    MG995_Servo.write(200);
  }
  hasBeenPrinted = true;
}
float ppm;
void MQ(){
  MQ135 gasSensor = MQ135(A0);
   ppm = gasSensor.getPPM();
  Serial.print ("ppm: ");
  Serial.println (ppm);
}


void sendsim808(const char* msg, int waitMs = 500) {
  sim808.println(msg);
 // while(sim808.available()) {
    //Serial.print((char)sim808.read());
   // delay(10);
//  }
  delay(waitMs);
}




void getGpsData()
{
 sendsim808("AT");
 sendsim808("AT+CGPSPWR=1");
 while(sim808.available())
 sim808.read();
 sim808.println("AT+CGPSINF=0");
 delay(5000);
 char s[300];
 int i=0;
 while(i<200&&sim808.available())
 {s[i]=(char)sim808.read();
 i++;
 delay(10);
 }
 delay(100);
 s[100]='\0';
 char lat[100],lon[100];
 strncpy(lat,s+27,11);
 lat[11]='\0';
 strncpy(lon,s+39,11);
 lon[11]='\0';
 latitude=atof(lat)/(double)100;
 longitude=atof(lon)/(double)100;

 
 
  Serial.println("02");
 delay(5000);
 
 
 
}

void postNum(char* nume,int val)
{
    char url2[] = "AT+HTTPPARA=\"URL\",\"http://ec2-18-212-247-203.compute-1.amazonaws.com:5050/?name="; //eu
    char url[150];
    strcpy(url,url2);
    Serial.println(nume);
    strcat(url,nume);
    char v[]="&value=";
    Serial.println(v);
    strcat(url,v);
    Serial.println(url);
    char buf[100];
    itoa(val,buf,10);
    buf[9]='\0';
    Serial.println(buf);
    strcat(url,buf);
    strcat(url,"\"");
    Serial.println(url);
Serial.println(url);
    
    
 sendsim808("AT");
 sendsim808("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
 sendsim808("AT+SAPBR=3,1,\"APN\",\"live.vodafone.com\"");
 sendsim808("AT+SAPBR=3,1,\"USER\",\"live\"");
 sendsim808("AT+SAPBR=1,1\n",3000);
 sendsim808("AT+HTTPINIT");  
 sendsim808("AT+HTTPPARA=\"CID\",1");
 sendsim808(url);
 sendsim808("AT+HTTPACTION=0");
 Serial.println(url);
}

void postLL(char* nume,double val)
{
  
    char url2[] = "AT+HTTPPARA=\"URL\",\"http://ec2-18-212-247-203.compute-1.amazonaws.com:5050/?name="; //eu
    char url[150];
    strcpy(url,url2);
    Serial.println(url);
    strcat(url,nume);
    Serial.println(url);
    char v[]="&value=";
    char buf[100];
    strcat(url,v);
    Serial.println(url);
    int l=val;
    itoa(l,buf,10);
    strcat(url,buf);
    Serial.println(url);
    l=val*100;
    l=l%100;
    char p[]=".";
    strcat(url,p);
    Serial.println(url);
    char buf2[100];
    itoa(l,buf2,10);
    strcat(url,buf2);
    strcat(url,"\"");
    Serial.println(url);
    
 sendsim808("AT");
 sendsim808("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
 sendsim808("AT+SAPBR=3,1,\"APN\",\"live.vodafone.com\"");
 sendsim808("AT+SAPBR=3,1,\"USER\",\"live\"");
 sendsim808("AT+SAPBR=1,1\n",3000);
 sendsim808("AT+HTTPINIT");  
 sendsim808("AT+HTTPPARA=\"CID\",1");
 sendsim808(url);
 sendsim808("AT+HTTPACTION=0");
}


void loop(){
  nr_oameni();
  DHT_Servo();
  MQ();
  //postNum("people",counter);
  //Serial.println(millis());
  unsigned long m=millis();
  m=m/10000*10000;
  Serial.println(m);
  if(m%20000==0&&m>=20000){
  getGpsData();
  postLL("latitudine",latitude);
  postLL("longitudine",longitude);
  postLL("humidity",h);
  postLL("temperature",t);
  postLL("ppm",ppm);
  postNum("people",counter);
  }
}
