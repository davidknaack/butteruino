
===================================================

  A Port of the ATMEL AVR Butterfly Bootloader
  Code to AVR-GCC/avr-libc

  by Martin Thomas, Kaiserslautern, Germany
  mthomas@rhrk.uni-kl.de 
  eversmith@heizung-thomas.de

===================================================

(Installation instruction at the end of this text)

3. May 2005 - Version 0.31
- fixed eeprom upload with buffer
- register-labels compatible with avr-libc 1.2.3

Apr 2004 - Version 0.3
No changes in functionality. Small "bug" fix in
OSCCAL_calibration (adopted from bf_gcc App., reported
for this by Steve H-C.


27. Jan 2004 - Version 0.2

No changes in functionality. The Bootloader identifies
itself as a ATmega16 since the ATmega169 is not supported
by AVRPROG (Version 1.37). 

Ignore the fuse and lock-bit readout. Read and Set is
not enabled. I've tried to enable them but AVRProg 
"sees" an ATmega16 and tries to parse the answers as
coming from such a device. Most of the lock and fuse-bit
are different in the ATmega169 so the parsing fails and
returns "nonsens".

Some cleanup. A lot of comments added (some of them 
might be only usefull to me ;) )



23. Jan 2004 - Version 0.1

During the development of a data-logger application
with the AVR-Butterfly there was a need to make
some changes in the Bootloader. The same problem
again: no IAR compiler. The same way to solve the
problem: a port of the code to avr-gcc/avr-libc.

This port is based on the ATMEL bootloader source
code Rev 0.2. Please use avr-gcc 3.3.1, avr-libc 
1.0 or WINAVR Sept. 2003 or later to compile and
link the bootloader.


The changes in main.c where minimal. The usual
stuff already known from the application code
port. But Atmel used a separate "lib" written
in "pure" assembly to access the low-level functions
for flash and eeprom read/write. Well, so far I
don't know how to use "mixed language sources" 
with the avr-gcc toolchain, so the low-level
routines have been implemented as inline assembler.
The avr-libc boot.h module written by Eric
Weddington served as a template (it's even
included in this archive to avoid conflicts with 
future versions of avr-libc). Three of the four 
low-level routines found in lowlevel.c come from 
boot.h with minimal changes. The read routine has 
been developed based on the ATMEL assembler code.

There is no new functionality in this port.
Beside of the Version Number (changed from
2.0 to 0.1) everthing should look and act like 
the orignial bootloader to AVRPROG. So far there 
have been no tests with avrdude which supports 
the Butterfly in the current (1/2004) cvs version.

With minimal changes in the start code which is
bound to the port that is connect to the Butterfly
"joystick" and some special power-save methods
this bootloader should work with most ATmega
controllers. Because of the block write methods
it might be a little faster than some other 
bootloaders.



--------------- Installation -----------------

## WARNING ##: via ISP you get full read/write 
access to fuses and lock-bits. You can lock
yourself out by setting them wrong. In the 
worst case you have to use the parallel/high
voltage programming method as described in the
Butterfly manual. Read the manual first! 
NO WARRANTY.

I've used the Atmel STK500. AVRISP should work
too with an external power supply. Software like 
PonyProg can be used as long as the used adapter 
is compatible with the Butterflys Vcc (max. 4.5V 
according to the manual).

1) Solder a pin-header on the ISP port of the
  Butterfly (see manual available at atmel.com).
2) Connect the Butterfly with the STK500, select
  VTARGET<=4.5V first.
3) Select the bootloader hex file and flash it
  (STK500/Program/Flash/Select .hex/[Program]).
4) Since the flash erases the chip and resets
  the look bits by default, re-enable the "Boot 
  Loader protection Mode 2 - SPM prohibited..." 
  to avoid that the bootloader is overwritten by
  application code that is uploaded via AVRPROG.
5) Test the bootloader by uploading an application
  with AVRPROG, the hex-file of the Atmel original
  is included in the archive for a convinient
  restore to the "state of delivery".


Summary of the "tagged" items in the STK500 plugin
of AVR-Studio for "Butterfly-default"

Fuses:
X  Brown-out detection disabled
X  JTAG Interface enabled
X  Boot Flash section size=1024 words
X  Boot Reset vector Enabled
X  Int. RC Osc.; Start-up time: 6CK+65ms

LockBits:
X  Mode 1: no memory lock
X  Application Protection Mode 1: no lock on SPM and LPM
X  Boot Loader Protection Mode 2: SPM prohibited

