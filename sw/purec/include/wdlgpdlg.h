#ifndef __PDLG_H__
#define __PDLG_H__

/*
 * exported printer dialog functions
 */

#include <portab.h>
#include <wdlgwdlg.h>

#ifndef __PRNDIALOG
#define __PRNDIALOG
typedef struct _prn_dialog { int dummy; } PRN_DIALOG;
#endif
typedef struct _pdlg_sub PDLG_SUB;
typedef struct _media_type MEDIA_TYPE;
typedef struct _media_size MEDIA_SIZE;
typedef struct _prn_tray PRN_TRAY;
typedef struct _prn_mode PRN_MODE;
typedef struct _prn_entry PRN_ENTRY;
typedef struct _dither_mode DITHER_MODE;
typedef struct _prn_settings PRN_SETTINGS;
typedef struct _drv_entry DRV_ENTRY;
typedef struct _drv_info DRV_INFO;

/** parameters for PDLG_HNDL callback functions
 */
struct PDLG_HNDL_args
{
	PRN_SETTINGS *settings;
	PDLG_SUB *sub;
	_WORD exit_obj;
};

typedef long __CDECL (*PDLG_INIT)(PRN_SETTINGS *settings, PDLG_SUB *sub);
typedef long __CDECL (*PDLG_HNDL)(struct PDLG_HNDL_args args);
typedef long __CDECL (*PDLG_RESET)(PRN_SETTINGS *settings, PDLG_SUB *sub);
typedef long __CDECL (*PRN_SWITCH)(DRV_ENTRY *drivers, PRN_SETTINGS *settings, PRN_ENTRY *old_printer, PRN_ENTRY *new_printer);

struct _drv_entry {
	DRV_ENTRY   *next;
};


/* Description of a paper-type/print-medium */
struct _media_type {
	MEDIA_TYPE   *next;
	_LONG        type_id;         /* ID of the paper format */
	char         name[32];        /* Name of paper format */
};


/* Description of a paper format */
struct _media_size {
	MEDIA_SIZE   *next;
	_LONG        size_id;         /* ID of the paper format */
	char         name[32];        /* Name of paper format */
};


/* Description of a printer */
struct _prn_mode {
	PRN_MODE *next;                  /* Pointer to the next print mode */
	_LONG        mode_id;            /* Mode-ID (index within the file) */
	_WORD        hdpi;               /* Horizontal resolution in dpi */
	_WORD        vdpi;               /* Vertical resolution in dpi */
	_LONG        mode_capabilities;  /* Mode capabilities */
	
	_LONG        color_capabilities; /* Available color modes */
	_LONG        dither_flags;       /* Flags that specify whether the
	                                    corresponding color mode is available
	                                    with or without dithering */
	MEDIA_TYPE  *paper_types;        /* Suitable paper types */
	_LONG        reserved;
	char         name[32];           /* Mode name */
};


struct _prn_tray {
	PRN_TRAY     *next;
	_LONG        tray_id;         /* Number of feed or eject tray */
	char         name[32];        /* Name of tray */
};


struct _dither_mode {
	DITHER_MODE  *next;
	_LONG        length;          /* Structure length */
	_LONG        format;          /* Data format */
	_LONG        reserved;        /* Reserved */
	_LONG        dither_id;       /* ID */
	_LONG        color_modes;     /* Supported color depths */
	_LONG        reserved1;       /* Reserved */
	_LONG        reserved2;       /* Reserved */
	char         name[32];        /* Name of dither mode */
};


/* Sub-dialog for device settings */
struct _pdlg_sub {
	/*   0 */ PDLG_SUB     *next;           /* Pointer to the successor in the list */
	/*   4 */ _LONG        length;          /* Structure length */
	/*   8 */ _LONG        format;          /* Data format */
	/*  12 */ _LONG        reserved;        /* Reserved */
	
	/*  16 */ DRV_ENTRY    *drivers;        /* Only for internal dialogs */
	/*  20 */ _WORD        option_flags;    /* Flags, inc. PDLG_PRINTING, PDLG_PREFS */
	/*  22 */ _WORD        sub_id;          /* ID for sub-dialogs, is entered for global sub-dialogs of pdlg_add() */
	/*  24 */ DIALOG       *dialog;         /* Pointer to structure of the window dialog or 0L */
	
	/*  28 */ OBJECT       *tree;           /* Pointer to the assembled object tree */
	/*  32 */ _WORD        index_offset;    /* Index offset of the sub-dialog */
	/*  34 */ _WORD        reserved1;       /* Reserved */
	/*  36 */ _LONG        reserved2;       /* Reserved */
	/*  40 */ _LONG        reserved3;       /* Reserved */
	/*  44 */ _LONG        reserved4;       /* Reserved */
	
