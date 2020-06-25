#include "Endpoints.h"

String readRequest(EthernetClient* client);
void handleRequest(EthernetClient* client, EthernetServer* server, String request, Elegoo_TFTLCD* tft, TouchScreen* ts);
void handleGet(EthernetClient* client, EthernetServer* server, String target, String request, Elegoo_TFTLCD* tft, TouchScreen* ts);
void handlePost(EthernetClient* client, String target, String request, Elegoo_TFTLCD* tft);
void sendHTTP(EthernetClient* client, int response, String options);
bool sendFile(EthernetClient* client, String fileName);
String paramString(String request);

void handleGet(EthernetClient* client, EthernetServer* server, String target, String request, Elegoo_TFTLCD* tft, TouchScreen* ts) {
  if(target == "/") {
    sendHTTP(client, 200, "");
    sendFile(client, "index.htm");
  } else if(target == "/index.htm" || target == "/script.js" || target == "/libs/jquery.js" || target == "/libs/noti.js") {
    sendHTTP(client, 200, "");
    sendFile(client, target.substring(1));
  } else if(target == "/style.css" || target == "/libs/noti.css") {
    sendHTTP(client, 200, "Content-Type: text/css\n");
    sendFile(client, target.substring(1));
  } else if(target == "/pot") {
    GET_pot(client, server);
  } else if(target == "/touch") {
    GET_touch(client, server, ts);
  } else if(target == "/close") {
    sendHTTP(client, 200, "");
    client->println("Closed");
  } else {
    sendHTTP(client, 404, "");
    tft->println("GET " + target);
  }
}

void handlePost(EthernetClient* client, String target, String request, Elegoo_TFTLCD* tft) {
  if(target == "/drawLine") {
    sendHTTP(client, 200, "");
    POST_drawLine(client, paramString(request), tft);
  } else if (target == "/fillScreen") {
    sendHTTP(client, 200, "");
    POST_fillScreen(client, paramString(request), tft);
  } else if (target == "/drawRow") {
    sendHTTP(client, 200, "");
    POST_drawRow(client, paramString(request), tft);
  } else {
    sendHTTP(client, 404, "");
    tft->println("POST " + target);
  }
}

String readRequest(EthernetClient* client) {
  String request = "";
  String line = "";
  char readed;

  while(client->connected() && client->available()) {
    readed = char(client->read());    
    line += readed;
    if(readed == '\n') {
      if(line.indexOf(" HTTP/1.1") >= 0) {        
        request += line;
      }
      if(line.indexOf("<params>") >= 0) {
        request += line;
      }
      line = "";
    }
  }
  // Giusto per beccare l'ultima riga che non sempre ha il CR LF
  request += line;
  return request;
}

void handleRequest(EthernetClient* client, EthernetServer* server, String request, Elegoo_TFTLCD* tft, TouchScreen* ts) {
  if(request.indexOf("GET ") >= 0) {
    String target = request.substring(request.indexOf("GET ") + 4, request.indexOf(" HTTP/1.1"));
    handleGet(client, server, target, request, tft, ts);
  } else if(request.indexOf("POST ") >= 0) {
    String target = request.substring(request.indexOf("POST ") + 5, request.indexOf(" HTTP/1.1"));
    handlePost(client, target, request, tft);
  }
}

void sendHTTP(EthernetClient* client, int response, String options) {
  switch(response) {
    case 200:
      client->println("HTTP/1.1 200 OK");
    break;
    case 404:
      client->println("HTTP/1.1 404 Not Found");      
    break;
    default:
      client->println("HTTP/1.1 500 Internal Server Error");      
    break;
  }
  client->println("Connection: close");
  client->println("Cache-Control: no-store");
  client->println(options);
}

bool sendFile(EthernetClient* client, String fileName) {
  File toServe;
  toServe = SD.open(fileName, FILE_READ);
  if(!toServe) return false;
  int bufferSize = 2048;
  int readed = 0;
  char readedBuffer[bufferSize+1];
  while(toServe.available()) {
    readed = toServe.read(readedBuffer, bufferSize);
    readedBuffer[readed] = 0;
    client->print(readedBuffer);   
  }
  toServe.close();
  return true;
}

String paramString(String request) {
  return (request.substring(request.indexOf("<params>") + 8, request.indexOf("</params>")));
}
