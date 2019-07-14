#include "WiFi.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include "wificonf.h"
// auslagern des webzeugs #include "myWebserver.h"

WebServer server(80);

const char *ssid = NETWORKNAME;
const char *password = NETWORKPASSWD;

void handleRoot()
{

  server.send(200, "text/plain", "hello from esp32!");
}

void handleNotFound()
{
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}


void setup_WebServer() {

    server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
}