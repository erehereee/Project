
/*
   WebSocketServer.ino

    Created on: 22.05.2015

*/


#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);

const char *ssid = "BINJAS BBI1";
const char *password = "Jagojuara1";

const char*  server = "www.bbi.my.id";  // Server URL

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
int nomor = 6;

int b1 = 2;
int ledSend = 17;
int ledKonek = 18;
int ledSocket = 5;

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

WiFiClientSecure client;

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
  TP = (String)P1 + (String)P2 + (String)P3 + (String)P4 + (String)P5 + (String)P6 + (String)P7 + (String)P8;
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
  if ((String)TP == "12345678") {
    stopTimer();
    webSocket.broadcastTXT("Stop");
    button_press_count = 0;
    sendEndpoint();
    TP = "";
    S1 = "";
    S2 = "";
    S3 = "";
    S4 = "";
    S5 = "";
    S6 = "";
    S7 = "";
    S8 = "";
    dataTag = 0;
    P1 = 0;
    P2 = 0;
    P3 = 0;
    P4 = 0;
    P5 = 0;
    P6 = 0;
    P7 = 0;
    P8 = 0;
    Serial.println("Shuttle Run Reset");
  }

  timerRun();

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
      strcpy(data, (const char*)payload);
      if ((String)data == "Start Shuttle Run") {
        dataTag = 0;
        webSocket.broadcastTXT("reset");
      }
      if ((String)data == "R1") {
        dataTag++;
        Serial.print("dataTag dari R1 = ");
        Serial.println(dataTag);
      }
      if ((String)data == "R2") {
        dataTag++;
        Serial.print("dataTag dari R2 = ");
        Serial.println(dataTag);
      }
      if (dataTag == 2) {
        if (Detik <= 9) {
          Serial.println("0" + Timer);
          S1 = "0" + (String)Timer;
        }
        S1 = (String)Timer;
        P1 = 2 / 2;
        webSocket.broadcastTXT("count");
        Serial.print("Total Putaran = ");
        Serial.println(P1);
        Serial.print("Waktu = ");
        Serial.println(S1);
      }
      if (dataTag == 4) {
        if (Detik <= 9) {
          Serial.println("0" + Timer);
          S2 = "0" + (String)Timer;
        }
        S2 = (String)Timer;
        P2 = 4 / 2;
        webSocket.broadcastTXT("count");
        Serial.print("Total Putaran = ");
        Serial.println(P2);
        Serial.print("Waktu = ");
        Serial.println(S2);
      }
      if (dataTag == 6) {
        if (Detik <= 9) {
          Serial.println("0" + Timer);
          S3 = "0" + (String)Timer;
        }
        S3 = (String)Timer;
        P3 = 6 / 2;
        webSocket.broadcastTXT("count");
        Serial.print("Total Putaran = ");
        Serial.println(P3);
        Serial.print("Waktu = ");
        Serial.println(S3);
      }
      if (dataTag == 8) {
        if (Detik <= 9) {
          Serial.println("0" + Timer);
          S4 = "0" + (String)Timer;
        }
        S4 = (String)Timer;
        P4 = 8 / 2;
        webSocket.broadcastTXT("count");
        Serial.print("Total Putaran = ");
        Serial.println(P4);
        Serial.print("Waktu = ");
        Serial.println(S4);
      }
      if (dataTag == 10) {
        if (Detik <= 9) {
          Serial.println("0" + Timer);
          S5 = "0" + (String)Timer;
        }
        S5 = (String)Timer;
        P5 = 10 / 2;
        webSocket.broadcastTXT("count");
        Serial.print("Total Putaran = ");
        Serial.println(P5);
        Serial.print("Waktu = ");
        Serial.println(S5);
      }
      if (dataTag == 12) {
        if (Detik <= 9) {
          Serial.println("0" + Timer);
          S6 = "0" + (String)Timer;
        }
        S6 = (String)Timer;
        P6 = 12 / 2;
        webSocket.broadcastTXT("count");
        Serial.print("Total Putaran = ");
        Serial.println(P6);
        Serial.print("Waktu = ");
        Serial.println(S6);
      }
      if (dataTag == 14) {
        if (Detik <= 9) {
          Serial.println("0" + Timer);
          S7 = "0" + (String)Timer;
        }
        S7 = (String)Timer;
        P7 = 14 / 2;
        webSocket.broadcastTXT("count");
        Serial.print("Total Putaran = ");
        Serial.println(P7);
        Serial.print("Waktu = ");
        Serial.println(S7);
      }
      if (dataTag == 16) {
        if (Detik <= 9) {
          Serial.println("0" + Timer);
          S8 = "0" + (String)Timer;
        }
        S8 = (String)Timer;
        P8 = 16 / 2;
        webSocket.broadcastTXT("count");
        Serial.print("Total Putaran = ");
        Serial.println(P8);
        Serial.print("Waktu = ");
        Serial.println(S8);
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
  String url = "/RFID/shuttle.php";
  url += "?p1=";
  url += (String)S1;
  url += "&p2=";
  url += (String)S2;
  url += "&p3=";
  url += (String)S3;
  url += "&p4=";
  url += (String)S4;
  url += "&p5=";
  url += (String)S5;
  url += "&p6=";
  url += (String)S6;
  url += "&p7=";
  url += (String)S7;

  Serial.println(url);

  //client.setCertificate(test_client_cert); // for client verification
  //client.setPrivateKey(test_client_key);  // for client verification

  Serial.println("\nStarting connection to server...");
  client.setInsecure();
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Connection: close\r\n\r\n");

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();
  }
  
  }
