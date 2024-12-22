#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include "list.h"
#include "warnings.h"
#include "memdebug.h"
#if defined(__TOS__) || defined(__atarist__)
#else
#define stricmp strcasecmp
#endif

#define DEFAULT_MAXERRS 25
#define DEFAULT_MAXWARNS 50
#define DEFAULT_IDLENGTH 32
#define MAX_WARNINGLEVEL 2
#define DEFAULT_WARNINGLEVEL 1
#define MAX_IDLENGTH 254 /* limit through object file format; cannot easily be changed */
#define DEFAULT_STACKSIZE 4096					/* default stacksize is Pure C compatible */

typedef struct _cflags {
	bool strict_ANSI;			/* -A, Strict ANSI */
	bool output_DRI;			/* -B, DRI object output */
	bool nested_comments;		/* -C, allow nested comments */
	short max_errors;			/* -E, Maximum number of errors, or 0 */
	short max_warnings;			/* -F, Maximum number of warnings, or 0 */
	bool optimize_size;			/* -G, Size optimization */
	bool cdecl_calling;			/* -H, standard (cdecl) calling, not Pure_C calling */
	bool no_jump_optimization;  /* -J, Dont optimize jumps */
	bool char_is_unsigned;		/* -K, default char is unsigned */
	short identifier_max_length;/* -L, maximum identifier length */
	bool string_merging;		/* -M, merge identical strings */
	char *output_directory;		/* -N, output directory */
	char *output_name;			/* -O, output file name */
	bool absolute_calls;		/* -P, use absolute calls */
	bool pascal_calling;		/* -Q, pascal calling */
	bool no_register_vars;		/* -R, no register variables */
	bool frame_pointer;			/* -S, force frame pointers (always) */
	bool stack_checking;		/* -T, stack checking */
	short verbose;				/* -V, number of v's = level of verbosity */
	bool add_underline;			/* -X, prepend underscore to identifiers */
	bool debug_infos;			/* -Y, generate debug information */
	bool no_register_reload;	/* -Z, no register optimization */
	bool i2_68010;				/* -1, >= 68010 */
	bool i2_68020;				/* -2, >= 68020 */
	bool i2_68030;				/* -3, 68030 */
	bool i2_68040;				/* -4, 68040 */
	bool i2_68851;				/* -5, 68851 */
	bool i2_68060;				/* -6, 68060 */
	bool use_FPU;				/* -8, enable floating point h/w */
	bool Coldfire;				/* -7, Coldfire (double is 64 bits) */

	bool default_int32;			/* -mno-short, default int 32 bits */

	int warning_level;
	signed char warning_enabled[WARN_MAX];
	
	bool no_output;
	
    strlist *defines;
    strlist *undefines;
    strlist *c_includes;
} C_FLAGS;

typedef struct _aflags {
	bool i2_68010;				/* -1, >= 68010 */
	bool i2_68020;				/* -2, >= 68020 */
	bool i2_68030;				/* -3, 68030 */
	bool i2_68040;				/* -4, 68040 */
	bool i2_68851;				/* -5, 68851 */
	bool i2_68060;				/* -6, 68060 */
	bool Coldfire;				/* -7, Coldfire (double is 64 bits) */
	bool use_FPU;				/* -8, enable floating point h/w */
	bool output_DRI;			/* -B, DRI object output */

	char *output_directory;			 /* -N, output directory */
	char *output_name;				 /* -O, output file name */
	bool supervisor;			     /* -S, default .super in assembly */
	bool debug_infos;				 /* -Y, generate debug information */
	bool undefined_external;	     /* -U, undefined symbols are external */
	short verbose;					 /* -V, number of v's = level of verbosity */
	bool no_include_line_listing;    /* -C */
	bool no_macro_line_listing;      /* -M */
	bool no_false_condition_listing; /* -F */
	bool list_all_macro_lines;       /* -A */
	bool print_listing;              /* -P */
	
	bool no_output;
	
    strlist *defines;
	strlist *as_includes;
} A_FLAGS;

