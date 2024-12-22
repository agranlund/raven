/* Demo fÅr LineA-Routinen */

#include <tos.h>
#include "linea.h"

BITBLT bitblt;
MFORM maus;
PATTERN fill;
SDB sprite1, sprite2, sprite3, sprite4;
SSB buffer1, buffer2, buffer3, buffer4;
int points[] = { 320,0, 430,150, 640,200, 430,250,
                 320,400, 210,250, 0,200, 210,150, };

extern FONTS *Fonts;

int main(void)
{
    int x, y, i;
    char s[81];
    linea_init();
    set_fg_bp(1); /* schwarz */
    set_ln_mask(0xFFFF); /* durchgezogene Linie */
    set_clip(0, 0, 639, 399, 0); /* Clipping aus */
    hide_mouse(); /* Maus ausschalten */
    set_pattern(fill, 1, 0); /* FÅllmuster */
    fill[0] = 0x5555;
    fill[1] = 0xAAAA;
    set_wrt_mode(REPLACE);
    filled_rect(0, 0, 640, 400);
    for(i = 0; i < 256; i += 2)
    {
        fill[0] = i;
        fill[1] = i*256;
        filled_rect(320-i, 200-i/2, 320+i, 200+i/2);
    }
    fill[0] = 0x3333;
    fill[1] = 0xCCCC;
    filled_polygon(points, 8);
    for(x = 0, y = 0; y < 401; x += 2, y += 2)
    {
        draw_line(x,y,640-x,400-y);
        draw_line(x,400-y,640-x,y);
        draw_line(640-x,y,x,400-y);
        draw_line(640-x,400-y,x,y);
    }
    set_wrt_mode(XOR);
    set_ln_mask(0x3333);
    for(y = 10; y < 391; y += 5)
        horizontal_line(10,y,630);
    for(x = 0; x < 641; x++)
        put_pixel(x,200,1);
    for(y = 0; y < 401; y++)
        put_pixel(320,y,1);
    for(i = 0; i <200; i += 4)
        draw_circle(320, 200, i, 1);
    /*** SPRITE DEMO ***/
    sprite1.xhot = sprite1.yhot = 8;
    sprite1.form = VDIFM;
    sprite1.bgcol = 1; sprite1.fgcol = 2;
    for(i = 0; i < 32; i++)
        sprite1.image[i] = 0xF0F0;
    sprite2.xhot = sprite2.yhot = 8;
    sprite2.form = XORFM;
    sprite2.bgcol = 1; sprite2.fgcol = 2;
    for(i = 0; i < 32; i++)
        sprite2.image[i] = 0xF0F0;
    sprite3.xhot = sprite3.yhot = 8;
    sprite3.form = VDIFM;
    sprite3.bgcol = 1; sprite3.fgcol = 2;
    for(i = 0; i < 32; i++)
        sprite3.image[i] = 0x8888;
    sprite4.xhot = sprite4.yhot = 8;
    sprite4.form = XORFM;
    sprite4.bgcol = 1; sprite4.fgcol = 2;
    for(i = 0; i < 32; i++)
        sprite4.image[i] = 0x8888;
    for(x = 10, y = 10; y < 391; x++, y++)
    {
        draw_sprite(x, y, &sprite1, &buffer1);
        draw_sprite(540-x, 400-y, &sprite2, &buffer2);
        draw_sprite(100+x, 400-y, &sprite3, &buffer3);
        draw_sprite(640-x, y, &sprite4, &buffer4);
        for(i = 0; i < 1000; i++)
            ;
        undraw_sprite(&buffer4);
        undraw_sprite(&buffer3);
        undraw_sprite(&buffer2);
        undraw_sprite(&buffer1);
    }
    fill[0] = 0xFFFF;
    fill[1] = 0xFFFF;
    set_wrt_mode(XOR);
    for(i = 0; i < 50; i++)
        filled_rect(0, 0, 640, 400);
    /*** BITBLT Demo ***/
    bitblt.s_xmin = 120;
    bitblt.s_ymin = 0;
    bitblt.d_xmin = 120;
    bitblt.d_ymin = 0;
    bitblt.op_tab[0] = bitblt.op_tab[1] = 0x8;
    bitblt.op_tab[2] = bitblt.op_tab[3] = 0x8;
    bitblt.s_form = bitblt.d_form = Physbase();
    bitblt.s_nxwd = bitblt.d_nxwd = 2;
    bitblt.s_nxln = bitblt.d_nxln = 80;
    bitblt.s_nxpl = bitblt.d_nxpl = 2;
    bitblt.p_addr = 0L;
    bitblt.plane_ct = 1;
    bitblt.fg_col = 1; bitblt.bg_col = 1;
    for(i = 1; i < 400; i++)
    {
        bitblt.b_wd = i;
        bitblt.b_ht = i;
        bit_blt(&bitblt);
    }
    fill[0] = 0x0;
    fill[1] = 0x0;
    set_wrt_mode(REPLACE);
    filled_rect(0, 0, 640, 400);
    set_wrt_mode(TRANS);
    /*** TEXTBLT DEMO ***/
    for(i = 1; i < 51; i++)
        s[i-1] = (char) i;
    s[50] = '\0';
    for(i = 0; i < 19; i++)
    {
        set_text_blt(Fonts->font[i%3], 0, (i*2)%33, 0, 1, 0);
        print_string(10, 10+19*i, i%3, s);
    }
    set_text_blt(Fonts->font[2], 0, 0, 0, 1, 0);
    print_string(10, 371, -2, "Weiter mit Taste ...");
    /*** Maus DEMO ***/
    maus.mf_xhot = maus.mf_yhot = 8;
    maus.mf_fg = 0; maus.mf_bg = 1;
    maus.mf_nplanes = 1;
    for(i = 0; i < 16; i++)
    {
        maus.mf_mask[i] = 0xFFFF-i-i*256;
        maus.mf_data[i] = i+i*256;
    }
    transform_mouse(&maus);
    show_mouse(0);
    Cnecin();
    return(0);
}
