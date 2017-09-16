

unsigned long millis_last_change = 0;
unsigned int count = 1;

// pump values:
byte Pump1PWM = 0;
byte Pump2PWM = 0;


const float POWER_PUMP1 = 0.5; // 50%
const float POWER_PUMP2 = 0.5; // 50%



void Wavemaker()
{
  int value = 0;
  long time_ms = millis();

  if (mode == 1) // sin anti-sync period: 100-10000
  {
    value = int(128 + 127 * sin(2  *  PI / duration * time_ms));
    Pump1PWM = 255 - value;
    Pump2PWM = value;
  }

  else if (mode == 2)  //sin sync period: 100-10000
  {
    value = int(128 + 127 * sin(2  *  PI / duration * time_ms));
    Pump1PWM = Pump2PWM = value;
  }

  else if (mode == 3) // tidal-swell period: 1000-9500
  {
    static byte PWM1 [] = {
      128, 172, 205, 222, 225, 214, 187, 190, 203, 205, 198, 181, 173, 182, 185, 181, 171, 155, 159, 163, 163, 157, 146, 128, 210, 246, 254, 240, 207, 128
    }; // Opposite
    static byte PWM2 [] = {
      128, 146, 157, 163, 163, 159, 155, 171, 181, 185, 182, 173, 181, 198, 205, 203, 190, 187, 214, 225, 222, 205, 172, 128, 210, 246, 254, 240, 207, 128
    };  // Same

    int millis1 = millis() - millis_last_change;

    Pump1PWM = int(map(millis1, 0, duration, PWM1 [count], PWM1 [count + 1]));
    Pump2PWM = int(map(millis1, 0, duration, PWM2 [count], PWM2 [count + 1]));

    if ((millis() - millis_last_change) >= duration)
    {
      millis_last_change = millis();
      count++;
      //Serial.print(Pump1PWM);Serial.print(":");Serial.println(Pump2PWM);
    }

    if ( count >= 29) // Opposite and same
    {
      count = 0;
    }
  }

  else if (mode == 4) // reef-crest period: 1000-5000
  {

    if ((millis() - millis_last_change) >= duration)
    {
      millis_last_change = millis();
      count += 1;
      value = int(128 + 127 * sin(2 * PI / 400 * time_ms));
      Pump1PWM = 255 - value;
      Pump2PWM = value;

      if ((count < 61) || (count > 81))
      {
        if (Pump1PWM < 128)
        {
          Pump1PWM += 128;
        }
        if (Pump2PWM < 128)
        {
          Pump2PWM += 128;
        }
      }
      else if ((count >= 61) && (count <= 81))
      {
        if (Pump1PWM < 128)
        {
          Pump1PWM += 50;
        }
        if (Pump2PWM < 128)
        {
          Pump2PWM += 50;
        }
      }
      //Serial.print(Pump1PWM);Serial.print(":");Serial.println(Pump2PWM);
    }
    if (count >= 142) // Opposite and same
    {
      count = 0;
    }
   
  }
  else if (mode == 5) // manual
  {
    Pump1PWM = Pump1PWM_temp;
    Pump2PWM = Pump2PWM_temp;
  }
  if (wavemaker_on == false)
  {
    Pump1PWM = Pump2PWM = 0;
  }

  // night mode
  if (isNightActive())
  {
    if ((millis() - millis_last_change) >= duration)
    {
      millis_last_change = millis();
      Pump1PWM = random(128,150);
      Pump2PWM = random(128,150);;
    }
    
  }

  analogWrite(wavemaker1, Pump1PWM);
  analogWrite(wavemaker2, Pump2PWM);
}

