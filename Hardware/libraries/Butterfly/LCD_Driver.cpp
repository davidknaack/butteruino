/*
			BUTTLCD -- Butterfly LCD Driver

			Copyright (C) Dean Camera, 2008

			dean [at] fourwalledcubicle [dot] com
				www.fourwalledcubicle.com
				
			Converted to Arduino library by DaveK
*/

/*
This is a basic driver for the Butterfly LCD. It offers the ability to
change the contrast and display strings (scrolling or static) from flash
or SRAM memory only.

This has been completly rewritten from the Atmel code; in this version, as
much processing as possible is performed by the string display routines
rather than the interrupt so that the interrupt executes as fast as possible.
*/

#define INC_FROM_DRIVER
#include "LCD_Driver.h"
#include "LCD_Driver_SegTable.h"

//LCD instance set up for user
BF_LCD LCD = BF_LCD();

//                                  LCD Text            + Nulls for scrolling + Null Termination
static volatile char     TextBuffer[LCD_TEXTBUFFER_SIZE + LCD_DISPLAY_SIZE    + 1] = {};
static volatile uint8_t  StrStart        		= 0;
static volatile uint8_t  StrEnd          		= 0;
static volatile uint8_t  ScrollCount     		= 0;
static volatile uint8_t  UpdateDisplay   		= false;
static volatile uint8_t  ShowColons      		= false;
static volatile uint8_t  ClearNext				= false;

volatile uint8_t  BF_LCD::ScrollFlags;

// ======================================================================================

/*
NAME:      | BF_LCD
PURPOSE:   | Constructs BF_LCD
ARGUMENTS: | None
RETURNS:   | None
*/
BF_LCD::BF_LCD( void )
{
	init();
	ScrollFlags = 0;	
	clear();
}

/*
NAME:      | init
PURPOSE:   | Initializes the Butterfly's LCD for correct operation, ready to display data
ARGUMENTS: | None
RETURNS:   | None
*/
void BF_LCD::init(void)
{
	// Set the initial contrast level to maximum:
	LCD_CONTRAST_LEVEL(0x0F);

	// Select asynchronous clock source, enable all COM pins and enable all segment pins:
	LCDCRB  = (1<<LCDCS) | (3<<LCDMUX0) | (7<<LCDPM0);

	// Set LCD prescaler to give a framerate of 64Hz:
	LCDFRR  = (0<<LCDPS0) | (3<<LCDCD0);   

	// Enable LCD and set low power waveform, enable start of frame interrupt:
	LCDCRA  = (1<<LCDEN) | (1<<LCDAB) | (1<<LCDIE);
}

/*
NAME:      | putS_f
PURPOSE:   | Displays a string from flash onto the Butterfly's LCD
ARGUMENTS: | Pointer to the start of the flash string
RETURNS:   | None
*/
void BF_LCD::prints_f(const char *FlashData)
{
	/* Rather than create a new buffer here (wasting RAM), the TextBuffer global
	is re-used as a temp buffer. Once the ASCII data is loaded in to TextBuffer,
	LCD_puts is called with it to post-process it into the correct format for the
	LCD interrupt.                                                                */

	strcpy_P((char*)&TextBuffer[0], FlashData);
	prints((char*)&TextBuffer[0]);
}

/*
NAME:      | prints
PURPOSE:   | Displays a string from SRAM onto the Butterfly's LCD
ARGUMENTS: | Pointer to the start of the SRAM string
RETURNS:   | None
*/
void BF_LCD::prints(const char *Data)
{
	ClearNext 			= false;  // putS always clears the current output
	uint8_t LoadB       = 0;
	uint8_t CurrByte;
	
	do
	{
		CurrByte = *(Data++);
		
		switch (CurrByte)
		{
		case 'a'...'z':
			CurrByte &= ~(1 << 5);                   // Translate to upper-case character
		case '*'...'_':                                // Valid character, load it into the array
			TextBuffer[LoadB++] = (CurrByte - '*');
			break;
		case 0x00:                                   // Null termination of the string - ignore for now so the nulls can be appended below
			break;
		default:                                     // Space or invalid character, use 0xFF to display a blank
			TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;
		}
	}
	while (CurrByte && (LoadB < LCD_TEXTBUFFER_SIZE));

	ScrollFlags = ((LoadB > LCD_DISPLAY_SIZE)? LCD_FLAG_SCROLL : 0x00);

	for (uint8_t Nulls = 0; Nulls < 7; Nulls++)
	TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;  // Load in nulls to ensure that when scrolling, the display clears before wrapping

	TextBuffer[LoadB] = 0x00;                           // Null-terminate string

	StrStart      = 0;
	StrEnd        = LoadB;
	ScrollCount   = LCD_SCROLLCOUNT_DEFAULT + LCD_DELAYCOUNT_DEFAULT;
	UpdateDisplay = true;
}

