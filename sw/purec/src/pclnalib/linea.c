#include <string.h>
#include <stdlib.h>
#include <linea.h>
#include <portaes.h>
#include <portvdi.h>
#include <tos.h>

LINEA *Linea;
VDIESC *Vdiesc;
FONTS *Fonts;
LINEA_FUNP *Linea_funp;

static void *linea0(void) 0xa000;
static void linea1(void) 0xa001;
static short linea2(void) 0xa002;
static void linea3(void) 0xa003;
static void linea4(void) 0xa004;
static void linea5(void) 0xa005;
static void linea6(void) 0xa006;
static void linea7(void) 0xa007;
static void linea8(void) 0xa008;
static void linea9(void) 0xa009;
static void lineaa(void) 0xa00a;
static void lineab(void) 0xa00b;
static void lineac(void) 0xa00c;
static void linead(void) 0xa00d;
static void lineae(void) 0xa00e;
static void lineaf(void) 0xa00f;

static void push_a2(void) 0x2F0A;
static long pop_a2(void) 0x245F;
static void *get_a1(void) 0x2049;
static void *get_a2(void) 0x204A;

static void push_a0a2a6_0(void) 0x48e7;
static void push_a0a2a6_1(void) 0x00a2;
static void pop_a0a2a6_0(void) 0x4cdf;
static void pop_a0a2a6_1(void) 0x4500;

static void move_a0a6(void) 0x2c48;
static void move_a0a2(void) 0x2448;
static void move_a1a2(void) 0x2449;

#define push_a0a2a6() push_a0a2a6_0(), push_a0a2a6_1()
#define pop_a0a2a6() pop_a0a2a6_0(), pop_a0a2a6_1()


static void push_a2toa6_0(void) 0x48e7;
static void push_a2toa6_1(void) 0x003e;
static void pop_a2toa6_0(void) 0x4cdf;
static void pop_a2toa6_1(void) 0x7c00;

#define push_a2toa6() push_a2toa6_0(), push_a2toa6_1()
#define pop_a2toa6() pop_a2toa6_0(), pop_a2toa6_1()

static FONT_HDR *__Font;


void linea_init(void)
{
	push_a2();
	Linea = linea0();
	Vdiesc = (VDIESC *)((char *)Linea - sizeof(VDIESC));
	Fonts = get_a1();
	Linea_funp = get_a2();
	pop_a2();
}


long get_pixel( int x, int y )
{
	short *ptsin;
	
	ptsin = Linea->ptsin;
	ptsin[0] = x;
	ptsin[1] = y;
	push_a2();
	linea2();
	return pop_a2(); /* !! */
}


void set_wrt_mode( int modus )
{
	Linea->wrt_mode = modus;
}


void set_clip( int x1, int y1, int x2, int y2, int modus )
{
	LINEA *linea = Linea;
	
	linea->xmn_clip = x1;
	linea->ymn_clip = y1;
	linea->xmx_clip = x2;
	linea->ymx_clip = y2;
	linea->clip = modus;
}


void set_pattern( int *pattern, int mask, int multifill )
{
	LINEA *linea = Linea;

	linea->patptr = pattern;
	linea->patmsk = mask;
	linea->multifill = multifill;
}


void set_ln_mask( int mask )
{
	Linea->ln_mask = mask;
}


void show_mouse( int flag )
{
	Linea->intin[0] = flag;
	push_a2();
	linea9();
	pop_a2();
}


void hide_mouse(void)
{
	push_a2();
	lineaa();
	pop_a2();
}


void put_pixel(int x, int y, long color)
{
	LINEA *linea = Linea;
	short *ptsin = linea->ptsin;
	short *intin;
	
	ptsin[0] = x;
	ptsin[1] = y;
	intin = linea->intin;
	if (linea->v_planes > 16)
		*((long *)intin) = color;
	else
		intin[0] = color;
	push_a2();
	linea1();
	pop_a2();
}


void copy_raster(void)
{
	push_a2();
	lineae();
	pop_a2();
}


