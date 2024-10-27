/*      LINEA.H

        LineA Definitions

        Copyright (C) Borland International 1990
        All Rights Reserved.
*/


#if  !defined( __LINEA_H__ )
#define __LINEA_H__

#if  !defined( __AES_H__ )
#include <aes.h>                    /* for MFORM                    */
#endif /* __AES_H__ */


#define REPLACE 0                   /* set_wrt_mode()               */
#define TRANS   1
#define XOR     2
#define INVERS  3

#define VDIFM 1                     /* sdb.form                     */
#define XORFM -1


typedef struct
{
    int xhot,                       /* X-Offset                     */
        yhot,                       /* Y-Offset                     */
        form,                       /* Format (1 = VDI, -1 = XOR)   */
        bgcol,                      /* Hintergrundfarbe             */
        fgcol,                      /* Vordergrundfarbe             */
        image[32];                  /* Sprite-Image                 */
} SDB;

typedef int SSB[10 + 4 * 64];

typedef int PATTERN[16];            /* Bei Bedarf vergrîûern        */

typedef struct fonthdr
{
    short id;                       /* Fontnummer                   */
    short size;                     /* Fontgrîûe in Punkten         */
    char facename[32];              /* Name                         */
    short ade_lo,                   /* kleinster ASCII-Wert         */
        ade_hi,                     /* grîûter ASCII-Wert           */
        top_dist,                   /* Abstand Top <-> Baseline     */
        asc_dist,                   /* Abstand Ascent <-> Baseline  */
        hlf_dist,                   /* Abstand Half <-> Baseline    */
        des_dist,                   /* Abstand Descent <-> Baseline */
        bot_dist,                   /* Abstand Bottom <-> Baseline  */
        wchr_wdt,                   /* maximale Zeichenbreite       */
        wcel_wdt,                   /* maximale Zeichenzellenbreite */
        lft_ofst,                   /* Offset links                 */
        rgt_ofst,                   /* Offset rechts                */
        thckning,                   /* Verbreiterungsfaktor fÅr Bold*/
        undrline,                   /* Dicke der Unterstreichung    */
        lghtng_m,                   /* Maske fÅr Light              */
        skewng_m;                   /* Maske fÅr Kursiv             */
    struct
    {
        unsigned             :12;   /* Frei                         */
        unsigned mono_spaced : 1;   /* Proportional/Monospaced      */
        unsigned f68000      : 1;   /* 8086-/68000 Format           */
        unsigned hot         : 1;   /* HOT verwenden                */
        unsigned system      : 1;   /* Default system font          */
    }   flags;
    char *hz_ofst;                  /* Horizontal Offset Table      */
    short *ch_ofst;                 /* Font-Offset-Tabelle          */
    void *fnt_dta;                  /* Zeichensatz-Image            */
    short frm_wdt,                  /* Breite des Font-Image        */
        frm_hgt;                    /* Hîhe des Fonts               */
    struct fonthdr *next;           /* NÑchster Font                */
}   FONT_HDR;

