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
#include <mint/osbind.h>
#include <mint/sysbind.h>
#include <mint/cookie.h>
#include "main.h"
#include "fatfs/source/diskio.h"

/* AHDI */
#define PUN_DEV			0x1f
#define PUN_UNIT		0x07
#define PUN_ACSI		0x00
#define PUN_SCSI		0x08
#define PUN_IDE			0x10
#define PUN_REMOVABLE	0x40
#define PUN_INVALID		0x80

typedef struct {
	uint16_t	puns;
	uint8_t		flags[MAX_DRIVES];
	uint32_t	partition_start[MAX_DRIVES];
	uint32_t	cookie;
	uint32_t*	cookie_ptr;
	uint16_t	version;
	uint16_t	max_sect_siz;
	uint32_t	reserved[MAX_DRIVES];
} ahdi_t;

static ahdi_t* ahdi = 0;
drive_t drives[MAX_DRIVES];
fileh_t files[MAX_FILES];
char    tmpbuf[512];

/*-----------------------------------------------------------------------*/
/* FatFs callbacks                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) {
	if ((pdrv >= MAX_DRIVES) || !drives[pdrv].valid) {
		return STA_NOINIT;
	}
	return 0;
}

DSTATUS disk_initialize(BYTE pdrv) {
	if ((pdrv >= MAX_DRIVES) || !drives[pdrv].valid) {
		return STA_NOINIT;
	}
	return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
	if ((pdrv < MAX_DRIVES) && drives[pdrv].valid) {
		drive_t *d = &drives[pdrv];
		dprintf("disk_read drv=%d sec=%lu+%u abs=%lu\n", pdrv, (unsigned long)sector, count, (unsigned long)(d->block_start + sector));
		if (Rwabs(8, buff, (short)count, -1, d->dev_phys, d->block_start + sector) != 0) {
			return RES_ERROR;
		}
		return RES_OK;
	}
	return RES_NOTRDY;
}

#if (FF_FS_READONLY == 0)
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
	if ((pdrv < MAX_DRIVES) && drives[pdrv].valid) {
		drive_t *d = &drives[pdrv];
		if (Rwabs(9, (void*)buff, (short)count, -1, d->dev_phys, d->block_start + sector) != 0) {
			return RES_ERROR;
		}
		return RES_OK;
	}
	return RES_NOTRDY;
}
#endif

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
	if ((pdrv >= MAX_DRIVES) || !drives[pdrv].valid) {
		return RES_NOTRDY;
	}
	switch (cmd) {
		case CTRL_SYNC:
			return RES_OK;

		case GET_SECTOR_SIZE:
			*(WORD*)buff = 512;
			return RES_OK;
	}
	return RES_PARERR;
}

DWORD get_fattime(void) {
	return ((DWORD)Tgetdate() << 16) | (DWORD)Tgettime();
}

/*-----------------------------------------------------------------------*/

#define bswap32(x)	__builtin_bswap32((x))

