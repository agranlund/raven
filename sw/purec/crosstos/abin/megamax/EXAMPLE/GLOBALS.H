
#ifndef TRUE
#define TRUE		1
#define FALSE		0
#define NULL		0L
#endif 

#define WORKSIZE	1
#define BORDSIZE	0

#define	min(a, b)	((a < b)? a : b)
#define max(a, b)	((a > b)? a : b)

/*
	Each window will have a grafport so that it is easy to maintain 
	drawing environments accross windows.
*/
typedef struct _grafport {
	int		handle;		/*  Virtual workstation handle  					*/
	MFDB	mfdb;		/*  memory definition block for virtual workstation	*/
} grafport;


/*
	Window definition.
*/
typedef struct _windowrec {
	struct _windowrec	*next;		/*  Window link						*/
	int					handle;		/* Window Handle					*/
	int					kind;		/* The windows type					*/
	int					fullsize;	/* In full-screen mode				*/
	char				title[30];	/* Title of window					*/
	GRECT				box;		/* Window Box dimensions			*/
	GRECT				work;		/* Window content region dimensions	*/
	grafport			graf;		/* Window graphics port definition  */
	int					(*updateproc)(); /* Pointer to update procedure */
} windowrec;
typedef windowrec *windowptr;


/*
	Declare the GEM specific variables required by the ROM routines.
*/

extern int contrl[12];
extern int intin[256],  ptsin[256];
extern int intout[256], ptsout[256];


/*
	Application Global Variables.
*/
extern OBJECT		*menubar;
extern int			phys_handle;

#ifdef extern
windowptr	firstwindow = NULL;
windowptr	thefrontwin = NULL;
#else
extern windowptr	firstwindow;
extern windowptr	thefrontwin;
#endif

/*
	Functions
*/
extern char 		*malloc();
extern windowptr	new_window();
extern windowptr	frontwindow();
extern windowptr	findwindowptr();
