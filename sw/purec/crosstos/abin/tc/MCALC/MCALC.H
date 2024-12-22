/* MCALC.H

   Copyright (c)  Heimsoeth & Borland  1988

*/



#define S_IREAD    0x0100  /* from SYS\STAT.H */
#define S_IWRITE 0x0080 /* from SYS\STAT.H */
#define O_BINARY 8 /* FCNTL.H */
#define strlen(a) ((int)strlen(a)) /* No warnings ... */

#define TRUE 1
#define FALSE 0

#define MSGHEADER "MICROCALC - Ein Turbo C Demonstrationsprogramm"
#define MSGKEYPRESS "Bitte eine beliebige Taste druecken."
#define MSGCOMMAND "  Bitte / fuer eine Kommandoliste."
#define MSGMEMORY "Verfuegbarer Speicher:"
#define MSGERROR "FEHLER"
#define MSGLOMEM "Zu wenig Speicherplatz fÅr eine weitere Zelle."
#define MSGEMPTY "Leer"
#define MSGTEXT  "Text"
#define MSGVALUE "Wert"
#define MSGFORMULA "Formel"
#define MSGAUTOCALC "AutoCalc"
#define MSGFORMDISPLAY "Formular"
#define MSGFILENAME "Name des Kalkulationsblatts eingeben:"
#define MSGNAME "Turbo C 1.1 MicroCalc Spreadsheet"
#define MSGCOLWIDTH "Neue Spaltenbreite eingeben:"
#define MSGNOOPEN "Datei kann nicht geoeffnet werden."
#define MSGOVERWRITE "Datei exististiert. Soll sie ueberschrieben werden?"
#define MSGFILELOMEM "Zu wenig Speicherblatt fÅr ganzes Kalkulationsblatt."
#define MSGNOMICROCALC "Dies ist kein Turbo C MicroCalc Kalkulationsblatt."
#define MSGNOEXIST "Diese Datei existiert nicht."
#define MSGGOTO "Gesuchte Zelle eingeben:"
#define MSGBADNUMBER "Sie muessen eine Zahl zwischen %d und %d eingeben."
#define MSGBADCELL "Diese Zelle existiert nicht."
#define MSGCELL1 "Erste zu formatierende Zelle eingeben:"
#define MSGCELL2 "Letzte zu formatierende Zelle:"
#define MSGDIFFCOLROW "Die Spalte oder Zeile muû dieselbe sein."
#define MSGRIGHTJUST "Soll die Zelle rechts-justiert werden?"
#define MSGDOLLAR "Moechten Sie Zahlen im Dollar Format?"
#define MSGCOMMAS "Wollen Sie Kommas in den Zahlen?"
#define MSGPLACES "Auf wieviele Dezimalstellen soll gerundet werden?"
#define MSGCOLUMNS "Soll in 132 Spalten gedruckt werden?"
#define MSGPRINT "Name des zu druckenden Datei eingeben, oder druecken Sie ENTER um auf den Drucker zu schreiben."
#define MSGBORDER "Den Rahmen drucken?"
#define MSGLOADING "Datei laden..."
#define MSGSAVING "Datei schreiben..."
#define MSGSAVESHEET "Aktuelles Kalkulationsblatt speichern?"
#define MSGSTACKERROR "Zu lange komplexe Zeileneingabe."

#define MENU "Rahmen, Format., Loeschen, Gehe, Spalte, Zeile, Edit., Darst., Auto., Beenden"
#define COMMAND "RFLGSZEDAB"
#define SMENU "Laden, Speichern, Drucken, Neu"
#define SCOMMAND "LSDN"
#define CMENU "Einfuegen, Loeschen, Breite"
#define CCOMMAND "ELB"
#define RMENU "Einfuegen, Loeschen"
#define RCOMMAND "EL"
#define UMENU "Neuberechnen, Formeln anzeigen"
#define UCOMMAND "NF"
#define YESNOSTRING "JN"
#define YESCHAR 'J'

#define MAXCOLS 75   /* WARNING: MAXCOLS*MAXROWS*4 < 32767 !!! */
#define MAXROWS 75
#define LEFTMARGIN 3
#define MINCOLWIDTH 3
#define MAXCOLWIDTH 80 - LEFTMARGIN
#define SCREENCOLS (80 - LEFTMARGIN) / MINCOLWIDTH + 1
#define SCREENROWS 20
#define DEFAULTWIDTH 10
#define DEFAULTFORMAT 0X42
#define MAXINPUT 78
#define MAXPLACES 10
#define TOPMARGIN 5
#define PARSERSTACKSIZE 20

