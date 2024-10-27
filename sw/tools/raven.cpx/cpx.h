/*
* File recreated using many available sources found in dusty internet.
* 
*
*/
#ifndef _CPX_H
#define _CPX_H

/* -------------------------------------------------------------------- */
/* CPX Header Structure. This is 'tacked' onto the front of Each CPX 	*/
/* with a special program.						*/
/* -------------------------------------------------------------------- */

typedef struct Cpxhead
{
    UWORD   magic;
    struct
    {
        unsigned reserved : 13;
        unsigned ram_resident : 1;
        unsigned boot_init : 1;
        unsigned set_only  : 1;
    } flags;
    LONG    cpx_id;
    UWORD   cpx_version;
    char    i_text[14];
    UWORD   icon[48];
    struct
    {
    	unsigned i_color :4;
    	unsigned reserved : 4;
    	unsigned i_char : 8;
    } i_info;
    char    text[18];
    struct
    {
    	unsigned c_board : 4;
    	unsigned c_text : 4;
    	unsigned pattern : 4;
    	unsigned c_back : 4;
    } t_info;
    char    buffer[64];
    char    reserved[306];
}   CPXHEAD;

typedef struct Prghead
{
	int	magic;
	long	tsize,
		dsize,
		bsize,
		ssize;
	int	fill[5];
} Prghead;

/*
 * Internal data structure for storing cpx headers in a linked list.
 * Data structure manipulation is in cpxhandl.c
 * We INCLUDE the header so that it can be sized and changed without
 * too much hassle. Note that there are some additional requirements
 * for the nodes than just the header information.
 */
typedef struct cpxnode
{
   char      fname[ 14 ];	 /* filename...   */
   int	     vacant;		 /* 1 = not vacant*/
   int	     SkipRshFix;	 /* Always 0 if non-resident. For residents
   				  * 0 first time CPXinit is called and then 
   				  * set to 1 so it will skip it thereafter
   				  */
   long      *baseptr;	         /* Basepage ptr
   				  * for resident cpxs
   				  */
   struct    cpxnode   *next;	 /* Next cpxnode      */
   CPXHEAD   cpxhead;		 /* cpx header struct */
				 /* NOTE: THESE TWO FIELDS MUST REMAIN
				  * CONTIGUOUS FOR ALL TIME!!!
				  */
   Prghead   prghead;		 /* program header of CPX */
   
} CPXNODE;
 
typedef struct mrets
{
    WORD x;
    WORD y;
    WORD buttons;
    WORD kstate;
} MRETS; 
 
/*  CPX DATA STRUCTURES
 *==========================================================================
 *  XCPB structure is passed TO the CPX
 *  CPXINFO structure pointer is returned FROM the CPX
 *
 *  xcpb structure is initialized in XCONTROL.C
 */


typedef struct Xcpb
{
     short 	handle;
     short 	booting;
     short 	reserved;  
     short 	SkipRshFix;

     CPXNODE * 	cdecl 	(*Get_Head_Node)(void );

     short   	cdecl 	(*Save_Header)( CPXNODE *ptr );
     
     void 		cdecl 	(*rsh_fix)( int num_obs, int num_frstr, int num_frimg,
							int num_tree, OBJECT *rs_object, 
                       	    TEDINFO *rs_tedinfo, BYTE *rs_strings[],
                       	    ICONBLK *rs_iconblk, BITBLK *rs_bitblk,
                       	    long *rs_frstr, long *rs_frimg, long *rs_trindex,
                       	    struct foobar *rs_imdope );
                       
     void 		cdecl 	(*rsh_obfix)( OBJECT *tree, int curob );

     short 		cdecl 	(*Popup)( char *items[], int num_items, int default_item,
							int font_size, GRECT *button, GRECT *world );

     void 		cdecl 	(*Sl_size)( OBJECT *tree, int base, int slider, int num_items,
                            int visible, int direction, int min_size );
                       
     void 		cdecl 	(*Sl_x)( OBJECT *tree, int base, int slider, int value,
							int num_min, int num_max, void (*foo)() );
                    
     void 		cdecl 	(*Sl_y)( OBJECT *tree, int base, int slider, int value,
							int num_min, int num_max, void (*foo)() );
                    
     void 		cdecl 	(*Sl_arrow)( OBJECT *tree, int base, int slider, int obj,
							int inc, int min, int max, int *numvar,
                            int direction, void (*foo)() );
                        
     void 		cdecl 	(*Sl_dragx)( OBJECT *tree, int base, int slider, int min,
                            int max, int *numvar, void (*foo)() );
                        
     void 		cdecl 	(*Sl_dragy)( OBJECT *tree, int base, int slider, int min,
                            int max, int *numvar, void (*foo)() );
     
     WORD 		cdecl 	(*Xform_do)( OBJECT *tree, WORD start_field, WORD puntmsg[] );
     
     GRECT 	* 	cdecl 	(*GetFirstRect)( GRECT *prect );
     GRECT 	* 	cdecl 	(*GetNextRect)( void );
     
     void 		cdecl 	(*Set_Evnt_Mask)( int mask, MOBLK *m1, MOBLK *m2, long time );

     WORD 		cdecl 	(*XGen_Alert)( int id );

     WORD 		cdecl 	(*CPX_Save)( void *ptr, long num );
     void   * 	cdecl 	(*Get_Buffer)( void );

     int    	cdecl	(*get_cookie)( long cookie, long *p_value );

     int     			Country_Code;
     
     void  		cdecl  	(*MFsave)( WORD saveit, MFORM *mf );          
} XCPB;



typedef struct Cpxinfo
{
     WORD	(*cpx_call)( GRECT *work );
     
     void	(*cpx_draw)( GRECT *clip );
     void	(*cpx_wmove)( GRECT *work );
     
     void	(*cpx_timer)( int *quit );
     void	(*cpx_key)( int kstate, int key, int *quit );
     void	(*cpx_button)( MRETS *mrets, int nclicks, int *quit );
     void	(*cpx_m1)( MRETS *mrets, int *quit );
     void	(*cpx_m2)( MRETS *mrets, int *quit );
     WORD	(*cpx_hook)( int event, int *msg, MRETS *mrets,
                                   int *key, int *nclicks );

     void  	(*cpx_close)( WORD flag );
} CPXINFO;



#define VERTICAL	0
#define HORIZONTAL	1

#define SAVE_DEFAULTS	0
#define MEM_ERR		1
#define FILE_ERR	2
#define FILE_NOT_FOUND	3

#define MFSAVE 1
#define MFRESTORE 0
 
#endif