	/*  48 */ PDLG_INIT    init_dlg;        /* Initialising function */
	/*  52 */ PDLG_HNDL    do_dlg;          /* Servicing function */
	/*  56 */ PDLG_RESET   reset_dlg;       /* Reset function */
	/*  60 */ _LONG        reserved5;       /* Reserved */
	
	/*  64 */ OBJECT       *sub_icon;       /* Pointer to the icon for the listbox */
	/*  68 */ OBJECT       *sub_tree;       /* Pointer to the object tree of the sub-dialog */
	/*  72 */ _LONG        reserved6;       /* Reserved */
	/*  76 */ _LONG        reserved7;       /* Reserved */
	
	/*  80 */ _LONG        private1;        /* Dialog-specific information */
	/*  84 */ _LONG        private2;
	/*  88 */ _LONG        private3;
	/*  92 */ _LONG        private4;
	/*  96 */ 
};

#define PDLG_CHG_SUB	0x80000000L
#define PDLG_IS_BUTTON	0x40000000L

#define PDLG_PREBUTTON	0x20000000L
#define PDLG_PB_OK		1
#define PDLG_PB_CANCEL	2
#define PDLG_PB_DEVICE	3

#define PDLG_BUT_OK		(PDLG_PREBUTTON | PDLG_PB_OK)
#define PDLG_BUT_CNCL   (PDLG_PREBUTTON | PDLG_PB_CANCEL)
#define PDLG_BUT_DEV    (PDLG_PREBUTTON | PDLG_PB_DEVICE)
/* other names */
#define PDLG_BUT_CANCEL PDLG_BUT_CNCL
#define PDLG_BUT_DEVICE  PDLG_BUT_DEV

/* sub_flags */
#define	PRN_STD_SUBS	0x0001	/* Standard sub-dialogs for NVDI printers */
#define	PRN_FSM_SUBS	0x0002	/* Standard sub-dialogs for FSM-printers */
#define	PRN_QD_SUBS		0x0004	/* Standard sub-dialogs for QuickDraw printers */

/* Device description */
struct _prn_entry {
	PRN_ENTRY    *next;                /* Pointer to the next device description */
	_LONG        length;               /* Structure length */
	_LONG        format;               /* Data format */
	_LONG        reserved;             /* Reserved */
	
	_WORD        driver_id;            /* Driver ID */
	_WORD        driver_type;          /* Driver type */
	_LONG        printer_id;           /* Printer ID */
	_LONG        printer_capabilities; /* Printer capabilities */
	_LONG        reserved1;            /* Reserved */
	
	_LONG        sub_flags;            /* Flags for the sub-dialogs */
	PDLG_SUB    *sub_dialogs;          /* Pointer to the list of sub-dialogs for this printer */
	PRN_SWITCH  setup_panel;           /* Initialise sub-dialog at printer change */
	PRN_SWITCH  close_panel;           /* Close sub-dialog at printer change */
	
	PRN_MODE    *modes;                /* List of available resolutions */
	MEDIA_SIZE  *papers;               /* List of available paper formats */
	PRN_TRAY    *input_trays;          /* List of feed trays */
	PRN_TRAY    *output_trays;         /* List of output trays */
	char        name[32];              /* Name of printer */
};


/*----------------------------------------------------------------------------------------*/ 
/* Printer capabilities																							*/
/*----------------------------------------------------------------------------------------*/ 
#define	PC_FILE			0x0001										/* Printer can be addressed via GEMDOS files */
#define	PC_SERIAL		0x0002										/* Printer can be accessed on the serial port */
#define	PC_PARALLEL		0x0004										/* Printer can be accessed on the parallel port */
#define	PC_ACSI			0x0008										/* Printer can output on the ACSI port */
#define	PC_SCSI			0x0010										/* Printer can output on the SCSI port */

#define	PC_BACKGROUND	0x0080										/* Driver can print in background */

#define	PC_SCALING		0x0100										/* Driver can scale page */
#define	PC_COPIES		0x0200										/* Driver can produce copies of a page */

/*----------------------------------------------------------------------------------------*/ 
/* Mode capabilities																							*/
/*----------------------------------------------------------------------------------------*/ 
#define	MC_PORTRAIT		0x0001										/* Page can be output in portrait format */
#define	MC_LANDSCAPE	0x0002										/* Page can be output in landscape format */
#define	MC_REV_PTRT		0x0004										/* Page can be output turned 180 deg. in portrait format */
#define	MC_REV_LNDSCP	0x0008										/* Page can be output turned 180 deg. in landscape format */
#define	MC_ORIENTATION	0x000f

#define	MC_SLCT_CMYK	0x0400										/* Driver kann bestimmte Farbebenen ausgeben */
#define	MC_CTRST_BRGHT	0x0800										/* Driver kann Kontrast und Helligkeit ver„ndern */


