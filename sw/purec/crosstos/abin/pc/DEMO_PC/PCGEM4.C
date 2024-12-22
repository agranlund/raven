/************************************************************************/
/*                                                                      */
/*       Beispielprogramm:                             EVNT_MULTI( )    */
/*                                            diverse WIND_-Routinen    */
/*                                                                      */
/*       Copyright (c) 1989 Borland International                       */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*       Include-Files einbinden.                                       */
/* -------------------------------------------------------------------- */

#include <aes.h>
#include <vdi.h>
#include <stdio.h>

#include "scancode.h"

/* -------------------------------------------------------------------- */
/*       Konstantendefinitionen.                                        */
/* -------------------------------------------------------------------- */

#define DESK                  0

#define W_KIND                NAME|CLOSER|FULLER|MOVER|SIZER
#define EV_KIND               MU_MESAG|MU_TIMER|MU_KEYBD

#define WINDOW_NAME           "Pure C - Beispielprogramm"

#define min(a, b)             ((a) < (b) ? (a) : (b))
#define max(a, b)             ((a) > (b) ? (a) : (b))

/* -------------------------------------------------------------------- */
/*    Typendefinition.                                                  */
/* -------------------------------------------------------------------- */

typedef enum
{
    FALSE,
    TRUE
}
boolean;

/* -------------------------------------------------------------------- */
/*    Funktionsprototypen.                                              */
/* -------------------------------------------------------------------- */

void    gem_prg( void );
void    hndl_window( void );
void    do_redraw( int w_handle, int x, int y, int w, int h );
void    new_size( int w_handle, int x, int y, int w, int h );
void    close_window( int w_handle );
void    clipping( int x, int y, int w, int h, int mode );
boolean rc_intersect( GRECT *r1, GRECT *r2 );
boolean open_window( void );

/* -------------------------------------------------------------------- */
/*       Globale Variablen.                                             */
/* -------------------------------------------------------------------- */

int     Msgbuff[8];                 /* Buffer fr Mitteilungen.         */

int     W_handle,                   /* Variablen zum Arbeiten mit einem */
        Wx, Wy, Ww, Wh;             /* Fenster - bei mehreren Fenstern  */
boolean W_fulled;                   /* ist ein Feld mit diesen Variabl. */
                                    /* zu vereinbaren.                  */
int     pxarray[128];               /* Feld fr Clipkoordinaten etc.    */
boolean Done = FALSE;               /* 'Mach weiter'-Flag.              */

/* -------------------------------------------------------------------- */
/*       Extern definierte globale Variable.                            */
/* -------------------------------------------------------------------- */

extern int handle;
extern int gl_wbox,
           gl_hbox;

/* -------------------------------------------------------------------- */
/*       void gem_prg( void );                                          */
/*                                                                      */
/*       Beispiel fr die Verwendung der Funktion evnt_multi( ), sowie  */
/*       diverser Funktionen der wind-Bibliothek in einem GEM-Programm. */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void gem_prg( void )
{
   int      style = 0;              /* Fllmuster fr Fenster.          */
   int      event,                  /* Ergebnis mit Ereignissen.        */
            mx, my,                 /* Mauskoordinaten.                 */
            mbutton,                /* Mausknopf.                       */
            mstate,                 /* Status des Mausknopfs.           */
            keycode,                /* Scancode einer Tastatureingabe.  */
            mclicks;                /* Anzahl Mausklicks.               */

   vsf_color( handle, 1 );          /* Farbindex setzen.                */
   vsf_interior( handle, 3 );       /* Schraffierte Muster zeichnen.    */

   if ( open_window( ) == TRUE )    /* Ein Fenster zu Beginn ”ffnen.    */
   {
      do
      {
         event = evnt_multi( EV_KIND,
                             1, 1, 1,
                             0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0,
                             Msgbuff,
                             1000, 0,  /* 1 s warten.                   */
                             &mx, &my,
                             &mbutton, &mstate,
                             &keycode, &mclicks);

         wind_update( BEG_UPDATE ); /* Jetzt darf GEM nicht mehr        */
                                    /* alleine zeichnen.                */
         if ( event & MU_MESAG )    /* Auswertung der Ereignisse.       */
         {
            if ( Msgbuff[0] >= WM_REDRAW && WM_NEWTOP >= Msgbuff[0] )
               hndl_window( );      /* Fensteraktion.                   */
         }
         else if ( event & MU_TIMER ) /* Nach 1 Sekunden anderen        */
         {                            /* Fensterinhalt zeichnen.        */
            style++;                  /* Fllmuster „ndern.             */
            if ( style == 13 )
               style = 1;
            vsf_style( handle, style );
            do_redraw( W_handle, Wx, Wy, Ww, Wh );

         }
         else if( event & MU_KEYBD )
         {
            if ( keycode == CNTRL_C ) /* Mit Control-C kann beendet     */
            {                         /* werden.                        */
               if ( W_handle != -1 )
                  close_window( W_handle );
               Done = TRUE;
            }
         }
         wind_update( END_UPDATE );
      }
      while ( !Done );
   }
}

