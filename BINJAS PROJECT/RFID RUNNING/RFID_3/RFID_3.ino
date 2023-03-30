//Tinggal Upload

#define RXD2 13
#define TXD2 15

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <SoftwareSerial.h>
SoftwareSerial RFID(RXD2, TXD2);


String serverName = "https://bbi.my.id/RFID/connect.php";
#define STASSID "BINJAS BBI3"
#define STAPSK "Jagojuara3"
String apiKeyValue = "tPmAT5Ab3j7F9";

String nodeID = "3";

int data;
int tempData[23];
String tempString = "";
String arrString[10];
int arrIndex = 0;
char hex[20];
int i = 0;
String sendStr = "";

String dataTag;
String Nama;
int dummy = 1;
unsigned long previousMillis = 0;
const long interval = 2000;

bool search(String temp) {
  for (int l = 0; l < arrIndex; l++) {
    Serial.println(arrString[l] == temp);
    if (arrString[l] == temp) {
      return false;
    }
  }
  return true;
}

void setup() {

  Serial.begin(115200);
  RFID.begin(57600);
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(4, 1);
}

void loop() {
  // wait for WiFi connection
  unsigned long currentMillis = millis();

  if (RFID.available()) {
    data = RFID.read();
    if (i < 22) {
      tempData[i] = data;
      i++;
    } else {
      for (int k = 0; k < 21; k++) {
        sprintf(hex, "%02X", tempData[k]); //convert number to hex
        tempString += hex;
        tempString += " ";
      }
      if (search(tempString)) {
        arrString[arrIndex] = tempString;
        arrIndex++;
      }
      tempString = "";
      i = 0;
    }
  }
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if ((WiFi.status() == WL_CONNECTED)) {
      if (arrIndex != 0) {
        for (int k = 0; k < arrIndex; k++) {
          sendStr += arrString[k] + ",";
        }
        Serial.println("Mengirim data :  " + sendStr);
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

        // Ignore SSL certificate validation
        client->setInsecure();
        HTTPClient https;
        String httpRequestData = "api_key=" + apiKeyValue + "&tag=" + urlEncode(sendStr) + "&nodeID=" + nodeID;
        String serverPath = serverName + "?" + httpRequestData;
        Serial.println(serverPath);
        sendStr = "";
        arrIndex = 0;
        memset(arrString, 0, 5 * sizeof(String));
        Serial.print("[HTTPS] begin...\n");
        // configure traged server and url
        if (https.begin(*client, serverPath.c_str())) {
          Serial.println(serverPath.c_str());
          Serial.print("[HTTP] GET...\n");
          digitalWrite(2, 0);
          // start connection and send HTTPS GET
          int httpCode = https.GET();
          // httpCode will be negative on error
          if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK) {
              const String& payload = https.getString();
              Serial.println("received payload:\n<<");
              Serial.println(payload);
              Serial.println(">>");
            }
          } else {
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
          }
        }
        https.end();
      }
      digitalWrite(2, 1);
    }
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
