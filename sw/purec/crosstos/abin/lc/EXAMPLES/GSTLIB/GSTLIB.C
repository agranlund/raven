/* lc -w -v -csfm -Lav -O -M gstlib.c */

/* librarian for GST .bin files 	(c) k.b.v. controls 02 jun 1989
 * read existing archive into memory, and build list of modules
 * process arguments, and modify the archive list
 * output a ranlib consisting of comment items
 * output the modules on the list to tmp file.  if ok unlink old and rename
 * we should be able to sort archive list into single pass order as default.
 * if we use a|b modifiers, the sorting is not done.
 * we will do everything in memory,  requiring approx 100k for typical archive.
 *
 * we must have a module name entry in the .bin file else we have problems
 * Prospero libraries have wierd module name entries.
 * if we convert to a more reasonable name in our MODULE structure 
 * then we can continue as per Unix.  
 * ie convert to "WIERD blah blah" to "wierd.bin"
 * but "normal.ext" remains as "normal.ext" 
 *
 */

#include <stdio.h>
#include <osbind.h>
#ifdef LATTICE
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#define lmalloc(n) malloc(n)
#endif

/* #define DEBUG */
#define JAN011980	((1 << 5) | 1)
#define UPDATE  1
#define LOAD	2
#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

/*
 * Modes, a la GEM-DOS:
 */
#define S_IJRON	0x01			/* Read-only */
#define S_IJHID	0x02			/* Hidden from search */
#define S_IJSYS	0x04			/* System, hidden from search */
#define S_IJVOL	0x08			/* Volume label in first 11 bytes */
#define S_IJDIR	0x10			/* Directory */
#define S_IJWAC	0x20			/* Written to and closed */

typedef struct {
	char	d_glob[12];	/* GEMDOS wildcard string from Fsfirst */
	char	d_mask;		/* Attribute mask from Fsfirst */
	char	d_dirent[4];	/* Offset into directory, MSB first */
	char	d_dirid[4];	/* Pointer to directory structure in GEM mem */
	char	d_fattr;	/* File attributes */
	long	d_tandd;	/* Time and date words */
	long	d_fsize;	/* File size */
	char 	d_fname[14];	/* File name */
} DMABUFFER;

typedef struct	tm {
	int	tm_sec;		/* Seconds (0-59) */
	int	tm_min;		/* Minutes (0-59) */
	int	tm_hour;	/* Hours (0-23); 0 = midnight */
	int	tm_mday;	/* Day of month (1..28,29,30,31) */
	int	tm_mon;		/* Month (0-11); 0 = January */
	int	tm_year;	/* Year AD since 1900 */
	int	tm_wday;	/* Day of week (0-6): 0 = Sunday */
	int	tm_yday;	/* Day of year (0-365,366) */
	int	tm_isdst;	/* Daylight savings time flag: */
				/* Non-standard, they make negative==NA */
} tm_t;

typedef unsigned long tand_t;	/* we use disk format for time */

#ifndef NARGS
tand_t tm_to_tand(tm_t *);
tm_t *tand_to_tm(tand_t);
void mytime(tand_t *);
char *myctime(tand_t *);
#else
tand_t tm_to_tand();
tm_t *tand_to_tm();
void mytime();
char *myctime();
#endif

typedef union any_ptr {
	char *name;
	struct mod_info *module;
	struct cref_info *cref;
} PTR;

typedef struct list {
	struct list *next;
	PTR p;
} LIST;

typedef struct mod_info {
	char *name;
	tand_t date;
	long size;
	unsigned char *buf;
	LIST *xdef;
	LIST *xref;
} MODULE;

typedef struct cref_info {
	char *name;
	LIST *xdef;
	LIST *xref;
} CREF;

#define MALLOC(x)	(x *)malloc(sizeof(x))
#define AFTER 	1
#define BEFORE	2
#define TRUE	1
#define VOIDP   char *		/* really want a void * */
LIST *libhead = NULL;
LIST *xrefhead = NULL;
LIST *beforemod = NULL;
LIST *ranlib = NULL;
char *aftmodname;

extern int errno;

int verbose, aftbef, sortflag, keepflag, quitflag;

#ifndef LATTICE
char *calloc(), *malloc(), *realloc(), *strrchr(), *strchr();
#endif