/*----------------------------------------------------------------------------------------*/ 
/* Setable dithering modes                                                                */
/*----------------------------------------------------------------------------------------*/ 
#define	DC_NONE			0												/* No dithering modes */
#define	DC_FLOYD		1												/* Simple Floyd-Steinberg */

#define	NO_DC_BITS		1


/*----------------------------------------------------------------------------------------*/ 
/* Setable color modes of a printer mode                                                  */
/*----------------------------------------------------------------------------------------*/ 
#define	CC_MONO			0x0001										/* 2 grey levels */
#define	CC_4_GREY		0x0002										/* 4 grey levels */
#define	CC_8_GREY		0x0004										/* 8 grey levels */
#define	CC_16_GREY		0x0008										/* 16 grey levels */
#define	CC_256_GREY		0x0010										/* 256 grey levels */
#define	CC_32K_GREY		0x0020										/* 32768 colors in grey levels wandeln */
#define	CC_65K_GREY		0x0040										/* 65536 colors in grey levels wandeln */
#define	CC_16M_GREY		0x0080										/* 16777216 colors in grey levels wandeln */

#define	CC_2_COLOR		0x0100										/* 2 colors */
#define	CC_4_COLOR		0x0200										/* 4 colors */
#define	CC_8_COLOR		0x0400										/* 8 colors */
#define	CC_16_COLOR		0x0800										/* 16 colors */
#define	CC_256_COLOR	0x1000										/* 256 colors */
#define	CC_32K_COLOR	0x2000										/* 32768 colors */
#define	CC_65K_COLOR	0x4000										/* 65536 colors */
#define	CC_16M_COLOR	0x8000										/* 16777216 colors */

#define	NO_CC_BITS		16



/** printer settings
 * 
 *  The following structure items can be read by the application:
 *  - length
 *  - page_flags
 *  - first_page
 *  - last_page
 *  - no_copies
 *  - orientation
 *  - scale
 *  - driver_id
 *  .
 *  All other entries should not be accessed. Data such as the printer 
 *  resolution or colour planes, for instance, should not be taken from the 
 *  settings structure but requested from the printer at the start of printing 
 *  (it is possible, for instance, that the printer driver is forced by a 
 *  shortage of memory to reduce the print resolution below the value entered 
 *  in PRN_SETTINGS).
 */
struct _prn_settings {
	_LONG  magic;            /* 'pset'                                 */
	_LONG  length;           /* Structure length */
	_LONG  format;           /* Structure type */
	_LONG  reserved;         /* reserved */
	
	_LONG  page_flags;       /* Flags, inc. even pages, odd pages */
#define	PG_EVEN_PAGES	0x0001										/* Only output even-numbered pages */
#define	PG_ODD_PAGES	0x0002										/* Only output odd-numbered pages */
	
	_WORD  first_page;       /* First page to be printed */
#define PG_MIN_PAGE 1
	_WORD  last_page;        /* Last page to be printed */
#define PG_MAX_PAGE 9999

	_WORD  no_copies;        /* Number of copies */
	
	_WORD  orientation;      /* Orientation */
#define	PG_UNKNOWN		0x0000										/* Orientation unknown or not adjustable */
#define	PG_PORTRAIT		0x0001										/* Output page in portrait format */
#define	PG_LANDSCAPE	0x0002										/* Output page in landscape format */
	
	fixed  scale;            /* Scaling: 0x10000L corresponds to 100% */
	_WORD  driver_id;        /* VDI device number */
	_WORD  driver_type;      /* Type of selected driver */
	_LONG  driver_mode;      /* Flags, inc. for background printing */
#define	DM_BG_PRINTING	0x0001										/* Flag for background printing */

	_LONG  reserved1;        /* reserved */
	_LONG  reserved2;        /* reserved */
	
	_LONG  printer_id;       /* Printer number */
	_LONG  mode_id;          /* Mode number */
	_WORD  mode_hdpi;        /* Horizontal resolution in dpi */
	_WORD  mode_vdpi;        /* Vertical resolution in dpi */
	_LONG  quality_id;       /* Print mode (hardware-set quality, e.g. Microweave or Econofast) */
	
	_LONG  color_mode;       /* Color mode */
	_LONG  plane_flags;      /* Flags for number of color planes to be output (e.g. cyan only) */
#define	PLANE_BLACK		0x0001
#define	PLANE_YELLOW	0x0002
#define	PLANE_MAGENTA	0x0004
#define	PLANE_CYAN		0x0008

	_LONG  dither_mode;      /* Dithering mode */
	_LONG  dither_value;     /* Parameter for dithering mode */
	
	_LONG  size_id;          /* Paper format */
	_LONG  type_id;          /* Paper type (normal, glossy) */
	_LONG  input_id;         /* Paper feed tray */
	_LONG  output_id;        /* Paper eject tray */
	
