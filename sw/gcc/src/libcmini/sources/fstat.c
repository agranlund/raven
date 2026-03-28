#include <stdlib.h>
#include <mint/osbind.h>
#include <errno.h>
#include <sys/stat.h>
#include "lib.h"

#define X_MAGIC   0x601a

int fstat(int handle, struct stat *buff)
{
    long fpos;
    long ftime;
    size_t fsize;

    fpos = Fseek(0, handle, SEEK_CUR);

    if (fpos < 0)
    {
        __set_errno(-fpos);
        return -1;
    }

    Fdatime(&ftime, handle, 0);

    fsize = Fseek(0, handle, SEEK_END);

    // intialize struct
    // Pure C did not initialize members which cannot be set with a handle!

    buff->st_dev   = 0; // cannot be determined
    buff->st_ino   = 0;
    buff->st_mode  = S_IFREG | S_IRUSR;
    buff->st_nlink = 1;
    buff->st_uid   = 0;
    buff->st_gid   = 0;
    buff->st_rdev  = buff->st_dev;
    buff->st_size  = fsize;
    buff->st_atime = ftime;
    buff->st_mtime = ftime;
    buff->st_ctime = ftime;

    if (fsize > 0)
    {
        unsigned char byte;

        Fseek(0, handle, SEEK_SET);

        if (Fread(handle, 1, &byte) == 1)
        {
            Fseek(0, handle, SEEK_SET);

            if (Fwrite(handle, 1, &byte) == 1)
                buff->st_mode |= S_IWUSR;

            if (fsize > 1)
            {
                unsigned short magic;

                Fseek(0, handle, SEEK_SET);

                if (Fread(handle, sizeof(magic), &magic) == sizeof(magic) && magic == X_MAGIC)
                    buff->st_mode |= S_IXUSR;
            }
        }
    }

    Fseek(fpos, handle, SEEK_SET);

    return 0;
}
