#include <avr/wdt.h>
#include <EEPROM.h>
#include <CmdMessenger.h>  // CmdMessenger
#include <time.h>
#include <SoftwareSerial.h>
//#define WATCHDOG


unsigned int config_signature = 12389021; // dummy value for init flag

const byte wavemaker1 = 5;     // Pin wavemaker 1
const byte wavemaker2 = 6;     // Pin wavemaker 2

/**
 * mode:
 * 1- MODE: SINE WAVE DESYNCHRONIZED
 * 2- MODE: SINE WAVE SYNCHRONIZED
 * 3- MODE: TIDAL SWELL
 * 4- MODE: REEF CREST
 * 5- MODE: MANUAL
 */
unsigned int mode = 1;
unsigned int duration = 5000; // Cycle duration in milliseconds for the mode 3.

// manual mode values
byte Pump1PWM_temp = 255;
byte Pump2PWM_temp = 255;

// turn on/off pumps:
boolean wavemaker_on = true;

// night mode:
boolean night_mode_enabled = false;
byte night_mode_start_hour;
byte night_mode_start_min;
byte night_mode_end_hour;
byte night_mode_end_min;

// Attach a new CmdMessenger object to the default Serial port
SoftwareSerial soft_serial(4,3);
CmdMessenger cmdMessenger = CmdMessenger(soft_serial);





void setup() {
//  setTime(0);
#ifdef WATCHDOG // Do not change this line!
  wdt_disable();
#endif // Do not change this line!
  Serial.begin(57600);
  Serial.println("Started");
  soft_serial.begin(57600);
  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  pinMode(wavemaker1, OUTPUT);             // Pino 45;
  pinMode(wavemaker2, OUTPUT);             // Pino 46;


  loadConfig();
#ifdef WATCHDOG // Do not change this line!
  wdt_enable(WDTO_8S);
#endif // Do not change this line! 
}

void loop() {
  // read commands from serial:
  cmdMessenger.feedinSerialData();
    
  // put your main code here, to run repeatedly:
  Wavemaker();
}

void saveConfig()
{
  
  int eeAddress = 0;
  EEPROM.put(eeAddress, config_signature);
  eeAddress +=sizeof(int);
  EEPROM.put(eeAddress, mode);
  eeAddress +=sizeof(int);
  EEPROM.put(eeAddress, duration);
  eeAddress +=sizeof(int);
  EEPROM.put(eeAddress, Pump1PWM_temp);
  eeAddress +=sizeof(byte);
  EEPROM.put(eeAddress, Pump2PWM_temp);
  eeAddress +=sizeof(byte);
  EEPROM.put(eeAddress, night_mode_enabled);
  eeAddress +=sizeof(boolean);
  EEPROM.put(eeAddress, night_mode_start_hour);
  eeAddress +=sizeof(byte);
  EEPROM.put(eeAddress, night_mode_start_min);
  eeAddress +=sizeof(byte);
  EEPROM.put(eeAddress, night_mode_end_hour);
  eeAddress +=sizeof(byte);
  EEPROM.put(eeAddress, night_mode_end_min);
  eeAddress +=sizeof(byte);  
}


void loadConfig()
{
  int eeAddress = 0;
  unsigned int read_signature;
  EEPROM.get(eeAddress, read_signature);
  if (read_signature != config_signature)
  {
    return;
  }
  
  eeAddress +=sizeof(int);
  EEPROM.get(eeAddress, mode);
  eeAddress +=sizeof(int);
  EEPROM.get(eeAddress, duration);
  eeAddress +=sizeof(int);
  EEPROM.get(eeAddress, Pump1PWM_temp);
  eeAddress +=sizeof(byte);
  EEPROM.get(eeAddress, Pump2PWM_temp);
  eeAddress +=sizeof(byte);
  EEPROM.get(eeAddress, night_mode_enabled);
  eeAddress +=sizeof(boolean);
  EEPROM.get(eeAddress, night_mode_start_hour);
  eeAddress +=sizeof(byte);
  EEPROM.get(eeAddress, night_mode_start_min);
  eeAddress +=sizeof(byte);
  EEPROM.get(eeAddress, night_mode_end_hour);
  eeAddress +=sizeof(byte);
  EEPROM.get(eeAddress, night_mode_end_min);
  eeAddress +=sizeof(byte);  
}



time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet);         //convert to time_t
}

bool isNightActive()
{
  if (night_mode_enabled)
  {
    byte currHour = hour();
    byte currMin = minute();
    time_t currHourMin = tmConvert_t(0,0,0,currHour,currMin,0);
    time_t startHourMin = tmConvert_t(0,0,0,night_mode_start_hour,night_mode_start_min,0);
    time_t endHourMin = tmConvert_t(0,0,0,night_mode_end_hour,night_mode_end_min,0);
  
    if ( startHourMin < endHourMin)
    {
       return ((currHourMin >= startHourMin) && (currHourMin <= endHourMin));
    }
    else // overnight
    {  
       return ((currHourMin >= startHourMin) || (currHourMin <= endHourMin));
    }
  }
  else
  {
    return false;
  }
}
