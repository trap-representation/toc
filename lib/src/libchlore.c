/* assumptions:
   The setlocale() function is never used
   The character set of the execution environment only consists of character-identifier and punctuator as defined in the Chlore grammar
*/

#define _POSIX_C_SOURCE 200809L

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/select.h>

#define LIB_NVM_IMPLEMENTATION_H "../../../nightVM/src/implementation.h"

#include LIB_NVM_IMPLEMENTATION_H

#define dl_params int argc, char **argv, nightVM_l *stack, void **code, nightVM_ui code_alignment, void **heap, nightVM_ui heap_alignment, nightVM_l reg_ssz_val, nightVM_l *reg_hsz_val, nightVM_l *reg_sp_val, nightVM_l *reg_cs_val, nightVM_l reg_pc_val, nightVM_l reg_lop_val, nightVM_l *call_stack, nightVM_l reg_clp_val, nightVM_l *gpr

#define dl_pass argc,argv,stack,code,code_alignment,heap,heap_alignment,reg_ssz_val,reg_hsz_val,reg_sp_val,reg_cs_val,reg_pc_val,reg_lop_val,call_stack,reg_clp_val,gpr

#define NULL_PTR 4611686018427387903
#define MAX_ADDR_MEM 4611686018427387903
#define EQV_HEAP 0x4000000000000000
#define BARE 0x3fffffffffffffff

#define TYP_HEAP 0x4000000000000000

static nightVM_l tag_heap(nightVM_l ptr){
  return ptr|0x4000000000000000;
}

static nightVM_l ptr_typ(nightVM_l ptr){
  return ptr&0x4000000000000000;
}

static nightVM_l rem_tag(nightVM_l ptr){
  if(ptr_typ(ptr)==TYP_HEAP){
    return ptr&0x3fffffffffffffff;
  }
  else{
    return ptr;
  }
}

#define Chlore_EOF -1
#define FD_STDOUT 1
#define FD_STDERR 2
#define FD_STDIN 0

/*
  errors.chloreh
*/

#define ESUCCESS 0
#define EINT 1
#define EOVF 2
#define EPERR 3
#define ECLOSE 4
#define EINP 5
#define EREAD 6
#define EWRITE 7
#define EOPEN 8
#define ESEEK 9
#define ELOCALE 10
#define EALLOC 11
#define ECRTSOCK 12
#define ESOCKCON 13
#define ERECV 14
#define ESEND 15
#define ECRTWATCH 16
#define EWATCH 17

unsigned int errn=ESUCCESS;

char *errors[]={"success\n", "internal error\n", "heap overflow\n", "print error\n", "failed to close file\n", "input error\n", "failed to read from file\n", "failed to write to file\n", "failed to open file\n", "seek failed\n", "failed setting locale\n", "internal allocation failure\n", "failed to create socket\n", "failed to establish connection\n", "failed to received from socket\n", "failed to send to socket\n", "failed to create watch\n", "error while watching\n"};

void perr(dl_params){
  char *s;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    if((s=cseq2str(&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])]))==NULL){
      errn=EALLOC;
      gpr[0]=Chlore_EOF;
      *reg_sp_val-=1;
      return;
    }
  }
  else{
    if((s=cseq2str(&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])]))==NULL){
      errn=EALLOC;
      gpr[0]=Chlore_EOF;
      *reg_sp_val-=1;
      return;
    }
  }
  if(write(FD_STDERR,s,strlen(s))==-1){
    free(s);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  free(s);
  if(write(FD_STDERR,errors[errn],strlen(errors[errn]))==-1){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
  }
  else{
    gpr[0]=0;
  }
  *reg_sp_val-=1;
}

void str_err(dl_params){
  stack[*reg_sp_val-2]=rem_tag(stack[*reg_sp_val-2]);
  cseqcpystr(&((nightVM_c *)*heap)[stack[*reg_sp_val-2]],errors[errn],stack[*reg_sp_val-1]);
  gpr[0]=0;
  *reg_sp_val-=2;
}

void get_errn(dl_params){
  stack[*reg_sp_val]=errn;
  gpr[0]=errn;
  *reg_sp_val+=1;
}

/*
  alloc2.chloreh
*/

typedef struct list_alloc{
  nightVM_l addr;
  size_t size;
  bool is_free;
  struct list_alloc *next_seg;
  struct list_alloc *prev_seg;
} lalloc;
static lalloc *alist_head=NULL;

/*#define CACHE_BLOCK_SIZE 64 //this is the cache block size. The more the size, the lesser will be the cache misses but the more will be the performance hit during a cache miss
  static lalloc *alloc_cache_chunk[CACHE_BLOCK_SIZE];
  static uintmax_t cache_hits[CACHE_BLOCK_SIZE];*/

static size_t seg_size_to_copy=0;

static uintmax_t tot_seg=1;

/*static uintmax_t free_in_cache=1;*/

static size_t tot_size=1;

/*void push_to_cache(lalloc *ptr){
  for(size_t i=0;i<CACHE_BLOCK_SIZE;i++){
  if(cache_hits[i]==0){
  if((alloc_cache_chunk[i]==NULL || !alloc_cache_chunk[i]->is_free) && ptr->is_free){
  free_in_cache++;
  }
  else if(alloc_cache_chunk[i]==NULL && !ptr->is_free){;}
  else if(alloc_cache_chunk[i]->is_free && !ptr->is_free){
  free_in_cache--;
  }
  alloc_cache_chunk[i]=ptr;
  cache_hits[i]=64; //64 is the cache hits count - if it falls down to 0, it means that this block can be removed from the cache
  //the more the cache hits count, the longer will a block stay in the cache before being dropped
  break;
  }
  else if(alloc_cache_chunk[i]==ptr){
  if(cache_hits[i]!=ULONG_MAX){
  cache_hits[i]++;
  }
  break;
  }
  }
  }

  void kill_if_in_cache(lalloc *ptr){
  for(size_t i=0;i<CACHE_BLOCK_SIZE;i++){
  if(alloc_cache_chunk[i]==ptr){
  cache_hits[i]=0;
  alloc_cache_chunk[i]=NULL;
  break;
  }
  }
  }*/

void free_chunk(lalloc *chunk, lalloc *lprev, lalloc *lnext){
  if(chunk->prev_seg!=NULL && chunk->prev_seg->is_free){ //check if the previous segment is free
    tot_seg--;
    lprev=chunk->prev_seg;
    chunk->size+=chunk->prev_seg->size;
    chunk->addr=chunk->prev_seg->addr;
    if(chunk->prev_seg==alist_head){
      alist_head=chunk;
      alist_head->prev_seg=NULL;
    }
    else{
      chunk->prev_seg=chunk->prev_seg->prev_seg;
      chunk->prev_seg->next_seg=chunk;
    }
  }
  if(chunk->next_seg!=NULL && chunk->next_seg->is_free){ //check if the next segment is free as well
    tot_seg--;
    lnext=chunk->next_seg;
    chunk->size+=chunk->next_seg->size;
    chunk->next_seg=chunk->next_seg->next_seg;
    if(chunk->next_seg!=NULL){
      chunk->next_seg->prev_seg=chunk;
    }
  }
  //depending on whether the previous chunk or the next chunk or both are free, we'll merge those chunks to create a new big chunk with size equal to the sum of sizes of the three (or two) chunks.
  //if neither of them are free, however, we'll just free the chunk that we have received a pointer to
  chunk->is_free=true;
  if(lprev!=NULL){
    /*    kill_if_in_cache(lprev);*/
    free(lprev);
  }
  if(lnext!=NULL){
    /*    kill_if_in_cache(lnext);*/
    free(lnext);
  }
}

