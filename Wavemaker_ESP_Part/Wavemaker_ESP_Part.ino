#include <Streaming.h>
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

struct info {
  unsigned int mode;
  unsigned int duration; // Cycle duration in milliseconds for the mode 3.
  
  // manual mode values
  byte Pump1Speed;
  byte Pump2Speed;
  
  // night mode:
  boolean night_mode_enabled;
  byte night_mode_start_hour;
  byte night_mode_start_min;
  byte night_mode_end_hour;
  byte night_mode_end_min;
};

SoftwareSerial swSer(D6, D5, false, 256);

void setup(void){
  SPIFFS.begin();
  Serial.begin(57600);
  swSer.begin(57600);

  MyWebServer.begin();  
  ArduinoOTA.setHostname(MyWebServer.DeviceName.c_str());  
  ArduinoOTA.begin();
  

  server.on("/pump1", handlePump1);
  server.on("/info", handleGetInfo);
  server.on("/set_night_end", handleSetNightEnd);
  server.on("/set_night_start", handleSetNightStart);
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
  info currState = getStateFromArduino();
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["mode"] = currState.mode;
  root["duration"] = currState.duration;
  root["pump_1_speed"] = currState.Pump1Speed;
  root["pump_2_speed"] = currState.Pump2Speed;
  root["night_enabled"] = currState.night_mode_enabled;
  JsonObject& night_start = jsonBuffer.createObject();
  night_start["hours"] =currState.night_mode_start_hour;
  night_start["minutes"] =currState.night_mode_start_min;
  JsonObject& night_end = jsonBuffer.createObject();
  night_end["hours"] =currState.night_mode_start_hour;
  night_end["minutes"] =currState.night_mode_start_min;
  
  root["night_start"] = night_start;
  root["night_end"] = night_end;
  
  char buffer[256];
  root.printTo(buffer, sizeof(buffer));

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


