/*
 * $Header: e:/lc/examples/bell/cpx\bell.c,v 1.6 1993/11/09 14:35:34 AGK Exp $
 *
 * bell.c - CPX portion of HiSoft Bell Sample CPX
 *
 * $Author: AGK $
 *
 * This example CPX/TSR combination form part of Lattice C/TT and
 * demonstrate the construction of CPXs under Lattice C. This example
 * aims to show many of the salient features of constructing a CPX, but
 * cannot provide as much detail as Atari's own documentation which you
 * should also refer to.
 *
 * This programs implements a CPX which controls a TSR which in turn allows
 * the system bell to be replaced by a sampled sound in place of the normal
 * bell.
 *
 * Compile this program simply by running make, note that phase 2
 * generates a lot of warnings about standard arguments for indirect
 * functions. This is a side effect of using the -rr option.
 *
 * $Log: bell.c,v $
 * Revision 1.6  1993/11/09  14:35:34  AGK
 * Path, struct names & pointer changes
 *
 * Revision 1.5  1992/06/11  16:47:48  AGK
 * Change so that we can work with pre-TOS 2.00's Malloc()
 *
 * Revision 1.4  1992/04/09  12:06:38  AGK
 * Changes to the way we generate self-redraws. Ideally we'd post a message
 * to ourself so that it could be merged by the AES, but since we don't know
 * our application ID, we can't, hence we try to optimise redraw as much as
 * possible but cop out in the impossible situations.
 *
 * Revision 1.3  1991/07/05  13:32:12  AGK
 * Stopped the user from double-clicking on the 'Standard Bell' entry.
 *
 * Revision 1.2  1991/05/23  13:11:58  AGK
 * Changes to use AVR format samples, fix a problem with a file opened for
 * write when we meant read/write
 *
 * Revision 1.1  1991/04/21  12:21:32  AGK
 * Initial revision
 *
 * Copyright (c) 1991, 1992 HiSoft
 */

#include <aes.h>
#include <vdi.h>
#include <cpx.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <gemout.h>
#include <oserr.h>
#include <acc.h>
#include "avr.h"
#include "tsr.h"
#include "rbell.h"

#define BELL_NLINES	6			// number of visible lines in 'window'

/*
 * This CPX saves its configuration information in the TSR file, so we
 * never need to use xcpb->CPX_Save, although if you needed to this is
 * how you do it. Rather than having to alter the startup code, because
 * of the way an executable file is laid out by the linker, using a far
 * variable as the *very first* initialised item in the first file
 * linked ensures that it occurs at the start of the data section hence
 * we can create a config block:
 *
 *	struct
 *	{
 *		char name[FMSIZE];
 *		int def1,def2;
 *	} far config={""};			// to force initialisation.
 *
 * This may then be saved by:
 *
 *	xcpb->CPX_Save(&config,sizeof(config));
 *
 */

struct config *cookie;			// pointer to public version of config
struct config config;			// private version of cookie which we use

/*
 * This points to a sound buffer which we have allocated via Mxalloc, we
 * have to be very careful that we always release this buffer before we
 * do anything which may cause an AES context switch.
 */
void *sound_buf=NULL;

XCPB *xcpb;						// XControl Parameter Block

GRECT desk;						// rectangle of desktop

MFORM Mbuffer;					// Mouse form save buffer

short cur_item;					// item at top of sample window
short last_cur_item;			// previous item at top of sample window

int samples;					// real number of samples
int max_samples;				// number of samples - BELL_NLINES

GRECT clip,clip2;				// general use clipping rectangles
MFDB mfdb;						// a screen MFDB for scroling

char fselpath[FMSIZE];			// paths used for storing filenames
char fselname[FMSIZE];

/*
 * This is the sample list definition. It consists of a doubly linked
 * list of entries containing the samples which are available in the
 * current sample directory.
 */
struct samp_list
{
	struct samp_list *next;		// next samp_list entry (or NULL)
	struct samp_list *prev;		// previous samp_list entry (or NULL)
	char name[FNSIZE];			// filename of sample in sample directory
	struct
	{
		long length;			// length of sample
		short rate;				// sample replay 'rate'
		char name[28];			// 28 character name - 0 padded
	} h;						// extracted from file header
	char zero;					// a zero since h.name is not guaranteed
								// null terminated.
};

struct samp_list simple_beep;	// dummy entry for standard bell

struct samp_list *root;			// root of list

struct samp_list *current_sound;	// the current sound
struct samp_list *first;			// pointer to element at top of window

avr_t avr_h;					// global AVR header for sample loading/saving

long mxalloc = 0;				// does this GEMDOS know about Mxalloc?

/*
 * set_bell_box_text - scan through list of samples, starting at p,
 * moving pointers such that the 'window' has the p'th sample at the top.
 * We also set the TOUCHEXIT flag on those which are active and track the
 * current (selected) sample here.
 */
void
set_bell_box_text(struct samp_list *p)
{
	int i;

	for (i=bell_text1; i<=bell_text6; i++)
	{
		if (p)		// if samples left to process
		{
			bell_box[i].ob_flags|=TOUCHEXIT;
			if (p==current_sound)
				bell_box[i].ob_state|=SELECTED;	// the current one
			else
				bell_box[i].ob_state&=~SELECTED;	// not current (!)
			bell_box[i].ob_spec=p->h.name;
			p=p->next;
		}
		else
		{
			static char null_string[]="";

			bell_box[i].ob_flags&=~TOUCHEXIT;	// deactivate entry
			bell_box[i].ob_spec=null_string;
			bell_box[i].ob_state&=~SELECTED;
		}
	}
}

/*
 * construct_path - build a GEMDOS path name out of a stem, which
 * optionally has junk on the end which we must delete, and a filename
 * which we are interested in.
 */
