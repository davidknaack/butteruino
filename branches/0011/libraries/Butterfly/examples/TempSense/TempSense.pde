/*
 * TempSense
 *
 * Read the Butterfly temperature sensor and output
 * the result in either Celsius or Fahrenheit. Also
 * supports 8x oversampling for improved accuracy.  
 *
 */
 
#include <butterfly_temp.h>

void setup() {
  Serial.begin(9600); 
}

void doSamples(){
  Serial.print("Default: ");
  Serial.println( TempSense.getTemp() );  
  
  Serial.print("CELSIUS: ");
  Serial.println( TempSense.getTemp(CELSIUS) );  

  Serial.print("FAHRENHEIT: ");
  Serial.println( TempSense.getTemp(FAHRENHEIT) );  
}

void loop() {  
  Serial.println("** With oversampling off **");
  doSamples();
  
  Serial.println("** With oversampling on **");
  TempSense.overSample = true;
  doSamples();

  Serial.println("** Reset default **");
  TempSense.units = FAHRENHEIT;
  doSamples();

  while(true);
}