#define addlist(x, y) inslist(x, y, NULL)
#ifndef NARGS
void inslist(LIST **, VOIDP, LIST *); /* want to check for ptr argument */
void dellist(LIST **, LIST *);
void mkxrefs(LIST *, LIST **);
void prtxrefs(LIST *);
void prtlist(LIST *, char *);
int sortxrefs(LIST **, LIST *);
int weneed(LIST *, MODULE *);
int weload(LIST **, LIST **, MODULE *);
int mkcode(int, char *, char *, int);
void fatal(char *, char *);
void faterr(char *, char *);
MODULE *gstread(unsigned char *, unsigned char *);
MODULE *mkranlib(LIST *, char *);
tand_t rdranlib(MODULE *, char *);
LIST *msearch(LIST *, char *);
LIST *nsearch(LIST *, char *);
CREF *addxsym(LIST **, char *);
char *newstring(char *, int);
char *cleannm(char *);
char *strlower(char *);
char *unwierd(char *, int);
#else
void fatal();
void faterr();
void inslist();
void dellist();
void mkxrefs();
void prtxrefs();
void prtlist();
int sortxrefs();
int weneed();
int weload();
int mkcode();
MODULE *gstread();
MODULE *mkranlib();
tand_t rdranlib();
LIST *msearch();
LIST *nsearch();
CREF *addxsym();
char *newstring();
char *strlower();
char *cleannm();
char *unwierd();
#endif

void usage()
{
	fprintf(stderr, 
		"Librarian for GST .bin object files (c) kbv controls 1989\n");
	fprintf(stderr, 
		"Usage: gstlib {dlmrtux}[vsk][a|b obmod] library [files..]\n");
	fprintf(stderr, "\toptions\td delete modules\n");
	fprintf(stderr, "\t\tm move modules\n");
	fprintf(stderr, "\t\tr replace modules\n");
	fprintf(stderr, "\t\tu update modules\n");
	fprintf(stderr, "\t\tl load modules\n");
	fprintf(stderr, "\t\tt table [modules..]\n");
	fprintf(stderr, "\t\tx extract [modules..]\n");
	fprintf(stderr, "\t\modifiers\tv verbose\n");
	fprintf(stderr, "\t\t\ta|b obmod with m|r replace after|before obmod\n");
	fprintf(stderr, "\t\t\ts sort library for single pass\n");
	fprintf(stderr, "\t\t\tk keep module datestamp when extracting\n");
	fprintf(stderr, "\tall modified libraries will have a ranlib\n");
	exit(1);
}

int main(argc, argv)
int argc;
char *argv[];
{
    char **myargv;
    char inpbuf[160], *p, *q, *r;
    int myargc = 0;
    int i, j;
    FILE *fp;
    myargv = NULL;
    for (i = 1; i < argc; i++) {
    	strlower(argv[i]);
	if (argv[i][0] == '@') {
	    if ((myargv = (char **)calloc(sizeof(char *), 200)) == NULL)
	    	fatal("exparg", "duff calloc");  
	    break;
	}
    }
    if (myargv) {
    	for (i = 0; i < argc; i++) {
	    if (argv[i][0] == '@') {
	        if ((fp = fopen(argv[i] + 1, "r")) == NULL) 
		    fatal("exparg", argv[i]);
	 	while (fgets(inpbuf, 160, fp) != NULL) {
		    p = inpbuf;
		    while (*p) {
		        while (*p == ' ' || *p == '\t' || *p == '\n') p++;
		        if (*p == '\0' || *p == '#') break;
			if (*p == '\\' && *(p+1) == '\n') break;
			q = p;
			for (j = 0; *p && *p != ' ' && *p != '\t' && *p != '\n'; j++,p++) ;
			if (j) {
			    if ((r = malloc(j+1)) == NULL)
			        fatal("exparg", "duff malloc");
			    strncpy(r, q, j);
			    r[j] = '\0';
			    myargv[myargc++] = r;
			}
		    }
		}
		fclose(fp);
	    }	
	    else myargv[myargc++] = argv[i];
	    if (myargc > 199) fatal("exparg", "too many args"); 
	}
    	myargv[myargc] = NULL;
	return (mymain(myargc, myargv));
    }
    return (mymain(argc, argv));
}

