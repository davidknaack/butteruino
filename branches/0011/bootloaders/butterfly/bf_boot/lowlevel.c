//
//  Low-level bootloader routines to replace "assembly.s90"
//  from the original ATMEL code. 
// 
//  See avr-libc's boot-module for more information
//  adopted to the BF-"system" (this is based on avr-libc 1.0).
//  Thanks to Eric B. Weddington author of boot.h.
//
//  1/2004 Martin Thomas, Kaiserslautern, Germany
//

#include <avr/io.h>

#define my_eeprom_is_ready() bit_is_clear(EECR, EEWE)

// There are some #ifdefs in avr/boot.h V1.0 that do not "know"
// about the ATmega169. So the functions have been copied here 
// and small modifications have been done to avoid problems.
// Be aware that there are some definitions from boot.h used 
// here which may change in the future (those starting with 
// two underlines). Because of this a copy of avr/boot.h from
// avr-libc 1.0 is included here to avoid conflicts (hopefully)
//#include <avr/boot.h>
#include "avr_libc_1_0_boot.h"

// #define _ATMEGA169
#define _ATMEGA16
#include "main.h"

// from avr/boot.h
// added "func" parameter and spm-busy check at end
#define _boot_page_write_alternate_bf(address,func)    \
({                                               \
    boot_spm_busy_wait();                        \
    while(!my_eeprom_is_ready());                   \
    __asm__ __volatile__                         \
    (                                            \
        "movw r30, %2\n\t"                       \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        ".word 0xffff\n\t"                       \
        "nop\n\t"                                \
        : "=m" (__SPM_REG)                       \
        : "r" ((unsigned char)func),             \
          "r" ((unsigned short)address)          \
        : "r30", "r31", "r0"                     \
    );                                           \
    boot_spm_busy_wait();                        \
})

// from avr/boot.h 
// added spm-busy check at end
#define _boot_page_fill_alternate_bf(address, data)\
({                                               \
    boot_spm_busy_wait();                        \
    while(!my_eeprom_is_ready());                   \
    __asm__ __volatile__                         \
    (                                            \
        "movw  r0, %3\n\t"                       \
        "movw r30, %2\n\t"                       \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        ".word 0xffff\n\t"                       \
        "nop\n\t"                                \
        "clr  r1\n\t"                            \
        : "=m" (__SPM_REG)                       \
        : "r" ((unsigned char)__BOOT_PAGE_FILL), \
          "r" ((unsigned short)address),         \
          "r" ((unsigned short)data)             \
        : "r0", "r30", "r31"                     \
    );                                           \
    boot_spm_busy_wait();                        \
})

// from avr/boot.h
// added spm-busy check at end, removed lockbit-masking, 
// removed r30/r31 init
#define _boot_lock_bits_set_alternate_bf(lock_bits)        \
({                                                         \
    boot_spm_busy_wait();                                  \
    while(!my_eeprom_is_ready());                             \
    __asm__ __volatile__                                   \
    (                                                      \
        "mov r0, %2\n\t"                                   \
        "sts %0, %1\n\t"                                   \
        "spm\n\t"                                          \
        ".word 0xffff\n\t"                                 \
        "nop\n\t"                                          \
        : "=m" (__SPM_REG)                                 \
        : "r" ((unsigned char)__BOOT_LOCK_BITS_SET),       \
          "r" ((unsigned char) lock_bits)                  \
        : "r0"                                             \
    );                                                     \
    boot_spm_busy_wait();                                  \
})

void write_page (unsigned int adr, unsigned char function)
{
	_boot_page_write_alternate_bf(adr,function);
}

void fill_temp_buffer (unsigned int data,unsigned int adr)
{
	_boot_page_fill_alternate_bf(adr, data);
}

unsigned int read_program_memory(unsigned int adr ,unsigned char param)
// to read lockbits give param=0x09, if param!=0 it is written to SPMCSR
// this is a "translation" from the original code to gcc-inline-assembler
{
	unsigned int retval;
	
	boot_spm_busy_wait();
	asm volatile
	(
		"movw r30, %3\n\t"
		"sbrc %2,0x00\n\t"
		"sts %0, %2\n\t"
		#ifdef LARGE_MEMORY  // If large memory (>64K) ELPM is needed to use RAMPZ
		"elpm\n\t"           // read LSB
		#else
		"lpm\n\t"            // R0 is now the LSB of the return value
		#endif
		"mov %A1,r0\n\t"
		"inc r30\n\t"
		#ifdef LARGE_MEMORY  // If large memory (>64K) ELPM is needed to use RAMPZ        
		"elpm\n\t"           // read MSB
		#else
		"lpm\n\t"            // R0 is now the MSB of the return value
		#endif
		"mov %B1,r0\n\t"  
		: "=m" (__SPM_REG),
		  "=r" (retval)
		: "r" ((unsigned char)param),
		  "r" ((unsigned short)adr)
		: "r30", "r31", "r0"
	);
	return retval;
}

void write_lock_bits (unsigned char val)
{
	_boot_lock_bits_set_alternate_bf(val);
}
