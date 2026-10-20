/*-------------------------------------------------------------------------------
 * FAT32 filsystem driver for TOS
 * (c) 2026 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mint/osbind.h>
#include <mint/sysbind.h>
#include <mint/sysvars.h>
#include <mint/cookie.h>
#include "main.h"


/*----------------------------------------------------------------------*/
/* Main                                                                 */
/*----------------------------------------------------------------------*/

extern uint32_t _PgmSize;

static uint16_t tos;
static uint8_t cpu;
static uint8_t dos;

int32_t supermain(void) {
	uint32_t cookie;

#if DEBUG
	dbg_Init(DBG_NATFEATS);
#endif

	if ((Getcookie(C_MiNT, (long*)&cookie) == C_FOUND) || (Getcookie(C_MagX, (long*)&cookie) == C_FOUND)) {
		return -2;
	}

	if (Getcookie(C__CPU, (long*)&cookie) != C_FOUND) { cookie = 0; }
	cpu = (uint8_t) (cookie & 0xff);
	tos = ((uint16_t **)0x04f2)[0][1];

	if (tos >= 0x400) { dos = 0x30; }
	else if (tos >= 0x300) { dos = 0x20; }
	else if (tos >= 0x205) { dos = 0x19; }
	else if (tos >= 0x106) { dos = 0x17; }
	else if (tos >= 0x104) { dos = 0x15; }
	else { dos = 0x13; }

	dprintf("cpu=%d, tos=%02x, dos=%02x\n", cpu, tos, dos);

	if (!hdd_Init()) {
		return -1;
	}

	InstallTraps();
    return 0;
}

int32_t main(void) {
    int32_t ret = Supexec(supermain);
    if (ret == 0) {
        Ptermres(_PgmSize, 0);
    }
    return ret;
}


/*----------------------------------------------------------------------*/
/* Utils                                                                */
/*----------------------------------------------------------------------*/

typedef struct {
	/* reserved DTA segment, 21 bytes */
	uint32_t		sclust;			/* state for f_resumedir */
	uint32_t		dptr;			/* state for f_resumedir */
	char			pat[11];		/* search pattern */
	char			attr;			/* search attributes */
	int8_t			dev;			/* device index */
	/* standard DTA segment */
    char            dta_attribute;	/* file attribute */
    unsigned short  dta_time;		/* file time stamp */
    unsigned short  dta_date;		/* file date stamp */
    long            dta_size;		/* file size */
    char            dta_name[14];	/* file name */
} dta_t;

static DIR    	fsfdir;
static FILINFO 	fsfinfo;
static char   	fspat[14];

#define getValidDriveIndex(x) (((x)>=0) && ((x)<MAX_DRIVES) && drives[(x)].valid) ? (x) : -1

#define TrapFunc(x,y)	case x: return y((y##Args*)sp)
#define TrapReturn(x) 	return (((uint64_t)((uint32_t)(x))<<32)|1)
#define TrapIgnore() 	return ((uint64_t)1<<32|0)

#ifndef EFILNF
#define EFILNF  (-33)
#define EPTHNF  (-34)
#define EACCDN  (-36)
#define EIHNDL  (-37)
#define ENSMEM  (-39)
#define ENMFIL  (-49)
#define ENSAME  (-48)
#define EREADF  (-18)
#define EWRITF  (-19)
#endif

uint32_t getProcId(void) {
	uint32_t** bppp = 0;
	OSHEADER* oshdr = *((OSHEADER **)(0x4f2L));
	oshdr = oshdr->os_beg;
	if (oshdr->os_version < 0x102) {
		bppp = (uint32_t**)0x602c;
	} else {
		bppp = (uint32_t**)oshdr->p_run;
	}
	return bppp ? (uint32_t)(*bppp) : 0;
}

static int16_t getFileIndexFromHandle(int16_t fh) {
	int16_t idx = (fh - FILEHANDLE_BASE);
	return ((idx >= 0) && (idx < MAX_FILES)) ? idx : -1;
}

static fileh_t* getFileHandle(int16_t fh) {
	int16_t idx = getFileIndexFromHandle(fh);
	return (idx >= 0) ? &files[idx] : 0;
}

