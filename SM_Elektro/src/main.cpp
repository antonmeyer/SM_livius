#include "WiFi.h"
#include "aREST.h"
#include "wificonf.h"
 
aREST rest = aREST();
 
WiFiServer server(80);
 
const char* ssid = NETWORKNAME;
const char* password =  NETWORKPASSWD;
 
int temperature;
int humidity;

int testFunction(String command) {
  Serial.println("Received rest request");
}
 
void setup()
{
 
  Serial.begin(115200);
 
  rest.function("test",testFunction);
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
 

  rest.set_id("1");
  rest.set_name("meinesp32");

  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);

   server.begin();
 
}
 
void loop() {
 
  WiFiClient client = server.available();
  if (client) {
 
    while(!client.available()){
      delay(5);
    }
    rest.handle(client);
  }
}
