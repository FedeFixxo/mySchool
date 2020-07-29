#include "MyPins.h"

void POST_drawLine(EthernetClient* client, String params, Elegoo_TFTLCD* tft);
void POST_fillScreen(EthernetClient* client, String params, Elegoo_TFTLCD* tft);
void POST_drawRow(EthernetClient* client, String params, Elegoo_TFTLCD* tft);
void GET_pot(EthernetClient* client, EthernetServer* server);
void GET_touch(EthernetClient* client, EthernetServer* server, TouchScreen* ts);


void POST_drawLine(EthernetClient* client, String params, Elegoo_TFTLCD* tft) {
  int x0 = params.substring(params.indexOf("<x0>") + 4, params.indexOf("</x0>")).toInt();
  int y0 = params.substring(params.indexOf("<y0>") + 4, params.indexOf("</y0>")).toInt();
  int x1 = params.substring(params.indexOf("<x1>") + 4, params.indexOf("</x1>")).toInt();
  int y1 = params.substring(params.indexOf("<y1>") + 4, params.indexOf("</y1>")).toInt();
  unsigned int color = params.substring(params.indexOf("<color>") + 7, params.indexOf("</color>")).toInt();
  tft->drawLine(x0, y0, x1, y1, color);
}

void POST_fillScreen(EthernetClient* client, String params, Elegoo_TFTLCD* tft) {
  unsigned int color = params.substring(params.indexOf("<color>") + 7, params.indexOf("</color>")).toInt();
  tft->fillScreen(color);
  tft->setCursor(0, 0);
}

void POST_drawRow(EthernetClient* client, String params, Elegoo_TFTLCD* tft) {
  Serial.println("OK");
  unsigned int rowIndex = params.substring(params.indexOf("<rowIndex>") + 10, params.indexOf("</rowIndex>")).toInt();
  unsigned int colIndex = params.substring(params.indexOf("<colIndex>") + 10, params.indexOf("</colIndex>")).toInt();
  unsigned int paramLength = params.substring(params.indexOf("<length>") + 8, params.indexOf("</length>")).toInt();
  String dataColor = params.substring(params.indexOf("<line>") + 6, params.indexOf("</line>"));
  for(int i = 0; i < paramLength; i++) {
    unsigned int color = dataColor.substring(i*6, i*6+5).toInt();
    Serial.println(color);
    tft->drawPixel(colIndex + i, rowIndex, color);
  }
  tft->setCursor(0, 0);
}

void GET_pot(EthernetClient* client, EthernetServer* server) {
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/event-stream");
  client->println();
  while(!server->available()) {
    client->println("data: " + String(analogRead(POT_PIN)) + "\n");
    delay(10);
  }
  client->println();
}

void GET_touch(EthernetClient* client, EthernetServer* server, TouchScreen* ts) {
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/event-stream");
  client->println();
  while(!server->available()) {
    TSPoint p = ts->getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if(p.z > 5) {
      client->println("data: { \"x\": " + String(map(p.y, 80, 900, 320, 0)) + ", \"y\": " + String(map(p.x, 120, 910, 240, 0)) + "}\n");
    }
    delay(1);
  }
  client->println();
}