void
construct_path(char *dest,const char *path,const char *name)
{
	char *s=NULL;		// used to track the position after final \ or :

	if (path[0] && path[1]==':')
		s=&dest[2];
	while (*dest++=*path)
		if (*path++=='\\')
			s=dest;
	if (!s)
		s=dest;
	strcpy(s,name);
}

/*
 * open_file - open a file along the current configuration path, in the
 * mode specified.
 */
int
open_file(const char *s,int mode)
{
	construct_path(fselpath,config.snd_path,s);
	return (int)Fopen(fselpath,mode);
}

/*
 * process_err - process error return from GEMDOS function
 */
void
process_err(int fd)
{
	if (fd==-EFILNF)
		xcpb->XGen_Alert(FILE_NOT_FOUND);
	else if (fd<0)
		xcpb->XGen_Alert(FILE_ERR);
}

/*
 * do_file - process a sample file, extracting the length and sample
 * rate information into p.
 *
 * This routine is woefully short on error checking and should be extended.
 */
void
do_file(struct samp_list *p)
{
	int fd=open_file(p->name,FO_READ);

	if (fd>=0)
	{
		static unsigned short freqs[]={0,12517,25033,50066};
		int i;

		Fread(fd,sizeof(avr_h),&avr_h);
		p->h.length=avr_h.avr_length;
		p->h.name[sizeof(avr_h.avr_name)-1]='\0';		// place sentinel in buffer
		strncpy(p->h.name,avr_h.avr_name,sizeof(avr_h.avr_name));
		if (p->h.name[sizeof(avr_h.avr_name)-1]!='\0')	// did they hit the sentinel ?
			strncpy(p->h.name+sizeof(avr_h.avr_name),avr_h.avr_xname,
			  sizeof(avr_h.avr_xname));

		/*
		 * Here we approximate the frequency stored in the header
		 */
		for (i=sizeof(freqs)/sizeof(freqs[0]); i--; )
			if (freqs[i]>=(avr_h.avr_frequency&0xffffff))
				p->h.rate=0x80|i;

		Fclose(fd);
		p->zero=0;
	}
	else
		process_err(fd);
}

/*
 * sort_list - sort the list of samples alphabetically. This is a simple
 * bubble sort (for the reasons below). This routine is used to ensure
 * that the display looks 'right'.
 */
void
sort_list(void)
{
	struct samp_list *p,*q;

	for (p=root; p; p=p->next)
		q=p;			// track last element for bubble sort;

	/*
	 * Sort the doubly linked list using a bubble sort. We bubble sort
	 * because its so simple, and we never expect there to be many
	 * samples (due to disk space constraints). Insertion or selection
	 * would be better and are left as an exercise.
	 */
	for (; q; q=q->prev)
		for (p=root->next; p; p=p->next)
			if (stricmp(p->prev->h.name,p->h.name)>=0)
			{
				struct samp_list s;

				/*
				 * Exchange the two items. We use a little witchcraft
				 * here to ensure that we only exchange the data entries
				 * of the samp_list entry and *not* the previous and
				 * next pointers.
				 */
				s=*p;
				memcpy(&p->name,&p->prev->name,
				  sizeof(s)-offsetof(struct samp_list,name));
				memcpy(&p->prev->name,&s.name,
				  sizeof(s)-offsetof(struct samp_list,name));
			}

	/*
	 * Locate current sample
	 */
	current_sound=NULL;
	for (p=root; p; p=p->next)
	{
		if (!p->name[0])	// indicates the "Standard Bell"
		{
			if (!current_sound)
				current_sound=p;
		}
		else if (!strcmp(p->name,config.snd_cur))
		{
			current_sound=p;
			break;
		}
	}
	/*
	 * Did we reach the end of the list without locating the current sample?
	 * i.e. someone has changed dirs etc. to somewhere where the sample no
	 * longer exists.
	 */
	if (!p)
		config.snd_cur[0]='\0';
}

/*
 * get_flist - retrieve list of files from the sample directory. This is
 * the main routine for building the sample list. We first build the sample
 * chain and then go through filling it all in.
 */
void
get_flist(GRECT *rect, GRECT *rrect)
{
	struct FILEINFO info;
	struct samp_list *p;

	/*
	 * Put up a marquee saying that we're scanning the sound directory,
	 * as this may take some time.
	 */
	rc_center((GRECT *)&bell_box[ROOT].ob_x,(GRECT *)&snd_scanning[ROOT].ob_x);
	clip=*(GRECT *)&snd_scanning[ROOT].ob_x;

	clip.g_x-=3;				// Allow for the outline on the box
	clip.g_y-=3;
	clip.g_w+=6;
	clip.g_h+=6;

	rc_intersect(rect,&clip);	// find intersection of the boxs - redundant ?
	objc_draw(snd_scanning, ROOT, MAX_DEPTH, ELTS(clip));

	/*
	 * We must initialise the simple_beep manually as we manipulate the
	 * elements of the structure during sort_list hence this may change
	 * on consecutive calls to get_flist();
	 */
	simple_beep.next=simple_beep.prev=NULL;
	simple_beep.name[0]='\0';
	simple_beep.h.length=0;
	strcpy(simple_beep.h.name,snd_standard);

	/*
	 * reset the slider position
	 */
	bell_box[bell_slider].ob_y=0;
	cur_item=last_cur_item=0;

	root=&simple_beep;			// seed the sample list
	samples=0;

	/*
	 * Scan through the sample directory locating matching entries and
	 * putting them on our list.
	 */
	if (!dfind(&info,config.snd_path,0))
		do
		{
			if (!(p=malloc(sizeof(struct samp_list))))
			{
				xcpb->XGen_Alert(MEM_ERR);
				break;
			}
			strcpy(p->name,info.name);	// fill in entry
			root->prev=p;		// and link into list
			p->prev=NULL;
			p->next=root;
			root=p;
		} while (!dnext(&info));

	/*
	 * Scan through the list we just built obtaining the sample rates,
	 * lengths etc.
	 */
	for (p=root; p; p=p->next)
	{
		samples++;
		if (p->name[0])			// the "Standard bell"
			do_file(p);
	}

	sort_list();				// sort the sucker

	/*
	 * Set maximum number of samples. If there are fewer than fit in the
	 * window then we set this to zero.
	 */
	max_samples=samples-BELL_NLINES;
	if (max_samples<0)
		max_samples=0;

	xcpb->Sl_size(bell_box, bell_bar, bell_slider, samples, BELL_NLINES,
	  VERTICAL, 2);				// size up the slider

	/*
	 * Initialise the window to show the top of the list, ideally this
	 * would put the current sample somewhere in the window, but this is
	 * potentially tricky (i.e. I can't be bothered right now !)
	 */
	first=root;
	set_bell_box_text(root);

	/*
	 * Compute a redraw rectangle for the window, and then pass it back to
	 * our caller to handle.
	 */
	objc_xywh(bell_box,bell_scroll,rrect);
	rc_union(&clip, rrect);
}