/* -------------------------------------------------------------------- */
/*       void hndl_window( void )                     handle windows    */
/*                                                                      */
/*       Behandlen der Windowaktionen.                                  */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void hndl_window( void )
{

   switch( Msgbuff[0] )
   {
      case WM_REDRAW:               /* Fensterinhalt neu zeichen.       */
         do_redraw( Msgbuff[3], Msgbuff[4], Msgbuff[5], Msgbuff[6], Msgbuff[7] );
      break;

      case WM_CLOSED:               /* Fenster wurde geschlossen.       */
         close_window( Msgbuff[3] );  /* Programm beendet.                */
         Done = TRUE;
      break;

      case WM_MOVED:                /* Fenster wurde bewegt oder in     */
      case WM_SIZED:                /* seiner Gr”že ver„ndert.          */
         if ( Msgbuff[6] < 10 * gl_wbox )  /* Mindestgr”že fr Fenster. */
            Msgbuff[6] = 10 * gl_wbox;
         if ( Msgbuff[7] < 5 * gl_hbox )
            Msgbuff[7] = 5 * gl_hbox;
         new_size( Msgbuff[3], Msgbuff[4], Msgbuff[5], Msgbuff[6], Msgbuff[7] );
      break;

      case WM_TOPPED:               /* Fenster (wurde) aktiviert.       */
      case WM_NEWTOP:
         wind_set( Msgbuff[3], WF_TOP, 0, 0, 0, 0 );
         wind_get( Msgbuff[3], WF_WORKXYWH, &Wx, &Wy, &Ww, &Wh );
      break;

      case WM_FULLED:               /* Fenster zoomen.                  */
         if (( W_fulled ^= TRUE ))
            wind_get( Msgbuff[3], WF_FULLXYWH, &Msgbuff[4], &Msgbuff[5], &Msgbuff[6], &Msgbuff[7] );
         else
            wind_get( Msgbuff[3], WF_PREVXYWH, &Msgbuff[4], &Msgbuff[5], &Msgbuff[6], &Msgbuff[7] );
         wind_get( Msgbuff[3], WF_WORKXYWH, &Wx, &Wy, &Ww, &Wh );
         new_size( Msgbuff[3], Msgbuff[4], Msgbuff[5], Msgbuff[6], Msgbuff[7] );
      break;
   }
}

/* -------------------------------------------------------------------- */
/*       boolean open_window( void );                                   */
/*                                                                      */
/*       ™ffnen eines Fensters. Bei erfolgreichem ™ffnen wird TRUE zu-  */
/*       rckgeliefert.                                                 */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      TRUE  falls ein Fenster ge”ffnet wer-  */
/*                                     konnte (dann wurden auch dessen  */
/*                                     globale Parameter gesetzt),      */
/*                               FALSE sonst.                           */
/* -------------------------------------------------------------------- */

boolean open_window( void )
{
   int new,
       xdesk, ydesk, wdesk, hdesk;
                                    /* Gr”že Arbeitsfl„che des Desktop. */
   wind_get( DESK, WF_WORKXYWH, &xdesk, &ydesk, &wdesk, &hdesk );
   if (( new = wind_create( W_KIND, xdesk, ydesk, wdesk, hdesk )) < 0 )
   {
      form_alert( 1, "[3][Kann kein Fenster ”ffnen.][Abbruch]" );
      return ( FALSE );
   }
   graf_mouse( M_OFF, 0 );          /* Maus aus.                        */
   wind_set( new, WF_NAME, WINDOW_NAME, 0, 0 ); /* Fenstername setzen.        */
                                    /* ™ffnendes Rechteck zeichnen.     */
   graf_growbox( 0, 0, 0, 0, wdesk / 10, hdesk / 10, wdesk / 3, hdesk / 3 );
   wind_open( new, wdesk / 10, hdesk / 10, wdesk / 3, hdesk / 3 );
                                    /* Fenster ”ffnen.                  */
                                    /* Arbeitsfl„che des Fensters best. */
   wind_get( new, WF_WORKXYWH, &Wx, &Wy, &Ww, &Wh );
   W_handle = new;
   W_fulled = FALSE;
   graf_mouse( M_ON, 0 );           /* Maus an.                         */
   graf_mouse( ARROW, 0 );

   return ( TRUE );                 /* ERFOLG!                          */
}

