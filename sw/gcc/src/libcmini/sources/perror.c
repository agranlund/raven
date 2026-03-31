/*
  Written by Pieter van Oostrum for libcmini
  Created on 2026-02-10
  Public domain
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

void perror(const char *s)
{
    char *error = strerror(errno);
    
    if (s != NULL && *s != '\0')
    {
        fputs(s, stderr);
        fputs(": ", stderr);
    }

    if (error != NULL)
        fputs(error, stderr);
    putc('\n', stderr);
}