/*
 * fill_ted - fill in a TEDINFO te_ptext field with << and >> characters
 * if the string is too long to fit and/or is scrolled off the start position.
 */
void
fill_ted(char *t,int maxlen,int cur_pos,const char *s)
{
	if (cur_pos!=0)
	{
		*t++='\xae';			// the << character
		maxlen--;
		while (*++s && cur_pos--)
			;
	}
	maxlen--;					// in case we have need a space on the end
	while (--maxlen)
	{
		if (*s)
			*t++=*s++;
		else
			*t++='_';
	}
	if (*s)						// didn't see terminator
		if (!s[1])
			*t++=*s;
		else
			*t++='\xaf'; 		// the >> character

	*t='\0';
}

/*
 * ted_window - manage the events generated by scrolling the tedinfo
 * 'window'. This is a bit kludgey as we don't bother to scroll the
 * region, but instead redraw it resulting in flicker.
 */
void
ted_window(OBJECT *tree,int object,int change,const char *s)
{
	GRECT rdrw;
	int cur_pos;
	unsigned slen,maxlen;
	TEDINFO *ted;

	ted=(TEDINFO *)tree[object].ob_spec;	// find tedinfo we're using

	maxlen=ted->te_txtlen;
	slen=(unsigned)strlen(s)+1;
	if (slen<maxlen)						// if already at maxiumum
		return;								// then do nothing

	/*
	 * We store the current position in the extended object type byte.
	 * This allows us to generalise this routine without needing an
	 * auxiliary variable, but does reduce the callers options.
	 */
	cur_pos=*(unsigned char *)&tree[object].ob_state;
	cur_pos+=change;

	/*
	 * Compute new position
	 */
	if (cur_pos<0)
	{
		change=0;
		cur_pos=0;
	}
	else if (cur_pos+maxlen>slen)
	{
		change=0;
		cur_pos=slen-maxlen;
	}

	*(unsigned char *)&tree[object].ob_state=cur_pos;	// save new position

	fill_ted(ted->te_ptext,maxlen,cur_pos,s);

	/*
	 * Decide whether a redraw is really necessary and perform it on the
	 * TEDINFO only.
	 */
	objc_xywh(bell_box,bell_sndpath,&rdrw);
	if (change)
		objc_draw(bell_box,ROOT,MAX_DEPTH,ELTS(rdrw));
}

/*
 * draw_bell - redraw the window based on slider movements. This is the
 * heart of the window code. It checks the amount of movement which
 * occurred, scrolls the box as necessary and then redraws the remaining
 * (if any) portion.
 */
