#include <stdio.h>

extern long strtol();

int atoi(str)
char *str;
{
    return (int)strtol(str, (char **)NULL, 10);
}
