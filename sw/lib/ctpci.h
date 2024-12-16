

/* Setscreen defines */

#define REZ_MI          0x4D49
#define REZ_VN          0x564E
#define REZ_VR          0x5652

#define CMD_GETMODE     0       /* REZ_MI, REZ_VN:0x0100 */
#define CMD_SETMODE     1
#define CMD_GETINFO     2
#define CMD_ALLOCPAGE   3 
#define CMD_FREEPAGE    4
#define CMD_FLIPPAGE    5
#define CMD_ALLOCMEM    6
#define CMD_FREEMEM     7
#define CMD_SETADR      8
#define CMD_ENUMMODES   9
#define CMD_TESTMODE    10      /* REZ_VN:0x0100 */
#define CMD_COPYPAGE    11
#define CMD_FILLMEM     12      /* REZ_VN:0x0101 */
#define CMD_COPYMEM     13
#define CMD_TEXTUREMEM  14
#define CMD_GETVERSION  15
#define CMD_LINEMEM     16
#define CMD_CLIPMEM     17
#define CMD_SYNCMEM     18
#define CMD_BLANK       19

#define CMD_RESET       20      /* REZ_VR:0x0200 */


/* ------------------------------------------
 * 4.24.22 SCREENINFO
 * ----------------------------------------*/

/* scrFlags */
 #define SCRINFO_OK         1

/* scrClut */
 #define NO_CLUT            0
 #define HARD_CLUT          1
 #define SOFT_CLUT          2

/* scrFormat */
 #define INTERLEAVE_PLANES  0
 #define STANDARD_PLANES    1
 #define PACKEDPIX_PLANES   2

/* bitFlags */
 #define STANDARD_BITS      1
 #define FALCON_BITS        2
 #define INTEL_BITS         8

typedef struct screeninfo
{
  int32_t size;        /* Size of structur           */
  int32_t devID;       /* Device ID number           */
  int8_t  name[64];    /* Friendly name of Screen    */
  int32_t scrFlags;    /* Some flags                 */
  int32_t frameadr;    /* Adress of framebuffer      */
  int32_t scrHeight;   /* Visible X res              */
  int32_t scrWidth;    /* Visible Y res              */
  int32_t virtHeight;  /* Virtual X res              */
  int32_t virtWidth;   /* Virtual Y res              */
  int32_t scrPlanes;   /* color Planes               */
  int32_t scrColors;   /* # of colors                */
  int32_t lineWrap;    /* # of bytes to next line    */
  int32_t planeWarp;   /* # of bytes to next plane   */
  int32_t scrFormat;   /* Screen format              */
  int32_t scrClut;     /* Type of clut               */
  int32_t redBits;     /* Mask of Red Bits           */
  int32_t greenBits;   /* Mask of Green Bits         */
  int32_t blueBits;    /* Mask of Blue Bits          */
  int32_t alphaBits;   /* Mask of Alpha Bits         */
  int32_t genlockBits; /* Mask of Genlock Bits       */
  int32_t unusedBits;  /* Mask of unused Bits        */
  int32_t bitFlags;    /* Bits organisation flags    */
  int32_t maxmem;      /* Max. memory in this mode   */
  int32_t pagemem;     /* Needed memory for one page */
  int32_t max_x;       /* Max. possible width        */
  int32_t max_y;       /* Max. possible heigth       */
}SCREENINFO;


/* ------------------------------------------
 * 4.24.24 SCRFILLMEMBLK
 * ----------------------------------------*/

/* operations */
 #define BLK_CLEAR        0
 #define BLK_AND          1
 #define BLK_ANDREVERSE   2
 #define BLK_COPY         3
 #define BLK_ANDINVERTED  4
 #define BLK_NOOP         5
 #define BLK_XOR          6
 #define BLK_OR           7
 #define BLK_XNOR         8
 #define BLK_EQUIV        9
 #define BLK_INVERT       10
 #define BLK_ORREVERSE    11
 #define BLK_COPYINVERTED 12
 #define BLK_ORINVERTED   13
 #define BLK_NAND         14
 #define BLK_SET          15

typedef struct _scrsetblk
{
  int32_t size;                  /* size of structure           */
  int32_t blk_status;            /* status bits of blk          */
  int32_t blk_op;                /* mode operation              */
  int32_t blk_color;             /* background fill color       */
  int32_t blk_x;                 /* x pos in total screen       */
  int32_t blk_y;                 /* y pos in total screen       */
  int32_t blk_w;                 /* width                       */
  int32_t blk_h;                 /* height                      */
  int32_t blk_unused;
} SCRFILLMEMBLK;


/* ------------------------------------------
 * 4.24.25 SCRLINEMEMBLK
 * ----------------------------------------*/

typedef struct _scrlineblk
{
  int32_t size;        /* size of structure            */
  int32_t blk_status;  /* status bits of blk           */
  int32_t blk_fgcolor; /* foreground fill color        */
  int32_t blk_bgcolor; /* background fill color        */
  int32_t blk_x1;      /* x1 pos dest in total screen  */
  int32_t blk_y1;      /* y1 pos dest in total screen  */
  int32_t blk_x2;      /* x2 pos dest in total screen  */
  int32_t blk_y2;      /* y2 pos dest in total screen  */
  int32_t blk_op;      /* mode operation               */
  int32_t blk_pattern; /* pattern (-1: solid line)     */
} SCRLINEMEMBLK;


/* ------------------------------------------
 * 4.24.26 SCRMEMBLK
 * ----------------------------------------*/

 #define BLK_ERR      0
 #define BLK_OK       1
 #define BLK_CLEARED  2

typedef struct _scrblk
{
  int32_t size;              /* Size of strukture                  */
  int32_t blk_status;        /* Status bits of blk                 */
  int32_t blk_start;         /* Start adress                       */
  int32_t blk_len;           /* Length of memblk                   */
  int32_t blk_x;             /* X pos in total screen              */
  int32_t blk_y;             /* Y pos in total screen              */
  int32_t blk_w;             /* Width                              */
  int32_t blk_h;             /* Height                             */
  int32_t blk_wrap;          /* Width in bytes, from: 2000-01-13   */
} SCRMEMBLK;


/* ------------------------------------------
 * 4.24.27 SCRTEXTUREMEMBLK
 * ----------------------------------------*/

typedef struct _scrtextureblk
{
  long size;                /* size of structure             */
  long blk_status;          /* status bits of blk            */
  long blk_src_x;           /* x pos source                  */
  long blk_src_y;           /* y pos source                  */
  long blk_dst_x;           /* x pos dest in total screen    */
  long blk_dst_y;           /* y pos dest in total screen    */
  long blk_w;               /* width                         */
  long blk_h;               /* height                        */
  long blk_op;              /* mode operation                */
  long blk_src_tex;         /* source texture address        */
  long blk_w_tex;           /* width texture                 */
  long blk_h_tex;           /* height texture                */
}SCRTEXTUREMEMBLK;

