#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef struct
{
    union {
        struct {
            uint8_t idx;
            uint8_t mask;
            uint8_t shift;
            uint8_t max;
        };
        uint32_t spec;
    };
    const char*     name;
    const char**    enames;
} cfg_entry_t;

extern bool cfg_Init();
extern void cfg_Add(const cfg_entry_t* cfg, int num);

extern int  cfg_Num();
extern const cfg_entry_t* cfg_Get(int idx);
extern const cfg_entry_t* cfg_Find(const char* name);

extern int  cfg_GetValue(const cfg_entry_t* entry);
extern void cfg_SetValue(const cfg_entry_t* entry, int val);

#endif // _CONFIG_H_

