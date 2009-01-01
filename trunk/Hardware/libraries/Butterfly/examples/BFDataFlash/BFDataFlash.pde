#include <dataflash.h>
#include <LCD_Driver.h>

bool test()
{
    bool result = true;

    // Set up to write data to SRAM buffer 1, starting at byte 0
    DataFlash.BufferWriteEnable(1,0);
    // Write a series of bytes into the buffer
    for(byte j=2; j<52; j++)
        DataFlash.WriteNextByte(j);

    // Write SRAM buffer 1 to flash page 1
    DataFlash.BufferToPage (1,1);            
    
    // Set up to read directly from flash page 1, starting at byte 0
    DataFlash.ContFlashReadEnable (1,0);

    // Read a series of bytes and check the value
    for ( byte j=2; j<52; j++ ) {
        if ( j != DataFlash.ReadNextByte()) {
            result = false;
            break;
        }
    }

    // Deactivate the flash chip when not in use.
    DataFlash.Deactivate();    
    return result;
}


void setup()
{
  LCD.prints("flash test");
  delay(2000);

  LCD.clear();
  if (test())
    LCD.prints("Success");  
  else
    LCD.prints("Failure");
}

void loop()
{
}
