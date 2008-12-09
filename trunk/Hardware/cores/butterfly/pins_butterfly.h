/*
  pins_butterfly.h - Pin name aliases for the Butterfly
*/

#ifndef Pins_Butterfly_h
#define Pins_Butterfly_h

// *** PORTB ***
#define PBPIN1 0         // PB Pad 1 PB0(/SS)
#define PBPIN2 1         // PB Pad 2 PB1(SCK) DataFlash, ISP
#define PBPIN3 2         // PB Pad 3 PB2(MOSI) DataFlash, ISP
#define PBPIN4 3         // PB Pad 4 PB3(MISO) DataFlash, ISP
#define PBPIN5 4         // PB Pad 5 PB4(OC0, PWM A Timer0) Joystick Center
#define JOYCENTER PBPIN5
#define PBPIN6 5         // PB Pad 6 PB5(OC1A, PWM A Timer1) Piezo Element
#define SPEAKER PBPIN6
#define PBPIN7 6         // PB Pad 7 PB6(OC1B, PWM B Timer1) Joystick A
#define JOYA PBPIN7 
#define PBPIN8 7         // PB Pad 8 PB7(OC2, PWM A Timer2) Joystick B
#define JOYB PBPIN8

//
// *** PORTD ***
#define PDPIN1 8    // PORTD Pad 1 PD0 (ICP1,SEG22) LCD15
#define PDPIN2 9    // PORTD Pad 2 PD1(INT0,SEG21) LCD16
#define PDPIN3 10   // PORTD Pad 3 PD2(SEG20) LCD18
#define PDPIN4 11   // PORTD Pad 4 PD3(SEG19) LCD13
#define PDPIN5 12   // PORTD Pad 5 PD4 (SEG18) LCD11
#define PDPIN6 13   // PORTD Pad 6 PD5(SEG17) LCD12
#define PDPIN7 14   // PORTD Pad 7 PD6(SEG16) LCD14
#define PDPIN8 15   // PORTD Pad 8 PD7(SEG15) LCD9
//
// *** PORTF ***
#define TEMP 0      //   No Pad - PF0 (ADC0) Temp Sensor
#define VOLT 1      //   No Pad - PF1 (ADC1) Voltage Reader
#define LIGHT 2     //   No Pad - PF2 (ADC2) Light Sensor
#define VCP 3       //   No Pad - PF3 (ADC3) VCP (?)
// JTAGEN fuse must be unprogrammed to use JTAG pins as IO
#define JTAGPIN1 16 // JTAG Pad 1 PF4 (TCK/ADC4)
#define ADC4 4
#define JTAGPIN5 17 // JTAG Pad 5 PF5 (TMS/ADC5)
#define ADC5 5
#define JTAGPIN3 18 // JTAG Pad 3 PF6 (TDO/ADC6)
#define ADC6 6
#define JTAGPIN7 19 // JTAG Pad 7 PF7 (TDI/ADC7)
#define ADC7 7

#endif
