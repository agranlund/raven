/*	Laser C object file format definitions
*/

#define  LMAGIC		0x0107	/*	Laser C magic number				*/

/*	Header prepended to each Laser object file.
*/
typedef struct {
	long				a_magic;	/* magic number						*/
	long				a_text;		/* size of text segment				*/
	long				a_data;		/* size of initialized data			*/
	long				a_bss;		/* size of uninitialized data		*/
	long				a_syms;		/* size of symbol table				*/
	long				a_entry;	/* entry point						*/
	long				a_trsize;	/* size of text relocation			*/
	long				a_drsize;	/* size of data relocation			*/
	}
	exec;

/*	Format of a relocation datum.
*/
typedef struct {
	long				r_address;		/* address which is relocated		*/
	unsigned long		r_info;			/* r_symbolnum, r_pcrel, r_length,	*/
										/* r_extern.						*/
	}
	reloc_info;

/*	NOTE:  If r_extern is zero, then r_address is actually and N_TYPE,
		and no symbol entry is present for the relocation.
*/

/*	Fields for r_info (above)
*/
#define  r_symbolnum(x)	((x>>8) & 0xffffffL)
#define  r_pcrel(x)		((x>>7) & 0x1L)
#define  r_length(x)	((x>>5) & 0x3L)
#define  r_extern(x)	((x>>4) & 0x1L)

/*	Symbol table entry
*/
typedef struct {
	char					*n_name;	/* index into string table			*/
	char					n_type;		/* type flag, i.e. N_TEXT etc		*/
	char					n_other;	/* unused							*/
	char					n_desc;		/* currently not used				*/
	long					n_value;	/* value of this sym				*/
	}
	nlist;

/*	Values for n_type (above)
*/
#define  N_UNDF		0x0		/*	undefined							*/
#define  N_ABS		0x2		/*	absolute							*/
#define  N_TEXT		0x4		/*	text								*/
#define  N_DATA		0x6		/*	data								*/
#define  N_BSS		0x8		/*	bss									*/

#define  N_EXT		0x01	/*	external bit, or'ed in				*/
#define  N_TYPE		0x1e	/*	mask for all the type bits			*/

/*	Following the relocation information is a long word (32-bit)
	which tells the length of the string table which follows.
	The length includes the four bytes of the long word (it
	includes own size).  Strings are zero (0) terminated.
*/

/*	GEMDOS executable file format
*/

/*  CP/M-68K header
*/
typedef struct {
    int        c_magic;    /* magic number (0x601A)       */
    long       c_text;     /* size of text segment        */
    long       c_data;     /* size of initialized data    */
    long       c_bss;      /* size of uninitialized data  */
    long       c_syms;     /* size of symbol table        */
    long       c_entry;    /* entry point                 */
    long       c_res;      /* reserved, always zero       */
    int        c_reloc;    /* size of data relocation     */
    } header;

/*  Symbol table entry
*/
typedef struct {
    char       name[8];    /* Symbol name                 */
    int        type;       /* Type (i.e. DEFINED|TEXT_REL)*/
    long       value;      /* Symbol value                */
    } symbol;

/*  CP/M-68K values for symbol types
*/
#define DEFINED       0x8000 /* The symbol is defined      */
#define EQUATED       0x4000 /* The symbol is an equate    */
#define GLOBAL        0x2000 /* The symbol is global       */
#define EQU_REG       0x1000 /* The symbol is a register   */
#define EXTERNAL      0x0800 /* The reference is external  */  
#define DAT_REL       0x0400 /* Data segment reference     */
#define TEX_REL       0x0200 /* Text segment reference     */
#define BSS_REL       0x0100 /* Bss segment reference      */

/* The above values may be OR'd together to indicate
	symbol type.

	One word (16-bit) of relocation information exists for each
	word of TEXT and DATA.  The type of relocation is indicated
	in bits 0-2 of the word.  If the relocation is an external
	reference, the remaining bits (15-3) form an index into the
	symbol table, thus indicating the name of the external
	reference.
*/

/*  CP/M-68K relocation word values (bits 0-2)
*/
#define NO_RELOC      0      /* No relocation necessary    */
#define DATA_BASED    1      /* Relocate from Data segment */
#define TEXT_BASED    2      /* Relocate from Text segment */
#define BSS_BASED     3      /* Relocate from Bss segment  */
#define UNDEF_SYMBOL  4      /* Symbolic reference         */
#define LONG_REF      5      /* Next relocation is long    */
#define PC_RELATIVE   6      /* Is a PC relative reference */
#define INSTRUCTION   7      /* Is an instruction          */

/*	The file format output by the linker (GEMDOS) is identical to the
	DRI object file format excepting the relocation information.  The
	GEMDOS loader will only relocate 32-bit references.  GEMDOS
	relocation information consists of a long (32-bit) word, indicating
	the offset into the program of the first long word to be relocated,
	followed by a series of relocation bytes (8-bit).  These bytes
	indicate the distance from the last offset relocated to the current
	offset to be relocated.  If a relocation byte is equal to 254, the
	last offset is incremented, but no relocation is done.  A
	relocation byte of zero means end-of-relocation-information.
*/

