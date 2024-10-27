#ifndef __PORTVDI_H__
#define __PORTVDI_H__

#ifndef __PORTAB_H__
#include <portab.h>
#endif
#include <stdint.h>

EXTERN_C_BEG

typedef struct _vdi_control {
	_WORD opcode;
	_WORD nptsin;
	_WORD nptsout;
	_WORD nintin;
	_WORD nintout;
	_WORD escape;
	_WORD handle;
} VDI_CONTROL;

#ifndef __VDIPB
#define __VDIPB
typedef struct
{
    _WORD    *contrl;
    _WORD    *intin;
    _WORD    *ptsin;
    _WORD    *intout;
    _WORD    *ptsout;
} VDIPB;
#endif

/* RGB intesities in promille */
typedef struct rgb_1000
{
	_WORD  red;    /* Red-Intensity in range [0..1000] */
 	_WORD  green;  /* Green-Intensity in range [0..1000] */
 	_WORD  blue;   /* Blue-Intensity in range [0..1000] */
} RGB1000;

extern VDIPB _VdiParBlk;

typedef int32_t fix31;

/*
 * should actually be unsigned short, to match wchar_t,
 * but that would break most bindings
 */
typedef short vdi_wchar_t; /* 16bit string, eg. for unicode */

#define fix31_to_point(a) ((_WORD)((((a) + 32768L) >> 16)))
#define point_to_fix31(a) (((fix31)(a)) << 16)


void vdi( VDIPB *vdipb );

typedef _WORD VdiHdl;   /* for better readability */

#ifndef __PXY
# define __PXY
typedef struct point_coord
{
	_WORD p_x;
	_WORD p_y;
} PXY;
#endif

/****** Control definitions *********************************************/

void    v_opnwk( _WORD work_in[16],  _WORD *handle, _WORD work_out[57]);
void    v_clswk( _WORD handle );
void    v_opnvwk( _WORD work_in[11], _WORD *handle, _WORD work_out[57]);
void    v_clsvwk( _WORD handle );
void    v_clrwk( _WORD handle );
void    v_updwk( _WORD handle );
_WORD    vst_load_fonts( _WORD handle, _WORD select );
void    vst_unload_fonts( _WORD handle, _WORD select );
void    vs_clip( _WORD handle, _WORD clip_flag, _WORD *pxyarray );
void  vs_clip_pxy(VdiHdl handle, PXY pxy[]);
void  vs_clip_off(VdiHdl handle);


/****** Output definitions **********************************************/

void    v_pline( _WORD handle, _WORD count, _WORD *pxyarray );
void    v_pmarker( _WORD handle, _WORD count, _WORD *pxyarray );
void    v_gtext( _WORD handle, _WORD x, _WORD y, const char *string );
void    v_gtextn( _WORD handle, _WORD x, _WORD y, const char *string, _WORD len );
void    v_gtext16( _WORD handle, _WORD x, _WORD y, const vdi_wchar_t *string );
void    v_gtext16n( _WORD handle, _WORD x, _WORD y, const vdi_wchar_t *string, _WORD len );
void    v_fillarea( _WORD handle, _WORD count, _WORD *pxyarray );
void    v_cellarray( _WORD handle, _WORD *pxyarray, _WORD row_length,
                     _WORD el_used, _WORD num_rows, _WORD wrt_mode,
                     _WORD *colarray );
void    v_contourfill( _WORD handle, _WORD x, _WORD y, _WORD index );
void    vr_recfl( _WORD handle, _WORD *pxyarray );
void    v_bar( _WORD handle, _WORD *pxyarray );
void    v_arc( _WORD handle, _WORD x, _WORD y, _WORD radius,
               _WORD begang, _WORD endang );
void    v_pieslice( _WORD handle, _WORD x, _WORD y, _WORD radius,
                    _WORD begang, _WORD endang );
void    v_circle( _WORD handle, _WORD x, _WORD y, _WORD radius );
void    v_ellarc( _WORD handle, _WORD x, _WORD y, _WORD xradius,
                  _WORD yradius, _WORD begang, _WORD endang );
void    v_ellpie( _WORD handle, _WORD x, _WORD y, _WORD xradius,
                  _WORD yradius, _WORD begang, _WORD endang );
void    v_ellipse( _WORD handle, _WORD x, _WORD y, _WORD xradius,
                   _WORD yradius  );
void    v_rbox  ( _WORD handle, _WORD *pxyarray );
void    v_rfbox ( _WORD handle, _WORD *pxyarray );
void    v_justified( _WORD handle, _WORD x, _WORD y, const char *string,
                     _WORD length, _WORD word_space,
                     _WORD char_space );
void	v_justified16(_WORD handle, _WORD x, _WORD y, const vdi_wchar_t *wstr, _WORD len, _WORD word_space, _WORD char_space);
void	v_justified16n(_WORD handle, _WORD x, _WORD y, const vdi_wchar_t *wstr, _WORD num, _WORD len, _WORD word_space, _WORD char_space);


/****** Attribute definitions *****************************************/

#define IP_HOLLOW       0
#define IP_1PATT        1
#define IP_2PATT        2
#define IP_3PATT        3
#define IP_4PATT        4
#define IP_5PATT        5
#define IP_6PATT        6
#define IP_SOLID        7


/* gsx modes */

#define MD_REPLACE      1
#define MD_TRANS        2
#define MD_XOR          3
#define MD_ERASE        4


/* gsx styles */

#define FIS_HOLLOW      0
#define FIS_SOLID       1
#define FIS_PATTERN     2
#define FIS_HATCH       3
#define FIS_USER        4


/* bit blt rules */

#define ALL_WHITE        0		/* D := 0 */
#define S_AND_D          1		/* D := S AND D */
#define S_AND_NOTD       2		/* D := S AND (NOT D) */
#define S_ONLY           3		/* D := S */
#define NOTS_AND_D       4		/* D := (NOT S) AND D */
#define D_ONLY           5		/* D := D */
#define S_XOR_D          6		/* D := S XOR D */
#define S_OR_D           7		/* D := S OR D */
#define NOT_SORD         8		/* D := NOT (S OR D) */
#define NOT_SXORD        9		/* D := NOT (S XOR D) */
#define D_INVERT        10		/* D := NOT D */
/*
 * There seems to be mismatch with the following 3 definitions (10-12)
 * - in all Atari-related sources i have seen, they are defined as
 *   NOT_D       == 10 (same as D_INVERT)
 *   S_OR_NOTD   == 11
 *   NOT_S       == 12
 * - PC-GEM defines them as
 *   S_OR_NOTD   == 11
 *   NOT_D       == 12
 *   NOT_S not defined
 * - The Atari VDI actually performs:
 *   10          == D := NOT D
 *   11          == D := S OR (NOT D)
 *   12          == D := NOT S
 */
#define NOT_D			10		/* D := NOT D */
#define S_OR_NOTD		11		/* D := S OR (NOT D) */
#define NOT_S			12		/* D := NOT S */
#define NOTS_OR_D       13		/* D := (NOT S) OR D */
#define NOT_SANDD       14		/* D := NOT (S AND D) */
#define ALL_BLACK       15		/* D := 1 */


/* v_bez modes */
#define BEZ_BEZIER		0x01
#define BEZ_POLYLINE	0x00
#define BEZ_NODRAW		0x02

/* v_bit_image modes */
#define IMAGE_LEFT		0
#define IMAGE_CENTER	1
#define IMAGE_RIGHT		2
#define IMAGE_TOP 		0
#define IMAGE_BOTTOM	2

/* v_justified modes */
#define NOJUSTIFY		0
#define JUSTIFY			1

/* vq_color modes */
#define COLOR_REQUESTED		0
#define COLOR_ACTUAL		1

/* vqin_mode & vsin_mode modes */
#define VINMODE_LOCATOR		1
#define VINMODE_VALUATOR	2
#define VINMODE_CHOICE		3
#define VINMODE_STRING		4

/* other names */
#define DEV_LOCATOR      VINMODE_LOCATOR
#define DEV_VALUATOR     VINMODE_VALUATOR
#define DEV_CHOICE       VINMODE_CHOICE
#define DEV_STRING       VINMODE_STRING

#ifdef __GEMLIB_OLDNAMES
#define LOCATOR			VINMODE_LOCATOR
#define VALUATOR		VINMODE_VALUATOR
#define CHOICE			VINMODE_CHOICE
#define STRING			VINMODE_STRING
#endif

/* input mode */
#define MODE_REQUEST     1
#define MODE_SAMPLE      2

