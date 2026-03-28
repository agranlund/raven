#include <stdlib.h>
#include <mint/osbind.h>
#include <errno.h>
#include <sys/stat.h>
#include "lib.h"

#define X_MAGIC   0x601a

int stat(const char *path, struct stat *buff)
{
    _DTA* old_dta = Fgetdta();
    _DTA dta;
    long ret;
    _DOSTIME* timeptr;

    Fsetdta(&dta);

    ret = Fsfirst(path, FA_DIR | FA_RDONLY | FA_HIDDEN | FA_SYSTEM);

    Fsetdta(old_dta);

    if (ret != 0)
    {
        __set_errno(ENOENT);
        return -1;
    }

    timeptr = (_DOSTIME*)&buff->st_atime;
    timeptr->time = dta.dta_time;
    timeptr->date = dta.dta_date;

    buff->st_dev   = (path[0] != '\0' && path[1] == ':') ? ((path[0] & ~0x20 /* toupper */) - 'A') : Dgetdrv();
    buff->st_ino   = 0;
    buff->st_mode  = S_IRUSR;
    buff->st_nlink = 1;
    buff->st_uid   = 0;
    buff->st_gid   = 0;
    buff->st_rdev  = buff->st_dev;
    buff->st_size  = dta.dta_size;
    buff->st_mtime = buff->st_atime;
    buff->st_ctime = buff->st_atime;

    if ((dta.dta_attribute & FA_RDONLY) == 0)
        buff->st_mode |= S_IWUSR;

    if ((dta.dta_attribute & FA_DIR) != 0)
    {
        buff->st_mode |= S_IFDIR | S_IXUSR;
    } else
    {
        long fh;

        buff->st_mode |= S_IFREG;

        fh = Fopen(path, FO_READ);

        if (fh >= 0)
        {
            unsigned short magic;

            if (Fread(fh, sizeof(magic), &magic) == sizeof(magic) && magic == X_MAGIC)
                buff->st_mode |= S_IXUSR;

            Fclose(fh);
        }
    }

    return 0;
}
