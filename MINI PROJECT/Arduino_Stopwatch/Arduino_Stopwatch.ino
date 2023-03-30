#include <ESP8266WiFi.h>

// Deklarasi pin yang digunakan
int startButton = 2;
int stopButton = 0;
int ledPin = D7;

// Variabel untuk menyimpan waktu
unsigned long startTime;
unsigned long stopTime;
unsigned long elapsedTime;
unsigned long previousTime;

// Variabel untuk menandai status stopwatch
bool stopwatchRunning = false;

void setup() {
  pinMode(startButton, INPUT_PULLUP);
  pinMode(stopButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // Jika tombol start ditekan dan stopwatch tidak sedang berjalan
  if (digitalRead(startButton) == LOW) {
    startTimer();
  }
  
  // Jika tombol stop ditekan dan stopwatch sedang berjalan
  if (digitalRead(stopButton) == LOW) {
    stopTimer();
  }

  timerRun();
  

}

void startTimer() {
    if(!stopwatchRunning) {
    stopwatchRunning = true;
    startTime = millis();
    previousTime = startTime;
    digitalWrite(ledPin, HIGH);
    }
}

void stopTimer() {
      if(stopwatchRunning) {
    stopwatchRunning = false;
    stopTime = millis();
    elapsedTime = stopTime - startTime;
    digitalWrite(ledPin, LOW);
    Serial.print("Waktu yang ditempuh: ");
    Serial.print(elapsedTime / 1000);
    Serial.print(".");
    Serial.print(elapsedTime % 1000);
    Serial.println(" detik");
    }
}

void timerRun() {
    if (stopwatchRunning) {
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= 10) { // Tampilkan setiap 10 milidetik
      previousTime = currentTime;
      elapsedTime = currentTime - startTime;
      Serial.print("Waktu yang ditempuh: ");
      Serial.print(elapsedTime / 1000);
      Serial.print(".");
      Serial.print(elapsedTime % 1000);
      Serial.println(" detik");
    }
  }
  }