void aaseg(dl_params){
  *reg_sp_val-=2;
  if(alist_head==NULL){
    if(*reg_hsz_val==0){
      stack[*reg_sp_val]=NULL_PTR;
      gpr[0]=1;
      errn=EINT;
      *reg_sp_val+=1;
      return;
    }
    alist_head=malloc(sizeof(*alist_head));
    if(alist_head==NULL){
      gpr[0]=1;
      errn=EINT;
      return;
    }
    alist_head->addr=0;
    alist_head->size=1;
    alist_head->is_free=true;
    alist_head->next_seg=NULL;
    alist_head->prev_seg=NULL;
  }
  if(stack[*reg_sp_val]==0){
    stack[*reg_sp_val]=NULL_PTR;
    gpr[0]=0;
    *reg_sp_val+=1;
    return;
  }
  /*  //check if a segment exists in the cache such that it is free, has a size==stack[*reg_sp_val], and is properly aligned. If there is, simply return that
      if(tot_seg>10 && free_in_cache>7){ // if there isn't at least 11 segments allocated, it's probably not worth peeking into the cache
      // if there isn't at least 8 free chunks in the cache, don't bother peeking into the cache
      for(size_t i=0;i<CACHE_BLOCK_SIZE;i++){
      if(alloc_cache_chunk[i]!=NULL){
      if(alloc_cache_chunk[i]->size==(size_t)stack[*reg_sp_val] && alloc_cache_chunk[i]->is_free && alloc_cache_chunk[i]->addr%stack[*reg_sp_val+1]==0){
      free_in_cache--; //we have one less free item in the cache
      if(cache_hits[i]!=ULONG_MAX){
      cache_hits[i]++; //we have a cache hit
      }
      stack[*reg_sp_val]=tag_heap(alloc_cache_chunk[i]->addr);
      alloc_cache_chunk[i]->is_free=false;
        gpr[0]=0;
      return;
      }
      else{
      cache_hits[i]--; //we have a cache miss
      }
      }
      else{
      break;
      }
      }
      }*/
  size_t best_fit=0;
  lalloc *ret_seg=NULL;
  lalloc *last_seg=NULL;
  lalloc *alist_trav=alist_head;
  //if there exists no such segment, traverse through the entire linked list to find a best fit
  while(alist_trav!=NULL){
    if(alist_trav->is_free && alist_trav->size>=(size_t)stack[*reg_sp_val] && alist_trav->size-stack[*reg_sp_val]>best_fit && alist_trav->addr%stack[*reg_sp_val+1]==0){ //always take segments which leaves off the most free space after allocation (and is properly aligned, of course)
      best_fit=alist_trav->size-stack[*reg_sp_val];
      ret_seg=alist_trav;
    }
    last_seg=alist_trav;
    alist_trav=alist_trav->next_seg;
  }
  if(best_fit>0){
    tot_seg++;
    ret_seg->is_free=false;
    ret_seg->size=stack[*reg_sp_val];
    alist_trav=ret_seg->next_seg; //creating another segment next to the segment we've got since we have more space than we need and creating another segment will let us use that leftover space later
    ret_seg->next_seg=malloc(sizeof(*ret_seg));
    if(ret_seg->next_seg==NULL){
      stack[*reg_sp_val]=NULL_PTR;
      gpr[0]=1;
      errn=EINT;
      *reg_sp_val+=1;
      return;
    }
    ret_seg->next_seg->size=best_fit;
    ret_seg->next_seg->addr=ret_seg->addr+stack[*reg_sp_val];
    ret_seg->next_seg->is_free=true;
    ret_seg->next_seg->prev_seg=ret_seg;
    ret_seg->next_seg->next_seg=alist_trav;
    if(ret_seg->next_seg->next_seg!=NULL){
      ret_seg->next_seg->next_seg->prev_seg=ret_seg->next_seg;
    }
    stack[*reg_sp_val]=tag_heap(ret_seg->addr);
  }
  else if(best_fit==0){
    if(last_seg->is_free && last_seg->addr%stack[*reg_sp_val+1]==0){ //checking if the last segment is free, has enough space, and is properly aligned. If it is, just increase the size of the last segment to what we actually need
      tot_size=tot_size+stack[*reg_sp_val]-last_seg->size;
      if(tot_size>(size_t)*reg_hsz_val){
        stack[*reg_sp_val]=NULL_PTR;
        gpr[0]=1;
        errn=EOVF;
        *reg_sp_val+=1;
        return;
      }
      last_seg->size=stack[*reg_sp_val];
      last_seg->is_free=false;
      stack[*reg_sp_val]=tag_heap(last_seg->addr);
      ret_seg=last_seg;
    }
    else{ //if not, we'll have to create a new segment, unfortunately
      tot_seg++;
      last_seg->next_seg=malloc(sizeof(*alist_trav));
      if(last_seg->next_seg==NULL){
        stack[*reg_sp_val]=NULL_PTR;
        gpr[0]=1;
        errn=EINT;
        *reg_sp_val+=1;
        return;
      }
      nightVM_l aligned_seg_loc=(tot_size+stack[*reg_sp_val+1]-1)-((tot_size+stack[*reg_sp_val+1]-1)%stack[*reg_sp_val+1]);
      if(last_seg->is_free){
        last_seg->size+=aligned_seg_loc-tot_size;
      }
      else{
        last_seg->next_seg->size=aligned_seg_loc-tot_size;
        last_seg->next_seg->addr=tot_size;
        last_seg->next_seg->prev_seg=last_seg;
        last_seg->next_seg->next_seg=malloc(sizeof(*alist_trav));
        last_seg->next_seg->is_free=true;
        last_seg=last_seg->next_seg;
      }
      last_seg->next_seg->size=stack[*reg_sp_val];
      last_seg->next_seg->addr=aligned_seg_loc;
      last_seg->next_seg->prev_seg=last_seg;
      last_seg->next_seg->next_seg=NULL;
      last_seg->next_seg->is_free=false;
      tot_size=aligned_seg_loc+stack[*reg_sp_val];
      if(tot_size>(size_t)*reg_hsz_val){
        stack[*reg_sp_val]=NULL_PTR;
        gpr[0]=1;
        errn=EOVF;
        *reg_sp_val+=1;
        return;
      }
      stack[*reg_sp_val]=tag_heap(last_seg->next_seg->addr);
      ret_seg=last_seg->next_seg;
    }
  }
  else{ //we have found a segment with exactly the amount of space we need
    stack[*reg_sp_val]=tag_heap(ret_seg->addr);
    ret_seg->is_free=false;
  }
  /*  if(tot_seg>10){
      push_to_cache(ret_seg); //try and see if something is removable in the cache or not, and push it in the cache if it is, replacing the removable item with `ret_seg'
      }*/
  gpr[0]=0;
  *reg_sp_val+=1;
}
void aseg(dl_params){
  *reg_sp_val-=1;
  if(alist_head==NULL){
    if(*reg_hsz_val==0){
      stack[*reg_sp_val]=NULL_PTR;
      gpr[0]=1;
      errn=EINT;
      *reg_sp_val+=1;
      return;
    }
    alist_head=malloc(sizeof(*alist_head));
    if(alist_head==NULL){
      gpr[0]=1;
      errn=EINT;
      return;
    }
    alist_head->addr=0;
    alist_head->size=1;
    alist_head->is_free=true;
    alist_head->next_seg=NULL;
    alist_head->prev_seg=NULL;
  }
  if(stack[*reg_sp_val]==0){
    stack[*reg_sp_val]=NULL_PTR;
    gpr[0]=0;
    *reg_sp_val+=1;
    return;
  }
  /*  //check if a segment exists in the cache such that it is free and has a size==stack[*reg_sp_val]. If there is, simply return that
      if(tot_seg>10 && free_in_cache>7){ // if there isn't at least 11 segments allocated, it's probably not worth peeking into the cache
      // if there isn't at least 8 free chunks in the cache, don't bother peeking into the cache
      for(size_t i=0;i<CACHE_BLOCK_SIZE;i++){
      if(alloc_cache_chunk[i]!=NULL){
      if(alloc_cache_chunk[i]->size==(size_t)stack[*reg_sp_val] && alloc_cache_chunk[i]->is_free){
      free_in_cache--; //we have one less free item in the cache
      if(cache_hits[i]!=ULONG_MAX){
      cache_hits[i]++; //we have a cache hit
      }
      stack[*reg_sp_val]=tag_heap(alloc_cache_chunk[i]->addr);
      alloc_cache_chunk[i]->is_free=false;
        gpr[0]=0;
      return;
      }
      else{
      cache_hits[i]--; //we have a cache miss
      }
      }
      else{
      break;
      }
      }
      }*/
  size_t best_fit=0;
  lalloc *ret_seg=NULL;
  lalloc *last_seg=NULL;
  lalloc *alist_trav=alist_head;
  //if there exists no such segment, traverse through the entire linked list to find a best fit
  while(alist_trav!=NULL){
    if(alist_trav->is_free && alist_trav->size>=(size_t)stack[*reg_sp_val] && alist_trav->size-stack[*reg_sp_val]>best_fit){ //always take segments which leaves off the most free space after allocation
      best_fit=alist_trav->size-stack[*reg_sp_val];
      ret_seg=alist_trav;
    }
    last_seg=alist_trav;
    alist_trav=alist_trav->next_seg;
  }
  if(best_fit>0){
    tot_seg++;
    ret_seg->is_free=false;
    ret_seg->size=stack[*reg_sp_val];
    alist_trav=ret_seg->next_seg; //creating another segment next to the segment we've got since we have more space than we need and creating another segment will let us use that leftover space later
    ret_seg->next_seg=malloc(sizeof(*ret_seg));
    if(ret_seg->next_seg==NULL){
      stack[*reg_sp_val]=NULL_PTR;
      gpr[0]=1;
      errn=EINT;
      *reg_sp_val+=1;
      return;
    }
    ret_seg->next_seg->size=best_fit;
    ret_seg->next_seg->addr=ret_seg->addr+stack[*reg_sp_val];
    ret_seg->next_seg->is_free=true;
    ret_seg->next_seg->prev_seg=ret_seg;
    ret_seg->next_seg->next_seg=alist_trav;
    if(ret_seg->next_seg->next_seg!=NULL){
      ret_seg->next_seg->next_seg->prev_seg=ret_seg->next_seg;
    }
    stack[*reg_sp_val]=tag_heap(ret_seg->addr);
  }
  else if(best_fit==0){
    if(last_seg->is_free){ //checking if the last segment is free and has enough space
      tot_size=tot_size+stack[*reg_sp_val]-last_seg->size;
      if(tot_size>(size_t)*reg_hsz_val){
        stack[*reg_sp_val]=NULL_PTR;
        gpr[0]=1;
        errn=EOVF;
        *reg_sp_val+=1;
        return;
      }
      last_seg->size=stack[*reg_sp_val];
      last_seg->is_free=false;
      stack[*reg_sp_val]=tag_heap(last_seg->addr);
      ret_seg=last_seg;
    }
    else{ //if not, we'll have to create a new segment, sadly
      tot_seg++;
      last_seg->next_seg=malloc(sizeof(*alist_trav));
      if(last_seg->next_seg==NULL){
        stack[*reg_sp_val]=NULL_PTR;
        gpr[0]=1;
        errn=EINT;
        *reg_sp_val+=1;
        return;
      }
      last_seg->next_seg->size=stack[*reg_sp_val];
      last_seg->next_seg->addr=tot_size;
      last_seg->next_seg->is_free=false;
      last_seg->next_seg->prev_seg=last_seg;
      last_seg->next_seg->next_seg=NULL;
      tot_size=tot_size+stack[*reg_sp_val];
      if(tot_size>(size_t)*reg_hsz_val){
        stack[*reg_sp_val]=NULL_PTR;
        gpr[0]=1;
        errn=EOVF;
        *reg_sp_val+=1;
        return;
      }
      stack[*reg_sp_val]=tag_heap(last_seg->next_seg->addr);
      ret_seg=last_seg->next_seg;
    }
  }
  else{ //we have found a segment with exactly the amount of space we need
    stack[*reg_sp_val]=tag_heap(ret_seg->addr);
    ret_seg->is_free=false;
  }
  /*  if(tot_seg>10){
      push_to_cache(ret_seg); //try and see if something is removable in the cache or not, and push it in the cache if it is, replacing the removable item with `ret_seg'
      }*/
  gpr[0]=0;
  *reg_sp_val+=1;
}

