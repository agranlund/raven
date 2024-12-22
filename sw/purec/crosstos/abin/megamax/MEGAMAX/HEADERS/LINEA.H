
#ifndef Lna_INIT

#define	Lna_INIT		0xa000	/* Initialize Line-A data structures	*/
#define	Lna_PUTPIXEL	0xa001	/* Put Pixel onto graphics screen		*/
#define	Lna_GETPIXEL	0xa002	/* Get Pixel value on graphics screen	*/
#define	Lna_LINE		0xa003	/* Draw a Line							*/
#define	Lna_HLINE		0xa004	/* Draw a Horizontal Line				*/
#define	Lna_FILLRECT	0xa005	/* Draw a filled box (rectangle)		*/
#define	Lna_FILLPOLY	0xa006	/* Draw a line polygon and fill it		*/
#define	Lna_BITBLIT		0xa007	/* Bit Block Transfer					*/
#define	Lna_TEXTBLIT	0xa008	/* Text Block Transfer					*/
#define	Lna_SHOWMOUSE	0xa009	/* Show Mouse Cursor					*/
#define	Lna_HIDEMOUSE	0xa00a	/* Hide Mouse Cursor					*/
#define	Lna_NEWMOUSE	0xa00b	/* Change Mouse form					*/
#define	Lna_UNSPRITE	0xa00c	/* Undraw sprite						*/
#define	Lna_DRAWSPRITE	0xa00d	/* Draw sprite							*/
#define	Lna_COPYRASTER	0xa00e	/* Copy Raster Form						*/
#define	Lna_SEEDFILL	0xa00f	/* Do Seed fill on polygon				*/


/*
	Miscellaneous Data Structure
*/
typedef struct {
    int x, y;
} point;


typedef struct {
	int top;
	int left;
	int bottom;
	int right;
} rect;


/*
	Font Header Data Structure
*/
typedef struct _fontform {
    int  fontid;        /* Font Identifier                 */
    int  fontsize;      /* Font Size in points             */
    char fontname[32];  /* Font name                       */

    int  lowascii;      /* lowest displayable ASCII char   */
    int  highascii;     /* highest displayable ASCII char  */ 

    /*
        Character drawing offsets (see vst_alignment())
    */
    int  top;           /* offset from baseline to top     */
    int  ascent;        /* offset from baseline to ascent  */
    int  half;          /* offset from baseline to half    */
    int  descent;       /* offset from baseline to descent */
    int  bottom;        /* offset from baseline to bottom  */

    int  largechar;     /* widest character in font        */
    int  largeboxchar;  /* widest character cell in font   */

    int  kern;          /* kerning offset                  */
    int  rightoffset;   /* right offset for italics        */

    /*
        Text Effects masks
    */
    int  boldmask;
    int  underlinemask;
    int  litemask;
    int  skewmask;

    struct {
        unsigned system    : 1;  /* is it a system font?   */
        unsigned horiz     : 1;  /* horiz offset table?    */
        unsigned swapbytes : 1;  /* integers are reversed? */
        unsigned monospace : 1;  /* is font monospace?     */
    } flags;

    int *horztable;     /* pointer to horizontal offset table */
    int *chartable;     /* pointer to character offset table  */
    int *fonttable;     /* pointer to font bit-image data */

    int formwidth;
    int formheight;

    struct _fontform *nextfont;  /* pointer to next font def */
} fontform;


