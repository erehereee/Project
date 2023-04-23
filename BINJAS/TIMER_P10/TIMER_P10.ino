#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecure.h>
#include <DMDESP.h>
#include <fonts/ElektronMart6x8.h>

#define DISPLAYS_WIDE 1 // Kolom Panel
#define DISPLAYS_HIGH 1 // Baris Panel
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);

WiFiClientSecure client;
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
unsigned long startTime12;
unsigned long stopTime12;
unsigned long elapsedTime12;
unsigned long previousTime12;
String lastTimer12;
String Timer12;

unsigned int minutes;
unsigned int seconds;

const char* ssid = "BINJAS BBI1";
const char* password = "Jagojuara1";
String serverName = "http://bbi.weris.link/shuttle.php";

bool stopwatchRunning = false;
bool stopwatchRunning12 = false;

int b1 = D1;
int led1 = D2;
int count = 0;
void setup() {
  // Serial.begin(921600);
  Serial.begin(115200);
  pinMode(b1, INPUT);
  pinMode(led1, OUTPUT);
  digitalWrite(led1, 0);

  Disp.start(); // Jalankan library DMDESP
  Disp.setBrightness(20); // Tingkat kecerahan
  Disp.setFont(ElektronMart6x8); // Tentukan huruf

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led1, 0);
    delay(500);
    Serial.print(".");
  }
  digitalWrite(led1, 1);
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // server address, port and URL
  webSocket.begin("192.168.8.114", 81);

  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  delay(5000);

}

void loop() {
  webSocket.loop();
  Disp.loop();
  if (digitalRead(b1) == 1) {
    digitalWrite(led1, 0);
  }
  if (digitalRead(b1) == 0) {
    digitalWrite(led1, 1);
  }
  if (digitalRead(b1) == 1 && (String)data == "Start")  {
    startTimer();
  }
  if (digitalRead(b1) == 0 && (String)data == "Start") {
    startTimer12();
    timerRun12();
  }
  if (digitalRead(b1) == 0 && minutes == 12) {
    stopTimer12();
    webSocket.sendTXT("Timer Reset");
    minutes = 0;
    seconds = 0;
  }
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
      digitalWrite(led1, 0);
      break;
    case WStype_CONNECTED: {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        digitalWrite(led1, 1);
        Disp.drawText(2, 0, "SOMO");
        Disp.drawText(2, 9, "GAS?");

        // send message to server when Connected
        webSocket.sendTXT("Shuttle LED Online");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      strcpy(data, (const char*)payload);
      if (digitalRead(b1) == 1) {
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
        if ((String)data == "Stop") {
          Disp.clear();
          stopTimer();
          sendEndpoint();
          Disp.drawText(0, 0, lastTimer);
          Disp.drawText(2, 8, "P=" + (String)count);
          count = 0;
        }
        if ((String)data == "count") {
          count++;
          Disp.clear();
          Disp.drawText(2, 8, "P=" + (String)count);
        }
      }
      if (digitalRead(b1) == 0) {
        if ((String)data == "Start") {
          Disp.clear();
          count = 0;
          startTimer12();
        }
        if ((String)data == "Stop") {
          Disp.clear();
          stopTimer12();
          Disp.drawText(0, 0, lastTimer12);
          if (minutes < 10) {
            Disp.drawText(0, 0, "0" + (String)minutes + ":" + (String)seconds);
          }
          if (seconds < 10) {
            Disp.drawText(0, 0, (String)minutes + ":" +  "0" + (String)seconds);
          }
          if (minutes >= 10 && seconds < 10) {
            Disp.drawText(0, 0, (String)minutes + ":" +  "0" + (String)seconds);
          }
          count = 0;
        }
      }

      // send message to server
      // webSocket.sendTXT("message here");
      break;
  }

}
void sendEndpoint() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverName + "?p3=" + (String)lastTimer;
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
void startTimer12() {
  if (!stopwatchRunning12) {
    stopwatchRunning12 = true;
    startTime12 = millis();
    previousTime12 = startTime12;
  }
}

void stopTimer12() {
  if (stopwatchRunning12) {
    stopwatchRunning12 = false;
    stopTime12 = millis();
    elapsedTime12 = stopTime12 - startTime12;

    // Tampilkan waktu yang ditempuh dalam format menit:detik
    Serial.print("Waktu yang ditempuh: ");
    Serial.print(minutes);
    Serial.print(":");
    lastTimer12 = (String)minutes + ":" + (String)seconds;
    Serial.print(seconds);
    Serial.println(" menit");
  }
}

void timerRun12() {
  if (stopwatchRunning12) {
    unsigned long currentTime12 = millis();
    if (currentTime12 - previousTime12 >= 10) { // Tampilkan setiap 10 milidetik
      previousTime12 = currentTime12;
      elapsedTime12 = currentTime12 - startTime12;

      // Konversi waktu dari milidetik menjadi menit dan detik
      minutes = (elapsedTime12 / 1000) / 60;
      seconds = (elapsedTime12 / 1000) % 60;

      // Tampilkan waktu yang ditempuh dalam format menit:detik
      Serial.print("Waktu yang ditempuh: ");
      Serial.print(minutes);
      Serial.print(":");
      Timer12 = (String)minutes + ":" + (String)seconds;
      Disp.drawText(0, 0, Timer12);
      if (minutes < 10) {
        Disp.drawText(0, 0, "0" + (String)minutes + ":" + (String)seconds);
      }
      if (seconds < 10) {
        Disp.drawText(0, 0, "0" + (String)minutes + ":" +  "0" + (String)seconds);
      }
      if (minutes >= 10 && seconds < 10) {
        Disp.drawText(0, 0, (String)minutes + ":" +  "0" + (String)seconds);
      }
      Serial.print(seconds);
      Serial.println(" menit");
    }
  }
}