static int16_t allocFileHandle(void) {
	int16_t i;
	for (i = 0; i < MAX_FILES; i++) {
		if (files[i].proc == 0) {
			return i;
		}
	}
	return -1;
}

static void freeFileHandle(fileh_t* f) {
	f->proc = 0;
}

static void createFcbFromPattern(const char* pat, char fcb[11]) {
	int i = 0;
	memset(fcb, ' ', 11);

	/* filename */
	while (*pat && (*pat != '.') && (i < 8)) {
		if (*pat == '*') {
			while (i < 8) {
				fcb[i++] = '?';
			}
			pat++;
			break;
		}
		fcb[i++] = (char)toupper((unsigned char)*pat++);
	}

	while (*pat && (*pat != '.')) { pat++; }
	if (*pat == '.') { pat++; }

	/* extension */
	i = 8;
	while (*pat && (i < 11)) {
		if (*pat == '*') {
			while (i < 11) {
				fcb[i++] = '?';
			}
			break;
		}
		fcb[i++] = (char)toupper((unsigned char)*pat++);
	}
}

static void createFcbFromFilename(const char* name, char fcb[11]) {
	int i = 0;
	memset(fcb, ' ', 11);
	while (*name && (*name != '.') && (i < 8)) {
		fcb[i++] = (char)toupper((unsigned char)*name++);
	}
	while (*name && (*name != '.')) {
		name++;
	}
	if (*name == '.') {
		name++;
	}
	i = 8;
	while (*name && (i < 11)) {
		fcb[i++] = (char)toupper((unsigned char)*name++);
	}
}

static bool compareFcb(const char* name, const char fcb[11]) {
	int i;
	char fcb2[11];
	createFcbFromFilename(name, fcb2);
	for (i = 0; i < 11; i++) {
		if (fcb[i] == '?') { continue; }
		if (fcb[i] != fcb2[i]) { return false; }
	}
	return true;
}

static bool compareAttr(uint8_t src, uint8_t dst) {
	uint8_t msk = (AM_RDO | AM_HID | AM_SYS | AM_DIR);
	return (((dst & msk) & ~(src & msk)) == 0) ? true : false;
}

static int16_t fixPath(const char* src, char* dst) {
	int16_t dev;
	const char* p = src;
	char* out;

	/* make absolute path */
	if (p[1] == ':') {
		dev = (p[0] >= 'a') ? (int16_t)(p[0] - 'a') : (int16_t)(p[0] - 'A');
		p += 2;
	} else {
		dev = (int16_t)Dgetdrv();
	}

	dev = getValidDriveIndex(dev);
	if (dev < 0) { return -1; }
	dst[0] = (char)('A' + dev);
	dst[1] = ':';
	dst[2] = 0;
	out = dst + 2;

	/* handle relative to current directory */
	if ((*p != '\\') && (*p != '/')) {
		int16_t cwdlen = (int16_t)strlen(drives[dev].cwd);
		if ((cwdlen + 3) >= 256) {
			return -1;
		}
		memcpy(out, drives[dev].cwd, cwdlen + 1);
		out += cwdlen;
		if ((out > (dst + 2)) && (out[-1] == '\\')) {
			out--;
		}
	}

	while (*p) {
		/* read one token between path delimiters */
		const char* tok;
		int16_t toklen;
		while ((*p == '\\') || (*p == '/')) { p++; }
		if (*p == 0) { break; }
		tok = p;
		while (*p && ((*p != '\\') && (*p != '/'))) { p++; }
		toklen = (int16_t)(p - tok);

		if ((toklen == 1) && (tok[0] == '.')) {
			/* "."" = current directory = no action needed */
		} else if ((toklen == 2) && (tok[0] == '.') && (tok[1] == '.')) {
			/* ".." = parent directory = remove last dir */
			while ((out > (dst + 2)) && (out[-1] != '\\')) { out--; }
			if (out > dst + 2) { out--; }
			*out = 0;
		} else {
			/* add token to path */
			if (((int16_t)(out - dst) + 1 + toklen) >= 256) {
				return -1;
			}
			*out++ = '\\';
			while (toklen--) {
				*out++ = (char)toupper((unsigned char)*tok++);
			}
			*out = 0;
		}
	}

	/* end with slash if root */
	if (dst[2] == 0) {
		dst[2] = '\\';
		dst[3] = 0;
	}
	return dev;
}

