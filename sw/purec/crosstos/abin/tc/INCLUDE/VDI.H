/*      VDI.H

        GEM VDI Definitions

        Copyright (c) Borland International 1990
        All Rights Reserved.
*/


#if  !defined( __VDI__ )
#define __VDI__


/****** Control definitions *********************************************/

void    v_opnwk( int *work_in,  int *handle, int *work_out);
void    v_clswk( int handle );
void    v_opnvwk( int *work_in, int *handle, int *work_out);
void    v_clsvwk( int handle );
void    v_clrwk( int handle );
void    v_updwk( int handle );
int             vst_load_fonts( int handle, int select );
void    vst_unload_fonts( int handle, int select );
void    vs_clip( int handle, int clip_flag, int *pxyarray );


/****** Output definitions **********************************************/

void    v_pline( int handle, int count, int *pxyarray );
void    v_pmarker( int handle, int count, int *pxyarray );
void    v_gtext( int handle, int x, int y, char *string );
void    v_fillarea( int handle, int count, int *pxyarray );
void    v_cellarray( int handle, int *pxyarray, int row_length,
                     int el_used, int num_rows, int wrt_mode,
                     int *colarray );
void    v_contourfill( int handle, int x, int y, int index );
void    vr_recfl( int handle, int *pxyarray );
void    v_bar( int handle, int *pxyarray );
void    v_arc( int handle, int x, int y, int radius,
               int begang, int endang );
void    v_pieslice( int handle, int x, int y, int radius,
                    int begang, int endang );
void    v_circle( int handle, int x, int y, int radius );
void    v_ellarc( int handle, int x, int y, int xradius,
                  int yradius, int begang, int endang );
void    v_ellpie( int handle, int x, int y, int xradius,
                  int yradius, int begang, int endang );
void    v_ellipse( int handle, int x, int y, int xradius,
                   int yradius  );
void    v_rbox  ( int handle, int *pxyarray );
void    v_rfbox ( int handle, int *pxyarray );
void    v_justified( int handle,int x, int y, char *string,
                     int length, int word_space,
                     int char_space );


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

#define ALL_WHITE        0
#define S_AND_D          1
#define S_AND_NOTD       2
#define S_ONLY           3
#define NOTS_AND_D       4
#define D_ONLY           5
#define S_XOR_D          6
#define S_OR_D           7
#define NOT_SORD         8
#define NOT_SXORD        9
#define D_INVERT        10
#define NOT_D           11
#define S_OR_NOTD       12
#define NOTS_OR_D       13
#define NOT_SANDD       14
#define ALL_BLACK       15


/* linetypes */

#define SOLID           1
#define LONGDASH        2
#define DOT             3
#define DASHDOT         4
#define DASH            5
#define DASH2DOT        6
#define USERLINE        7

#define SQUARE          0
#define ARROWED         1
#define ROUND           2

int     vswr_mode( int handle, int mode );
void    vs_color( int handle, int index, int *rgb_in );
int     vsl_type( int handle, int style );
void    vsl_udsty( int handle, int pattern );
int     vsl_width( int handle, int width );
int     vsl_color( int handle, int color_index );
void    vsl_ends( int handle, int beg_style, int end_style );
int     vsm_type( int handle, int symbol );
int     vsm_height( int handle, int height );
int     vsm_color( int handle, int color_index );
void    vst_height( int handle, int height, int *char_width,
                    int *char_height, int *cell_width,
                    int *cell_height );
int     vst_point( int handle, int point, int *char_width,
                    int *char_height, int *cell_width,
                    int *cell_height );
int     vst_rotation( int handle, int angle );
int     vst_font( int handle, int font );
int     vst_color( int handle, int color_index );
int     vst_effects( int handle, int effect );
void    vst_alignment( int handle, int hor_in, int vert_in,
                       int *hor_out, int *vert_out );
int     vsf_interior( int handle, int style );
int     vsf_style( int handle, int style_index );
int     vsf_color( int handle, int color_index );
int     vsf_perimeter( int handle, int per_vis );
void    vsf_udpat( int handle, int *pfill_pat, int planes );


/****** Raster definitions *********************************************/

typedef struct
{
        void            *fd_addr;
        int             fd_w;
        int             fd_h;
        int             fd_wdwidth;
        int             fd_stand;
        int             fd_nplanes;
        int             fd_r1;
        int             fd_r2;
        int             fd_r3;
} MFDB;

void    vro_cpyfm( int handle, int vr_mode, int *pxyarray,
                   MFDB *psrcMFDB, MFDB *pdesMFDB );
void    vrt_cpyfm( int handle, int vr_mode, int *pxyarray,
                   MFDB *psrcMFDB, MFDB *pdesMFDB,
                   int *color_index );
void    vr_trnfm( int handle, MFDB *psrcMFDB, MFDB *pdesMFDB );
void    v_get_pixel( int handle, int x, int y, int *pel,
                     int *index );


/****** Input definitions **********************************************/

void    vsin_mode( int handle, int dev_type, int mode );
void    vrq_locator( int handle, int x, int y, int *xout,
                     int *yout, int *term );
