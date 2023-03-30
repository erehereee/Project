
/*
   WebSocketServer.ino

    Created on: 22.05.2015

*/


#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);

const char *ssid = "BINJAS BBI1";
const char *password = "Jagojuara1";

void setup() {
  // Serial.begin(921600);
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
      Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    
  webSocket.begin();
  Serial.println("WebSocket server started");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      digitalWrite(ledSocket, 0);
      break;
    case WStype_CONNECTED:
      {
        digitalWrite(ledSocket, 1);
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "Server Online");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      



      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
  }

}