/*
	Text Data Structure
*/
typedef struct  {
    int      xdda;         /* drawing work variable               */
    int      ddainc;       /* drawing work variable               */
    int      scaledir;     /* drawing work variable               */
    int      mono;         /* monospaced font flag                */
    int      fontx;        /* character (x, y) in font def        */
    int      fonty;
    int      scrnx;        /* charcter (x, y) on screen           */
    int      scrny;
    int      charwidth;    /* height of character                 */
    int      charheight;   /* width of character                  */
    char	*fontdata;     /* pointer to font bit-image data	  */ 
    int      fontwidth;    /* width of font form                  */
    int      fontstyle;    /* font style                          */
    int      litemask;     /* mask for dehilited text             */
    int      skewmask;     /* mask for italics text               */
    int      boldmask;     /* mask for bold text                  */
    int      fsuper;       /* offset for superscript text         */
    int      fsub;         /* offset for subscript text           */
    int      scaleflag;    /* 0 = no scaling                      */
    int      textdir;      /* text orientation flag               */
    int      forecolor;    /* foreground text color               */
    char	 *textefx;     /* pointer to start of text special    */
                           /* effects buffer                      */
    int      scalebuf;     /* offset for scale buffer in textefx  */
    int      backcolor     /* background text color               */
} textblock;


typedef struct {
    /*
        Drawing Environment
    */
    int  vplanes;           /* Number of video planes */
    int  vwrap;             /* Number of bytes per video scan */
    int *cntrl;             /* pointer to VDI contrl array */
    int *intin;             /* pointer to VDI intin array */
    int *ptsin;             /* pointer to VDI ptsin array */
    int *intout;            /* pointer to VDI intout array */
    int *ptsout;            /* pointer to VDI ptsout array */
    int  plane0;            /* color bit mask for plane 0 */
    int  plane1;            /* color bit mask for plane 1 */
    int  plane2;            /* color bit mask for plane 2 */
    int  plane3;            /* color bit mask for plane 3 */
    int  minusone;          /* -1 used in XOR mode */
    int  linemask;          /* VDI line style */
    int  writemode;         /* VDI write mode */
    int  x1, y1, x2, y2;    /* drawing rectangle */
    int *patptr;            /* pointer to current VDI fill patter */
    int  patmask;           /* size of fill pattern mask */
    int  planefill;         /* number of planes to fill (0 = 1 plane) */
    int  clipflag;          /* clipping flag (0 = no clipping) */
    int  xminclip, yminclip;    /* clipping rectangle */
    int  xmaxclip, ymaxclip;

    /*
        Font Information
    */
    textblock thetext;      /* Text Drawing Block                  */

    /*
        Miscellaneous Drawing Variables
    */
    int  copymode;          /* copy mode for raster operations */
    int (*seedabort)();     /* pointer to seed fill abort routine */
} lineaport; 


typedef struct {
    rect source;
    rect destin;
} copyblock;


typedef struct {
    int   x;
    int   y;
    int  *base;
    int   offset;
    int   width;
    int   plane_offset;
} bitblock;


typedef struct {
    int   width;        /* width of bit block   */
    int   height;       /* height of bit block  */
    int   planecount;   /* number of planes     */
    int   ForeColor;
    int   BackColor;
    char  table[4];

    /*
        Bit blocks to Blit
    */
    bitblock source;
    bitblock destin;
   
    /*
        Pattern Information
    */ 
    int  *patbuf;
    int   pat_offset;
    int   pat_width;
    int   pat_plane_offset;
    int   pat_mask;
   
    /*
        Temp Work space
    */ 
    int   work[12]; 
} blitblock;


typedef struct {
    int x;            /* x offset of hot spot */
    int y;            /* y offset of hot spot */
    int format;       /* 0 = Copy, 1 = XOR    */
    int forecolor;    /* background color     */
    int backcolor;    /* foreground color     */
    int image[32];    /* bit-image of sprite  */
} sprite;
typedef sprite mouse;

/*
    Save area for area behind Sprite.  Needs to be
        4 * sizeof(Sprite) so that all four color
        planes can be saved.
*/
typedef sprite spriteback[4];

extern lineaport *a_init();

/*
	Used by Line-A routines
		_lnaport  == pointer to line-a variables.
		_fonthdrs == pointer to three pointers to system font headers
*/
extern lineaport *_lnaport;
extern fontform  **_fonthdrs;

#endif
