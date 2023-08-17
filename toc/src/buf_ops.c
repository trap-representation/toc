#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "buf_ops.h"

// this function appends the contents of buf upto the pos-1 index to write_buf starting at write_buf_written_contents

unsigned int flush_to_buf(char *buf, size_t *pos, char **write_buf, size_t *write_buf_written_contents){
  if(*pos>0){
    *write_buf=realloc(*write_buf,*write_buf_written_contents+*pos);
    if(*write_buf==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    memcpy(&(*write_buf)[*write_buf_written_contents],buf,*pos);
    *write_buf_written_contents=*write_buf_written_contents+*pos;
    *pos=0;
  }
  return 0;
}

// this function tries writing the character add_char to the buffer buf at the position pos. If however, pos is >= the maximum size of buf - buf_max, and write_thresh is not set, it returns 1; otherwise, if write_thresh is set, it tries to start appending the contents of buf to write_buf starting at position write_buf_written_contents of write_buf

unsigned int append_to_buf(char add_char, char *buf, size_t *pos, size_t buf_max, bool write_thresh, char **write_buf, size_t *write_buf_written_contents){
  if(*pos>=buf_max){
    if(!write_thresh){
      return 1;
    }
    else{
      *write_buf=realloc(*write_buf,*write_buf_written_contents+buf_max);
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      if(*write_buf==NULL){
        return 1;
      }
      memcpy(&write_buf[*write_buf_written_contents],buf,buf_max);
      *write_buf_written_contents=*write_buf_written_contents+buf_max;
      *pos=0;
      return 0;
    }
  }
  else{
    buf[*pos]=add_char;
    *pos=*pos+1;
    return 0;
  }
}
