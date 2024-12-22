#include "dumpfile.h"
#include <stdio.h>
#include <gemlib.h>

#define NO_BYTES 64				/* number of bytes to display */

char directory[60] = "A:\\*.*";
char selection[12] = " ";
char file_name[80];				/* name of the file */
FILE *fp;						/* file pointer */
long file_length;				/* lenght of file */
OBJECT *dial_adr;				/* address of dialog box */


void 
ic_main()
{
	OBJECT *edit_adr;
	int object_no;
	int finished = 0;
	long address = 0;

	/* load resource file */
	if (!rsrc_load("dumpfile.rsc"))
	{
		/* test if error */
		fatal("Resource file not loaded");
	}

	disp_info();

	graf_mouse(M_ON, 0);
	/* open file */
	open_file();

	/* address of dialog box FORM1 */
	dial_adr = rs_addrdial(FORM1);

	/* clear the editable text field */
	strcpy(edit_adr = rs_addredit(dial_adr, ADDRESS), "000000");

	/* draw the form */
	rs_drawdial(dial_adr);
	display_file(0);
	do
	{
		/* give the user control */
		object_no = form_do(dial_adr, ADDRESS);

		/* process depending on exit button */
		switch (object_no)
		{
			case EXIT_BUTTON:
				/* deselect the button that caused the exit */
				rs_objunselect(dial_adr, object_no);
				rs_drawobject(dial_adr, object_no);
				finished = 1;
				break;
			case ADDRESS:
				sscanf(edit_adr, "%x", &address);
				display_file(address);
				/* deselect the button that caused the exit */
				rs_objunselect(dial_adr, object_no);
				rs_drawobject(dial_adr, object_no);
				break;
			default:
				rs_objselect(dial_adr, object_no);
				rs_drawobject(dial_adr, object_no);
				write_byte(address + object_no - FIRSTELEMENT, object_no);
				rs_objunselect(dial_adr, object_no);
				rs_drawobject(dial_adr, object_no);
		}
	}
	while (!finished);

	/* delete the dialog box and retore the screen */
	rs_erasedial();

	graf_mouse(M_OFF, 0);

	/* free the memory used by the resource file */
	rsrc_free();

	/* close the file */
	close_file();
}

/*----------------------*/
/* fatal          */
/*----------------------*/
void 
fatal(error)
	char *error;
{
	printf("Fatal Error : %s\n", error);
	exit(0);
}

/*----------------------*/
/* open_file        */
/*----------------------*/
void 
open_file()
{
	char *p;
	short state;

	if (fsel_input(directory, selection, &state) && state)
	{
		strcpy(file_name, directory);
		if (p = strrchr(file_name, '\\'))
			*(++p) = '\0';
		strcat(file_name, selection);
		if (fp = fopen(file_name, "rb+"))
		{
			fseek(fp, 0, 2);
			file_length = ftell(fp);
			return;
		}
	}
	fatal("file not found");
}

/*----------------------*/
/* close_file       */
/*----------------------*/
void 
close_file()
{
	fclose(fp);
}

/*----------------------*/
/* display_file      */
/*----------------------*/
void 
display_file(address)
	long address;
{
	int i, line_no = -1;
	int ch;
	char *str_adr;

	if (address > file_length)
		address = max(file_length - NO_BYTES, 0);
	fseek(fp, address, 0);
	for (i = 0; i < NO_BYTES; i++)
	{
		if ((i & 7) == 0)
		{
			line_no++;
			sprintf(rs_addredit(dial_adr, FIRSTADDRESS + line_no), "%06x", address + i);
			rs_drawobject(dial_adr, FIRSTADDRESS + line_no);
			str_adr = rs_addredit(dial_adr, FIRSTCHARACTER + line_no);
		}
		if (feof(fp))
			ch = 255;
		else
			ch = fgetc(fp);
		str_adr[(i & 7) + 1] = ch;
		drawch(ch, i);
	}
	for (i = 0; i < 8; i++)
		rs_drawobject(dial_adr, FIRSTCHARACTER + i);
}

/*----------------------*/
/* drawch        */
/*----------------------*/
void 
drawch(character, index)
	int character, index;
{
	sprintf(rs_addredit(dial_adr, FIRSTELEMENT + index), "%02x", character);
	rs_drawobject(dial_adr, FIRSTELEMENT + index);
}

/*----------------------*/
/* read_ch         */
/*----------------------*/
int 
read_ch()
{
	int ch;

	do
		ch = evnt_keybd() % 256;
	while (ch != 10 && !isxdigit(ch));
	return (ch);
}

/*----------------------*/
/* write_byte     */
/*----------------------*/
void 
write_byte(address, object_no)
	long address;
	int object_no;
{
	int ch1, ch2;
	char *edit_adr;
	int value;

	edit_adr = rs_addredit(dial_adr, object_no);
	if (address > file_length)
		return;

	/* read a byte from the keyboard */
	if (!isxdigit(ch1 = read_ch()))
		return;
	sprintf(edit_adr, "0%c", ch1);
	rs_drawobject(dial_adr, object_no);
	if (isxdigit(ch2 = read_ch()))
	{
		sprintf(edit_adr, "%c%c", ch1, ch2);
		rs_drawobject(dial_adr, object_no);
	}
	/* write the byte to the file */
	sscanf(edit_adr, "%x", &value);

	fseek(fp, address, 0);
	fputc(value, fp);
}

/*----------------------*/
/* display_info     */
/*----------------------*/
void 
disp_info()
{
	int dial_no;

	dial_no = init_box(70, 17, 10);
	button_box(dial_no, 30, 15, " Cont... ", 7);
	text_box(dial_no, 14, 2, "File Display and modification.");
	text_box(dial_no, 6, 4, "This program lets you display a file whose");
	text_box(dial_no, 6, 5, "name is selected with the file selector.");
	text_box(dial_no, 6, 6, "The file selected is displayed in hex and ASCII.");
	text_box(dial_no, 6, 7, "You can move within the displayed file by modifying the");
	text_box(dial_no, 6, 8, "address.");
	text_box(dial_no, 6, 9, "To change a byte, click on it with the mouse");
	text_box(dial_no, 6, 10, "and then enter the new hexadecimal value with the keyboard");
	text_box(dial_no, 6, 11, "The modification is then made in the file.");
	draw_box(dial_no);
}