static void splitPath(const char* path, char* dir, char* pat) {
	const char* p = path;
	const char* sep = 0;
	while (*p) {
		if (*p == '\\') {
			sep = p;
		}
		p++;
	}
	if (sep) {
		int len = (int)(sep - path) + 1;
		strcpy(pat, sep + 1);
		memcpy(dir, path, len);
		dir[len] = 0;
	} else if (path[1] == ':') {
		dir[0] = path[0];
		dir[1] = ':';
		dir[2] = 0;
		strcpy(pat, path + 2);
	} else {
		dir[0] = 0;
		strcpy(pat, path);
	}
}

static void updateDta(dta_t* dta, DIR* dir, FILINFO* fi) {
	const char* name = fi->altname[0] ? fi->altname : fi->fname;
	int i;
	dta->sclust = dir->obj.sclust;
	dta->dptr   = dir->dptr;
	dta->dta_attribute = (char)fi->fattrib;
	dta->dta_time = fi->ftime;
	dta->dta_date = fi->fdate;
	dta->dta_size = (long)fi->fsize;
	for (i = 0; (i < 13) && name[i]; i++) {
		dta->dta_name[i] = (char)toupper((unsigned char)name[i]);
	}
	dta->dta_name[i] = '\0';
}

static int32_t fresultToTos(FRESULT r) {
	switch (r) {
		case FR_OK:             return 0;
		case FR_NO_FILE:        return EFILNF;
		case FR_NO_PATH:        return EPTHNF;
		case FR_INVALID_NAME:   return EPTHNF;
		case FR_DENIED:         return EACCDN;
		case FR_EXIST:          return EACCDN;
		default:                return EREADF;
	}
}

/*----------------------------------------------------------------------*/
/* GEMDOS                                                               */
/*----------------------------------------------------------------------*/

typedef struct { } gd_Pterm0Args;
static uint64_t gd_Pterm0(gd_Pterm0Args* sp) {
	uint32_t proc = getProcId();
	if (proc > 0) {
		int16_t i;
		for (i = 0; i < MAX_FILES; i++) {
			if (files[i].proc == proc) {
				f_close(&files[i].fil);
				freeFileHandle(&files[i]);
			}
		}
	}
	TrapIgnore();
}

typedef struct { uint16_t retcode; } gd_PtermArgs;
static uint64_t gd_Pterm(gd_PtermArgs* sp) {
	return gd_Pterm0((gd_Pterm0Args*)sp);
}

typedef struct { int32_t keepcnt; int16_t retcode; } gd_PtermresArgs;
static uint64_t gd_Ptermres(gd_PtermresArgs* sp) {
	return gd_Pterm0((gd_Pterm0Args*)sp);
}

typedef struct { _DISKINFO* buf; int16_t driveno; } gd_DfreeArgs;
static uint64_t gd_Dfree(gd_DfreeArgs* sp) {
	int16_t dev = (sp->driveno == 0) ? Dgetdrv() : (int16_t)(sp->driveno - 1);
	dev = getValidDriveIndex(dev);
	if (dev >= 0) {
		FATFS* fs;
		DWORD nclst;
		char path[3];
		path[0] = (char)('A' + dev);
		path[1] = ':';
		path[2] = 0;
		if (f_getfree(path, &nclst, &fs) == FR_OK) {
			sp->buf->b_free   = nclst;
			sp->buf->b_total  = fs->n_fatent - 2;
			sp->buf->b_secsiz = 512;
			sp->buf->b_clsiz  = fs->csize;
			TrapReturn(0);
		}
		TrapReturn(EREADF);
	}
	TrapIgnore();
}

typedef struct { int16_t drv; } gd_DsetdrvArgs;
static uint64_t gd_Dsetdrv(gd_DsetdrvArgs* sp) {
	dprintf("Dsetdrv %d\n", sp->drv);
	TrapIgnore();
}