void __saveds
draw_bell(void)
{
	GRECT scroll_box,rdrw;
	short up[8],down[8],*scroll;
	int change;
	int i;
	int noscroll;

	/*
	 * XControl 1.0 has a bug which allows Sl_drag to return a value one
	 * greater than the maximum allowed when you have a large number of
	 * elements (as we often will have). This check ensures that at
	 * least we won't crash although the display will be partially
	 * corrupted.
	 */
	if (cur_item>max_samples)
		return;

	change=cur_item-last_cur_item;	// compute change which has occurred
	last_cur_item=cur_item;

	scroll=NULL;					// assume we can't scroll initially
	/*
	 * Compute default redraw rectangle (whole box)
	 */
	objc_xywh(bell_box,bell_scroll,&scroll_box);

	rdrw=scroll_box;

	/*
	 * If the scrolling region is partially off screen then we give up
	 * trying to build a scroll region (since we can't see all of it)
	 * and just redraw. There is a potential optimisation which blits
	 * the visible parts, but is it really worth the effort, for the
	 * sake of a user who's hell bent on trying to break you?
	 */
	rc_intersect(&desk,&rdrw);

	if (!rc_equal(&rdrw,&scroll_box))	// intersection was not the whole
		noscroll=1;	// note that rdrw is now the on-screen redraw rectangle
	else
		noscroll=0;

	if (change>0)						// +ve change, i.e. move down
	{
		/*
		 * Scan forward through the window elements to find the new first
		 * element (i.e. the one at the top of the window).
		 */
		for (i=change; i; i--)
			first=first->next;

		/*
		 * If change is less than a full screen then we can usefully
		 * blit part of the window up (since we are moving down).
		 */
		if (!noscroll && change<BELL_NLINES)
		{
			/*
			 * Compute the scroll rectangle. This is hairy, and you
			 * really have to trace through it to convince yourself its
			 * right. Watch the -1s which are the width/height to xy/xy
			 * fudge factors.
			 */
			up[0]=up[4]=scroll_box.g_x;
			up[2]=up[6]=scroll_box.g_x+scroll_box.g_w-1;

			up[5]=scroll_box.g_y;
			up[1]=scroll_box.g_y+bell_box[bell_text6].ob_height*change;
			up[3]=scroll_box.g_y+scroll_box.g_h-1;
			up[7]=up[3]-bell_box[bell_text6].ob_height*change;
			scroll=up;

			/*
			 * Compute new redraw area based on the height of a single
			 * window element.
			 */
			objc_xywh(bell_box,bell_text6-change+1,&rdrw);
			rdrw.g_h*=change;
		}
	}
	else if (change<0)				// -ve change, i.e. move up
	{
		/*
		 * Scan backward through the window elements to find the new first
		 * element (i.e. the one at the top of the window).
		 */
		for (i=change; i; i++)
			first=first->prev;

		/*
		 * If change is less than a full screen then we can usefully
		 * blit part of the window down (since we are moving up).
		 */
		if (!noscroll && change>-BELL_NLINES)
		{
			/*
			 * Compute the scroll rectangle. This is hairy, and you
			 * really have to trace through it to convince yourself its
			 * right. Watch the -1s which are the width/height to xy/xy
			 * fudge factors.
			 */
			down[0]=down[4]=scroll_box.g_x;
			down[2]=down[6]=scroll_box.g_x+scroll_box.g_w-1;

			down[1]=scroll_box.g_y;
			down[5]=scroll_box.g_y-bell_box[bell_text1].ob_height*change;
			down[7]=scroll_box.g_y+scroll_box.g_h-1;
			down[3]=down[7]+bell_box[bell_text1].ob_height*change;
			scroll=down;

			/*
			 * Compute new redraw area based on the height of a single
			 * window element.
			 */
			objc_xywh(bell_box,bell_text1,&rdrw);
			rdrw.g_h*=-change;
		}
	}

	if (change)							// is there anything to do ?
	{
		set_bell_box_text(first);		// reset the text in the window
		graf_mouse(M_OFF,NULL);			// hide the mouse

		/*
		 * If we have a scroll region then blit it now, we draw on the
		 * AES handle which is a bit naughty really, but we know we're
		 * only blitting...
		 */
		if (scroll)
		{
			vs_clip(xcpb->handle,0,NULL);
			vro_cpyfm(xcpb->handle,S_ONLY,scroll,&mfdb,&mfdb);
		}

		/*
		 * Redraw the remaining region. This has already been computed
		 * for us, so we just get on and draw.
		 */
		objc_draw(bell_box,ROOT,MAX_DEPTH,ELTS(rdrw));
		graf_mouse(M_ON,NULL);
	}
}

/*
 * save_defaults - save the current settings. This routine saves out the
 * settings to the TSR which is our companion. We have to save them
 * there because it must know the location of the sample directory at boot
 * time when the CPX is not available. We could save the current sample
 * name using CPX_Save, but it seems silly to call two routines when one
 * will do.
 */
int
save_defaults(void)
{
	int fd;
	struct gemohdr hdr;
	long ssize;

	/*
	 * Find offset of the first element we *don't* want to save in the
	 * configuration information. If you don't understand the offsetof
	 * macro check out the STDDEF.H header file in a book on ANSI C.
	 */
	ssize=offsetof(struct config,snd_tsr);
	graf_mouse(BUSY_BEE, NULL);

	fd=(int)Fopen(config.snd_tsr,FO_RW);

	/*
	 * If we didn't manage to open the TSR file using the path it had
	 * saved in the cookie jar then ask the user to find it for us.
	 * This can happen if a user has a strange AUTO program
	 * configuration utility.
	 */
	while (fd == -EFILNF || fd == -EDRIVE || fd == -EPTHNF) {
		short button;

		/*
		 * If we didn't manage to find the TSR by ourselves then we save
		 * its location in itself so we don't have to ask every time.
		 */
		ssize=offsetof(struct config,snd_play);

		/*
		 * Build a suitable path for the file selector
		 */
		strcpy(fselname,"BELL.PRG");
		construct_path(fselpath,config.snd_tsr,"*.PRG");

		if (fsel_exinput(fselpath,fselname,&button,snd_locatetsr) && button)
		{
			/*
			 * Got an OK selection
			 */
			graf_mouse(BUSY_BEE, NULL);			// hide it again

			/*
			 * Construct a pathname for the file the user selected and
			 * have another go. We keep trying until either they find us
			 * a file or they cancel.
			 */
			construct_path(config.snd_tsr,fselpath,fselname);
			fd=(int)Fopen(config.snd_tsr,FO_RW);
		}
		else
		{
			strcpy(config.snd_tsr,cookie->snd_tsr);		// implement cancel
			return 0;
		}
	}
	if (fd<0)
	{
		/*
		 * Some sort of error which wasn't file not found, so process it via
		 * form_error.
		 */
		fd=-fd;
		if (fd<50)
			fd-=31;
		form_error(fd);
		return 0;
	}

	/*
	 * Read in the executable header and rewrite the configuration info
	 * which is located at the start of the data segment. Again this
	 * code could do with considerably more error checks.
	 */
	Fread(fd,sizeof(hdr),&hdr);
	Fseek(hdr.g_ssize[GO_TEXT],fd,FSEEK_CUR);	// seek to data section
	Fwrite(fd,ssize,&config);					// write out config info
	Fclose(fd);
	return 1;
}

/*
 * play_sound - play the current sample. This routine exists solely as
 * a target for a Supexec() call.
 */
long
play_sound(void)
{
	config.snd_play(sound_buf,current_sound->h.length,current_sound->h.rate);
	return 0;				// to stop warnings appearing
}

/*
 * wait_sample - wait for a sample to stop playing. We only really want
 * this to happen when clicking between items to we don't try and overlap
 * them. Again this must be called in Supervisor mode.
 */
long
wait_sample(void)
{
	config.snd_wait();
	return 0;				// to stop warnings appearing
}

/*
 * release_buffer - check if a buffer is allocated and free it.
 */
void
release_buffer(void)
{
	if (sound_buf)
	{
		Mfree(sound_buf);
		sound_buf=NULL;
	}
}