void flush(dl_params){
  *reg_sp_val-=1;
  if(alist_head==NULL){
    gpr[0]=1;
    errn=EINT;
    return;
  }
  lalloc *lprev=NULL;
  lalloc *lnext=NULL;
  if(stack[*reg_sp_val]!=NULL_PTR){
    stack[*reg_sp_val]=rem_tag(stack[*reg_sp_val]);
    /*  //fast check for the specified address in the cache
        if(tot_seg>10 && free_in_cache<7){ //if there is less than 7 free blocks in the cache, there might be a chance that the chunk we're looking for is in the cache
        for(size_t i=0;i<CACHE_BLOCK_SIZE;i++){
        if(alloc_cache_chunk[i]!=NULL){
        if(alloc_cache_chunk[i]->addr==stack[*reg_sp_val]){
        seg_size_to_copy=alloc_cache_chunk[i]->size;
        if(cache_hits[i]!=ULONG_MAX){
        cache_hits[i]++; //it's definitely a cache hit
        }
        if(!alloc_cache_chunk[i]->is_free){
        free_chunk(alloc_cache_chunk[i],lprev,lnext);
          gpr[0]=0;
        return;
        }
        else{
          gpr[0]=0;
        return;
        }
        }
        else{
        cache_hits[i]--; //a cache miss
        }
        }
        else{
        break;
        }
        }
        }*/
    lalloc *alist_trav=alist_head;
    while(alist_trav!=NULL){
      if(alist_trav->addr==stack[*reg_sp_val]){
        seg_size_to_copy=alist_trav->size;
        if(!alist_trav->is_free){
          free_chunk(alist_trav,lprev,lnext);
          break;
        }
        else{
          break;
        }
      }
      alist_trav=alist_trav->next_seg;
    }
  }
  gpr[0]=0;
}

void rseg(dl_params){
  if(alist_head==NULL){
    if(*reg_hsz_val==0){
      stack[*reg_sp_val-2]=NULL_PTR;
      gpr[0]=1;
      errn=EINT;
      *reg_sp_val-=1;
      return;
    }
    alist_head=malloc(sizeof(*alist_head));
    if(alist_head==NULL){
      gpr[0]=1;
      errn=EINT;
      return;
    }
    alist_head->addr=0;
    alist_head->size=1;
    alist_head->is_free=true;
    alist_head->next_seg=NULL;
    alist_head->prev_seg=NULL;
  }
  nightVM_l realloc_seg=0;
  if(stack[*reg_sp_val-1]!=NULL_PTR){
    realloc_seg=rem_tag(stack[*reg_sp_val-1]);
    flush(dl_pass); //flush the segment that we want to reallocate first
    if((size_t)stack[*reg_sp_val-1]<seg_size_to_copy){
      seg_size_to_copy=stack[*reg_sp_val-1];
    }
  }
  else{
    *reg_sp_val-=1;
    seg_size_to_copy=0;
  }
  if(stack[*reg_sp_val-1]==0){
    stack[*reg_sp_val-1]=NULL_PTR;
    gpr[0]=0;
    return;
  }
  aseg(dl_pass); //and then aseg a new segment
  if(stack[*reg_sp_val-1]!=NULL_PTR){
    memmove(&((unsigned char *)*heap)[rem_tag(stack[*reg_sp_val-1])],&((unsigned char *)*heap)[realloc_seg],seg_size_to_copy); //finally copy the data from the old segment to the new aaseg'ed segment
    //this algorithm basically emulates C equivalent of `free()' (flush) and then `malloc()' (aseg). However, a free and then malloc is not guaranteed to retain the contents of the heap, but this library does guarantee that a flush and an aseg following the flush will retain the contents of the heap unless a data is written to the heap in between the calls, which is impossible (unless this library itself is changed that'll make it do so). So we can just flush the segement that we don't need and then aseg a new segment and finally copy the data from the old (now possibly flush'ed) segment to the new segment
    //this effectively does what we need, so we don't need to write a new algorithm for rseg
  }
}

void arseg(dl_params){
  if(alist_head==NULL){
    if(*reg_hsz_val==0){
      stack[*reg_sp_val-3]=NULL_PTR;
      gpr[0]=1;
      errn=EINT;
      *reg_sp_val-=2;
      return;
    }
    alist_head=malloc(sizeof(*alist_head));
    if(alist_head==NULL){
      gpr[0]=1;
      errn=EINT;
      return;
    }
    alist_head->addr=0;
    alist_head->size=1;
    alist_head->is_free=true;
    alist_head->next_seg=NULL;
    alist_head->prev_seg=NULL;
  }
  nightVM_l realloc_seg=0;
  if(stack[*reg_sp_val-1]!=NULL_PTR){
    realloc_seg=rem_tag(stack[*reg_sp_val-1]);
    flush(dl_pass); //flush the segment that we want to reallocate first
    if((size_t)stack[*reg_sp_val-2]<seg_size_to_copy){
      seg_size_to_copy=stack[*reg_sp_val-2];
    }
  }
  else{
    *reg_sp_val-=1;
    seg_size_to_copy=0;
  }
  if(stack[*reg_sp_val-2]==0){
    stack[*reg_sp_val-2]=NULL_PTR;
    gpr[0]=0;
    *reg_sp_val+=1;
    return;
  }
  aaseg(dl_pass); //and then aaseg a new segment
  if(stack[*reg_sp_val-1]!=NULL_PTR){
    memmove(&((unsigned char *)*heap)[rem_tag(stack[*reg_sp_val-1])],&((unsigned char *)*heap)[realloc_seg],seg_size_to_copy); //finally copy the data from the old segment to the new aseg'ed segment
    //this algorithm basically emulates C equivalent of `free()' (flush) and then an aligned `malloc()' (aaseg). However, a free and then malloc is not guaranteed to retain the contents of the heap, but this library does guarantee that a flush and an aaseg following the flush will retain the contents of the heap unless a data is written to the heap in between the calls, which is impossible (unless this library itself is changed that'll make it do so). So we can just flush the segement that we don't need and then aaseg a new segment and finally copy the data from the old (now possibly flush'ed) segment to the new segment
    //this effectively does what we need, so we don't need to write a new algorithm for arseg
  }
}

/*
  io.chloreh
*/

void out_p(dl_params){
  if(stack[*reg_sp_val-1]==NULL_PTR){
    if(write(FD_STDOUT,"(null)",6)==-1){
      gpr[0]=Chlore_EOF;
      errn=EPERR;
      *reg_sp_val-=1;
      return;
    }
  }
  else{
    char efms[1];
    int n;
    if((n=snprintf(efms,1,"%" PRINVMxUNS,(nightVM_uns)stack[*reg_sp_val-1]))<0){
      gpr[0]=Chlore_EOF;
      errn=EPERR;
      *reg_sp_val-=1;
      return;
    }
    char *fms;
    if((fms=malloc(n+1))==NULL){
      gpr[0]=Chlore_EOF;
      errn=EPERR;
      *reg_sp_val-=1;
      return;
    }
    if((n=snprintf(fms,n+1,"%" PRINVMxUNS,(nightVM_uns)stack[*reg_sp_val-1]))<0){
      free(fms);
      gpr[0]=Chlore_EOF;
      errn=EPERR;
      *reg_sp_val-=1;
      return;
    }
    if(write(FD_STDOUT,fms,n)==-1){
      free(fms);
      gpr[0]=Chlore_EOF;
      errn=EPERR;
      *reg_sp_val-=1;
      return;
    }
    free(fms);
  }
  gpr[0]=0;
  *reg_sp_val-=1;
}

