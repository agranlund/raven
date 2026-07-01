#include "hw/ym.h"
#include "hw/cpu.h"

#define YMRD *((volatile uint8_t*)(RV_PADDR_YM + 0))
#define YMWR *((volatile uint8_t*)(RV_PADDR_YM + 2))
#define YMREG YMRD

#define YM_MASK_MCP4251_CS		(1 << 0)
#define YM_MASK_MCP4251_CLK		(1 << 1)
#define YM_MASK_MCP4251_DTA		(1 << 2)
#define YM_MASK_PAM8302_SD		(1 << 6)

bool ym_Init(void)
{
	// portA is output
	YMREG = 14;
	YMWR = 0xfe;
	YMREG = 7;
	YMWR = YMRD | 0x40;

	// initial volume and speaker setting
	ym_Volume(200);
	ym_Speaker(true);
	return true;
}

static const uint16_t volume_table[32] = {
      0,   2,   2,   3,   3,   4,   5,   5,
      6,   7,   9,  10,  12,  14,  17,  19,
     23,  27,  31,  37,  43,  51,  60,  71,
     83,  97, 114, 134, 158, 185, 218, 256,
};

void ym_Volume(uint8_t vol)
{
	if (krev < 0xA2) {
		return;
	}

	uint32_t ipl = cpu_SetIPL(7);

	YMREG = 14;
	uint8_t v = YMRD & ~(YM_MASK_MCP4251_DTA | YM_MASK_MCP4251_CLK | YM_MASK_MCP4251_CS);
	YMWR = v | YM_MASK_MCP4251_DTA | YM_MASK_MCP4251_CLK | YM_MASK_MCP4251_CS;

	// left
	uint16_t cmd = volume_table[vol >> 3];
	YMWR = v | YM_MASK_MCP4251_DTA | YM_MASK_MCP4251_CLK;
	for (uint16_t bit = (1 << 15); bit != 0; bit >>= 1) {
		v = (cmd & bit) ? (v | YM_MASK_MCP4251_DTA) : (v & ~YM_MASK_MCP4251_DTA);
		YMWR = v;
		YMWR = v | YM_MASK_MCP4251_CLK;
	}
	YMWR = v | YM_MASK_MCP4251_DTA | YM_MASK_MCP4251_CLK | YM_MASK_MCP4251_CS;

	// right
	cmd |= 0x1000;
	YMWR = v | YM_MASK_MCP4251_DTA | YM_MASK_MCP4251_CLK;
	for (uint16_t bit = (1 << 15); bit != 0; bit >>= 1) {
		v = (cmd & bit) ? (v | YM_MASK_MCP4251_DTA) : (v & ~YM_MASK_MCP4251_DTA);
		YMWR = v;
		YMWR = v | YM_MASK_MCP4251_CLK;
	}
	YMWR = v | YM_MASK_MCP4251_DTA | YM_MASK_MCP4251_CLK | YM_MASK_MCP4251_CS;

	cpu_SetIPL(ipl);
}

void ym_Speaker(bool enable)
{
	uint32_t ipl = cpu_SetIPL(7);
	YMREG = 14;
	YMWR = enable ? (YMRD | YM_MASK_PAM8302_SD) : (YMRD & ~YM_MASK_PAM8302_SD);
	cpu_SetIPL(ipl);
}
