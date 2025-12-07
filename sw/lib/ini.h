/*-------------------------------------------------------------------------------
 * very simple inifile parser
 * (c)2025 Anders Granlund
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

#ifndef _INIFILE_H_
#define _INIFILE_H_

/*
 * header-only library.
 *
 * options:
 *  INI_IMPL            : include implementation code
 *  INI_NO_FILE         : don't include file code
 *  INI_FILE_TOS 1/0    : TOS or libc file functions (default TOS)
 */
 
#include <stdint.h>
#include <stdbool.h>

/*-------------------------------------------------------------------------------
 * declaration
 *-----------------------------------------------------------------------------*/

typedef struct _ini_t{
    const char* data;
    const char* end;
    struct _ini_t* parent;
} ini_t;

extern bool ini_Load(ini_t* ini, const char* fname);
extern void ini_Unload(ini_t* ini);

extern void ini_Init(ini_t* ini, char* buf);
extern bool ini_GetSection(ini_t* out, ini_t* ini, const char* name);
extern const char* ini_GetStr(ini_t* ini, const char* name, const char* def);
extern int32_t ini_GetInt(ini_t* ini, const char* name, int32_t def);


/*-------------------------------------------------------------------------------
 * implementation
 *-----------------------------------------------------------------------------*/

#if defined(INI_IMPL)

#ifndef INI_FILE_TOS
#define INI_FILE_TOS 1
#endif

#if !defined(INI_NO_FILE) && INI_FILE_TOS
#include <tos.h>
#elif !defined(INI_NO_FILE)
#include <stdio.h>
#endif

static int32_t ini_strcmp(const char* s1, const char* s2) { while(*s1 && *s2) { if (*s1++ != *s2++) { return 1; } } return 0; }
static int32_t ini_strncmp(const char* s1, const char* s2, int32_t l) { while (l) { if (*s1++ != *s2++) { return 1; } l--; } return 0; }
static int32_t ini_strlen(const char* s1) { int32_t l = 0; while (*s1++) { l++; } return l; }
static void ini_memset(char* p, uint8_t v, int32_t l) { while (l) { *p++ = v; l--; } }

#if defined(INI_NO_FILE)
#define INI_NO_MALLOC 1
#else
#define INI_NO_MALLOC 0
#endif

#if !INI_NO_MALLOC
#include <malloc.h>
static void* ini_malloc(uint32_t l) { void* m = malloc(l); if (m) { ini_memset(m, 0, l); } return m; }
static void ini_free(void* buf) { if (buf) { free(buf); } }
#else
static void* ini_malloc(uint32_t l) { return 0; }
static void ini_free(void* buf) { }
#endif

void ini_Init(ini_t* ini, char* buf) {
    bool quote = false;
    bool comment = false;
    ini->data = buf;
    while (buf && *buf) {
        char c = *buf;
        if (c == '#') {
            comment = true;
        }
        if (comment) {
            *buf = 0;
            if (c == '\r' || c == '\n') {
                comment = false;
            }
        } else {
            if (c == '\"') {
                *buf = 0;
                quote = !quote;
            }
            if (!quote) {
                if ((c >= 'A') && (c <= 'Z') && !quote) { *buf = c - 'A' + 'a'; }
                else if (c == 32) { *buf = 0; }
                else if (c == '=') { *buf = 0; }
            }
            if ((c >= 127) || (c <= 31)) { *buf = 0; }
        }
        buf++;
    }
    ini->end = buf;
    ini->parent = 0;
}

