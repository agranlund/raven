
#include <stdio.h>
extern long lseek();

rewind(stream)
FILE *stream;
{
    fseek(stream, 0L, 0);
    return 0;
}
