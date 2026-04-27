#ifndef KEYBOARD_H
#define KEYBOARD_H

#define MAX_INPUT 256

int kb_read_line(char *buf);
int readLine(char *buf);
int keyPressed(void);

                                                           
int kb_enable_nonblocking(void);

  
               
           
                                                          
                                       
                           
   
int kb_poll_line(char *out, int out_max);

                                                                   
int kb_input_in_progress(void);

#endif
