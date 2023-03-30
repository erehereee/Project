#define DHTTYPE DHT22
#include "DHT.h"
#include <PZEM004Tv30.h>
#include <Adafruit_INA219.h>
PZEM004Tv30 ptc(&Serial2);
Adafruit_INA219 ps (0x41);
Adafruit_INA219 cr (0x40);

int ts1 = 37; // Temperature Sensor 1 (rak atas)
int ts2 = 35; // Temperature Sensor 2 (luar)
int ts3 = 33; // Temperature Sensor 3 (rak bawah)
int FK = 6;
int FS = 7;
int heat = 5;

int rak;
float ras;
int luark;
int luars;
int rbk;
int rbs;
int pwm = 0;

unsigned long t;
unsigned long t1;
unsigned long tSeb = 0;
unsigned long prev = 0;
unsigned long tSeb1 = 0;

unsigned long dt;
unsigned long dt1;

float outKendaliSuhu;
float outKendaliRH;

float err1;
float errInt;
float errDer;
float errSeb = 0;

float err2;
float errInt1;
float errDer1;
float errSeb1 = 0;

float psV = 0;
float psC = 0;
float psP = 0;

float crV = 0;
float crC = 0;
float crP = 0;

float VAC = 0;
float IAC = 0;
float PAC = 0;

float ptot = 0;


DHT dht1(ts1, DHTTYPE);
DHT dht2(ts2, DHTTYPE);
DHT dht3(ts3, DHTTYPE);

float SH = 65;
float KPS = 6.10; //6.10 stable
float KIS = 0.0000001; //0.00000001 low steady state error
float KDS = 0.0;

float SK = 30;
float KPK = 0.0;
float KIK = 0.0;
float KDK = 0.0;

void setup()
{
  Serial.begin(9600);
  pinMode(FK, OUTPUT);
  pinMode(FS, OUTPUT);
  pinMode(heat, OUTPUT);

  dht1.begin();
  dht2.begin();
  dht3.begin();
  ps.begin();
  cr.begin();
  
}

void loop()
{
  psV = ps.getBusVoltage_V();
  psC = ps.getCurrent_mA() / 1000;
  psP = psV * psC;
  crV = cr.getBusVoltage_V();
  crC = cr.getCurrent_mA() / 1000;
  crP = crV * crC;
  VAC = ptc.voltage();
  IAC = ptc.current();
  PAC = ptc.power();
  ptot = psP + crP + PAC;
  rak = dht1.readHumidity();
  ras = dht1.readTemperature();
  luark = dht2.readHumidity();
  luars = dht2.readTemperature();
  rbk = dht3.readHumidity();
  rbs = dht3.readTemperature();

  outKendaliSuhu = kendaliSuhu(ras);
  if(outKendaliSuhu > 255){
    outKendaliSuhu = 255;
  }
  if(outKendaliSuhu < 0){
    outKendaliSuhu = 0;
  }
  if(ras > 70){
    digitalWrite(heat, 1);
    analogWrite(FK, 255);
  }
  if(ras < 70){
    digitalWrite(heat, 0);
    analogWrite(FK, 0);
  }
  if(rak > 30 && rak < 80 || rak >= 80) {
    pwm = 255;
    analogWrite(FK, pwm);
  }
  if(rak <= 30) {
    pwm = 0;
    analogWrite(FK, pwm);
  }


  
  analogWrite(FS, outKendaliSuhu);

  Serial.print(SH);
  Serial.print(",");
  Serial.print(ras);
  Serial.print(",");
  Serial.print(outKendaliSuhu);
  Serial.print(",");
  Serial.print(SK);
  Serial.print(",");
  Serial.print(rak);
  Serial.print(",");
  Serial.print(pwm);
  Serial.print(",");
  Serial.print(psV);
  Serial.print(",");
  Serial.print(psC);
  Serial.print(",");
  Serial.print(psP);
  Serial.print(",");
  Serial.print(crV);
  Serial.print(",");
  Serial.print(crV);
  Serial.print(",");
  Serial.print(crP);
  Serial.print(",");
  Serial.print(VAC);
  Serial.print(",");
  Serial.print(IAC);
  Serial.print(",");
  Serial.print(PAC);
  Serial.print(",");
  Serial.println(ptot);
  delay(1000);
//  Serial.print(",");
//  Serial.print("setpoint RH :");
//  Serial.print(SK);
//  Serial.print(",");
//  Serial.print("presentValue RH :");
//  Serial.print(rak);
//  Serial.print(",");
//  Serial.print("sinyalKontrol RH :");
//  Serial.println(outKendaliRH);
}
