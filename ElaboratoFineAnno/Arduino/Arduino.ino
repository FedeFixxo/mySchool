#include <Elegoo_TFTLCD.h>
#include <Ethernet.h>
#include <SD.h>
#include "Utils.h"

Elegoo_TFTLCD tft(A3, A2, A1, A0, A4);
byte mac[6] = { 0x90, 0xA2, 0xDA, 0xF9, 0xE7, 0x20 };
IPAddress ip(192, 168, 1, 69);
IPAddress gateway(192, 168, 1, 1);
EthernetServer server(80);

void setup() {  
  Serial.begin(9600);
  tftBegin(&tft);
  Ethernet.begin(mac, ip);
  server.begin();

  tft.print("Server @ ");
  tft.println(Ethernet.localIP());
  
  if(SD.begin(4))
    tft.println("SD OK");
  else
    printColor(&tft, "ERROR: SD", 0xF800);
  
  pinMode(BUZZER, OUTPUT);
  pinMode(POT_GND, OUTPUT);
  pinMode(POT_VCC, OUTPUT);
  pinMode(POT_PIN, INPUT);
  
  digitalWrite(POT_GND, LOW);
  digitalWrite(POT_VCC, HIGH);
  analogRead(POT_PIN);
  
  tone(BUZZER, 523, 250);
  delay(250);
  tone(BUZZER, 659, 250);
  delay(250);
  tone(BUZZER, 784, 250);
  delay(250);
  tone(BUZZER, 1046);
  delay(500);
  noTone(BUZZER);  
}

void loop() {
  EthernetClient client = server.available();
  if(client) {    
    handleRequest(&client, &server, readRequest(&client), &tft);
    client.stop();
  }
}
