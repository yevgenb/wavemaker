#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>
#include <FS.h>
#include <WiFiManager.h>  
#include <myWebServer.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <Time.h>



void setup(void){
  SPIFFS.begin();
  Serial.begin(115200);
  Serial1.begin(57600);
  
  // OTA part:
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  /*
  WiFi.begin(ssid, password);
  Serial.println("");

  Serial.print("Connecting to WIFI...");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  */
  MyWebServer.begin();  
  ArduinoOTA.setHostname(MyWebServer.DeviceName.c_str());  
  ArduinoOTA.begin();
  

  server.on("/pump1", handlePump1);
  server.on("/info", handleGetInfo);
  server.on("/set_night_end", handleSetNightEnd);
  server.on("/set_night_start", handleSetNightStart);

  //server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  ArduinoOTA.handle();
  MyWebServer.handle();
}

void handlePump1()
{
  // get the value of request argument "state" and convert it to an int
  if (server.hasArg("speed"))
  {
    int speedPct = server.arg("speed").toInt();
    int speed = map(speedPct,0,100,0,255);
    String speedstr = String(speed);
   
    server.send(200, "text/plain", String("Pump 1 is now at ") + speedstr);
  }
}

void handleGetInfo()
{
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["mode"] = random(1,5);
  root["duration"] = random(1000,10000);
  root["pump_1_speed"] = random(128,255);
  root["pump_2_speed"] = random(128,255);
  root["night_enabled"] = true;
  JsonObject& night_start = jsonBuffer.createObject();
  night_start["hours"] = hour(now()) -2;
  night_start["minutes"] = 0;
  JsonObject& night_end = jsonBuffer.createObject();
  night_end["hours"] = hour(now()) +2;
  night_end["minutes"] = 0;
  
  root["night_start"] = night_start;
  root["night_end"] = night_end;
  
  char buffer[256];
  root.printTo(buffer, sizeof(buffer));
  Serial.println(buffer);

  server.send(200, "application/json", buffer);
}

void handleSetNightStart()
{
   if (server.hasArg("hours") && server.hasArg("minutes"))
  {
    int hours =  server.arg("hours").toInt();
    int minutes =  server.arg("minutes").toInt();
    
    char str[100];
    sprintf(str,"Night starts now at %2d:%2d",hours, minutes);
    server.send(200, "text/plain", str);
  }
  else
  {
    sendBadRequest();
  }
}

void handleSetNightEnd()
{
  if (server.hasArg("hours") && server.hasArg("minutes"))
  {
    int hours =  server.arg("hours").toInt();
    int minutes =  server.arg("minutes").toInt();
    
    char str[100];
    sprintf(str,"Night ends now at %2d:%2d",hours, minutes);
    server.send(200, "text/plain", str);
  }
  else
  {
    sendBadRequest();
  }
}


void sendBadRequest()
{
  server.send(400, "text/plain", "Bad request");
}

