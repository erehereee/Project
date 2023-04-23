#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>
SoftwareSerial RFID(13, 15);
WebSocketsClient webSocket;
WiFiClientSecure client;
String server = "http://bbi.weris.link/connect.php";
String apiKeyValue = "tPmAT5Ab3j7F9";

String t1p1 = "ccffff205100340e2801190a530607031a8f";
String t1p2 = "ccffff2051003405100340e2801190a530607031a8f";
String t1p3 = "ccffff205100340e2801190a530607031a8";

String t2p1 = "ccffff205100340e2801190a530607031a7f";
String t2p2 = "ccffff2051003405100340e2801190a530607031a7f";
String t2p3 = "ccffff205100340e2801190a530607031a7";

String t3p1 = "ccffff205100340e2801190a53060702ea08e";
String t3p2 = "ccffff2051003405100340e2801190a53060702ea08e";
String t3p3 = "ccffff205100340e2801190a53060702ea08";

String t4p1 = "ccffff205100340e2801190a530607031abf";
String t4p2 = "ccffff2051003405100340e2801190a530607031abf";
String t4p3 = "ccffff205100340e2801190a530607031ab";

String t5p1 = "ccffff205100340e2801190a530607031a6f";
String t5p2 = "ccffff2051003405100340e2801190a530607031a6f";
String t5p3 = "ccffff205100340e2801190a530607031a6";

String t6p1 = "ccffff205100340e2801190a530607031a9f";
String t6p2 = "ccffff2051003405100340e2801190a530607031a9f";
String t6p3 = "ccffff205100340e2801190a530607031a9";


String t7p1 = "ccffff205100340e2801190a530607031aaf";
String t7p2 = "ccffff2051003405100340e2801190a530607031aaf";
String t7p3 = "ccffff205100340e2801190a530607031aa";

String t8p1 = "ccffff205100340e2801190a530607031a5f";
String t8p2 = "ccffff2051003405100340e2801190a530607031a5f";
String t8p3 = "ccffff205100340e2801190a530607031a5";

String t9p1 = "ccffff205100340e2801190a530607031adf";
String t9p2 = "ccffff2051003405100340e2801190a530607031adf";
String t9p3 = "ccffff205100340e2801190a530607031ad";

String t10p1 = "ccffff205100340e2801190a53060702ea05e";
String t10p2 = "ccffff2051003405100340e2801190a53060702ea05e";
String t10p3 = "ccffff205100340e2801190a53060702ea05";

char data1[200];

String nodeID = "2";
String rfidData = "";
String dataLost1 = "";
String dataLost2 = "";
String dataLost3 = "";
String dataLost4 = "";
String dataLost5 = "";
String dataLost6 = "";
String DataTag = "";
String LostData = "";
String tagId = "";
String newData;
String S1 = "";
String S2 = "";
String S3 = "";
String S4 = "";
String S5 = "";
String S6 = "";
String S7 = "";
String S8 = "";
String S9 = "";
String S10 = "";

int count = 0;
int req = 0;
int count1 = 0;

unsigned long previousMillis = 0;
const long interval = 10000;

byte led1 = 4;
byte led2 = 0;
byte b1 = 14;
void setup() {
  Serial.begin(115200);
  RFID.begin(57600);

  WiFiManager wm;
  wm.setBreakAfterConfig(true);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(b1, INPUT);

  bool res;
  res = wm.autoConnect("R2");
  if(!res) {
        Serial.println("Failed to connect");
        digitalWrite(led1, 0);
        ESP.restart();
        delay(5000);
    } 
    else {
        digitalWrite(led1, 1);
        Serial.println("BBI BINJAS Running READY!");
    }

  // server address, port and URL
  webSocket.begin("192.168.8.114", 81);
  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
}


void loop() {
  if(digitalRead(b1) == 1) {
    digitalWrite(led1, 0);
    digitalWrite(led2, 1);
    shuttleR2();
  }
  if(digitalRead(b1) == 0) {
    digitalWrite(led1, 1);
    digitalWrite(led2, 0);
    runningR2();
  }
}

void shuttleR2() {
  webSocket.loop();
  if (count1 == 1) {
    sendTag();
  }
}

