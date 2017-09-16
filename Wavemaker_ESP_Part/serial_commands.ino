


void getStateFromArduino()
{ 
   // send request to arduino
   swSer.flush();
   swSer.println(F("8;"));
   swSer.flush();
   // wait for response
   unsigned int timeout = 0;
 
    while ( swSer.available() ==0 ) {
    yield();
      if( ++timeout > 1000){ // set this to your timeout value in milliseconds
         // your error handling code here
        break;
      }
    }
   String str = swSer.readStringUntil('\n');
   Serial.println(F("Received from arduino:"));
   Serial.println(str);
   currState.mode = getValue(str,',',0).toInt();
   currState.duration = getValue(str,',',1).toInt();
   currState.Pump1Speed = getValue(str,',',2).toInt();
   currState.Pump2Speed = getValue(str,',',3).toInt();

   currState.curr_time = getValue(str,',',4).toInt();
   currState.night_mode_enabled = getValue(str,',',5).toInt();
   currState.night_mode_start_hour = getValue(str,',',6).toInt();
   currState.night_mode_start_min = getValue(str,',',7).toInt();
   currState.night_mode_end_hour = getValue(str,',',8).toInt();
   currState.night_mode_end_min = getValue(str,',',9).toInt();
   
    
   Serial << "Parsed" << currState.mode << ":" << currState.duration << ":" << currState.Pump1Speed << ":" << currState.Pump2Speed;
   Serial << ":" << currState.night_mode_enabled << ":" << currState.night_mode_start_hour << ":" << currState.night_mode_start_min;
   Serial << ":" << currState.night_mode_end_hour << ":" << currState.night_mode_end_min << ":" << currState.curr_time << "\n";
}

void syncTime()
{
     Serial << F("Sending time to Arduino \n");
    // send request to arduino
     swSer.flush();
     swSer.print(F("6,"));
     swSer.print(now());
     swSer.println(F(";"));
     swSer.flush();
     // wait for response
     unsigned int timeout = 0;
   
      while ( swSer.available() ==0 ) 
      {
        yield();
        if( ++timeout > 1000)
        { // set this to your timeout value in milliseconds
           // your error handling code here
          break;
        }
      }
      String str = swSer.readString();
      Serial << F("Time rensponse received:") << str << "\n";
      if (str.startsWith("OK"))
      {
        Serial << F("Time on Arduino set to ") << now() << "\n";
        lastNtpUpdate = now();
      }
}


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean  setNightMode(char* responseBuff)
{
  Serial << F("Sending mode to Arduino : \n");
  Serial << "7," << currState.night_mode_enabled << "," << currState.night_mode_start_hour << "," << currState.night_mode_start_min
  << "," << currState.night_mode_end_hour << "," << currState.night_mode_end_min << ";" ; 
  
  swSer << "7," << currState.night_mode_enabled << "," << currState.night_mode_start_hour << "," << currState.night_mode_start_min
  << "," << currState.night_mode_end_hour << "," << currState.night_mode_end_min << ";" ; 

   // wait for response
   unsigned int timeout = 0;
   
   while ( swSer.available() ==0 ) 
   {
      yield();
      if( ++timeout > 1000)
      { // set this to your timeout value in milliseconds
         // your error handling code here
        break;
      }
   }
   String str = swSer.readStringUntil('\n');
   Serial << F("SetNightMode rensponse received:") << str << "\n";
   if (str.startsWith("OK"))
   {
       return true;
   }
   else
   {
       str.toCharArray(responseBuff, 100);
       return false;
   }
}

boolean  setMode(char* responseBuff)
{
  Serial << F("Sending mode to Arduino : \n");
  Serial << "1," << currState.mode << ";"; 
  
  swSer << "1," << currState.mode << ";"; 

   // wait for response
   unsigned int timeout = 0;
   
   while ( swSer.available() ==0 ) 
   {
      yield();
      if( ++timeout > 1000)
      { // set this to your timeout value in milliseconds
         // your error handling code here
        break;
      }
   }
   String str = swSer.readStringUntil('\n');
   Serial << F("SetMode rensponse received:") << str << "\n";
   if (str.startsWith("OK"))
   {
       return true;
   }
   else
   {
       str.toCharArray(responseBuff, 100);
       return false;
   }
}