typedef struct
{
	bool no_fastload;			/* -F, dont set fastload bit */
	bool malloc_for_stram;		/* -M, mallocs for ST-RAM */
	bool program_to_stram;		/* -R, load program to ST-RAM */
	bool create_new_object;		/* -J, make new object file (library) */
	short verbose;				/* -V, verbosity */
	bool debug_infos;			/* -Y, generate debug infos */
	bool load_map;				/* generate load map */
	bool global_symbols;		/* -G, add global symbols */
	bool local_symbols;			/* -L, add local symbols */
	bool nm_list;				/* display 'nm' symbol list */
	long heap_size;
	long text_start;
	long data_start;
	long bss_start;
	bool binary;				/* omit executable header */
	long imgsize;
	char *output_filename;
	long stacksize;
} LD_FLAGS;

typedef enum
{
	FT_UNKNOWN = 0,
	FT_NONE = ' ',
	FT_CSOURCE = 'c',
	FT_ASSOURCE = 's',
	FT_OBJECT = 'o',
	FT_HEADER = 'h',
	FT_LIBRARY = 'l',
	FT_SHAREDLIB = 'S',
	FT_PROGRAM = 'p',
	FT_PROJECT = 'P',
} FILETYPE;

typedef struct project PRJ;

typedef struct _filearg filearg;
struct _filearg {
	filearg *next;

	strlist *dependencies;
	FILETYPE filetype;
	union {
		C_FLAGS *cflags;
		A_FLAGS *aflags;
		PRJ *prj;
	} u;
	time_t src_time;
	time_t obj_time;
	char name[1];
};

struct project
{
	char *filename;
	char *directory;
	LD_FLAGS ld_flags;
	C_FLAGS c_flags;
	A_FLAGS a_flags;
	filearg *inputs;
	filearg *output;
	bool needs_ahcc;
};

typedef struct _makeopts {
	bool make_all;
	int verbose;
	bool ignore_date;
	bool silent;
	bool debug;
	bool nfdebug;
	bool dryrun;
	bool print_directory;
	char *directory;
} MAKEOPTS;

#define _(x) x
#define N_(x) x

extern char const program_name[];

extern char const Warning[];
extern char const Error[];
extern char const suff_o[];
extern char const suff_prg[];
extern bool errout_nfdebug;


void adddef(strlist **defines, const char *str);
void doincl(strlist **includes, const char *str);

void errout_va(const char *format, va_list args) __attribute__((format(printf, 1, 0)));
void errout(const char *format, ...) __attribute__((format(printf, 1, 2)));
void oom(size_t size) __attribute__((noreturn));
void strfreev(char **argv);
char **split_args(const char *argv0, const char *argstring, int *pargc, char delim);

void append_slash(char *s);
char *strrslash(const char *f);
char *basename(const char *f);
bool is_absolute_path(const char *s);
FILETYPE filetype(const char *filename);
char *build_path(const char *dir, const char *fname);
char *change_suffix(const char *filename, const char *ext);
bool file_exists(const char *f);
char *dirname(const char *f);
void strbslash(char *str);
void strfslash(char *str);
char *get_cwd(void);
int ch_dir(const char *path);


int get_warning_level(warning_category category);
int get_warning_idx(warning_category category);
void init_cflags(C_FLAGS *flg);
void free_cflags(C_FLAGS *flg);
C_FLAGS *copy_cflags(const C_FLAGS *src);
bool parse_cc_options(const char *arg, C_FLAGS *flg);

void init_aflags(A_FLAGS *flg);
void free_aflags(A_FLAGS *flg);
A_FLAGS *copy_aflags(const A_FLAGS *src);
bool parse_as_options(const char *arg, A_FLAGS *flg);


void init_ldflags(LD_FLAGS *flg);
void free_ldflags(LD_FLAGS *flg);
bool parse_ld_options(const char *arg, LD_FLAGS *flg);

bool domake(PRJ *prj, MAKEOPTS *opts);

PRJ *loadmake(MAKEOPTS *opts, const char *f);
void free_project(PRJ *prj);

void set_pcdir(const char *argv0);
const char *get_pcdir(void);
const char *get_libdir(void);
const char *get_includedir(void);
const char *get_compiler_executable(void);
const char *get_assembler_executable(void);
const char *get_linker_executable(void);
const char *get_ahcc_executable(void);
const char *get_ahcl_executable(void);
void exec_exit(void);
int linker(int argc, const char **argv);
int compiler(int argc, const char **argv);
