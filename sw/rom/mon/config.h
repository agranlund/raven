#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef struct
{
    const char * name;
    const char * const * opts;
    uint8_t flags;
    uint8_t addr;
    uint8_t bits;
    uint8_t shift;
    uint32_t min;
    uint32_t max;
    uint32_t def;
} cfg_entry_t;

extern bool cfg_Init();
extern void cfg_Add(const cfg_entry_t* cfg, int num);

extern int  cfg_Num();
extern const cfg_entry_t* cfg_Get(int idx);
extern const cfg_entry_t* cfg_Find(const char* name);

extern uint32_t cfg_GetValue(const cfg_entry_t* entry);
extern void cfg_SetValue(const cfg_entry_t* entry, uint32_t val);

#endif // _CONFIG_H_

