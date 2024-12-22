/************************************************************************/
/*                                                                      */
/*       clockacc.c                                                     */
/*                                                                      */
/*       Beispielprogramm fÅr Accessories.                              */
/*                                                                      */
/*       Durch bloûes éndern der Extension im Dateinamen, lÑût sich     */
/*       Programm als normale GEM-Anwendung oder aber als Accessory     */
/*       betreiben.                                                     */
/*                                                                      */
/*       Das Programm zeigt in einem Fenster die Uhrzeit und Datum an.  */
/*                                                                      */
/*       Copyright (c) 1989 Borland International                       */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*       Headerdateien einbinden.                                       */
/* -------------------------------------------------------------------- */

#include <aes.h>
#include <vdi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------- */
/*       Makros.                                                        */
/* -------------------------------------------------------------------- */

#define min(a, b)             ((a) < (b) ? (a) : (b))
#define max(a, b)             ((a) > (b) ? (a) : (b))

/* -------------------------------------------------------------------- */
/*       Extern definierte globale Variablen.                           */
/* -------------------------------------------------------------------- */
                                    /* Mittels dieser Variablen kann    */
extern int _app;                    /* das Programm feststellen, ob es  */
                                    /* als Accessory oder normale App-  */
                                    /* likation gestartet wurde.        */
/* -------------------------------------------------------------------- */
/*       Globale Variablen.                                             */
/* -------------------------------------------------------------------- */

int    whandle;                     /* Handle fÅr geîffnetes Fenster.   */
char   title[] = " Turbo C Clock "; /* Titelzeile des Fensters.         */
int    gl_wchar,                    /* Grîûe und Breite eines Buchsta-  */
       gl_hchar,                    /* ben (wichtig falls mit unter-    */
       gl_wbox,                     /* schiedlichen Bildschirmauflî-    */
       gl_hbox;                     /* sungen gearbeitet wird) bzw.     */
                                    /* einer Box.                       */
int    phys_handle,                 /* Handles fÅr GEM und VDI.         */
       handle;
int    max_x,                       /* Maximale Grîûe der ArbeitsflÑche */
       max_y;
int    appl_id,                     /* Identifikationsnummer des Prog.  */
       menu_id;                     /* Id.-nummer im MenÅ 'Desk'.       */

time_t Tim = 0;                     /* MinutenzÑhler.                   */

char Time_pattern[40] = "  %H:%M - %a %d.%b %Y";

/* -------------------------------------------------------------------- */
/*       Funktionsprototypen.                                           */
/* -------------------------------------------------------------------- */

void open_window( void );
int  rc_intersect( GRECT *r1, GRECT *r2 );
void mouse_on( void );
void mouse_off( void );
void redraw_window( int all );
int  handle_message( int pipe[8] );
void event_loop( void );

/* -------------------------------------------------------------------- */
/*       void main( void );                                             */
/*                                                                      */
/*       KernstÅck des Programms.                                       */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void main( void )
{
   int i;
   int work_in[11];
   int work_out[57];

                                 /* ----------------------------------- */
                                 /* Programminitialisierung:            */
                                 /* ----------------------------------- */
   appl_id = appl_init( );
   if ( appl_id != -1 )
   {
      for ( i = 0; i < 10; i++ )
         work_in[i]  = 1;
      work_in[10] = 2;
      phys_handle = graf_handle( &gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox );
      handle = phys_handle;
      v_opnvwk( work_in, &handle, work_out );
      if ( handle != 0 )
      {
         max_x = work_out[0];
         max_y = work_out[1];
         if ( !_app )             /* Falls das Programm als Accessory   */
                                  /* gestartet wurde, hat _app den      */
                                  /* null, sonst eins.                  */

            menu_id = menu_register( appl_id, "  Turbo C Clock" );
         else
         {
            graf_mouse( 0, (void*)0 );
            open_window( );
         }

                                 /* ----------------------------------- */
                                 /* Event Loop                          */
                                 /* ----------------------------------- */
         event_loop( );

                                 /* ----------------------------------- */
                                 /* Deinitialisierung                   */
                                 /* ----------------------------------- */

         v_clsvwk( handle );
      }
      appl_exit( );
   }
   exit( 0 );
}

/* -------------------------------------------------------------------- */
/*       void open_window( void );                                      */
/*                                                                      */
/*       ôffnen eines Fensters fÅr die Zeitanzeige.                     */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void open_window( void )
{
   if ( whandle <= 0 )
   {
      whandle = wind_create( NAME|CLOSER|MOVER, 0, 0, max_x + 1, max_y + 1 );
      if ( whandle <= 0 )
         return;

      wind_set( whandle, WF_NAME, title );
      wind_open( whandle, max_x / 10, max_y / 10, max_x / 3 * 2, max_y / 5 );
   }
   else
      wind_set( whandle, WF_TOP );
}

/* -------------------------------------------------------------------- */
/*       boolean rc_intersect( GRECT *r1, GRECT *r2 );                  */
/*                                                                      */
/*       Berechnung der SchnittflÑche zweier Rechtecke.                 */
/*                                                                      */
/*       -> r1, r2               Pointer auf Rechteckstruktur.          */
/*                                                                      */
/*       <-                      == 0  falls sich die Rechtecke nicht   */
/*                                     schneiden,                       */
/*                               != 0  sonst.                           */
/* -------------------------------------------------------------------- */

int rc_intersect( GRECT *r1, GRECT *r2 )
{
   int x, y, w, h;

   x = max( r2->g_x, r1->g_x );
   y = max( r2->g_y, r1->g_y );
   w = min( r2->g_x + r2->g_w, r1->g_x + r1->g_w );
   h = min( r2->g_y + r2->g_h, r1->g_y + r1->g_h );

   r2->g_x = x;
   r2->g_y = y;
   r2->g_w = w - x;
   r2->g_h = h - y;

   return ( ((w > x) && (h > y) ) );
}