void out_uc(dl_params){
  char efms[1];
  int n;
  if((n=snprintf(efms,1,"%" PRINVMUC,(nightVM_uc)stack[*reg_sp_val-1]))<0){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  char *fms;
  if((fms=malloc(n+1))==NULL){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if((n=snprintf(fms,n+1,"%" PRINVMUC,(nightVM_uc)stack[*reg_sp_val-1]))<0){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if(write(FD_STDOUT,fms,n)==-1){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  free(fms);
  gpr[0]=0;
  *reg_sp_val-=1;
}

void out_us(dl_params){
  char efms[1];
  int n;
  if((n=snprintf(efms,1,"%" PRINVMUS,(nightVM_us)stack[*reg_sp_val-1]))<0){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  char *fms;
  if((fms=malloc(n+1))==NULL){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if((n=snprintf(fms,n+1,"%" PRINVMUS,(nightVM_us)stack[*reg_sp_val-1]))<0){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if(write(FD_STDOUT,fms,n)==-1){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  free(fms);
  gpr[0]=0;
  *reg_sp_val-=1;
}

void out_ui(dl_params){
  char efms[1];
  int n;
  if((n=snprintf(efms,1,"%" PRINVMUI,(nightVM_ui)stack[*reg_sp_val-1]))<0){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  char *fms;
  if((fms=malloc(n+1))==NULL){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if((n=snprintf(fms,n+1,"%" PRINVMUI,(nightVM_ui)stack[*reg_sp_val-1]))<0){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if(write(FD_STDOUT,fms,n)==-1){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  free(fms);
  gpr[0]=0;
  *reg_sp_val-=1;
}

void out_c(dl_params){
  char efms[1];
  int n;
  if((n=snprintf(efms,1,"%" PRINVMC,(nightVM_c)stack[*reg_sp_val-1]))<0){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  char *fms;
  if((fms=malloc(n+1))==NULL){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if((n=snprintf(fms,n+1,"%" PRINVMC,(nightVM_c)stack[*reg_sp_val-1]))<0){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if(write(FD_STDOUT,fms,n)==-1){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  free(fms);
  gpr[0]=0;
  *reg_sp_val-=1;
}

void out_s(dl_params){
  char efms[1];
  int n;
  if((n=snprintf(efms,1,"%" PRINVMS,(nightVM_s)stack[*reg_sp_val-1]))<0){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  char *fms;
  if((fms=malloc(n+1))==NULL){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if((n=snprintf(fms,n+1,"%" PRINVMS,(nightVM_s)stack[*reg_sp_val-1]))<0){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if(write(FD_STDOUT,fms,n)==-1){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  free(fms);
  gpr[0]=0;
  *reg_sp_val-=1;
}

void out_i(dl_params){
  char efms[1];
  int n;
  if((n=snprintf(efms,1,"%" PRINVMI,(nightVM_i)stack[*reg_sp_val-1]))<0){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  char *fms;
  if((fms=malloc(n+1))==NULL){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if((n=snprintf(fms,n+1,"%" PRINVMI,(nightVM_i)stack[*reg_sp_val-1]))<0){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if(write(FD_STDOUT,fms,n)==-1){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  free(fms);
  gpr[0]=0;
  *reg_sp_val-=1;
}

void out_l(dl_params){
  char efms[1];
  int n;
  if((n=snprintf(efms,1,"%" PRINVML,(nightVM_l)stack[*reg_sp_val-1]))<0){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  char *fms;
  if((fms=malloc(n+1))==NULL){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if((n=snprintf(fms,n+1,"%" PRINVML,(nightVM_l)stack[*reg_sp_val-1]))<0){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  if(write(FD_STDOUT,fms,n)==-1){
    free(fms);
    gpr[0]=Chlore_EOF;
    errn=EPERR;
    *reg_sp_val-=1;
    return;
  }
  free(fms);
  gpr[0]=0;
  *reg_sp_val-=1;
}

void out_char(dl_params){
  char c=stack[*reg_sp_val-1];
  if(write(FD_STDOUT,&c,1)==-1){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
  }
  else{
    gpr[0]=0;
  }
  *reg_sp_val-=1;
}

void close_file(dl_params){
  if(close(stack[*reg_sp_val-1])==-1){
    gpr[0]=-1;
    errn=ECLOSE;
  }
  else{
    gpr[0]=0;
  }
  *reg_sp_val-=1;
}

void lseek_file(dl_params){
  if(lseek(stack[*reg_sp_val-3],stack[*reg_sp_val-2],stack[*reg_sp_val-1])==(off_t)-1){
    gpr[0]=-1;
    errn=ESEEK;
  }
  else{
    gpr[0]=0;
  }
  *reg_sp_val-=3;
}

void scanin_str(dl_params){
  stack[*reg_sp_val-1]=rem_tag(stack[*reg_sp_val-1]);
  gpr[0]=0;
  int in;
  if(stack[*reg_sp_val-2]>0){
    ((nightVM_c *)*heap)[stack[*reg_sp_val-1]]=0;
  }
  else{
    return;
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-2];i++){
    if((in=fgetc(stdin))==EOF || in=='\n' || in=='\r'){
      ((nightVM_c *)*heap)[stack[*reg_sp_val-1]+i]='\n';
      ((nightVM_c *)*heap)[stack[*reg_sp_val-1]+i+1]=0;
      break;
    }
    else{
      ((nightVM_c *)*heap)[stack[*reg_sp_val-1]+i]=in;
      ((nightVM_c *)*heap)[stack[*reg_sp_val-1]+i+1]=0;
      gpr[0]++;
    }
  }
  if(in==EOF && ferror(stdin)){
    gpr[0]=Chlore_EOF;
    errn=EINP;
  }
  *reg_sp_val-=2;
}

void scanin_char(dl_params){
  int in;
  in=fgetc(stdin);
  gpr[0]=0;
  if(in==EOF){
    if(ferror(stdin)){
      gpr[0]=-2;
      errn=EINP;
    }
    else{
      gpr[0]=Chlore_EOF;
    }
    stack[*reg_sp_val]=gpr[0];
  }
  else{
    stack[*reg_sp_val]=in;
  }
  *reg_sp_val+=1;
}

void readchar_file(dl_params){
  char c;
  if(stack[*reg_sp_val-1]!=FD_STDIN){
    gpr[0]=read(stack[*reg_sp_val-1],&c,1);
    if(gpr[0]==-1){
      gpr[0]=Chlore_EOF;
      errn=EREAD;
    }
    stack[*reg_sp_val-1]=c;
  }
  else{
    scanin_char(dl_pass);
  }
}

void writechar_file(dl_params){
  char c=stack[*reg_sp_val-1];
  gpr[0]=write(stack[*reg_sp_val-2],&c,1);
  if(gpr[0]==-1){
    errn=EWRITE;
  }
  *reg_sp_val-=2;
}

void write_file(dl_params){
  unsigned char *write_buf;
  if((write_buf=malloc(stack[*reg_sp_val-1]))==NULL){
    gpr[0]=-1;
    errn=EINT;
    *reg_sp_val-=3;
    return;
  }
  nightVM_uc *read_from;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    read_from=(nightVM_uc *)*heap;
  }
  else{
    read_from=(nightVM_uc *)*code;
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    write_buf[i]=read_from[rem_tag(stack[*reg_sp_val-2])+i];
  }
  gpr[0]=write(stack[*reg_sp_val-3],write_buf,stack[*reg_sp_val-1]);
  if(gpr[0]==-1){
    errn=EREAD;
  }
  free(write_buf);
  *reg_sp_val-=3;
}

void writestr_file(dl_params){
  char c;
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
      c=((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-2])+i];
    }
    else{
      c=((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-2])+i];
    }
    if(c==0){
      break;
    }
    nightVM_i wr=write(stack[*reg_sp_val-3],&c,1);
    if(wr==-1){
      gpr[0]=-1;
      errn=EWRITE;
      break;
    }
    else if(wr==0){
      break;
    }
    gpr[0]++;
  }
  *reg_sp_val-=3;
}

void wwritestr_file(dl_params){ /* nonstandard! */
  writestr_file(dl_pass);
}

void read_file(dl_params){
  stack[*reg_sp_val-2]=rem_tag(stack[*reg_sp_val-2]);
  if(stack[*reg_sp_val-1]!=FD_STDIN){
    unsigned char *read_buf;
    if((read_buf=malloc(stack[*reg_sp_val-1]))==NULL){
      gpr[0]=-1;
      errn=EINT;
      *reg_sp_val-=3;
      return;
    }
    gpr[0]=read(stack[*reg_sp_val-3],read_buf,stack[*reg_sp_val-1]);
    if(gpr[0]==-1){
      errn=EREAD;
    }
    for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
      ((nightVM_uc *)*heap)[stack[*reg_sp_val-2]+i]=read_buf[i];
    }
    free(read_buf);
    *reg_sp_val-=3;
  }
  else{
    gpr[0]=0;
    int in;
    if(stack[*reg_sp_val-1]>0){
      ((nightVM_uc *)*heap)[stack[*reg_sp_val-2]]=0;
    }
    else{
      return;
    }
    for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
      if((in=fgetc(stdin))!=EOF){
        ((nightVM_uc *)*heap)[stack[*reg_sp_val-2]+i]=in;
        gpr[0]++;
      }
      else{
        break;
      }
    }
    if(in==EOF && ferror(stdin)){
      gpr[0]=-1;
      errn=EREAD;
    }
    *reg_sp_val-=3;
  }
}

void readstr_file(dl_params){
  stack[*reg_sp_val-2]=rem_tag(stack[*reg_sp_val-2]);
  if(stack[*reg_sp_val-1]!=FD_STDIN){
    unsigned char *read_buf;
    if((read_buf=malloc(stack[*reg_sp_val-1]))==NULL){
      gpr[0]=-1;
      errn=EINT;
      *reg_sp_val-=3;
      return;
    }
    gpr[0]=read(stack[*reg_sp_val-3],read_buf,stack[*reg_sp_val-1]);
    if(gpr[0]==-1){
      errn=EREAD;
    }
    for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
      ((nightVM_c *)*heap)[stack[*reg_sp_val-2]+i]=read_buf[i];
      ((nightVM_c *)*heap)[stack[*reg_sp_val-2]+i+1]=0;
    }
    free(read_buf);
    *reg_sp_val-=3;
  }
  else{
    gpr[0]=0;
    int in;
    if(stack[*reg_sp_val-1]>0){
      ((nightVM_c *)*heap)[stack[*reg_sp_val-2]]=0;
    }
    else{
      return;
    }
    for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
      if((in=fgetc(stdin))!=EOF){
        ((nightVM_c *)*heap)[stack[*reg_sp_val-2]+i]=in;
        ((nightVM_c *)*heap)[stack[*reg_sp_val-2]+i+1]=0;
        gpr[0]++;
      }
      else{
        break;
      }
    }
    if(in==EOF && ferror(stdin)){
      gpr[0]=-1;
      errn=EREAD;
    }
    *reg_sp_val-=3;
  }
}

void wreadstr_file(dl_params){ /* nonstandard! */
  readstr_file(dl_pass);
}

void get_argc(dl_params){
  stack[*reg_sp_val]=argc;
  gpr[0]=argc;
  *reg_sp_val+=1;
}

void get_argv(dl_params){
  cseqcpystr(&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-3])],argv[stack[*reg_sp_val-2]],stack[*reg_sp_val-1]);
  gpr[0]=0;
  *reg_sp_val-=3;
}

void print_n_str_fd(dl_params, int fd){
  gpr[0]=0;
  nightVM_c *read_from;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    read_from=(nightVM_c *)*heap;
  }
  else{
    read_from=(nightVM_c *)*code;
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-2] && read_from[rem_tag(stack[*reg_sp_val-1])]!=0;i++){
    char c=read_from[rem_tag(stack[*reg_sp_val-1])+i];
    if(write(fd,&c,1)==-1){
      gpr[0]=Chlore_EOF;
      errn=EPERR;
      return;
    }
    gpr[0]++;
  }
  *reg_sp_val-=2;
}

void print_n_str_err(dl_params){
  print_n_str_fd(dl_pass,FD_STDERR);
}

void print_n_str(dl_params){
  print_n_str_fd(dl_pass,FD_STDOUT);
}

void print_str_fd(dl_params, int fd){
  char *s;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    if((s=cseq2str(&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])]))==NULL){
      errn=EALLOC;
      gpr[0]=Chlore_EOF;
      *reg_sp_val-=1;
      return;
    }
  }
  else{
    if((s=cseq2str(&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])]))==NULL){
      errn=EALLOC;
      gpr[0]=Chlore_EOF;
      *reg_sp_val-=1;
      return;
    }
  }
  gpr[0]=write(fd,s,strlen(s));
  if(gpr[0]==-1){
    gpr[0]=Chlore_EOF;
    errn=EPERR;
  }
  else{
    gpr[0]=0;
  }
  free(s);
  *reg_sp_val-=1;
}

void print_str_err(dl_params){
  print_str_fd(dl_pass,FD_STDERR);
}

void print_str(dl_params){
  print_str_fd(dl_pass,FD_STDOUT);
}

void print_fmt_fd(dl_params, int fd){
  char *fmt_string=NULL;
  *reg_sp_val-=1;
  if(ptr_typ(stack[*reg_sp_val])==TYP_HEAP){
    if((fmt_string=cseq2str(&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val])]))==NULL){
      errn=EALLOC;
      gpr[0]=-1;
      return;
    }
  }
  else{
    if((fmt_string=cseq2str(&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val])]))==NULL){
      errn=EALLOC;
      gpr[0]=-1;
      return;
    }
  }
  unsigned int state=0;
  /*
   * state 0 -> nothing special; just continue reading
   * state 1 -> encountered a conversion specifier indicator (%)
   * state 3 -> type-specifier has been read
   * state 4 -> two-character type-specifier[0]  ----\ read in
   * state 5 -> two-character type-specifier[1]  ----/ two parts
   */
  unsigned int type_specifier=0;
  /*
   * type_specifier 0 -> char
   * type_specifier 1 -> short
   * type_specifier 2 -> int
   * type_specifier 3 -> long
   * type_specifier 4 -> character equivalent
   * type_specifier 5 -> string
   * type_specifier 6 -> unsigned char
   * type_specifier 7 -> unsigned short
   * type_specifier 8 -> unsigned int
   * type_specifier 9 -> hex
   * type_specifier 10 -> octal
   * type_specifier 11 -> unsigned
   * type_specifier 12 -> hex char
   * type_specifier 13 -> hex short
   * type_specifier 14 -> hex int
   * type_specifier 15 -> octal char
   * type_specifier 16 -> octal short
   * type_specifier 17 -> octal int
   * type_specifier 18 -> pointer
   */

  #define spec_c 0
  #define spec_s 1
  #define spec_i 2
  #define spec_l 3
  #define spec_h 4
  #define spec_t 5
  #define spec_uc 6
  #define spec_us 7
  #define spec_ui 8
  #define spec_x 9
  #define spec_o 10
  #define spec_u 11
  #define spec_xc 12
  #define spec_xs 13
  #define spec_xi 14
  #define spec_oc 15
  #define spec_os 16
  #define spec_oi 17
  #define spec_p 18

  for(size_t i=0;fmt_string[i]!='\0';i++){
    if(state==0){
      if(fmt_string[i]=='%'){
        state=1;
      }
      else{
        if(write(fd,&fmt_string[i],1)==-1){
          free(fmt_string);
          gpr[0]=-1;
          errn=EPERR;
          return;
        }
      }
    }
    else if(state==1||state==4){
      if(fmt_string[i]=='%'){
        if(write(fd,"%",1)==-1){
          free(fmt_string);
          gpr[0]=-1;
          errn=EPERR;
          return;
        }
        state=0;
      }
      else{
        switch(fmt_string[i]){
        case 'p':
          if(state==1){
            type_specifier=spec_p;
            state=3;
          }
          else{
            free(fmt_string);
            gpr[0]=-1;
            errn=EPERR;
            return;
          }
          break;
        case 'c':
          if(state==1){
            type_specifier=spec_c;
          }
          else if(type_specifier==spec_x){
            type_specifier=spec_xc;
          }
          else if(type_specifier==spec_o){
            type_specifier=spec_oc;
          }
          else if(type_specifier==spec_u){
            type_specifier=spec_uc;
          }
          state=3;
          break;
        case 's':
          if(state==1){
            type_specifier=spec_s;
          }
          else if(type_specifier==spec_x){
            type_specifier=spec_xs;
          }
          else if(type_specifier==spec_o){
            type_specifier=spec_os;
          }
          else if(type_specifier==spec_u){
            type_specifier=spec_us;
          }
          state=3;
          break;
        case 'i':
          if(state==1){
            type_specifier=spec_i;
          }
          else if(type_specifier==spec_x){
            type_specifier=spec_xi;
          }
          else if(type_specifier==spec_o){
            type_specifier=spec_oi;
          }
          else if(type_specifier==spec_u){
            type_specifier=spec_ui;
          }
          state=3;
          break;
        case 'l':
          if(state==1){
            type_specifier=spec_l;
            state=3;
          }
          else{
            free(fmt_string);
            gpr[0]=-1;
            errn=EPERR;
            return;
          }
          break;
        case 'u':
          if(state==1){
            type_specifier=spec_u;
            state=4;
          }
          else{
            free(fmt_string);
            gpr[0]=-1;
            errn=EPERR;
            return;
          }
          break;
        case 'x':
          if(state==1){
            type_specifier=spec_x;
            state=4;
          }
          else{
            free(fmt_string);
            gpr[0]=-1;
            errn=EPERR;
            return;
          }
          break;
        case 'h':
          if(state==1){
            type_specifier=spec_h;
            state=3;
          }
          else{
            free(fmt_string);
            gpr[0]=-1;
            errn=EPERR;
            return;
          }
          break;
        case 't':
          if(state==1){
            type_specifier=spec_t;
            state=3;
          }
          else{
            free(fmt_string);
            gpr[0]=-1;
            errn=EPERR;
            return;
          }
          break;
        case 'o':
          if(state==1){
            type_specifier=spec_os;
            state=4;
          }
          else{
            free(fmt_string);
            gpr[0]=-1;
            errn=EPERR;
            return;
          }
          break;
        default:
          free(fmt_string);
          gpr[0]=-1;
          errn=EPERR;
          return;
        }
      }
    }
    if(state==3){
      char efms[1];
      int n;
      char c;
      char *fms;
      switch(type_specifier){
      case spec_c:
        if((n=snprintf(efms,1,"%" PRINVMC,(nightVM_c)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMC,(nightVM_c)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_s:
        if((n=snprintf(efms,1,"%" PRINVMS,(nightVM_s)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMS,(nightVM_s)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        *reg_sp_val-=1;
        break;
      case spec_i:
        if((n=snprintf(efms,1,"%" PRINVMI,(nightVM_i)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMI,(nightVM_i)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_l:
        if((n=snprintf(efms,1,"%" PRINVML,(nightVM_l)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVML,(nightVM_l)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_h:
        c=stack[*reg_sp_val-1];
        if(write(fd,&c,1)==-1){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        *reg_sp_val-=1;
        break;
      case spec_t:
        print_str_fd(dl_pass,fd);
        if(gpr[0]==EOF){
          free(fmt_string);
          gpr[0]=-1;
          errn=EPERR;
          return;
        }
        break;
      case spec_uc:
        if((n=snprintf(efms,1,"%" PRINVMUC,(nightVM_uc)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMUC,(nightVM_uc)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_us:
        if((n=snprintf(efms,1,"%" PRINVMUS,(nightVM_us)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMUS,(nightVM_us)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_ui:
        if((n=snprintf(efms,1,"%" PRINVMUI,(nightVM_ui)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMUI,(nightVM_ui)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_xc:
        if((n=snprintf(efms,1,"%" PRINVMxUC,(nightVM_uc)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMxUC,(nightVM_uc)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_xs:
        if((n=snprintf(efms,1,"%" PRINVMxUS,(nightVM_us)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMxUS,(nightVM_us)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
      case spec_xi:
        if((n=snprintf(efms,1,"%" PRINVMxUI,(nightVM_ui)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMxUI,(nightVM_ui)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
      case spec_oc:
        if((n=snprintf(efms,1,"%" PRINVMoUC,(nightVM_uc)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMoUC,(nightVM_uc)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
      case spec_os:
        if((n=snprintf(efms,1,"%" PRINVMoUS,(nightVM_us)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMoUS,(nightVM_us)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_oi:
        if((n=snprintf(efms,1,"%" PRINVMoUI,(nightVM_ui)stack[*reg_sp_val-1]))<0){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((fms=malloc(n+1))==NULL){
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if((n=snprintf(fms,n+1,"%" PRINVMoUI,(nightVM_ui)stack[*reg_sp_val-1]))<0){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        if(write(fd,fms,n)==-1){
          free(fms);
          gpr[0]=-1;
          errn=EPERR;
          *reg_sp_val-=1;
          return;
        }
        free(fms);
        *reg_sp_val-=1;
        break;
      case spec_p:
        if(stack[*reg_sp_val-1]==NULL_PTR){
          if(write(fd,"(null)",6)==-1){
            gpr[0]=Chlore_EOF;
            errn=EPERR;
            *reg_sp_val-=1;
            return;
          }
        }
        else{
          if((n=snprintf(efms,1,"%" PRINVMxUNS,(nightVM_uns)stack[*reg_sp_val-1]))<0){
            gpr[0]=Chlore_EOF;
            errn=EPERR;
            *reg_sp_val-=1;
            return;
          }
          char *fms;
          if((fms=malloc(n+1))==NULL){
            gpr[0]=Chlore_EOF;
            errn=EPERR;
            *reg_sp_val-=1;
            return;
          }
          if((n=snprintf(fms,n+1,"%" PRINVMxUNS,(nightVM_uns)stack[*reg_sp_val-1]))<0){
            free(fms);
            gpr[0]=Chlore_EOF;
            errn=EPERR;
            *reg_sp_val-=1;
            return;
          }
          if(write(FD_STDOUT,fms,n)==-1){
            free(fms);
            gpr[0]=Chlore_EOF;
            errn=EPERR;
            *reg_sp_val-=1;
            return;
          }
          free(fms);
        }
        gpr[0]=0;
        *reg_sp_val-=1;
        break;
      default:
        free(fmt_string);
        gpr[0]=-1;
        errn=EPERR;
        return;
      }
      type_specifier=0;
      state=0;
    }
  }
  free(fmt_string);
  gpr[0]=0;
}

void print_fmt(dl_params){
  print_fmt_fd(dl_pass,FD_STDOUT);
}

void print_fmt_err(dl_params){
  print_fmt_fd(dl_pass,FD_STDERR);
}

void remove_file(dl_params){
  nightVM_c *s;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    s=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  }
  else{
    s=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])];
  }
  gpr[0]=remove(s);
  stack[*reg_sp_val-1]=gpr[0];
}

void rename_file(dl_params){
  nightVM_c *new;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    new=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  }
  else{
    new=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])];
  }
  nightVM_c *old;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    old=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-2])];
  }
  else{
    old=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-2])];
  }
  gpr[0]=rename(old,new);
  stack[*reg_sp_val-2]=gpr[0];
  *reg_sp_val-=1;
}

void open_file(dl_params){
  char *s;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    if((s=cseq2str(&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-2])]))==NULL){
      errn=EALLOC;
      gpr[0]=-1;
      *reg_sp_val-=1;
      return;
    }
  }
  else{
    if((s=cseq2str(&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-2])]))==NULL){
      errn=EALLOC;
      gpr[0]=-1;
      *reg_sp_val-=1;
      return;
    }
  }
  gpr[0]=open(s,stack[*reg_sp_val-1]);
  free(s);
  stack[*reg_sp_val-2]=gpr[0];
  if(gpr[0]==-1){
    errn=EOPEN;
  }
  *reg_sp_val-=1;
}

void scanin_uc(dl_params){
  nightVM_uc get_uc;
  gpr[0]=scanf("%" SCNNVMUC,&get_uc);
  stack[*reg_sp_val]=get_uc;
  if(gpr[0]==EOF){
    gpr[0]=Chlore_EOF;
    errn=EINP;
  }
  *reg_sp_val+=1;
}

void scanin_us(dl_params){
  nightVM_us get_us;
  gpr[0]=scanf("%" SCNNVMUS,&get_us);
  stack[*reg_sp_val]=get_us;
  if(gpr[0]==EOF){
    gpr[0]=Chlore_EOF;
    errn=EINP;
  }
  *reg_sp_val+=1;
}

void scanin_ui(dl_params){
  nightVM_ui get_ui;
  gpr[0]=scanf("%" SCNNVMUI,&get_ui);
  stack[*reg_sp_val]=get_ui;
  if(gpr[0]==EOF){
    gpr[0]=Chlore_EOF;
    errn=EINP;
  }
  *reg_sp_val+=1;
}

void scanin_c(dl_params){
  nightVM_c get_c;
  gpr[0]=scanf("%" SCNNVMC,&get_c);
  stack[*reg_sp_val]=get_c;
  if(gpr[0]==EOF){
    gpr[0]=Chlore_EOF;
    errn=EINP;
  }
  *reg_sp_val+=1;
}

void scanin_s(dl_params){
  nightVM_s get_s;
  gpr[0]=scanf("%" SCNNVMS,&get_s);
  stack[*reg_sp_val]=get_s;
  if(gpr[0]==EOF){
    gpr[0]=Chlore_EOF;
    errn=EINP;
  }
  *reg_sp_val+=1;
}

void scanin_i(dl_params){
  nightVM_i get_i;
  gpr[0]=scanf("%" SCNNVMI,&get_i);
  stack[*reg_sp_val]=get_i;
  if(gpr[0]==EOF){
    gpr[0]=Chlore_EOF;
    errn=EINP;
  }
  *reg_sp_val+=1;
}

void scanin_l(dl_params){
  gpr[0]=scanf("%" SCNNVML,&stack[*reg_sp_val]);
  if(gpr[0]==EOF){
    gpr[0]=Chlore_EOF;
    errn=EINP;
  }
  *reg_sp_val+=1;
}

/*
  math.chloreh
*/

void sqroot(dl_params){
  gpr[0]=sqrt(stack[*reg_sp_val-1]);
  stack[*reg_sp_val-1]=gpr[0];
}

void power(dl_params){
  gpr[0]=pow(stack[*reg_sp_val-2],stack[*reg_sp_val-1]);
  stack[*reg_sp_val-2]=gpr[0];
  *reg_sp_val-=1;
}

/*
  string.chloreh
*/

void cpy_mem(dl_params){
  nightVM_uc *s1;
  nightVM_uc *s2;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    s1=&((nightVM_uc *)*heap)[rem_tag(stack[*reg_sp_val-2])];
  }
  else{
    s1=&((nightVM_uc *)*code)[rem_tag(stack[*reg_sp_val-2])];
  }
  s2=&((nightVM_uc *)*heap)[rem_tag(stack[*reg_sp_val-3])];
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    s2[i]=s1[i];
  }
  gpr[0]=0;
  *reg_sp_val-=3;
}

void cmp_mem(dl_params){
  nightVM_uc *s1;
  nightVM_uc *s2;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    s1=&((nightVM_uc *)*heap)[rem_tag(stack[*reg_sp_val-2])];
  }
  else{
    s1=&((nightVM_uc *)*code)[rem_tag(stack[*reg_sp_val-2])];
  }
  s2=&((nightVM_uc *)*heap)[rem_tag(stack[*reg_sp_val-3])];
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    s2[i]=s1[i];
  }
  gpr[0]=0;
  *reg_sp_val-=3;
}

void set_mem(dl_params){
  nightVM_uc *cs;
  if(ptr_typ(stack[*reg_sp_val-3])==TYP_HEAP){
    cs=&((nightVM_uc *)*heap)[rem_tag(stack[*reg_sp_val-3])];
  }
  else{
    cs=&((nightVM_uc *)*code)[rem_tag(stack[*reg_sp_val-3])];
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    cs[i]=stack[*reg_sp_val-2];
  }
  *reg_sp_val-=3;
}

void cpy_str(dl_params){
  nightVM_c *s1;
  nightVM_c *s2;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    s1=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-2])];
  }
  else{
    s1=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-2])];
  }
  s2=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-3])];
  cseqcpycseq(s2,s1,stack[*reg_sp_val-1]);
  gpr[0]=0;
  *reg_sp_val-=3;
}

void wcpy_str(dl_params){ /* nonstandard! */
  cpy_str(dl_pass);
}

void cmp_n_str(dl_params){
  nightVM_c *s1;
  nightVM_c *s2;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    s1=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-2])];
  }
  else{
    s1=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-2])];
  }
  if(ptr_typ(stack[*reg_sp_val-3])==TYP_HEAP){
    s2=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-3])];
  }
  else{
    s2=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-3])];
  }
  size_t s1_len=cseqlen(s1);
  size_t s2_len=cseqlen(s2);
  if(s1_len>(size_t)stack[*reg_sp_val-1]){
    s1[stack[*reg_sp_val-1]]=0;
  }
  if(s2_len>(size_t)stack[*reg_sp_val-1]){
    s2[stack[*reg_sp_val-1]]=0;
  }
  gpr[0]=cseqcmpcseq(s1,s2);
  stack[*reg_sp_val-3]=gpr[0];
  *reg_sp_val-=2;
}

