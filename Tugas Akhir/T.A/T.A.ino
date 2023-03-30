//28 88 AC 48 F6 5 3C 5
//28 EF 85 48 F6 16 3C F1
//28 DD 70 48 F6 BC 3C F
//28 60 A1 48 F6 64 3C FF
//28 BC CD B 0 0 0 3E

void (* err)(void) = 0;
void (* rep)(void) = 198;

#define TINY_GSM_MODEM_SIM800
#define SerialMon Serial
#define DHTTYPE DHT22
#define b1 A8
#define b2 A9
#define b3 A10
#define b4 2
#define TPIN 37
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13
#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
Adafruit_INA219 ps (0x41);
Adafruit_INA219 cr (0x40);
SoftwareSerial SerialAT(11, 10);
PZEM004Tv30 ptc(&Serial2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(TPIN);
DallasTemperature sensors(&oneWire);
DeviceAddress S1 = { 0x28, 0xBC, 0xCD, 0xB, 0x0, 0x0, 0x0, 0x3E }; //BELAKANG BAWAH
DeviceAddress S2 = { 0x28, 0xEF, 0x85, 0x48, 0xF6, 0x16, 0x3C, 0xF1 }; //DEPAN ATAS
DeviceAddress S3 = { 0x28, 0xDD, 0x70, 0x48, 0xF6, 0xBC, 0x3C, 0xF }; //BELAKANG ATAS
DeviceAddress S4 = { 0x28, 0x60, 0xA1, 0x48, 0xF6, 0x64, 0x3C, 0xFF }; //DEPAN BAWAH

int dummy = 1;
int LED1 = 23;
int LED2 = 25;
int LED3 = 27;
int DHT1 = 35; // Temperature Sensor 1 (rak atas)
int DHT2 = 33; // Temperature Sensor 2 (luar)
int FK = 6;
int FS = 7;
int heat = 5;

float KR;
float KL;
float SBL;
float SBD;
float STL;
float STG;

float RK;
float RS;

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

unsigned long t;
unsigned long tSend;
unsigned long tSer;
unsigned long tserSeb;
unsigned long tsendSeb;
unsigned long tSeb = 0;

unsigned long dt;
unsigned long senddata = 30000;
unsigned long ser = 1000;

float outKendaliSuhu;

float err1;
float errInt;
float errDer;
float errSeb = 0;

int pwm = 0;

int p1;
int p2;
int p3;
int p4;

String RSTATE;
String RON = "ON";
String ROFF = "OFF";
String FSTATE ;
String FON = "ON";
String FOFF = "OFF";

const char apn[]      = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

char server[]   = "cec-unsrat.com";
int port = 80;

TinyGsm        modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, port);

DHT dht1(DHT1, DHTTYPE);
DHT dht2(DHT2, DHTTYPE);

float SH = 63;
float KPS = 6.8; //6.10 stable
float KIS = 0.00000001; //0.00000001 low steady state error
float KDS = 0.0;

void setup()
{
  SerialMon.begin(115200);
  delay(10);
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(FK, OUTPUT);
  pinMode(FS, OUTPUT);
  pinMode(heat, OUTPUT);
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);

  dht1.begin();
  dht2.begin();
  sensors.begin();
  ps.begin();
  cr.begin();

    SerialAT.begin(9600);
    delay(6000);
  
    SerialMon.println("Initializing modem...");
    lcd.setCursor (0,0);
    lcd.print("Mencari...");
    modem.restart();
    SerialMon.println(modem.getModemInfo());
    lcd.setCursor (0,1);
    lcd.print(modem.getModemInfo());
    delay(1000);
    lcd.clear();
  
    SerialMon.print("Waiting for network...");
    lcd.setCursor (0,0);
    lcd.print("Wait for net...");
    if (!modem.waitForNetwork()) {
      lcd.clear();
      SerialMon.println(" fail");
      lcd.setCursor (0,0);
      lcd.print("fail");
      modem.restart();
      digitalWrite(LED1, 0);
      delay(10000);
      lcd.clear();
      err();
    }
    lcd.setCursor (0,1);
    lcd.print("success");
    SerialMon.println(" success");
    delay(1000);
    lcd.clear();
  
    if (modem.isNetworkConnected()) {
      lcd.clear();
      SerialMon.println("Network connected");
      lcd.setCursor (0,0);
      lcd.print("Net connected");
      digitalWrite(LED1, 1);
      delay(1000);
      lcd.clear();
    }
  
    SerialMon.print("Connecting ");
    SerialMon.print(apn);
    lcd.setCursor (0,0);
    lcd.print("To ");
    lcd.print(apn);
    lcd.print("...");
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      lcd.clear();
      SerialMon.println(" fail");
      lcd.setCursor (0,0);
      lcd.print("fail");
      modem.restart();
      digitalWrite(LED2, 0);
      delay(10000);
      lcd.clear();
      err();
    }
    lcd.setCursor (0,1);
    lcd.print("Success");
    SerialMon.println(" success");
    delay(1000);
    lcd.clear();
  
    if (modem.isGprsConnected()) {
      lcd.clear();
      lcd.setCursor (0,0);
      lcd.print("GPRS connected");
      digitalWrite(LED2, 1);
      SerialMon.println("GPRS connected");
      delay(1000);
      lcd.clear();
    }
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
  KR = dht1.readHumidity();
  KL = dht2.readHumidity();
  sensors.requestTemperatures(); 
  STL = sensors.getTempC(S1);
  STG = sensors.getTempC(S2);
  SBL = sensors.getTempC(S3);
  SBD = sensors.getTempC(S4);
  RK = (KR + KL) / 2;
  RS = (SBL + SBD + STG + STL) / 4;
  p1 = digitalRead(b1);
  p2 = digitalRead(b2);
  p3 = digitalRead(b3);
  p4 = digitalRead(b4);