/* vqt_cachesize modes */
#define CACHE_CHAR		0
#define CACHE_MISC		1

/* vqt_devinfo return values */
#define DEV_MISSING		0
#define DEV_INSTALLED		1

/* vsf_perimeter modes */
#define PERIMETER_OFF		0
#define PERIMETER_ON		1

/* linetypes */

#define SOLID           1
#define LONGDASH        2
#define DOT             3
#define DASHDOT         4
#define DASH            5
#define DASH2DOT        6
#define USERLINE        7

/* other names */
#define LT_SOLID		SOLID
#define LT_LONGDASH		LONGDASH
#define LT_DOTTED		DOT
#define LT_DASHDOT		DASHDOT
#define LT_DASHED		DASH
#define LT_DASHDOTDOT	DASH2DOT
#define LT_USERDEF		USERLINE
#define LDASHED			LONGDASH
#define DOTTED			DOT
#define DASHDOTDOT		DASH2DOT

/* line ends */

#define SQUARED          0
#define ARROWED          1
#define ROUNDED          2
/* other names */
#define ROUND ROUNDED
#define SQUARE SQUARED
#define LE_SQUARED	SQUARED
#define LE_ARROWED	ARROWED
#define LE_ROUNDED	ROUNDED

/* polymarker types */

#define MRKR_DOT		1
#define MRKR_PLUS 		2
#define MRKR_ASTERISK	3
#define MRKR_BOX		4
#define MRKR_CROSS		5
#define MRKR_DIAMOND	6

/* other names */
#define MT_DOT		MRKR_DOT
#define MT_PLUS		MRKR_PLUS
#define MT_ASTERISK	MRKR_ASTERISK
#define MT_SQUARE	MRKR_BOX
#define MT_DCROSS	MRKR_CROSS
#define MT_DIAMOND	MRKR_DIAMOND
#define PM_DOT           MRKR_DOT
#define PM_PLUS          MRKR_PLUS
#define PM_ASTERISK      MRKR_ASTERISK
#define PM_SQUARE        MRKR_BOX
#define PM_DIAGCROSS     MRKR_CROSS
#define PM_DIAMOND       MRKR_DIAMOND

/* vst_alignment modes */
#undef TA_BOTTOM /* clashes with Win32 */
#undef TA_TOP /* clashes with Win32 */
#undef TA_CENTER /* clashes with Win32 */
#define TA_LEFT         	0 /* horizontal */
#define TA_CENTER       	1
#define TA_RIGHT        	2
#define TA_BASE         	0 /* vertical */
#define TA_HALF         	1
#define TA_ASCENT       	2
#define TA_BOTTOM       	3
#define TA_DESCENT      	4
#define TA_TOP          	5

/* horizontal text alignment */
#define ALI_LEFT         TA_LEFT
#define ALI_CENTER       TA_CENTER
#define ALI_RIGHT        TA_RIGHT

/* vertical text alignment */
#define ALI_BASE         TA_BASE
#define ALI_HALF         TA_HALF
#define ALI_ASCENT       TA_ASCENT
#define ALI_BOTTOM       TA_BOTTOM
#define ALI_DESCENT      TA_DESCENT
#define ALI_TOP          TA_TOP

/* vst_charmap modes */
#define MAP_BITSTREAM   0
#define MAP_ATARI       1
#define MAP_UNICODE     2 /* for vst_map_mode, NVDI 4 */

/* text effects */
#define TXT_NORMAL       0x0000
#define TXT_THICKENED    0x0001
#define TXT_LIGHT        0x0002
#define TXT_SKEWED       0x0004
#define TXT_UNDERLINED   0x0008
#define TXT_OUTLINED     0x0010
#define TXT_SHADOWED     0x0020

/* other names */
#define	TF_NORMAL		TXT_NORMAL
#define TF_THICKENED	TXT_THICKENED
#define TF_LIGHTENED	TXT_LIGHT
#define TF_SLANTED		TXT_SKEWED
#define TF_UNDERLINED	TXT_UNDERLINED
#define TF_OUTLINED		TXT_OUTLINED
#define TF_SHADOWED		TXT_SHADOWED

/* vst_error modes */
#define APP_ERROR		0
#define SCREEN_ERROR	1

/* vst_error return values */
#undef NO_ERROR /* clashes with Win32 */
#define NO_ERROR		0
#define CHAR_NOT_FOUND	1
#define FILE_READERR 	8
#define FILE_OPENERR 	9
#define BAD_FORMAT		10
#define CACHE_FULL		11
#define MISC_ERROR		(-1)

/* vst_kern tmodes */
#define TRACK_NONE		0
#define TRACK_NORMAL 	1
#define TRACK_TIGHT		2
#define TRACK_VERYTIGHT	3

/* vst_kern pmodes */
#define PAIR_OFF		0
#define PAIR_ON			1

/* vst_scratch modes */
#define SCRATCH_BOTH		0
#define SCRATCH_BITMAP		1
#define SCRATCH_NONE		2

/* v_updwk return values */
#define SLM_OK			0 /* no error */
#define SLM_ERROR		2 /* general printer error */
#define SLM_NOTONER		3 /* toner empty */
#define SLM_NOPAPER		5 /* paper empty */

#define	PRINTER_OK			0
#define	PRINTER_ERROR		(-1)
#define	PRINTER_NOTREADY	(-2)
#define	PRINTER_NOPAPER		(-9)
#define	DRIVER_NOEXIST		(-15)


/****** Escape library *******************************************************/

#define O_B_BOLDFACE     '0' /* OUT-File definitions for v_alpha_text */
#define O_E_BOLDFACE     '1'
#define O_B_ITALICS      '2'
#define O_E_ITALICS      '3'
#define O_B_UNDERSCORE   '4'
#define O_E_UNDERSCORE   '5'
#define O_B_SUPERSCRIPT  '6'
#define O_E_SUPERSCRIPT  '7'
#define O_B_SUBSCRIPT    '8'
#define O_E_SUBSCRIPT    '9'
#define O_B_NLQ          'A'
#define O_E_NLQ          'B'
#define O_B_EXPANDED     'C'
#define O_E_EXPANDED     'D'
#define O_B_LIGHT        'E'
#define O_E_LIGHT        'F'
#define O_PICA           'W'
#define O_ELITE          'X'
#define O_CONDENSED      'Y'
#define O_PROPORTIONAL   'Z'

#define O_GRAPHICS       "\033\033GEM,%d,%d,%d,%d,%s"

#define MUTE_RETURN     -1 /* definitions for vs_mute */
#define MUTE_ENABLE      0
#define MUTE_DISABLE     1

#define OR_PORTRAIT      0 /* definitions for v_orient */
#define OR_LANDSCAPE     1

#define TRAY_MANUAL     -1 /* definitions for v_tray */
#define TRAY_DEFAULT     0
#define TRAY_FIRSTOPT    1

#define XBIT_FRACT       0 /* definitions for v_xbit_image */
#define XBIT_INTEGER     1

#define XBIT_LEFT        0
#define XBIT_CENTER      1
#define XBIT_RIGHT       2

#define XBIT_TOP         0
#define XBIT_MIDDLE      1
#define XBIT_BOTTOM      2

_WORD    vswr_mode( _WORD handle, _WORD mode );
void    vs_color( _WORD handle, _WORD index, const _WORD rgb_in[3] );
_WORD    vsl_type( _WORD handle, _WORD style );
void    vsl_udsty( _WORD handle, _WORD pattern );
_WORD    vsl_width( _WORD handle, _WORD width );
_WORD    vsl_color( _WORD handle, _WORD color_index );
void    vsl_ends( _WORD handle, _WORD beg_style, _WORD end_style );
_WORD    vsm_type( _WORD handle, _WORD symbol );
_WORD    vsm_height( _WORD handle, _WORD height );
_WORD    vsm_color( _WORD handle, _WORD color_index );
void     vst_height( _WORD handle, _WORD height, _WORD *char_width,
                    _WORD *char_height, _WORD *cell_width,
                    _WORD *cell_height );
_WORD    vst_point( _WORD handle, _WORD point, _WORD *char_width,
                    _WORD *char_height, _WORD *cell_width,
                    _WORD *cell_height );
_WORD    vst_rotation( _WORD handle, _WORD angle );
_WORD    vst_font( _WORD handle, _WORD font );
_WORD    vst_color( _WORD handle, _WORD color_index );
_WORD    vst_effects( _WORD handle, _WORD effect );
void    vst_alignment( _WORD handle, _WORD hor_in, _WORD vert_in,
                       _WORD *hor_out, _WORD *vert_out );