void wcmp_n_str(dl_params){ /* nonstandard! */
  cmp_n_str(dl_pass);
}

void chr_str(dl_params){
  gpr[0]=-1;
  nightVM_c *s;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    s=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  }
  else{
    s=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])];
  }
  for(nightVM_l i=0;s[i]!=0;i++){
    if(s[i]==stack[*reg_sp_val-2]){
      gpr[0]=i;
      break;
    }
  }
  stack[*reg_sp_val-2]=gpr[0];
  *reg_sp_val-=1;
}

void wchr_str(dl_params){ /* nonstandard! */
  chr_str(dl_pass);
}

void chr_n_str(dl_params){
  gpr[0]=-1;
  nightVM_c *s;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    s=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-2])];
  }
  else{
    s=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-2])];
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-1] && s[i]!=0;i++){
    if(s[i]==stack[*reg_sp_val-3]){
      gpr[0]=i;
      break;
    }
  }
  stack[*reg_sp_val-3]=gpr[0];
  *reg_sp_val-=2;
}

void wchr_n_str(dl_params){ /* nonstandard! */
  chr_n_str(dl_pass);
}

void cmp_str(dl_params){
  nightVM_c *s1;
  nightVM_c *s2;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    s1=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  }
  else{
    s1=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])];
  }
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    s2=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-2])];
  }
  else{
    s2=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-2])];
  }
  gpr[0]=cseqcmpcseq(s1,s2);
  stack[*reg_sp_val-2]=gpr[0];
  *reg_sp_val-=1;
}

