#include <stdio.h>

extern long strtol();

long atol(str)
char *str;
{
    return strtol(str, (char **)NULL, 10);
}