_WORD    vsf_interior( _WORD handle, _WORD style );
_WORD    vsf_style( _WORD handle, _WORD style_index );
_WORD    vsf_color( _WORD handle, _WORD color_index );
_WORD    vsf_perimeter( _WORD handle, _WORD per_vis );
_WORD vsf_xperimeter(VdiHdl , _WORD vis, _WORD style);
void    vsf_udpat( _WORD handle, _WORD *pfill_pat, _WORD planes );
_WORD v_copies          (VdiHdl , _WORD count);
_WORD v_orient          (VdiHdl , _WORD orientation);
_WORD v_page_size       (VdiHdl , _WORD page_id);
_WORD v_trays           (VdiHdl , _WORD input, _WORD output, _WORD *set_input, _WORD *set_output);
void v_ps_halftone     (VdiHdl , _WORD _index, _WORD _angle, _WORD _frequency ); 
_WORD vq_calibrate      (VdiHdl , _WORD *flag);
_WORD vq_page_name      (VdiHdl , _WORD page_id, char *page_name, _LONG *page_width, _LONG *page_height);
_WORD vq_tray_names     (VdiHdl , char *input_name, char *output_name, _WORD *input, _WORD *output);
_WORD vs_calibrate      (VdiHdl , _WORD flag, _WORD *rgb);
void v_etext(VdiHdl handle, _WORD x, _WORD y, const char *string, _WORD offsets[]);
void v_tray(_WORD handle, _WORD tray);
void v_setrgbi(_WORD handle, _WORD primtype, _WORD r, _WORD g, _WORD b, _WORD i);
void v_xbit_image(short handle, const char *filename, _WORD aspect, _WORD x_scale, _WORD y_scale, _WORD h_align, _WORD v_align, _WORD rotation, _WORD background, _WORD foreground, _WORD xy[]);
void v_topbot(_WORD handle, _WORD height, _WORD *char_width, 
                 _WORD *char_height, _WORD *cell_width, 
                 _WORD *cell_height);
void vs_bkcolor(_WORD handle, _WORD color);
void v_pat_rotate(_WORD handle, _WORD angle);
void vs_grayoverride(_WORD handle, _WORD grayval);


/****** Raster definitions *********************************************/

#if !defined(__MFDB__) && !defined(__MFDB)
#define __MFDB__
#define __MFDB
typedef struct
{
        void            *fd_addr;
        _WORD             fd_w;
        _WORD             fd_h;
        _WORD             fd_wdwidth;
        _WORD             fd_stand;
        _WORD             fd_nplanes;
        _WORD             fd_r1;
        _WORD             fd_r2;
        _WORD             fd_r3;
} MFDB;
#endif

void    vro_cpyfm( _WORD handle, _WORD vr_mode, _WORD *pxyarray,
                   MFDB *psrcMFDB, MFDB *pdesMFDB );
void    vrt_cpyfm( _WORD handle, _WORD vr_mode, _WORD *pxyarray,
                   MFDB *psrcMFDB, MFDB *pdesMFDB,
                   _WORD *color_index );
void    vr_trnfm( _WORD handle, MFDB *psrcMFDB, MFDB *pdesMFDB );
void    v_get_pixel( _WORD handle, _WORD x, _WORD y, _WORD *pel,
                     _WORD *index );


/****** Input definitions **********************************************/

#ifdef OS_ATARI
/*
 * type of handler passed to the routine installed by vex_butv.
 * On atari, this is an interupt routine that gets the
 * new mouse button mask in d0.
 */
typedef void (*VEX_BUTV)(void);
/*
 * type of handler passed to the routine installed by vex_curv/vex_motv.
 * On atari, this is an interupt routine that gets the
 * new mouse position x/y in d0/d1.
 */
typedef void (*VEX_CURV)(void);
typedef void (*VEX_MOTV)(void);
/*
 * type of handler passed to the routine installed by vex_timv.
 * On atari, this is an interupt routine that gets the
 * new mouse position x/y in d0/d1.
 */
typedef void (*VEX_TIMV)(void);
/*
 * type of handler passed to the routine installed by vex_wheelv.
 * On atari, this is an interupt routine that gets the
 * wheel number in d0, and the amount in d1.
 */
typedef void (*VEX_WHEELV)(void);
#else
typedef void (*VEX_BUTV)(_WORD newmask);
typedef void (*VEX_CURV)(_WORD x, _WORD y);
typedef void (*VEX_MOTV)(_WORD x, _WORD y);
typedef void (*VEX_TIMV)(void);
typedef void (*VEX_WHEELV)(_WORD wheel_number, _WORD wheel_amount);
#endif

_WORD    vsin_mode( _WORD handle, _WORD dev_type, _WORD mode );
void    vrq_locator( _WORD handle, _WORD x, _WORD y, _WORD *xout,
                     _WORD *yout, _WORD *term );
_WORD    vsm_locator( _WORD handle, _WORD x, _WORD y, _WORD *xout,
                     _WORD *yout, _WORD *term );
void    vrq_valuator( _WORD handle, _WORD valuator_in,
                      _WORD *valuator_out, _WORD *terminator );
void    vsm_valuator( _WORD handle, _WORD val_in, _WORD *val_out,
                      _WORD *term, _WORD *status );
void    vrq_choice( _WORD handle, _WORD ch_in, _WORD *ch_out );
_WORD    vsm_choice( _WORD handle, _WORD *choice );
void vrq_string( _WORD handle, _WORD max_length, _WORD echo_mode, _WORD *echo_xy, char *string );
void vrq_string16( _WORD handle, _WORD max_length, _WORD echo_mode, _WORD *echo_xy, _WORD *string );
_WORD vsm_string( _WORD handle, _WORD max_length, _WORD echo_mode, _WORD *echo_xy, char *string );
_WORD vsm_string16( _WORD handle, _WORD max_length, _WORD echo_mode, _WORD *echo_xy, _WORD *string );
void    vsc_form( _WORD handle, _WORD *pcur_form );
void    vex_timv( _WORD handle, VEX_TIMV tim_addr, VEX_TIMV *otim_addr, _WORD *tim_conv );
void    v_show_c( _WORD handle, _WORD reset );
void    v_hide_c( _WORD handle );
void    vq_mouse( _WORD handle, _WORD *pstatus, _WORD *x, _WORD *y );
void    vex_butv( _WORD handle, VEX_BUTV pusrcode, VEX_BUTV *psavcode);
void    vex_motv( _WORD handle, VEX_MOTV pusrcode, VEX_MOTV *psavcode);
void    vex_curv( _WORD handle, VEX_CURV pusrcode, VEX_CURV *psavcode);
void    vex_wheelv(VdiHdl handle, VEX_WHEELV pusrcode, VEX_WHEELV *psavcode);
void    vq_key_s( _WORD handle, _WORD *pstatus );


/****** Inquire definitions *******************************************/

void    vq_extnd( _WORD handle, _WORD owflag, _WORD *work_out );
_WORD    vq_color( _WORD handle, _WORD color_index,
                  _WORD set_flag, _WORD *rgb );
void    vql_attributes( _WORD handle, _WORD *attrib );
void    vqm_attributes( _WORD handle, _WORD *attrib );
void    vqf_attributes( _WORD handle, _WORD *attrib );
void    vqt_attributes( _WORD handle, _WORD *attrib );
void    vqt_extent( _WORD handle, const char *string, _WORD *extent );
void  vqt_extent16(VdiHdl handle, const vdi_wchar_t *wstr, _WORD *extent);
void    vqt_extentn( _WORD handle, const char *string, _WORD len, _WORD *extent );
void  vqt_extent16n  (VdiHdl , const vdi_wchar_t *wstr, _WORD num, _WORD *extent);
_WORD    vqt_width( _WORD handle, _WORD character,
                   _WORD *cell_width, _WORD *left_delta,
                   _WORD *right_delta );
_WORD    vqt_name( _WORD handle, _WORD element_num, char *name);
void    vq_cellarray( _WORD handle, _WORD *pxyarray,
                      _WORD row_length, _WORD num_rows,
                      _WORD *el_used, _WORD *rows_used,
                      _WORD *status, _WORD *colarray );
void    vqin_mode( _WORD handle, _WORD dew_type, _WORD *input_mode );
void    vqt_fontinfo( _WORD handle, _WORD *minADE, _WORD *maxADE,
                      _WORD *distances, _WORD *maxwidth,
                      _WORD *effects );

