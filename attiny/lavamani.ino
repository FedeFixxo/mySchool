#include <Adafruit_NeoPixel.h>

#define PIN_RING 0
#define PIN_PAR 1
#define RING_SIZE 16
#define WAIT_TIME 5000
#define BLINK_TIME 500

/* Variabili */
long long unsigned int time;
unsigned int elapsed;
bool prevState = false;
bool endOn = false;
uint32_t rosso;
uint32_t verde;
unsigned short ledAccesi = -1;
	/* Oggetti */
	Adafruit_NeoPixel ring;

/* Funzioni */
void setup() {
  pinMode(PIN_PAR, INPUT);
  ring = Adafruit_NeoPixel(RING_SIZE, PIN_RING, NEO_GRB + NEO_KHZ800);
  ring.begin();
  rosso = ring.Color(255, 0, 0);
  verde = ring.Color(0, 255, 0);
  initSeq(3, RING_SIZE);
}

void loop() {  
  if(digitalRead(PIN_PAR)) {
    if(!prevState) {
      time = millis();
      prevState = true;
    }
    elapsed = millis() - time;
    if(elapsed <= WAIT_TIME) {
      if(ledAccesi != elapsed/(WAIT_TIME/16)) {
        ledAccesi = elapsed/(WAIT_TIME/16);
      	ring.fill(rosso, 0, ledAccesi+1);
      	ring.show();
      }
    } else {
      if(!endOn && (elapsed%(2*BLINK_TIME)) <= BLINK_TIME) {
      	ring.fill(verde, 0, 16);
      	ring.show();
        endOn = true;
      } else if(endOn && (elapsed%(2*BLINK_TIME)) > BLINK_TIME) {
      	ring.clear();
      	ring.show();
        endOn = false;
      }
    }    
  } else if(prevState) {
    ring.clear();
    ring.show();
    ledAccesi = -1;
    prevState = false;
  }
  delay(100); // Molto utile nel simulatore per non appesantire troppo
}

// Bland spero corretto del campo HUE su 3 giri 
// Da Rosso a Verde a Blu
void initSeq(short numeroGiri, short numLed) {
  uint32_t color;
  const short mezzoGiro = numLed / 2;
  const short scalinoBlend = 256 / mezzoGiro;
  for(int i = 0; i < numLed * numeroGiri; i++) {
    switch( ( i / mezzoGiro) % 6) {
      case 0:
      	color = scalinoBlend * (i % +8) <= 255 ? ring.Color(255, (scalinoBlend*(i % 8)), 0) : ring.Color(255, 255, 0);
      break;
      case 1:
      	color = 255 - (scalinoBlend * (i % mezzoGiro)) >= 0 ? ring.Color(255 - (scalinoBlend * (i % mezzoGiro)), 255, 0) : ring.Color(0, 255, 0);
      break;
      case 2:
      	color = scalinoBlend * (i % mezzoGiro) <= 255 ? ring.Color(0, 255, (scalinoBlend*(i % mezzoGiro))) : ring.Color(0, 255, 255);
      break;
      case 3:
      	color = 255 - (scalinoBlend * (i % mezzoGiro)) >= 0 ? ring.Color(0, 255 - (scalinoBlend * (i % mezzoGiro)), 255) : ring.Color(0, 0, 255);
      break;
      case 4:
      	color = scalinoBlend * (i % mezzoGiro) <= 255 ? ring.Color(scalinoBlend*(i % mezzoGiro), 0, 255) : ring.Color(255, 0, 255);
      break;
      case 5:
      	color = 255 - (scalinoBlend * (i % mezzoGiro)) >= 0 ? ring.Color(255, 0, 255 - (scalinoBlend * (i % mezzoGiro))) : ring.Color(255, 0, 0);
      break;
    }
    ring.setPixelColor(i%numLed, color);
  	ring.show();
    delay(20);
  }
  ring.clear();
  ring.show();
}
