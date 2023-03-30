/*
   WebSocketClient.ino

    Created on: 24.05.2015

*/
\


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <DMDESP.h>
#include <fonts/ElektronMart6x8.h>

#define DISPLAYS_WIDE 1 // Kolom Panel
#define DISPLAYS_HIGH 1 // Baris Panel
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

char data[200];

unsigned long startTime;
unsigned long stopTime;
unsigned long elapsedTime;
unsigned long previousTime;
unsigned long Detik;
unsigned long Milidetik;
unsigned long lastDetik;
unsigned long lastMilidetik;
String Timer;
String lastTimer;

const char* ssid = "BINJAS BBI1";
const char* password = "Jagojuara1";
String serverName = "https://bbi.my.id/RFID/shuttle.php";

bool stopwatchRunning = false;

int dataStopValue = 0;
int button_press_count = 0;
int button_state = 0;
int prev_button_state = 0;
int b1 = 2;
int ledSend = 5;
int ledKonek = 4;
int nomor = 5;
int count = 0;
void setup() {
  // Serial.begin(921600);
  Serial.begin(115200);
  pinMode(b1, INPUT_PULLUP);
  pinMode(ledSend, OUTPUT);
  pinMode(ledKonek, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Disp.start(); // Jalankan library DMDESP
  Disp.setBrightness(20); // Tingkat kecerahan
  Disp.setFont(ElektronMart6x8); // Tentukan huruf


  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

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
  Disp.loop();
//  button_state = digitalRead(b1);
//  if (button_state != prev_button_state) {
//    if (button_state == HIGH) {
//    } else { //button press
//      button_press_count++;
//    }
//    delay(250);
//
//    if (button_press_count == 1) {
//      stopTimer();
//      if (Detik <= 9) {
//        Disp.drawText(0, 0, "0" + lastTimer);
//      }
//      Disp.drawText(0, 0, lastTimer);
//      Disp.drawText(2, 8, "GAS?");
//    }
//    if (button_state == HIGH) {
//      button_press_count = 0;
//    }
//
//    prev_button_state = button_state;
//  }


  timerRun();

}

void startTimer() {
  if (!stopwatchRunning) {
    stopwatchRunning = true;
    startTime = millis();
    previousTime = startTime;
  }
}

void stopTimer() {
  if (stopwatchRunning) {
    stopwatchRunning = false;
    stopTime = millis();
    elapsedTime = stopTime - startTime;
    lastDetik = elapsedTime / 1000;
    lastMilidetik = elapsedTime % 100;

    Serial.print("Finish : ");
    Serial.print(Detik);
    Serial.print(":");
    Serial.println(Milidetik);
    lastTimer = (String)Detik + ":" + (String)Milidetik;
  }
}

void timerRun() {
  if (stopwatchRunning) {
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= 10) { // Tampilkan setiap 10 milidetik
      previousTime = currentTime;
      elapsedTime = currentTime - startTime;
      Detik = elapsedTime / 1000;
      Milidetik = elapsedTime % 1000;
      Serial.print("Waktu yang ditempuh: ");
      Serial.print(Detik);
      Serial.print(":");
      if (Milidetik > 990) {
        Milidetik = 0; // menambah nol di depan jika milidetik kurang dari 10
      }
      Serial.println(Milidetik);

      Timer = (String)Detik + ":" + (String)Milidetik;
      Disp.drawText(0, 0, Timer);
      if (Milidetik < 10) {
        Disp.drawText(0, 0, "0" + (String)Detik + ":" + (String)Milidetik + " ");
      }
      if (Detik <= 9) {
        Disp.drawText(0, 0, "0" + Timer);
      }
    }
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
        Disp.drawText(2, 0, "SOMO");
        Disp.drawText(2, 9, "GAS?");

        // send message to server when Connected
        webSocket.sendTXT("Shuttle LED Online");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      strcpy(data, (const char*)payload);
      if ((String)data == "reset") {
        Disp.clear();
        count = 0;
        Disp.drawText(2, 8, "P=" + (String)count);
      }
      if ((String)data == "Start") {
        Disp.clear();
        count = 0;
        startTimer();
        Disp.drawText(2, 8, "P=" + (String)count);
      }
      if ((String)data == "count") {
        count++;
        Disp.clear();
        Disp.drawText(2, 8, "P=" + (String)count);
      }
      if ((String)data == "Stop") {
        Disp.clear();
        stopTimer();
        sendEndpoint();
        Disp.drawText(0, 0, lastTimer);
        Disp.drawText(2, 8, "P=" + (String)count);
        count = 0;
      }

      // send message to server
      // webSocket.sendTXT("message here");
      break;
  }

}

void sendEndpoint() {
  if ((WiFi.status() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();

    //create an HTTPClient instance
    HTTPClient https;
    String serverPath = serverName + "?p8=" + (String)lastTimer;
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, serverPath.c_str())) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          digitalWrite(ledSend, 1);
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        digitalWrite(ledSend, 0);
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      delay(1000);
      digitalWrite(ledSend, 0);
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}
