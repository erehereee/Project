#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "BINJAS BBI1";
const char* password = "Jagojuara1";
String serverName = "http://bbi.weris.link/renang.php";

int dummy = 1;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {

}

void sendEndpoint() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverName + "?data=" + (String)dummy;
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
