/*
   WebSocketServer.ino

    Created on: 22.05.2015

*/


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);

const char* apssid = "Node1"; // SSID Access Point
const char* appassword = "renang1";
const char* ssid = "BINJAS BBI1";
const char* password = "Jagojuara1";

int dataStopValue = 0;
int button_press_count = 0;
int button_state = 0;
int prev_button_state = 0;

int b1 = 2;

char data[200];


void setup() {
  // Serial.begin(921600);
  Serial.begin(115200);
  pinMode(b1, INPUT_PULLUP);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_AP_STA); // Set mode ke Access Point dan WiFi klien
  WiFi.softAP(apssid, appassword); // Konfigurasi Access Point
  Serial.println("Access Point configured");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  webSocket.begin();
  Serial.println("WebSocket server started");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
    button_state = digitalRead(b1);
  if (button_state != prev_button_state) {
    if (button_state == HIGH) {
      digitalWrite(LED_BUILTIN, LOW);
    } else { //button press
      digitalWrite(LED_BUILTIN, HIGH);
      button_press_count++;
    }
    delay(250);

    if (button_press_count == 1) {
    webSocket.sendTXT(0, "1");
  }

  if (button_state == HIGH) {
    button_press_count = 0;
  }
  

    prev_button_state = button_state;
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Server Online");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      strcpy(data, (const char*)payload);
      



      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
  }

}
