#include <timer2_RTC.h>

void setup() 
{ 
  pinMode(SPEAKER, OUTPUT);  
  
  Serial.begin(9600); 
  Serial.println("Butterfly RTC Example");

// The RTCTimer can be started with no 'tick' callback. In this
// case you must check regularly to see if the time has changed.
//  RTCTimer.init( 0 );  

// The RTCTimer can also be started with a 'tick' callback. In this
// case there is no need to check whether the time has changed,
// RTCTimer will call the routine you specify when the time changes.
  RTCTimer.init( secTick );
} 

void secTick()
{
  beep();
  Serial.print(RTCTimer.hour, DEC);
  Serial.print(":");
  Serial.print(RTCTimer.minute, DEC);
  Serial.print(":");
  Serial.println(RTCTimer.second, DEC);
}

void loop()
{
  /*  
  // When you are not using the 'tick' callback, you may
  // check the RTCTimer.timeChanged variable to see if
  // the time variables have been updated. Each time the
  // second timer ticks timeChanged will be updated. You
  // can reset timeChanged at any time, or you may leave
  // it to overflow when it reaches a value of 255.
  if (RTCTimer.timeChanged > 0){
    secTick();
    RTCTimer.timeChanged = 0;
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
