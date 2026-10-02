#include "sys.h"
#include "hw/i2c.h"
#include "hw/rtc.h"

#define I2C_24AA02E48      0x52

static uint8_t mac_addr[6];

void mac_Read(uint8_t addr, uint8_t* buf, uint8_t siz)
{
	i2c_Aquire();
	i2c_Start();
	i2c_Write((I2C_24AA02E48 << 1) | 0x00);		// enter write mode
	i2c_Write(addr);                      		// write address
	i2c_Start();
	i2c_Write((I2C_24AA02E48 << 1) | 0x01);  	// enter read mode
	for (int i=0; i<siz-1; i++) {
		*buf++ = i2c_Read(0);                   // read data, send ack
	}
	*buf = i2c_Read(1);                         // read data, send !ack
	i2c_Stop();
	i2c_Release();
}

void mac_Write(uint8_t addr, uint8_t* buf, uint8_t siz)
{
	i2c_Aquire();
	i2c_Start();
	i2c_Write((I2C_24AA02E48 << 1) | 0x00); 	// enter write mode
	i2c_Write(addr);                     		// write address
	for (int i=0; i<siz; i++) {
		i2c_Write(buf[i]);               		// write value
	}
	i2c_Stop();
	i2c_Release();
}

bool mac_Addr(uint8_t (*macaddr)[6]) {
	if (macaddr) {
		(*macaddr)[0] = mac_addr[0];
		(*macaddr)[1] = mac_addr[1];
		(*macaddr)[2] = mac_addr[2];
		(*macaddr)[3] = mac_addr[3];
		(*macaddr)[4] = mac_addr[4];
		(*macaddr)[5] = mac_addr[5];
	}
	if ((mac_addr[0] == 0xff) &&
		(mac_addr[1] == 0xff) &&
		(mac_addr[2] == 0xff) &&		
		(mac_addr[3] == 0xff) &&
		(mac_addr[4] == 0xff) &&
		(mac_addr[5] == 0xff)) {
			return false;
		}
	return true;
}

bool mac_Init()
{
	mac_addr[0] = mac_addr[1] = mac_addr[2] = 0xff;
	mac_addr[3] = mac_addr[4] = mac_addr[5] = 0xff;
	if (krev >= 0xA2) {
		mac_Read(0xfa, mac_addr, 6);
	}
	return mac_Addr(0);
}
