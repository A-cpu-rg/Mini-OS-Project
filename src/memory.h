#ifndef MEMORY_H
#define MEMORY_H

#define VRAM_SIZE   65536   /* 64 KB virtual RAM */
#define MAX_BLOCKS  256     /* Max allocations at once */

void  mem_init(void);
void *my_alloc(int size);
void  my_dealloc(void *ptr);
int   mem_free_space(void);
void  mem_dump(void);        /* Debug: print allocation table */

#endif
