#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "nvm_sources.h"
#include TOC_NVM_OPCODES_H
#include "write_error.h"
#include "comp_properties.h"
#define TOC_ADD_KEYWORD
#include "psg_idens.h"
#include "code_gen.h"

extern compilation_attributes comp_attr;

#define OUTBUF_SIZE 1048576

static nightVM_uc *outbuf=NULL;
static size_t outbuf_filled_upto=0;
static FILE *outstream=NULL;

static inline unsigned int add_to_outstream(unsigned char *buf, size_t size){
  if(outstream==NULL){
    if((outstream=fopen(comp_attr.outfile,"wb"))==NULL){
      fprintf(stderr,"implementation error: failed to open file %s for writing\n",comp_attr.outfile);
      return 1;
    }
  }
  if(outbuf==NULL){
    if((outbuf=malloc(OUTBUF_SIZE*sizeof(nightVM_uc)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
  }
  if(OUTBUF_SIZE-outbuf_filled_upto>=size){
    memcpy(&outbuf[outbuf_filled_upto],buf,size*sizeof(nightVM_uc));
    outbuf_filled_upto+=size;
  }
  else{
    if(fwrite(outbuf,sizeof(nightVM_uc),outbuf_filled_upto,outstream)<outbuf_filled_upto){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      return 1;
    }
    memcpy(outbuf,buf,size*sizeof(nightVM_uc));
    outbuf_filled_upto=size;
  }
  return 0;
}

static inline unsigned int flush_to_outstream(void){
  if(outstream!=NULL && outbuf!=NULL){
    if(fwrite(outbuf,sizeof(nightVM_uc),outbuf_filled_upto,outstream)<outbuf_filled_upto){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      return 1;
    }
  }
  return 0;
}

static inline void clean_add_to_stream_res(void){
  fclose(outstream);
  free(outbuf);
}

unsigned int gen_code_push_statement(push_statement *phrase_push_statement, str_table *string_table, sym_table *symbol_table){
  nightVM_uc pad_nop=op_nop;
  for(nightVM_ui i=0;i<phrase_push_statement->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  if(phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushuc){
    nightVM_uc nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_uc=numeric_constant_to_uc(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_uc*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_uc=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_uc*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_uc=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_uc*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_uc=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_uc*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushus){
    nightVM_uc nVM_uc=op_pushus;
    nightVM_us nVM_us;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_us=numeric_constant_to_us(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_us*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_us,SIZEOF_US)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_us=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_us*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_us,SIZEOF_US)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_us=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_us*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_us,SIZEOF_US)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_us=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_us*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_us,SIZEOF_US)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushui){
    nightVM_uc nVM_uc=op_pushui;
    nightVM_ui nVM_ui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_ui=numeric_constant_to_ui(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_ui*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_ui,SIZEOF_UI)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_ui=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_ui*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_ui,SIZEOF_UI)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_ui=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_ui*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_ui,SIZEOF_UI)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_ui=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_ui*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_ui,SIZEOF_UI)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushc){
    nightVM_uc nVM_uc=op_pushc;
    nightVM_c nVM_c;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_c=numeric_constant_to_c(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_c*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_c,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_c=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_c*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_c,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_c=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_c*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_c,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_c=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_c*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_c,1)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushs){
    nightVM_uc nVM_uc=op_pushs;
    nightVM_s nVM_s;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_s=numeric_constant_to_s(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_s*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_s,SIZEOF_S)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_s=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_s*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_s,SIZEOF_S)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_s=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_s*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_s,SIZEOF_S)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_s=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_s*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_s,SIZEOF_S)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushi){
    nightVM_uc nVM_uc=op_pushi;
    nightVM_i nVM_i;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_i=numeric_constant_to_i(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_i*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_i,SIZEOF_I)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_i=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_i*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_i,SIZEOF_I)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_i=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_i*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_i,SIZEOF_I)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_i=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_i*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_i,SIZEOF_I)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushl){
    nightVM_uc nVM_uc=op_pushl;
    nightVM_l nVM_l;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_l=numeric_constant_to_l(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_l*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_l=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_l*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_l=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_l*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_l=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_l*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushp){
    nightVM_uc nVM_uc=op_pushl;
    nightVM_l nVM_l;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_l=numeric_constant_to_p(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_l*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_l=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL){
        nVM_l*=phrase_push_statement->phrase_sign->sign_code;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(comp_attr.pic){
      if(phrase_push_statement->child_type==lex_type_string_constant){
        bool negative=false;
        nVM_l=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table)-phrase_push_statement->address-1-SIZEOF_L;
        if(phrase_push_statement->phrase_sign!=NULL){
          nVM_l*=phrase_push_statement->phrase_sign->sign_code;
        }
        if(nVM_l<0){
          nVM_l*=-1;
          negative=true;
        }
        if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
          return 1;
        }
        nVM_uc=op_pushpc;
        if(add_to_outstream((unsigned char *)&nVM_uc,1)){
          return 1;
        }
        if(negative){
          nVM_uc=op_subl;
          if(add_to_outstream((unsigned char *)&nVM_uc,1)){
            return 1;
          }
        }
        else{
          nVM_uc=op_addl;
          if(add_to_outstream((unsigned char *)&nVM_uc,1)){
            return 1;
          }
        }
      }
      else if(phrase_push_statement->child_type==phrase_type_symbol){
        bool negative=false;
        nVM_l=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL)-phrase_push_statement->address-1-SIZEOF_L;
        if(phrase_push_statement->phrase_sign!=NULL){
          nVM_l*=phrase_push_statement->phrase_sign->sign_code;
        }
        if(nVM_l<0){
          nVM_l*=-1;
          negative=true;
        }
        if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
          return 1;
        }
        nVM_uc=op_pushpc;
        if(add_to_outstream((unsigned char *)&nVM_uc,1)){
          return 1;
        }
        if(negative){
          nVM_uc=op_subl;
          if(add_to_outstream((unsigned char *)&nVM_uc,1)){
            return 1;
          }
        }
        else{
          nVM_uc=op_addl;
          if(add_to_outstream((unsigned char *)&nVM_uc,1)){
            return 1;
          }
        }
      }
    }
    else{
      if(phrase_push_statement->child_type==lex_type_string_constant){
        nVM_l=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
        if(phrase_push_statement->phrase_sign!=NULL){
          nVM_l*=phrase_push_statement->phrase_sign->sign_code;
        }
        if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
          return 1;
        }
      }
      else if(phrase_push_statement->child_type==phrase_type_symbol){
        nVM_l=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->parent->tu,false,NULL);
        if(phrase_push_statement->phrase_sign!=NULL){
          nVM_l*=phrase_push_statement->phrase_sign->sign_code;
        }
        if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
          return 1;
        }
      }
    }
  }
  return 0;
}

