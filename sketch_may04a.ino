#include <SoftwareSerial.h>
#include <stdlib.h>
SoftwareSerial sim808(10, 11); // RX, TX


unsigned long lastActionTime = 0;

char buffer[80];
byte pos = 0;
double latitude=9;
double longitude;

int contentLength = 0;

void resetBuffer() {
  memset(buffer, 0, sizeof(buffer));
  pos = 0;
}

void sendsim808(const char* msg, int waitMs = 500) {
  sim808.println(msg);
 // while(sim808.available()) {
    //Serial.print((char)sim808.read());
   // delay(10);
//  }
  delay(waitMs);
}

void setup()
{
  sim808.begin(9600);
  Serial.begin(9600);
//  post("",1);
  //getGpsData();
  Serial.println("begin");
  char llon[]="longitude";
  char llat[]="latitude";
  getGpsData();
  postLL(llon,longitude);
  postLL(llat,latitude);
}


void loop()
{ 
  
  delay(3000);
}


void getGpsData()
{
 sendsim808("AT");
 sendsim808("AT+CGPSPWR=1");
 sim808.println("AT+CGPSINF=0");
 delay(5000);
 char s[100];
 int i=0;
 while(i<100&&sim808.available())
 {s[i]=(char)sim808.read();
 i++;
 delay(10);
 }
 s[55]='\0';
 Serial.println(s+27);
 char lat[100],lon[100];
 strncpy(lat,s+27,11);
 lat[11]='\0';
 Serial.println(lat);
 strncpy(lon,s+39,11);
 lon[11]='\0';
 Serial.println(lon);
 latitude=atof(lat)/(double)100;
 longitude=atof(lon)/(double)100;
 Serial.println(latitude);
 Serial.println(longitude);
 
 
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