/* -------------------------------------------------------------------- */
/*       void mouse_on( void );                                         */
/*                                                                      */
/*       Mauszeiger anschalten.                                         */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void mouse_on( void )

{
   graf_mouse( M_ON, (void *)0 );
}

/* -------------------------------------------------------------------- */
/*       void mouse_off( void );                                        */
/*                                                                      */
/*       Mauszeiger ausschalten.                                        */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void mouse_off( void )
{
   graf_mouse( M_OFF, (void *)0 );
}

/* -------------------------------------------------------------------- */
/*       void redraw_window( int all );                                 */
/*                                                                      */
/*       Fensterinhalt neu zeichnen, nachdem er zuvor aus irgendeinem   */
/*       Grunde zerstîrt wurde, oder weil das Fenster neu geîffnet      */
/*       wurde.                                                         */
/*                                                                      */
/*       -> all                  == 0  Nur Datum und Uhrzeit neu,       */
/*                               != 0  gesamten Fensterinhalt neu       */
/*                                     schreiben                        */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void redraw_window( int all )
{
   GRECT     box,
             work;
   int       clip[4];
   char      s[40];                    /* ErhÑlt Datum und Uhrzeit      */
   time_t    tim;

   if( whandle <= 0 )                  /* Wenn kein Fenster auf ist,    */
      return;                          /* brauch auch nicht gezeichnet  */
                                       /* zu werden.                    */

                                       /* Wieder eine Minute vergangen? */
   if ( ((tim = time( &tim ) / 60 ) > Tim ) || all )
   {
      Tim = tim;                       /* Ja, dann Minuten merken.      */
      tim *= 60;

      strftime( s, 40, Time_pattern, localtime( &tim ));

      mouse_off( );

      vsf_color( handle, 0 );                       /* set white fill   */
      vswr_mode( handle, 1 );                       /* set replace mode */

      wind_get( whandle, WF_WORKXYWH, &work.g_x, &work.g_y, &work.g_w, &work.g_h );
      wind_get( whandle, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h );
      work.g_w = min( work.g_w, max_x - work.g_x + 1 );
      work.g_h = min( work.g_h, max_y - work.g_y + 1 );

      while ( box.g_w > 0 && box.g_h > 0 )
      {
         if( rc_intersect( &work, &box ) )
         {
            clip[0] = box.g_x;
            clip[1] = box.g_y;
            clip[2] = box.g_x + box.g_w - 1;
            clip[3] = box.g_y + box.g_h - 1;

            vs_clip( handle, 1, clip );
            if ( all )
               vr_recfl( handle, clip );              /* fill rectangle */
            v_gtext( handle, (work.g_x + work.g_w / 2) - strlen( s ) * gl_wchar / 2,
                     work.g_y + max_y / 10, s );
         }
         wind_get( whandle, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h );
      }
      mouse_on( );
   }
}

/* -------------------------------------------------------------------- */
/*       int handle_message( int pipe[8] );                             */
/*                                                                      */
/*       Auswertung der Ereignisse des Multi-Events bezÅglich des       */
/*       Messagebuffers.                                                */
/*                                                                      */
/*       ->                      Zeiger auf den Inhalt des Message-     */
/*                               buffers.                               */
/*                                                                      */
/*       <-                      Falls das Fenster geschlossen wird     */
/*                               und das Programm nicht als Accessory   */
/*                               gestartet wurde wird eine 1 zurÅckge-  */
/*                               geben zum Zeichen der Programmbeendi-  */
/*                               gung, sonst immer eine 0.              */
/* -------------------------------------------------------------------- */

int handle_message( int pipe[8] )
{
   switch ( pipe[0] )
   {
      case WM_REDRAW:
         redraw_window( 1 );
      break;

      case WM_TOPPED:
         wind_set( whandle, WF_TOP );
      break;

      case WM_CLOSED:
         if ( pipe[3] == whandle )
         {
            wind_close( whandle );
            wind_delete( whandle );
            whandle = 0;
         }
         if ( _app )
            return ( 1 );
      break;

      case WM_MOVED:
      case WM_SIZED:
         if ( pipe[3] == whandle )
            wind_set( whandle, WF_CURRXYWH,  pipe[4], pipe[5], pipe[6], pipe[7] );
     break;

      case AC_OPEN:
         if ( pipe[4] == menu_id )
            open_window( );
      break;

      case AC_CLOSE:
         if ( pipe[3] == menu_id )
            whandle = 0;
      break;
   }
   return ( 0 );
}

/* -------------------------------------------------------------------- */
/*    event_loop()                                                      */
/*                                                                      */
/*    Die Multi-Event-Schleife.                                         */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void event_loop( void )
{
   int x, y,
       kstate,
       key,
       clicks,
       event,
       state;
   int pipe[8];
   int quit;

   quit = 0;
   do
   {
      event = evnt_multi( MU_MESAG | MU_TIMER,
                          2, 0x1, 1,
                          0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0,
                          pipe,
                          1000, 0,
                          &x, &y, &state, &kstate, &key, &clicks );

      wind_update( BEG_UPDATE );

      if ( event & MU_MESAG )
         quit = handle_message( pipe );

      if ( event & MU_TIMER )
         redraw_window( 0 );

      wind_update( END_UPDATE );
   }
   while ( !quit );
}

/* -------------------------------------------------------------------- */
/*       End von CLOCKACC.C                                             */
/* -------------------------------------------------------------------- */