int mymain(argc, argv)
int argc;
char **argv;
{
	char *libname, *cmd, *p;
	int ret = 2;
	int fd, keepran, exist = 1;
	setbuf(stdout, NULL);

	if (argc < 3) 
		usage();
	p = cmd = *++argv;
	switch (*cmd) {
		case 'r':
		case 'u': exist = 0;
		case 'd':
		case 'm':
		case 'l': keepran = 0; break;
		case 't':
		case 'x': keepran = 1; break;
		default:
			fatal("Usage",
			 "gstlib {dmrutx}[vsk][a|b modname] archive files..");
			break;
	}
	while (*++p) {
		switch (*p) {
		case 'a': aftbef |= AFTER; break;
		case 'b': aftbef |= BEFORE; break;
		case 'v': verbose++; break;
		case 's': sortflag = TRUE; break;
		case 'k': keepflag = TRUE; break;
		case 'q': quitflag = TRUE; break;
		default: usage(); break;
		}
	}
	if (aftbef) aftmodname = *++argv;
	libname = *++argv;
	if ((fd = open(libname, O_RAW)) > 0) close(fd);
	else {
		if (exist) fatal(libname, "library does not exist");
		else if (verbose) 
			fprintf(stderr, "creating new library %s\n", libname);
	}
	if (fd > 0) readlib(&libhead, libname, keepran);
	if (aftbef) {
		if (*cmd != 'r' || *cmd != 'm' || *cmd != 'u') 
			fatal("a or b modifiers only with r or m option", cmd);
		if ((beforemod = msearch(libhead, aftmodname)) == NULL)
			fatal(aftmodname, "not in library");
		if (aftbef & AFTER) {
			beforemod = beforemod->next;
		}
	}
	else beforemod = NULL;
	switch (*cmd) {
		case 'd': ret = delete(libname, ++argv); break;
		case 'l': ret = replace(libname, ++argv, LOAD); break;
		case 'm': ret = move(libname, ++argv); break;
		case 'r': ret = replace(libname, ++argv, 0); break;
		case 't': ret = table(libname, ++argv); break;
		case 'u': ret = replace(libname, ++argv, UPDATE); break;
		case 'x': ret = extract(libname, ++argv); break;
	}
	myexit(ret);
}

myexit(ret)
int ret;
{
	if (ferror(stdout)) {
		fprintf(stderr, "stream error on <stdout>\n");
		ret |= 1;
	}
	if (quitflag) {
		fprintf(stderr, "press return to continue\n");
		getchar();
	}
	exit(ret);
}

/* print the table.
 * if (verbose == 0) just the modnames
 * if (verbose >  0) we do size and datestamp
 * if (verbose >  1) we do xdefs (and xrefs)
 * if (verbose >  3) we do a cross-reference of symbols too.
 */
int table(libname, namelist)
char *libname, **namelist;
{
	register LIST *l;
	register MODULE *m;
	LIST *xlisthead = NULL;
	LIST *tablehead = NULL;
	register char *name;
	int all = 1;
	name = NULL;
	if (*namelist) all = 0;
	while (all || (name = *namelist++)) {
		for (l = libhead; l != NULL; l = l->next) {
			m = l->p.module;
			if ((name != NULL) && (strcmp(name, m->name) != 0)) 
				continue;
			addlist(&tablehead, (VOIDP)m);
			if (verbose) {
				printf("%-16.16s size:%5ld date:%s", 
					m->name, m->size, myctime(&m->date));
			}
			else printf("%s\n", m->name);
			if (verbose > 1)
				prtlist(m->xdef, " xdef:");
			if (verbose > 2)
				prtlist(m->xref, "  xref:");
			if (all = 0) break;
		}
		all = 0;
	} 
	if (verbose > 3) {
		mkxrefs(tablehead, &xlisthead);
		prtxrefs(xlisthead);
	}
	return 0;
}

/* replace the modules found in the file, (it may have multiple mods)
 * if update mode is set, we only replace out of date modules
 */
replace(libname, namelist, mode)
char *libname, **namelist;
int mode;
{
	char *name; 
	register LIST *l, *xl;
	LIST *onemod, *resolved = NULL, *unres = NULL;
	register MODULE *m;
	tand_t old, new;
	int count = 0, ret = 0;
	if (mode & LOAD)
		for (l = libhead; l; l = l->next) 
			if (weload(&resolved, &unres, l->p.module)) ret |= 1;
	while (name = *namelist++) {
		onemod = NULL;
		readlib(&onemod, name, 0);
		for (l = onemod; l; l = l->next) {
			m = l->p.module;
			new = m->date;
			new = (new >> 16) | (new << 16);
			if (mode & LOAD) {
				if (weneed(unres, m) == 0) continue;
				if (weload(&resolved, &unres, m)) ret |= 1;
			}
			if (xl = msearch(libhead, m->name)) {
				old = xl->p.module->date;
				old = (old >> 16) | (old << 16);
				if (!(mode & UPDATE) || old < new) {
					xl->p.module = l->p.module;
					if (verbose) 
						printf("r %s\n", m->name);
					count++;
				}
			}
			else {
				inslist(&libhead, (VOIDP)l->p.module, beforemod);
				if (verbose) printf("a %s\n", m->name);
				count++;
			}
		}
	}
	if (verbose > 1 && (mode & LOAD))
		prtlist(unres, "unresolved:");
	if (ret) return ret;
	if ((mode & UPDATE) && (count == 0)) return 0;
	return (copyback(libhead, libname));
}

move(libname, namelist)
char *libname, **namelist;
{
	char *name; 
	LIST *l;
	MODULE *m;
	while (name = *namelist++) {
		if ((l = msearch(libhead, name)) == NULL) {
			printf("move: %s is not in library", name);
		}
		else {
			m = l->p.module;
			dellist(&libhead, l);		
			inslist(&libhead, (VOIDP)m, beforemod);
			if (verbose) printf("m %s\n", m->name);
		}
	}
	return (copyback(libhead, libname));
}