static int16_t hdd_ScanDrives(void) {
	uint8_t* secbuf = (uint8_t*)tmpbuf;
	int16_t numdrives = 0;
	int16_t i;
	uint32_t drvbits = *((volatile uint32_t*)0x4c2);

	dprintf("puns=%d drvbits=%08lx\n", ahdi->puns, drvbits);
	if (ahdi->puns == 0) {
		return 0;
	}

	for (i = 2; i < MAX_DRIVES; i++) {
		int16_t  dev_phys;
		uint32_t block_start;
		uint16_t bytesPerSector;
		uint8_t  clusterSize;
		uint16_t rootEntCnt;
		uint16_t totSec16;
		uint16_t fatsz16;
		uint32_t totsec32;

		if (!(drvbits & (1 << i))) {
			continue;
		}

		if (ahdi->flags[i] & PUN_INVALID) {
			continue;
		}

		dev_phys    = 2 + (ahdi->flags[i] & PUN_DEV);
		block_start = ahdi->partition_start[i];

		dprintf("drive %c: dev_phys=%d block_start=%lu\n", 'A'+i, dev_phys, block_start);

		if (Rwabs(8, secbuf, 1, -1, dev_phys, block_start) != 0) {
			continue;
		}

		bytesPerSector = (uint16_t)secbuf[0x0c] << 8 | secbuf[0x0b];
		clusterSize    = secbuf[0x0d];
		rootEntCnt     = (uint16_t)secbuf[0x12] << 8 | secbuf[0x11];
		totSec16       = (uint16_t)secbuf[0x14] << 8 | secbuf[0x13];
		fatsz16        = (uint16_t)secbuf[0x17] << 8 | secbuf[0x16];
		totsec32       = bswap32(*((uint32_t*)&secbuf[0x20]));

		dprintf("  bps=%u cls=%u rootent=%u tot16=%u fatsz16=%u tot32=%lu\n",
			bytesPerSector, clusterSize, rootEntCnt, totSec16, fatsz16, totsec32);

		/* initial sanity check before bothering fatfs */
		if ((bytesPerSector == 512) &&
			(rootEntCnt == 0) &&
			(totSec16 == 0) &&
			(fatsz16 == 0) &&
			(totsec32 > 65535) &&
			((clusterSize & (clusterSize - 1)) == 0))
		{
			FRESULT res;
			char path[4];
			drive_t *d = &drives[i];

			d->dev_phys    = dev_phys;
			d->block_start = block_start;
			d->block_size  = 512;
			d->valid       = 1;

			path[0] = (char)('A' + i);
			path[1] = ':';
			path[2] = 0;

			res = f_mount(&d->fs, path, 1);
			dprintf("  mount %s res=%d\n", path, res);
			dprintf("  volbase=%lu fatbase=%lu database=%lu dirbase=%lu csize=%d\n",
				d->fs.volbase, d->fs.fatbase, d->fs.database, d->fs.dirbase, d->fs.csize);

			if (res == FR_OK) {
				/* fake bpb, TOS wont use it but needs it to look sane */
				uint32_t ncl = d->fs.n_fatent - 2;
				d->bpb.recsiz = 512;
				d->bpb.clsiz  = (int16_t)d->fs.csize;
				d->bpb.clsizb = (int16_t)(d->fs.csize << 9);
				d->bpb.rdlen  = 0;
				d->bpb.fsiz   = (d->fs.fsize > 0x7FFF) ? 0x7FFF : (int16_t)d->fs.fsize;
				d->bpb.fatrec = (int16_t)((d->fs.fatbase - d->fs.volbase) + d->fs.fsize);
				d->bpb.datrec = (int16_t)(d->fs.database - d->fs.volbase);
				d->bpb.numcl  = (ncl > 32767) ? 32767 : (int16_t)ncl;
				d->bpb.bflags = 1;
				d->cwd[0] = '\\';
				d->cwd[1] = 0;

				numdrives++;
				D({
					DIR dir;
					FILINFO fi;
					if (f_opendir(&dir, path) == FR_OK) {
						while ((f_readdir(&dir, &fi) == FR_OK) && fi.fname[0]) {
							dprintf("  %s %ld\n", fi.fname, fi.fsize);
						}
						f_closedir(&dir);
					}
				});
			} else {
				d->valid = 0;
			}
		}
	}

	return numdrives;
}

bool hdd_Init(void) {
	dprintf("hdd_Init\n");

	ahdi = (*((ahdi_t**)0x516L));
	if (!ahdi) {
		dprintf("no ahdi\n");
		return false;
	}
	if (ahdi->cookie != 'AHDI') {
		dprintf("bad ahdi cookie\n");
		return false;
	}
	if (ahdi->version < 0x300) {
		dprintf("ahdi version too old: %04x\n", ahdi->version);
		return false;
	}

	memset(drives, 0, sizeof(drive_t) * MAX_DRIVES);
	memset(files, 0, sizeof(fileh_t) * MAX_FILES);

	if (hdd_ScanDrives() <= 0)
		return false;

	return true;
}