typedef struct { const int8_t* path; } gd_DcreateArgs;
static uint64_t gd_Dcreate(gd_DcreateArgs* sp) {
	int16_t dev;
	dprintf("Dcreate [%s]\n", sp->path);
	dev = fixPath((const char*)sp->path, tmpbuf);
	if (dev >= 0) {
		dprintf(" path [%s]\n", tmpbuf);
		TrapReturn(fresultToTos(f_mkdir(tmpbuf)));
	}
	TrapIgnore();
}

typedef struct { const int8_t* path; } gd_DdeleteArgs;
static uint64_t gd_Ddelete(gd_DdeleteArgs* sp) {
	int16_t dev;
	dprintf("Ddelete [%s]\n", sp->path);
	dev = fixPath((const char*)sp->path, tmpbuf);
	if (dev >= 0) {
		dprintf(" path [%s]\n", tmpbuf);
		TrapReturn(fresultToTos(f_unlink(tmpbuf)));
	}
	TrapIgnore();
}

typedef struct { const int8_t* path; } gd_DsetpathArgs;
static uint64_t gd_Dsetpath(gd_DsetpathArgs* sp) {
	int16_t dev;
	dprintf("Dsetpath [%s]\n", sp->path);
	dev = fixPath((const char*)sp->path, tmpbuf);
	if (dev >= 0) {
		if (tmpbuf[3] != 0) {
			FILINFO fi;
			if ((f_stat(tmpbuf, &fi) != FR_OK) || !(fi.fattrib & AM_DIR)) {
				TrapReturn(EPTHNF);
			}
		}
		strncpy(drives[dev].cwd, tmpbuf + 2, 259);
		drives[dev].cwd[259] = 0;
		dprintf(" cwd [%s]\n", drives[dev].cwd);
		TrapReturn(0);
	}
	TrapIgnore();
}

