#ifndef __dsp_reloc_h__
#define __dsp_reloc_h__ 1

struct reloc {
	int32_t  r_vaddr;	/* (virtual) address of reference */
	int32_t  r_symndx;	/* index into symbol table */
	uint32_t r_type;	/* relocation type */
	};

/*
 *   relocation types for all products and generics
 */

/*
 * All generics
 *	reloc. already performed to symbol in the same section
 */
#define  R_ABS		0L
#define RELOC  struct reloc
#define RELSZ sizeof(RELOC)

#endif
