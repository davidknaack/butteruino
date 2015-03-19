The Butteruino project is the core code and libraries to integrate the AVR Butterfly with the Arduino environment. It is currently working with Arduino 0012 and allows Arduino sketches to be very easily uploaded to the Butterfly.

It may also work with more recent releases, but I have not tested it. If you try it and have problems (or solutions!) please log an issue so the project can be updated.

[![](http://www.atmel.com/dyn/resources/prod_images/Butterfly.jpg)](http://www.atmel.com/dyn/products/tools_card.asp?tool_id=3146)

The [AVR Butterfly](http://www.atmel.com/dyn/products/tools_card.asp?tool_id=3146) is an Atmel ATMega169PV based demonstration platform which includes several useful peripherals such as an LCD, temperature sensor and speaker. Header pins can be added to the board to provide access to some the ATMega's port pins. It includes a bootloader that allows the user to download new programs to the board using a standard serial port.

Butteruino is a adaptation of the Arduino core to work well when running on the AVR Butterfly along with several libraries that provide easy access to the Butterfly's built-in features. While the physical format of the AVR Butterfly is not compatible with Arduino standards (it will not attach to shields), the Arduino software libraries make developing for the AVR Butterfly much easier for new users and the libraries make access to the LCD as simple as an LCD.print() statement.
