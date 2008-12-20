#include "butterfly_temp.h"
#include "WProgram.h"
#include "wiring.h"
#include <avr/pgmspace.h>

#warning Ignore these three warnings about initialized variables in program memory area
// This table is from the AVR Butterfly sample from ATMEL
prog_uint16_t TEMP_Fahrenheit_pos[] PROGMEM =  
{ // Positive Fahrenheit temps (ADC-value) for 0 to 140 degrees F  
938, 935, 932, 929, 926, 923, 920, 916, 913, 909, 906, 902, 898, 
894, 891, 887, 882, 878, 874, 870, 865, 861, 856, 851, 847, 842, 
837, 832, 827, 822, 816, 811, 806, 800, 795, 789, 783, 778, 772, 
766, 760, 754, 748, 742, 735, 729, 723, 716, 710, 703, 697, 690, 
684, 677, 670, 663, 657, 650, 643, 636, 629, 622, 616, 609, 602, 
595, 588, 581, 574, 567, 560, 553, 546, 539, 533, 526, 519, 512, 
505, 498, 492, 485, 478, 472, 465, 459, 452, 446, 439, 433, 426, 
420, 414, 408, 402, 396, 390, 384, 378, 372, 366, 360, 355, 349, 
344, 338, 333, 327, 322, 317, 312, 307, 302, 297, 292, 287, 282, 
277, 273, 268, 264, 259, 255, 251, 246, 242, 238, 234, 230, 226, 
222, 219, 215, 211, 207, 204, 200, 197, 194, 190, 187, 
};

// This table is from the AVR Butterfly sample from ATMEL
prog_uint16_t TEMP_Celsius_pos[] PROGMEM =    
{ // Positive Celsius temps (ADC-value) for 0 to 60 degrees C
806,796,786,775,765,754,743,732,720,709,697,685,673,661,649,
636,624,611,599,586,574,562,549,537,524,512,500,488,476,464,
452,440,429,418,406,396,385,374,364,354,344,334,324,315,306,
297,288,279,271,263,255,247,240,233,225,219,212,205,199,193,
187,
};

prog_uint16_t TEMP_Celsius_neg[] PROGMEM =    
{  // Negative Celsius temps (ADC-value) from -1 to -15 degrees C
815,825,834,843,851,860,868,876,883,891,898,904,911,917,923,
};


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
