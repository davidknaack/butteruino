//*****************************************************************************
//
//      COPYRIGHT (c) ATMEL Norway, 1996-2001
//
//      The copyright to the document(s) herein is the property of
//      ATMEL Norway, Norway.
//
//      The document(s) may be used  and/or copied only with the written
//      permission from ATMEL Norway or in accordance with the terms and
//      conditions stipulated in the agreement/contract under which the
//      document(s) have been supplied.
//
//*****************************************************************************
//
//  File........: DATAFLASH.C
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: All AVRs with built-in HW SPI
//
//  Description.: Functions to access the Atmel AT45Dxxx dataflash onboard
//                Butterfly development board
//
//  Revisions...:
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20011017 - 1.00 - Beta release                                  -  RM
//  20011017 - 0.10 - Generated file                                -  RM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20040121          added compare and erase function              - M.Thomas
//  20081228          Converted to Arduino Library for Butterfly	- Dave K
//
//*****************************************************************************

/* 
   remark mthomas: If you plan to use the dataflash functions in own code
   for (battery powered) devices: disable the "chip select" after accessing
   the Dataflash. The current draw with cs enabled is "very" high. You can
   simply use the macro DF_CS_inactive already defined by Atmel after every
   DF access.

   The coin-cell battery on the Butterfly is not a reliable power-source if data
   in the flash-array should be changed (write/erase).
   See the Dataflash datasheet for the current needed during write-accesses.
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "dataflash.h"

#define PageBits 9;
#define PageSize 264;

#define DF_CS_inactive sbi(PORTB,0)
#define	DF_CS_active cbi(PORTB,0);
#define DF_reset DF_CS_inactive; DF_CS_inactive;

extern byte device_id;
extern BF_DataFlash dataFlash;

/*****************************************************************************
*
*	Function name : BF_DataFlash()
*
*	Returns :		Instance
*
*	Parameters :	None
*
*	Purpose :		Constructor
*
******************************************************************************/
BF_DataFlash::BF_DataFlash(void)
{
	DF_SPI_init();
}


/*****************************************************************************
*
*	Function name : Activate
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Sets chip select to activate dataflash chip
*
******************************************************************************/
void BF_DataFlash::Activate(void);
{
	DF_CS_active;							//to reset dataflash command decoder
}



/*****************************************************************************
*
*	Function name : Deactivate
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Clears chip select to deactivate dataflash chip.
*					This is useful to save power.
*
******************************************************************************/
void BF_DataFlash::Deactivate(void);
{
	DF_CS_inactive;							//make sure to toggle CS signal in order
}



/*****************************************************************************
*
*	Function name : DF_SPI_init
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Sets up the HW SPI in Master mode, Mode 3
*					Note -> Uses the SS line to control the DF CS-line.
*
******************************************************************************/
void BF_DataFlash::DF_SPI_init (void)
{
	PORTB |= (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);
	DDRB |= (1<<DDB2) | (1<<DDB1) | (1<<DDB0);				//Set MOSI, SCK AND SS as outputs
	SPSR = (1<<SPI2X);                                      //SPI double speed settings
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL);	//Enable SPI in Master mode, mode 3, Fosc/4
}



/*****************************************************************************
*
*	Function name : DF_SPI_RW
*
*	Returns :		Byte read from SPI data register (any value)
*
*	Parameters :	Byte to be written to SPI data register (any value)
*
*	Purpose :		Read and writes one byte from/to SPI master
*
******************************************************************************/
byte BF_DataFlash::DF_SPI_RW (byte output)
{
	byte input;
	
	SPDR = output;							//put byte 'output' in SPI data register
	while(!(SPSR & _BV(SPIF)));				//wait for transfer complete, poll SPIF-flag
	input = SPDR;							//read value in SPI data reg.
	
	return input;							//return the byte clocked in from SPI slave
}



/*****************************************************************************
*
*	Function name : Read_DF_status
*
*	Returns :		One status byte. Consult Dataflash datasheet for further
*					decoding info
*
*	Parameters :	None
*
*	Purpose :		Status info concerning the Dataflash is busy or not.
*					Status info concerning compare between buffer and flash page
*					Status info concerning size of actual device
*
*   mt: the 'if' marked with 'mt 200401' is a possible optimisation
*   if only one type of Dataflash is used (like on the Butterfly).
*   If the uC controls different types of dataflash keep the PageBits
*   and PageSize decoding in this function to avoid problems.
******************************************************************************/
byte BF_DataFlash::Read_DF_status (void)
{
	byte result;
	byte index_copy;
	
	DF_CS_reset;							//reset dataflash command decoder
	
	result = DF_SPI_RW(StatusReg);			//send status register read op-code
	result = DF_SPI_RW(0x00);				//dummy write to get result
	
	device_id = ((result & 0x3C) >> 2);		//get the device id bits, butterfly dataflash should be 0111
	
	return result;							//return the read status register value
}



