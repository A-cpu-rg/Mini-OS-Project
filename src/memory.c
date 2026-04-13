#include "memory.h"
#include "screen.h"
#include "string.h"

/* ── Virtual RAM ── */
static char vram[VRAM_SIZE];

typedef struct {
    int start;   /* Byte offset into vram  */
    int size;    /* Bytes in this block    */
    int used;    /* 1 = occupied, 0 = free */
} Block;

static Block table[MAX_BLOCKS];
static int   vram_top = 0;

/* ────────────────────────────────────────────
   mem_init  — call once at startup
   ──────────────────────────────────────────── */
void mem_init(void) {
    vram_top = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        table[i].start = 0;
        table[i].size  = 0;
        table[i].used  = 0;
    }
    for (int i = 0; i < VRAM_SIZE; i++) vram[i] = 0;
}

/* ────────────────────────────────────────────
   my_alloc  — first-fit: reuse freed blocks
   before expanding vram_top. This is the key
   fix that prevents free-space shrinking.
   ──────────────────────────────────────────── */
void *my_alloc(int size) {
    if (size <= 0) return (void *)0;

    /* PASS 1: scan for a freed block large enough (first-fit reuse) */
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!table[i].used && table[i].size >= size) {
            table[i].used = 1;
            /* Zero-fill reused region */
            for (int j = 0; j < size; j++)
                vram[table[i].start + j] = 0;
            return (void *)(&vram[table[i].start]);
        }
    }

    /* PASS 2: no reusable block — carve fresh from vram_top */
    if (vram_top + size > VRAM_SIZE) return (void *)0;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!table[i].used && table[i].size == 0) {  /* truly empty slot */
            table[i].start = vram_top;
            table[i].size  = size;
            table[i].used  = 1;
            vram_top      += size;
            return (void *)(&vram[table[i].start]);
        }
    }
    return (void *)0;
}

/* ────────────────────────────────────────────
   my_dealloc  — mark block free; zero it out
   ──────────────────────────────────────────── */
void my_dealloc(void *ptr) {
    if (!ptr) return;
    char *p      = (char *)ptr;
    int   offset = (int)(p - vram);
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (table[i].used && table[i].start == offset) {
            for (int j = 0; j < table[i].size; j++)
                vram[table[i].start + j] = 0;
            table[i].used = 0;
            return;
        }
    }
}

/* ────────────────────────────────────────────
   mem_free_space  — count bytes in all freed
   (reusable) slots + bytes never yet touched.
   This is the correct free-space metric.
   ──────────────────────────────────────────── */
int mem_free_space(void) {
    int freed = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!table[i].used && table[i].size > 0)
            freed += table[i].size;
    }
    return (VRAM_SIZE - vram_top) + freed;
}

/* ────────────────────────────────────────────
   mem_dump  — debug: full allocation table
   Uses screen helpers (no stdio/printf).
   ──────────────────────────────────────────── */
void mem_dump(void) {
    char num[20];

    screen_print("\n=== MEMORY DUMP ===\n");

    screen_print("VRAM size  : ");
    my_int_to_str(VRAM_SIZE, num);
    screen_print(num);
    screen_print(" bytes\n");

    screen_print("vram_top   : ");
    my_int_to_str(vram_top, num);
    screen_print(num);
    screen_print(" bytes\n");

    screen_print("Free       : ");
    my_int_to_str(mem_free_space(), num);
    screen_print(num);
    screen_print(" bytes\n");

    screen_print("-------------------\n");

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (table[i].used) {
            screen_print("Block ");
            my_int_to_str(i, num);
            screen_print(num);
            screen_print(" | start=");
            my_int_to_str(table[i].start, num);
            screen_print(num);
            screen_print(" | size=");
            my_int_to_str(table[i].size, num);
            screen_print(num);
            screen_print("\n");
        }
    }

    screen_print("===================\n");
}