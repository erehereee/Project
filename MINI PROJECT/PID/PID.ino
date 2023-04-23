unsigned long t;
unsigned long tSeb = 0;
unsigned long dt;

float outKendaliSuhu;

float err1;
float errInt;
float errDer;
float errSeb = 0;

float SH = 0;
float KPS = 0; 
float KIS = 0;
float KDS = 0;

float CV;

void setup()
{
  Serial.begin(115200);
  delay(10);
}

void loop()
{

  outKendaliSuhu = kendaliSuhu(CV);

}

float kendaliSuhu(int inputSuhu) {
  t = millis();
  dt = (t - tSeb);

  err1 = SH - inputSuhu;
  errInt += (err1 + errSeb) * dt / 2;
  errDer = (err1 - errSeb) / dt;

  float out = (KPS * err1) + (KIS * errInt) + (KDS * errDer) ;
  tSeb = t;
  errSeb = err1;
  return out;
}
