/*
   WebSocketClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <SoftwareSerial.h>
SoftwareSerial RFID(13, 15);

WebSocketsClient webSocket;

char data1[200];

const char* ssid = "BINJAS BBI1";
const char* password = "Jagojuara1";
String serverName = "https://bbi.my.id/RFID/connect.php";

int ledSend = 5;
int ledKonek = 4;
int nomor = 1;
String nodeID = "3";
int data;
int tempData[23];
String tempString = "";
String arrString[10];
int arrIndex = 0;
char hex[20];
int i = 0;
String sendStr = "";

boolean kondisi = true;
int count = 0;

bool search(String temp) {
  for (int l = 0; l < arrIndex; l++) {
    Serial.println(arrString[l] == temp);
    if (arrString[l] == temp) {
      return false;
    }
  }
  return true;
}

void setup() {
  // Serial.begin(921600);
  Serial.begin(115200);
  RFID.begin(57600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledKonek, 0);
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  digitalWrite(ledKonek, 1);
  Serial.println("Connected to WiFi");

  // server address, port and URL
  webSocket.begin("192.168.8.114", 81);
  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

}

void loop() {
  webSocket.loop();
  if (count == 0) {
    sendStr = "";
  }
  if (count == 1) {
    sendTag();
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      digitalWrite(ledSend, 0);
      break;
    case WStype_CONNECTED: {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        digitalWrite(ledSend, 1);

        // send message to server when Connected
        webSocket.sendTXT("Shuttle R2 Online");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      strcpy(data1, (const char*)payload);
      if ((String)data1 == "Start") {
        sendStr = "";
        count = 1;
      }
      if ((String)data1 == "Stop") {
        count = 0;
        Serial.println("Shuttle Run Stop");
      }

      // send message to server
      // webSocket.sendTXT("message here");
      break;
  }

}

void sendTag() {
  if (RFID.available()) {
    data = RFID.read();
    if (i < 22) {
      tempData[i] = data;
      i++;
    } else {
      for (int k = 0; k < 21; k++) {
        sprintf(hex, "%02X", tempData[k]); //convert number to hex
        tempString += hex;
        tempString += " ";
      }
      if (search(tempString)) {
        arrString[arrIndex] = tempString;
        arrIndex++;
      }
      tempString = "";
      i = 0;
    }
  }
  if (arrIndex != 0) {
    for (int k = 0; k < arrIndex; k++) {
      sendStr += arrString[k] + ",";
    }
    Serial.println("Mengirim data :  " + sendStr);
    webSocket.sendTXT("R2");
    sendStr = "";
    arrIndex = 0;
    memset(arrString, 0, 5 * sizeof(String));
  }
}