	fixed  contrast;         /* Contrast: 0x10000L corresponds to normal setting */
	fixed  brightness;       /* Brightness: 0x10000L corresponds to normal setting */
	_LONG  reserved3;        /* reserved */
	_LONG  reserved4;        /* reserved */
	_LONG  reserved5;        /* reserved */
	_LONG  reserved6;        /* reserved */
	_LONG  reserved7;        /* reserved */
	_LONG  reserved8;        /* reserved */
	char device[128];        /* Filename for printout */
	
#ifdef __PRINTING__
	TPrint   mac_settings;   /* Settings of the Mac printer-driver */
#else
	struct
	{
	  char inside[120];
	} mac_settings;
#endif
};


struct _drv_info {
	_LONG        magic;           /* 'pdnf'                           */
	_LONG        length;          /* Structure length */
	_LONG        format;          /* Data format */
	_LONG        reserved;        /* Reserved */
	
	_WORD        driver_id;       /* Driver number for the VDI */
	_WORD        driver_type;     /* Driver type */
	_LONG        reserved1;       /* reserved */
	_LONG        reserved2;       /* reserved */
	_LONG        reserved3;       /* reserved */
	
	PRN_ENTRY   *printers;        /* List of printers belonging to driver */
	DITHER_MODE *dither_modes;    /* List of dither modes supported by driver */
	_LONG        reserved4;       /* reserved */
	_LONG        reserved5;       /* reserved */
	_LONG        reserved6;       /* reserved */
	_LONG        reserved7;       /* reserved */
	_LONG        reserved8;       /* reserved */
	_LONG        reserved9;       /* reserved */
	char         device[128];     /* Output file of printer driver */
};


/* <dialog_flags> for pdlg_create() */
#define PDLG_3D    0x0001         /* Auswahl im 3D-Look anzeigen      */

/* <option_flags> for pdlg_open/do() */
#define PDLG_PREFS         0x0000       /* Display preference dialog */
#define PDLG_PRINT         0x0001       /* Display print dialog */
#define PDLG_ALWAYS_COPIES 0x0010       /* Always offer copies */
#define PDLG_ALWAYS_ORIENT 0x0020       /* Always offer landscape format */
#define PDLG_ALWAYS_SCALE  0x0040       /* Always offer scaling */
#define PDLG_EVENODD       0x0100       /* Offer option for odd and even sides */

/* <button> for pdlg_evnt()/pdlg_do */
#define PDLG_CANCEL 1                   /* "Cancel" was selected */
#define PDLG_OK     2                   /* "OK" was pressed */


_WORD pdlg_add_printers(PRN_DIALOG *prn_dialog, DRV_INFO *drv_info);
_WORD pdlg_add_sub_dialogs(PRN_DIALOG *prn_dialog, PDLG_SUB *sub_dialog);
_WORD pdlg_close(PRN_DIALOG *prn_dialog, _WORD *x, _WORD *y);
PRN_DIALOG *pdlg_create(_WORD dialog_flags);
_WORD pdlg_delete(PRN_DIALOG *prn_dialog);
_WORD pdlg_dflt_settings(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings);
_WORD pdlg_do(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, const char *document_name, _WORD option_flags);
_WORD pdlg_evnt(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, EVNT *events, _WORD *button);
_WORD pdlg_free_settings(PRN_SETTINGS *prn_settings);
_LONG pdlg_get_setsize(void);
PRN_SETTINGS *pdlg_new_settings(PRN_DIALOG *prn_dialog);
_WORD pdlg_open(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings, const char *document_name, _WORD option_flags, _WORD x, _WORD y);
_WORD pdlg_remove_printers(PRN_DIALOG *prn_dialog);
_WORD pdlg_remove_sub_dialogs(PRN_DIALOG *prn_dialog);
_WORD pdlg_update(PRN_DIALOG *prn_dialog, const char *document_name);
_WORD pdlg_use_settings(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings);
_WORD pdlg_validate_settings(PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings);
_WORD pdlg_save_default_settings( PRN_DIALOG *prn_dialog, PRN_SETTINGS *settings );


/*
 * The following functions require NVDI version 5.x or higher
 */
 
DRV_INFO *v_create_driver_info( _WORD handle, _WORD driver_id );
_WORD v_delete_driver_info( _WORD handle, DRV_INFO *drv_info );
_WORD v_read_default_settings( _WORD handle, PRN_SETTINGS *settings );
_WORD v_write_default_settings( _WORD handle, PRN_SETTINGS *settings );
_WORD v_opnprn(_WORD aes_handle, PRN_SETTINGS *settings, _WORD work_out[]);



#endif /* __PDLG_H__ */
