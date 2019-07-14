#include "myWebServer.h"
#include "mysml.h"

int sumVal = 5800;
int humidity;

void handlegetData();

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

  //the basics moved to myWebServer.h
  setup_WebServer();

  server.on("/getdata", handlegetData);

  server.begin();
  Serial.println("HTTP server started");

  setup_uart();
}

void loop()
{
  handle_uart_event();
  server.handleClient();
}

// not best practice, we keep it here to have access on the global variables
// ToDo find a better solution
// is a callback from the Web-Server
void handlegetData()
{
  String jsonresponse;
 StaticJsonDocument<512> doc;
  JsonObject root = doc.to<JsonObject>();
  JsonObject meters = root.createNestedObject("meters");
  JsonObject meter1 = meters.createNestedObject("meter1");


  // we need to ensure, that one set of values belongs together
  // hardware serial delivers independant new values
  // so we copy over the values in the critical section
  // we might optimize numbers of copy and just block the hw serial buffer


  portENTER_CRITICAL(&EMHparseMutex);
  uint64_t sumVal = EMH_sumVal;
  uint32_t actVal = EMH_actVal;
  char manuf[4];
  strncpy (manuf, EMH_vendor,3);
  manuf[3] =0; // null terminated string
 
  char meterID[10];
  strncpy (meterID, EMH_serial,9);
  meterID[9] = 0;

  portEXIT_CRITICAL(&EMHparseMutex);

  meter1["manuf"] = manuf;
  meter1["serial"] = meterID;
  meter1["sumVal"] = sumVal;
  meter1["sumUnit"] = "Wh";
  meter1["actVal"] = actVal;
  meter1["actUnit"] = "W";
  
  JsonObject meter2 = meters.createNestedObject("meter2");
  meter2["manuf"] = "Conrad";
  meter2["serial"] = "765432";
  meter2["sumCnt"] = sumVal++;
  meter2["sumUnit"] = "10000/kWh";


  serializeJson(root, jsonresponse);
  server.send(200,"text/json",jsonresponse);
  
}