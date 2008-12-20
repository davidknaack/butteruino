#include "butterfly_temp.h"
#include "WProgram.h"
#include "wiring.h"
#include <avr/pgmspace.h>
#include "tempslookup.h"


TempSensor::TempSensor(int units)
{
    this->units = units;
    overSample = false;
}

int TempSensor::getTemp()
{
  return getTemp(this->units);
}

int TempSensor::getTemp(int units)
{
  // get the first sample from the temp sensor
  int v = analogRead(TEMP);

  // if using oversampling add 7 more samples and then divide by 8
  if(overSample){
    for(int i=0; i<7; i++)    
      v += analogRead(TEMP);
    v = v >> 3;
  }
  
  // convert the a2d reading to temperature, depending on units setting
  switch (units) {
    case CELSIUS:
      v = mapToC(v);
      break;
      
    case FAHRENHEIT:
      v = mapToF(v);
      break;
  }
  
  return v;
}

int TempSensor::mapToF(int a2d)
{  
  int i;
  for (i=0; i<=141; i++){   // Find the temperature
    if ((prog_uint16_t)a2d > pgm_read_word_near( TEMP_Fahrenheit_pos +i )){
       break;
    }
  }     
  return i;
}

int TempSensor::mapToC(int a2d)
{
  int v = 0;
  if( a2d > 810){   // If it's a negative temperature
    for (int i=0; i<=25; i++){   // Find the temperature
      if ((prog_uint16_t)a2d <= pgm_read_word_near( TEMP_Celsius_neg + i)){
        v = 0-i; // Make it negative
        break;
      }
    }
  } 
  else if ( a2d < 800 ) {  // If it's a positive temperature
    for (int i=0; i<100; i++) {
      if ((prog_uint16_t)a2d >= pgm_read_word_near( TEMP_Celsius_pos + i)){
        v = i; 
        break;
      }
    }        
  }
  return v;
}

// Set up an instance 
TempSensor TempSense = TempSensor(CELSIUS);