bool ini_Load(ini_t* ini, const char* fname) {
#if defined(INI_NO_FILE)
    ini_memset((void*)ini, 0, sizeof(ini_t));
    return false;
#elif INI_FILE_TOS
    int32_t fresult = Fopen(fname, 0);
    ini_memset((void*)ini, 0, sizeof(ini_t));
    if (fresult) {
        int16_t fp = (int16_t)fresult;
        fresult = Fseek(0, fp, 2);
        if (fresult > 0) {
            void* buf = ini_malloc(fresult + 2);
            if (buf) {
                Fseek(0, fp, 0);
                Fread(fp, fresult, buf);
                ini_Init(ini, (char*)buf);
            }
        }
        Fclose(fp);
    }
    return ini->data ? true : false;
#else
    FILE* fp = fopen(fname, "rb");
    ini_memset(ini, 0, sizeof(ini_t));
    if (fp) {
        int32_t fresult;
        fseek(fp, 0, SEEK_END);
        fresult = ftell(fp);
        if (fresult > 0) {
            void* buf = ini_malloc(fresult + 2);
            if (buf) {
                fseek(fp, 0, SEEK_SET);
                fread(buf, fresult, 1, fp);
                ini_Init(ini, (char*)buf);
            }
        }
        fclose(fp);
    }
    return ini->data ? true : false;
#endif
}

void ini_Unload(ini_t* ini) {
    if (ini) {
        #if defined(INI_NO_FILE)
        if (!ini->parent) { ini_free(ini->data); }
        #endif
        ini_memset((void*)ini, 0, sizeof(ini_t));
    }
}

static const char* ini_NextEntry(ini_t* ini, const char* where) {
    while (ini && where && (where < ini->end)) {
        if (*where != 0) { return where; }
        where++;
    }
    return 0;
}

static const char* ini_NextKeyValue(ini_t* ini, const char* where, const char** key, const char** val) {
    *val = 0; *key = ini_NextEntry(ini, where ? where : ini->data);
    if (*key && (**key != '[')) {
        *val = ini_NextEntry(ini, *key + ini_strlen(*key));
        if (*val && (**val != '[')) {
            return *val + ini_strlen(*val);
        }
    }
    return 0;
}

static const char* ini_NextSection(ini_t* ini, const char* where) {
    while(ini && where && (where < ini->end)) {
        where = ini_NextEntry(ini, where);
        if (where) {
            if (*where == '[') { return where; }
            where += ini_strlen(where);
        }
    }
    return 0;
}

bool ini_GetSection(ini_t* out, ini_t* ini, const char* name) {
    int32_t l = ini_strlen(name);
    const char* w = ini ? ini->data : 0;
    ini_memset((void*)out, 0, sizeof(ini_t));
    while (w && (w < ini->end)) {
        w = ini_NextSection(ini, w);
        if (w) {
            int32_t l2 = ini_strlen((char*)w);
            if (l2 == (l + 2)) {
                if (ini_strncmp(name, w+1, l) == 0) {
                    out->data = w + l2;
                    w = ini_NextSection(ini, out->data);
                    out->end = w ? w : ini->end;
                    out->parent = ini;
                    return true;
                }
            }
            w += l2;
        }
    }
    out->parent = ini;
    return false;
}

const char* ini_GetStr(ini_t* ini, const char* name, const char* def) {
    const char* w = ini ? ini->data : 0;
    while (w && (w < ini->end)) {
        const char *key, *val;
        w = ini_NextKeyValue(ini, w, &key, &val);
        if (w && (ini_strcmp(key, name) == 0)) {
            return val;
        }
    }
    return def;
}

int32_t ini_GetInt(ini_t* ini, const char* name, int32_t def) {
    const char* val = ini_GetStr(ini, name, 0);
    if (val && *val) {
        int32_t total = 0;
        bool neg = false; bool hex = false;
        if (*val == '-') { neg = true; val++; }
        if (*val == '$') { hex = true; val++; }
        if (hex) { while (1) {
            char c = *val++;
            if (c >= '0' && c <= '9') { total = (total << 4) | (c - '0'); }
            else if (c >= 'a' && c <= 'f') { total = (total << 4) | (c - 'a' + 10); }
            else { break; } }
        } else { while (1) {
            char c = *val++;
            if (c >= '0' && c <= '9') { total *= 10; total += (c - '0'); }
            else { break; } }
        }
        return neg ? -total : total;
    }
    return def;
}

#endif /* INI_IMPL */

#endif /* _INIFILE_H_ */
