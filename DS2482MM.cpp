// DS2482-800 library by Mario Marini aka marinaio67
// Use with DallasTempI2C library (my mod of DallasTemperature 3.7.2 lib)
// marinimario@live.it

#include "Arduino.h"   
#include "DS2482MM.h"
#include <Wire.h>


DS2482MM::DS2482MM()
{
	mAddress = 0x18;
	mError = 0;
	Wire.begin();
}

DS2482MM::DS2482MM(uint8_t address)
{
	mAddress = 0x18 | address;
	mError = 0;
	Wire.begin();
}

uint8_t DS2482MM::getAddress()
{
	return mAddress;
}

uint8_t DS2482MM::getError()
{
	return mError;
}

void DS2482MM::begin()
{
	Wire.beginTransmission(mAddress);
}

uint8_t DS2482MM::end()
{
	return Wire.endTransmission();
}

void DS2482MM::writeByte(uint8_t data)
{
	Wire.write(data); 
}

uint8_t DS2482MM::readByte()
{
	Wire.requestFrom(mAddress,1u);
	return Wire.read();
}

uint8_t DS2482MM::checkPresence()
{
	begin();
	return !end() ? true : false;
}

void DS2482MM::deviceReset()
{
	begin();
	write(DS2482_COMMAND_RESET);
	end();
}

void DS2482MM::setReadPointer(uint8_t readPointer)
{
	begin();
	writeByte(DS2482_COMMAND_SRP);
	writeByte(readPointer);
	end();
}

uint8_t DS2482MM::readStatus()
{
	setReadPointer(DS2482_POINTER_STATUS);
	return readByte();
}

uint8_t DS2482MM::readData()
{
	setReadPointer(DS2482_POINTER_DATA);
	return readByte();
}

uint8_t DS2482MM::readConfig()
{
	setReadPointer(DS2482_POINTER_CONFIG);
	return readByte();
}

void DS2482MM::setStrongPullup()
{
	writeConfig(readConfig() | DS2482_CONFIG_SPU);
}

void DS2482MM::clearStrongPullup()
{
	writeConfig(readConfig() & !DS2482_CONFIG_SPU);
}

uint8_t DS2482MM::waitOnBusy()
{
	uint8_t status;

	for(int i=1000; i>0; i--)
	{
		status = readStatus();
		if (!(status & DS2482_STATUS_BUSY))
			break;
		delayMicroseconds(20);
	}

		if (status & DS2482_STATUS_BUSY)
		mError = DS2482_ERROR_TIMEOUT;

		return status;
}

void DS2482MM::writeConfig(uint8_t config)
{
	waitOnBusy();
	begin();
	writeByte(DS2482_COMMAND_WRITECONFIG);
	writeByte(config | (~config)<<4);
	end();
	
	if (readByte() != config)
		mError = DS2482_ERROR_CONFIG;
}


int DS2482MM::channel_select(int channel){

	unsigned char ch, ch_read, read_channel; 

	switch (channel){
  default: 
  case 0: 
	  ch = DS2482_CH_IO0; 
	  ch_read = DS2482_RCH_IO0; 
	  break;
  case 1: 
	  ch = DS2482_CH_IO1; 
	  ch_read = DS2482_RCH_IO1; 
	  break;
  case 2: 
	  ch = DS2482_CH_IO2; 
	  ch_read = DS2482_RCH_IO2; 
	  break;
  case 3: 
	  ch = DS2482_CH_IO3; 
	  ch_read = DS2482_RCH_IO3; 
	  break;
  case 4: 
	  ch = DS2482_CH_IO4; 
	  ch_read = DS2482_RCH_IO4; 
	  break;
  case 5: 
	  ch = DS2482_CH_IO5; 
	  ch_read = DS2482_RCH_IO5; 
	  break;
  case 6: 
	  ch = DS2482_CH_IO6; 
	  ch_read = DS2482_RCH_IO6; 
	  break;
  case 7: 
	  ch = DS2482_CH_IO7; 
	  ch_read = DS2482_RCH_IO7; 
	  break;
	}

	Wire.beginTransmission(mAddress);
	Wire.write(DS2482_CMD_CHSL);
	Wire.write(ch);
	Wire.endTransmission();
	Wire.requestFrom(mAddress, 1);
	read_channel = Wire.read();

	return (read_channel == ch_read);
}   

uint8_t DS2482MM::wireReset()
{
	waitOnBusy();
	clearStrongPullup();
    waitOnBusy();
	begin();
	writeByte(DS2482_COMMAND_RESETWIRE);
	end();

	uint8_t status = waitOnBusy();

	if (status & DS2482_STATUS_SD)
	{
		mError = DS2482_ERROR_SHORT;
	}

	return (status & DS2482_STATUS_PPD) ? true : false;
}

