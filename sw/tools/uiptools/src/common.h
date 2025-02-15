#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <osbind.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdbool.h>

struct Repsonse {
  char* malloc_block;
  char* current;
  size_t size;
};

extern _DTA  dta;  /* global! */

void fstrcat(struct Repsonse* response, char* format, ...);
void joinpath(char *pth1, const char *pth2);
bool ensureFolderExists(const char* path, bool stripFileName);
void convertToGemDosPath(char* path);
void sanitiseGemDosPath(char* path);
void sanitiseGemDosName(char* path);
bool deletePath(const char* path);
bool getFileSize(const char* path, int* size);
int Fclose_safe(int16_t* fd);
uint64_t getMicroseconds();

#endif // __COMMON_H__
