#ifndef MEMORY_H
#define MEMORY_H

  
                                   
  
                   
                                 
   

void  memory_init(void);
void *my_alloc(unsigned int size);
void  my_dealloc(void *ptr);

                                                     
unsigned int memory_bytes_free(void);
unsigned int memory_bytes_used(void);

#endif

