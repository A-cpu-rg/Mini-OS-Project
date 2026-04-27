  
                                                       
  
               
                   
                     
   
#include "memory.h"

#define RAM_SIZE (1024u * 1024u)          

typedef struct BlockHeader {
    unsigned int       size;                    
    unsigned int       free;                         
    struct BlockHeader *next;
} BlockHeader;

static unsigned char ram[RAM_SIZE];
static BlockHeader  *head = (BlockHeader *)0;
static int           inited = 0;

static unsigned int align8(unsigned int n) {
    return (n + 7u) & ~7u;
}

void memory_init(void) {
    head = (BlockHeader *)(void *)ram;
    head->size = RAM_SIZE - (unsigned int)sizeof(BlockHeader);
    head->free = 1u;
    head->next = (BlockHeader *)0;
    inited = 1;
}

static void split_block(BlockHeader *b, unsigned int req) {
                                                                   
    unsigned char *base = (unsigned char *)(void *)b;
    BlockHeader *n = (BlockHeader *)(void *)(base + sizeof(BlockHeader) + req);
    n->size = b->size - req - (unsigned int)sizeof(BlockHeader);
    n->free = 1u;
    n->next = b->next;
    b->size = req;
    b->next = n;
}

void *my_alloc(unsigned int size) {
    if (size == 0) return (void *)0;
    if (!inited) memory_init();

    unsigned int req = align8(size);

    BlockHeader *cur = head;
    while (cur) {
        if (cur->free && cur->size >= req) {
            unsigned int remaining = cur->size - req;
            if (remaining >= (unsigned int)sizeof(BlockHeader) + 8u) {
                split_block(cur, req);
            }
            cur->free = 0u;
            return (void *)(cur + 1);
        }
        cur = cur->next;
    }
    return (void *)0;                 
}

static void coalesce_all(void) {
    BlockHeader *cur = head;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            unsigned char *end = (unsigned char *)(void *)cur
                               + sizeof(BlockHeader)
                               + cur->size;
            if (end == (unsigned char *)(void *)cur->next) {
                cur->size += (unsigned int)sizeof(BlockHeader) + cur->next->size;
                cur->next = cur->next->next;
                continue;
            }
        }
        cur = cur->next;
    }
}

void my_dealloc(void *ptr) {
    if (!ptr) return;
    if (!inited) return;

                                                                     
    unsigned char *p = (unsigned char *)ptr;
    unsigned char *start = ram + sizeof(BlockHeader);
    unsigned char *end   = ram + RAM_SIZE;
    if (p < start || p >= end) return;

    BlockHeader *b = ((BlockHeader *)ptr) - 1;
    unsigned char *bh = (unsigned char *)(void *)b;
    if (bh < ram || bh + sizeof(BlockHeader) > end) return;

    b->free = 1u;
    coalesce_all();
}

unsigned int memory_bytes_free(void) {
    if (!inited) memory_init();
    unsigned int total = 0;
    for (BlockHeader *cur = head; cur; cur = cur->next)
        if (cur->free) total += cur->size;
    return total;
}

unsigned int memory_bytes_used(void) {
    if (!inited) memory_init();
    unsigned int total = 0;
    for (BlockHeader *cur = head; cur; cur = cur->next)
        if (!cur->free) total += cur->size;
    return total;
}

