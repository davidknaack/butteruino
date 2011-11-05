/*
  pins_arduino.c - pin definitions for the Arduino board
  Part of Arduino / Wiring Lite

  Copyright (c) 2005 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: pins_arduino.c 344 2007-10-06 14:47:50Z mellis $
*/

#include <avr/io.h>
#include "wiring_private.h"
#include "pins_arduino.h"

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

//  Butterfly with ATMEL ATMEGA8 & 168
//  See pins_butterfly.h for names that can be used for the pins
//
//      Light
// Volt
// USART    LCD              Joystick
// USI
//      PORTB  JTAG  PORTD     ISP
//
// Pin mapping (thanks to Joe from SmileyMicros.com for the
//              Butterfly Alternate Pin Uses PDF ) 
// *** PORTB ***
// PORTB Pad 1 PB0 (/SS)
// PORTB Pad 2 PB1(SCK) DataFlash, ISP
// PORTB Pad 3 PB2(MOSI) DataFlash, ISP
// PORTB Pad 4 PB3(MISO) DataFlash, ISP
// PORTB Pad 5 PB4 (OC0, PWM Output A, Timer0) Joystick Center
// PORTB Pad 6 PB5(OC1A, PWM Output A for Timer1) Piezo Element
// PORTB Pad 7 PB6(OC1B, PWM Output B for Timer1) Joystick A
// PORTB Pad 8 PB7(OC2, PWM Output A for Timer2) Joystick B
//
// *** PORTD ***
// PORTD Pad 1 PD0 (ICP1,SEG22) LCD15
// PORTD Pad 2 PD1(INT0,SEG21) LCD16
// PORTD Pad 3 PD2(SEG20) LCD18
// PORTD Pad 4 PD3(SEG19) LCD13
// PORTD Pad 5 PD4 (SEG18) LCD11
// PORTD Pad 6 PD5(SEG17) LCD12
// PORTD Pad 7 PD6(SEG16) LCD14
// PORTD Pad 8 PD7(SEG15) LCD9
//
// *** PORTF ***
//   No Pad - PF0 (ADC0) Temp Sensor
//   No Pad - PF1 (ADC1) Voltage Reader
//   No Pad - PF2 (ADC2) Light Sensor
//   No Pad - PF3 (ADC3) VCP (?)
// JTAG Pad 1 PF4 (TCK/ADC4)  
// JTAG Pad 5 PF5 (TMS/ADC5)
// JTAG Pad 3 PF6 (TDO/ADC6)
// JTAG Pad 7 PF7 (TDI/ADC7)
//  * note JTAGEN fuse must be unprogrammed to use JTAG pins for IO
//
// *** PORTE ***
// I didn't map these since they aren't really useful as
// general purpose IO. Some can be used for pin change interrupts
// and other things, but for most users it would probably be
// confusing to have them in the Arduino pins mapping. Advanced
// users are free to access them as usual.
//
// Serial Pad 1 PE0 (RXD) Serial receive
// Serial Pad 2 PE1 (TXD) Serial transmit
//     No Pad - PE2 (AIN0/XCK) nc?
//     No Pad - PE3 (AIN1)     nc? 
//    USI Pad 1 PE4 (SCL,USCL)
//    USI Pad 2 PE5 (SDA/DI)
//    USI Pad 3 PE6 (D0)
//     No Pad - PE7       RST_FLASH
//
//

#define PB 2
#define PD 3
#define PF 4

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint8_t PROGMEM port_to_mode_PGM[] = {
    NOT_A_PORT,
    NOT_A_PORT,
    &DDRB,
    &DDRD,
    &DDRF,
};

const uint8_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	&PORTB,
	&PORTD,
	&PORTF,
};

const uint8_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	&PINB,
	&PIND,
	&PINF,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PB, /* 0 */
	PB,
	PB,
	PB,
	PB,
	PB,
	PB,
	PB,
	PD, /* 8 */
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
    PF, /* 16 */ 
    PF, 
    PF, 
    PF, 
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV(0), /* 0, port B */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0), /* 8, port D */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(4), /* 16, port F */ // JTAG conn 1
	_BV(5),                  // JTAG conn 5
	_BV(6),                  // JTAG conn 3
	_BV(7),                  // JTAG conn 7
	
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	NOT_ON_TIMER, /* 0 - port B */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	TIMER0A,
	TIMER1A,
	TIMER1B,
	TIMER2A,
    NOT_ON_TIMER, /* 8 - Port D */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER, /* 16 - Port F */
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
};