/*
NAME:      | appendc
PURPOSE:   | Appends a character from SRAM onto the Butterfly's LCD
ARGUMENTS: | The charater to append
RETURNS:   | None
*/
void BF_LCD::appendc(char Data)
{
	// ClearNext indicates that a println was used and so new lines should first clear old lines.
	if (ClearNext)
		clear();

	uint8_t LoadB = StrEnd - LCD_DISPLAY_SIZE -1;
	// Todo: If added the character will exceed LCD_TEXTBUFFER_SIZE, first shift the contents of the buffer
	// back by one and then proceed to add this new character at the end.
	// Als, this routine is mostly just a copy of LCD_puts, review them to see if it makes sense to
	// combine them into a single routine.

	switch (Data)
	{
	case '\n':									// Writing a newline will cause a clear before the next write
		ClearNext = true;						
		break;
	case 'a'...'z':
		Data &= ~(1 << 5);                      // Translate to upper-case character
	case '*'...'_':                             // Valid character, load it into the array
		TextBuffer[LoadB++] = (Data - '*');
		break;
	case 0x00:                              	// Null termination of the string - ignore for now so the nulls can be appended below
		break;
	default:                          			// Space or invalid character, use 0xFF to display a blank
		TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;
	}

	ScrollFlags = ((LoadB > LCD_DISPLAY_SIZE)? LCD_FLAG_SCROLL : 0x00);

	for (uint8_t Nulls = 0; Nulls < 7; Nulls++)
		TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;  // Load in nulls to ensure that when scrolling, the display clears before wrapping

	TextBuffer[LoadB] = 0x00;                           // Null-terminate string

	StrStart      = 0;
	StrEnd        = LoadB;
	ScrollCount   = LCD_SCROLLCOUNT_DEFAULT + LCD_DELAYCOUNT_DEFAULT;
	UpdateDisplay = true;
}


/*
NAME:      | clear
PURPOSE:   | Clear the LCD
ARGUMENTS: | None
RETURNS:   | None
*/
void BF_LCD::clear(void)
{
	for (uint8_t Nulls = 0; Nulls < (LCD_DISPLAY_SIZE + 1); Nulls++)
		TextBuffer[Nulls] = LCD_SPACE_OR_INVALID_CHAR;  // Load in nulls to ensure that when scrolling, the display clears before wrapping
	TextBuffer[LCD_DISPLAY_SIZE + 1] = 0x00;                           // Null-terminate string

	ClearNext 	  = false;
	StrStart      = 0;
	StrEnd        = LCD_DISPLAY_SIZE + 1;
	ScrollCount   = LCD_SCROLLCOUNT_DEFAULT + LCD_DELAYCOUNT_DEFAULT;
	UpdateDisplay = true;
}


/*
NAME:      | showColons
PURPOSE:   | Routine to turn on or off the LCD's colons
ARGUMENTS: | Boolean - true to turn on colons
RETURNS:   | None
*/
void BF_LCD::showColons(const uint8_t ColonsOn)
{
	ShowColons    = ColonsOn;
	UpdateDisplay = true;
} 

/*
NAME:      | write
PURPOSE:   | Routine to print characters to the LCD
ARGUMENTS: | character to print
RETURNS:   | None
*/
void BF_LCD::write(uint8_t b) 
{
	appendc((char)b);
} 

//void BF_LCD::println(void)
//{
//  ClearNext = true;
//}

/*
NAME:      | LCD_WriteChar (static, inline)
PURPOSE:   | Routine to write a character to the correct LCD registers for display
ARGUMENTS: | Character to display, LCD character number to display character on
RETURNS:   | None
*/
static inline void LCD_WriteChar(const uint8_t Byte, const uint8_t Digit)
{
	uint8_t* BuffPtr = (uint8_t*)(LCD_LCDREGS_START + (Digit >> 1));
	uint16_t SegData = 0x0000;

	if (Byte != LCD_SPACE_OR_INVALID_CHAR)              // Null indicates invalid character or space
	SegData = pgm_read_word(&LCD_SegTable[Byte]);   

	for (uint8_t BNib = 0; BNib < 4; BNib++)
	{
		uint8_t MaskedSegData = (SegData & 0x0000F);

		if (Digit & 0x01)
		*BuffPtr = ((*BuffPtr & 0x0F) | (MaskedSegData << 4));
		else
		*BuffPtr = ((*BuffPtr & 0xF0) | MaskedSegData);

		BuffPtr += 5;
		SegData >>= 4;
	}   
}


/*
NAME:      | LCD_vect (ISR, blocking)
PURPOSE:   | ISR to handle the display and scrolling of the current display string onto the LCD
ARGUMENTS: | None
RETURNS:   | None
*/
ISR(LCD_vect)
{
	if (LCD.ScrollFlags & LCD_FLAG_SCROLL)
	{
		if (!(ScrollCount--))
		{
			UpdateDisplay = true;
			ScrollCount   = LCD_SCROLLCOUNT_DEFAULT;
		}
	}

	if (UpdateDisplay)
	{
		for (uint8_t Character = 0; Character < LCD_DISPLAY_SIZE; Character++)
		{
			uint8_t Byte = (StrStart + Character);

			if (Byte >= StrEnd)
			Byte -= StrEnd;
			
			LCD_WriteChar(TextBuffer[Byte], Character);
		}
		
		if ((StrStart + LCD_DISPLAY_SIZE) == StrEnd)    // Done scrolling message on LCD once
		LCD.ScrollFlags |= LCD_FLAG_SCROLL_DONE;
		
		if (StrStart++ == StrEnd)
		StrStart     = 1;

		if (ShowColons)
		*((uint8_t*)(LCD_LCDREGS_START + 8)) = 0x01;
		else
		*((uint8_t*)(LCD_LCDREGS_START + 8)) = 0x00;

		UpdateDisplay  = false;                         // Clear LCD management flags, LCD update is complete
	}
}