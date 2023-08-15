#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "write_error.h"

#define FILE_READ_CHUNK_SIZE 1048576

void write_error_from_tokens(char *err, token *tok_pri, token *tok_sec, char *file_name){
  token *dis=tok_pri->token_class!=tokens_list_end?tok_pri:tok_sec;
  write_error(err,dis->start_x,dis->start_y,dis->end_x,dis->end_y,file_name);
}

void write_error(char *err, uintmax_t start_x, uintmax_t start_y, uintmax_t end_x, uintmax_t end_y, char *file_name){
  FILE *err_file;
  if((err_file=fopen(file_name,"r"))==NULL){
    fprintf(stderr,"implementation error: failed to open file %s for reading\n",file_name);
    return;
  }
  char *read_buf;
  if((read_buf=malloc(FILE_READ_CHUNK_SIZE))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return;
  }
  size_t read_size;
  bool read_end=false;
  uintmax_t pos_x=1;
  uintmax_t pos_y=1;
  bool write=false;
  int pad;
  int file_pad;
  while(!read_end && (read_size=fread(read_buf,1,FILE_READ_CHUNK_SIZE,err_file))){
    if(ferror(err_file)){
      fprintf(stderr,"implementation error: error while reading from file %s\n",file_name);
      fclose(err_file);
      free(read_buf);
      return;
    }
    for(size_t i=0;i<read_size;){
      if(write){
        fputc(read_buf[i],stderr);
        if(pos_y==end_y && pos_x>end_x){
          if(read_buf[i]=='\n'){
            if(end_y==start_y){
              for(int i=0;i<pad+file_pad;i++){
                fputc(' ',stderr);
              }
              for(uintmax_t i=1;i<start_x;i++){
                fputc(' ',stderr);
              }
              for(uintmax_t i=start_x;i<pos_x;i++){
                fputc('^',stderr);
              }
            }
            else{
              for(int i=0;i<pad+file_pad;i++){
                fputc(' ',stderr);
              }
              for(uintmax_t i=1;i<pos_x;i++){
                fputc('^',stderr);
              }
            }
            fputc('\n',stderr);
            read_end=true;
            break;
          }
        }
        else if(read_buf[i]=='\n'){
          if(end_y<start_y){
            for(int i=0;i<pad+file_pad;i++){
              fputc(' ',stderr);
            }
            for(uintmax_t i=1;i<start_x;i++){
              fputc(' ',stderr);
            }
            fputc('^',stderr);
            fputc('\n',stderr);
            read_end=true;
            break;
          }
          else if(pos_y==start_y){
            for(int i=0;i<pad+file_pad;i++){
              fputc(' ',stderr);
            }
            for(uintmax_t i=1;i<start_x;i++){
              fputc(' ',stderr);
            }
            for(uintmax_t i=start_x;i<pos_x;i++){
              fputc('^',stderr);
            }
          }
          else{
            for(int i=0;i<pad+file_pad;i++){
              fputc(' ',stderr);
            }
            for(uintmax_t i=1;i<pos_x;i++){
              fputc('^',stderr);
            }
          }
          pos_y++;
          pos_x=1;
          fputc('\n',stderr);
          if(pos_y==end_y && end_x==0){
            read_end=true;
            break;
          }
          for(int i=0;i<file_pad;i++){
            fputc(' ',stderr);
          }
          fprintf(stderr,"|%ju:%ju: %n",pos_y,pos_x,&pad);
        }
        else{
          pos_x++;
        }
        i++;
      }
      else{
        if(pos_y==start_y){
          fprintf(stderr,"%s",err);
          fprintf(stderr,"%s%n",file_name,&file_pad);
          fprintf(stderr,":%ju:%ju: %n",pos_y,pos_x,&pad);
          write=true;
        }
        else if(read_buf[i]=='\n'){
          pos_y++;
          pos_x=1;
          i++;
        }
        else{
          pos_x++;
          i++;
        }
      }
    }
  }
  if(ferror(err_file)){
    fprintf(stderr,"implementation error: error while reading from file %s\n",file_name);
  }
  free(read_buf);
  fclose(err_file);
}
