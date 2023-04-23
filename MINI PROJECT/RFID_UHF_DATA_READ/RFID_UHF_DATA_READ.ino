#include <SoftwareSerial.h>
SoftwareSerial RFID(13, 15);

String rfidData = "";

void setup() {
  Serial.begin(115200);
  RFID.begin(57600);

}

void loop() {
  if (RFID.available() > 0) {
    while (RFID.available() > 0) {
      char rfidChar = RFID.read();
      rfidData += String(rfidChar, HEX);
      delay(5);
    }
    int len = rfidData.length();
    Serial.print("DATA LENGTH : ");
    Serial.println(len);
    Serial.print("DATA RAW : ");
    Serial.println(rfidData);

    rfidData = "";
  }
}