void wcmp_str(dl_params){ /* nonstandard! */
  cmp_str(dl_pass);
}

void byte_count(dl_params){
  nightVM_c *s;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    s=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  }
  else{
    s=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])];
  }
  gpr[0]=cseqlen(s);
  stack[*reg_sp_val-1]=gpr[0];
}

void str_len(dl_params){
  nightVM_c *s;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    s=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  }
  else{
    s=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])];
  }
  gpr[0]=cseqlen(s);
  stack[*reg_sp_val-1]=gpr[0];
}

void next_char(dl_params){
  nightVM_c *s;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    s=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  }
  else{
    s=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])];
  }
  gpr[0]=stack[*reg_sp_val-1]+sizeof(nightVM_c);
  stack[*reg_sp_val-1]=s[rem_tag(gpr[0])];
}

void is_space(dl_params){
  gpr[0]=0;
  if(stack[*reg_sp_val-1]=='\n' || stack[*reg_sp_val-1]==' ' || stack[*reg_sp_val-1]=='\t' || stack[*reg_sp_val-1]=='\f' || stack[*reg_sp_val-1]=='\r' || stack[*reg_sp_val-1]=='\v'){
    gpr[0]=1;
  }
  stack[*reg_sp_val-1]=gpr[0];
}

/*
  exit.chloreh
*/

