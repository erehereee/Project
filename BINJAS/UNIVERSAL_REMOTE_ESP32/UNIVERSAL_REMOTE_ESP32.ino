
/*
   WebSocketServer.ino

    Created on: 22.05.2015

*/


#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <HTTPClient.h>

WebSocketsServer webSocket = WebSocketsServer(81);

const char *ssid = "BINJAS BBI1";
const char *password = "Jagojuara1";

String server = "http://bbi.weris.link/shuttle.php";  // Server URL

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
String S1;
String S2;
String S3;
String S4;
String S5;
String S6;
String S7;
String S8;

bool stopwatchRunning = false;

int button_press_count = 0;
int button_state = 0;
int prev_button_state = 0;

int b1 = 2;
int ledSend = 16;
int ledKonek = 18;
int ledSocket2 = 15;
int ledSocket = 17;

char data[200];
String TP;

int dataTag = 0;
int P1 = 0;
int P2 = 0;
int P3 = 0;
int P4 = 0;
int P5 = 0;
int P6 = 0;
int P7 = 0;
int P8 = 0;

int c = 0;
int R1 = 0;
int R2 = 0;
int f1 = 0;
int f2 = 0;
int f3 = 0;
int v = 0;

WiFiClient client;

void setup() {
  // Serial.begin(921600);
  Serial.begin(115200);
  pinMode(b1, INPUT_PULLUP);
  pinMode(ledSend, OUTPUT);
  pinMode(ledKonek, OUTPUT);
  pinMode(ledSocket, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledKonek, 0);
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  digitalWrite(ledKonek, 1);
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
  TP = (String)P1 + (String)P2 + (String)P3;
  button_state = digitalRead(b1);
  if (button_state != prev_button_state) {
    if (button_state == HIGH) {
    } else { //button press
      button_press_count++;
    }
    delay(250);

    if (button_press_count == 1) {
      digitalWrite(ledSend, 1);
      dataTag = 0;
      startTimer();
      webSocket.broadcastTXT("Start");
    }

    if (button_state == HIGH) {

      button_press_count = 0;
      delay(1000);
      digitalWrite(ledSend, 0);
    }
    //  if ((String)TP == "12345678") {
    //    webSocket.broadcastTXT("Stop");
    //    button_press_count = 0;
    //  }


    prev_button_state = button_state;
  }
  if ((String)TP == "123") {
    stopTimer();
    webSocket.broadcastTXT("Stop");
    button_press_count = 0;
    sendEndpoint();
    c = 0;
    TP = "";
    S1 = "";
    S2 = "";
    S3 = "";
    dataTag = 0;
    P1 = 0;
    P2 = 0;
    P3 = 0;
    f1 = 0;
    f2 = 0;
    f3 = 0;
    R1 = 0;
    R2 = 0;
    v = 0;
    Serial.println("Shuttle Run Reset");
  }


  timerRun();

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      if (num < 2) {
        digitalWrite(ledSocket, 0);
        digitalWrite(ledSocket2, 0);
      }
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        if (num == 2) {
          digitalWrite(ledSocket, 1);
          Serial.println("ALL CONNECTED");
        }
        // send message to client
        webSocket.sendTXT(num, "Server Online");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      strcpy(data, (const char*)payload);
      if ((String)data == "Start Shuttle Run" && v == 0) {
        v = 1;
        dataTag = 0;
        c = 1;
        R1 = 1;
        R2 = 0;
        webSocket.broadcastTXT("reset");
      }
      if ((String)data == "Timer Reset") {
        webSocket.broadcastTXT("Timer Stop");
      }
      if ((String)data == "Start Shuttle Run" && v == 1) {
        Serial.println("NULL");
        c = c;
        R1 = R1;
        R2 = R2;
      }
      if ( c == 1) {
        if ((String)data == "R2" && R2 == 0) {
          dataTag++;
          R2 = 1;
          R1 = 0;
          Serial.print("dataTag dari R2 = ");
          Serial.println(dataTag);
        }
        if ((String)data == "R2" && R2 == 1) {
          dataTag = dataTag;
          Serial.println("R2 NO COUNT");
        }
        if ((String)data == "R1" && R1 == 0) {
          dataTag++;
          R1 = 1;
          R2 = 0;
          Serial.print("dataTag dari R1 = ");
          Serial.println(dataTag);
        }
        if ((String)data == "R1" && R1 == 1) {
          dataTag = dataTag;
          Serial.println("R1 NO COUNT");
        }
        if (dataTag == 2) {
          if (Detik <= 9) {
            Serial.println("0" + Timer);
            S1 = "0" + (String)Timer;
          }
          S1 = (String)Timer;
          P1 = 2 / 2;
          Serial.print("Total Putaran = ");
          Serial.println(P1);
          Serial.print("Waktu = ");
          Serial.println(S1);
        }
        if (P1 == 1 && f1 == 0) {
          f1 = 1;
          webSocket.broadcastTXT("count");
        }
        if (P1 == 1 && f1 == 1) {
          Serial.println("P1 NO SEND ");
        }
        if (dataTag == 4) {
          if (Detik <= 9) {
            Serial.println("0" + Timer);
            S2 = "0" + (String)Timer;
          }
          S2 = (String)Timer;
          P2 = 4 / 2;
          Serial.print("Total Putaran = ");
          Serial.println(P2);
          Serial.print("Waktu = ");
          Serial.println(S2);
        }
        if (P2 == 2 && f2 == 0) {
          f2 = 1;
          webSocket.broadcastTXT("count");
        }
        if (P2 == 2 && f2 == 1) {
          Serial.println("P2 NO SEND ");
        }
        if (dataTag == 6) {
          if (Detik <= 9) {
            Serial.println("0" + Timer);
            S3 = "0" + (String)Timer;
          }
          S3 = (String)Timer;
          P3 = 6 / 2;
          Serial.print("Total Putaran = ");
          Serial.println(P3);
          Serial.print("Waktu = ");
          Serial.println(S3);
        }
        if (P3 == 3 && f3 == 0) {
          f3 = 1;
          webSocket.broadcastTXT("count");
        }
        if (P3 == 3 && f3 == 1) {
          Serial.println("P3 NO SEND ");
        }
      }





      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
  }

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

    //    Serial.print("Finish : ");
    //    Serial.print(Detik);
    //    Serial.print(":");
    //    Serial.println(Milidetik);
    lastTimer = (String)Detik + ":" + (String)Milidetik;
    Serial.print("Finish : ");
    Serial.println(lastTimer);
  }
}

//void timeSample() {
//  if (Detik <= 9) {
//    Serial.println("0" + lastTimer);
//    sample = "0" + lastTimer;
//  }
//  sample = lastTimer;
//  Serial.println(lastTimer);
//}

void timerRun() {
  if (stopwatchRunning) {
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= 10) { // Tampilkan setiap 10 milidetik
      previousTime = currentTime;
      elapsedTime = currentTime - startTime;
      Detik = elapsedTime / 1000;
      Milidetik = elapsedTime % 1000;
      //      Serial.print("Waktu yang ditempuh: ");
      //      Serial.print(Detik);
      //      Serial.print(":");
      if (Milidetik > 990) {
        Milidetik = 0; // menambah nol di depan jika milidetik kurang dari 10
      }
      //      Serial.println(Milidetik);
      Timer = (String)Detik + ":" + (String)Milidetik;
    }
  }
}

void sendEndpoint() {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    String url = server + "?p1=" + (String)S1 + "&p2=" + (String)S2;
    Serial.println("Request URL: " + url);
    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
    http.begin(url); //HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
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
