/*****************************************************************************
*
* Copyright (C) 1996-1998 Atmel Corporation
*
* File          : main.c
* Created       : 16-jun-99
* Last Modified : 3-dec-1999  (mt: ??)
* Author(s)     : BBrandal, PKastnes, ARodland, LHM
*
* Description   : This Program allows an AVR with bootloader capabilities to 
*                 Read/write its own Flash/EEprom. 
* 
*
****************************************************************************
*
* 1/2004 port to avr-gcc/avr-libc by Martin Thomas, Kaiserslautern, Germany
*
*  Many functions used by "AVRPROG" (fuses) have been disabled by ATMEL in 
*  the original source code not by me. Please 'diff' against the original 
*  source to see everything that has been changed for the gcc port.
*  As in the BF-application source most of my changes are marked with "mt" 
*  or enclosed with "//mtA" "//mtE".
*
*  The boot interrupt vector is included (this bootloader is completly in
*  ".text" section). If you need this space for further functions you have to
*  add a separate section for the bootloader functions and add an attribute
*  for this section to _all_ function prototypes of functions in the loader. 
*  With this the interrupt vector will be placed at .0000 and the bootloader 
*  code (without interrupt vector) at the adress you define in the linker
*  options for the newly created section. Take care since the pin-change 
*  interrupt is used in the Butterflys bootloader for power-saving reasons.
*  See the avr-libc FAQ and the avr-libc boot.h documentation for further 
*  details.
*
*  5/2005 - added receive-buffer in function BufferLoad()
*         - made register-definitions compatible with avr-libc V1.2.3
*
****************************************************************************/

// mt redundant: #define ENABLE_BIT_DEFINITIONS

//mtA
// #include <inavr.h>
// #include "iom169.h"
// #include "assembly.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <lowlevel.h>

// Enable functions originaly disabled
// #define ENABLEREADFUSELOCK // keep this disabled until AVRPROG supports ATmega169
//mtE

// ATmega169 is not suppored by AVRPROG V1.37, so "mimic" an ATmega16 instead
// #define _ATMEGA169 
#define _ATMEGA16

//* Select Boot Size (select one, comment out the others) */
// #define _B128
//#define _B256  
//#define _B512 
#define _B1024 

#include "main.h"

unsigned char gBuffer[UART_RX_BUFFER_SIZE];

void sendchar(char ) ;
char recchar(void) ;

// cbA void USART_Init(unsigned int );
unsigned char BufferLoad(unsigned int , unsigned char ) ;
void BlockRead(unsigned int , unsigned char ) ;
void OSCCAL_calibration(void) ;
void Delay(unsigned int) ;

unsigned int address;
unsigned char device;

