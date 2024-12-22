#include <stdbool.h>
#include <stddef.h>

#if 1

#include <stdint.h>

extern uint8_t ta_base[0x1000*16];

#define TA_ALIGN		(4)						// Word size for pointer alignment
#define TA_BASE			(ta_base)				// Address of tinyalloc control data structure
#define TA_HEAP_START	(0x8000)					// Heap space start address
#define TA_HEAP_LIMIT	((1024*1024*16) - 1)	// Heap space end address
#define TA_HEAP_BLOCKS	(256)					// Max. number of memory chunks
#define TA_SPLIT_THRESH	(16)					// Size threshold for splitting chunks

//#define TA_DISABLE_COMPACT
//#define TA_DISABLE_SPLIT

#endif

bool ta_init();
void *ta_alloc(size_t num);
void *ta_calloc(size_t num, size_t size);
bool ta_free(void *ptr);

size_t ta_num_free();
size_t ta_num_used();
size_t ta_num_fresh();
size_t ta_biggest_block();

bool ta_check();