void DS2482MM::wireWriteByte(uint8_t data, uint8_t power)
{
	waitOnBusy();
	if (power)
		setStrongPullup();
	begin();
	writeByte(DS2482_COMMAND_WRITEBYTE);
	writeByte(data);
	end();
}

uint8_t DS2482MM::wireReadByte()
{
	waitOnBusy();
	begin();
	writeByte(DS2482_COMMAND_READBYTE);
	end();
	waitOnBusy();
	return readData();
}

void DS2482MM::wireWriteBit(uint8_t data, uint8_t power)
{
	waitOnBusy();
	if (power)
		setStrongPullup();
	begin();
	writeByte(DS2482_COMMAND_SINGLEBIT);
	writeByte(data ? 0x80 : 0x00);
	end();
}

uint8_t DS2482MM::wireReadBit()
{
	wireWriteBit(1);
	uint8_t status = waitOnBusy();
	return status & DS2482_STATUS_SBR ? 1 : 0;
}

void DS2482MM::wireSkip()
{
	wireWriteByte(WIRE_COMMAND_SKIP);
}

void DS2482MM::wireSelect(const uint8_t rom[8])
{
	wireWriteByte(WIRE_COMMAND_SELECT);
	for (int i=0;i<8;i++)
		wireWriteByte(rom[i]);
}

void DS2482MM::wireResetSearch()
{
	searchLastDiscrepancy = 0;
	searchLastDeviceFlag = 0;

	for (int i = 0; i < 8; i++)
	{
		searchAddress[i] = 0;
	}

}

uint8_t DS2482MM::wireSearch(uint8_t *address)
{
	uint8_t direction;
	uint8_t last_zero=0;

	if (searchLastDeviceFlag)
		return 0;

	if (!wireReset())
		return 0;

	waitOnBusy();

	wireWriteByte(WIRE_COMMAND_SEARCH);

	for(uint8_t i=0;i<64;i++)
	{
		int searchByte = i / 8; 
		int searchBit = 1 << i % 8;

		if (i < searchLastDiscrepancy)
			direction = searchAddress[searchByte] & searchBit;
		else
			direction = i == searchLastDiscrepancy;

		waitOnBusy();
		begin();
		writeByte(DS2482_COMMAND_TRIPLET);
		writeByte(direction ? 0x80 : 0x00);
		end();

		uint8_t status = waitOnBusy();

		uint8_t id = status & DS2482_STATUS_SBR;
		uint8_t comp_id = status & DS2482_STATUS_TSB;
		direction = status & DS2482_STATUS_DIR;

		if (id && comp_id)
		{
			return 0;
		}
		else
		{
			if (!id && !comp_id && !direction)
			{
				last_zero = i;
			}
		}

		if (direction)
			searchAddress[searchByte] |= searchBit;
		else
			searchAddress[searchByte] &= ~searchBit;

	}

	searchLastDiscrepancy = last_zero;

	if (!last_zero)
		searchLastDeviceFlag = 1;

	for (uint8_t i=0; i<8; i++)
		address[i] = searchAddress[i];

	return 1;
}

#if DS2482MM_CRC8_TABLE

static const uint8_t PROGMEM dscrc_table[] = {
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};


uint8_t DS2482MM::crc8(const uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;

	while (len--) {
		crc = pgm_read_byte(dscrc_table + (crc ^ *addr++));
	}
	return crc;
}
#else

uint8_t DS2482MM::crc8(const uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;
	
	while (len--) {
		uint8_t inbyte = *addr++;
		for (uint8_t i = 8; i; i--) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}
#endif

void DS2482MM::reset_search()
{
	wireResetSearch();
}

uint8_t DS2482MM::search(uint8_t *newAddr)
{
	return wireSearch(newAddr);
}

uint8_t DS2482MM::reset(void)
{
	return wireReset();
}

void DS2482MM::select(const uint8_t rom[8])
{
	wireSelect(rom);
}

void DS2482MM::skip(void)
{
	wireSkip();
}

void DS2482MM::write(uint8_t v, uint8_t power)
{
	wireWriteByte(v, power);	
}

uint8_t DS2482MM::read(void)
{
	return wireReadByte();
}

uint8_t DS2482MM::read_bit(void)
{
	return wireReadBit();
}

void DS2482MM::write_bit(uint8_t v)
{
	wireWriteBit(v);
}








