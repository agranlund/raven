#ifndef __PS2_H__
#define __PS2_H__

#define PS2_HWPORT          P3
#define PS2_HWDIR           P3_DIR
#define PS2M_HWBIT_CLK      2
#define PS2K_HWBIT_CLK      3
#define PS2M_HWBIT_DTA      6
#define PS2K_HWBIT_DTA      7

#define PS2M_HWMASK_CLK     (1<<PS2M_HWBIT_CLK)
#define PS2K_HWMASK_CLK     (1<<PS2K_HWBIT_CLK)
#define PS2M_HWMASK_DTA     (1<<PS2M_HWBIT_DTA)
#define PS2K_HWMASK_DTA     (1<<PS2K_HWBIT_DTA)

#define PS2_BUFFER_SIZE     64
#define PS2_BUFFER_MASK     (PS2_BUFFER_SIZE-1)
#define PS2_ERROR_TIME_MS   100
#define PS2_SHIFTREG_INIT   (1<<11)


#define PS2K_FLAG_EXT0      (1<<0)
#define PS2K_FLAG_BREAK     (1<<1)
#define PS2K_FLAG_EXT1      (1<<2)

#define PS2_KEY_LED_SCROLLLOCK  0x01
#define PS2_KEY_LED_NUMLOCK     0x02
#define PS2_KEY_LED_CAPSLOCK    0x04

typedef struct {
    union {
        struct {
            uint8_t key;
            uint8_t ext;
        };
        uint16_t data;
    };
} ps2key_t;

typedef struct {
    union {
        struct {
            int16_t x;
            int16_t y;
            int8_t  z;
            uint8_t b;
        };
        uint32_t data;
    };
} ps2mouse_t;

#if !defined(DISABLE_PS2)

extern void InitPS2(void);
extern void ProcessPS2(void);
extern void UpdatePs2KeyboardLed(uint8_t led);

extern void PS2Interrupt0(void) __interrupt(INT_NO_INT0);
extern void PS2Interrupt1(void) __interrupt(INT_NO_INT1);

#endif /* !DISABLE_PS2 */

#endif /* __PS2_H__ */
