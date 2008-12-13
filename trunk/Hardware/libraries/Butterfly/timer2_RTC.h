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
#include <stdint.h>

namespace Timer2RTC {
	typedef void (*ClockChangeCallback_t) ();

	extern ClockChangeCallback_t clockChangeCallback;
	extern volatile uint8_t  timeChanged;
	extern volatile uint8_t  second;
	extern volatile uint8_t  minute;
	extern volatile uint8_t  hour;
	extern volatile uint8_t  day;
	extern volatile uint8_t  month;
	extern volatile uint16_t year;

	void init( ClockChangeCallback_t clockChangeCallback );
	void timerTick();
}

#endif