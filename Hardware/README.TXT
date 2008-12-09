
Things you need to do to make it work:

* copy the files & folders into your arduino Hardware directory.
* tap the button after uploading starts
* dont hold the button down the entire time.

Things to do in the future:
Butterfly library for 
	- temperature
	- flash
	- light sensor
	- speaker
	- LCD
	- Joystick
	- clock speeds
	
- check analog pins (jtag port)
- check digital pinouts on port b and D
- check all the timing
	
 
OTHER NOTES: 

These are things I've done in the files to make it kind of work. They may or may not be necessary.
I tried using conditional compilation but I wasn't being picked up so I just copied the core and
made changes to that.


* added following to boards.txt to support as a target compilation board
	bfly.name=Butterfly

	bfly.upload.protocol=butterfly
	bfly.upload.maximum_size=14336
	bfly.upload.speed=19200

	bfly.bootloader.low_fuses=0xE2
	bfly.bootloader.high_fuses=0x98
	bfly.bootloader.extended_fuses=0xFF
	bfly.bootloader.path=butterfly
	bfly.bootloader.file=bf_boot.hex
	bfly.bootloader.unlock_bits=0x3F
	bfly.bootloader.lock_bits=0x0F

	bfly.build.mcu=atmega169
        bfly.build.f_cpu=8000000L
        bfly.build.core=butterfly

* added Bf_boot to bootloaders dir
	- copied hex from bf_boot up to bootloaders/bf_boot and renamed bf_boot.hex

* removed second external interupt from hardware/cores/arduino/WInterrupts.c: -> changes not picked up??

* changed boards.txt core = butterfly
	- copied arduino core folder and renamed to be buttefly
	- changes to hardware/cores/butterfly/WInterrupts.c
	- changes to hardware/cores/butterfly/wiring.c  TCCR0 - > TCCR0A  
	- changes to hardware/cores/butterfly/wiring_analog.c: In function 'analogWrite':
	- changes to hardware/cores/butterfly/wiring_digital.c: In function 'turnOffPWM':

* added avrispv2 to programmers.txt
* added butterfly to external programmers
* added clock divider code to init() in wiring.c


Changes by DaveK 
http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1222630663/29#29
[[
I've added osccal.c/h which are used by wiring.c. This is the OSCCAL_Calibrate() function I mentioned earlier. I've modified it by removing the CLKPR reset that it used.

In wiring.c I've added CLKPR_Calibrate() which checks F_CPU then sets CLKPR to the scaling that gets it the closest, based on the 8MHz RC oscillator.

In wiring.c Init() I call CLKPR_Calibrate() to set the clock prescaler, then OSCCAL_Calibrate() to try to use the 32kHz crystal to try to get the clock within a few percent of the target rate.

I tested on my Butterfly at 1,000,000, 4,000,000, 7,372,800, and 8,000,000. I tried at 500,000 too, but that didn't work. Everything else worked well enough that serial comm works. At 1mhz I was using 9600, and at 7.3mhz 115200 worked.

The delays that delayMicroseconds() produces are wrong when running with F_CPU < 8MHz, but delay() usually comes in pretty close. For very slow clocks I think the timer0 configuration for millis is going to have to be changed.

In wiring.c Init() I updated the setting of the A2D prescale in ADCSRA. I try to find the first prescale value under 200kHz and use that.

In wiring_serial.c I updated beginSerial() to set U2X when F_CPU is <= 1MHz, and to use the appropriate calculation for UBRR.

I think that's it. The result is that you can set your clock frequency in boards.txt (1,2,4 and 8Mhz should all work, as well as fairly wide deviations from those), and serial communication should behave like it is supposed to, and the calibration routine should get the clock within 2 or 3% of the requested frequency. The calibration routine does take just a little longer to start up, but it's under a second.

It could probably be better, but at least it seems to be working here.
]]
-------------------------------------------------------------------------
No Longer needed:

* change preferences.txt to upload.using=butterfly

	This preferences file is found in this folder:

	    * /Users/<USERNAME>/Library/Arduino/preferences.txt (Mac)
	    * c:\Documents and Settings\<USERNAME>\Application Data\Arduino\preferences.txt (Windows)
	    * ~/.arduino/preferences.txt (Linux) 
-------------------------------------------------------------------------
www.brokentoaster.com/arduino012_butterfly.zip