// mt__C_task void main(void)
int main(void)
{
    void (*funcptr)( void ) = 0x0000;       // Set up function pointer 

    unsigned int tempi;
    char val;
    char OK = 1;    

    OSCCAL_calibration();   // Calibrate the OSCCAL byte

    ACSR = (1<<ACD);

    // Disable Digital input on PF0-2 (power save)
    DIDR1 = (7<<ADC0D);

    PORTB = 0xFF;       // Enable pullups on Ports B and E
    PORTE = 0xFF;
    
    // boost IntRC to 2Mhz to achieve 19200 baudrate
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 4, Inter RC 8Mhz / 4 = 2Mhz
    CLKPR = (1<<CLKPS1);    

    UBRRH = 0;//(unsigned char)(baudrate>>8);
    UBRRL = 12;//(unsigned char)baudrate;

    // Enable 2x speed
    UCSRA = (1<<U2X);

    // Enable receiver and transmitter
    UCSRB = (1<<RXEN)|(1<<TXEN)|(0<<RXCIE)|(0<<UDRIE);

    // Async. mode, 8N1
    UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);
        
    MCUCR = (1<<IVCE);       
    MCUCR = (1<<IVSEL);             //move interruptvectors to the Boot sector    

    sei(); // mt __enable_interrupt();
    
    // mt PCMSK1 = (1<<PORTB6) | (1<<PORTB4);
	PCMSK1 = (1<<PINB6) | (1<<PINB4);       // set pin-change interrupt mask
  	EIFR   = (1<<PCIF1);                    // clear external intterupt flag 1
	EIMSK  = (1<<PCIE1);                    // enable external interrupt 1

	for(;OK;)
	{
		if(!(PINB & (1<<PINB6)))	// mt if(!(PINB & (1<<PORTB6)))
        {   // joystick "up" detected
            MCUCR = (1<<IVCE); 
            MCUCR = (0<<IVSEL);             //move interruptvectors to the Application sector
            funcptr();      // Jump to application sector   
        }
        //else if(PINB & (1<<PORTB4))                     // If joystick not in the ENTER-position
		else if(PINB & (1<<PINB4))                     // If joystick not in the ENTER-position
        {
            UCSRA = 0x02;
            SMCR = (3<<SM0) | (1<<SE);      // Enable Power-save mode
			// mt __sleep();  // Go to sleep
            asm volatile ("sleep"::);		 // Go to sleep
			
            SMCR = 0;                       // Just woke, disable sleep
        }
        else
        {	
            val = recchar();
   
            if( val == 0x1B)
         	{							// AVRPROG connection
          		while (val != 'S')				// Wait for signon 
          		{
          		  val = recchar();
          		}
                sendchar('A');					// Report signon
                sendchar('V');
                sendchar('R');
                sendchar('B');
                sendchar('O');
                sendchar('O');
                sendchar('T');
                
                cli(); 	// mt __disable_interrupt();
                
                OK = 0;
            }
    		else
	    		sendchar('?');
        }
	}			

      for(;;)                               // If PINDX is pulled low: programmingmode. (mt: here PINB4)
      {
        val=recchar();
         
        if(val=='a')                        //Autoincrement?
        {
          sendchar('Y');		    		//Autoincrement is quicker
        }
 
        else if(val=='A')                   //write address 
        {
          address=recchar();                //read address 8 MSB
          address=(address<<8)|recchar();
        
          address=address<<1;               //convert from word address to byte address
          sendchar('\r');
        }
        
        else if(val=='b')
		{									// Buffer load support
			sendchar('Y');					// Report buffer load supported
			sendchar((UART_RX_BUFFER_SIZE >> 8) & 0xFF);
											// Report buffer size in bytes
			sendchar(UART_RX_BUFFER_SIZE & 0xFF);
		}

		else if(val=='B')					// Start buffer load
		{
			tempi = recchar() << 8;			// Load high byte of buffersize
			tempi |= recchar();				// Load low byte of buffersize
			val = recchar();				// Load memory type ('E' or 'F')
			sendchar (BufferLoad(tempi,val));
											// Start downloading of buffer
		}
		
		else if(val == 'g')					// Block read
		{
			tempi = (recchar() << 8) | recchar();

			val = recchar();				// Get memtype
			BlockRead(tempi,val);			// Perform the block read
		}		
/*
        else if(val=='c')                   //Write program memory, low byte
        {       
          ldata=recchar();
          sendchar('\r');
        }

        else if(val== 'C')                  //Write program memory, high byte 
        {
          data=ldata|(recchar()<<8);
		  if (device == devtype)
          {
            fill_temp_buffer(data,(address)); //call asm routine. 
          }
          address=address+2;  
          sendchar('\r');
        }
*/        
        else if(val=='e')                   //Chip erase 
        {
		  if (device == devtype)
          {
            for(address=0;address < APP_END;address += PAGESIZE)    //Application section = 60 pages
            {
                write_page(address,(1<<PGERS) + (1<<SPMEN));    //Perform page erase
                write_page(address,(1<<RWWSRE) + (1<<SPMEN));   //Re-enable the RWW section
            }
          }
          write_page(address,(1<<RWWSRE) + (1<<SPMEN));   //Re-enable the RWW section
          sendchar('\r');        
        }

        else if(val=='E')                   //Exit upgrade
        {
		  WDTCR = (1<<WDCE) | (1<<WDE);     //Enable Watchdog Timer to give reset
		  sendchar('\r');
        }
/*
        else if(val=='l')                   // write lockbits 
        {
		  if (device == devtype)
          {
            write_lock_bits(recchar());
  		  }
          sendchar('\r');
        }
       
        else if(val== 'm')                  // write page
        {
		  if (device == devtype)
          {
            write_page(address,(1<<PGERS) + (1<<SPMEN));    //Perform page erase
            write_page((address),0x05);       
            write_page(address,(1<<RWWSRE) + (1<<SPMEN));   //Re-enable the RWW section
           }
          sendchar('\r');
        }
*/        
        
        else if(val=='P')     // Enter programming mode 
        {
          sendchar('\r');
        }
        
        else if(val=='L')   // Leave programming mode
        { 
          sendchar('\r');
        }
        
        else if (val=='p')		// mt: return programmer type
        {
          sendchar('S');		// serial programmer
        }        
/*        
        else if(val=='R')                   //Read program memory 
        {        
          write_page(0,(1<<RWWSRE) + (1<<SPMEN));   //Re-enable the RWW section
//          SPMCSR = (1<<RWWSRE) | (1<<SPMEN);
//          __store_program_memory();
//          while((SPMCSR & 0x01));
          
          intval=read_program_memory(address,0x00);
          sendchar((char)(intval>>8));      //send MSB  
          sendchar((char)intval);           //send LSB
          address=address+2;  
        }

        else if (val == 'D')		// write EEPROM
        {
		  if (device == devtype)
          {
            EEARL = address;
            EEARH = (address >> 8);
            address++;
            EEDR = recchar();
            EECR |= (1<<EEMWE);
            EECR |= (1<<EEWE);
            while (EECR & (1<<EEWE))
              ;
          }
          sendchar('\r');
        }

        else if (val == 'd')		// read eeprom
        {
          EEARL = address;
          EEARH = (address >> 8);
          address++;
          EECR |= (1<<EERE);
          sendchar(EEDR);
        }       
*/
#ifdef ENABLEREADFUSELOCK
#warning "Extension 'ReadFuseLock' enabled"
// mt TODO: Read fuse bit seems to work for clock speed, other settings are not
// interpreted correctly. Locks and high fuse do not work at all (in AVRPROG 1.37)
// Reason for this should be the difference between ATmega16 and ATmega169.
// AVRPROG interprets the results as from an ATmega16 while they are from an ATmega169
        else if(val=='F')                   // read fuse bits
        {
          sendchar(read_program_memory(0x0000,0x09)); // 0x09 for (1<<BLBSET)|(1<<SPMEN)
        }

        else if(val=='r')                   // read lock bits
        { 
          sendchar(read_program_memory(0x0001,0x09));
        }        

        else if(val=='N')                   // read high fuse bits
        {
          // mt sendchar(read_program_memory(0x0003));
		  sendchar(read_program_memory(0x0003,0x09));
        }        

        else if(val=='Q')                   // read extended fuse bits
        {
          sendchar(read_program_memory(0x0002,0x09));
        }
#endif	
// end of ENABLEREADFUSELOCK section

        else if(val=='t')                   // Return programmer type 
        {
          sendchar(devtype);
          sendchar(0);
        }

        else if ((val=='x')||(val=='y'))	// mt: clear and set LED ignored
        {
          recchar();
          sendchar('\r');
        }
       
       else if (val=='T')	// mt:  set device/programmer type in bootloader (?)
       {
         device = recchar();
          sendchar('\r');
       }
        
        else if (val=='S')                  // Return software identifier 
        {
          sendchar('A');
          sendchar('V');
          sendchar('R');
          sendchar('B');
          sendchar('O');
          sendchar('O');
          sendchar('T');
        }                
        
        else if (val=='V')                  // Return Software Version
        {
          sendchar('0');	// mt: changed from 2;0 to 0;1 
          sendchar('2');
        }        

        else if (val=='s')                  // Return Signature Byte
        {
          sendchar(sig_byte1);
          sendchar(sig_byte2);
          sendchar(sig_byte3);
        }       

        else if(val!=0x1b)                  // if not esc
        {
          sendchar('?');
        }
      }
	  return 0;
}