/****** Escape definitions *********************************************/

void    vq_chcells( _WORD handle, _WORD *rows, _WORD *columns );
void    v_exit_cur( _WORD handle );
void    v_enter_cur( _WORD handle );
void    v_curup( _WORD handle );
void    v_curdown( _WORD handle );
void    v_curright( _WORD handle );
void    v_curleft( _WORD handle );
void    v_curhome( _WORD handle );
void    v_eeos( _WORD handle );
void    v_eeol( _WORD handle );
void    vs_curaddress( _WORD handle, _WORD row, _WORD column );
void    v_curaddress( _WORD handle, _WORD row, _WORD column );
void    v_curtext( _WORD handle, const char *string );
void	v_curtext16n(_WORD handle, const vdi_wchar_t *wstr, _WORD num);
void    v_rvon( _WORD handle );
void    v_rvoff( _WORD handle );
void    vq_curaddress( _WORD handle, _WORD *row, _WORD *column );
_WORD    vq_tabstatus( _WORD handle );
void    v_hardcopy( _WORD handle );
void    v_dspcur( _WORD handle, _WORD x, _WORD y );
void    v_rmcur( _WORD handle );
void    v_form_adv( _WORD handle );
void    v_output_window( _WORD handle, _WORD *xyarray );
void    v_clear_disp_list( _WORD handle );
void    v_bit_image( _WORD handle, const char *filename,
                     _WORD aspect, _WORD x_scale, _WORD y_scale,
                     _WORD h_align, _WORD v_align, _WORD *xyarray );
void    vq_scan( _WORD handle, _WORD *g_slice, _WORD *g_page,
                 _WORD *a_slice, _WORD *a_page, _WORD *div_fac);
void    v_alpha_text( _WORD handle, const char *string );
void	v_alpha_text16n(_WORD handle, const vdi_wchar_t *wstr, _WORD num);
_WORD   vs_palette( _WORD handle, _WORD palette );
void	v_sound( _WORD handle, _WORD frequency, _WORD duration );
_WORD	vs_mute( _WORD handle, _WORD action );
_WORD    vqp_films( _WORD handle, char *film_names );
_WORD    vqp_filmname(_WORD handle, _WORD index, char *name);
void    vqp_state( _WORD handle, _WORD *port, _WORD *film,
                   _WORD *lightness, _WORD *interlace,
                   _WORD *planes, _WORD *indexes );
void    vsp_state( _WORD handle, _WORD port, _WORD film_num,
                   _WORD lightness, _WORD interlace, _WORD planes,
                   _WORD *indexes );
void    vsp_save( _WORD handle );
void    vsp_message( _WORD handle );
_WORD    vqp_error( _WORD handle );
void    v_meta_extents( _WORD handle, _WORD min_x, _WORD min_y,
                        _WORD max_x, _WORD max_y );
void    v_write_meta( _WORD handle,
                      _WORD num_intin, _WORD *intin,
                      _WORD num_ptsin, _WORD *ptsin );
void    vm_coords( _WORD handle, _WORD llx, _WORD lly, _WORD urx, _WORD ury );
void    vm_filename( _WORD handle, const char *filename );
void    vm_pagesize( _WORD handle, _WORD pgwidth, _WORD pdheight );
void    v_offset( _WORD handle, _WORD offset );
void    v_fontinit( _WORD handle, const void * font_header );
void    v_escape2000( _WORD handle, _WORD times );

void    vt_resolution( _WORD handle, _WORD xres, _WORD yres,
                       _WORD *xset, _WORD *yset );
void    vt_axis( _WORD handle, _WORD xres, _WORD yres,
                 _WORD *xset, _WORD *yset );
void    vt_origin( _WORD handle, _WORD xorigin, _WORD yorigin );
void    vq_tdimensions( _WORD handle, _WORD *xdimension, _WORD *ydimension );
void    vt_alignment( _WORD handle, _WORD dx, _WORD dy );
void    vsp_film( _WORD handle, _WORD index, _WORD lightness );
void    vsc_expose( _WORD handle, _WORD state );


/* return values for vq_vgdos() inquiry */
#define GDOS_NONE      (-2L)          /* no GDOS installed */
#define GDOS_FSM       0x5F46534DL    /* '_FSM' - FSMGDOS installed  */
#define GDOS_FNT       0x5F464E54L    /* '_FNT' - FONTGDOS installed */
#define GDOS_ATARI     0x0007E88AL    /* GDOS 1.1 von Atari Corp.    */
#define GDOS_AMC       0x0007E864L    /* AMCGDos von Arnd Beissner   */
#define GDOS_AMCLIGHT  0x0007E8BAL    /* GEMINI-Spezial-GDos von Arnd Beissner */
#define GDOS_NVDI      0x00000000L    /* NVDI von Bela GmbH */
#define GDOS_TTF       0x3e5d0957L    /* TTF-GDOS */

/* Testet ob ein GDOS geladen ist */
/* Achtung, das ABC-GEM (GEM 2.1) schmiert bei dieser Funktion ab!!! */
_WORD    vq_gdos( void );
_LONG	vq_vgdos( void );

/****** Bezier definitions *********************************************/

_WORD    v_bez_on( _WORD handle );
void    v_bez_off( _WORD handle );
_WORD    v_bez_con( _WORD handle, _WORD onoff );
void    v_set_app_buff( _WORD handle, void **buf_p, _WORD nparagraphs );
void    v_bez( _WORD handle, _WORD count, _WORD *xyarr, char *bezarr, _WORD *extent, _WORD *totpts, _WORD *totmoves );
void    v_bez_fill( _WORD handle, _WORD count, _WORD *xyarr, char *bezarr, _WORD *extent, _WORD *totpts, _WORD *totmoves );
_WORD   v_bez_qual( _WORD handle, _WORD prcnt, _WORD *actual );


/****** FSMGDOS definitions ********************************************/

typedef struct
{
    _WORD    value;
    _WORD    remainder;

}   fsm_int;

typedef struct
{
    fsm_int    x;
    fsm_int    y;

}   fsm_fpoint_t;

typedef struct
{
    fsm_fpoint_t     pt;
    fsm_fpoint_t     cpt;
    fsm_int          sharp;

}   fsm_data_fpoint_t;

typedef struct fsm_component_t
{
    _WORD                       resevered1;
    struct fsm_component_t    *nextComponent;
    unsigned char             numPoints;
    unsigned char             numCurves;
    unsigned char             numContours;
    unsigned char             reserved2[13];
    fsm_data_fpoint_t         *points;
    unsigned char             *startPts;

}   fsm_component_t;


void    vqt_f_extent( _WORD handle, const char *string, _WORD *extent );
void	vqt_f_extent16  (VdiHdl, const vdi_wchar_t *str, _WORD extent[]);
void    vqt_f_extentn( _WORD handle, const char *string, _WORD len, _WORD *extent );
void	vqt_f_extent16n (VdiHdl, const vdi_wchar_t *str, _WORD num, _WORD extent[]);
void    v_killoutline( _WORD handle, fsm_component_t *component );
/* void    v_getoutline( _WORD handle, _WORD ch, fsm_component_t **component ); */
void    v_getoutline(_WORD handle, _WORD ch, _WORD *xyarray, char *bezarray, _WORD maxpts, _WORD *count);
void    vst_scratch( _WORD handle, _WORD mode );
void    vst_error( _WORD handle, _WORD mode, _WORD *errorvar );
void    vqt_advance( _WORD handle, _WORD ch, _WORD *advx, _WORD *advy,
                       _WORD *remx, _WORD *remy );
void    vqt_advance32( _WORD handle, _WORD ch, fix31 *advx, fix31 *advy);
_WORD    vst_arbpt( _WORD handle, _WORD point, _WORD *chwd, _WORD *chht,
                   _WORD *cellwd, _WORD *cellht );
fix31   vst_arbpt32( _WORD handle, fix31 point, _WORD *chwd, _WORD *chht,
                   _WORD *cellwd, _WORD *cellht );
