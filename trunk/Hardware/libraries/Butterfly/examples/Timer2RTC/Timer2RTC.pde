#include <timer2_RTC.h>

void setup() 
{ 
  pinMode(SPEAKER, OUTPUT);  
  
  Serial.begin(9600); 
  Serial.println("Butterfly RTC Example");

// Timer2RTC can be started with no 'tick' callback. In this
// case you must check regularly to see if the time has changed.
// If you need to do something that takes more than a moment this
// is a good way to do it. 
//  Timer2RTC::init( 0 );  

// Timer2RTC can also be started with a 'tick' callback. In this
// case there is no need to check whether the time has changed,
// Timer2RTC will call the routine you specify when the time changes.
  Timer2RTC::init( secTick );
} 

void secTick()
{
  beep();
  Serial.print(Timer2RTC::hour, DEC);
  Serial.print(":");
  Serial.print(Timer2RTC::minute, DEC);
  Serial.print(":");
  Serial.println(Timer2RTC::second, DEC);
}

void loop()
{
  /*  
  // When you are not using the 'tick' callback, you may
  // check the Timer2RTC::timeChanged variable to see if
  // the time variables have been updated. Each time the
  // second timer ticks timeChanged will be updated. You
  // can reset timeChanged at any time, or you may leave
  // it overflow when it reaches a value of 255.
  if (Timer2RTC::timeChanged > 0){
    secTick();
    Timer2RTC::timeChanged = 0;
  }
  //*/
}

void beep()
{
  for (int i=0; i<20; i++)
  {
      digitalWrite(SPEAKER,HIGH);
      delayMicroseconds(50);
      digitalWrite(SPEAKER,LOW);
      delayMicroseconds(50);    
  }
}