unsigned int gen_code_phrase_instruction(instruction *phrase_instruction){
  nightVM_uc pad_nop=op_nop;
  for(nightVM_ui i=0;i<phrase_instruction->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  nightVM_uc nVM_uc;
  if(phrase_instruction->instruction_code==key_addc){
    nightVM_uc nVM_uc=op_adduc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_adds){
    nightVM_uc nVM_uc=op_addus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_addi){
    nightVM_uc nVM_uc=op_addui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_addp){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_addl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+(SIZEOF_L+1+1+5+1+SIZEOF_L)+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_addl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<5;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_orl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_addl){
    nightVM_uc nVM_uc=op_addl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_adduc){
    nightVM_uc nVM_uc=op_adduc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_addus){
    nightVM_uc nVM_uc=op_addus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_addui){
    nightVM_uc nVM_uc=op_addui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_subc){
    nightVM_uc nVM_uc=op_subc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_subs){
    nightVM_uc nVM_uc=op_subs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_subi){
    nightVM_uc nVM_uc=op_subl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_subp){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_subl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+(SIZEOF_L+1+1+5+1+SIZEOF_L)+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_subl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<5;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_orl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_subl){
    nightVM_uc nVM_uc=op_subl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_subuc){
    nightVM_uc nVM_uc=op_subuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_subus){
    nightVM_uc nVM_uc=op_subus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_subui){
    nightVM_uc nVM_uc=op_subui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_mulc){
    nightVM_uc nVM_uc=op_mulc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_muls){
    nightVM_uc nVM_uc=op_muls;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_muli){
    nightVM_uc nVM_uc=op_mull;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_mull){
    nightVM_uc nVM_uc=op_mull;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_muluc){
    nightVM_uc nVM_uc=op_muluc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_mulus){
    nightVM_uc nVM_uc=op_mulus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_mului){
    nightVM_uc nVM_uc=op_mului;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_divc){
    nightVM_uc nVM_uc=op_divc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_divs){
    nightVM_uc nVM_uc=op_divs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_divi){
    nightVM_uc nVM_uc=op_divl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_divl){
    nightVM_uc nVM_uc=op_divl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_divuc){
    nightVM_uc nVM_uc=op_divuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_divus){
    nightVM_uc nVM_uc=op_divus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_divui){
    nightVM_uc nVM_uc=op_divui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_remc){
    nightVM_uc nVM_uc=op_remc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_rems){
    nightVM_uc nVM_uc=op_rems;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_remi){
    nightVM_uc nVM_uc=op_reml;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_reml){
    nightVM_uc nVM_uc=op_reml;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_remuc){
    nightVM_uc nVM_uc=op_remuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_remus){
    nightVM_uc nVM_uc=op_remus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_remui){
    nightVM_uc nVM_uc=op_remui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_lshc){
    nightVM_uc nVM_uc=op_lshc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_lshs){
    nightVM_uc nVM_uc=op_lshs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_lshi){
    nightVM_uc nVM_uc=op_lshl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_lshl){
    nightVM_uc nVM_uc=op_lshl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_lshuc){
    nightVM_uc nVM_uc=op_lshuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_lshus){
    nightVM_uc nVM_uc=op_lshus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_lshui){
    nightVM_uc nVM_uc=op_lshui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_rshc){
    nightVM_uc nVM_uc=op_rshc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_rshs){
    nightVM_uc nVM_uc=op_rshs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_rshi){
    nightVM_uc nVM_uc=op_rshl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_rshl){
    nightVM_uc nVM_uc=op_rshl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_rshuc){
    nightVM_uc nVM_uc=op_rshuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_rshus){
    nightVM_uc nVM_uc=op_rshus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_rshui){
    nightVM_uc nVM_uc=op_rshui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_orc){
    nightVM_uc nVM_uc=op_orc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_ors){
    nightVM_uc nVM_uc=op_ors;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_ori){
    nightVM_uc nVM_uc=op_orl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_orl){
    nightVM_uc nVM_uc=op_orl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_oruc){
    nightVM_uc nVM_uc=op_oruc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_orus){
    nightVM_uc nVM_uc=op_orus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_orui){
    nightVM_uc nVM_uc=op_orui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_andc){
    nightVM_uc nVM_uc=op_andc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_ands){
    nightVM_uc nVM_uc=op_ands;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_andi){
    nightVM_uc nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_andl){
    nightVM_uc nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_anduc){
    nightVM_uc nVM_uc=op_anduc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_andus){
    nightVM_uc nVM_uc=op_andus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_andui){
    nightVM_uc nVM_uc=op_andui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_notc){
    nightVM_uc nVM_uc=op_notc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_nots){
    nightVM_uc nVM_uc=op_nots;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_noti){
    nightVM_uc nVM_uc=op_notl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_notl){
    nightVM_uc nVM_uc=op_notl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_notuc){
    nightVM_uc nVM_uc=op_notuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_notus){
    nightVM_uc nVM_uc=op_notus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_notui){
    nightVM_uc nVM_uc=op_notui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_xorc){
    nightVM_uc nVM_uc=op_xorc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_xors){
    nightVM_uc nVM_uc=op_xors;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_xori){
    nightVM_uc nVM_uc=op_xorl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_xorl){
    nightVM_uc nVM_uc=op_xorl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_xoruc){
    nightVM_uc nVM_uc=op_xoruc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_xorus){
    nightVM_uc nVM_uc=op_xorus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_xorui){
    nightVM_uc nVM_uc=op_xorui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_swap){
    nightVM_uc nVM_uc=op_swap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_pop){
    nightVM_uc nVM_uc=op_pop;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_dup){
    nightVM_uc nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jmp || phrase_instruction->instruction_code==key_ret){
    nightVM_uc nVM_uc=op_jmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jeq){
    nightVM_uc nVM_uc=op_jeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jgt){
    nightVM_uc nVM_uc=op_jgt;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jls){
    nightVM_uc nVM_uc=op_jls;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jle){
    nightVM_uc nVM_uc=op_jle;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jge){
    nightVM_uc nVM_uc=op_jge;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jne){
    nightVM_uc nVM_uc=op_jne;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jz){
    nightVM_uc nVM_uc=op_jz;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_jnz || phrase_instruction->instruction_code==key_jmp_panic){
    nightVM_uc nVM_uc=op_jnz;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_over){
    nightVM_uc nVM_uc=op_over;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_panic){
    nightVM_uc nVM_uc=op_panic;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_write0 || phrase_instruction->instruction_code==key_vwrite0){
    nightVM_uc nVM_uc=op_str0;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_write1 || phrase_instruction->instruction_code==key_vwrite1){
    nightVM_uc nVM_uc=op_str1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_write2 || phrase_instruction->instruction_code==key_vwrite2){
    nightVM_uc nVM_uc=op_str2;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_write3 || phrase_instruction->instruction_code==key_vwrite3){
    nightVM_uc nVM_uc=op_str3;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_write4 || phrase_instruction->instruction_code==key_vwrite4){
    nightVM_uc nVM_uc=op_str4;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_write5 || phrase_instruction->instruction_code==key_vwrite5){
    nightVM_uc nVM_uc=op_str5;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_write6 || phrase_instruction->instruction_code==key_vwrite6){
    nightVM_uc nVM_uc=op_str6;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_write7 || phrase_instruction->instruction_code==key_vwrite7){
    nightVM_uc nVM_uc=op_str7;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_get0 || phrase_instruction->instruction_code==key_vget0){
    nightVM_uc nVM_uc=op_ldr0;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_get1 || phrase_instruction->instruction_code==key_vget1){
    nightVM_uc nVM_uc=op_ldr1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_get2 || phrase_instruction->instruction_code==key_vget2){
    nightVM_uc nVM_uc=op_ldr2;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_get3 || phrase_instruction->instruction_code==key_vget3){
    nightVM_uc nVM_uc=op_ldr3;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_get4 || phrase_instruction->instruction_code==key_vget4){
    nightVM_uc nVM_uc=op_ldr4;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_get5 || phrase_instruction->instruction_code==key_vget5){
    nightVM_uc nVM_uc=op_ldr5;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_get6 || phrase_instruction->instruction_code==key_vget6){
    nightVM_uc nVM_uc=op_ldr6;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_get7 || phrase_instruction->instruction_code==key_vget7){
    nightVM_uc nVM_uc=op_ldr7;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_loadc || phrase_instruction->instruction_code==key_vloadc){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldcc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldhc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_loads || phrase_instruction->instruction_code==key_vloads){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldcs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldhs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_loadi || phrase_instruction->instruction_code==key_vloadi){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldci;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldhi;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_loadl || phrase_instruction->instruction_code==key_loadp || phrase_instruction->instruction_code==key_vloadl || phrase_instruction->instruction_code==key_vloadp){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldcl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldhl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_loaduc || phrase_instruction->instruction_code==key_vloaduc){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldcuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldhuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_loadus || phrase_instruction->instruction_code==key_vloadus){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldcus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldhus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_loadui || phrase_instruction->instruction_code==key_vloadui){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldcui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_ldhui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_aloadc || phrase_instruction->instruction_code==key_valoadc){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldcc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldhc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_aloads || phrase_instruction->instruction_code==key_valoads){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldcs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldhs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_aloadi || phrase_instruction->instruction_code==key_valoadi){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldci;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldhi;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_aloadl || phrase_instruction->instruction_code==key_aloadp || phrase_instruction->instruction_code==key_valoadl || phrase_instruction->instruction_code==key_valoadp){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldcl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldhl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_aloaduc || phrase_instruction->instruction_code==key_valoaduc){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldcuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldhuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_aloadus || phrase_instruction->instruction_code==key_valoadus){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldcus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldhus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_aloadui || phrase_instruction->instruction_code==key_valoadui){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldcui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_aldhui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_storec || phrase_instruction->instruction_code==key_vstorec){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_stores || phrase_instruction->instruction_code==key_vstores){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sths;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_storei || phrase_instruction->instruction_code==key_vstorei){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthi;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_storel || phrase_instruction->instruction_code==key_storep || phrase_instruction->instruction_code==key_vstorel || phrase_instruction->instruction_code==key_vstorep){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_storeuc || phrase_instruction->instruction_code==key_vstoreuc){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_storeus || phrase_instruction->instruction_code==key_vstoreus){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_storeui || phrase_instruction->instruction_code==key_vstoreui){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_astorec || phrase_instruction->instruction_code==key_vastorec){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_astores || phrase_instruction->instruction_code==key_vastores){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sths;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_astorei || phrase_instruction->instruction_code==key_vastorei){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthi;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_astorel || phrase_instruction->instruction_code==key_astorep || phrase_instruction->instruction_code==key_vastorel || phrase_instruction->instruction_code==key_vastorep){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_astoreuc || phrase_instruction->instruction_code==key_vastoreuc){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_astoreus || phrase_instruction->instruction_code==key_vastoreus){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthus;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_astoreui || phrase_instruction->instruction_code==key_vastoreui){
    nVM_uc=op_dup;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_nop;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjeq;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_trap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushuc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_rjmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_andl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_sthui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_hlt){
    nightVM_uc nVM_uc=op_halt;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_copy){
    nightVM_uc nVM_uc=op_copy;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_pcopy){
    nightVM_uc nVM_uc=op_pcopy;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_popa){
    nightVM_uc nVM_uc=op_popa;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_put){
    nightVM_uc nVM_uc=op_sts;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_pushsp){
    nightVM_uc nVM_uc=op_pushsp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_hltr){
    nightVM_uc nVM_uc=op_haltr;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_nop){
    nightVM_uc nVM_uc=op_nop;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_incsp){
    nightVM_uc nVM_uc=op_incsp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_decsp){
    nightVM_uc nVM_uc=op_decsp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_exit){
    nightVM_uc nVM_uc=op_halt;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_force_panic){
    nightVM_uc nVM_uc=op_force_panic;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_pushlt){
    nightVM_uc nVM_uc=op_pushlt;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_open){
    if(comp_attr.std==std_chlore2x){
      fprintf(stderr,"warning: open is an implementation-specific keyword of 4NI\n");
    }
    nightVM_uc nVM_uc=op_open;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_invoke){
    if(comp_attr.std==std_chlore2x){
      fprintf(stderr,"warning: invoke is an implementation-specific keyword of 4NI\n");
    }
    nightVM_uc nVM_uc=op_invoke;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_pushpc){
    nightVM_uc nVM_uc=op_pushpc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_pushcs){
    nightVM_uc nVM_uc=op_pushcs;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_code==key_call){
    nVM_uc=op_pushpc;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_pushl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nightVM_l nVM_l=1+SIZEOF_L+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=op_addl;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_swap;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=op_jmp;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  return 0;
}

nightVM_l get_size_of_type(translation_unit *tu, struct_tag_definition_sequence *phrase_struct_tag_definition_sequence, struct_definition_table *struct_table, char *tag, char **type_matched){
  nightVM_l size=0;
  nightVM_l add_to_size=0;
  while(1){
    struct_tag_definition *phrase_struct_tag_definition=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition;
    if(phrase_struct_tag_definition->child_type==phrase_type_type){
      if(phrase_struct_tag_definition->child.phrase_type->type_code==key_uc){
        add_to_size+=1;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_us){
        add_to_size+=SIZEOF_US;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_ui){
        add_to_size+=SIZEOF_UI;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_c){
        add_to_size+=1;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_s){
        add_to_size+=SIZEOF_S;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_i){
        add_to_size+=SIZEOF_I;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_l){
        add_to_size+=SIZEOF_L;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_p){
        add_to_size+=SIZEOF_L;
        *type_matched=NULL;
      }
    }
    else if(phrase_struct_tag_definition->child_type==phrase_type_struct_name){
      add_to_size+=lookup_in_struct_definition_table(phrase_struct_tag_definition->child.phrase_struct_name->lex_identifier,struct_table,tu,false,NULL,ret_size);
      *type_matched=phrase_struct_tag_definition->child.phrase_struct_name->lex_identifier;
    }
    if(strcmp(phrase_struct_tag_definition->lex_identifier,tag)==0){
      break;
    }
    size+=add_to_size;
    add_to_size=0;
    phrase_struct_tag_definition_sequence=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence;
  }
  return size;
}

unsigned int gen_code_tagged_expression(tagged_expression *phrase_tagged_expression, struct_definition_table *struct_table){
  nightVM_l size=0;
  struct_definition *phrase_struct_definition;
  char *lookup=phrase_tagged_expression->phrase_struct_name->lex_identifier;
  lookup_in_struct_definition_table(lookup,struct_table,phrase_tagged_expression->parent->parent->tu,false,&phrase_struct_definition,ret_size);
  tag_sequence *phrase_tag_sequence=phrase_tagged_expression->phrase_tag_sequence;
  while(phrase_tag_sequence!=NULL){
    size+=get_size_of_type(phrase_tagged_expression->parent->parent->tu,phrase_struct_definition->phrase_struct_tag_definition_sequence,struct_table,phrase_tag_sequence->phrase_tag->lex_identifier,&lookup);
    if(lookup!=NULL){
      lookup_in_struct_definition_table(lookup,struct_table,phrase_tagged_expression->parent->parent->tu,false,&phrase_struct_definition,ret_size);
    }
    else{
      break;
    }
    phrase_tag_sequence=phrase_tag_sequence->phrase_tag_sequence;
  }
  nightVM_uc pad_nop=op_nop;
  for(nightVM_ui i=0;i<phrase_tagged_expression->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  nightVM_uc nVM_uc=op_pushl;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  nightVM_l nVM_l=size;
  if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
    return 1;
  }
  return 0;
}

unsigned int gen_code_sizeof_statement(sizeof_statement *phrase_sizeof_statement, struct_definition_table *struct_table){
  nightVM_uc pad_nop=op_nop;
  for(nightVM_ui i=0;i<phrase_sizeof_statement->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  nightVM_uc nVM_uc=op_pushl;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  nightVM_l nVM_l;
  if(phrase_sizeof_statement->child_type==phrase_type_type){
    if(phrase_sizeof_statement->child.phrase_type->type_code==key_uc){
      nVM_l=1;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_code==key_us){
      nVM_l=SIZEOF_US;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_code==key_ui){
      nVM_l=SIZEOF_UI;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_code==key_c){
      nVM_l=1;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_code==key_s){
      nVM_l=SIZEOF_S;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_code==key_i){
      nVM_l=SIZEOF_I;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_code==key_l){
      nVM_l=SIZEOF_L;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_code==key_p){
      nVM_l=SIZEOF_L;
    }
  }
  else if(phrase_sizeof_statement->child_type==phrase_type_struct_name){
    nVM_l=lookup_in_struct_definition_table(phrase_sizeof_statement->child.phrase_struct_name->lex_identifier,struct_table,phrase_sizeof_statement->parent->parent->tu,false,NULL,ret_size);
  }
  if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
    return 1;
  }
  return 0;
}

unsigned int gen_code_alignof_statement(alignof_statement *phrase_alignof_statement, struct_definition_table *struct_table){
  nightVM_uc pad_nop=op_nop;
  for(nightVM_ui i=0;i<phrase_alignof_statement->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  nightVM_uc nVM_uc=op_pushl;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  nightVM_l nVM_l;
  if(phrase_alignof_statement->child_type==phrase_type_type){
    if(phrase_alignof_statement->child.phrase_type->type_code==key_uc){
      nVM_l=1;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_code==key_us){
      nVM_l=ALIGNOF_US;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_code==key_ui){
      nVM_l=ALIGNOF_UI;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_code==key_c){
      nVM_l=1;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_code==key_s){
      nVM_l=ALIGNOF_S;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_code==key_i){
      nVM_l=ALIGNOF_I;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_code==key_l){
      nVM_l=ALIGNOF_L;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_code==key_p){
      nVM_l=ALIGNOF_L;
    }
  }
  else if(phrase_alignof_statement->child_type==phrase_type_struct_name){
    nVM_l=lookup_in_struct_definition_table(phrase_alignof_statement->child.phrase_struct_name->lex_identifier,struct_table,phrase_alignof_statement->parent->parent->tu,false,NULL,ret_align);
  }
  if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
    return 1;
  }
  return 0;
}

static inline unsigned int write_string_table(str_table *string_table){
  while(string_table!=NULL){
    size_t len=cseqlen(string_table->string);
    for(size_t i=0;i<len+1;i++){
      if(add_to_outstream((unsigned char *)&string_table->string[i],1)){
        return 1;
      }
    }
    string_table=string_table->next_node;
  }
  return 0;
}

static inline unsigned int write_libraries(libs *libraries){
  while(libraries!=NULL){
    size_t len=cseqlen(libraries->lib_name);
    for(size_t i=0;i<len+1;i++){
      if(add_to_outstream((unsigned char *)&libraries->lib_name[i],1)){
        return 1;
      }
    }
    libraries=libraries->next;
  }
  return 0;
}

static inline unsigned int write_libraries_init(libs *libraries){
  while(libraries!=NULL){
    nightVM_uc pad_nop=op_nop;
    for(nightVM_ui i=0;i<libraries->align_pad;i++){
      if(add_to_outstream((unsigned char *)&pad_nop,1)){
        return 1;
      }
    }
    if(comp_attr.pic){
      nightVM_uc nVM_uc=op_pushl;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
      libraries->address=libraries->refi_address+1+SIZEOF_L-libraries->address;
      if(add_to_outstream((unsigned char *)&libraries->address,SIZEOF_L)){
        return 1;
      }
      nVM_uc=op_pushpc;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
      nVM_uc=op_subl;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
      nVM_uc=op_open;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    else{
      nightVM_uc nVM_uc=op_pushl;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
      if(add_to_outstream((unsigned char *)&libraries->address,SIZEOF_L)){
        return 1;
      }
      nVM_uc=op_open;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    libraries=libraries->next;
  }
  return 0;
}

unsigned int gen_code_statement(statement *phrase_statement, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries){
  if(phrase_statement->child_type==phrase_type_push_statement){
    if(gen_code_push_statement(phrase_statement->child.phrase_push_statement,string_table,symbol_table)){
      return 1;
    }
  }
  else if(phrase_statement->child_type==phrase_type_alignof_statement){
    if(gen_code_alignof_statement(phrase_statement->child.phrase_alignof_statement,struct_table)){
      return 1;
    }
  }
  else if(phrase_statement->child_type==phrase_type_sizeof_statement){
    if(gen_code_sizeof_statement(phrase_statement->child.phrase_sizeof_statement,struct_table)){
      return 1;
    }
  }
  else if(phrase_statement->child_type==phrase_type_instruction){
    if(gen_code_phrase_instruction(phrase_statement->child.phrase_instruction)){
      return 1;
    }
  }
  else if(phrase_statement->child_type==phrase_type_tagged_expression){
    if(gen_code_tagged_expression(phrase_statement->child.phrase_tagged_expression,struct_table)){
      return 1;
    }
  }
  else if(phrase_statement->child_type==phrase_type_label && strcmp(phrase_statement->child.phrase_label->lex_identifier,"main")==0){
    if(write_libraries_init(libraries)){
      return 1;
    }
  }
  return 0;
}

unsigned int gen_code_statement_sequence(statement_sequence *phrase_statement_sequence, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries){
  while(phrase_statement_sequence!=NULL){
    if(gen_code_statement(phrase_statement_sequence->phrase_statement,string_table,symbol_table,struct_table,libraries)){
      return 1;
    }
    phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
  }
  return 0;
}

unsigned int gen_code_translation_unit(translation_unit *phrase_translation_unit, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries){
  if(gen_code_statement_sequence(phrase_translation_unit->phrase_statement_sequence,string_table,symbol_table,struct_table,libraries)){
    return 1;
  }
  return 0;
}

unsigned int gen_code_translation_unit_list(translation_unit_list *tu_list, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries){
  if(comp_attr.out_fileformat==oform_esff23){
    nightVM_ui magic=0X000E3EFF;
    if(add_to_outstream((unsigned char *)&magic,SIZEOF_UI)){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      clean_add_to_stream_res();
      return 1;
    }
    nightVM_ui specification_version=0x00000023;
    if(add_to_outstream((unsigned char *)&specification_version,SIZEOF_UI)){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.code_alignment,SIZEOF_UI)){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.heap_alignment,SIZEOF_UI)){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.heap_size,SIZEOF_UNS)){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.stack_size,SIZEOF_UNS)){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      clean_add_to_stream_res();
      return 1;
    }
    nightVM_uns entry_point=lookup_in_symbol_table("main",symbol_table,NULL,0,NULL);
    if(add_to_outstream((unsigned char *)&entry_point,SIZEOF_UNS)){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      clean_add_to_stream_res();
      return 1;
    }
  }
  if(write_string_table(string_table)){
    return 1;
  }
  if(write_libraries(libraries)){
    return 1;
  }
  while(tu_list!=NULL){
    if(gen_code_translation_unit(tu_list->tu,string_table,symbol_table,struct_table,libraries)){
      clean_add_to_stream_res();
      return 1;
    }
    tu_list=tu_list->next_tu;
  }
  if(flush_to_outstream()){
    clean_add_to_stream_res();
    return 1;
  }
  clean_add_to_stream_res();
  return 0;
}