#define HIGHLIGHT TRUE
#define NOHIGHLIGHT FALSE
#define UPDATE TRUE
#define NOUPDATE FALSE
#define FORMAT TRUE
#define NOFORMAT FALSE
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define TEXT 0
#define VALUE 1
#define FORMULA 2
#define COLADD 0
#define COLDEL 1
#define ROWADD 2
#define ROWDEL 3
#define OVERWRITE 0X80
#define RJUSTIFY 0X40
#define COMMAS 0X20
#define DOLLAR 0X10

struct CELLREC
{  char attrib;
   union
   {  char text[MAXINPUT + 1];
      double value;
      struct
      {  double fvalue;
         char formula[MAXINPUT + 1];
      } f;
   } v;
};

typedef struct CELLREC *CELLPTR;

#define memsize (((long) Malloc(-1L)) - 1000)
#define textcellsize(s) (((strlen(s) >> 1) + 2) << 1)
#define valuecellsize sizeof(double)
#define formulacellsize(s) (((strlen(s) >> 1) + 6) << 1)


#define BS              0x08
#define FORMFEED        0x0C /* CTRL-L */
#define CR              0x0D
#define ESC             0x1B
#define HOMEKEY         0x147
#define ENDKEY          0x144 /* F10 */
#define UPKEY           0x148
#define DOWNKEY         0x150
#define PGUPKEY         0x13C /* F2 */
#define PGDNKEY         0x13D /* F3 */
#define LEFTKEY         0x14B
#define INSKEY          0x152
#define RIGHTKEY        0x14D
#define DELKEY          0x7F
#define CTRLLEFTKEY     0x173
#define CTRLRIGHTKEY    0x174
#define F1              0x13B
#define F2              0x13C
#define F3              0x13D
#define F4              0x13E
#define F5              0x13F
#define F6              0x140
#define F7              0x141
#define F8              0x142
#define F9              0x143
#define F10             0x144

int getkey(void);
int editstring(char *s, char *legal, int maxlength);
int getint(int *number, int low, int high);
void getinput(int c);
void scroll(int direction, int lines, int x1, int y1, int x2, int y2);
void writef(int col, int row, int width, va_list arg_list, ...);
void printcol(void);
void printrow(void);
void displcell(int col, int row, int highlighting, int updating);
void displcol(int col, int updating);
void displrow(int row, int updating);
void displscreen(int updating);
void clearinput(void);
void showcelltype(void);
double parse(char *s, int *att);
int alloctext(int col, int row, char *s);
int allocvalue(int col, int row, double amt);
int allocformula(int col, int row, char *s, double amt);
void deletecell(int col, int row, int display);
void printfreemem(void);
void moverowup(void);
void moverowdown(void);
void movecolleft(void);
void movecolright(void);
void recalc(void);
void changeautocalc(int newmode);
void changeformdisplay(int newmode);
void errormsg(char *s);
void colstring(int col, char *colstr);
void centercolstring(int col, char *colstr);
void setleftcol(void);
void setrightcol(void);
void settoprow(void);
void setbottomrow(void);
void setlastcol(void);
void setlastrow(void);
void act(char *s);
void initvars(void);
int getcommand(char *msgstr, char *comstr);
void mainmenu(void);
void editcell(CELLPTR ecell);
int setoflags(int col, int row, int display);
void clearoflags(int col, int row, int display);
void updateoflags(int col, int row, int display);
void loadsheet(char *filename);
int getcell(int *col, int *row);
char *cellstring(int col, int row, int formatting);
void writeprompt(char *prompt);
int getyesno(int *yesno, char *prompt);
void swap(int *val1, int *val2);
void redrawscreen(void);
void checkforsave(void);
void savesheet(void);
int formulastart(char **input, int *col, int *row);
int rowwidth(int row);
void fixformula(int col, int row, int action, int place);
void clearlastcol(void);
void run(void);
void gotocell(void);

#if !defined(MAIN)

extern CELLPTR cell[MAXCOLS][MAXROWS], curcell;
extern unsigned char format[MAXCOLS][MAXROWS];
extern unsigned char colwidth[MAXCOLS];
extern unsigned char colstart[SCREENCOLS];
extern char formdisplay;
extern char changed;
extern char autocalc;
extern int leftcol, rightcol, toprow, bottomrow, curcol, currow, lastcol,
           lastrow, direction;
extern long memleft;
extern char stop;
extern char matherror;

#endif
