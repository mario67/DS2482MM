// DS2482-800 library by Mario Marini aka marinaio67
// Use with DallasTempI2C library (my mod of DallasTemperature 3.7.2 lib)
// marinimario@live.it

#include "Arduino.h"    
#ifndef __DS2482MM_H__
#define __DS2482MM_H__

#include <inttypes.h>

// Chose between a table based CRC (flash expensive, fast)
// or a computed CRC (smaller, slow)
#define ONEWIRE_CRC8_TABLE 			1

#define DS2482_COMMAND_RESET		0xF0	// Device reset

#define DS2482_COMMAND_SRP			0xE1 	// Set read pointer
	#define DS2482_POINTER_STATUS		0xF0
		#define DS2482_STATUS_BUSY			(1<<0)
		#define DS2482_STATUS_PPD			(1<<1)
		#define DS2482_STATUS_SD			(1<<2)
		#define DS2482_STATUS_LL			(1<<3)
		#define DS2482_STATUS_RST 			(1<<4)
		#define DS2482_STATUS_SBR			(1<<5)
		#define DS2482_STATUS_TSB 			(1<<6)
		#define DS2482_STATUS_DIR 			(1<<7)
	#define DS2482_POINTER_DATA			0xE1
	#define DS2482_POINTER_CONFIG		0xC3
		#define DS2482_CONFIG_APU			(1<<0)
		#define DS2482_CONFIG_SPU			(1<<2)
		#define DS2482_CONFIG_1WS			(1<<3)


#define DS2482_COMMAND_WRITECONFIG	0xD2
#define DS2482_COMMAND_RESETWIRE	0xB4
#define DS2482_COMMAND_WRITEBYTE	0xA5
#define DS2482_COMMAND_READBYTE		0x96
#define DS2482_COMMAND_SINGLEBIT	0x87
#define DS2482_COMMAND_TRIPLET		0x78

#define WIRE_COMMAND_SKIP			0xCC
#define WIRE_COMMAND_SELECT			0x55
#define WIRE_COMMAND_SEARCH			0xF0

#define DS2482_ERROR_TIMEOUT		(1<<0)
#define DS2482_ERROR_SHORT			(1<<1)
#define DS2482_ERROR_CONFIG			(1<<2)

// *************************************** aggiunta per DS2482-800 *****************************************
#define DS2482_CMD_CHSL		0xC3	//< DS2482 Channel Select
// DS2482 channel selection code for defines
#define DS2482_CH_IO0		0xF0	//< DS2482 Select Channel IO0
#define DS2482_CH_IO1		0xE1	//< DS2482 Select Channel IO1
#define DS2482_CH_IO2		0xD2	//< DS2482 Select Channel IO2
#define DS2482_CH_IO3		0xC3	//< DS2482 Select Channel IO3
#define DS2482_CH_IO4		0xB4	//< DS2482 Select Channel IO4
#define DS2482_CH_IO5		0xA5	//< DS2482 Select Channel IO5
#define DS2482_CH_IO6		0x96	//< DS2482 Select Channel IO6
#define DS2482_CH_IO7		0x87	//< DS2482 Select Channel IO7

// DS2482 channel selection read back code for defines
#define DS2482_RCH_IO0		0xB8	//< DS2482 Select Channel IO0
#define DS2482_RCH_IO1		0xB1	//< DS2482 Select Channel IO1
#define DS2482_RCH_IO2		0xAA	//< DS2482 Select Channel IO2
#define DS2482_RCH_IO3		0xA3	//< DS2482 Select Channel IO3
#define DS2482_RCH_IO4		0x9C	//< DS2482 Select Channel IO4
#define DS2482_RCH_IO5		0x95	//< DS2482 Select Channel IO5
#define DS2482_RCH_IO6		0x8E	//< DS2482 Select Channel IO6
#define DS2482_RCH_IO7		0x87	//< DS2482 Select Channel IO7
// ************************************* fine aggiunta per DS2482-800 *****************************************

class DS2482MM
{
public:
	DS2482MM();
	DS2482MM(uint8_t address);

	int channel_select(int channel);  // **************** aggiunta per DS2482-800 *********************

	uint8_t getAddress();
	uint8_t getError();
	uint8_t checkPresence();

	void deviceReset();
	void setReadPointer(uint8_t readPointer);
	uint8_t readStatus();
	uint8_t readData();
	uint8_t waitOnBusy();
	uint8_t readConfig();
	void writeConfig(uint8_t config);
	void setStrongPullup();
	void clearStrongPullup();
	uint8_t wireReset();
	void wireWriteByte(uint8_t data, uint8_t power = 0);
	uint8_t wireReadByte();
	void wireWriteBit(uint8_t data, uint8_t power = 0);
	uint8_t wireReadBit();
	void wireSkip();
	void wireSelect(const uint8_t rom[8]);
	void wireResetSearch();
	uint8_t wireSearch(uint8_t *address);

	void reset_search();
	uint8_t search(uint8_t *newAddr);
	static uint8_t crc8(const uint8_t *addr, uint8_t len);
	uint8_t reset(void);
	void select(const uint8_t rom[8]);
	void skip(void);
	void write(uint8_t v, uint8_t power = 0);
	uint8_t read(void);
	uint8_t read_bit(void);
	void write_bit(uint8_t v);

private:
	void begin();
	uint8_t end();
	void writeByte(uint8_t);
	uint8_t readByte();

	int mAddress;
	uint8_t mError;
	
	uint8_t searchAddress[8];
	uint8_t searchLastDiscrepancy;
	uint8_t searchLastDeviceFlag;
};

#endif
