#include <Streaming.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>
#include <FS.h>
#include <myWebServerAsync.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

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
  long curr_time;
};

info currState;

time_t lastNtpUpdate = 0;

SoftwareSerial swSer(D6, D5, false, 256);

void setup(void){
  SPIFFS.begin();
  Serial.begin(57600);
  swSer.begin(57600);

  WebServer.begin();  
  ArduinoOTA.setHostname(WebServer.DeviceName.c_str());  
  ArduinoOTA.begin();
  

  server.on("/pump1", handlePump1);
  server.on("/info", handleGetInfo);
  server.on("/set_night_end", handleSetNightEnd);
  server.on("/set_night_start", handleSetNightStart);
  server.on("/set_mode", handleSetMode);
}

void loop(void)
{
  ArduinoOTA.handle();
  WebServer.handle();
  // update arduino time if needed:
  if ( (lastNtpUpdate == 0 || (now() - lastNtpUpdate > WebServer.UpdateNTPEvery)*60)
      && timeStatus() == timeSet)
  {
      //syncTime();
  }
}

void handlePump1(AsyncWebServerRequest *request)
{
  // get the value of request argument "state" and convert it to an int
  if (request->hasArg("speed"))
  {
    int speedPct = request->arg("speed").toInt();
    int speed = map(speedPct,0,100,0,255);
    String speedstr = String(speed);
   
    request->send(200, "text/plain", String("Pump 1 is now at ") + speedstr);
  }
}

void handleGetInfo(AsyncWebServerRequest *request)
{  
  //getStateFromArduino();
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["mode"] = currState.mode;
  root["duration"] = currState.duration;
  root["pump_1_speed"] = currState.Pump1Speed;
  root["pump_2_speed"] = currState.Pump2Speed;
  root["curr_time"] = currState.curr_time;
  root["night_enabled"] = currState.night_mode_enabled;
  JsonObject& night_start = jsonBuffer.createObject();
  night_start["hours"] =currState.night_mode_start_hour;
  night_start["minutes"] =currState.night_mode_start_min;
  JsonObject& night_end = jsonBuffer.createObject();
  night_end["hours"] =currState.night_mode_end_hour;
  night_end["minutes"] =currState.night_mode_end_min;
  
  root["night_start"] = night_start;
  root["night_end"] = night_end;
  
  
  char buffer[256];
  root.printTo(buffer, sizeof(buffer));

  request->send(200, "application/json", buffer);
}

void handleSetNightStart(AsyncWebServerRequest *request)
{
  if (request->hasArg("hours") && request->hasArg("minutes"))
  {
    int hours =  request->arg("hours").toInt();
    int minutes =  request->arg("minutes").toInt();
    
  // TODO: validate here:
    currState.night_mode_start_min = minutes;
    currState.night_mode_start_hour = hours;
    char str[100];
    if (setNightMode(str))
    {
      sprintf(str,"Night ends now at %2d:%2d",hours, minutes);
      request->send(200, F("text/plain"), str);
    }
    else
    {
       request->send(500, F("text/plain"), str);
    }
  }
  else
  {
    sendBadRequest(request);
  }
}

void handleSetNightEnd(AsyncWebServerRequest *request)
{
  if (request->hasArg("hours") && request->hasArg("minutes"))
  {
    int hours =  request->arg("hours").toInt();
    int minutes =  request->arg("minutes").toInt();

    // TODO: validate here:
    currState.night_mode_end_min = minutes;
    currState.night_mode_end_hour = hours;
    char str[100];
    if (setNightMode(str))
    {
      sprintf(str,"Night ends now at %2d:%2d",hours, minutes);
      request->send(200, F("text/plain"), str);
    }
    else
    {
       request->send(500, F("text/plain"), str);
    }
  }
  else
  {
    sendBadRequest(request);
  }
}

void handleSetMode(AsyncWebServerRequest *request)
{
   if (request->hasArg("mode"))
  {
    int mode =  request->arg("mode").toInt();
   
    
    // TODO: validate here:
    currState.mode = mode;
    char str[100];
    if (setMode(str))
    {
      sprintf(str,"Mode set to %2d", mode);
      request->send(200, F("text/plain"), str);
    }
    else
    {
       request->send(500, F("text/plain"), str);
    }
  }
  else
  {
    sendBadRequest(request);
  }
}


void sendBadRequest(AsyncWebServerRequest *request)
{
  request->send(400, "text/plain", F("Bad request"));
}