/*
void USART_Init(unsigned int baudrate)
{
    // Set baud rate
    UBRR0H = (unsigned char)(baudrate>>8);
    UBRR0L = (unsigned char)baudrate;

    // Enable 2x speed
    UCSR0A = (1<<U2X0);

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(0<<RXCIE0)|(0<<UDRIE0);

    // Async. mode, 8N1
    UCSR0C = (0<<UMSEL0)|(0<<UPM00)|(0<<USBS0)|(3<<UCSZ00)|(0<<UCPOL0);
}
*/

void sendchar(char data)
{
    int i = 0;
    
    UDR = data;
    
    if(SREG & 0x80)
    {
		while ( !(UCSRA&0x40) && (i<10000) )
		{
			i++;
		}
    }
    else 
        while( !(UCSRA&0x40) );
        
    UCSRA=UCSRA|0x40;                             //delete TXCflag
}


char recchar(void)
{
    int i = 0;
    
    if(SREG & 0x80)
    {
		while (!(UCSRA & (1<<RXC)) && (i<10000))
		{
			i++;
		}
    }
    else
        while(!(UCSRA & (1<<RXC)));
        
    return UDR;
}


// mtA
// #pragma vector = PCINT1_vect
// __interrupt void PCINT1_interrupt(void)
// mt TODO: find out if the handler is needed
// mtE
SIGNAL(SIG_PIN_CHANGE1)
{
//    __no_operation();
}