void runningR2() {
  if (RFID.available() > 0) {
    while (RFID.available() > 0) {
      char rfidChar = RFID.read();
      rfidData += String(rfidChar, HEX);
      delay(5);
    }
    count++;
    int len = rfidData.length(); // Calculate the length of newData
    if(len > 40) {
      dataLost1 += rfidData.substring(40, 76) + ",";
      dataLost2 += rfidData.substring(80, 116) + ",";
      dataLost3 += rfidData.substring(120, 156) + ",";
      dataLost4 += rfidData.substring(160, 196) + ",";
      dataLost5 += rfidData.substring(200, 236) + ",";
    }
    
    String newDataS = rfidData.substring(0, 35);
    String newDataM = rfidData.substring(0, 36);
    String newDataL = rfidData.substring(0, len - 4);
    String newData41S = rfidData.substring(0, 36);
    String newData41M = rfidData.substring(0, 37);
    String newData41L = rfidData.substring(0, len - 4);
    
    Serial.print("DATA LENGTH : ");
    Serial.println(len);
    Serial.print("DATA RAW : ");
    Serial.println(rfidData);
    
    if (newDataM == (String)t1p1 || newDataL == (String)t1p2 || newDataS == (String)t1p3){
      S1 += (String)t1p1 + ",";
      Serial.println("Data Masuk Adalah Tag 1");
    }
    if (newDataM == (String)t2p1 || newDataL == (String)t2p2 || newDataS == (String)t2p3) {
      S2 += (String)t2p1 + ",";
      Serial.println("Data Masuk Adalah Tag 2");
    }
    if (newData41M == (String)t3p1 || newData41L == (String)t3p2 || newData41S == (String)t3p3) {
      S3 += (String)t3p1 + ",";
      Serial.println("Data Masuk Adalah Tag 3");
    }
    if (newDataM == (String)t4p1 || newDataL == (String)t4p2 || newDataS == (String)t4p3) {
      S4 += (String)t4p1 + ",";
      Serial.println("Data Masuk Adalah Tag 4");
    }
    if (newDataM == (String)t5p1 || newDataL == (String)t5p2 || newDataS == (String)t5p3) {
      S5 += (String)t5p1 + ",";
      Serial.println("Data Masuk Adalah Tag 5");
    }
    if (newDataM == (String)t6p1 || newDataL == (String)t6p2 || newDataS == (String)t6p3) {
      S6 += (String)t6p1 + ",";
      Serial.println("Data Masuk Adalah Tag 6");
    }
    if (newDataM == (String)t7p1 || newDataL == (String)t7p2 || newDataS == (String)t7p3) {
      S7 += (String)t7p1 + ",";
      Serial.println("Data Masuk Adalah Tag 7");
    }
    if (newDataM == (String)t8p1 || newDataL == (String)t8p2 || newDataS == (String)t8p3) {
      S8 += (String)t8p1 + ",";
      Serial.println("Data Masuk Adalah Tag 8");
    }
    if (newDataM == (String)t9p1 || newDataL == (String)t9p2 || newDataS == (String)t9p3) {
      S9 += (String)t9p1 + ",";
      Serial.println("Data Masuk Adalah Tag 9");
    }
    if (newData41M == (String)t10p1 || newData41L == (String)t10p2 || newData41S == (String)t10p3) {
      S10 += (String)t10p1 + ",";
      Serial.println("Data Masuk Adalah Tag 10");
    }
    if (newDataM != (String)t1p1 && newDataL != (String)t1p2 && newDataS != (String)t1p3 && 
        newDataM != (String)t2p1 && newDataL != (String)t2p2 && newDataS != (String)t2p3 &&
        newData41M != (String)t3p1 && newData41L != (String)t3p2 && newData41S != (String)t3p3 &&
        newDataM != (String)t4p1 && newDataL != (String)t4p2 && newDataS != (String)t4p3 &&
        newDataM != (String)t5p1 && newDataL != (String)t5p2 && newDataS != (String)t5p3 &&
        newDataM != (String)t6p1 && newDataL != (String)t6p2 && newDataS != (String)t6p3 &&
        newDataM != (String)t7p1 && newDataL != (String)t7p2 && newDataS != (String)t7p3 &&
        newDataM != (String)t8p1 && newDataL != (String)t8p2 && newDataS != (String)t8p3 &&
        newDataM != (String)t9p1 && newDataL != (String)t9p2 && newDataS != (String)t9p3 &&
        newData41M != (String)t10p1 && newData41L != (String)t10p2 && newData41S != (String)t10p3)
    {
      Serial.println("Invalid Tag");
      dataLost6 += rfidData + ",";
    }
    if (count == 10) {
      req = 1;
    }
    rfidData = "";
    previousMillis = millis();
  }
    DataTag = S1 + S2 + S3 + S4 + S5 + S6 + S7 + S8 + S9 + S10;
    LostData =  dataLost1 + dataLost2 + dataLost3 + dataLost4 + dataLost5 + dataLost6;
  if (req == 1) {
    sendEndpoint();
    count = 0;
    req = 0;
  }
  if (count == 0) {
    S1 = "";
    S2 = "";
    S3 = "";
    S4 = "";
    S5 = "";
    S6 = "";
    S7 = "";
    S8 = "";
    S9 = "";
    S10 = "";
    dataLost1 = "";
    dataLost2 = "";
    dataLost3 = "";
    dataLost4 = "";
    dataLost5 = "";
    dataLost6 = "";
    DataTag = "";
  }
  if (millis() - previousMillis > interval && count != 0) {
    Serial.println("Mengirim Data Secara Otomatis");
    sendEndpoint();
    count = 0;
  }
}

void sendEndpoint() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = server + "?api_key=" + apiKeyValue + "&tag=" + urlEncode(DataTag) + "&dataLost=" + urlEncode(LostData) + "&nodeID=" + nodeID;
    Serial.println("Request URL: " + url);
    http.begin(url);

    int httpCode = http.GET();
    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }

    http.end();
  }
}

String urlEncode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (unsigned int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        // send message to server when Connected
        webSocket.sendTXT("Shuttle R2 Online");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      strcpy(data1, (const char*)payload);
      if ((String)data1 == "Start") {
        count1 = 1;
      }
      if ((String)data1 == "Stop") {
        count1 = 0;
        Serial.println("Shuttle Run Stop");
      }

      // send message to server
      // webSocket.sendTXT("message here");
      break;
  }

}

void sendTag() {
  if (RFID.available()) { // Check if the RFID reader is sending data
    char c = RFID.read(); // Read the received character from RFID reader
    if (c == '\n') { // If the received character is a newline character
      Serial.println("Data Tag R2 Masuk!"); // Print the tag ID in hexadecimal format
      webSocket.sendTXT("R2");// tampilkan input di serial monitor
      tagId = ""; // Reset the tag ID variable
    } else {
      tagId += c; // Add the received character to the tag ID variable
    }
  }
}
