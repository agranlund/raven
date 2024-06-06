#include <stdlib.h>
#include <osbind.h>
#include <errno.h>
#include <sys/types.h>
#ifndef	_BITS_TYPES_H
#define	_BITS_TYPES_H	1
#endif
#include <sys/stat.h>

#define X_MAGIC   0x601a

int stat(const char *path, struct stat *buff)
{
    _DTA* old_dta = Fgetdta();
    _DTA dta;
    long ret;

    Fsetdta(&dta);

    ret = Fsfirst(path, FA_DIR | FA_RDONLY | FA_HIDDEN | FA_SYSTEM);

    if (ret == 0) {
        buff->st_dev   = (path[0] != '\0' && path[1] == ':') ? ((path[0] & ~0x20 /* toupper */) - 'A') : Dgetdrv();
        buff->st_ino   = 0;
        buff->st_mode  = S_IRUSR;
        buff->st_nlink = 1;
        buff->st_uid   = 0;
        buff->st_gid   = 0;
        buff->st_rdev  = buff->st_dev;
        buff->st_size  = dta.dta_size;
        buff->st_atime = dta.dta_time;
        buff->st_mtime = dta.dta_time;
        buff->st_ctime = dta.dta_time;

        if ((dta.dta_attribute & FA_RDONLY) == 0) {
            buff->st_mode |= S_IWUSR;
        }

        if ((dta.dta_attribute & FA_DIR) != 0) {
            buff->st_mode |= S_IFDIR | S_IXUSR;
        } else {
            long fh;

            buff->st_mode |= S_IFREG;

            fh = Fopen(path, FO_READ);

            if (fh >= 0) {
                unsigned short magic;

                if (Fread(fh, sizeof(magic), &magic) == sizeof(magic)) {
                    if (magic == X_MAGIC) {
                        buff->st_mode |= S_IXUSR;
                    }
                }

                Fclose(fh);
            }
        }

        errno = 0;
    } else {
        ret   = -1;
        errno = ENOENT;
    }

    Fsetdta(old_dta);

    return ret;
}
