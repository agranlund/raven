#ifndef __MEMDEBUG_H__
#define __MEMDEBUG_H__

#define MEM_GARBAGE_FRIENDLY 0

/*
 * values for DEBUG_ALLOC:
 * 0 - disable
 * 1 - track number of blocks and total allocated size
 * 2 - track every block and where it is allocated
 * 3 - same as 2 and track maximum allocated size & number
 * 4 - same as 3 and write log file to "dbgalloc.trc"
 */
#ifndef DEBUG_ALLOC
#  define DEBUG_ALLOC 0
#endif

#ifdef HAVE_GTK
#  define HAVE_GLIB 1
#  include <glib.h>
#endif

#if DEBUG_ALLOC >= 2
void *dbg_malloc(size_t n, const char *file, long line);
void *dbg_calloc(size_t n, size_t s, const char *file, long line);
char *dbg_strdup(const char *s, const char *file, long line);
char *dbg_strndup(const char *s, size_t len, const char *file, long line);
void dbg_free(void *p, const char *file, long line);
void *dbg_realloc(void *p, size_t s, const char *file, long line);
#endif

#define g_malloc(n) malloc(n)
#define g_try_malloc(n) malloc(n)
#define g_calloc(n, s) calloc(n, s)
#define g_malloc0(n) calloc(n, 1)
#define g_realloc(ptr, s) realloc(ptr, s)
#define g_free free

#undef g_new
#define g_new(t, n) ((t *)g_malloc(sizeof(t) * (n)))
#undef g_renew
#define g_renew(t, p, n) ((t *)g_realloc(p, sizeof(t) * (n)))
#undef g_new0
#define g_new0(t, n) ((t *)g_calloc((n), sizeof(t)))

char *g_strdup(const char *);
char *g_strndup(const char *, size_t len);

#if DEBUG_ALLOC >= 2
#define malloc(n) dbg_malloc(n, __FILE__, __LINE__)
#define calloc(n, s) dbg_calloc(n, s, __FILE__, __LINE__)
#define g_strdup(s) dbg_strdup(s, __FILE__, __LINE__)
#define g_strndup(s, n) dbg_strndup(s, n, __FILE__, __LINE__)
#endif
#if DEBUG_ALLOC >= 1
#define free(p) dbg_free(p, __FILE__, __LINE__)
#define realloc(p, n) dbg_realloc(p, n, __FILE__, __LINE__)
#endif

#if MEM_GARBAGE_FRIENDLY
#define mem_garbage_clear(p) p = NULL
#else
#define mem_garbage_clear(p)
#endif

void _crtexit(void);

#endif /* __MEMDEBUG_H__ */