/*****************************************************************************
*
*	Function name : Page_To_Buffer
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of page to be transferred to buffer
*
*	Purpose :		Transfers a page from flash to dataflash SRAM buffer
*					
******************************************************************************/
void BF_DataFlash::Page_To_Buffer (unsigned int PageAdr, byte BufferNo)
{
	DF_CS_reset;												//reset dataflash command decoder

	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (BufferNo == 1)											//transfer flash page to buffer 1
		DF_SPI_RW(FlashToBuf1Transfer);							//transfer to buffer 1 op-code
	else	
		DF_SPI_RW(FlashToBuf2Transfer);							//transfer to buffer 2 op-code

	DF_SPI_RW((byte)(PageAdr >> (16 - PageBits)));				//upper part of page address
	DF_SPI_RW((byte)(PageAdr << (PageBits - 8)));				//lower part of page address
	DF_SPI_RW(0x00);											//don't cares
	
	DF_CS_reset;												//init transfer
	
	while(!(Read_DF_status() & 0x80));							//monitor the status register, wait until busy-flag is high
}



/*****************************************************************************
*
*	Function name : Buffer_Read_Byte
*
*	Returns :		One read byte (any value)
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*
*	Purpose :		Reads one byte from one of the dataflash
*					internal SRAM buffers
*
******************************************************************************/
byte BF_DataFlash::Buffer_Read_Byte (byte BufferNo, unsigned int IntPageAdr)
{
	byte data = 0;
	
	DF_CS_reset;							//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)							//read byte from buffer 1
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
	else
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code

	DF_SPI_RW(0x00);							//don't cares
	DF_SPI_RW((byte)(IntPageAdr>>8));			//upper part of internal buffer address
	DF_SPI_RW((byte)(IntPageAdr));				//lower part of internal buffer address
	DF_SPI_RW(0x00);							//don't cares
	data = DF_SPI_RW(0x00);						//read byte
	
	return data;								//return the read data byte
}



/*****************************************************************************
*
*	Function name : Buffer_Read_Str
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be read
*					*BufferPtr	->	address of buffer to be used for read bytes
*
*	Purpose :		Reads one or more bytes from one of the dataflash
*					internal SRAM buffers, and puts read bytes into
*					buffer pointed to by *BufferPtr
*
******************************************************************************/
void BF_DataFlash::Buffer_Read_Str (byte BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, byte *BufferPtr)
{
	unsigned int i;

	DF_CS_reset;								//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)							//read byte(s) from buffer 1
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
	else
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code

	DF_SPI_RW(0x00);							//don't cares
	DF_SPI_RW((byte)(IntPageAdr>>8));			//upper part of internal buffer address
	DF_SPI_RW((byte)(IntPageAdr));				//lower part of internal buffer address
	DF_SPI_RW(0x00);							//don't cares
	for( i=0; i<No_of_bytes; i++)
	{
		*(BufferPtr) = DF_SPI_RW(0x00);			//read byte and put it in AVR buffer pointed to by *BufferPtr
		BufferPtr++;							//point to next element in AVR buffer
	}
}



/*****************************************************************************
*
*	Function name : Buffer_Write_Enable
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to start writing from
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*					
*	Purpose :		Enables continous write functionality to one of the dataflash buffers
*					buffers. NOTE : User must ensure that CS goes high to terminate
*					this mode before accessing other dataflash functionalities 
*
******************************************************************************/
void BF_DataFlash::Buffer_Write_Enable (byte BufferNo, unsigned int IntPageAdr)
{
	DF_CS_reset;								//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)							//write enable to buffer 1
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		
	DF_SPI_RW(0x00);							//Don't care
	DF_SPI_RW((byte)(IntPageAdr>>8));			//Upper part of internal buffer address
	DF_SPI_RW((byte)(IntPageAdr));				//Lower part of internal buffer address
}



/*****************************************************************************
*
*	Function name : Buffer_Write_Byte
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to write byte to
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*					Data		->	Data byte to be written
*
*	Purpose :		Writes one byte to one of the dataflash
*					internal SRAM buffers
*
******************************************************************************/
void BF_DataFlash::Buffer_Write_Byte (byte BufferNo, unsigned int IntPageAdr, byte Data)
{
	
	DF_CS_reset;								//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)							//write byte to buffer 1
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code

	DF_SPI_RW(0x00);							//don't cares
	DF_SPI_RW((byte)(IntPageAdr>>8));			//upper part of internal buffer address
	DF_SPI_RW((byte)(IntPageAdr));				//lower part of internal buffer address
	DF_SPI_RW(Data);							//write data byte
}



