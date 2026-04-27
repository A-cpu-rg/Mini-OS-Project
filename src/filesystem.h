#ifndef FILESYSTEM_H
#define FILESYSTEM_H

  
                                                
                                                    
   

#define FS_MAX_FILES   64
#define FS_NAME_MAX    32

typedef struct {
    int           used;
    char          name[FS_NAME_MAX];
    unsigned int  size;
    char         *data;                               
} File;

void fs_init(void);

                                        
int  fs_touch(const char *name);
int  fs_write(const char *name, const char *data);

                                          
int  fs_read(const char *name, const char **out_data, unsigned int *out_size);

                                                  
int  fs_list(char out_names[][FS_NAME_MAX], int max);

                                                            
int  fs_list_detailed(char out_names[][FS_NAME_MAX], unsigned int out_sizes[], int max);

                                                             
int  fs_delete(const char *name);

#endif

