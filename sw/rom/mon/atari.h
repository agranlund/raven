#ifndef _ATARI_H_
#define _ATARI_H_

#define ENABLE_ACIA_MIDI_EMU    1
#define ENABLE_ACIA_KEYB_EMU    1

#define PADDR_ACIA      (BIOS_EMU_MEM + 0xC00)
#define ACIA_KEYB_CTRL  0x00
#define ACIA_KEYB_DATA  0x02
#define ACIA_MIDI_CTRL  0x04
#define ACIA_MIDI_DATA  0x06

extern bool atari_Init();
extern void atari_Start();

#endif // _ATARI_H_