delete(libname, namelist)
char *libname, **namelist;
{
	char *name; 
	LIST *l;
	int ret = 0;
	if (*namelist == NULL) return -1;
	while (name = *namelist++) {
		if (l = msearch(libhead, name)) {
			dellist(&libhead, l);
			if (verbose) printf("d %s\n", name);
		}
		else {
			printf("delete: %s not in archive\n", name);
			ret |= 1;
		}
	}
	if (ret) return(ret);
	return (copyback(libhead, libname));
}

extract(libname, namelist)
char *libname, **namelist;
{
	char *name; 
	char tmpname[80];
	LIST *l, *xl, onemod;
	MODULE *m;
	int dummy = 0;
	int ret = 0;
	if (*namelist == NULL) xl = libhead;
	else xl = NULL;
	for (;;) {
		if (xl == NULL) name = *namelist++;
		else {
			m = xl->p.module;
			xl = xl->next;
			name = m->name;
		}
		if (name == NULL) break;
		if (l = msearch(libhead, name)) {
			m = l->p.module;
			strcpy(tmpname, m->name);
			if (*tmpname == '\0') 
				sprintf(tmpname, "dummy%03d.bin", ++dummy);
			if (verbose) printf("x %s\n", tmpname);
			onemod.p.module = l->p.module;
			onemod.next = NULL;
			writemod(&onemod, tmpname, keepflag);
		}
		else {
			printf("extract: %s not in %s\n", name, libname);
			ret |= 1;
		}
	}
	return ret;
}

/* read a file into memory, and append details to listp
 * if a single module with no ranlib we use the file datestamp else jan 1 1970
 * if the first module is "ranlib" then we re-adjust date fields from there
 * and eliminate "ranlib" from our enquiries.
 */
int readlib(listp, name, keepran)
LIST **listp;
char *name;
int keepran;
{
	DMABUFFER filestat;
	long count;
	int chunk;
	tand_t t;
	unsigned char *mp, *p, *endp;
	MODULE *m, *ranmod = NULL;
	LIST *l;
	int fd;
	if (mystat(name, &filestat) < 0) faterr("cannot mystat", name);
	if ((mp = lmalloc(filestat.d_fsize)) == NULL)
		fatal("readlib", "lmalloc");
	if ((fd = open(name, O_RAW)) < 0) faterr("cant open", name);
	for (p = mp, count = filestat.d_fsize; count > 0L; p += chunk) {
		chunk = (count > 4096L) ? 4096 : count;
		if (read(fd, p, chunk) != chunk) faterr("read error", name);
		count -= chunk;
	}
	close(fd);
	for (count = 0L, p = mp, endp = mp + filestat.d_fsize; p < endp; ) {
		if ((m = gstread(p, endp)) == NULL) fatal("duff module", name);
		m->date = JAN011980;
		addlist(listp, (VOIDP)m);
		count += m->size;
		p += m->size;
	}
	if (l = msearch(*listp, "ranlib.bin")) {
		ranmod = l->p.module;
		if (l != *listp) fatal("ranlib.bin", "is not at beginning");
		if (keepran == 0) dellist(listp, l);
		for (l = *listp; l; l = l->next) {
			m = l->p.module;
			t = rdranlib(ranmod, m->name);
			m->date = t;
			D(printf("module:%s %s", m->name, myctime(&m->date));)
		}
	} 
	else if ((l = *listp) && (l->next == NULL)) 
		l->p.module->date = filestat.d_tandd;
	return 0;
}

/* given a memory address, we read the memory to analyse the module.
 * if ok, we return the details in a freshly malloced MODULE structure
 * else we return NULL
 */
