/************************************************************************/
/*                                                                      */
/*       PCFRAME.C                                                      */
/*                                                                      */
/*       Programmsequenz zum Starten eines GEM-Programms.               */
/*                                                                      */
/*       Dieses oder ein Ñhnliches ProgrammgerÅst leitet jedes GEM-     */
/*       Programm ein.                                                  */
/*       (In den weiteren GEM-Beispielen wird dieser Programmteil als   */
/*       gegeben vorausgesetzt).                                        */
/*                                                                      */
/*       Copyright (c) 1989 Borland International.                      */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*       include - Files einbinden.                                     */
/* -------------------------------------------------------------------- */

#include <vdi.h>
#include <aes.h>
#include <stdio.h>
#include <stdlib.h>

/* -------------------------------------------------------------------- */
/*       Typendefinition.                                               */
/* -------------------------------------------------------------------- */

typedef enum
{
    FALSE,
    TRUE
}
boolean;

/* -------------------------------------------------------------------- */
/*       Globale Variablen.                                             */
/* -------------------------------------------------------------------- */

int work_in[12],
    work_out[57];

int handle,
    phys_handle;

int gl_hchar,
    gl_wchar,
    gl_hbox,
    gl_wbox;

int gl_apid;

/* ------------------------------------------------------------------- */
/*    Funktionsprototypen.                                             */
/* ------------------------------------------------------------------- */

boolean open_vwork( void );
void    close_vwork( void );
void    gem_prg( void );

/* -------------------------------------------------------------------- */
/*       boolean open_vwork( void );                                    */
/*                                                                      */
/*       Workstation îffnen ...                                         */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      TRUE   falls das VDI initialisiert     */
/*                                      werden konnte,                  */
/*                               FALSE  sonst.                          */
/* -------------------------------------------------------------------- */

boolean open_vwork( void )
{
   register int i;

   if (( gl_apid = appl_init() ) != -1 )
   {
      for ( i = 1; i < 10; work_in[i++] = 1 );
      work_in[10] = 2;
      phys_handle = graf_handle( &gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox );
      work_in[0]  = handle = phys_handle;

      v_opnvwk( work_in, &handle, work_out );

      return ( TRUE );
   }
   else
      return ( FALSE );
}

/* -------------------------------------------------------------------- */
/*       void close_vwork( void );                                      */
/*                                                                      */
/*       ... und wieder schlieûen.                                      */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void close_vwork( void )
{
   v_clsvwk( handle );

   appl_exit( );
}

/* -------------------------------------------------------------------- */
/*       void main( void )                                              */
/*                                                                      */
/*       Hier steht Ihr C-Programm!                                     */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Programmendecode:                      */
/*                                         0 fehlerfreier Ablauf        */
/*                                        -1 Fehler in der Programm-    */
/*                                           initialisierung            */
/* -------------------------------------------------------------------- */

void main( void )
{
   if ( open_vwork( ) == TRUE )
   {

      /* ================================ */
      /* FÅgen Sie Ihr Programm hier ein: */
      /* -------------------------------- */

      gem_prg( );

      /* ================================ */

      close_vwork( );
      exit ( 0 );
   }
   else
   {
      fprintf( stderr, "Fehler bei der Programminitialisierung!" );
      exit ( -1 );
   }
}

/* -------------------------------------------------------------------- */
/*       Ende des GEM-ProgrammgerÅsts.                                  */
/* -------------------------------------------------------------------- */
