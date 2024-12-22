/* CONIO.H
   FÅr Zeichen-Orientierte Fenster auf ATARI ST

   Copyright (c)  Heimsoeth & Borland  1988

*/

#define setmem(a,b,c) memset(a,c,b) /* Aus MEM.H */
#define movmem(a,b,c) memmove(b,a,c)

enum text_modes
{ LAST = -1, BW40, C40, BW80, C80, MONO = 7 };

struct text_info
{       unsigned char winleft;          /* x1 */
        unsigned char wintop;           /* y1 */
        unsigned char winright;         /* x2 */
        unsigned char winbottom;        /* y2 */
        unsigned char attribute;        /* Attribut */
        unsigned char normattr;         /* Normal-Attribut */
        unsigned char currmode;         /* MONO */
        unsigned char screenheight;     /* bottom-top */
        unsigned char screenwidth;      /* right-left */
        unsigned char curx;                     /* Aktuelle X-Koordinate */
        unsigned char cury;                     /* Aktuelle Y-Koordinate */
        /* Hier folgen NEUE Informationen */
        unsigned char xmax;                     /* Maximales x2 */
        unsigned char ymax;                     /* Maximales y2 */
        int app_handle;         /* Application Handle */
        int vdi_handle;         /* Virtual Device Handle */
        int winstyle;           /* Fenster-FÅllmuster */
};

/**** Funktionen ****/
int window(int x1, int y1, int x2, int y2); /* Fenster definieren */
void textmode(int mode);

void clreol(void); /* Bis Zeilenende lîschen */
void clrscr(void); /* Aktuelles Fenster lîschen */
void gotoxy(int x, int y); /* Cursor setzen */
void delline(void); /* Aktuelle Zeile lîschen und scrollen */
void insline(void); /* Zeile einfÅgen und scrollen */

int movetext(int sx1, int sy1, int sx2, int sy2, int dx1, int dy1);
int gettext(int x1, int y1, int x2, int y2, void *dest); /* yet NOT supported */
int puttext(int x1, int y1, int x2, int y2, void *source);

int cprintf(const char *format, ...); /* printf(...) */
int cputs(const char *string); /* puts() */
int couts(const char *string); /* puts() ohne CRLF */
int putch(int ch); /* putchar() */
int cscanf(const char *format, ...); /* scanf(), yet NOT supported */
int getche(void); /* getchar() */

void gettextinfo(struct text_info *inforec); /* Textinfo-Struktur kopieren */
void textattr(int attribute); /* Textattribut */
void textbackground(int color); /* Hintergrund-Farbe */
void textcolor(int color); /* Textfarbe */
int wherex(void); /* Cursor-Spalte */
int wherey(void); /* Cursor-Zeile */
char *getpass(const char *prompt); /* yet NOT supported */

/**** Nur aus KompatibilitÑtsgrÅnden vorhanden ****/
void highvideo(void);
void normvideo(void);
void lowvideo(void);
void nosound(void);
extern int directvideo;

/**** Erweiterungen ****/
void curleft(void); /* Cursor links */
void curright(void); /* Cursor rechts */
void curup(void); /* Cursor rauf */
void curdown(void); /* Cursor runter */
void curhome(void); /* Cursor home */
void newline(void); /* Cursor CRLF */
void curon(void); /* Cursor einschalten */
void curoff(void); /* Cursor ausschalten */
void revon(void); /* Revers Video Mode einschalten */
void revoff(void); /* Revers Video Mode ausschalten */

int winstyle(int style, int frame); /* VDI-Hintergrundstyle fÅr punktierte Muster */
void cwindfill(int, int, int, int);


/* Hier wird getrickst, damit eine Initialisierung von conio nicht
   notwendig ist */

#ifndef __CONIO
#define main _cont
#endif
