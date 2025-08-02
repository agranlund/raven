#ifndef _ATARI_H_
#define _ATARI_H_

#define ENABLE_ACIA_MIDI_EMU    1
#define ENABLE_ACIA_KEYB_EMU    1

#define RV_ACIAEMU_BASE     0x00500000UL
#define RV_PADDR_ACIA       (RV_ACIAEMU_BASE + 0xC00)

#define ACIA_KEYB_CTRL      0x00
#define ACIA_KEYB_DATA      0x02
#define ACIA_MIDI_CTRL      0x04
#define ACIA_MIDI_DATA      0x06


#ifndef __ASM__

extern bool atari_Init();
extern void atari_Start();
extern uint32_t atari_DetectTos();

#endif // __ASM__
#endif // _ATARI_H_
