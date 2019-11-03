#include "myWebServer.h"
#include "mysml.h"
#include "myS0Cnt.h"


int sumVal = 5800;
int humidity;

void handlegetData();
void handlegetrawSML();

void setup()
{

  Serial.begin(115200);
  //for esp32 it seems to be UART_1 on the USB bridge
  //UART_0 and _2 aer on the pin layout
  //we can mix espressif API and Arduino API)

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500); Serial.print(":-(");
  }

  Serial.println("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  //the basics moved to myWebServer.h
  setup_WebServer();

  server.on("/getdata", handlegetData);
  server.on("/getrawsml", handlegetrawSML);

  server.begin();
  Serial.println("HTTP server started");
  deltatime = 3600000;
  setup_uart();
  setup_S0(18);
}

void loop()
{
  //check WiFi Connection
  while (WiFi.status() != WL_CONNECTED)
  {delay(500); Serial.print(":-(");}

  handle_uart_event();
  server.handleClient();
}

// not best practice, we keep it here to have access on the global variables
// ToDo find a better solution
// is a callback from the Web-Server

void handlegetrawSML()
{
//ugly hack could have race conditions with len and data
char datapage[2048];
char *dpp = datapage;

for (int i=0;i <len; i++) {
    sprintf(dpp +i*2, "%02X", data[i]);
  }
datapage[len] = 0;
server.send(200,"text/plain", datapage);
}

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
  //that are the only values which could change periodicly
  uint64_t sumVal = EMH_sumVal;
  uint32_t actVal = EMH_actVal;
  portEXIT_CRITICAL(&EMHparseMutex);

  char manuf[4];
  strncpy (manuf, EMH_vendor,3);
  manuf[3] =0; // null terminated string
  char meterID[27]; //9 x 2 digit + 8 seperator + 1x 0x0
  //strncpy (meterID, EMH_serial,9);
  sprintf(meterID, "%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X", EMH_serial[0], EMH_serial[1], EMH_serial[2], EMH_serial[3], EMH_serial[4], EMH_serial[5], EMH_serial[6],EMH_serial[7],EMH_serial[8]);
  meterID[26] = 0; // 0 terminated string

  meter1["manuf"] = manuf;
  meter1["serial"] = meterID;
  meter1["sumVal"] = sumVal;
  meter1["sumUnit"] = "0.1 Wh";
  meter1["actVal"] = actVal;
  meter1["actUnit"] = "0.1 W";
  JsonObject meter2 = meters.createNestedObject("meter2");
  meter2["manuf"] = "KDK3-80";
  meter2["serial"] = "765432";
  meter2["sumCnt"] = sumCnt;
  meter2["sumUnit"] = "1000/kWh";
  meter2["actWatt"] = 3600000/deltatime;
  meter2["actdeltaTime_ms"] = millis() - oldtime;

  serializeJson(root, jsonresponse);
  server.send(200,"text/json",jsonresponse);
}