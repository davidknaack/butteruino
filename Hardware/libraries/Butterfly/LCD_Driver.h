/*
               BUTTLCD -- Butterfly LCD Driver

               Copyright (C) Dean Camera, 2008

            dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com
				  
			Converted to Arduino library by DaveK
*/

#ifndef LCDDRIVER_H
#define LCDDRIVER_H

   // INCLUDES:
   #include <avr/io.h>
   #include <avr/pgmspace.h>
   #include <avr/interrupt.h>
   #include <stdbool.h>

   // DEFINES:
   #define LCD_LCDREGS_START          ((uint8_t*)&LCDDR0)
   #define LCD_SPACE_OR_INVALID_CHAR  0xFF
   
   #define LCD_CONTRAST_LEVEL(level)  do{ LCDCCR = (0x0F & level); }while(0)
   #define LCD_WAIT_FOR_SCROLL_DONE() do{ while (!(ScrollFlags & LCD_FLAG_SCROLL_DONE)) {} }while(0)
   
   #define LCD_SCROLLCOUNT_DEFAULT    6
   #define LCD_DELAYCOUNT_DEFAULT     20
   #define LCD_TEXTBUFFER_SIZE        20
   #define LCD_SEGBUFFER_SIZE         19
   #define LCD_DISPLAY_SIZE           6

   #define LCD_FLAG_SCROLL            (1 << 0)
   #define LCD_FLAG_SCROLL_DONE       (1 << 1)   

#include "Print.h"

class BF_LCD : public Print
{
public:
	static volatile uint8_t ScrollFlags;   

	BF_LCD( void );
	void prints_f(const char *FlashData);
	void prints(const char *Data);
	void appendc(char Data);
	void clear(void);
	void init(void);
	void showColons(const uint8_t ColonsOn);
   
	#if defined(INC_FROM_DRIVER)
	static inline void LCD_WriteChar(const uint8_t Byte, const uint8_t Digit);
	#endif
	
	virtual void write(uint8_t);
};

extern BF_LCD LCD;

#endif 