/*****************************************************************************
*
*	Function name : Buffer_Write_Str
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be written
*					*BufferPtr	->	address of buffer to be used for copy of bytes
*									from AVR buffer to dataflash buffer 1 (or 2)
*
*	Purpose :		Copies one or more bytes to one of the dataflash
*					internal SRAM buffers from AVR SRAM buffer
*					pointed to by *BufferPtr
*
******************************************************************************/
void BF_DataFlash::Buffer_Write_Str (byte BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, byte *BufferPtr)
{
	unsigned int i;

	DF_CS_reset;								//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)							//write byte(s) to buffer 1
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code

	DF_SPI_RW(0x00);							//don't cares
	DF_SPI_RW((byte)(IntPageAdr>>8));			//upper part of internal buffer address
	DF_SPI_RW((byte)(IntPageAdr));				//lower part of internal buffer address
	for( i=0; i<No_of_bytes; i++)
	{
		DF_SPI_RW(*(BufferPtr));				//write byte pointed at by *BufferPtr to dataflash buffer location
		BufferPtr++;							//point to next element in AVR buffer
	}
}



/*****************************************************************************
*
*	Function name : Buffer_To_Page
*
*	Returns :		None
*
*	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of flash page to be programmed
*
*	Purpose :		Transfers a page from dataflash SRAM buffer to flash
*					
******************************************************************************/
void BF_DataFlash::Buffer_To_Page (byte BufferNo, unsigned int PageAdr)
{
	DF_CS_reset;												//reset dataflash command decoder
		
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)											//program flash page from buffer 1
		DF_SPI_RW(Buf1ToFlashWE);								//buffer 1 to flash with erase op-code
	else	
		DF_SPI_RW(Buf2ToFlashWE);								//buffer 2 to flash with erase op-code

	DF_SPI_RW((byte)(PageAdr >> (16 - PageBits)));				//upper part of page address
	DF_SPI_RW((byte)(PageAdr << (PageBits - 8)));				//lower part of page address
	DF_SPI_RW(0x00);											//don't cares
	
	DF_CS_reset;												//initiate flash page programming
	
	while(!(Read_DF_status() & 0x80));							//monitor the status register, wait until busy-flag is high
}



/*****************************************************************************
*
*	Function name : Cont_Flash_Read_Enable
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
*					IntPageAdr	->	Internal page address where cont.read starts from
*
*	Purpose :		Initiates a continuous read from a location in the DataFlash
*
******************************************************************************/
void BF_DataFlash::Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr)
{
	DF_CS_reset;							//reset dataflash command decoder
	
	DF_SPI_RW(ContArrayRead);													//Continuous Array Read op-code
	DF_SPI_RW((byte)(PageAdr >> (16 - PageBits)));								//upper part of page address
	DF_SPI_RW((byte)((PageAdr << (PageBits - 8))+ (IntPageAdr>>8)));			//lower part of page address and MSB of int.page adr.
	DF_SPI_RW((byte)(IntPageAdr));												//LSB byte of internal page address
	DF_SPI_RW(0x00);															//perform 4 dummy writes
	DF_SPI_RW(0x00);															//in order to intiate DataFlash
	DF_SPI_RW(0x00);															//address pointers
	DF_SPI_RW(0x00);
}



/*****************************************************************************
*
*	Function name : Page_Buffer_Compare
*
*	Returns :		0 match, 1 if mismatch
*
*	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of flash page to be compared with buffer
*
*	Purpose :		comparte Buffer with Flash-Page
*
*   added by Martin Thomas, Kaiserslautern, Germany. This routine was not 
*   included by ATMEL
*					
******************************************************************************/
byte BF_DataFlash::Page_Buffer_Compare(byte BufferNo, unsigned int PageAdr)
{
	byte stat;
	
	DF_CS_reset;												//reset dataflash command decoder
	
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)									
		DF_SPI_RW(FlashToBuf1Compare);	
	else
		DF_SPI_RW(FlashToBuf2Compare);						
	
	DF_SPI_RW((byte)(PageAdr >> (16 - PageBits)));				//upper part of page address
	DF_SPI_RW((byte)(PageAdr << (PageBits - 8)));				//lower part of page address
	DF_SPI_RW(0x00);											//don't cares
	
	DF_CS_reset;												//reset dataflash command decoder
	
	do {
		stat=Read_DF_status();
	} while(!(stat & 0x80));									//monitor the status register, wait until busy-flag is high
	
	return (stat & 0x40);
}



/*****************************************************************************
*
*	Function name : Page_Erase
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page to be erased
*
*	Purpose :		Sets all bits in the given page (all bytes are 0xff)
*
*	function added by mthomas. 
*
******************************************************************************/
void BF_DataFlash::Page_Erase (unsigned int PageAdr)
{
	DF_CS_reset;												//reset dataflash command decoder

	DF_SPI_RW(PageErase);										//Page erase op-code
	DF_SPI_RW((byte)(PageAdr >> (16 - PageBits)));				//upper part of page address
	DF_SPI_RW((byte)(PageAdr << (PageBits - 8)));				//lower part of page address and MSB of int.page adr.
	DF_SPI_RW(0x00);											//dont cares

	DF_CS_reset;												//initiate flash page erase

	while(!(Read_DF_status() & 0x80));							//monitor the status register, wait until busy-flag is high
}
// *****************************[ End Of DATAFLASH.C ]*************************