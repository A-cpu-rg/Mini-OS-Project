  
                                               
  
               
                                              
                     
   
#include "filesystem.h"
#include "memory.h"
#include "string.h"

static File files[FS_MAX_FILES];
static int  fs_inited = 0;

static void zero_files(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        files[i].used = 0;
        files[i].name[0] = '\0';
        files[i].size = 0;
        files[i].data = (char *)0;
    }
}

void fs_init(void) {
    zero_files();
    fs_inited = 1;
}

static int name_valid(const char *name) {
    if (!name) return 0;
    if (name[0] == '\0') return 0;
    if (my_strlen(name) >= FS_NAME_MAX) return 0;
    return 1;
}

static int find_file(const char *name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].used && my_strcmp(files[i].name, name) == 0) return i;
    }
    return -1;
}

static int find_free_slot(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!files[i].used) return i;
    }
    return -1;
}

int fs_touch(const char *name) {
    if (!fs_inited) fs_init();
    if (!name_valid(name)) return 0;
    if (find_file(name) >= 0) return 1;

    int idx = find_free_slot();
    if (idx < 0) return 0;

    files[idx].used = 1;
    my_strcpy(files[idx].name, name);
    files[idx].size = 0;
    files[idx].data = (char *)0;
    return 1;
}

int fs_write(const char *name, const char *data) {
    if (!fs_inited) fs_init();
    if (!name_valid(name)) return 0;
    if (!data) data = "";

    int idx = find_file(name);
    if (idx < 0) {
        idx = find_free_slot();
        if (idx < 0) return 0;
        files[idx].used = 1;
        my_strcpy(files[idx].name, name);
        files[idx].size = 0;
        files[idx].data = (char *)0;
    }

    if (files[idx].data) {
        my_dealloc(files[idx].data);
        files[idx].data = (char *)0;
        files[idx].size = 0;
    }

    unsigned int len = (unsigned int)my_strlen(data);
    char *buf = (char *)my_alloc(len + 1u);
    if (!buf) return 0;

    for (unsigned int i = 0; i < len; i++) buf[i] = data[i];
    buf[len] = '\0';

    files[idx].data = buf;
    files[idx].size = len;
    return 1;
}

int fs_read(const char *name, const char **out_data, unsigned int *out_size) {
    if (!fs_inited) fs_init();
    if (!name_valid(name)) return 0;
    if (!out_data || !out_size) return 0;

    int idx = find_file(name);
    if (idx < 0) return 0;

    *out_data = files[idx].data ? files[idx].data : "";
    *out_size = files[idx].size;
    return 1;
}

int fs_list(char out_names[][FS_NAME_MAX], int max) {
    if (!fs_inited) fs_init();
    if (!out_names || max <= 0) return 0;
    int n = 0;
    for (int i = 0; i < FS_MAX_FILES && n < max; i++) {
        if (files[i].used) {
            my_strcpy(out_names[n], files[i].name);
            n++;
        }
    }
    return n;
}

int fs_list_detailed(char out_names[][FS_NAME_MAX], unsigned int out_sizes[], int max) {
    if (!fs_inited) fs_init();
    if (!out_names || !out_sizes || max <= 0) return 0;
    int n = 0;
    for (int i = 0; i < FS_MAX_FILES && n < max; i++) {
        if (files[i].used) {
            my_strcpy(out_names[n], files[i].name);
            out_sizes[n] = files[i].size;
            n++;
        }
    }
    return n;
}

int fs_delete(const char *name) {
    if (!fs_inited) fs_init();
    if (!name_valid(name)) return 0;

    int idx = find_file(name);
    if (idx < 0) return 0;

    if (files[idx].data) my_dealloc(files[idx].data);
    files[idx].used = 0;
    files[idx].name[0] = '\0';
    files[idx].size = 0;
    files[idx].data = (char *)0;
    return 1;
}

