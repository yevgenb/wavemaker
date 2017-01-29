
info state;

info getStateFromArduino()
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
   state.mode = getValue(str,',',0).toInt();
   state.duration = getValue(str,',',1).toInt();
   state.Pump1Speed = getValue(str,',',2).toInt();
   state.Pump2Speed = getValue(str,',',3).toInt();
   
   state.night_mode_enabled = getValue(str,',',4).toInt();
   state.night_mode_start_hour = getValue(str,',',5).toInt();
   state.night_mode_start_min = getValue(str,',',6).toInt();
   state.night_mode_end_hour = getValue(str,',',7).toInt();
   state.night_mode_end_min = getValue(str,',',8).toInt();

   Serial << "Parsed" << state.mode << ":" << state.duration << ":" << state.Pump1Speed << ":" << state.Pump2Speed;
   Serial << ":" << state.night_mode_enabled << ":" << state.night_mode_start_hour << ":" << state.night_mode_start_min;
   Serial << ":" << state.night_mode_end_hour << ":" << state.night_mode_end_min << "\n";
   return state;
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
