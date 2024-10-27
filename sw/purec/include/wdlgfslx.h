#ifndef __FSLX_H__
#define __FSLX_H__

/*
 * exportierte Dateiauswahl-Funktionen
 */

#include <portab.h>
#if !defined(__TOS) && !defined(__TOS_H__)
#include <mintbind.h>
#endif
#if !defined(__VDI__)
#include <portvdi.h>
#endif
#include <wdlgevnt.h>


/* Sorting modes */

#define SORTBYNAME  0
#define SORTBYDATE  1
#define SORTBYSIZE  2
#define SORTBYTYPE  3
#define SORTBYNONE  4
#define SORTDEFAULT (-1)              /* MagiC 6.10 */

/* Flags for file selection */

#define DOSMODE     1
#define NFOLLOWSLKS 2
#define GETMULTI    8

/* fslx_set_flags */

#define SHOW8P3     1

#ifndef GEMLIB_XATTR
/* purec pctoslib defined __TOS in the file that defines the structure XATTR */
/* sozobonx xdlibs defined _file_h_ or _filesys_h_ in both files where the structure XATTR is defined */
/* MiNTLib defines the structure in <mintbind.h> or <mint/mintbind.h> */
/* in other case (XATTR not defined at this point), we go the old way and use "void" instead */
#  if defined(__TOS) || defined(_file_h_) || defined(_filesys_h_) || defined(_MINT_MINTBIND_H) || defined(__XATTR)
#    define GEMLIB_XATTR XATTR
#  else /* struct XATTR defined */
#    define GEMLIB_XATTR void
#  endif /* struct XATTR defined */
#endif /* GEMLIB_XATTR */

struct XFSL_FILTER_args {
	char *path;
	char *name;
	GEMLIB_XATTR *xa;
};
/* note: the callback needs arguments on stack;
   but since we pass the whole structure, the
   arguments will be pushed on the stack anyway */
typedef _WORD __CDECL (*XFSL_FILTER)(struct XFSL_FILTER_args);

typedef struct _fslx_dialog { int dummy; } XFSL_DIALOG;


XFSL_DIALOG *fslx_open(
	const char *title,
	_WORD x, _WORD y,
	_WORD *handle,
	char *path, _WORD pathlen,
	char *fname, _WORD fnamelen,
	const char *patterns,
	XFSL_FILTER filter,
	char *paths,
	_WORD sort_mode,
	_WORD flags);

_WORD fslx_evnt(
	XFSL_DIALOG *fsd,
	EVNT *events,
	char *path,
	char *fname,
	_WORD *button,
	_WORD *nfiles,
	_WORD *sort_mode,
	char **pattern);

XFSL_DIALOG *fslx_do(
	const char *title,
	char *path, _WORD pathlen,
	char *fname, _WORD fnamelen,
	const char *patterns,
	XFSL_FILTER filter,
	char *paths,
	_WORD *sort_mode,
	_WORD flags,
	_WORD *button,
	_WORD *nfiles,
	char **pattern);

_WORD fslx_getnxtfile(XFSL_DIALOG *fsd, char *fname);
_WORD fslx_close(XFSL_DIALOG *fsd);
_WORD fslx_set_flags(_WORD flags, _WORD *oldval);


/** parameters for UTXT_FN callback functions
 */
struct UTXT_FN_args
{
	_WORD x;
	_WORD y;
	_WORD *clip_rect;
	_LONG id;
	_LONG pt;
	_LONG ratio;
	char *string;
};

/* note: the callback needs arguments on stack;
   but since we pass the whole structure, the
   arguments will be pushed on the stack anyway */
typedef void __CDECL (*UTXT_FN)(struct UTXT_FN_args);

typedef struct _fnts_item FNTS_ITEM;

struct _fnts_item
{
    FNTS_ITEM   *next;                  /* Pointer to the next font or 0L (end of list) */
    UTXT_FN     display;                /* Pointer to the display function for application's own fonts */
    _LONG       id;                     /* ID of font, >= 65536 for application's own fonts */
    _WORD       index;                  /* Index of font (if a VDI-font) */
    char        mono;                   /* Flag for equidistant fonts */
    unsigned char outline;              /* Flag for vector font */
    _WORD       npts;                   /* Number of predefined point sizes */
    char        *full_name;             /* Pointer to complete name */
    char        *family_name;           /* Pointer to family name */
    char        *style_name;            /* Pointer to style name */
    char        *pts;                   /* Pointer to field with point sizes */
    _LONG       reserved[4];            /* Reserved, must be 0 */
};