unsigned char BufferLoad(unsigned int size, unsigned char mem)
{
	int data, tempaddress, cnt;
	
	// store values to be programmed in temporary buffer
	for (cnt=0; cnt<UART_RX_BUFFER_SIZE; cnt++) {
		if (cnt<size) gBuffer[cnt]=recchar();
		else gBuffer[cnt]=0xFF;
	}
	cnt=0;
	
	tempaddress = address;					// Store address in page
	
	if (device == devtype)
	{
		if (mem == 'F')
		{

			do {
				data = gBuffer[cnt++];
				data |= (gBuffer[cnt++]<<8);
				fill_temp_buffer(data,(address));
											//call asm routine. 
				address=address+2;  		// Select next word in memory
				size -= 2;					// Reduce number of bytes to write by two
	    
			} while(size);					// Loop until all bytes written

			tempaddress &= 0xFF80;			// Ensure the address points to the first byte in the page
			write_page((tempaddress),0x05);	// Program page contents

			write_page(tempaddress,(1<<RWWSRE) + (1<<SPMEN));
											//Re-enable the RWW section
			if (address != (address & 0xFF80))
			{								// Ensure that the address points to the beginning of the next page
				address &= 0xFF80;
				address += PAGESIZE;
			}
		}									// End FLASH
		
		if (mem == 'E')						// Start EEPROM
        {
			address>>=1;
			do {
	 	        EEARL = address;			// Setup EEPROM address
	            EEARH = (address >> 8);
	            address++;					// Select next byte
	            EEDR = gBuffer[cnt++];			// Load data to write
	            EECR |= (1<<EEMWE);			// Write data into EEPROM
	            EECR |= (1<<EEWE);
	            while (EECR & (1<<EEWE))	// Wait for EEPROM write to finish
	              ;
				size--;						// Decreas number of bytes to write
			} while(size);					// Loop until all bytes written
		}
      	return '\r';						// Report programming OK
	}
	return 0;								// Report programming failed
}

void BlockRead(unsigned int size, unsigned char mem)
{
	unsigned int data;
	
	if (mem == 'E')							// Read EEPROM
	{
		do {
			EEARL = address;				// Setup EEPROM address
			EEARH = (address >> 8);
			address++;						// Select next EEPROM byte
			EECR |= (1<<EERE);				// Read EEPROM
			sendchar(EEDR);					// Transmit EEPROM data to PC
			size--;							// Decrease number of bytes to read
		} while (size);					// Repeat until all block has been read
	}
	else									// Read Flash
	{
		do {
			data = read_program_memory(address,0x00);
			sendchar((char)data);			//send LSB
			sendchar((char)(data >> 8));	//send MSB  
			address += 2;  					// Select next word in memory
			size -= 2;						// Subtract two bytes from number of bytes to read
		} while (size);					// Repeat until all block has been read
	}
}





/*****************************************************************************
*
*   Function name : OSCCAL_calibration
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calibrate the internal OSCCAL byte, using the external 
*                   32,768 kHz crystal as reference
*
*****************************************************************************/
void OSCCAL_calibration(void)
{
    unsigned char calibrate = FALSE;
    int temp;
    unsigned char tempL;

    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    Delay(1000);    // wait for external crystal to stabilise
    
    while(!calibrate)
    {
        cli(); // mt __disable_interrupt();  // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
	while ( !(TIFR2 & (1<<OCF2A)) ); // while ( !(TIFR2 && (1<<OCF2A)) );   // wait for timer2 compareflag
    
        TCCR1B = 0; // stop timer1

        sei(); // __enable_interrupt();  // enable global interrupt
    
        if ( (TIFR1 & (1<<TOV1)) ) // if ( (TIFR1 && (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
            calibrate = TRUE;   // the interRC is correct
    
        TCCR1B = (1<<CS10); // start timer1
    }
}


/*****************************************************************************
*
*   Function name : Delay
*
*   Returns :       None
*
*   Parameters :    unsigned int millisec
*
*   Purpose :       Delay-loop
*
*****************************************************************************/
void Delay(unsigned int millisec)
{
    unsigned char i; // mt int i;
    
    while (millisec--)
        for (i=0; i<125; i++);
}
