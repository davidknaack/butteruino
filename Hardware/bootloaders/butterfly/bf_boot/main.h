// mt: removed all definitions but mega169
//  added mega16 since mega169 is not supported by AVRPROG V1.37

#ifdef _ATMEGA169
  #define sig_byte3 0x1E
  #define sig_byte2 0x94
  #define sig_byte1 0x05
  
  #define devtype 0x79       // Mega 169 device code
  
  #define PAGESIZE 128       // Size in Bytes
  
  #ifdef _B128
    #define APP_PAGES ((2*8192 / PAGESIZE)- (2*128 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*8192 / PAGESIZE)- (2*256 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*8192 / PAGESIZE)- (2*512 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*8192 / PAGESIZE)- (2*1024 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif  
#endif

// ATmega16 added by mt
#ifdef _ATMEGA16
  #define sig_byte3 0x1E
  #define sig_byte2 0x94
  #define sig_byte1 0x03
  
  #define devtype 0x75       // Mega16 device code
  
  #define PAGESIZE 128       // Size in Bytes
  
  #ifdef _B128
    #define APP_PAGES ((2*8192 / PAGESIZE)- (2*128 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*8192 / PAGESIZE)- (2*256 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*8192 / PAGESIZE)- (2*512 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*8192 / PAGESIZE)- (2*1024 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif  
#endif

#define UART_RX_BUFFER_SIZE PAGESIZE

#define TRUE    1
#define FALSE   0