MODULE *gstread(mp, endp)
unsigned char *mp;
register unsigned char *endp;
{
	register unsigned char *p;
	register MODULE *m;
	register int len, byte;
	register short section;
	char *newname;
	if ((m = MALLOC(MODULE)) == NULL) fatal("gstread", "malloc");
	m->name = "";
	m->buf = mp;
	m->size = m->date = 0L;
	m->xdef = m->xref = NULL;
	p = mp;
	if (*p != 0xfb || *(p+1) != 0x01) return NULL;
	p += 2;
	len = *p++;
	newname = unwierd(p, len);
	m->name = newstring(newname, strlen(newname));
	p += len;
	while (p < endp) {
		if (*p++ != 0xfb) continue;
		switch (byte = *p++) {
		case 0xfb: break;
		case 0x01: m->size = (long)(p - mp -2); return m;
		case 0x02: len = *p++; p += len; break;
		case 0x03: p += 4; break;
		case 0x04: p += 2; break;
		case 0x05: p += 4; break;
		case 0x06:				/* XDEF */ 
			len = *p++;
			addlist(&m->xdef, (VOIDP)newstring(p, len));
			p += len;
			p += 4;				/* .kbv was missing */
			section = (*p << 8) + *(p+1);
			p += 2;
			break;
		case 0x07: 
			p += 5;
			while (*p++ != 0xfb) p += 2;
			break;
		case 0x10:				/* XREF */
			section = (*p << 8) + *(p+1);
			p += 2;
			len = *p++;
			if (section >= 0) addlist(&m->xref, (VOIDP)newstring(p, len));
			p += len;
			break;
		case 0x12:
			p += 2;
			break;
		case 0x13: m->size = (long)(p - mp); return m;
		default:
			printf("escape 0xfb%02x in %s\n", byte, m->name);  
			fatal("gstread", "unknown object type");
			break;
		}
	}
	fatal("gstread", "unexpected EOF");
}

/* insert an item before list entry and update head if empty
 * if posn is NULL, then we are adding to the end of list
 * if posn is not in the list, we've had it
 */
void inslist(head, str, posn)
LIST **head;
VOIDP str;
register LIST *posn;
{
	register LIST *l, *new;
	if ((new = MALLOC(LIST)) == NULL) fatal("inslist", "malloc");
	new->next = posn;
	new->p.name = str;
	for (l = *head; l; l = l->next) {
		if (l->next == posn) {
			l->next = new;
			return;
		}
	}
	*head = new;
	return;
}

/* delete entry from list
 */
void dellist(head, lentry)
LIST **head;
LIST *lentry;
{
	register LIST *l;
	if (lentry == *head) *head = lentry->next; 
	else {
		for (l = *head; l->next; l = l->next) {
			if (l->next == lentry) {
				l->next = lentry->next;
				break;
			}
		}
	}
	free(lentry);
}

int weneed(reflist, newmod)
register LIST *reflist;
register MODULE *newmod;
{
	register LIST *l;
	if (reflist == NULL) return 0;
	for (l = newmod->xdef; l; l = l->next)
		if (nsearch(reflist, l->p.name)) return 1;
	return 0;
}

/* merge the new module's symbols into our lists
 * if multiple xdefs then shout
 */
int weload(pdeflist, preflist, newmod)
MODULE *newmod;
LIST **pdeflist, **preflist;
{
	register LIST *l, *xl;
	register char *name;
	int ret = 0;
	for (l = newmod->xdef; l; l = l->next) {
		name = l->p.name;
		if (xl = nsearch(*preflist, name))
			dellist(preflist, xl);
		if (nsearch(*pdeflist, name)) {
			ret |= 1;
			printf("%s is multiply defined in %s\n", 
				name, newmod->name);
		}
		else addlist(pdeflist, name);
	}
	for (l = newmod->xref; l; l = l->next) {
		name = l->p.name;
		if (nsearch(*pdeflist, name)) continue;
		if (nsearch(*preflist, name) == NULL)
			addlist(preflist, name);
	}
	return ret;
}

/* build an xref list from the module list
 */
void mkxrefs(list, xlist)
LIST *list;
LIST **xlist;
{
	LIST *modlist;
	register LIST *l;
	register MODULE *m;
	register CREF *xr;
	char *symbol, *modname;
	
	for (modlist = list; modlist; modlist = modlist->next) {
		m = modlist->p.module;
		modname = m->name;
		for (l = m->xdef; l; l = l->next) {
			symbol = l->p.name;
			xr = addxsym(xlist, symbol);
			addlist(&xr->xdef, (VOIDP)modname);
		}
		for (l = m->xref; l; l = l->next) {
			symbol = l->p.name;
			xr = addxsym(xlist, symbol);
			addlist(&xr->xref, (VOIDP)modname);
		}
	}
}

/* display the xref list
 */
void prtxrefs(xlist)
LIST *xlist;
{
	register LIST *l, *xl;
	register CREF *xr;
	for (l = xlist; l; l = l->next) {
		xr = l->p.cref;
		printf("symbol:%s", xr->name);
		if (xl = xr->xdef) {
			printf("\txdef:");
			for ( ; xl; xl = xl->next) printf(" %s", xl->p.name);
		}
		printf("\n");
		prtlist(xr->xref, "  xref:");
	}
}

void prtlist(list, title)
register LIST *list;
char *title;
{
	register LIST *l;
	char buf[256];
	l = list;
	while (l) {
		strcpy(buf, title);
		for (; l; l = l->next) {
			strcat(buf, " ");
			strcat(buf, l->p.name);
			if (strlen(buf) > 60) break;
		}
		printf("%s\n", buf);
	}
}

