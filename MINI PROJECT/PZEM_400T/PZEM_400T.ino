#include <PZEM004Tv30.h>
PZEM004Tv30 ptc(&Serial2);

void setup()
{
   Serial.begin(9600);

   Serial.print("Voltage = ");
   Serial.print(ptc.voltage);
   Serial.print("Current = ");
   Serial.print(ptc.current);
   Serial.print("Power = ");
   Serial.print(ptc.power);
}

void loop()
{

}
