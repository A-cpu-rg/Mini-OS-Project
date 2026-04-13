#ifndef FS_H
#define FS_H

#define FS_MAX_FILES    64
#define FS_MAX_NAME     48
#define FS_MAX_DATA   1024

/* ── Single file entry ── */
typedef struct {
    char name[FS_MAX_NAME];
    char data[FS_MAX_DATA];
    int  size;
    int  used;
} FSEntry;

void fs_init   (void);
int  fs_create (const char *name);              /* 0=ok  -1=exists  -2=full */
int  fs_write  (const char *name, const char *data); /* 0=ok  -1=not found */
int  fs_read   (const char *name, char *buf, int buf_size); /* len or -1    */
int  fs_delete (const char *name);              /* 0=ok  -1=not found       */
int  fs_ls     (char *buf, int buf_size);       /* returns file count        */
int  fs_count  (void);
int  fs_exists (const char *name);

#endif /* FS_H */