/* we go thru the whole list of xdef'd symbols.
 * if it is possible to find any module that contains xref to symbol
 * we move the xdef MODULE info in the main list to after the xref MODULE
 * we give up if we make moves but list in unchanged.
 * we give up if we have 50 attempts to sort list.
 */
int sortxrefs(list, xlist)
LIST **list;
LIST *xlist;
{
	register LIST *l, *xl;
	LIST *xrefl, *refpsn, *defpsn, *copyhead;
	MODULE *m;
	register CREF *xr;
	register int exchanges, changed, attempts;
	char *modname;
	
	copyhead = NULL;
	for (l = *list; l; l = l->next) 
		addlist(&copyhead, (VOIDP)l);
	for (attempts = 0; attempts < 50; attempts++) {
	    exchanges = 0;
	    for (l = *list, xl = copyhead; l; l = l->next, xl = xl->next)
		xl->p.module = l->p.module;
	    for (l = xlist; l; l = l->next) {
		xr = l->p.cref;
		for (xl = xr->xdef; xl; xl = xl->next) {
			defpsn = msearch(*list, xl->p.name);
			modname = xl->p.name;
			for (xrefl = xr->xref; xrefl; xrefl = xrefl->next) {
				if (refpsn = msearch(defpsn, xrefl->p.name)) {
					D(printf("moving %s after %s\n", modname, xrefl->p.name);)
					m = defpsn->p.module;
					dellist(list, defpsn);
					inslist(list, (VOIDP)m, refpsn->next); 
					defpsn = refpsn->next;
					exchanges++;
				}
			}
		}  
	    }
	    if (verbose > 3)
	    	printf("attempt :%d moves done :%d\n", attempts, exchanges);
	    if (exchanges == 0) return 0;
	    for (l = *list, xl = copyhead, changed = 0; l; l = l->next, xl = xl->next) {
		if (xl->p.module != l->p.module) {
			changed = 1;
			break;
		}
	    }
	    if (changed == 0) return 1;
	}
	return 1;
}

tand_t rdranlib(ranmod, name)
MODULE *ranmod;
char *name;
{
	register MODULE *m;
	register unsigned char *p;
	tm_t t;
	int byte, len;
	char buf[128];
	sprintf(buf,"module:%-16.16s", name);
	m = ranmod;
	p = m->buf;
	if (*p != 0xfb || *(p+1) != 0x01) return JAN011980;
	p += 2;
	len = *p++;
	if (strncmp(p, "ranlib.bin", len)) return JAN011980;
	p += len;
	for (;;) {
		if (*p++ != 0xfb) return 0L;
		switch (byte = *p++) {
		case 0x01: return JAN011980;
		case 0x02: 
			len = *p++; 
			if (strncmp(p, buf, strlen(buf)) == 0) {
				strncpy(buf, p, len);
				p = buf + 32;
				t.tm_mon  = atoi(p) - 1;    p += 3;
				t.tm_mday = atoi(p);        p += 3;
				t.tm_year = atoi(p) - 1900; p += 5;
				t.tm_hour = atoi(p);        p += 3;
				t.tm_min  = atoi(p);        p += 3;
				t.tm_sec  = atoi(p); 
				return (tm_to_tand(&t));
			}
			p += len; 
			break;
		case 0x13: return JAN011980;
		default:
			fatal("rdranlib", "unknown object type");
			break;
		}
	}
}

MODULE *mkranlib(list, name)
LIST *list;
char *name;
{
	register LIST *xl;
	register unsigned char *p;
	register MODULE *m;
	register int pass;
	MODULE *ranmod;
	LIST *l, onemod;
	tm_t *tp;
	char buf[1000], *mp = NULL;
	if ((ranmod = m = MALLOC(MODULE)) == NULL) fatal("mkranlib", "MALLOC");
	m->name = name;
	m->size = 0L;
	m->buf  = NULL;
	mytime(&m->date);
	m->xref = m->xdef = NULL;
	onemod.p.module = m;
	onemod.next = list;
	for (pass = 0; pass < 2; pass++) {
		p = mp;
		sprintf(buf, "%s", name);
		D(printf("mkranlib: %s\n", buf);)
		p += mkcode(0x01, p, buf, pass);
		for (l = &onemod; l; l = l->next) {
			m = l->p.module;
			tp = tand_to_tm(m->date);
			sprintf(buf,
		 	 "module:%-16.16s %8ld %02d %02d %04d %02d %02d %02d ",
				m->name, m->size,
				tp->tm_mon+1,
				tp->tm_mday,
				tp->tm_year+1900,
				tp->tm_hour,
				tp->tm_min,
				tp->tm_sec);
			D(printf("%s\n", buf);)
			p += mkcode(0x02, p, buf, pass);
			if (xl = m->xdef) {
more:
				strcpy(buf, "xdef:");
				for (; xl; xl = xl->next) {
					strcat(buf, " ");
					strcat(buf, xl->p.name);
					if (strlen(buf) > 60) break;
				}
				D(printf("%s\n", buf);)
				p += mkcode(0x02, p, buf, pass);
				if (xl) goto more;
			}
		}
		p += mkcode(0x13, p, NULL, pass);
		m = ranmod;
		m->size = (long)(p - mp);
		if (pass == 0)
			if ((p = mp = m->buf = malloc((unsigned)m->size)) == NULL) 
				fatal("mkranlib", "malloc");
	}
	D(printf("ranlib size: %ld\n", m->size);)
	return m;
}

