#define WFRVERSION 2

#define N_OBJS 16

#define TOP_WINOBJS	(NAME+CLOSER+FULLER+BACKDROP+ICONIFIER)
#define RGT_WINOBJS (UPARROW+DNARROW+VSLIDE)
#define BOT_WINOBJS (LFARROW+RTARROW+HSLIDE)

/* WINDOW-Structure for MagiC-Kernel */

typedef struct {
  int16_t   state;
  int16_t   attr;
  void      *own;         /* (APPL *)                     */
  int16_t   kind;         /* von wind_create()            */
  char      *name;        /* Zeiger auf Titelzeile        */
  char      *info;        /* Zeiger auf Infozeile         */
  GRECT     curr;
  GRECT     prev;
  GRECT     full;
  GRECT     work;
  GRECT     overall;      /* Umriss                       */
  GRECT     unic;
  GRECT     min;          /* Minimale Groesse             */
  int16_t   oldheight;    /* alte Hoehe vor Shading       */
  int16_t   hslide;       /* horizontale Schieberposition */
  int16_t   vslide;       /* vertikale Schieberposition   */
  int16_t   hslsize;      /* horizontale Schiebergroesse  */
  int16_t   vslsize;      /* vertikale Schiebergroesse    */
  void      *wg;          /* Rechteckliste                */
  void      *nextwg;      /* naechstes Rechteck der Liste */
  int16_t   whdl;
  OBJECT    tree[N_OBJS];
  int16_t   is_sizer;
  int16_t   is_info;
  int16_t   is_rgtobjects;
  int16_t   is_botobjects;
  TEDINFO   ted_name;
  TEDINFO   ted_info;
} WININFO;


/* Bits von state */

#define OPENED 1
#define COVERED 2
#define ACTIVE 4
#define LOCKED 8
#define ICONIFIED 32
#define SHADED 64

typedef struct {
  int16_t   flags;
  int16_t   h_inw;
  void      *finfo_inw;
} WINFRAME_SETTINGS;

/* Bits von flags */

#define NO_BDROP 1

typedef struct {
  int16_t   version;                  /* Version number of structure */
  int32_t   wsizeof;                  /* Size of the WINDOW-structure */
  int16_t   whshade;                  /* Height of a shaded window */
  void      (*wbm_create)( WININFO *w );
  void      (*wbm_skind) ( WININFO *w );
  void      (*wbm_ssize) ( WININFO *w );
  void      (*wbm_sslid) ( WININFO *w, int16_t vertical );
  void      (*wbm_sstr)  ( WININFO *w );
  void      (*wbm_sattr) ( WININFO *w, int16_t chbits );
  void      (*wbm_calc)  ( int16_t kind, int16_t *fg );
  int16_t   (*wbm_obfind)( WININFO *w, int16_t x, int16_t y );
} WINFRAME_HANDLER;