/* -------------------------------------------------------------------- */
/*       void do_redraw( int w_handle, int x, int y, int w, int h );    */
/*                                                                      */
/*       Neu zeichnen des Inhalts eines Fensters.                       */
/*                                                                      */
/*       -> w_handle             Handle des zu schlieženden Fensters.   */
/*          x, y          x-,y-Koordinate des Zeichenbereichs.          */
/*          w, h          H”he und Breite des Zeichenbereichs.          */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void do_redraw( int w_handle, int x, int y, int w, int h )
{
   GRECT t1, t2;

   t2.g_x     = pxarray[0] = x;     /* Zeichenkoordinaten setzen.       */
   t2.g_y     = pxarray[1] = y;
   t2.g_w     = w;
   t2.g_h     = h;
   pxarray[2] = x + w - 1;
   pxarray[3] = y + h - 1;

   graf_mouse( M_OFF, 0 );
   wind_get( w_handle, WF_FIRSTXYWH, &t1.g_x, &t1.g_y, &t1.g_w, &t1.g_h );
   while ( t1.g_w && t1.g_h )
   {
      if ( rc_intersect( &t2, &t1 ) == TRUE )
      {                             /* Nur berechnetes Rechteck         */
                                    /* neu zeichen, sonst k”nnten       */
                                    /* andere Bildschirminhalte         */
                                    /* zerst”rt werden.                 */
         clipping( t1.g_x, t1.g_y, t1.g_w, t1.g_h, TRUE );
         v_bar( handle, pxarray );
      }
      wind_get( w_handle, WF_NEXTXYWH, &t1.g_x, &t1.g_y, &t1.g_w, &t1.g_h );
   }
   clipping( t1.g_x, t1.g_y, t1.g_w, t1.g_h, FALSE );

   graf_mouse( M_ON, 0 );
}

/* -------------------------------------------------------------------- */
/*       void new_size( int w_handle, int x, int y, int w, int h );     */
/*                                                                      */
/*       Fenster an neue Gr”že anpassen.                                */
/*                                                                      */
/*       -> w_handle             Fensterhandle.                         */
/*          x, y, w, h           Auženkoordinaten des Fensters.         */
/*                                                                      */
/*       <-                      Die globalen Variablen, die die Koor-  */
/*                               dinaten der Fensterarbeitsfl„che ent-  */
/*                               halten, werden updated.                */
/* -------------------------------------------------------------------- */

void new_size( int w_handle, int x, int y, int w, int h )
{
   wind_set( w_handle, WF_CURRXYWH, x, y, w, h );
   wind_get( w_handle, WF_WORKXYWH, &Wx, &Wy, &Ww, &Wh );
}

/* -------------------------------------------------------------------- */
/*       void close_window( int w_handle );                             */
/*                                                                      */
/*       Schliežen eines Fensters.                                      */
/*                                                                      */
/*       -> w_handle             Handle des zu schlieženden Fensters.   */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void close_window( int w_handle )
{
   int x, y, w, h;

   wind_get( w_handle, WF_CURRXYWH, &x, &y, &w, &h );
   graf_shrinkbox( 0, 0, 0, 0, x, y, w, h );
   wind_close( w_handle );
   wind_delete( w_handle );
}

/* -------------------------------------------------------------------- */
/*       boolean rc_intersect( GRECT *r1, GRECT *r2 );                  */
/*                                                                      */
/*       Berechnung der Schnittfl„che zweier Rechtecke.                 */
/*                                                                      */
/*       -> r1, r2               Pointer auf Rechteckstruktur.          */
/*                                                                      */
/*       <-                      TRUE  falls sich die Rechtecke         */
/*                                     schneiden,                       */
/*                               FALSE sonst.                           */
/* -------------------------------------------------------------------- */

boolean rc_intersect( GRECT *r1, GRECT *r2 )
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

   return ( (boolean) ((w > x) && (h > y) ) );
}

/* -------------------------------------------------------------------- */
/*       void clipping( int x, int y, int w, int h, int mode );         */
/*                                                                      */
/*       Last but not least: (Re-)Set Clipping Rectangle ...            */
/*                                                                      */
/*       -> x                    x-Koordinate der Clipp' Begrenzung.    */
/*          y                    y-Koordinate       - " -          .    */
/*          w                    Breite des begrenzten Rechtecks.       */
/*          h                    H”he         - " -             .       */
/*          mode                 TRUE  - clipping on,                   */
/*                               FALSE - clipping off.                  */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void clipping( int x, int y, int w, int h, int mode )
{
   pxarray[0] = x;
   pxarray[1] = y;
   pxarray[2] = x + w - 1;
   pxarray[3] = y + h - 1;

   vs_clip( handle, mode, pxarray );
}

/* -------------------------------------------------------------------- */
/*       Ende der Beispielprogramms fr EVNT_MULTI( ).                  */
/* -------------------------------------------------------------------- */