void    vqt_devinfo( _WORD handle, _WORD devnum, _WORD *devexits, char *filename, char *device_name );
_WORD	vq_devinfo(_WORD handle, _WORD device, _WORD *dev_exists, char *file_name, char *device_name);
_WORD    v_flushcache( _WORD handle );
void    vqt_cachesize( _WORD handle, _WORD which_cache, _LONG *size );
void    vqt_get_tables( _WORD handle, _WORD **gascii, _WORD **sytle );
_WORD    v_loadcache( _WORD handle, const char *filename, _WORD mode );
_WORD    v_savecache( _WORD handle, const char *filename );
_WORD    vst_setsize( _WORD handle, _WORD point, _WORD *chwd, _WORD *chht, _WORD *cellwd, _WORD *cellht );
fix31   vst_setsize32( _WORD handle, fix31 point, _WORD *chwd, _WORD *chht, _WORD *cellwd, _WORD *cellht );
_WORD    vst_skew( _WORD handle, _WORD skew );
void	v_shtext( _WORD handle, _WORD x, _WORD y, const char *text, _WORD color, _WORD xshadow, _WORD yshadow);
void	vqt_get_table(_WORD handle, _WORD **map);
void	vqt_get_tables( _WORD handle, _WORD **gascii, _WORD **style );
void	vst_charmap(_WORD handle, _WORD mode);
void	vst_kern(_WORD handle, _WORD tmode, _WORD pmode, _WORD *tracks, _WORD *pairs);
void	vqt_fontheader(_WORD handle, char *buffer, char *pathname);
void	vqt_pairkern(_WORD handle, _WORD ch1, _WORD ch2, fix31 *x, fix31 *y);
void	vqt_trackkern(_WORD handle, fix31 *x, fix31 *y);
void	v_getbitmap_info(_WORD handle, _WORD ch,
        	fix31 *advancex, fix31 *advancey, fix31 *xoffset, fix31 *yoffset,
        	_WORD *width, _WORD *height, _WORD **bitmap);



void	v_opnbm(_WORD *work_in, MFDB *bitmap, _WORD *handle, _WORD *work_out);
void	v_clsbm(_WORD handle);
void	vq_scrninfo(_WORD handle, _WORD *work_out);

void v_get_driver_info(_WORD device, _WORD select, char *info_string);


void vqt_real_extent(_WORD handle, _WORD x, _WORD y, const char *string, _WORD *extent);
void vqt_real_extentn(_WORD handle, _WORD x, _WORD y, const char *string, _WORD len, _WORD *extent);
void vqt_real_extent16n(_WORD handle, _WORD x, _WORD y, const vdi_wchar_t *wstring, _WORD num, _WORD *extent);


/****** SPEEDO definitions ********************************************/

#define IMG_MASK	0x1
#define IMG_OK		0x1

#define TGA_MASK	0x00000110
#define	TGA_TYPE_2	0x4

#if 0
#define APPL	0
#define DOC		1
#define CREAT	2
#define REM		3
#endif

typedef struct
{
	_WORD nbplanes;
	_WORD width;
	_WORD height;
} BIT_IMAGE;

_WORD vq_margins(_WORD handle, _WORD *top, _WORD *bot, _WORD *lft, _WORD *rgt, _WORD *xdpi, _WORD *ydpi);
_WORD vq_driver_info(_WORD handle, _WORD *lib, _WORD *drv, _WORD *plane, _WORD *attr, char name[27]);
_WORD vq_bit_image(_WORD handle, _WORD *ver, _WORD *maximg, _WORD *form);
_WORD vs_page_info(_WORD handle, _WORD type, const char txt[60]);
_WORD vs_crop(_WORD handle, _WORD ltx1, _WORD lty1, _WORD ltx2, _WORD lty2, _WORD ltlen, _WORD ltoffset);
_WORD vq_image_type(_WORD handle, const char *file, BIT_IMAGE *img);
_WORD vs_save_disp_list(_WORD handle, const char *name);
_WORD vs_load_disp_list(_WORD handle, const char *name);



/*
 * The following functions requires NVDI version 3.x or higher
 */

/** structure to store information about a font */
#ifndef __XFNT_INFO
#define __XFNT_INFO
typedef struct
{
	int32_t		size;				/* length of the structure, initialize this entry before
	                                     calling vqt_xfntinfo() */
	_WORD		format;				/* font format, e.g. 4 for TrueType */
	_WORD		id;					/* font ID, e.g. 6059 */
	_WORD		index;				/* index */
	char		font_name[50];		/* font name, e.g. "Century 725 Italic BT" */
	char		family_name[50];	/* name of the font family, e.g. "Century725 BT" */
	char		style_name[50];		/* name of the font style, e.g. "Italic" */
	char		file_name1[200];	/* name of the first font file,
	                                     e.g. "C:\\FONTS\\TT1059M_.TTF" */
	char		file_name2[200];	/* name of the 2nd font file */
	char		file_name3[200];	/* name of the 3rd font file */
	_WORD		pt_cnt;				/* number of available point sizes (vst_point()),
	                                     e.g. 10 */
	_WORD		pt_sizes[64];		/* available point sizes,
                                         e.g. { 8, 9, 10, 11, 12, 14, 18, 24, 36, 48 } */
} XFNT_INFO;
#endif

void    v_ftext( _WORD handle, _WORD x, _WORD y, const char *string );
void	v_ftext_offset(_WORD handle, _WORD x, _WORD y, const char *sstr, const _WORD *offset);
void    v_ftextn( _WORD handle, _WORD x, _WORD y, const char *string, _WORD len );
void	v_ftext16       (VdiHdl, _WORD x, _WORD y, const vdi_wchar_t *wstr);
void	v_ftext16n      (VdiHdl, _WORD x, _WORD y, const vdi_wchar_t *wstr, _WORD num);
void	v_ftext_offset16(VdiHdl, _WORD x, _WORD y, const vdi_wchar_t *wstr, const _WORD *offset);
void	v_ftext_offset16n(VdiHdl, _WORD x, _WORD y, const vdi_wchar_t *wstr, _WORD num, const _WORD *offset);
_WORD	vq_ext_devinfo (VdiHdl, _WORD device, _WORD *dev_exists, char *file_path, char *file_name, char *name);
_WORD	vqt_ext_name    (VdiHdl, _WORD __index, char *name, _WORD *font_format, _WORD *flags);
_WORD	vqt_name_and_id (VdiHdl, _WORD font_format, char *font_name, char *ret_name);
_WORD	vqt_xfntinfo    (VdiHdl, _WORD flags, _WORD id, _WORD __index, XFNT_INFO *info);
_WORD vst_name 	(VdiHdl, _WORD font_format, char *font_name, char *ret_name);
void  vst_track_offset(VdiHdl, fix31 offset, _WORD pairmode, _WORD *tracks, _WORD *pairs);
/* another name for vst_track_offset */
#define vst_kern_info vst_track_offset
void  vst_width	(VdiHdl, _WORD width, _WORD *char_width, _WORD *char_height, _WORD *cell_width, _WORD *cell_height);

/*
 * The following functions requires NVDI version 4.x or higher
 */
_WORD vqt_char_index (_WORD handle, _WORD scr_index, _WORD scr_mode, _WORD dst_mode);
_WORD vst_map_mode   (_WORD handle, _WORD mode);

#define vqt_is_char_available(handle,unicode) \
	(vqt_char_index(handle,unicode,CHARIDX_UNICODE,CHARIDX_DIRECT)!=0xFFFF)

/*
 * The following functions requires NVDI version 5.x or higher
 */

/*----------------------------------------------------------------------------------------*/
/* Function witch use for the printer dialog from WDialog                                 */
/*----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------*/
/* Konstanten fr Pixelformate																				*/
/*----------------------------------------------------------------------------------------*/
#define	PX_1COMP	0x01000000L										/* Pixel besteht aus einer benutzten Komponente: Farbindex */
#define	PX_2COMP	0x02000000L										/* Pixel besteht aus zwei benutzten Komponenten, z.B. AG */
#define	PX_3COMP	0x03000000L										/* Pixel besteht aus drei benutzten Komponenten, z.B. RGB */
#define	PX_4COMP	0x04000000L										/* Pixel besteht aus vier benutzten Komponenten, z.B. CMYK */

#define	PX_REVERSED	0x00800000L										/* Pixel wird in umgekehrter Bytreihenfolge ausgegeben */
#define	PX_xFIRST	0x00400000L										/* unbenutzte Bits liegen vor den benutzen (im Motorola-Format betrachtet) */
#define	PX_kFIRST	0x00200000L										/* K liegt vor CMY (im Motorola-Format betrachtet) */
#define	PX_aFIRST	0x00100000L										/* Alphakanal liegen vor den Farbbits (im Motorola-Format betrachtet) */