void ph_clean(dl_params){
  if(alist_head==NULL){
    gpr[0]=1;
    errn=EINT;
    return;
  }
  while(alist_head!=NULL){
    lalloc *alist_trav=alist_head;
    alist_head=alist_head->next_seg;
    free(alist_trav);
  }
  gpr[0]=0;
}

/*
  nonstandard!
  locale.chloreh
*/

#define locale_ALL 0
#define locale_COLLATE 1
#define locale_CTYPE 2
#define locale_MONETARY 3
#define locale_NUMERIC 4
#define locale_TIME 5

char *native_locale="Chlore";

#define offset_locale_lctable_decimal_point 0
#define offset_locale_lctable_thousands_sep SIZEOF_L
#define offset_locale_lctable_grouping offset_locale_lctable_thousands_sep+SIZEOF_L
#define offset_locale_lctable_mon_decimal_point offset_locale_lctable_grouping+SIZEOF_L
#define offset_locale_lctable_mon_thousands_sep offset_locale_lctable_mon_decimal_point+SIZEOF_L
#define offset_locale_lctable_mon_grouping offset_locale_lctable_mon_thousands_sep+SIZEOF_L
#define offset_locale_lctable_positive_sign offset_locale_lctable_mon_grouping+SIZEOF_L
#define offset_locale_lctable_negative_sign offset_locale_lctable_positive_sign+SIZEOF_L
#define offset_locale_lctable_currency_symbol offset_locale_lctable_negative_sign+SIZEOF_L
#define offset_locale_lctable_frac_digits offset_locale_lctable_currency_symbol+1
#define offset_locale_lctable_p_cs_precedes offset_locale_lctable_frac_digits+1
#define offset_locale_lctable_n_cs_precedes offset_locale_lctable_p_cs_precedes+1
#define offset_locale_lctable_p_sep_by_space offset_locale_lctable_n_cs_precedes+1
#define offset_locale_lctable_n_sep_by_space offset_locale_lctable_p_sep_by_space+1
#define offset_locale_lctable_p_sign_posn offset_locale_lctable_n_sep_by_space+1
#define offset_locale_lctable_n_sign_posn offset_locale_lctable_p_sign_posn+1
#define offset_locale_lctable_int_curr_symbol offset_locale_lctable_n_sign_posn+SIZEOF_L
#define offset_locale_lctable_int_frac_digits offset_locale_lctable_int_curr_symbol+1
#define offset_locale_lctable_int_p_cs_precedes offset_locale_lctable_int_frac_digits+1
#define offset_locale_lctable_int_n_cs_precedes offset_locale_lctable_int_p_cs_precedes+1
#define offset_locale_lctable_int_p_sep_by_space offset_locale_lctable_int_n_cs_precedes+1
#define offset_locale_lctable_int_n_sep_by_space offset_locale_lctable_int_p_sep_by_space+1
#define offset_locale_lctable_int_p_sign_posn offset_locale_lctable_int_n_sep_by_space+1
#define offset_locale_lctable_int_n_sign_posn offset_locale_lctable_int_p_sign_posn+1
#define size_locale_lctable offset_locale_lctable_int_n_sign_posn+1

void set_locale(dl_params){
  if(!(stack[*reg_sp_val-2]==locale_ALL || stack[*reg_sp_val-2]==locale_COLLATE || stack[*reg_sp_val-2]==locale_CTYPE || stack[*reg_sp_val-2]==locale_MONETARY || stack[*reg_sp_val-2]==locale_NUMERIC || stack[*reg_sp_val-2]==locale_TIME)){
    gpr[0]=-1;
    errn=ELOCALE;
    *reg_sp_val-=2;
    return;
  }
  nightVM_c *s;
  if(ptr_typ(stack[*reg_sp_val-1])==TYP_HEAP){
    s=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  }
  else{
    s=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-1])];
  }
  if(!(cseqcmpstr(s,"Chlore")==0 || cseqcmpstr(s,"C")==0 ||(cseqcmpstr(s,"")==0 && (strcmp(native_locale,"Chlore")==0 || strcmp(native_locale,"C")==0)))){
    gpr[0]=-1;
    errn=ELOCALE;
    *reg_sp_val-=2;
    return;
  }
  gpr[0]=0;
  *reg_sp_val-=2;
}

