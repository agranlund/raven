/************************************************************************/
/*                                                                      */
/*       INSPDEMO.C                                                     */
/*                                                                      */
/*       Ein Beispielprogramm fÅr die Verwendung des Pure Debuggers.    */
/*                                                                      */
/*       Copyright (c) 1989 Borland International.                      */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

#include <stdio.h>

/* -------------------------------------------------------------------- */
/*       Strukturdefinition einer einfach verketteten Liste.            */
/*                                                                      */
/*       Hier: Liste enthÑlt Hotelbeschreibungen bestehend aus          */
/*             Hotelname, HotelKategorie und einem Zeiger auf den       */
/*             nÑchsten Listeneintrag.                                  */
/* -------------------------------------------------------------------- */

typedef struct hlist
{
    char            name[40];
    int             category;
    struct hlist    *next;
}
    HLIST;

/* -------------------------------------------------------------------- */
/*       Initialisierung der Hotelliste:                                */
/* -------------------------------------------------------------------- */

HLIST Hotel6 =
{
    "Strand Motel",
    2,
    0L
};

HLIST Hotel5 =
{
    "Paradise Hotel",
    4,
    &Hotel6
};

HLIST Hotel4 =
{
    "Grillton",
    5,
    &Hotel5
};

HLIST Hotel3 =
{
    "Last palace",
    0,
    &Hotel4
};

HLIST Hotel2 =
{
    "Grand Holyday",
    3,
    &Hotel3
};

HLIST Hotel1 =
{
    "Hotel Very Best",
    1,
    &Hotel2
};

/* -------------------------------------------------------------------- */
/*       void main( void );                                             */
/*                                                                      */
/*       Ausgabe der Hotelliste auf dem Monitor.                        */
/* -------------------------------------------------------------------- */

void main( void )
{
    HLIST *hp;                           /* Zeiger auf einen Listenein- */
                                         /* trag.                       */
    hp = &Hotel1;                        /* Initialisierung des Listen- */
                                         /* zeigers auf den ersten      */
                                         /* Eintrag.                    */

    puts( "Hotelnamensliste:" );
    puts( "-----------------" );

    while ( hp != 0L )                   /* Letzter Eintrag ist durc h  */
    {                                    /* Nullzeiger gekennzeichnet.  */
        printf( "\nName: %s\t\tCategory: %d", hp->name, hp->category );
        hp = hp->next;                   /* Zeiger auf nÑchsten Eintrag */
    }                                    /* setzen.                     */

    puts( "\n\nWeiter mit <Return>" );
    getchar( );
}

/* -------------------------------------------------------------------- */
/*       Ende INSPDEMO.C                                                */
/* -------------------------------------------------------------------- */