#define	PX_PACKED	0x00020000L										/* Bits sind aufeinanderfolgend abgelegt */
#define	PX_PLANES	0x00010000L										/* Bits sind auf mehrere Ebenen verteilt (Reihenfolge: 0, 1, ..., n) */
#define	PX_IPLANES	0x00000000L										/* Bits sind auf mehrere Worte verteilt (Reihenfolge: 0, 1, ..., n) */

#define	PX_USES1	0x00000100L										/* 1 Bit des Pixels wird benutzt */
#define	PX_USES2	0x00000200L										/* 2 Bit des Pixels werden benutzt */
#define	PX_USES3	0x00000300L										/* 3 Bit des Pixels werden benutzt */
#define	PX_USES4	0x00000400L										/* 4 Bit des Pixels werden benutzt */
#define	PX_USES8	0x00000800L										/* 8 Bit des Pixels werden benutzt */
#define	PX_USES15	0x00000f00L										/* 15 Bit des Pixels werden benutzt */
#define	PX_USES16	0x00001000L										/* 16 Bit des Pixels werden benutzt */
#define	PX_USES24	0x00001800L										/* 24 Bit des Pixels werden benutzt */
#define	PX_USES30	0x00001e00L										/* 30 Bit des Pixels werden benutzt */
#define	PX_USES32	0x00002000L										/* 32 Bit des Pixels werden benutzt */
#define	PX_USES48	0x00003000L										/* 48 Bit des Pixels werden benutzt */

#define	PX_1BIT		0x00000001L										/* Pixel besteht aus 1 Bit */
#define	PX_2BIT		0x00000002L										/* Pixel besteht aus 2 Bit */
#define	PX_3BIT		0x00000003L										/* Pixel besteht aus 3 Bit */
#define	PX_4BIT		0x00000004L										/* Pixel besteht aus 4 Bit */
#define	PX_8BIT		0x00000008L										/* Pixel besteht aus 8 Bit */
#define	PX_16BIT	0x00000010L										/* Pixel besteht aus 16 Bit */
#define	PX_24BIT	0x00000018L										/* Pixel besteht aus 24 Bit */
#define	PX_32BIT	0x00000020L										/* Pixel besteht aus 32 Bit */
#define	PX_48BIT	0x00000030L										/* Pixel besteht aus 48 Bit */
#define	PX_64BIT	0x00000040L										/* Pixel besteht aus 64 Bit */

#define	PX_CMPNTS	0x0f000000L										/* Maske fr Anzahl der Pixelkomponenten */
#define	PX_FLAGS	0x00f00000L										/* Maske fr diverse Flags */
#define	PX_PACKING	0x00030000L										/* Maske fr Pixelformat */
#define	PX_USED		0x00003f00L										/* Maske fr Anzahl der benutzten Bits */
#define	PX_BITS		0x0000003fL										/* Maske fr Anzahl der Bits pro Pixel */

/*----------------------------------------------------------------------------------------*/
/* Pixelformate fr ATARI-Grafik																				*/
/*----------------------------------------------------------------------------------------*/
#define	PX_ATARI1	( PX_PACKED | PX_1COMP | PX_USES1 | PX_1BIT )
#define	PX_ATARI2	( PX_IPLANES | PX_1COMP | PX_USES2 | PX_2BIT )
#define	PX_ATARI4	( PX_IPLANES | PX_1COMP | PX_USES4 | PX_4BIT )
#define	PX_ATARI8	( PX_IPLANES | PX_1COMP | PX_USES8 | PX_8BIT )
#define	PX_FALCON15	( PX_PACKED | PX_3COMP | PX_USES16 | PX_16BIT )

/*----------------------------------------------------------------------------------------*/
/* Pixelformate fr Macintosh																					*/
/*----------------------------------------------------------------------------------------*/
#define	PX_MAC1		( PX_PACKED | PX_1COMP | PX_USES1 | PX_1BIT )
#define	PX_MAC4		( PX_PACKED | PX_1COMP | PX_USES4 | PX_4BIT )
#define	PX_MAC8		( PX_PACKED | PX_1COMP | PX_USES8 | PX_8BIT )
#define	PX_MAC15	( PX_xFIRST | PX_PACKED | PX_3COMP | PX_USES15 | PX_16BIT )
#define	PX_MAC32	( PX_xFIRST | PX_PACKED | PX_3COMP | PX_USES24 | PX_32BIT )

/*----------------------------------------------------------------------------------------*/
/* Pixelformate fr Grafikkarten																				*/
/*----------------------------------------------------------------------------------------*/
#define	PX_VGA1		( PX_PACKED | PX_1COMP | PX_USES1 | PX_1BIT )
#define	PX_VGA4		( PX_PLANES | PX_1COMP | PX_USES4 | PX_4BIT )
#define	PX_VGA8		( PX_PACKED | PX_1COMP | PX_USES8 | PX_8BIT )
#define	PX_VGA15	( PX_REVERSED | PX_xFIRST | PX_PACKED | PX_3COMP | PX_USES15 | PX_16BIT )
#define	PX_VGA16	( PX_REVERSED | PX_PACKED | PX_3COMP | PX_USES16 | PX_16BIT )
#define	PX_VGA24	( PX_REVERSED | PX_PACKED | PX_3COMP | PX_USES24 | PX_24BIT )
#define	PX_VGA32	( PX_REVERSED | PX_xFIRST | PX_PACKED | PX_3COMP | PX_USES24 | PX_32BIT )

#define	PX_MATRIX16	( PX_PACKED | PX_3COMP | PX_USES16 | PX_16BIT )

#define	PX_NOVA32	( PX_PACKED | PX_3COMP | PX_USES24 | PX_32BIT )

/*----------------------------------------------------------------------------------------*/
/* Pixelformate fr Drucker																					*/
/*----------------------------------------------------------------------------------------*/
#define	PX_PRN1		( PX_PACKED | PX_1COMP | PX_USES1 | PX_1BIT )
#define	PX_PRN8		( PX_PACKED | PX_1COMP | PX_USES8 | PX_8BIT )
#define	PX_PRN32	( PX_xFIRST | PX_PACKED | PX_3COMP | PX_USES24 | PX_32BIT )

/*----------------------------------------------------------------------------------------*/
/* bevorzugte (schnelle) Pixelformate fr Bitmaps 														*/
/*----------------------------------------------------------------------------------------*/

#define	PX_PREF1	( PX_PACKED | PX_1COMP | PX_USES1 | PX_1BIT )
#define	PX_PREF2	( PX_PACKED | PX_1COMP | PX_USES2 | PX_2BIT )
#define	PX_PREF4	( PX_PACKED | PX_1COMP | PX_USES4 | PX_4BIT )
#define	PX_PREF8	( PX_PACKED | PX_1COMP | PX_USES8 | PX_8BIT )
#define	PX_PREF15	( PX_xFIRST | PX_PACKED | PX_3COMP | PX_USES15 | PX_16BIT )
#define	PX_PREF32	( PX_xFIRST | PX_PACKED | PX_3COMP | PX_USES24 | PX_32BIT )

#define	PX_RGB		( PX_PACKED | PX_3COMP | PX_USES24 | PX_24BIT )
#define	PX_BGR		( PX_REVERSED | PX_PACKED | PX_3COMP | PX_USES24 | PX_24BIT )
#define	PX_RGBA		( PX_PACKED | PX_3COMP | PX_USES32 | PX_32BIT )
#define	PX_BGRA		( PX_REVERSED | PX_PACKED | PX_3COMP | PX_USES32 | PX_32BIT )

/*----------------------------------------------------------------------------------------*/
/* Farbtabellen																									*/
/*----------------------------------------------------------------------------------------*/

enum
{
	CSPACE_UNKNOWN =	0x0000,
	CSPACE_RGB		=	0x0001,
	CSPACE_ARGB		=	0x0002,
	CSPACE_CMYK		=	0x0004,
	CSPACE_HSL		=	0x0008,	/* ###BETA */

	CSPACE_YCbCr	=	0x0100,	/* ###BETA */
	CSPACE_YCCK		=	0x0200,	/* ###BETA */
	CSPACE_GRAY		=	0x0400,	/* ###BETA */
	CSPACE_AGRAY	=	0x0800	/* ###BETA */
};

enum
{
	CSPACE_1COMPONENT	= 0x0001,
	CSPACE_2COMPONENTS	= 0x0002,
	CSPACE_3COMPONENTS	= 0x0003,
	CSPACE_4COMPONENTS	= 0x0004
};

typedef struct
{
	unsigned short reserved;
	unsigned short red;
	unsigned short green;
	unsigned short blue;
} COLOR_RGB;