/*
 * load_sample - load a sample into memory. If sound_buf is not set up
 * we load it into the cookie's buffer.
 */
void
load_sample(const char *s,long length)
{
	int fd=open_file(s,FO_READ);

	if (fd>=0)
	{
		Fseek(sizeof(avr_t),fd,FSEEK_SET);
		Fread(fd,length,sound_buf?sound_buf:(cookie+1));
		Fclose(fd);
	}
	else
	{
		/*
		 * process_err can let the AES run, so we must release the memory
		 * first just in case a context switch occurs.
		 */
		release_buffer();
		process_err(fd);
	}
}

/*
 * insert_filename - extract an AES style filename ("xxxx    yyy") from
 * a GEMDOS ("xxxx.yyy") one.
 */
void
insert_filename(char *s,const char *t)
{
	int i;

	for (i=8; *t && i--; )			// scan 'til we see the dot
		if (*t=='.')				// and then pad with spaces
			*s++=' ';
		else
			*s++=*t++;

	if (*t++=='.')					// if there was an extension
		while (*s++=*t++)			// add it now
			;
	else
		*s='\0';					// else terminate the string
}

/*
 * extract_filename - extract a GEMDOS filename ("xxxx.yyy") from an AES
 * style ("xxxx    yyy") one.
 */
void
extract_filename(char *s,const char *t)
{
	int i;

	for (i=8; i--; t++)				// copy until we get a space char
		if (*t!=' ')
			*s++=*t;

	if (*t!=' ')					// if the next character is not a space
	{
		*s++='.';					// then it must be an extension
		while (*s++=*t++)
			;
	}
	else
		*s='\0';					// else terminate the string
}

/*
 * rewrite_avr - write changes made to the sample name/rate etc. back to
 * the sample stored on the disk.
 */
void
rewrite_avr(struct samp_list *p)
{
	int fd;
	static unsigned short freqs[]={6258,12517,25033,50066};

	fd=open_file(p->name,FO_RW);
	Fread(fd,sizeof(avr_t),&avr_h);	// load existing header
	Fseek(0,fd,FSEEK_SET);			// seek back to start
	/*
	 * Fill in all of the name
	 */
	strncpy(avr_h.avr_name,p->h.name,sizeof(avr_h.avr_name));
	strncpy(avr_h.avr_xname,p->h.name+sizeof(avr_h.avr_name),sizeof(avr_h.avr_xname));
	avr_h.avr_frequency=freqs[p->h.rate&3];
	Fwrite(fd,sizeof(avr_t),&avr_h);	// write back to disk
	Fclose(fd);
}

/*
 * reconfigure - reconfigure a sample entry. The user has double clicked
 * on an entry in the sample window hence we allow them to edit the entry.
 */
void
reconfigure(short button,GRECT *rect)
{
	int quit=0;
	int obj;
	struct samp_list *p;
	static char *items[]=			// array for Popup() handler
	{
		poptext_1,
		poptext_2,
		poptext_3,
		poptext_4,
	};
	short curitem;
	int redraw_parent=1;			// flag to redraw our parent object

	/*
	 * Fill in the current window coordinates based on the parent box.
	 */
	snd_reconfigure[ROOT].ob_x = bell_box[ROOT].ob_x;
	snd_reconfigure[ROOT].ob_y = bell_box[ROOT].ob_y;

	/*
	 * Find out which string the user actually clicked on
	 */
	obj=button-bell_text1;
	p=first;
	while (p && obj--)
		p=p->next;

	if (p->name[0])		// p->name[0] == 0 indicates the "Standard Bell"
	{
		/*
		 * Fill in the reconfiguration dialogue. This looks complicated, but
		 * isn't, we're only filling in the elements, but its confused by
		 * the TEDINFO references.
		 */
		strcpy(((TEDINFO *)snd_reconfigure[reconf_sampname].ob_spec)->te_ptext,
		  p->h.name);
		insert_filename(((TEDINFO *)snd_reconfigure[reconf_filename].ob_spec)->te_ptext,p->name);
		curitem=p->h.rate-0x80;
		((TEDINFO *)snd_reconfigure[reconf_samprate].ob_spec)->te_ptext=items[curitem];

		objc_draw(snd_reconfigure,ROOT,MAX_DEPTH,PTRS(rect));
		do
		{
			short msg[8];

			/*
			 * Sit and wait for a suitable message from Xform_do()
			 */
			button = xcpb->Xform_do(snd_reconfigure, reconf_sampname, msg);

			if ((button != -1) && (button & 0x8000))
				button &= 0x7FFF;				// mask off double clicks

			if (button == -1)
			{
				switch (msg[0])
				{
					case AC_CLOSE:				// ac_close means cancel
						button=reconf_btcancel;
						redraw_parent=0;
						break;

					case WM_CLOSED:				// wm_close means ok
						redraw_parent=0;
						button=reconf_btok;
						break;
				}
			}

			snd_reconfigure[button].ob_state&=~SELECTED;
			switch (button)
			{
				case reconf_btok:
					/*
					 * OK means that we must go through copying the updated
					 * information back into the samp_list entry. We watch
					 * to see if anything has actually changed, if it has
					 * then we must modify the file on disk.
					 */
					extract_filename(fselname,
					  ((TEDINFO *)snd_reconfigure[reconf_filename].ob_spec)->te_ptext);
					if (strcmp(fselname,p->name))
					{
						/*
						 * The name has changed, we build both new and old
						 * filenames and then rename it.
						 */
						construct_path(fselpath,config.snd_path,p->name);
						strcpy(p->name,fselname);
						construct_path(fselname,config.snd_path,p->name);
						Frename(0,fselpath,fselname);
					}
					if (strcmp(p->h.name,
					  ((TEDINFO *)snd_reconfigure[reconf_sampname].ob_spec)->te_ptext) ||
					  p->h.rate!=curitem+0x80)
					{
						/*
						 * Something has changed such that we must modify
						 * the file. Hence we open it up and rewrite the
						 * relevant info at the start of it.
						 */
						strcpy(p->h.name,
						  ((TEDINFO *)snd_reconfigure[reconf_sampname].ob_spec)->te_ptext);
						p->h.rate=curitem+0x80;
						rewrite_avr(p);

						/*
						 * The name may have changed hence we must sort the
						 * list again.
						 */
						sort_list();
					}
					/* falls thru' */

				case reconf_btcancel:
					/*
					 * Easy Huh!
					 */
					quit = 1;
					break;

				case reconf_samprate:
					/*
					 * Obtain rectangle of sample rate popup activation
					 * button and call the popup draw/handle routine.
					 */
					objc_xywh(snd_reconfigure,reconf_samprate,&clip);
					obj=xcpb->Popup(items,sizeof(items)/sizeof(items[0]),
					  curitem, IBM, &clip, rect);

					/*
					 * If an object was actually selected, then update our
					 * settings.
					 */
					if (obj!=NIL)
					{
						curitem=obj;
						((TEDINFO *)snd_reconfigure[reconf_samprate].ob_spec)->te_ptext=items[curitem];
					}

					/*
					 * Redraw the popup button.
					 */
					objc_draw(snd_reconfigure,ROOT,MAX_DEPTH,ELTS(clip));
					break;

			}
		} while (!quit);
		/*
		 * Sometimes we don't need to redraw the parent 'cos we're going
		 * away anyway.
		 */
		if (redraw_parent)
		{
			/*
			 * Reset the window contents (since we may have sorted the list)
			 */
			set_bell_box_text(first);

			/*
			 * And ensure that any movements are reflected in the parent object.
			 */
			bell_box[ROOT].ob_x=snd_reconfigure[ROOT].ob_x;
			bell_box[ROOT].ob_y=snd_reconfigure[ROOT].ob_y;
			objc_draw(bell_box,ROOT,MAX_DEPTH,PTRS((GRECT *)&bell_box[ROOT].ob_x));
		}
	}
}

