/*
  vqt_name()
  vqt_width()
*/

#include <gemdefs.h>

int contrl[12];
int intin[256],  ptsin[256];
int intout[256], ptsout[256];

main()
{
	MFDB	source;
	int		handle;

	/*
		Start up ROM.
	*/
	appl_init();
	handle = open_workstation(&source);

	/*
	    Display some font statistics
	*/
	show_font_info(handle, 1);

	show_char_info(handle, 'W');
	show_char_info(handle, 'e');
	show_char_info(handle, '.');

	/*
		Wait & Close the virtual workstation.
	*/
	wait(handle);
	v_clsvwk(handle);
	appl_exit();
}

show_font_info(handle, face_num)
    int handle;
    int face_num;
{
    struct {
        char font_name[16];
        char font_style[16];
        int  font_id;
    } fontinfo;

    fontinfo.font_id = vqt_name(handle, face_num, &fontinfo);

    printf("Font name  == <%s>\n", fontinfo.font_name);
    printf("Font style == <%s>\n", fontinfo.font_style);
    printf("Font ID    == <%d>\n", fontinfo.font_id);
}

show_char_info(handle, thechar)
    int  handle;
    char thechar;
{
    int cell_width, left, right;

    vqt_width(handle, thechar, &cell_width, &left, &right);

    printf("The character [%c] is %d pixels wide.\n",
                        thechar,  cell_width - (left + right));
}
