#include "MyHTTP.h"

void tftBegin(Elegoo_TFTLCD* tft) {
  tft->reset();
  tft->begin(0x9341);
  tft->setRotation(1);
  tft->fillScreen(0);
  tft->setCursor(0, 0);
  tft->setTextColor(0x07E0);
  tft->setTextSize(2); // 15 righe da 25 char
  tft->setTextWrap(true);
  return;
}

void printColor(Elegoo_TFTLCD* tft, String msg, unsigned int color) {
  tft->setTextColor(color);
  tft->println(msg);
  tft->setTextColor(0x07E0);
  return;
}

void potTone(int duration, int buzzerPin, int sens, int* potVal) {
  unsigned long int now = millis();
  int prevPotVal = *potVal;
  while(millis() <= now + duration) {
    prevPotVal = *potVal;
    *potVal = analogRead(POT_PIN);
    *potVal = *potVal < 32 ? 32 : *potVal;
    tone(buzzerPin, *potVal);
    delay(100);
    if(*potVal <= ( prevPotVal - sens ) || *potVal >= ( prevPotVal + sens)) {
      noTone(buzzerPin);
    }
  }
  return;
}
