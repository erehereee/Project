
/*
   WebSocketServer.ino

    Created on: 22.05.2015

*/


#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>

const char *ssid = "BINJAS BBI1";
const char *password = "Jagojuara1";

const char*  server = "www.bbi.my.id";  // Server URL

int dummy = 1;

WiFiClientSecure client;

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

    
}

void loop() {

  sendEndpoint();
  
}

void sendEndpoint() {

  String url = "/RFID/renang.php";
  url += "?nomor=";
  url += (String)dummy;
  url += "&waktu=";
  url += (String)dummy;

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
