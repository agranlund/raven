
#include <gemdefs.h>
#include <osbind.h>
#include <obdefs.h>

/*
    Declare VDI globals
*/
int contrl[12];
int intin[256],  ptsin[256];
int intout[256], ptsout[256];

main()
{
    int     handle;
    int     pxyarray[8];
    MFDB    source, destin;

    /*
        Initialize AES & VDI.
    */
    appl_init();
    handle = open_workstation(&source);
    destin = source;

    v_gtext(handle, 80, 60, "Moving the Menu Bar...");

    /*
        Set the source and destination rectangles.
    */
    rect_set(&pxyarray[0], 0, 0, source.fd_w, 40);
    rect_set(&pxyarray[4], 0, 100, source.fd_w, 140);

    /*
        Do the Copy.
    */
    vro_cpyfm(handle, S_ONLY, pxyarray, &source,  &destin);

    v_gtext(handle, 80, 80, "Press RETURN to end.");
    Bconin(2);

    appl_exit();
}
