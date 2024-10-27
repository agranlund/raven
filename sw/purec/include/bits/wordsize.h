/* Determine the wordsize from the preprocessor defines.  */

#if defined __MSHORT__
# define __WORDSIZE	16
#else
# define __WORDSIZE	32
#endif

/* In any case use the 32-bit system call interface.  */
#define __SYSCALL_WORDSIZE		32
