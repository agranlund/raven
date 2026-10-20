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

#ifndef _MAIN_H_
#define _MAIN_H_
#include <stdint.h>
#include <stdbool.h>
#include "fatfs/source/ff.h"

#define DEBUG 0


#define MAX_DRIVES 		16
#define MAX_FILES		16
#define FILEHANDLE_BASE	(128-MAX_FILES)

typedef struct {
	FATFS		fs;
	int16_t		valid;
	int16_t		dev_phys;
	uint32_t	block_start;
	uint32_t	block_size;
	_BPB		bpb;
	char		cwd[260];
} drive_t;

typedef struct {
	FIL			fil;
	uint32_t	proc;
	drive_t*	drive;
	uint16_t	ftime;
	uint16_t	fdate;
} fileh_t;

typedef struct {
    uint16_t magic; 	/* 0x601a       */
    uint32_t tlen;  	/* size text    */
    uint32_t dlen;  	/* size data    */
    uint32_t blen;  	/* size bss     */
    uint32_t slen;  	/* size symbols */
    uint32_t res1;
    uint32_t prgflag;
    uint16_t absflag;
} prghdr_t;

extern drive_t drives[MAX_DRIVES];
extern fileh_t files[MAX_FILES];
extern char    tmpbuf[512];

extern uint16_t cpu_SetSR(int16_t sr);
extern void cpu_FlushCache(int16_t cpu);
extern void InstallTraps(void);
extern bool hdd_Init(void);


#define DBG_NULL		0
#define DBG_NATFEATS	1

#if DEBUG
	void dbg_Init(int16_t mode);
	void dbg_Printf(const char* str, ...);
	#define dprintf(...) dbg_Printf(__VA_ARGS__)
	#define D(x) x
#else
	#define dbg_Init(x)
	#define dbg_Printf(...)
	#define dprintf(...)
	#define D(x)
#endif

#endif /* _MAIN_H_ */