typedef struct
{
	unsigned short cyan;
	unsigned short magenta;
	unsigned short yellow;
	unsigned short black;
} COLOR_CMYK;

typedef union
{
	COLOR_RGB rgb;
	COLOR_CMYK cmyk;
} COLOR_ENTRY;

#define	COLOR_TAB_MAGIC	0x63746162L /* 'ctab' */

typedef struct
{
	int32_t		magic;				/* 'ctab' */
	int32_t		length;
	int32_t		format;
	int32_t		reserved;

	int32_t		map_id;
	int32_t		color_space;
	int32_t		flags;
	int32_t		no_colors;

	int32_t		reserved1;
	int32_t		reserved2;
	int32_t		reserved3;
	int32_t		reserved4;

	COLOR_ENTRY	colors[1];
} COLOR_TAB;

/* vordefinierte Tabelle mit 256 Eintr„gen */
typedef struct
{
	int32_t		magic;				/* 'ctab' */
	int32_t		length;
	int32_t		format;
	int32_t		reserved;

	int32_t		map_id;
	int32_t		color_space;
	int32_t		flags;
	int32_t		no_colors;

	int32_t		reserved1;
	int32_t		reserved2;
	int32_t		reserved3;
	int32_t		reserved4;

	COLOR_ENTRY	colors[256];
} COLOR_TAB256;

typedef COLOR_TAB *CTAB_PTR;
typedef COLOR_TAB *CTAB_REF;


typedef void INVERSE_CTAB;
typedef INVERSE_CTAB *ITAB_REF;

#define	CBITMAP_MAGIC	0x6362746dL /* 'cbtm' */

typedef struct _gcbitmap GCBITMAP;
struct _gcbitmap
{
	int32_t		magic;			/* Strukturkennung 'cbtm' */
	int32_t		length;			/* Strukturl„nge */
	int32_t		format;			/* Strukturformat (0) */
	int32_t		reserved;		/* reserviert (0) */

	unsigned char *addr;		/* Adresse der Bitmap */
	int32_t		width;			/* Breite einer Zeile in Bytes */
	int32_t		bits;			/* Bittiefe */
	uint32_t	px_format;		/* Pixelformat */

	int32_t		xmin;			/* minimale diskrete x-Koordinate der Bitmap */
	int32_t		ymin;			/* minimale diskrete y-Koordinate der Bitmap */
	int32_t		xmax;			/* maximale diskrete x-Koordinate der Bitmap + 1 */
	int32_t		ymax;			/* maximale diskrete y-Koordinate der Bitmap + 1 */

	CTAB_REF	ctab;			/* Verweis auf die Farbtabelle oder 0L */
	ITAB_REF 	itab;			/* Verweis auf die inverse Farbtabelle oder 0L */
	int32_t		color_space;	/* Farbraum */
	int32_t		reserved1;		/* reserviert (0) */
};

/*----------------------------------------------------------------------------------------*/
/* Transfermodes for Bitmaps															  */
/*----------------------------------------------------------------------------------------*/

/* Moduskonstanten */
#define	T_NOT				4	/* Konstante fr Invertierung bei logischen Transfermodi */
#define	T_COLORIZE			16	/* Konstante fr Einf„rbung */

#define	T_LOGIC_MODE		0
#define	T_DRAW_MODE			32
#define	T_ARITH_MODE		64	/* Konstante fr Arithmetische Transfermodi */
#define	T_DITHER_MODE		128	/* Konstante frs Dithern */

/* logische Transfermodi */
#define	T_LOGIC_COPY		(T_LOGIC_MODE+0)
#define	T_LOGIC_OR			(T_LOGIC_MODE+1)
#define	T_LOGIC_XOR			(T_LOGIC_MODE+2)
#define	T_LOGIC_AND			(T_LOGIC_MODE+3)
#define	T_LOGIC_NOT_COPY	(T_LOGIC_MODE+4)
#define	T_LOGIC_NOT_OR		(T_LOGIC_MODE+5)
#define	T_LOGIC_NOT_XOR		(T_LOGIC_MODE+6)
#define	T_LOGIC_NOT_AND		(T_LOGIC_MODE+7)

/* Zeichenmodi */
#define	T_REPLACE			 (T_DRAW_MODE+0)
#define	T_TRANSPARENT		 (T_DRAW_MODE+1)
#define	T_HILITE			 (T_DRAW_MODE+2)
#define	T_REVERS_TRANSPARENT (T_DRAW_MODE+3)

/* arithmetische Transfermodi */
#define	T_BLEND				(T_ARITH_MODE+0)
#define	T_ADD				(T_ARITH_MODE+1)
#define	T_ADD_OVER			(T_ARITH_MODE+2)
#define	T_SUB				(T_ARITH_MODE+3)
#define	T_MAX				(T_ARITH_MODE+5)
#define	T_SUB_OVER			(T_ARITH_MODE+6)
#define	T_MIN				(T_ARITH_MODE+7)

#ifndef __RECT16
#define __RECT16
typedef struct			/* Rechteck fr 16-Bit-Koordinaten */
{
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
} RECT16;

typedef struct			/* Rechteck fr 32-Bit-Koordinaten */
{
	int32_t	x1;
	int32_t	y1;
	int32_t	x2;
	int32_t	y2;
} RECT32;
#endif


int32_t		v_color2nearest		(_WORD handle, int32_t color_space, COLOR_ENTRY *color, COLOR_ENTRY *nearest_color);
uint32_t	v_color2value		(_WORD handle, int32_t color_space, COLOR_ENTRY *color);
COLOR_TAB *	v_create_ctab		(_WORD handle, int32_t color_space, uint32_t px_format);
ITAB_REF	v_create_itab		(_WORD handle, COLOR_TAB *ctab, _WORD bits );
uint32_t	v_ctab_idx2value	(_WORD handle, _WORD __index );
_WORD		v_ctab_idx2vdi		(_WORD handle, _WORD __index);
_WORD		v_ctab_vdi2idx		(_WORD handle, _WORD vdi_index);
_WORD		v_delete_ctab		(_WORD handle, COLOR_TAB *ctab);
_WORD		v_delete_itab		(_WORD handle, ITAB_REF itab);
int32_t		v_get_ctab_id		(_WORD handle);
_WORD		v_get_outline		(_WORD handle, _WORD __index, _WORD x_offset, _WORD y_offset, _WORD *pts, char *flags, _WORD max_pts);
_WORD		v_open_bm		(_WORD base_handle, GCBITMAP *bitmap, _WORD color_flags, _WORD unit_flags, _WORD pixel_width, _WORD pixel_height);
_WORD		v_resize_bm		(_WORD handle, _WORD width, _WORD height, int32_t b_width, unsigned char *addr);
void		v_setrgb		(_WORD handle, _WORD type, _WORD r, _WORD g, _WORD b);
int32_t		v_value2color		(_WORD handle, uint32_t value, COLOR_ENTRY *color);
_WORD		vq_ctab			(_WORD handle, int32_t ctab_length, COLOR_TAB *ctab);
int32_t		vq_ctab_entry		(_WORD handle, _WORD __index, COLOR_ENTRY *color);
int32_t		vq_ctab_id		(_WORD handle);
_WORD		vq_dflt_ctab		(_WORD handle, int32_t ctab_length, COLOR_TAB *ctab);
int32_t		vq_hilite_color		(_WORD handle, COLOR_ENTRY *hilite_color);
_WORD		vq_margins		(_WORD handle, _WORD *top_margin, _WORD *bottom_margin, _WORD *left_margin, _WORD *right_margin, _WORD *hdpi, _WORD *vdpi);
int32_t		vq_max_color		(_WORD handle, COLOR_ENTRY *hilite_color);
int32_t		vq_min_color		(_WORD handle, COLOR_ENTRY *hilite_color);
int32_t		vq_prn_scaling		(_WORD handle);
int32_t		vq_px_format		(_WORD handle, uint32_t *px_format);
int32_t		vq_weight_color		(_WORD handle, COLOR_ENTRY *hilite_color);
int32_t		vqf_bg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vqf_fg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vql_bg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vql_fg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vqm_bg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vqm_fg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vqr_bg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vqr_fg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vqt_bg_color		(_WORD handle, COLOR_ENTRY *fg_color);
int32_t		vqt_fg_color		(_WORD handle, COLOR_ENTRY *fg_color);
void		vr_transfer_bits	(_WORD handle, GCBITMAP *src_bm, GCBITMAP *dst_bm, const _WORD *src_rect, const _WORD *dst_rect, _WORD mode);
_WORD		vs_ctab			(_WORD handle, COLOR_TAB *ctab);
_WORD		vs_ctab_entry		(_WORD handle, _WORD __index, int32_t color_space, COLOR_ENTRY *color);
_WORD		vs_dflt_ctab		(_WORD handle);
_WORD		vs_document_info	(_WORD vdi_handle, _WORD type, const char *s, _WORD wchar);
_WORD		vs_hilite_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *hilite_color);
_WORD		vs_max_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *min_color);
_WORD		vs_min_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *min_color);
_WORD		vs_weight_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *weight_color);
_WORD		vsf_bg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *bg_color);
_WORD		vsf_fg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *fg_color);
_WORD		vsl_bg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *bg_color);
_WORD		vsl_fg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *fg_color);
_WORD		vsm_bg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *bg_color);
_WORD		vsm_fg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *fg_color);
_WORD		vsr_bg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *bg_color);
_WORD		vsr_fg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *fg_color);
_WORD		vst_bg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *bg_color);
_WORD		vst_fg_color		(_WORD handle, int32_t color_space, COLOR_ENTRY *fg_color);


