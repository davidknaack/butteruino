int main(void)
{
    LCD_Init();
   
   LCD_puts_f(PSTR("AVR BUTTERFLY GCC"));
   LCD_puts("AVR BUTTERFLY GCC");

   for (;;) {}
   
   return 0;
} 