/*
 * update_current - update the cookie's copy of the configuration
 * settings. If necessary we reload the contents of the public sound
 * buffer from the disk file.
 * We should really issue an error to the user if the buffer is too
 * small, again an area for further devlopment.
 */
void
update_current(void)
{
	long cplen=config.snd_length;

	*cookie=config;

	if (cplen>config.snd_maxlen)	// ensure we can never write off the end
		cplen=config.snd_maxlen;

	if (config.snd_cur[0])			// not the standard bell
	{
		release_buffer();			// belt and braces, me thinks...
		load_sample(config.snd_cur,cplen);
	}
}

/*
 * select_sample - the user has clicked on a sample entry. We wait for
 * them to release the button before playing the sample and if they drag
 * whilst doing this we track their actions, scrolling as necessary.
 */
void
select_sample(int button)
{
	short obj;
	MRETS mk;
	struct samp_list *p;
	int i;

	obj=button;
	do
	{
		/*
		 * If we have a valid selection (always true first time)
		 */
		if (obj>=bell_text1 && obj<=bell_text6)
		{
			if (bell_box[obj].ob_flags&TOUCHEXIT)
				button=obj;
		}
		else if (mk.x>=bell_box[ROOT].ob_x &&
		  mk.x<bell_box[ROOT].ob_x+bell_box[bell_scroll].ob_width)
		{
			/*
			 * We are inside an invisible vertical box which exists
			 * above and below valid entries. Inside this region we
			 * scroll the box to reveal more entries.
			 */
			obj=0;							// clear redraw slider flag
			objc_xywh(bell_box,bell_slider,&clip);

			if (mk.y<bell_box[ROOT].ob_y)	// is mouse above the box ?
			{
				if (cur_item>0)				// if we're not at the top
				{
					cur_item--;				// move up an item
					xcpb->Sl_y(bell_box, bell_bar, bell_slider,
					  cur_item, max_samples, 0, draw_bell);
					/*
					 * Pretend its a click on the top object
					 */
					button=bell_text1;
					obj=1;					// and set redraw flag
				}
			}
			else if (mk.y>bell_box[ROOT].ob_y+bell_box[bell_scroll].ob_height)
			{
				/* else if mouse is below box */
				if (cur_item<max_samples)	// if not already at the bottom
				{
					cur_item++;				// move down an item
					xcpb->Sl_y(bell_box, bell_bar, bell_slider,
					  cur_item, max_samples, 0, draw_bell);
					/*
					 * Pretend its a click on the bottom object
					 */
					button=bell_text6;
					obj=1;					// and set redraw flag
				}
			}

			/*
			 * If the redraw flag is set then redraw the box. We assume
			 * that this is almost always overlapping and rather than
			 * undrawing the old slider and redrawing the new one, we
			 * optimise into a single redraw region formed from the
			 * union of the old and new positions.
			 */
			if (obj)
			{
				objc_xywh(bell_box,bell_slider,&clip2);
				rc_union(&clip2,&clip);
				objc_draw(bell_box,ROOT,MAX_DEPTH,ELTS(clip));
			}
		}

		/*
		 * Decide which text string the click is actually on
		 */
		obj=button-bell_text1;
		p=first;
		while (p && obj--)
			p=p->next;

		/*
		 * If we're clicking on something which isn't the current item
		 * we must change the highlighting.
		 */
		if (p!=current_sound)
		{
			/*
			 * Scan through the items to locate the currently selected
			 * one (if any - it may be 'off screen')
			 */
			for (i=bell_text1; i<=bell_text6; i++)
			{
				if (bell_box[i].ob_state&SELECTED)
				{
					objc_xywh(bell_box,i,&clip);
					objc_change(bell_box,i,0,ELTS(clip),0,1);
					break;
				}
			}

			/*
			 * Change the selected status of the new current item.
			 */
			objc_xywh(bell_box,button,&clip);
			objc_change(bell_box,button,0,ELTS(clip),SELECTED,1);
			current_sound=p;
		}

		/*
		 * A loop to hover over the currently selected item, we wait
		 * until either a move has occurred to another item, or the user
		 * has released the button.
		 */
		do
		{
			graf_mkstate(&mk.x, &mk.y, &mk.buttons, &mk.kstate);
			obj=objc_find(bell_box,ROOT,MAX_DEPTH,mk.x,mk.y);
		} while (button==obj && (mk.buttons&1));
	} while (mk.buttons&1);

	current_sound=p;	// update the current sound - Belt & Braces ?

	/*
	 * Copy the information from the sample list entry to the current
	 * configuration block.
	 */
	strcpy(config.snd_cur,p->name);
	config.snd_rate=p->h.rate;
	config.snd_length=p->h.length;

	/*
	 * We're going to play the sample now, as this may take some time we
	 * switch to a busy rodent for the duration.
	 */
	xcpb->MFsave(MFSAVE, &Mbuffer);
	graf_mouse(BUSY_BEE, NULL);

	/*
	 * If the name is empty then we're looking at the standard bell and
	 * don't need to load anything up.
	 */
	if (p->name[0])
	{
		/*
		 * CPXs should not allocate memory, sadly we need it here so we
		 * can preserve the contents of the existing buffer for the
		 * purpose of cancelling. Since we free it immediately
		 * afterwards with no intervening AES calls we should be safe.
		 */
		sound_buf = mxalloc ? Mxalloc(p->h.length,0) : Malloc(p->h.length);
		if ((long)sound_buf <= 0)
		{
			xcpb->XGen_Alert(MEM_ERR);
			goto nomem;					// was that a goto !!!
		}
		else
			load_sample(p->name,p->h.length);
	}
	else
		sound_buf=NULL;

	/*
	 * Play the sample, then wait for it to finish. The routines which
	 * do this must be called at the Supervisor level as they play with
	 * the hardware.
	 */
	Supexec(play_sound);
	Supexec(wait_sample);

	/*
	 * We must release the memory as quickly as possible so that there
	 * is no danger of it becoming lost as a result of a resolution
	 * change etc.
	 */
	release_buffer();

nomem:
	xcpb->MFsave(MFRESTORE, &Mbuffer);
}