typedef struct
{
    short v_planes,                 /* # Bitplanes (1, 2 od. 4)     */
        v_lin_wr,                   /* # Bytes/Scanline             */
        *contrl,
        *intin,
        *ptsin,                     /* Koordinaten-Eingabe          */
        *intout,
        *ptsout,                    /* Koordinaten-Ausgabe          */
        fg_bp_1,                    /* Plane 0                      */
        fg_bp_2,                    /* Plane 1                      */
        fg_bp_3,                    /* Plane 2                      */
        fg_bp_4,                    /* Plane 3                      */
        lstlin;
    unsigned short ln_mask;         /* Linienmuster                 */
    short wrt_mode,                 /* Schreib-Modus                */
        x1, y1, x2, y2;             /* Koordinaten                  */
    void *patptr;                   /* FÅllmuster                   */
    unsigned short patmsk;          /* .. dazugehîrige Maske        */
    short multifill,                /* FÅllmuster fÅr Planes        */
        clip,                       /* Flag fÅr Clipping            */
        xmn_clip, ymn_clip,
        xmx_clip, ymx_clip,         /* Clipping Rechteck            */
                                    /* Rest fÅr text_blt:           */
        xacc_dda,
        dda_inc,                    /* Vergrîûerungsfaktor          */
        t_sclsts,                   /* Vergrîûerungsrichtung        */
        mono_status,                /* Proportionalschrift          */
        sourcex, sourcey,           /* Koordinaten im Font          */
        destx, desty,               /* Bildschirmkoordinaten        */
        delx, dely;                 /* Breite und Hîhe des Zeichen  */
    void *fbase;                    /* Start der Font-Daten         */
    short fwidth,                   /* Breite des Fontimage         */
        style;                      /* Schreibstil                  */
    unsigned short litemask,        /* Maske fÅr Light              */
             skewmask;              /* Maske fÅr Kursiv             */
    short weight,                   /* Breite bei Bold              */
        r_off,                      /* Kursiv-Offset rechts         */
        l_off,                      /* Kursiv-Offset links          */
        scale,                      /* Vergrîûerung ja/nein         */
        chup,                       /* Rotationswinkel *10          */
        text_fg;                    /* Textfarbe                    */
    void *scrtchp;                  /* Buffer                       */
    short scrpt2,                   /* Index in Buffer              */
        text_bg,                    /* unbenutzt                    */
        copy_tran;                  /* --                           */
    short (*fill_abort)(void);      /* Testet Seed Fill             */
/*
 * variables below were never documented, and
 * may differ depending on TOS version used.
 * The layout is below is maybe valid for MULTITOS only.
 */
    short (*user_dev_init)(void);   /* ptr to user routine before dev_init */
    short (*user_esc_init)(void);   /* ptr to user routine before esc_init */
    long reserved2[8];
    short (**routines)(void);       /* ptr to primitives vector list */
    void *curdev;					/* ptr to a current device structure */
    short blt_mode;                 /* 0: soft BiT BLiT 1: hard BiT BLiT */
    short reserved3;
    short reqx_col[240][3];         /* extended request color array */
    short *sv_blk_ptr;              /* point to proper save block   */
    long fg_bplanes;                /* fg bit plns flags (bit 0 is plane 0) */
    short fg_bp_5;                  /* Plane 4                      */
    short fg_bp_6;                  /* Plane 5                      */
    short fg_bp_7;                  /* Plane 6                      */
    short fg_bp_8;                  /* Plane 7                      */
	short _save_len;                /* height of saved form         */
	short *_save_addr;              /* screen addr of 1st word of plane 0 */
	short _save_stat;               /* cursor save status */
	long _save_area[256];           /* save up to 8 planes. 16 longs/plane */
	short q_circle[80];             /* space to build circle coordinates */
	
	short byte_per_pix;             /* number of bytes per pixel (0 if < 1) */
	short form_id;                  /* scrn form 2 ST, 1 stndrd, 3 pix */
	long vl_col_bg;                 /* escape background color (long value) */
	long vl_col_fg;                 /* escape foreground color (long value) */
	long pal_map[256];              /* either a mapping of reg's or true val */
	short (*primitives[40])(void);  /* space to copy vectors into */
} LINEA;

