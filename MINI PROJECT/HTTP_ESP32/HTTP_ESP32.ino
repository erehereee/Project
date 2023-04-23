#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "BINJAS BBI1";
const char* password = "Jagojuara1";
String serverName = "http://bbi.weris.link/renang.php";

int dummy = 1;

void setup() {

    Serial.begin(115200);

    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

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
