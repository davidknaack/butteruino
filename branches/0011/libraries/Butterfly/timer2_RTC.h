/* Timer2 watch crystal based RTC library 

	This is a very basic RTC clock library that assumes that
	there is a 32768Hz watch crystal attached to Timer2. It	does
	not provide any functions for rendering the time to a string 
	or for 12/24 hour formatting, as these are interface issues.
	
	A tick callback is provided for the user to attach to if desired.
	The callback is in the context of the ISR, so code attached to
	it should be kept as fast as possible. 
	
	The timeChanged flag is incremented in the timerTick routine. It may
	be monitored by the user code to react to clock changes without	attaching
	to the ISR. The user code is responsible for resetting the flag.
 */

#ifndef timer2_RTC_h
#define timer2_RTC_h
#include <avr/pgmspace.h>
#include <stdint.h>

class Timer2RTC {
public:
	typedef void (*ClockChangeCallback_t) ();

	ClockChangeCallback_t clockChangeCallback;
	volatile uint8_t  timeChanged;
	volatile uint8_t  second;
	volatile uint8_t  minute;
	volatile uint8_t  hour;
	volatile uint8_t  day;
	volatile uint8_t  month;
	volatile uint16_t year;

	Timer2RTC( void );
	void init( ClockChangeCallback_t clockChangeCallback );
	void timerTick();
};

extern Timer2RTC RTCTimer;

#endif