typedef struct { const int8_t *fname; int16_t attr; } gd_FcreateArgs;
static uint64_t gd_Fcreate(gd_FcreateArgs* sp) {
	int16_t dev = fixPath((const char*)sp->fname, tmpbuf);
	if (dev >= 0) {
		FRESULT r;
		uint32_t proc;
		int16_t idx = allocFileHandle();
		if (idx < 0) {
			TrapReturn(ENSMEM);
		}
		r = f_open(&files[idx].fil, tmpbuf, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
		if (r != FR_OK) {
			TrapReturn(fresultToTos(r));
		}
		proc = getProcId();
		files[idx].proc  = proc ? proc : 1;
		files[idx].drive = &drives[dev];
		files[idx].ftime = 0;
		files[idx].fdate = 0;
		TrapReturn(FILEHANDLE_BASE + idx);
	}
	TrapIgnore();
}

typedef struct { const int8_t *fname; int16_t mode; } gd_FopenArgs;
static uint64_t gd_Fopen(gd_FopenArgs* sp) {
	int16_t dev;
	dprintf("Fopen %d [%s]\n", sp->mode, sp->fname);
	dev = fixPath((const char*)sp->fname, tmpbuf);
	if (dev >= 0) {
		FRESULT r;
		FILINFO fi;
		uint8_t flags;
		uint32_t proc;
		int16_t idx = allocFileHandle();
		dprintf(" handle = %d, path = [%s]\n", idx, tmpbuf);
		if (idx < 0) {
			TrapReturn(ENSMEM);
		}
		flags = FA_OPEN_EXISTING;
		switch (sp->mode & 3) {
			case 0:  flags |= FA_READ; break;
			case 1:  flags |= FA_WRITE; break;
			default: flags |= FA_READ | FA_WRITE; break;
		}
		r = f_open(&files[idx].fil, tmpbuf, flags);
		dprintf(" res = %d\n", r);
		if (r != FR_OK) {
			TrapReturn(fresultToTos(r));
		}
		files[idx].ftime = (f_stat(tmpbuf, &fi) == FR_OK) ? fi.ftime : 0;
		files[idx].fdate = (f_stat(tmpbuf, &fi) == FR_OK) ? fi.fdate : 0;
		proc = getProcId();
		files[idx].proc  = proc ? proc : 1;
		files[idx].drive = &drives[dev];
		TrapReturn(FILEHANDLE_BASE + idx);
	}
	TrapIgnore();
}

typedef struct { int16_t handle; } gd_FcloseArgs;
static uint64_t gd_Fclose(gd_FcloseArgs* sp) {
	fileh_t* f = getFileHandle(sp->handle);
	if (f) {
		f_close(&f->fil);
		freeFileHandle(f);
		TrapReturn(0);
	}
	TrapIgnore();
}

typedef struct { int16_t handle; int32_t count; void *buf; } gd_FreadArgs;
static uint64_t gd_Fread(gd_FreadArgs* sp) {
	fileh_t* f = getFileHandle(sp->handle);
	dprintf("Fread %d, %d, fh = %08lx\n", sp->handle, sp->count, (uint32_t)f);
	if (f) {
		UINT br;
		FRESULT r = f_read(&f->fil, sp->buf, (UINT)sp->count, &br);
		dprintf(" res = %d, br = %d\n", r, br);
		TrapReturn((r == FR_OK) ? (int32_t)br : fresultToTos(r));
	}
	TrapIgnore();
}

typedef struct { int16_t handle; int32_t count; void *buf; } gd_FwriteArgs;
static uint64_t gd_Fwrite(gd_FwriteArgs* sp) {
	fileh_t* f = getFileHandle(sp->handle);
	if (f) {
		UINT bw;
		FRESULT r = f_write(&f->fil, sp->buf, (UINT)sp->count, &bw);
		TrapReturn((r == FR_OK) ? (int32_t)bw : fresultToTos(r));
	}
	TrapIgnore();
}

typedef struct { const int8_t *fname; } gd_FdeleteArgs;
static uint64_t gd_Fdelete(gd_FdeleteArgs* sp) {
	int16_t dev = fixPath((const char*)sp->fname, tmpbuf);
	if (dev >= 0) {
		TrapReturn(fresultToTos(f_unlink(tmpbuf)));
	}
	TrapIgnore();
}

typedef struct { int32_t offset; int16_t handle; int16_t seekmode; } gd_FseekArgs;
static uint64_t gd_Fseek(gd_FseekArgs* sp) {
	fileh_t* f = getFileHandle(sp->handle);
	dprintf("Fseek %d, %d, %d, fh = %08lx\n", sp->offset, sp->handle, sp->seekmode, (uint32_t)f);
	if (f) {
		FSIZE_t pos;
		FRESULT r;
		switch (sp->seekmode) {
			case 0:  pos = (FSIZE_t)sp->offset; break;
			case 1:  pos = f_tell(&f->fil) + (FSIZE_t)sp->offset; break;
			case 2:  pos = f_size(&f->fil) + (FSIZE_t)sp->offset; break;
			default: TrapReturn(EIHNDL);
		}
		r = f_lseek(&f->fil, pos);
		TrapReturn((r == FR_OK) ? (int32_t)f_tell(&f->fil) : fresultToTos(r));
	}
	TrapIgnore();
}

typedef struct { const int8_t *filename; int16_t wflag; int16_t attrib; } gd_FattribArgs;
static uint64_t gd_Fattrib(gd_FattribArgs* sp) {
	int16_t dev;
	dprintf("Fattrib [%s]\n", sp->filename);
	dev = fixPath((const char*)sp->filename, tmpbuf);
	if (dev >= 0) {
		FILINFO fi;
		if (sp->wflag == 0) {
			FRESULT r = f_stat(tmpbuf, &fi);
			dprintf(" read_res = %d\n", r);
			TrapReturn((r == FR_OK) ? (int32_t)fi.fattrib : fresultToTos(r));
		} else {
			FRESULT r = f_chmod(tmpbuf, (BYTE)sp->attrib, AM_RDO | AM_HID | AM_SYS | AM_ARC);
			dprintf(" write_res = %d\n", r);
			if (r != FR_OK) {
				TrapReturn(fresultToTos(r));
			}
			r = f_stat(tmpbuf, &fi);
			dprintf(" stat_res = %d\n", r);
			TrapReturn((r == FR_OK) ? (int32_t)fi.fattrib : fresultToTos(r));
		}
	}
	TrapIgnore();
}

typedef struct { int8_t *path; int16_t driveno; } gd_DgetpathArgs;
static uint64_t gd_Dgetpath(gd_DgetpathArgs* sp) {
	int16_t dev = (sp->driveno == 0)
		? (int16_t)Dgetdrv()
		: (int16_t)(sp->driveno - 1);
	dev = getValidDriveIndex(dev);
	dprintf("Dgetpath %d, dev=%d\n", sp->driveno, dev);
	if (dev >= 0) {
		const char* cwd = drives[dev].cwd;
		strcpy((char*)sp->path, (cwd[1] == 0) ? "" : cwd);
		dprintf(" path [%s]\n", sp->path);
		TrapReturn(0);
	}
	TrapIgnore();
}

typedef struct { const int8_t *filename; int16_t attr; } gd_FsfirstArgs;
static uint64_t gd_Fsfirst(gd_FsfirstArgs* sp) {
	int16_t dev;
	dta_t* dta;
	dprintf("Fsfirst [%s] %02x\n", sp->filename, sp->attr);
	dev = fixPath((const char*)sp->filename, tmpbuf);
	if (dev >= 0) {
		splitPath(tmpbuf, tmpbuf, fspat);
		dprintf(" dir [%s] pat [%s]\n", tmpbuf, fspat);
		dta = (dta_t*)Fgetdta();
		dta->dev  = (int8_t)dev;
		dta->attr = (char)sp->attr;
		createFcbFromPattern(fspat, dta->pat);

		if (f_opendir(&fsfdir, tmpbuf) != FR_OK) {
			TrapReturn(EPTHNF);
		}

		if (sp->attr & AM_VOL) {
			char volpath[3];
			volpath[0] = (char)('A' + dev);
			volpath[1] = ':';
			volpath[2] = 0;
			fsfinfo.fattrib = AM_VOL;
			fsfinfo.ftime = 0;
			fsfinfo.fdate = 0;
			fsfinfo.fsize = 0;
			fsfinfo.altname[0] = 0;
			if (f_getlabel(volpath, fsfinfo.fname, 0) != FR_OK) {
				fsfinfo.fname[0] = 0;
			}
			updateDta(dta, &fsfdir, &fsfinfo);
			TrapReturn(0);
		}

		while ((f_readdir(&fsfdir, &fsfinfo) == FR_OK) && fsfinfo.fname[0]) {
			const char* name = fsfinfo.altname[0] ? fsfinfo.altname : fsfinfo.fname;
			dprintf(" name[%s] alt[%s] -> [%s]\n", fsfinfo.fname, fsfinfo.altname, name);
			if (compareFcb(name, dta->pat) && compareAttr((uint8_t)sp->attr, fsfinfo.fattrib)) {
				updateDta(dta, &fsfdir, &fsfinfo);
				dprintf(" match\n");
				TrapReturn(0);
			}
		}
		TrapReturn(EFILNF);
	}
	TrapIgnore();
}

typedef struct { } gd_FsnextArgs;
static uint64_t gd_Fsnext(gd_FsnextArgs* sp) {
	dta_t* dta = (dta_t*)Fgetdta();
	int16_t dev = getValidDriveIndex((int16_t)dta->dev);
	dprintf("Fsnext %d, dev=%d\n", dta->dev, dev);
	if (dev >= 0) {

		f_resumedir(&fsfdir, &drives[dev].fs, dta->sclust, dta->dptr);

		while ((f_readdir(&fsfdir, &fsfinfo) == FR_OK) && fsfinfo.fname[0]) {
			const char* name = fsfinfo.altname[0] ? fsfinfo.altname : fsfinfo.fname;
			dprintf(" name[%s] alt[%s] -> [%s]\n", fsfinfo.fname, fsfinfo.altname, name);
			if (compareFcb(name, dta->pat) && compareAttr((uint8_t)dta->attr, fsfinfo.fattrib)) {
				updateDta(dta, &fsfdir, &fsfinfo);
				dprintf(" match\n");
				TrapReturn(0);
			}
		}
		TrapReturn(ENMFIL);
	}
	TrapIgnore();
}

typedef struct { int16_t mode; int8_t* fname; const char* cmdline; const char* env; } gd_PexecArgs;
static uint64_t gd_Pexec(gd_PexecArgs* sp) {
    if ((sp->mode == PE_LOAD) || (sp->mode == PE_LOADGO)) {
        int16_t dev = fixPath((const char*)sp->fname, tmpbuf);
        if (dev >= 0) {
            FIL fil;
            UINT br;
            BASEPAGE* pd;
            prghdr_t hdr;
			int32_t fsize;
			int32_t result = 0;
            if (f_open(&fil, tmpbuf, FA_READ | FA_OPEN_EXISTING) != FR_OK) {
                TrapReturn(EFILNF);
			}

			if (f_read(&fil, &hdr, sizeof(prghdr_t), &br) != FR_OK || (br != sizeof(prghdr_t)) || (hdr.magic != 0x601A)) {
                f_close(&fil);
                TrapReturn(EFILNF);
            }

			/* create basepage */
			if (dos >= 0x0019) {
				result = Pexec(7, hdr.prgflag, sp->cmdline, sp->env);	
			} else {
				result = Pexec(5, 0L, sp->cmdline, sp->env);
			}

			if (result <= 0) {
				f_close(&fil);
				TrapReturn(result);
			}
			pd = (BASEPAGE*)result;
			pd->p_tbase = pd->p_lowtpa + sizeof(BASEPAGE);
			pd->p_tlen = hdr.tlen;
			pd->p_dbase = pd->p_tbase + pd->p_tlen;
			pd->p_dlen = hdr.dlen;
			pd->p_bbase = pd->p_dbase + pd->p_dlen;
			pd->p_blen = hdr.blen;

			/* verify memory for file */
			fsize = f_size(&fil);
			if (fsize >= (pd->p_hitpa - pd->p_lowtpa)) {
				f_close(&fil);
				TrapReturn(ENSMEM);
			}
			
			/* load */
			f_read(&fil, pd->p_tbase, fsize - sizeof(prghdr_t), &br);
			f_close(&fil);

			/* verify memory for program */
			if ((sizeof(BASEPAGE) + hdr.tlen + hdr.dlen + hdr.blen) >= (pd->p_hitpa - pd->p_lowtpa)) {
				TrapReturn(ENSMEM);
			}

			/* relocate */
			if (hdr.absflag == 0) {
				int32_t relsize = fsize - (sizeof(prghdr_t) + hdr.tlen + hdr.dlen + hdr.slen);
				if (relsize > 0) {
					uint8_t* relp = (uint8_t*)(pd->p_tbase + hdr.tlen + hdr.dlen + hdr.slen);
					uint8_t* relo = (uint8_t*)(pd->p_tbase);
					uint32_t reld = *((uint32_t*)relp);
					relp += 4;
					while (reld && (relsize > 0)) {
						if (reld == 1) {
							relo += 0xfe;
						} else {
							relo += (uint8_t)reld;
							*((uint32_t*)relo) += (uint32_t)pd->p_tbase;
						}
						reld = (uint32_t)(*relp++);
						relsize--;
					}
				}
			}

			/* clear bss */
			memset(pd->p_bbase, 0, pd->p_blen);

			/* flush caches */
			cpu_FlushCache(cpu);

			/* PE_LOAD: done */
			if (sp->mode == 3) {
				TrapReturn((int32_t)pd);
			}

			/* PE_LOADGO: fall through to PE_GOTHENFREE */
			sp->mode = PE_GO_FREE;
			sp->fname = 0L;
			sp->cmdline = (char*)pd;
			sp->env = 0L;
		}
    }
    TrapIgnore();
}

typedef struct { int16_t reserved; const int8_t *oldname; const int8_t *newname; } gd_FrenameArgs;
static uint64_t gd_Frename(gd_FrenameArgs* sp) {
	int16_t dev1 = fixPath((const char*)sp->oldname, tmpbuf);
	if (dev1 >= 0) {
		int16_t dev2 = fixPath((const char*)sp->newname, tmpbuf + 256);
		if (dev1 != dev2) {
			TrapReturn(ENSAME);
		}
		TrapReturn(fresultToTos(f_rename(tmpbuf, tmpbuf + 256)));
	}
	TrapIgnore();
}

typedef struct { _DOSTIME *timeptr; int16_t handle; int16_t wflag; } gd_FdatimeArgs;
static uint64_t gd_Fdatime(gd_FdatimeArgs* sp) {
	fileh_t* f = getFileHandle(sp->handle);
	if (f) {
		if (sp->wflag == 0) {
			sp->timeptr->time = f->ftime;
			sp->timeptr->date = f->fdate;
			TrapReturn(0);
		} else {
			FILINFO fi;
			fi.fdate = sp->timeptr->date;
			fi.ftime = sp->timeptr->time;
			if (f_utime_fil(&f->fil, &fi) == FR_OK) {
				f->fdate = sp->timeptr->date;
				f->ftime = sp->timeptr->time;
				TrapReturn(0);
			}
			TrapReturn(EACCDN);
		}
	}
	TrapIgnore();
}

uint64_t DispatchGemdos(int16_t* sp) {
	uint16_t op = *sp++;
	switch(op) {
		TrapFunc( 0, gd_Pterm0);
		TrapFunc(14, gd_Dsetdrv);
		TrapFunc(49, gd_Ptermres);
		TrapFunc(54, gd_Dfree);
		TrapFunc(57, gd_Dcreate);
		TrapFunc(58, gd_Ddelete);
		TrapFunc(59, gd_Dsetpath);
		TrapFunc(60, gd_Fcreate);
		TrapFunc(61, gd_Fopen);
		TrapFunc(62, gd_Fclose);
		TrapFunc(63, gd_Fread);
		TrapFunc(64, gd_Fwrite);
		TrapFunc(65, gd_Fdelete);
		TrapFunc(66, gd_Fseek);
		TrapFunc(67, gd_Fattrib);
		TrapFunc(71, gd_Dgetpath);
		TrapFunc(75, gd_Pexec);
		TrapFunc(76, gd_Pterm);
		TrapFunc(78, gd_Fsfirst);
		TrapFunc(79, gd_Fsnext);
		TrapFunc(86, gd_Frename);
		TrapFunc(87, gd_Fdatime);
	}
	TrapIgnore();
}

/*----------------------------------------------------------------------*/
/* BIOS                                                                 */
/*----------------------------------------------------------------------*/

typedef struct { int16_t rwflag; void *buff; int16_t cnt; int16_t recnr; int16_t dev; int32_t lrecno; } bios_RwabsArgs;
uint64_t bios_Rwabs(bios_RwabsArgs* sp) {
	int16_t dev = getValidDriveIndex(sp->dev);
	if ((dev >= 0) && ((sp->rwflag & (1 << 3)) == 0)) {
		int32_t lrecno = (sp->recnr < 0) ? sp->lrecno : (int32_t)(sp->recnr);
		int32_t result = Rwabs(sp->rwflag | (1<<3), sp->buff, sp->cnt, -1, drives[dev].dev_phys, drives[dev].block_start + lrecno);
		TrapReturn(result);
	}
	TrapIgnore();
}

typedef struct { int16_t dev; } bios_GetbpbArgs;
uint64_t bios_Getbpb(bios_GetbpbArgs* sp) {
	int16_t dev = getValidDriveIndex(sp->dev);
	if (dev >= 0) {
		TrapReturn(&(drives[dev].bpb));
	}
	TrapIgnore();
}

typedef struct { int16_t dev; } bios_MediachArgs;
uint64_t bios_Mediach(bios_MediachArgs* sp) {
	int16_t dev = getValidDriveIndex(sp->dev);
	if (dev >= 0) {
		TrapReturn(0);
	}
	TrapIgnore();
}

uint64_t DispatchBios(int16_t* sp) {
	uint16_t op = *sp++;
	switch(op) {
		TrapFunc( 4, bios_Rwabs);
		TrapFunc( 7, bios_Getbpb);
		TrapFunc( 9, bios_Mediach);
	}
	TrapIgnore();
}
