#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef enum
{
    CFGFLAG_INVERT = (1 << 0),
} cfg_flag_t;

typedef struct
{
    const char * name;  // human readable name
    uint16_t addr;      // address in pram
    uint16_t mask;      // bitmask at addr
    uint32_t def;       // default value
    uint32_t min;       // min value
    uint32_t max;       // max value
    uint16_t div;       // divider for storage
} cfg_entry_t;

extern bool cfg_Init();
extern void cfg_Reset();

extern void cfg_Add(const cfg_entry_t* cfg, int num);

extern int  cfg_Num();
extern const cfg_entry_t* cfg_Get(int idx);
extern const cfg_entry_t* cfg_Find(const char* name);

extern uint32_t cfg_GetValue(const cfg_entry_t* entry);
extern void cfg_SetValue(const cfg_entry_t* entry, uint32_t val);

#endif // _CONFIG_H_