void filllctable(dl_params){
  nightVM_c *ch=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-1])];
  nightVM_l ptr;
  nightVM_c cmax=CHAR_MAX;
  memcpy(&ptr,&ch[offset_locale_lctable_decimal_point],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],".",strlen("."));
  memcpy(&ptr,&ch[offset_locale_lctable_thousands_sep],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ptr,&ch[offset_locale_lctable_grouping],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ptr,&ch[offset_locale_lctable_mon_decimal_point],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ptr,&ch[offset_locale_lctable_mon_thousands_sep],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ptr,&ch[offset_locale_lctable_mon_grouping],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ptr,&ch[offset_locale_lctable_positive_sign],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ptr,&ch[offset_locale_lctable_negative_sign],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ptr,&ch[offset_locale_lctable_currency_symbol],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ch[offset_locale_lctable_frac_digits],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_p_cs_precedes],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_n_cs_precedes],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_p_sep_by_space],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_n_sep_by_space],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_p_sign_posn],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_n_sign_posn],&cmax,sizeof(nightVM_c));
  memcpy(&ptr,&ch[offset_locale_lctable_int_curr_symbol],sizeof(nightVM_l));
  cseqcpystr(&ch[rem_tag(ptr)],"",strlen(""));
  memcpy(&ch[offset_locale_lctable_int_frac_digits],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_int_p_cs_precedes],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_int_n_cs_precedes],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_int_p_sep_by_space],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_int_n_sep_by_space],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_int_p_sign_posn],&cmax,sizeof(nightVM_c));
  memcpy(&ch[offset_locale_lctable_int_n_sign_posn],&cmax,sizeof(nightVM_c));
}

/*
  nonstandard!
  socket.chloreh
*/

void create_socket(dl_params){
  gpr[0]=socket(stack[*reg_sp_val-3],stack[*reg_sp_val-2],stack[*reg_sp_val-1]);
  stack[*reg_sp_val-3]=gpr[0];
  if(gpr[0]==-1){
    errn=ECRTSOCK;
  }
  *reg_sp_val-=2;
}

void sock_connect(dl_params){
  nightVM_c *css;
  if(ptr_typ(stack[*reg_sp_val-7])==TYP_HEAP){
    css=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-7])];
  }
  else{
    css=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-7])];
  }
  char *ss=cseq2str(css);
  if(ss==NULL){
    gpr[0]=1;
    errn=EINT;
    *reg_sp_val-=7;
    return;
  }
  nightVM_c *csn;
  if(ptr_typ(stack[*reg_sp_val-6])==TYP_HEAP){
    csn=&((nightVM_c *)*heap)[rem_tag(stack[*reg_sp_val-6])];
  }
  else{
    csn=&((nightVM_c *)*code)[rem_tag(stack[*reg_sp_val-6])];
  }
  char *sn=cseq2str(csn);
  if(sn==NULL){
    free(ss);
    gpr[0]=1;
    errn=EINT;
    *reg_sp_val-=7;
    return;
  }
  struct addrinfo hints;
  struct addrinfo *result;
  memset(&hints,0,sizeof(hints));
  hints.ai_family=stack[*reg_sp_val-5];
  hints.ai_socktype=stack[*reg_sp_val-4];
  hints.ai_flags=stack[*reg_sp_val-3];
  hints.ai_protocol=stack[*reg_sp_val-2];
  if(getaddrinfo(sn,ss,&hints,&result)!=0){
    free(ss);
    free(sn);
    gpr[0]=1;
    errn=EINT;
    *reg_sp_val-=7;
    return;
  }
  free(ss);
  free(sn);
  struct addrinfo *rp;
  for(rp=result;rp!=NULL;rp=rp->ai_next){
    if(connect(stack[*reg_sp_val-1],rp->ai_addr,rp->ai_addrlen)!=1){
      break;
    }
  }
  freeaddrinfo(result);
  if(rp==NULL){
    gpr[0]=1;
    errn=ESOCKCON;
    *reg_sp_val-=7;
    return;
  }
  gpr[0]=0;
  *reg_sp_val-=7;
}

void sock_recv(dl_params){
  stack[*reg_sp_val-2]=rem_tag(stack[*reg_sp_val-2]);
  unsigned char *read_buf;
  if((read_buf=malloc(stack[*reg_sp_val-1]))==NULL){
    gpr[0]=-1;
    errn=EINT;
    *reg_sp_val-=4;
    return;
  }
  gpr[0]=recv(stack[*reg_sp_val-4],read_buf,stack[*reg_sp_val-1],stack[*reg_sp_val-3]);
  if(gpr[0]==-1){
    errn=ERECV;
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    ((nightVM_uc *)*heap)[stack[*reg_sp_val-2]+i]=read_buf[i];
  }
  free(read_buf);
  *reg_sp_val-=4;
}

void sock_send(dl_params){
  unsigned char *write_buf;
  if((write_buf=malloc(stack[*reg_sp_val-1]))==NULL){
    gpr[0]=-1;
    errn=EINT;
    *reg_sp_val-=4;
    return;
  }
  nightVM_uc *read_from;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    read_from=(nightVM_uc *)*heap;
  }
  else{
    read_from=(nightVM_uc *)*code;
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    write_buf[i]=read_from[rem_tag(stack[*reg_sp_val-2])+i];
  }
  gpr[0]=send(stack[*reg_sp_val-4],write_buf,stack[*reg_sp_val-1],stack[*reg_sp_val-3]);
  if(gpr[0]==-1){
    errn=ESEND;
  }
  free(write_buf);
  *reg_sp_val-=4;
}

void sock_recvstr(dl_params){
  stack[*reg_sp_val-2]=rem_tag(stack[*reg_sp_val-2]);
  char *read_buf;
  if((read_buf=malloc(stack[*reg_sp_val-1]))==NULL){
    gpr[0]=-1;
    errn=EINT;
    *reg_sp_val-=4;
    return;
  }
  gpr[0]=recv(stack[*reg_sp_val-4],read_buf,stack[*reg_sp_val-1],stack[*reg_sp_val-3]);
  if(gpr[0]==-1){
    errn=ERECV;
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    ((nightVM_c *)*heap)[stack[*reg_sp_val-2]+i]=read_buf[i];
    ((nightVM_c *)*heap)[stack[*reg_sp_val-2]+i+1]=0;
  }
  free(read_buf);
  *reg_sp_val-=4;
}

void sock_sendstr(dl_params){
  char *write_buf;
  if((write_buf=malloc(stack[*reg_sp_val-1]))==NULL){
    gpr[0]=-1;
    errn=EINT;
    *reg_sp_val-=4;
    return;
  }
  nightVM_c *read_from;
  if(ptr_typ(stack[*reg_sp_val-2])==TYP_HEAP){
    read_from=(nightVM_c *)*heap;
  }
  else{
    read_from=(nightVM_c *)*code;
  }
  for(nightVM_l i=0;i<stack[*reg_sp_val-1];i++){
    write_buf[i]=read_from[rem_tag(stack[*reg_sp_val-2])+i];
  }
  gpr[0]=send(stack[*reg_sp_val-4],write_buf,stack[*reg_sp_val-1],stack[*reg_sp_val-3]);
  if(gpr[0]==-1){
    errn=ESEND;
  }
  free(write_buf);
  *reg_sp_val-=4;
}

/*
  nonstandard!
  watch.chloreh
*/

#define WATCH_N 100
#define READFD 0
#define WRITEFD 1
#define EXCEPTFD 2

static fd_set fdrwe_set[WATCH_N][3];
static int watches[WATCH_N]={0};

void fdset_zero(dl_params){
  FD_ZERO(&fdrwe_set[stack[*reg_sp_val-2]][stack[*reg_sp_val-1]]);
  gpr[0]=0;
  *reg_sp_val-=2;
}

void fdset_set(dl_params){
  FD_SET(stack[*reg_sp_val-3],&fdrwe_set[stack[*reg_sp_val-2]][stack[*reg_sp_val-1]]);
  gpr[0]=0;
  *reg_sp_val-=3;
}

void fdset_clear(dl_params){
  FD_CLR(stack[*reg_sp_val-3],&fdrwe_set[stack[*reg_sp_val-2]][stack[*reg_sp_val-1]]);
  gpr[0]=0;
  *reg_sp_val-=3;
}

void fdset_isset(dl_params){
  gpr[0]=FD_ISSET(stack[*reg_sp_val-3],&fdrwe_set[stack[*reg_sp_val-2]][stack[*reg_sp_val-1]]);
  stack[*reg_sp_val-3]=gpr[0];
  *reg_sp_val-=2;
}

void watch_new(dl_params){
  for(size_t i=0;i<WATCH_N;i++){
    if(watches[i]==0){
      stack[*reg_sp_val]=i;
      gpr[0]=i;
      *reg_sp_val+=1;
      return;
    }
  }
  stack[*reg_sp_val]=-1;
  gpr[0]=-1;
  errn=ECRTWATCH;
  *reg_sp_val+=1;
}

void watch_close(dl_params){
  watches[stack[*reg_sp_val-1]]=0;
  gpr[0]=0;
  *reg_sp_val-=1;
}

void watch(dl_params){
  fd_set *rd, *wr, *ex;
  if(stack[*reg_sp_val-4]==-1){
    rd=NULL;
  }
  else{
    rd=&fdrwe_set[stack[*reg_sp_val-5]][READFD];
  }
  if(stack[*reg_sp_val-3]==-1){
    wr=NULL;
  }
  else{
    wr=&fdrwe_set[stack[*reg_sp_val-5]][WRITEFD];
  }
  if(stack[*reg_sp_val-2]==-1){
    ex=NULL;
  }
  else{
    ex=&fdrwe_set[stack[*reg_sp_val-5]][EXCEPTFD];
  }
  gpr[0]=select(stack[*reg_sp_val-1],rd,wr,ex,NULL);
  if(gpr[0]==-1){
    errn=EWATCH;
  }
  *reg_sp_val-=5;
}