//  tSer = millis();

    tSend = millis();
    if(t - tsendSeb >= senddata) {
       tsendSeb = tSend;
    if(client.connected()) {
      lcd.clear();
      lcd.print("Send data....");
      Serial.println("Send data....");
      digitalWrite(LED3, 1);
      String contentType = "application/x-www-form-urlencoded";
      String data =  String(SBL) + " " + String(SBD) + " " + String(STL) + " " + String(STG) + " " + String(RK) + " " + String(RS) + " " + String(psV) + " " + String(psC) + " " + String(ptot) + " " + String(crV) + " " + String(crC) + " " + String(VAC) + " " + String(IAC);
      http.post("/box/post.php/", contentType, "api_key=anjing&arr=" + data);
  
      int statusCode = http.responseStatusCode();
      String response = http.responseBody();
      lcd.print(statusCode);
      Serial.println(statusCode);
      lcd.setCursor(0,1);
      lcd.print(response);
      Serial.print(response);
      digitalWrite(LED3, 0);
    }
    if(!client.connect(server, port)) {
      lcd.print("Fail. ");
      err();
    }
    lcd.clear();
  }
  outKendaliSuhu = kendaliSuhu(STL);
  if (outKendaliSuhu > 255) {
    outKendaliSuhu = 255;
  }
  if (outKendaliSuhu < 0) {
    outKendaliSuhu = 0;
  }
  if (STL > 70) {
    digitalWrite(heat, 1);
    analogWrite(FK, 255);
    RSTATE=ROFF;
  }
  if (STL < 70) {
    digitalWrite(heat, 0);
    analogWrite(FK, 0);
    RSTATE=RON;
  }
  if (RK > 30 && RK < 80 || RK >= 80) {
    pwm = 255;
    analogWrite(FK, pwm);
    FSTATE=FON;
  }
  if (RK <= 30) {
    pwm = 0;
    analogWrite(FK, pwm);
    FSTATE=FOFF;
  }
  analogWrite(FS, outKendaliSuhu);

//  if (t - tserSeb >= ser) {
//    tserSeb = tSer;
//    Serial.print(SBL);
//    Serial.print(",");
//    Serial.print(SBD);
//    Serial.print(",");
//    Serial.print(STL);
//    Serial.print(",");
//    Serial.print(STG);
//    Serial.print(",");
//    Serial.print(RS);
//    Serial.print(",");
//    Serial.print(KR);
//    Serial.print(",");
//    Serial.print(KL);
//    Serial.print(",");
//    Serial.print(RK);
//    Serial.print(",");
//    Serial.print(psV);
//    Serial.print(",");
//    Serial.print(psC);
//    Serial.print(",");
//    Serial.print(psP);
//    Serial.print(",");
//    Serial.print(crV);
//    Serial.print(",");
//    Serial.print(crC);
//    Serial.print(",");
//    Serial.print(crP);
//    Serial.print(",");
//    Serial.print(VAC);
//    Serial.print(",");
//    Serial.print(IAC);
//    Serial.print(",");
//    Serial.print(PAC);
//    Serial.print(",");
//    Serial.print(ptot);
//    Serial.print(",");
//    Serial.print(RSTATE);
//    Serial.print(",");
//    Serial.print(FSTATE);
//    Serial.println(",");
//  }
}

float kendaliSuhu(int inputSuhu) {
  t = millis();
  dt = (t - tSeb);

  err1 = SH - inputSuhu;
  errInt += (err1 + errSeb) * dt / 2;
  errDer = (err1 - errSeb) / dt;

  float out = (KPS * err1) + (KIS * errInt) + (KDS * errDer) ;
  tSeb = t;
  errSeb = err1;
  return out;
}