/*
 * cpx_call - the main CPX driver entry point. We spend most of our time
 * in here dispatching the events from Xform_do which it doesn't want to
 * handle itself.
 */
int __stdargs __saveds
cpx_call(GRECT *rect)
{
	short button;
	int quit=0;
	GRECT rrect;

	/*
	 * Try to find the cookie describing the configuration and sample
	 * buffer. If its not around then get out'a here quick!
	 */
	if (!xcpb->getcookie(HSBL_COOKIE,(long *)&cookie))
	{
		form_alert(1,snd_tsrmissing);
		return 0;
	}

	/*
	 * Find the desktop area, we need this when blitting to ensure that
	 * everything is on screen.
	 */
	wind_get(DESK, WF_WORKXYWH, &desk.g_x, &desk.g_y, &desk.g_w, &desk.g_h);

	/*
	 * We copy the cookie to a private buffer. This ensures that we can
	 * always implement proper cancels.
	 */
	config=*cookie;

	/*
	 * Initialise location of form within CPX window
	 */
	bell_box[ROOT].ob_x = rect->g_x;
	bell_box[ROOT].ob_y = rect->g_y;

	/*
	 * Fill in contents of scrolling TEDINFO holding sample path
	 */
	fill_ted(((TEDINFO *)bell_box[bell_sndpath].ob_spec)->te_ptext,
	  ((TEDINFO *)bell_box[bell_sndpath].ob_spec)->te_txtlen,
	  0,config.snd_path);

	xcpb->MFsave(MFSAVE, &Mbuffer);
	graf_mouse(BUSY_BEE, NULL);
	get_flist(rect, &rrect);		// read the file list from disk

	rc_union(rect, &rrect);			// merge get_flist()s redraw with ours
	objc_draw(bell_box, ROOT, MAX_DEPTH, ELTS(rrect));
	xcpb->MFsave(MFRESTORE, &Mbuffer);

	do
	{
		MRETS mk;
		short ox, oy;
		int double_click;
		short msg[8];

		/*
		 * Sit around waiting for a message
		 */
		button = xcpb->Xform_do(bell_box, 0, msg);

		/* Check if we have a double click item */
		if ((button != -1) && (button & 0x8000))
		{
			double_click = 1;
			button &= 0x7FFF;
		}
		else
			double_click=0;

		/*
		 * If it wasn't a button then try to turn it into one.
		 */
		if (button == -1)
		{
			switch (msg[0])
			{
				case AC_CLOSE:				// ac_close means cancel
					button=bell_btcancel;
					break;

				case WM_CLOSED:				// wm_close means ok
					button=bell_btok;
					break;
			}
		}

		switch (button)
		{
			case bell_btok:
				update_current();			// update the current settings
				/* falls thru' */

			case bell_btcancel:
				quit = 1;					// give up at last
				bell_box[button].ob_state&=~SELECTED;
				break;

			case bell_btsave:
				/*
				 * Save the defaults; we give them one last chance as
				 * we're about to start writing to disk files.
				 */
				if (xcpb->XGen_Alert(SAVE_DEFAULTS))
				{
					xcpb->MFsave(MFSAVE, &Mbuffer);
					if (save_defaults())
						update_current();	// didn't cancel at last moment
					xcpb->MFsave(MFRESTORE, &Mbuffer);
				}

				/*
				 * Redraw the save button which is still selected.
				 */
				objc_xywh(bell_box,bell_btsave,&clip);
				objc_change(bell_box,bell_btsave,0,ELTS(clip),0,1);
				break;

			case bell_uparrow:
			case bell_dnarrow:
				/*
				 * User is manipulating one of the arrow keys, so call
				 * the XControl arrow handling code.
				 */
				xcpb->Sl_arrow(bell_box, bell_bar, bell_slider, button,
				  button==bell_uparrow?-1:1,
				  max_samples, 0, &cur_item, VERTICAL, draw_bell);
				break;

			case bell_bar:
				/*
				 * This is a click on the bar behind the slider, i.e. a
				 * page up or page down request.
				 */
				graf_mkstate(&mk.x, &mk.y, &mk.buttons, &mk.kstate);
				objc_offset(bell_box, bell_slider, &ox, &oy);

				/*
				 * Decide whether it was up or down and move by the
				 * number of lines in a window less 1, this ensures that
				 * you can always see what was at the limit of the
				 * window before the action.
				 */
				ox = (mk.y < oy) ? -(BELL_NLINES-1) : (BELL_NLINES-1);
				xcpb->Sl_arrow(bell_box, bell_bar, bell_slider, -1, ox,
				  max_samples, 0, &cur_item, VERTICAL, draw_bell);
				break;

			case bell_slider:
				/*
				 * Slider is being dragged, again just call the XControl
				 * routine to do most of the work.
				 */
				xcpb->MFsave(MFSAVE, &Mbuffer);
				graf_mouse(FLAT_HAND, NULL);
				xcpb->Sl_dragy(bell_box, bell_bar, bell_slider,
				  max_samples, 0, &cur_item, draw_bell);
				xcpb->MFsave(MFRESTORE, &Mbuffer);
				break;

			case bell_sndpath:
				/*
				 * This is a click on the TEDINFO which shows the sample
				 * directory. This means that they want to change it,
				 * hence we build a file selector with the current path
				 * and let them change it as they wish.
				 */
				strcpy(fselpath,config.snd_path);
				fselname[0]='\0';
				xcpb->MFsave(MFSAVE, &Mbuffer);
				if (fsel_exinput(fselpath,fselname,&button,snd_fsel) && button)
				{
					struct samp_list *p,*q;

					graf_mouse(BUSY_BEE, NULL);
					/*
					 * We got here, so we should have a new pathname in
					 * fselpath which we copy to our config area.
					 */
					strcpy(config.snd_path,fselpath);

					/*
					 * We must release the memory which we malloc'ed
					 * before, since we have it in strictly limited
					 * quantities.
					 */
					for (p=root; p; p=q)
					{
						q=p->next;
						free(p);
					}

					/*
					 * Read the new file list
					 */
					get_flist(rect, &rrect);
					fill_ted(((TEDINFO *)bell_box[bell_sndpath].ob_spec)->te_ptext,
					  ((TEDINFO *)bell_box[bell_sndpath].ob_spec)->te_txtlen,
					  0,config.snd_path);
					objc_draw(bell_box, ROOT, MAX_DEPTH, ELTS(rrect));
				}
				xcpb->MFsave(MFRESTORE, &Mbuffer);
				break;

			case bell_sndlfarrow:
			case bell_sndrtarrow:
				/*
				 * Scroll the TEDINFO window so that more/less is revealed.
				 */
				ted_window(bell_box,bell_sndpath,button==bell_sndlfarrow?-1:1,
				  config.snd_path);
				break;

			case bell_text1:
			case bell_text2:
			case bell_text3:
			case bell_text4:
			case bell_text5:
			case bell_text6:
				/*
				 * If the user has double clicked then we let them
				 * reconfigure the entry they are manipulating,
				 * otherwise we just go off and play the thing.
				 */
				if (double_click)
					reconfigure(button,rect);
				else
					select_sample(button);
				break;
		}
	} while (!quit);

	if (sound_buf)
		Mfree(sound_buf);

	return 0;
}