void seed_fill(void)
{
	push_a2();
	lineaf();
	pop_a2();
}


void bit_blt(BITBLT *bitblt)
{
	short saved[3];
	
	saved[0] = bitblt->plane_ct;
	saved[1] = bitblt->fg_col;
	saved[2] = bitblt->bg_col;
	push_a0a2a6();
	move_a0a6();
	linea7();
	pop_a0a2a6();
	bitblt->plane_ct = saved[0];
	bitblt->fg_col = saved[1];
	bitblt->bg_col = saved[2];
}


void draw_line(int x1, int y1, int x2, int y2)
{
	LINEA *linea = Linea;
	
	linea->x1 = x1;
	linea->y1 = y1;
	linea->x2 = x2;
	linea->y2 = y2;
	linea->lstlin = -1;
	push_a2();
	linea3();
	pop_a2();
}


void horizontal_line( int x1, int y1, int x2 )
{
	LINEA *linea = Linea;
	
	linea->x1 = x1;
	linea->y1 = y1;
	linea->x2 = x2;
	push_a2();
	linea4();
	pop_a2();
}


void draw_sprite( int x, int y, SDB *sdb, SSB *ssb )
{
	(void) x; /* x already in d0 */
	(void) y; /* y already in d1 */
	(void) sdb; /* sdb already in a0 */
	(void) ssb; /* ssb moved below */
	push_a2toa6();
	move_a1a2();
	linead();
	pop_a2toa6();
}


void undraw_sprite( SSB *ssb )
{
	(void) ssb; /* ssb moved below */
	push_a2();
	move_a0a2();
	lineac();
	pop_a2();
}


void filled_rect( int x1, int y1, int x2, int y2 )
{
	LINEA *linea = Linea;
	
	linea->x1 = x1;
	linea->y1 = y1;
	linea->x2 = x2;
	linea->y2 = y2;
	push_a2();
	linea5();
	pop_a2();
}


void transform_mouse(MFORM *mform)
{
	LINEA *linea = Linea;
	short *intin = linea->intin;
	
	linea->intin = (void *)mform;
	push_a2();
	lineab();
	pop_a2();
	linea->intin = intin;
}


void set_fg_bp(int auswahl)
{
	LINEA *linea = Linea;
	short *fg = &linea->fg_bp_1;
	short mask = 0x1;
	
	*fg++ = auswahl & mask; auswahl >>= 1;
	*fg++ = auswahl & mask; auswahl >>= 1;
	*fg++ = auswahl & mask; auswahl >>= 1;
	*fg++ = auswahl & mask; auswahl >>= 1;
	/*
	 * only set next vars if really needed,
	 * not only for optimization, but also
	 * because those variables might not
	 * even exist on older TOS
	 */
	if (linea->v_planes >= 8)
	{
		short *fg = &linea->fg_bp_5;
		*fg++ = auswahl & mask; auswahl >>= 1;
		*fg++ = auswahl & mask; auswahl >>= 1;
		*fg++ = auswahl & mask; auswahl >>= 1;
		*fg++ = auswahl & mask; auswahl >>= 1;
	}
}


void set_text_blt(FONT_HDR *font, int scale, int style, int chup, int text_fg, int text_bg)
{
	LINEA *linea;
	FONT_HDR *hdr;
	
	if (font != NULL)
		__Font = font;
	hdr = __Font;
	linea = Linea;
	linea->scale = scale;
	linea->style = style;
	linea->chup = chup;
	if ((linea->mono_status = hdr->flags.mono_spaced) != 0)
		linea->delx = hdr->wcel_wdt;
	linea->l_off = hdr->lft_ofst;
	linea->r_off = hdr->rgt_ofst;
	linea->weight = hdr->thckning;
	linea->litemask = hdr->lghtng_m;
	linea->skewmask = hdr->skewng_m;
	linea->fbase = hdr->fnt_dta;
	linea->fwidth = hdr->frm_wdt;
	linea->dely = hdr->frm_hgt;
	linea->dda_inc = 0;
	linea->t_sclsts = 0;
	linea->sourcey = 0;
	linea->text_fg = text_fg;
	linea->text_bg = text_bg;
	linea->scrtchp = NULL;
	linea->xacc_dda = 0x8000;
	linea->scrpt2 = 0x40;
}


