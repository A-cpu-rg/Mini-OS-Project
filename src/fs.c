/*
 * fs.c  —  Virtual File System
 * No stdio / stdlib. Uses only string.h (custom) for helpers.
 */
#include "fs.h"
#include "string.h"

static FSEntry  table[FS_MAX_FILES];
static int      file_count = 0;

/* ── helpers ────────────────────────────────────────── */
static int find(const char *name) {
    for (int i = 0; i < FS_MAX_FILES; i++)
        if (table[i].used && my_strcmp(table[i].name, name) == 0)
            return i;
    return -1;
}

static int empty_slot(void) {
    for (int i = 0; i < FS_MAX_FILES; i++)
        if (!table[i].used) return i;
    return -1;
}

/* Copy at most n-1 chars, always null-terminate */
static void safe_copy(char *dst, const char *src, int max) {
    int i = 0;
    while (i < max - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

/* ── public API ─────────────────────────────────────── */
void fs_init(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        table[i].used = 0;
        table[i].size = 0;
        table[i].name[0] = '\0';
        table[i].data[0] = '\0';
    }
    file_count = 0;
}

int fs_exists(const char *name) { return find(name) >= 0; }

int fs_create(const char *name) {
    if (!name || name[0] == '\0') return -3;
    if (find(name) >= 0)          return -1;   /* already exists */
    int slot = empty_slot();
    if (slot < 0)                 return -2;   /* full            */

    safe_copy(table[slot].name, name, FS_MAX_NAME);
    table[slot].data[0] = '\0';
    table[slot].size    = 0;
    table[slot].used    = 1;
    file_count++;
    return 0;
}

int fs_write(const char *name, const char *data) {
    int idx = find(name);
    if (idx < 0) return -1;

    int dlen = my_strlen(data);
    if (dlen >= FS_MAX_DATA) dlen = FS_MAX_DATA - 1;

    safe_copy(table[idx].data, data, FS_MAX_DATA);
    table[idx].size = dlen;
    return 0;
}

int fs_read(const char *name, char *buf, int buf_size) {
    int idx = find(name);
    if (idx < 0) return -1;

    int len = table[idx].size;
    if (len >= buf_size) len = buf_size - 1;
    safe_copy(buf, table[idx].data, len + 1);
    buf[len] = '\0';
    return len;
}

int fs_delete(const char *name) {
    int idx = find(name);
    if (idx < 0) return -1;

    table[idx].used    = 0;
    table[idx].size    = 0;
    table[idx].name[0] = '\0';
    table[idx].data[0] = '\0';
    file_count--;
    return 0;
}

/*
 * fs_ls — writes lines "filename (Nb)\n" into buf.
 * Returns number of files listed.
 */
int fs_ls(char *buf, int buf_size) {
    int pos   = 0;
    int count = 0;

    for (int i = 0; i < FS_MAX_FILES && pos < buf_size - 60; i++) {
        if (!table[i].used) continue;

        /* name */
        int nlen = my_strlen(table[i].name);
        safe_copy(buf + pos, table[i].name, buf_size - pos);
        pos += nlen;

        /* size tag */
        buf[pos++] = ' ';
        buf[pos++] = '(';
        char num[12];
        my_int_to_str(table[i].size, num);
        int nl = my_strlen(num);
        safe_copy(buf + pos, num, buf_size - pos);
        pos += nl;
        buf[pos++] = ' ';
        buf[pos++] = 'B';
        buf[pos++] = ')';
        buf[pos++] = '\n';
        count++;
    }
    buf[pos] = '\0';
    return count;
}

int fs_count(void) { return file_count; }
