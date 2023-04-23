/*
   WebSocketClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <SoftwareSerial.h>
SoftwareSerial RFID(13, 15);

WebSocketsClient webSocket;

char data1[200];

const char* ssid = "BINJAS BBI1";
const char* password = "Jagojuara1";

int ledSend = 5;
int ledKonek = 4;
String tagId = "";

int count = 0;
int x = 0;

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
  if (count == 1) {
    nullTag();
  }
  if (count == 2) {
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
        webSocket.sendTXT("Shuttle R1 Online");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      strcpy(data1, (const char*)payload);
      if ((String)data1 == "Start") {
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
  if (RFID.available()) { // Check if the RFID reader is sending data
    char c = RFID.read(); // Read the received character from RFID reader
    if (c == '\n') { // If the received character is a newline character
      Serial.println("Data Tag R1 Masuk!"); // Print the tag ID in hexadecimal format
      webSocket.sendTXT("R1");// tampilkan input di serial monitor
      tagId = ""; // Reset the tag ID variable
    } else {
      tagId += c; // Add the received character to the tag ID variable
    }
  }
}



void nullTag() { // Jika ada data dari modul UHF RFID reader
  if (RFID.available()) { // Check if the RFID reader is sending data
    char c = RFID.read(); // Read the received character from RFID reader
    if (c == '\n') { // If the received character is a newline character
      Serial.println("Start"); // Print the tag ID in hexadecimal format
      webSocket.sendTXT("Start Shuttle Run");
      count = 2;
      tagId = ""; // Reset the tag ID variable
    } else {
      tagId += c; // Add the received character to the tag ID variable
    }
  }
}