int mkcode(code, mp, buf, doflag)
register char *mp, *buf;
register int code, doflag;
{
	register char *p;
	register int len;
	p = mp;
	if (doflag) {
		*p++ = 0xfb;
		*p++ = code;
	}
	else p += 2;
	if (buf != NULL) {
		len = strlen(buf);
		if (doflag) {
			*p++ = len;
			strncpy(p, buf, len);
		}
		else p++;
		p += len;
	}
	return (p - mp);
}

/* lookup a module name in LIST of modules
 */
LIST *msearch(list, name)
LIST *list;
char *name;
{
	register LIST *l;
	register MODULE *m;
	for (l = list; l ; l = l->next) {
		m = l->p.module;
		if (m && (strcmp(m->name, name) == 0)) return l;
	}
	return NULL;
}

/* lookup a name in LIST of names
 */
LIST *nsearch(list, name)
LIST *list;
register char *name;
{
	register LIST *l;
	for (l = list; l ; l = l->next) {
		if ((strcmp(l->p.name, name) == 0)) return l;
	}
	return NULL;
}

/* insert symbol into sorted list of xref symbols 
 * create a new node if reqd. return ptr to CREF
 */
CREF *addxsym(xlist, name)
LIST **xlist;
char *name;
{
	register LIST *l;
	register CREF *xr;
	register int cmp;
	for (l = *xlist; l ; l = l->next) {
		xr = l->p.cref;
		cmp = strcmp(name, xr->name);
		if (cmp == 0) return xr;
		if (cmp <  0) break;
	}
	if ((xr = MALLOC(CREF)) == NULL)
		fatal("addxsym", "MALLOC");
	xr->name = name;
	xr->xref = xr->xdef = NULL;
	inslist(xlist, (VOIDP)xr, l);
	return xr;
}

/* copy archive back to tmp file inserting "ranlib" module
 */
int copyback(liblist, libname)
LIST *liblist;
char *libname;
{
	int ret = 0;
	LIST *xlist, *list;
	char tempname[200], *p;
	strcpy(tempname, libname);
	if (p = strrchr(tempname, '\\')) p++;
	else p = tempname;
	strcpy(p, "libname.tmp");
	list = liblist;
	xlist = NULL;
	if (sortflag) {
		mkxrefs(list, &xlist);
		if (ret = sortxrefs(&list, xlist))
			fprintf(stderr, "unable to sort library %s\n", libname);
	}
	if (msearch(list, "ranlib.bin") == NULL)
		inslist(&list, (VOIDP)mkranlib(list, "ranlib.bin"), list);
	ret |= writemod(list, tempname, 0);
	if (ret == 0) {
		unlink(libname);
		if (rename(tempname, libname) != 0) faterr("rename", libname);
 	}
	return ret;
}

/* write a list of modules to name
 */
int writemod(list, name, touch)
LIST *list;
char *name;
int touch;
{
	register LIST *l;
	register MODULE *m;
	register int fd, chunk, ret;
	register long count;
	register unsigned char *p;
	
	if ((fd = creat(name, O_RAW)) < 0) faterr("cant creat", name);
	for (l = list; l != NULL; l = l->next) {
		m = l->p.module;
		for (count = m->size, p = m->buf; count > 0L; ) {
			chunk = (count > 4096L) ? 4096 : count;
			if (write(fd, p, chunk) != chunk) 
				faterr("write error", name);
			count -= chunk;
			p += chunk;
		}
	}
	ret = close(fd);
	if (touch) keepdate(name, m->date);
	return ret;
}
		
char *newstring(mp, len)
register char *mp;
int len;
{
	register char *dst, *str;
	if ((str = dst = malloc(len + 1)) == NULL) fatal("newstring", "malloc");
	while (len--) *dst++ = *mp++;
	*dst = '\0';
	D(printf("%s\n", str);)
	return str;
}
 	
/* make a normal lowercase filename 
 * with a .bin extension if none already
 */
