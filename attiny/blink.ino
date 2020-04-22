#define LEDPIN 0
#define BLINKTIME 1000

void setup()
{
  pinMode(LEDPIN, OUTPUT);
}

void loop()
{
  digitalWrite(LEDPIN, HIGH);
  delay(BLINKTIME);
  digitalWrite(LEDPIN, LOW);
  delay(BLINKTIME);
}
