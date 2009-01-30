#include <avr/io.h>
#include <avr/interrupt.h>
#include <wiring.h>

#include "timer2_RTC.h"

// http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=397642#397642
// Month length lookup table.
extern const char PROGMEM MonthLength[];
const char MonthLength[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// An instance of the timer
Timer2RTC RTCTimer = Timer2RTC();

/*
Timer2RTC::ClockChangeCallback_t Timer2RTC::clockChangeCallback;
volatile uint8_t  Timer2RTC::timeChanged;
volatile uint8_t  Timer2RTC::second;
volatile uint8_t  Timer2RTC::minute;
volatile uint8_t  Timer2RTC::hour;
volatile uint8_t  Timer2RTC::day;
volatile uint8_t  Timer2RTC::month;
volatile uint16_t Timer2RTC::year;
*/
	
ISR(TIMER2_OVF_vect) {
  RTCTimer.timerTick();
}

Timer2RTC::Timer2RTC( void )
{
	init(0);
}

void Timer2RTC::init( ClockChangeCallback_t userCallback )
{
	clockChangeCallback = userCallback;

	// Startup date and time.
	timeChanged = 0;
	second		= 0;
	minute		= 0;
	hour		= 0;
	day			= 0;
	month		= 0;
	year		= 0;
		
    cli(); 

    TIMSK2 &= ~(1<<TOIE2);               // disable OCIE2A and TOIE2
    ASSR    =  (1<<AS2);                 // select asynchronous operation of Timer2
    TCNT2   = 0;                         // clear TCNT2A
    TCCR2A |=  (1<<CS22);                // select precaler: 32.768 kHz / 64 = 1 sec between each overflow
    while((ASSR & 0x01) | (ASSR & 0x04));// wait for TCN2UB and TCR2UB to be cleared
    TIFR2   = 0xFF;                      // clear interrupt-flags
    TIMSK2 |= (1<<TOIE2);                // enable Timer2 overflow interrupt

    sei();
}

void Timer2RTC::timerTick(void)
{
	static char LeapMonth;
    second++;               

    if (second == 60) {
        second = 0;
        minute++;

        if (minute > 59) {
            minute = 0;
            hour++;

            if (hour > 23) {
                hour = 0;
                day++;

                // If February check for leap year 
                if (month == 2)
                    if (!(year & 0x0003))  
                        if (year%100 == 0)
                            if (year%400 == 0)
                                LeapMonth = 1;
                            else
                                LeapMonth = 0;
                        else
                            LeapMonth = 1;
                    else
                        LeapMonth = 0;
                else
                    LeapMonth = 0;

                // Check for month length
                if (day > (MonthLength[month] + LeapMonth)) {
                    day = 1;
                    month++;

                    if (month > 12) {
                        month = 1;
                        year++;
                    }
                }
            }
        }
    }

	timeChanged++;
	// If the user has provided a callback, call it now.
	if (clockChangeCallback != 0)
		clockChangeCallback();
}