typedef struct
{
    short _angle;
    short begang;
    FONT_HDR *cur_font;             /* Zeiger auf Header akt. Font  */
    short reserved5[23];            /* reserviert                   */
    short m_pos_hx;                 /* X-Koordinate Maus            */
    short m_pos_hy;                 /* Y-Koordinate Maus            */
    short m_planes;                 /* Zeichenmodus der Maus        */
    short m_cdb_bg;                 /* Maus Hintergrundfarbe        */
    short m_cdb_fg;                 /* Maus Vordergrundfarbe        */
    short mask_form[32];            /* Vordergrund und Maske        */
    short inq_tab[45];              /* wie vq_extnd()               */
    short dev_tab[45];              /* wie v_opnwk()                */
    short gcurx;                    /* X-Position Maus              */
    short gcury;                    /* Y-Position Maus              */
    short m_hid_ct;                 /* Anzahl der hide_mouse-calls  */
    short mouse_bt;                 /* Status der Mausknîpfe        */
    short req_col[16][3];           /* Interne Daten fÅr vq_color() */
    short siz_tab[15];              /* wie v_opnwk()                */
    short term_ch;                  /* 16 bit character info        */
    short chc_mode;                 /* mode of the choice device    */
    void *cur_work;                 /* Attribute der akt. Workstn.  */
    FONT_HDR *def_font;             /* Standard Systemzeichensatz   */
    FONT_HDR *font_ring[4];         /* Zeichensatzlisten            */
    short font_count;               /* Anzahl der ZeichensÑtze      */
    short line_cw;                  /* current line width           */
    short loc_mode;                 /* mode of locator device       */
    short num_qc_lines;             /* # of lines in quarter circle */
    long trap14sav;                 /* space to save return addr    */
    long col_or_mask;               /* some modes this is ORed in VS_COLOR */
    long col_and_mask;              /* some modes this is ANDed in VS_COLOR */
    long trap14bsav;                /* space to save return addr    */
    short reserved0[32];            /* reserviert                   */
    short st_rmode;                 /* mode of the string device    */
    short val_mode;                 /* mode of the valuator device  */
    char cur_ms_stat;               /* Mausstatus                   */
    char reserved1;                 /* reserviert                   */
    short v_hid_cnt;                /* Anzahl der Hide_cursor-calls */
    short cur_x;                    /* X-Position Maus              */
    short cur_y;                    /* Y-Position Maus              */
    char cur_flag;                  /* != 0: Maus neu zeichnen      */
    char mouse_flag;                /* != 0: Maus-Interrupt ein     */
    long trap1sav;                  /* space to save return address */
    short v_sav_xy[2];              /* gerettete X-Y-Koordinaten    */
    short save_len;                 /* Anzahl der Bildschirmzeilen  */
    void *save_addr;                /* Erstes Byte im Bildspeicher  */
    short save_stat;                /* Dirty-Flag                   */
    long save_area[4][16];          /* Buffer fÅr Bild unter Maus   */
    void (*user_tim)( void );       /* Timer-Interrupt-Vektor       */
    void (*next_tim)( void );       /* alter Interrupt              */
    void (*user_but)( void );       /* Maustasten-Vektor            */
    void (*user_cur)( void );       /* Maus-Vektor                  */
    void (*user_mot)( void );       /* Mausbewegungs-Vektor         */
    short v_cel_ht;                 /* Zeichenhîhe                  */
    short v_cel_mx;                 /* maximale Cursorspalte        */
    short v_cel_my;                 /* maximale Cursorzeile         */
    short v_cel_wr;                 /* Characterzeilenbreite        */
    short v_col_bg;                 /* Hintergrundfarbe             */
    short v_col_fg;                 /* Vordergrundfarbe             */
    void *v_cur_ad;                 /* Adresse der Cursorposition   */
    short v_cur_off;                /* Vertikaler Bildschirmoffset  */
    short v_cur_xy[2];              /* X-Y-Cursor                   */
    char v_period;                  /* Blinkgeschwindigkeit         */
    char v_cur_ct;                  /* ZÑhler fÅr Blinken           */
    void *v_fnt_ad;                 /* Zeiger auf Font              */
    short v_fnt_nd;                 /* grîûter ASCII-Wert           */
    short v_fnt_st;                 /* kleinster ASCII-Wert         */
    short v_fnt_wd;                 /* Breite des Fontimage in Byte */
    short v_rez_hz;                 /* Bildschirmbreite in Pixel    */
    short *v_off_ad;                /* Font-Offset-Tabelle          */
    unsigned char v_stat_0;         /* Cursorflag                   */
                                    /* #0: blink enabled */
                                    /* #1: currently invers */
                                    /* #2: cursor visible */
                                    /* #3: wrapping on (ESC v */
                                    /* #4: invers on (ESC p) */
                                    /* #5: cursor position saved in v_sav_xy (ESC j) */
    unsigned char v_delay;          /* Cursor blink delay           */
    short v_rez_vt;                 /* Bildschirmhîhe in Pixel      */
    short bytes_lin;                /* Bytes pro Pixelzeile         */
}   VDIESC;

