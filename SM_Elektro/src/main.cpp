#include "WiFi.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include "wificonf.h"
// auslagern des webzeugs #include "myWebserver.h"

WebServer server(80);

const char *ssid = NETWORKNAME;
const char *password = NETWORKPASSWD;

int sumVal = 5800;
int humidity;

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

void handlegetData()
{
  String jsonresponse;
 StaticJsonDocument<512> doc;
  JsonObject root = doc.to<JsonObject>();
  JsonObject meters = root.createNestedObject("meters");
  JsonObject meter1 = meters.createNestedObject("meter1");
  meter1["manuf"] = "EMH";
  meter1["serial"] = "1234567";
  meter1["sumVal"] = sumVal++;
  meter1["sumUnit"] = "kWh";
  
  JsonObject meter2 = meters.createNestedObject("meter2");
  meter2["manuf"] = "Conrad";
  meter2["serial"] = "765432";
  meter2["sumCnt"] = sumVal++;
  meter2["sumUnit"] = "10000/kWh";


  serializeJson(root, jsonresponse);
  server.send(200,"text/json",jsonresponse);
  
}

void setup()
{

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/getdata", handlegetData);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

 
}

void loop()
{

  server.handleClient();
}