/* Definitions for <font_flags> with fnts_create() */

#define FNTS_BTMP   1                   /* Display bitmap fonts */
#define FNTS_OUTL   2                   /* Display vector fonts */
#define FNTS_MONO   4                   /* Display equidistant fonts */
#define FNTS_PROP   8                   /* Display proportional fonts */
#define FNTS_ALL    15

/* Definitions for <dialog_flags> with fnts_create() */
#define FNTS_3D     1                   /* Use 3D-design */

/* Definitions for <button_flags> with fnts_open() */
#define FNTS_SNAME      0x01           /* Select checkbox for names */
#define FNTS_SSTYLE     0x02           /* Select checkbox for styles */
#define FNTS_SSIZE      0x04           /* Select checkbox for height */
#define FNTS_SRATIO     0x08           /* Select checkbox for width/height ratio */

#define FNTS_CHNAME     0x0100         /* Display checkbox for names */
#define FNTS_CHSTYLE    0x0200         /* Display checkbox for styles */
#define FNTS_CHSIZE     0x0400         /* Display checkbox for height */
#define FNTS_CHRATIO    0x0800         /* Display checkbox for width/height ratio */
#define FNTS_RATIO      0x1000         /* Width/height ratio adjustable */
#define FNTS_BSET       0x2000         /* "Set" button selectable */
#define FNTS_BMARK      0x4000         /* "Mark" button selectable */

/* Definitions for <button> with fnts_evnt() */

#define FNTS_CANCEL     1              /* "Cancel was selected */
#define FNTS_OK         2              /* "OK" was pressed */
#define FNTS_SET        3              /* "Set" was selected */
#define FNTS_MARK       4              /* "Mark" was selected */
#define FNTS_OPT        5              /* The application's own button was selected */
#define FNTS_OPTION		FNTS_OPT

typedef struct _fnt_dialog { int dummy; } FNT_DIALOG;

FNT_DIALOG *fnts_create(_WORD vdi_handle, _WORD no_fonts, _WORD font_flags, _WORD dialog_flags, const char *sample, const char *opt_button);
_WORD fnts_delete(FNT_DIALOG *fnt_dialog, _WORD vdi_handle);
_WORD fnts_open(FNT_DIALOG *fnt_dialog, _WORD button_flags, _WORD x, _WORD y, _LONG id, fix31 pt, _LONG ratio);
_WORD fnts_close(FNT_DIALOG *fnt_dialog, _WORD *x, _WORD *y);

_WORD fnts_get_no_styles(FNT_DIALOG *fnt_dialog, _LONG id);
_LONG fnts_get_style(FNT_DIALOG *fnt_dialog, _LONG id, _WORD index);
_WORD fnts_get_name(FNT_DIALOG *fnt_dialog, _LONG id, char *full_name, char *family_name, char *style_name);
_WORD fnts_get_info(FNT_DIALOG *fnt_dialog, _LONG id, _WORD *mono, _WORD *outline);

_WORD fnts_add(FNT_DIALOG *fnt_dialog, FNTS_ITEM *user_fonts);
void fnts_remove(FNT_DIALOG *fnt_dialog);
_WORD fnts_update(FNT_DIALOG *fnt_dialog, _WORD button_flags, _LONG id, _LONG pt, _LONG ratio);

_WORD fnts_evnt(FNT_DIALOG *fnt_dialog, EVNT *events, _WORD *button, _WORD *check_boxes, _LONG *id, _LONG *pt, _LONG *ratio);
_WORD fnts_do(FNT_DIALOG *fnt_dialog, _WORD button_flags, _LONG id_in, _LONG pt_in, _LONG ratio_in, _WORD *check_boxes, _LONG *id, _LONG *pt, _LONG *ratio);

#endif /* __FSLX_H__ */
