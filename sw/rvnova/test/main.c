/*-------------------------------------------------------------------------------
 * NOVA driver test
 * (c)2025 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  whthout even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#include "rvtest.h"

/*-----------------------------------------------------------------------------*/
typedef struct {
    const char* name;
    void(*func)(void);
} option_t;

static option_t options[] = {
    { "fillrate",   test_speed },
    { "sprites",    test_sprites },
    { "scroll",     test_scroll },
    { "rasterdev",  test_raster },
    { 0, 0 }
};


/*-----------------------------------------------------------------------------*/
static uint8_t backup_data[2048];

nova_xcb_t* nova;
card_t* card;

uint32_t hz200(void) {
    return *((uint32_t*)0x4baUL);
}

uint32_t vram_addr(vec_t* v) {
    uint32_t bytes_per_pixel = ((nova->planes + 1) & ~7) >> 3;
    return (uint32_t)nova->base + ((uint32_t)v->y * nova->pitch) + ((uint32_t)v->x * bytes_per_pixel);
}

void waitkey_release(void) {
    while(Cconis() == -1) { Cconin(); }
}
int16_t waitkey_press(void) {
    while(Cconis() != -1);
    return (int16_t)(Cconin() & 0xff);
}

int16_t pollkey_press(void) {
    return (Cconis() == -1) ? (int16_t)(Cconin() & 0xff) : -1;
}


/*-----------------------------------------------------------------------------*/
static long err(const char* str) {
    printf("\nERROR: "); printf(str);
    printf("Press any key to exit.\n");
    while(Cconis() != -1);
    Cconin();        
    return -1;
}

static void clearscreen(void) {
    Cconws("\n\033E\n");
}


/*-----------------------------------------------------------------------------*/
static long supermain(void)
{
    int i, num_options;
    while(Cconis() == -1) { Cconin(); }
    if (Getcookie(C_NOVA, &((long)nova)) != C_FOUND) {
        return err("NOVA not found\n");
    }

    /* temp */
    card = (card_t*) *((uint32_t*)&nova->unknown8A[0]);

    Cursconf(0, 0);

    while (1) {
        uint32_t oldbase;
        waitkey_release();
        clearscreen();
        printf("ptr:  %08lx\n", (uint32_t)card);
        printf("card: %s\n", card->name);
        printf("vram: %ldKb\n", card->vram_size / 1024);
        printf("bank: %ldKb x %ld\n", card->bank_size / 1024, card->bank_count);
        printf("addr: %08lx\n", card->bank_addr);
        printf("\nSelect:\n");
        for(num_options=0; (num_options<9) && (options[num_options].func != 0); num_options++) {
            printf(" %d. %s\n", num_options + 1, options[num_options].name);
        }
        printf(" x. exit\n");
        i = waitkey_press() - '1';
        waitkey_release();
        if ((i < 0) || (i >= num_options)) {
            break;
        }

        /* backup */
        oldbase = (uint32_t)nova->base;
        memcpy(backup_data, (void*)(oldbase+ card->bank_size - 2048), 2048);
        /* run test */
        clearscreen();
        options[i].func();
        /* restore */
        nova->p_setscreen((void*)oldbase);
        memcpy((void*)(oldbase + card->bank_size - 2048), backup_data, 2048);
    }
    clearscreen();
    waitkey_release();
    return 0;
}

int main() {
    return (int)Supexec(supermain);
}

