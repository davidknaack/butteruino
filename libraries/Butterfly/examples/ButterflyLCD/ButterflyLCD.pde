#include <butterfly_temp.h>
#include <LCD_Driver.h>
#include <timer2_RTC.h>

void setup()
{
  // Print a string from program memory. Faster than print, 
  // and uses less ram than prints
  LCD.prints_f(PSTR("BUTTERDUINO"));
  delay( 3000 );
  
  // Print a string from RAM. Faster than print, and can
  // print strings that are not set at compile time.
  LCD.prints("TEMP Sensor");
  delay( 3000 );
  
  // Set up the RTC timer to call the secTick() function every second.
  RTCTimer.init( secTick );
}

void secTick()
{
  // print() and println() functions can also be used to print
  // strings, and are not as efficient as prints(). However, both
  // will append to whatever is on the display. println() will cause
  // the display to be cleared before the next character is printed.
  LCD.print( TempSense.getTemp(FAHRENHEIT) );
  LCD.println( " F" );
}

void loop()
{
  
}