/*
 * udef_* support from gemlib.
 */
#define udef_vs_color vs_color
#define udef_vswr_mode vswr_mode
#define udef_vsf_color vsf_color
#define udef_vsf_interior vsf_interior
#define udef_vsf_perimeter vsf_perimeter
#define udef_vsf_xperimeter vsf_xperimeter
#define udef_vsf_style vsf_style
#define udef_vsf_udpat vsf_udpat
#define udef_vsl_color vsl_color
#define udef_vsl_ends vsl_ends
#define udef_vsl_type vsl_type
#define udef_vsl_udsty vsl_udsty
#define udef_vsl_width vsl_width
#define udef_vsm_color vsm_color
#define udef_vsm_height vsm_height
#define udef_vsm_type vsm_type
#define udef_vst_alignment vst_alignment
#define udef_vst_color vst_color
#define udef_vst_effects vst_effects
#define udef_vst_error vst_error
#define udef_vst_font vst_font
#define udef_vst_height vst_height
#define udef_vst_point vst_point
#define udef_vst_rotation vst_rotation
#define udef_vst_scratch vst_scratch
#define udef_v_clrwk v_clrwk
#define udef_v_clsvwk v_clsvwk
#define udef_v_clswk v_clswk
#define udef_v_flushcache v_flushcache
#define udef_v_loadcache v_loadcache
#define udef_v_opnvwk v_opnvwk
#define udef_v_opnwk v_opnwk
#define udef_v_set_app_buff v_set_app_buff
#define udef_v_updwk v_updwk
#define udef_vs_clip vs_clip
#define udef_vs_clip_pxy vs_clip_pxy
#define udef_vs_clip_off vs_clip_off
#define udef_vst_load_fonts vst_load_fonts
#define udef_vst_unload_fonts vst_unload_fonts
#define udef_v_clear_disp_list v_clear_disp_list
#define udef_v_copies v_copies
#define udef_v_dspcur v_dspcur
#define udef_v_form_adv v_form_adv
#define udef_v_hardcopy v_hardcopy
#define udef_v_orient v_orient
#define udef_v_output_window v_output_window
#define udef_v_page_size v_page_size
#define udef_v_rmcur v_rmcur
#define udef_v_trays v_trays
#define udef_vq_calibrate vq_calibrate
#define udef_vq_page_name vq_page_name
#define udef_vq_scan vq_scan
#define udef_vq_tabstatus vq_tabstatus
#define udef_vq_tray_names vq_tray_names
#define udef_vs_calibrate vs_calibrate
#define udef_vs_palette vs_palette
#define udef_v_sound v_sound
#define udef_vs_mute vs_mute
#define udef_vq_tdimensions vq_tdimensions
#define udef_vt_alignment vt_alignment
#define udef_vt_axis vt_axis
#define udef_vt_origin vt_origin
#define udef_vt_resolution vt_resolution
#define udef_v_meta_extents v_meta_extents
#define udef_v_write_meta v_write_meta
#define udef_vm_coords vm_coords
#define udef_vm_pagesize vm_pagesize
#define udef_vm_filename vm_filename
#define udef_vsc_expose vsc_expose
#define udef_vsp_film vsp_film
#define udef_vqp_filmname vqp_filmname
#define udef_v_offset v_offset
#define udef_v_fontinit v_fontinit
#define udef_v_escape2000 v_escape2000
#define udef_v_alpha_text v_alpha_text
#define udef_v_alpha_text16n v_alpha_text16n
#define udef_v_curdown v_curdown
#define udef_v_curhome v_curhome
#define udef_v_curleft v_curleft
#define udef_v_curright v_curright
#define udef_v_curtext v_curtext
#define udef_v_curtext16n v_curtext16n
#define udef_v_curup v_curup
#define udef_v_eeol v_eeol
#define udef_v_eeos v_eeos
#define udef_v_enter_cur v_enter_cur
#define udef_v_exit_cur v_exit_cur
#define udef_v_rvoff v_rvoff
#define udef_v_rvon v_rvon
#define udef_vq_chcells vq_chcells
#define udef_vq_curaddress vq_curaddress
#define udef_vs_curaddress vs_curaddress
#define udef_v_bit_image v_bit_image
#define udef_v_curaddress udef_vs_curaddress
#define udef_vq_cellarray vq_cellarray
#define udef_vq_color vq_color
#define udef_vq_extnd vq_extnd
#define udef_vqf_attributes vqf_attributes
#define udef_vqin_mode vqin_mode
#define udef_vql_attributes vql_attributes
#define udef_vqm_attributes vqm_attributes
#define udef_vqt_attributes vqt_attributes
#define udef_vqt_cachesize vqt_cachesize
#define udef_vqt_extent vqt_extent
#define udef_vqt_extent16 vqt_extent16
#define udef_vqt_extent16n vqt_extent16n
#define udef_vqt_fontinfo vqt_fontinfo
#define udef_vqt_get_table vqt_get_table
#define udef_vqt_name vqt_name
#define udef_vqt_width vqt_width
#define udef_vq_gdos vq_gdos
#define udef_vq_vgdos vq_vgdos
#define udef_v_hide_c v_hide_c
#define udef_v_show_c v_show_c
#define udef_vex_butv vex_butv
#define udef_vex_curv vex_curv
#define udef_vex_motv vex_motv
#define udef_vex_wheelv vex_wheelv
#define udef_vex_timv vex_timv
#define udef_vq_key_s vq_key_s
#define udef_vq_mouse vq_mouse
#define udef_vrq_choice vrq_choice
#define udef_vrq_locator vrq_locator
#define udef_vrq_string vrq_string
#define udef_vrq_valuator vrq_valuator
#define udef_vsc_form vsc_form
#define udef_vsin_mode vsin_mode
#define udef_vsm_choice vsm_choice
#define udef_vsm_locator vsm_locator
#define udef_vsm_string vsm_string
#define udef_vsm_valuator vsm_valuator
#define udef_v_arc v_arc
#define udef_v_bar v_bar
#define udef_v_cellarray v_cellarray
#define udef_v_circle v_circle
#define udef_v_contourfill v_contourfill
#define udef_v_ellarc v_ellarc
#define udef_v_ellipse v_ellipse
#define udef_v_ellpie v_ellpie
#define udef_v_fillarea v_fillarea
#define udef_v_gtext v_gtext
#define udef_v_gtext16 v_gtext16
#define udef_v_gtext16n v_gtext16n
#define udef_v_justified v_justified
#define udef_v_justified16n v_justified16n
#define udef_v_pieslice v_pieslice
#define udef_v_pline v_pline
#define udef_v_pmarker v_pmarker
#define udef_v_rbox v_rbox
#define udef_v_rfbox v_rfbox
#define udef_vr_recfl vr_recfl
#define udef_v_get_pixel v_get_pixel
#define udef_vr_trnfm vr_trnfm
#define udef_vro_cpyfm vro_cpyfm
#define udef_vrt_cpyfm vrt_cpyfm

/*
 * Some useful extensions.
 */

void  vdi_array2str (const _WORD *src, char  *des, _WORD len);
_WORD vdi_str2array (const char  *src, _WORD *des);
_WORD vdi_str2arrayn (const char  *src, _WORD *des, _WORD len);
_WORD vdi_wstrlen   (const _WORD *wstr);

EXTERN_C_END

#endif /* __PORTVDI_H__ */
