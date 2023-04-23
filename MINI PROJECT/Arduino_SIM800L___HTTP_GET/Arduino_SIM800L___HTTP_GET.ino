#define TINY_GSM_MODEM_SIM800
#define SerialMon Serial
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(11, 10);

const char apn[]      = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

char server[]   = "cec-unsrat.com";
int port = 80;

TinyGsm        modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, port);

int dummy = 1;

void setup()
{
  SerialMon.begin(115200);
  delay(10);

    SerialAT.begin(9600);
    delay(6000);
  
    SerialMon.println("Initializing modem...");
    modem.restart();
    SerialMon.println(modem.getModemInfo());
  
    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
      SerialMon.println(" fail");
      modem.restart();
      delay(10000);
    }
    SerialMon.println(" success");
  
    if (modem.isNetworkConnected()) {
      SerialMon.println("Network connected");
      delay(1000);
    }
  
    SerialMon.print("Connecting ");
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      SerialMon.println(" fail");
      modem.restart();
      delay(10000);
    }
    SerialMon.println(" success");
  
    if (modem.isGprsConnected()) {
      SerialMon.println("GPRS connected");
    }
}

void loop()
{
    if(client.connected()) {
      Serial.println("Send data....");
      String data =  String(dummy);
      http.get("/box/post.php?data=" + data);
  
      int statusCode = http.responseStatusCode();
      String response = http.responseBody();
      Serial.print("Respon Code : ");
      Serial.println(statusCode);
      Serial.print("Payload : ");
      Serial.println(response);
    }
    if(!client.connect(server, port)) {
      Serial.println("Fail. ");
    }
  
}