/*
 * cpx_init - main entry to the CPX, we arrive here when called either
 * during boot up or at the users request.
 */
CPXINFO * __stdargs __saveds
cpx_init(XCPB * Xcpb)
{
	xcpb = Xcpb;

	/*
	 * If we are booting then we must try and load the sample which the
	 * user has nominated as the default one. We are explicitly silent
	 * about errors as we know nothing of the current status of the machine.
	 */
	if (xcpb->booting)
	{
		/*
		 * Try to find our cookie
		 */
		if (xcpb->getcookie(HSBL_COOKIE,(long *)&cookie))
		{
			if (cookie->snd_cur[0])			// not standard bell
			{
				int fd;

				config=*cookie;				// copy cookie to config

				/*
				 * Open the file, this uses config.snd_path, hence why
				 * we bothered to copy it at all.
				 */
				fd=open_file(config.snd_cur,FO_READ);
				if (fd>=0)
				{
					/*
					 * No error opening, so read in the sample to the buffer.
					 */
					Fseek(sizeof(avr_t),fd,FSEEK_SET);
					Fread(fd,config.snd_length,cookie+1);
					Fclose(fd);
					*cookie=config;
				}
				else
					cookie->snd_cur[0]=0;	// error so reset to standard bell
			}
		}
		return (CPXINFO *) 1;				// indicate we want to keep going
	}
	else
	{
		static CPXINFO cpxinfo={cpx_call};

		appl_init();						// initialise private tables

		/*
		 * We don't have a RCS2 style resource file which needs fixing
		 * since DERCS has already done the work. However we do need to
		 * insert the heap we allocated into the malloc'able areas,
		 * hence the call to addheap(). We must explicitly set the
		 * SkipRshFix flag as no-one else is going to do it for us.
		 */
		if(!xcpb->SkipRshFix)
		{
			static long heap[16384/sizeof(long)];	// heap space for malloc()

			mxalloc = xcpb->getcookie('_FRB', NULL);
			xcpb->SkipRshFix=1;
			_addheap(heap,sizeof(heap));	// must only do this once
		}
		return &cpxinfo;
	}
}
