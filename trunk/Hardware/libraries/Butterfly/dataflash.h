//*****************************************************************************
//
//*****************************************************************************

#ifndef dataflash_h
#define dataflash_h

#include <stdint.h> 

class BF_DataFlash
{
private:
	void DF_SPI_init (void);
	uint8_t DF_SPI_RW (uint8_t output);

public:
	BF_DataFlash(void);
	
	uint8_t ReadDFStatus (void);
	void Activate(void);
	void Deactivate(void);
	
	void EnterDeepPowerDown(void);
	void ExitDeepPowerDown(void);

	void BufferToPage (uint8_t BufferNo, uint16_t PageAdr);
	void PageToBuffer (uint16_t PageAdr, uint8_t BufferNo);

	void ContFlashReadEnable (uint16_t PageAdr, uint16_t IntPageAdr);
	void BufferReadEnable (uint8_t BufferNo, uint16_t IntPageAdr);
	uint8_t BufferReadByte (uint8_t BufferNo, uint16_t IntPageAdr);
	void BufferReadStr (uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_uint8_ts, uint8_t *BufferPtr);
	uint8_t ReadNextByte(void);
	
	void BufferWriteEnable (uint8_t BufferNo, uint16_t IntPageAdr);
	void BufferWriteByte (uint8_t BufferNo, uint16_t IntPageAdr, uint8_t Data);
	void BufferWriteStr (uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_uint8_ts, uint8_t *BufferPtr);
	void WriteNextByte (uint8_t data);

	uint8_t PageBufferCompare(uint8_t BufferNo, uint16_t PageAdr);
	void PageErase (uint16_t PageAdr);
};

extern BF_DataFlash DataFlash;

#endif