void text_blt(int x, int y, unsigned char c)
{
	LINEA *linea;
	FONT_HDR *hdr;
	
	linea = Linea;
	if (x >= 0)
	{
		linea->destx = x;
		linea->desty = y;
	}
	hdr = __Font;
	c = (c - hdr->ade_lo) & 0xff;
	if (hdr->flags.mono_spaced)
	{
		linea->sourcex = c * (unsigned short)hdr->wcel_wdt;
	} else
	{
		short *offsets = hdr->ch_ofst + c;
		x = offsets[0];
		linea->sourcex = x;
		linea->delx = offsets[1] - x;
	}
	push_a2();
	linea8();
	pop_a2();
}


void print_string( int x, int y, int xoff, char *string)
{
	LINEA *linea;
	FONT_HDR *hdr;
	unsigned char c;
	
	linea = Linea;
	linea->destx = x;
	linea->desty = y;
	hdr = __Font;
	while (*string != '\0')
	{
		c = (*(const unsigned char *)string - hdr->ade_lo) & 0xff;
		if (hdr->flags.mono_spaced)
		{
			linea->sourcex = c * (unsigned short)hdr->wcel_wdt;
		} else
		{
			short *offsets = hdr->ch_ofst + c;
			x = offsets[0];
			linea->sourcex = x;
			linea->delx = offsets[1] - x;
		}
		push_a2();
		linea8();
		pop_a2();
		linea->destx += xoff;
		string++;
	}
}


void __filled_polygon(void)
{
	push_a2();
	linea6();
	pop_a2();
}


void filled_polygon(int *xy, int count)
{
	int i;
	int miny, maxy;
	
	for (i = 0; i < (count * 2); i++)
		Linea->ptsin[i] = xy[i];
	Linea->contrl[1] = count;
	count *= 2;
	Linea->ptsin[count] = xy[0];
	Linea->ptsin[count + 1] = xy[1];
	miny = 32767;
	maxy = 0;
	for (i = 1; i < count; i += 2)
	{
		if (xy[i] < miny)
			miny = xy[i];
		if (xy[i] > maxy)
			maxy = xy[i];
	}
	for (i = miny; i <= maxy; i++)
	{
		Linea->y1 = i;
		__filled_polygon();
	}
}


static void put4pixel(int x, int y, int radius, long color)
{
	put_pixel(x, y + radius, color);
	put_pixel(x, y - radius, color);
	put_pixel(x + radius, y, color);
	put_pixel(x - radius, y, color);
}


static void put8pixel(int x, int y, int xradius, int yradius, long color)
{
	put_pixel(x + xradius, y + yradius, color);
	put_pixel(x + xradius, y - yradius, color);
	put_pixel(x - xradius, y + yradius, color);
	put_pixel(x - xradius, y - yradius, color);
	if (xradius != yradius)
	{
		put_pixel(x + yradius, y + xradius, color);
		put_pixel(x + yradius, y - xradius, color);
		put_pixel(x - yradius, y + xradius, color);
		put_pixel(x - yradius, y - xradius, color);
	}
}


void draw_circle(int x, int y, int radius, long color)
{
	int xradius;
	int yradius;
	int diff;
	int off;
	int tmp;
	
	put4pixel(x, y, radius, color);
	if (radius > 1)
	{
		xradius = 1;
		yradius = radius;
		off = 0;
		diff = 3;
		radius = diff - 2 * radius;
		do {
			off += diff;
			if (((tmp = off + radius) < 0 ? -tmp : tmp) < (off < 0 ? -off : off))
			{
				off = tmp;
				radius += 2;
				yradius--;
			}
			diff += 2;
			put8pixel(x, y, xradius, yradius, color);
			xradius += 1;
		} while (xradius < yradius);
	}
}