typedef struct
{
    int b_wd,                       /* Breite des Blocks in Pixeln  */
        b_ht,                       /* Hîhe des Blocks in Pixeln    */
        plane_ct,                   /* Anzahl der Farbplanes        */
        fg_col,                     /* Vordergrundfarbe             */
        bg_col;                     /* Hintergrundfarbe             */
    char op_tab[4];                 /* VerknÅpfung (fÅr jede Plane) */
    int s_xmin,                     /* X-Quellraster                */
        s_ymin;                     /* Y-Quellraster                */
    void *s_form;                   /* Adresse des Quellrasters     */
    int s_nxwd,                     /* Offset zum nÑchsten Wort     */
        s_nxln,                     /* Breite des Quellrasters      */
        s_nxpl,                     /* Offset zur nÑchsten Plane    */
        d_xmin,                     /* X-Zielraster                 */
        d_ymin;                     /* Y-Zielraster                 */
    void *d_form;                   /* Adresse des Zielrasters      */
    int d_nxwd,                     /* Offset zum nÑchsten Wort     */
        d_nxln,                     /* Breite des Quellrasters      */
        d_nxpl;                     /* Offset zur nÑchsten Plane    */
    void *p_addr;                   /* 16-Bit-Masken zum Undieren   */
    int p_nxln,                     /* Breite der Maske in Bytes    */
        p_nxpl,                     /* Offset zur nÑchsten Plane    */
        p_mask;                     /* Hîhe der Maske in Zeilen     */
    char filler[24];                /* Interner Buffer              */
}   BITBLT;

typedef struct
{
    FONT_HDR *font[3];
} FONTS;

typedef struct
{
    int (*funp[16])( void );
} LINEA_FUNP;


void linea_init( void );
void put_pixel( int x, int y, long color );
long get_pixel( int x, int y );
void draw_line(int x1, int y1, int x2, int y2);
        /* set_fg_bp(), set_ln_mask(), set_wrt_mode() */
void horizontal_line( int x1, int y1, int x2 );
        /* set_fg_bp(), set_wrt_mode(), set_pattern() */
void filled_rect( int x1, int y1, int x2, int y2 );
        /* set_fg_bp(), set_wrt_mode(), set_pattern(), set_clipping() */
void filled_polygon( int *xy, int count );
        /* set_fg_bp(), set_wrt_mode(), set_pattern(), set_clipping() */
void bit_blt(BITBLT *bitblt);
void text_blt( int x, int y, unsigned char c );
        /* set_txtblt() */
void show_mouse( int flag );
void hide_mouse( void );
void transform_mouse( MFORM *mform );
void undraw_sprite( SSB *ssb );
void draw_sprite( int x, int y, SDB *sdb, SSB *ssb );
void copy_raster( void );                   /* 14, COPY RASTER FORM */
void seed_fill( void );                     /* 15, SEED FILL        */
        /* WARNING: 14 & 15 are NOT supported ! */

void set_fg_bp( int auswahl );
void set_ln_mask( int mask );
void set_wrt_mode( int modus );
void set_pattern( int *pattern, int mask, int multifill );
void set_clip( int x1, int y1, int x2, int y2, int modus );
void set_text_blt( FONT_HDR *font, int scale, int style, int chup,
                   int text_fg, int text_bg );

void draw_circle( int x, int y, int radius, long color );
void print_string( int x, int y, int xoff, char *string );


extern LINEA *Linea;
extern VDIESC *Vdiesc;
extern FONTS *Fonts;
extern LINEA_FUNP *Linea_funp;

#endif /* __LINEA_H__ */

/**************************************************************************/