char *unwierd(name, len)
char *name;
int len;
{
	register char *p, *dst;
	static char wierdbuf[200];
	for (p = name, dst = wierdbuf, *dst = '\0'; len > 0; len--, p++) {
		if (*p == ' ') break;
		*dst++ = (*p >= 'A' && *p <= 'Z') ? *p + ('a' - 'A') : *p;
	}
	*dst++ = '\0';
	*dst++ = '\0';
	if (strchr(wierdbuf+1, '.') == NULL) strcat(wierdbuf, ".bin");
	return wierdbuf;
}
	
char *strlower(s)
char *s;
{
	register char *p;
	for (p = s; *p; p++)
		*p = (*p >= 'A' && *p <= 'Z') ? *p + ('a' - 'A') : *p;
	return s;
}
	
char *cleannm(name)
char *name;
{
	register char *p;
	if (p = strrchr(name, '\\')) return p+1;
	if (p = strrchr(name, ':')) return p+1;
	return name;
}

void faterr(s, t)
char *s, *t;
{
	fprintf(stderr, "gstlib: Fatal error: %s %s\n", s, t);
	perror("Tos error");
	myexit(1);
}

void fatal(s, t)
char *s, *t;
{
	fprintf(stderr, "gstlib: Fatal error: %s %s\n", s, t);
	myexit(1);
}

#ifndef LATTICE
rename(old, new)
char *new, *old;
{
	int ret;
	ret = Frename(0, old, new);
	if (ret < 0) errno = -ret;
	return ret;
}
#endif

keepdate(name, date)
char *name;
tand_t date;
{
	int fd;
	if ((fd = Fopen(name, 0)) > 0) {
		Fdatime((short *)&date, fd, 1);
		Fclose(fd);
	}
	else {
		errno = -fd;
		perror(name);
	}
	return fd;
}

int mystat(fullname, statptr) 
char *fullname;
DMABUFFER *statptr;
{
	DMABUFFER *saved;
	char *name;
	int ret = 0;

	name = cleannm(fullname);
	saved = (DMABUFFER *)Fgetdta();
	Fsetdta(statptr);
	ret = Fsfirst(fullname, 0);
	if (ret < 0) {
		errno = -ret;
		ret = -1;
	}
/*	else if (strcmp(name, statptr->d_fname) != 0) ret = -1; */
	Fsetdta(saved);
	return ret;
}

tm_t *tand_to_tm(tandd)
register tand_t tandd;
{
	static tm_t tm;
	static int monbeg[2][12] = { 
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
		{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 306, 335 }
	};
	register unsigned time;
	register unsigned date;
	int leap;
	time = tandd >> 16;
	date = tandd & 0xffffL;
	tm.tm_sec = (time & 0x1f)<< 1;		/* Seconds (0-59) */
	tm.tm_min = (time >> 5) & 0x3f;	/* Minutes (0-59) */
	tm.tm_hour = (time >> 11) & 0x1f;	/* Hours (0-23); 0 = midnight */
	tm.tm_mday = ((date) & 0x1f);	/* Day of month (1..28,29,30,31) */
	tm.tm_mon = ((date >> 5) & 0x0f) - 1;	/* Month (0-11); 0 = January */
	tm.tm_year = ((date >> 9) & 0x3f)+80;	/* Year AD since 1900 */
	leap = ((tm.tm_year %4) == 0);
	tm.tm_yday = monbeg[leap][tm.tm_mon] + tm.tm_mday - 1;
	tm.tm_wday = (3 + tm.tm_year + (tm.tm_year << 2) + tm.tm_yday) % 7;
	return &tm;
}

tand_t tm_to_tand(tp)
register tm_t *tp;
{
	register tand_t tandd = 0L;
	tandd <<= 5; tandd |= (tp->tm_hour);		
	tandd <<= 6; tandd |= (tp->tm_min);
	tandd <<= 5; tandd |= (tp->tm_sec >> 1);
	tandd <<= 7; tandd |= (tp->tm_year - 80);
	tandd <<= 4; tandd |= (tp->tm_mon + 1);
	tandd <<= 5; tandd |= (tp->tm_mday);
	return tandd;
}

void mytime(tandp)
tand_t *tandp;
{
	*tandp = Tgettime();
	*tandp = (*tandp << 16) | (Tgetdate() & 0xffffL);
}

char *myctime(tandp)
tand_t *tandp;
{
	static char ctimbuf[30];
	static char *monnm[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	static char *daynm[] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	tm_t *tp;
	tp = tand_to_tm(*tandp); 
	sprintf(ctimbuf, "%s %s %2d %02d:%02d:%02d %04d GMT\n",
		daynm[tp->tm_wday], monnm[tp->tm_mon], tp->tm_mday, 
		tp->tm_hour, tp->tm_min, tp->tm_sec, tp->tm_year+1900);
	return ctimbuf;
}