int     vsm_locator( int handle, int x, int y, int *xout,
                     int *yout, int *term );
void    vrq_valuator( int handle, int valuator_in,
                      int *valuator_out, int *terminator );
void    vsm_valuator( int handle, int val_in, int *val_out,
                      int *term, int *status );
void    vrq_choice( int handle, int ch_in, int *ch_out );
int     vsm_choice( int handle, int *choice );
void    vrq_string( int handle, int max_length, int echo_mode,
                    int *echo_xy, char *string );
int     vsm_string( int handle, int max_length, int echo_mode,
                    int *echo_xy, char *string );
void    vsc_form( int handle, int *pcur_form );
void    vex_timv( int handle, int (*tim_addr)(), int (**otim_addr)(),
                  int *tim_conv );
void    v_show_c( int handle, int reset );
void    v_hide_c( int handle );
void    vq_mouse( int handle, int *pstatus, int *x, int *y );
void    vex_butv( int handle, int (*pusrcode)(), int (**psavcode)() );
void    vex_motv( int handle, int (*pusrcode)(), int (**psavcode)() );
void    vex_curv( int handle, int (*pusrcode)(), int (**psavcode)() );
void    vq_key_s( int handle, int *pstatus );


/****** Inquire definitions *******************************************/

void    vq_extnd( int handle, int owflag, int *work_out );
int     vq_color( int handle, int color_index,
                  int set_flag, int *rgb );
void    vql_attributes( int handle, int *attrib );
void    vqm_attributes( int handle, int *attrib );
void    vqf_attributes( int handle, int *attrib );
void    vqt_attributes( int handle, int *attrib );
void    vqt_extent( int handle, char *string, int *extent );
int     vqt_width( int handle, char character,
                   int *cell_width, int *left_delta,
                   int *right_delta );
int     vqt_name( int handle, int element_num, char *name );
void    vq_cellarray( int handle, int *pxyarray,
                      int row_length, int num_rows,
                      int *el_used, int *rows_used,
                      int *status, int *colarray );
void    vqin_mode( int handle, int dew_type, int *input_mode );
void    vqt_fontinfo( int handle, int *minADE, int *maxADE,
                      int *distances, int *maxwidth,
                      int *effects );


/****** Escape definitions *********************************************/

void    vq_chcells( int handle, int *rows, int *columns );
void    v_exit_cur( int handle );
void    v_enter_cur( int handle );
void    v_curup( int handle );
void    v_curdown( int handle );
void    v_curright( int handle );
void    v_curleft( int handle );
void    v_curhome( int handle );
void    v_eeos( int handle );
void    v_eeol( int handle );
void    vs_curaddress( int handle, int row, int column );
void    v_curtext( int handle, char *string );
void    v_rvon( int handle );
void    v_rvoff( int handle );
void    vq_curaddress( int handle, int *row, int *column );
int     vq_tabstatus( int handle );
void    v_hardcopy( int handle );
void    v_dspcur( int handle, int x, int y );
void    v_rmcur( int handle );
void    v_form_adv( int handle );
void    v_output_window( int handle, int *xyarray );
void    v_clear_disp_list( int handle );
void    v_bit_image( int handle, const char *filename,
                     int aspect, int x_scale, int y_scale,
                     int h_align, int v_align, int *xyarray );
void    vq_scan( int handle, int *g_slice, int *g_page,
                 int *a_slice, int *a_page, int *div_fac);
void    v_alpha_text( int handle, char *string );
void    vs_palette( int handle, int palette );
void    vqp_films( int handle, char *film_names );
void    vqp_state( int handle, int *port, char *film_name,
                   int *lightness, int *interlace,
                   int *planes, int *indexes );
void    vsp_state( int handle, int port, int film_num,
                   int lightness, int interlace, int planes,
                   int *indexes );
void    vsp_save( int handle );
void    vsp_message( int handle );
int     vqp_error( int handle );
void    v_meta_extents( int handle, int min_x, int min_y,
                        int max_x, int max_y );
void    v_write_meta( int handle,
                      int num_intin, int *intin,
                      int num_ptsin, int *ptsin );
void    vm_coords( int handle, int llx, int lly, int urx, int ury );
void    vm_filename( int handle, const char *filename );
void    vm_pagesize( int handle, int pgwidth, int pdheight );
void    v_offset( int handle, int offset );
void    v_fontinit( int handle, int fh_high, int fh_low );
void    v_escape2000( int times );

void    vt_resolution( int handle, int xres, int yres,
                       int *xset, int *yset );
void    vt_axis( int handle, int xres, int yres,
                 int *xset, int *yset );
void    vt_origin( int handle, int xorigin, int yorigin );
void    vq_tdimensions( int handle, int *xdimension, int *ydimension );
void    vt_alignment( int handle, int dx, int dy );
void    vsp_film( int handle, int index, int lightness );
void    vsc_expose( int handle, int state );

int     vq_gdos( void );


#endif

/***********************************************************************/
