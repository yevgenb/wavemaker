#include <TimeLib.h>


// This is the list of recognized commands.  
// In order to receive, attach a callback function to these events
enum
{
  kCommandList    , 
  kSetMode        ,   
  kSetDuration    , 
  kSetSpeed       ,
  kTurnOn         ,
  kTurnOff        ,
  kSetTime        ,
  kSetNightMode   ,
  kStatus         ,
 
};

// Callbacks define on which received commands we take action
void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kCommandList, OnCommandList);
  cmdMessenger.attach(kSetMode, OnSetMode);
  cmdMessenger.attach(kSetDuration, OnSetDuration);
  cmdMessenger.attach(kSetSpeed, OnSetSpeed);
  cmdMessenger.attach(kTurnOn, OnTurnOn);
  cmdMessenger.attach(kTurnOff, OnTurnOff);
  cmdMessenger.attach(kStatus, OnStatus);
  cmdMessenger.attach(kSetTime, OnSetTime);
  cmdMessenger.attach(kSetNightMode, OnSetNightMode);
}

// Called when a received command has no attached function
void OnUnknownCommand()
{
  Serial.println(F("This command is unknown!"));
  OnCommandList();
}

// Callback function that sets mode
void OnSetMode()
{
  // Read led brightness argument, expects value between 0 to 255
  mode = cmdMessenger.readInt16Arg();
  saveConfig();
  Serial.println(F("OK"));
}

void OnSetDuration()
{
  // Read led brightness argument, expects value between 0 to 255
  int d = cmdMessenger.readInt16Arg();
  if (d < 100 || d > 50000)
  {
     Serial.println(F("ERROR: duration shoud be in the range of 100-50000"));
     return;
  }
  duration = d;
  saveConfig();
  Serial.println(F("OK")); 
}

void OnTurnOn()
{
  wavemaker_on = true;
}

void OnTurnOff()
{
  
  wavemaker_on = false;
}


void OnSetSpeed()
{
  int pump = cmdMessenger.readInt16Arg();
  if (pump < 0 || pump > 2) // we support two pumps only
  {
     Serial.println(F("ERROR: pump number should be 1 or 2"));
     return;
  }
  int speed = cmdMessenger.readInt16Arg();
  if (speed < 0 || speed > 255) 
  {
     Serial.println(F("ERROR: speed should be in the range of 0-255"));
     return;
  }
  if (pump == 1)
  {
    Pump1PWM_temp = speed;
  }
  else
  {
    Pump2PWM_temp = speed;
   }
  saveConfig();
  Serial.println(F("OK"));  
}

void OnStatus()
{
  Serial.print(mode);
  Serial.print(F(","));
  Serial.print(duration);
  Serial.print(F(","));
  Serial.print(Pump1PWM);
  Serial.print(F(","));
  Serial.print(Pump2PWM);
  Serial.print(F(","));
  Serial.print(now());
  Serial.print(F(","));
  Serial.print(night_mode_enabled);
  Serial.print(F(","));
  Serial.print(night_mode_start_hour);
  Serial.print(F(","));
  Serial.print(night_mode_start_min);
  Serial.print(F(","));
  Serial.print(night_mode_end_hour);
  Serial.print(F(","));
  Serial.print(night_mode_end_min);
  Serial.print(F(","));
  Serial.println(isNightActive()); 
}


void OnSetTime()
{
  unsigned long time = cmdMessenger.readInt32Arg();
  setTime(time);
  Serial.println(F("OK"));
}

void OnSetNightMode()
{
  night_mode_enabled = cmdMessenger.readBoolArg();
  if (night_mode_enabled) // read start/end times
  {
    int tmp = cmdMessenger.readInt16Arg();
    if (tmp < 0 || tmp > 23) 
    {
       Serial.println(F("ERROR: hours should be 0-23"));
       return;
    }
    night_mode_start_hour = tmp;
    tmp = cmdMessenger.readInt16Arg();
    if (tmp < 0 || tmp > 59) 
    {
       Serial.println(F("ERROR: minutes should be 0-59"));
       return;
    }
    night_mode_start_min = tmp;
    tmp = cmdMessenger.readInt16Arg();
    if (tmp < 0 || tmp > 23) 
    {
       Serial.println(F("ERROR: hours should be 0-23"));
       return;
    }
    night_mode_end_hour = tmp;
    tmp = cmdMessenger.readInt16Arg();
    if (tmp < 0 || tmp > 59) 
    {
       Serial.println(F("ERROR: minutes should be 0-59"));
       return;
    }
    night_mode_end_min = tmp;
  }
  else
  {
    night_mode_start_hour = 0;
    night_mode_start_min = 0;
    night_mode_end_hour = 0 ;
    night_mode_end_min = 0;
  }
  saveConfig();
  Serial.println(F("OK"));  
}

// Callback function that shows a list of commands
void OnCommandList()
{
  Serial.println(F("Available commands"));
  Serial.println(F(" 0;                 - This command list"));
  Serial.println(F(" 1,<mode>;     - Set mode:\n 1 - SINE WAVE DESYNCHRONIZED \n 2 - MODE: SINE WAVE SYNCHRONIZED\n 3 - TIDAL SWELL\n 4 - MODE: REEF CREST\n 5 - MODE: MANUAL"));
  Serial.println(F(" 2,<duration>; - Set duration of cycle (ms)")); 
  Serial.println(F(" 3,<pump num>, <speed>; - Set pump speed in manual mode. pump_num: 1 or 2. Speed: 0-255"));
  Serial.println(F(" 4; - Turn pumps on"));
  Serial.println(F(" 5; - Turn pumps off"));
  Serial.println(F(" 6,<seconds since 1/1/1970>; - Set time"));
  Serial.println(F(" 7,<0/1>,<start_hour>,<start_min>,<end_hour>,<end_min>; - Set night mode"));
  Serial.println(F(" 8; - Get status"));
  
}


