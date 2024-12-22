/*
  v_curtext()
  v_eeos()
*/

#include <gemdefs.h>
#include <osbind.h>
#include <ctype.h>

int contrl[12];
int intin[256],  ptsin[256];
int intout[256], ptsout[256];

#define ESC				0x1b

#define SPECIAL			0x00
#define RIGHT_ARROW		0x4d
#define LEFT_ARROW		0x4b
#define DOWN_ARROW		0x50
#define UP_ARROW		0x48
#define HOME			0x47
#define INSERT			0x52

#define STATUS_ROW	25
#define STATUS_COL	1

main()
{
	MFDB	source;
	int		handle;

	int		keycode, keychar;
	long	newkey;
	int		row, column;
	int		maxrow, maxcolumn;

	/*
		Start up ROM.
	*/
	appl_init();
	handle = open_workstation(&source);

	/*
		Begin the Text mode.
	*/
	v_enter_cur(handle);

	/*
		Get the size of the text display
	*/
	vq_chcells(handle, &maxrow, &maxcolumn);

	row			 = 1;
	column		 = 1;

	do {
		if (isalpha(keychar) || isspace(keychar)) {
			do_char(handle, keychar);
			keychar = SPECIAL;
			keycode = RIGHT_ARROW;
		}

		if (keychar == SPECIAL) {
			switch(keycode) {
				case UP_ARROW:
					v_curup(handle);

					if (! --row) 
						row++;
				break;

				case DOWN_ARROW:
					v_curdown(handle);

					if (++row > maxrow) 
						row--;
				break;

				case RIGHT_ARROW:
					v_curright(handle);

					if (++column > maxcolumn)
						column--;
				break;

				case LEFT_ARROW:
					v_curleft(handle);

					if (! --column)
						column++;
				break;

				case HOME:
					row    = 1;
					column = 1;
					v_curhome(handle);
				break;

				case INSERT:
					v_eeos(handle);
				break;
			}
		}

		/*
			Position the cursor to the new position.
		*/
		vs_curaddress(handle, row, column);

		/*
			Display the Status line.
		*/
		do_status_line(handle);

		/*
			Get Keycode.
		*/
		newkey = Bconin(2);

		/*
			Separate the real keys from the special keys.  The low word of
				the long returned from Bconin() is the actual key pressed and
				the high word returned is the code for one of the special keys
				on the Atari keyboard.
		*/
		keychar	= newkey & 0xffff;
		keycode	= (newkey >> 16) & 0xffff;
	} while(keychar != ESC);

	/*
		Quit out of text mode.
	*/
	v_exit_cur(handle);

	/*
		Close the virtual workstation.
	*/
	v_clsvwk(handle);
	appl_exit();
}

do_status_line(handle)
{
	int		current_row, current_column;
	char	outline[80];

	/*
		Obtain current row and column information.
	*/
	vq_curaddress(handle, &current_row, &current_column);

	/*
		Move cursor to status line
	*/
	vs_curaddress(handle, STATUS_ROW, STATUS_COL);

	/*
		Build text to be printed as status line
	*/
	sprintf(outline, "row == %d - col == %d", current_row, current_column);

	/*
		Clean up the previous status line.
	*/
	v_eeol(handle);

	/*
		display the current status line in reverse video.
	*/
	v_rvon(handle);
	v_curtext(handle, outline);
	v_rvoff(handle);

	/*
		restore the original cursor position.
	*/		
	vs_curaddress(handle, current_row, current_column);
}

/*
    Print the character.  
*/
do_char(handle, c)
	int handle;
	int c;
{
	char outline[2];

	outline[0] = c;
	outline[1] = 0;
	v_curtext(handle, outline);
}
