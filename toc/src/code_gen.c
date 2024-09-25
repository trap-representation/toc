#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "conv.h"
#include "nvm_sources.h"
#include TOC_NVM_OPCODES_H
#include "write_error.h"
#include "comp_properties.h"
#include "tree.h"
#include "code_gen.h"

extern compilation_attributes comp_attr;

#define ESFF23xCOM_STRL "#!/usr/bin/env -S nightVM -i\n"

#define OUTBUF_SIZE 1048576

static ysm_uc *outbuf=NULL;
static size_t outbuf_filled_upto=0;
static FILE *outstream=NULL;

static unsigned int add_to_outstream(unsigned char *buf, size_t size){
  if(outstream==NULL){
    if((outstream=fopen(comp_attr.outfile,"wb"))==NULL){
      fprintf(stderr,"implementation error: failed to open file %s for writing\n",comp_attr.outfile);
      return 1;
    }
  }
  if(outbuf==NULL){
    if((outbuf=malloc(OUTBUF_SIZE*sizeof(ysm_uc)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
  }
  if(OUTBUF_SIZE-outbuf_filled_upto>=size){
    memcpy(&outbuf[outbuf_filled_upto],buf,size*sizeof(ysm_uc));
    outbuf_filled_upto+=size;
  }
  else{
    if(fwrite(outbuf,sizeof(ysm_uc),outbuf_filled_upto,outstream)<outbuf_filled_upto){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      return 1;
    }
    memcpy(outbuf,buf,size*sizeof(ysm_uc));
    outbuf_filled_upto=size;
  }
  return 0;
}

static unsigned int flush_to_outstream(void){
  if(outstream!=NULL && outbuf!=NULL){
    if(fwrite(outbuf,sizeof(ysm_uc),outbuf_filled_upto,outstream)<outbuf_filled_upto){
      fprintf(stderr,"implementation error: failed to write to file %s\n",comp_attr.outfile);
      return 1;
    }
  }
  return 0;
}

static void clean_add_to_stream_res(void){
  fclose(outstream);
  free(outbuf);
}

unsigned int gen_code_push_statement(push_statement *phrase_push_statement, str_table *string_table, sym_table *symbol_table){
  ysm_uc pad_nop=OP_NOP;
  for(ysm_ui i=0;i<phrase_push_statement->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  if(phrase_push_statement->phrase_push_instruction->push_instruction_type==push_instruction_pushuc){
    ysm_uc nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_uc=numeric_constant_to_uc(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_uc*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_uc=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_uc*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_uc=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_uc*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_uc=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_uc*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_type==push_instruction_pushus){
    ysm_uc nVM_uc=OP_PUSHUS;
    ysm_us nVM_us;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_us=numeric_constant_to_us(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_us*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_us,SIZEOF_US)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_us=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_us*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_us,SIZEOF_US)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_us=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_us*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_us,SIZEOF_US)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_us=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_us*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_us,SIZEOF_US)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_type==push_instruction_pushui){
    ysm_uc nVM_uc=OP_PUSHUI;
    ysm_ui nVM_ui;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_ui=numeric_constant_to_ui(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_ui*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_ui,SIZEOF_UI)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_ui=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_ui*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_ui,SIZEOF_UI)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_ui=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_ui*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_ui,SIZEOF_UI)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_ui=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_ui*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_ui,SIZEOF_UI)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_type==push_instruction_pushc){
    ysm_uc nVM_uc=OP_PUSHC;
    ysm_c nVM_c;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_c=numeric_constant_to_c(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_c*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_c,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_c=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_c*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_c,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_c=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_c*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_c,1)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_c=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_c*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_c,1)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_type==push_instruction_pushs){
    ysm_uc nVM_uc=OP_PUSHS;
    ysm_s nVM_s;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_s=numeric_constant_to_s(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_s*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_s,SIZEOF_S)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_s=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_s*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_s,SIZEOF_S)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_s=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_s*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_s,SIZEOF_S)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_s=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_s*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_s,SIZEOF_S)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_type==push_instruction_pushi){
    ysm_uc nVM_uc=OP_PUSHI;
    ysm_i nVM_i;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_i=numeric_constant_to_i(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_i*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_i,SIZEOF_I)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_i=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_i*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_i,SIZEOF_I)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_i=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_i*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_i,SIZEOF_I)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_i=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_i*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_i,SIZEOF_I)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_type==push_instruction_pushl){
    ysm_uc nVM_uc=OP_PUSHL;
    ysm_l nVM_l;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_l=numeric_constant_to_l(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_l*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      nVM_l=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_l*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      nVM_l=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_l*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_l=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_l*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
  }
  else if(phrase_push_statement->phrase_push_instruction->push_instruction_type==push_instruction_pushp){
    ysm_uc nVM_uc=OP_PUSHL;
    ysm_l nVM_l;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      nVM_l=numeric_constant_to_p(phrase_push_statement->child.lex_numeric_constant);
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_l*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      nVM_l=phrase_push_statement->child.lex_character_constant[0];
      if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
        nVM_l*=-1;
      }
      if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
        return 1;
      }
    }
    else if(comp_attr.pic){
      if(phrase_push_statement->child_type==lex_type_string_constant){
        bool negative=false;
        nVM_l=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table)-phrase_push_statement->address-1-SIZEOF_L;
        if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
          nVM_l*=-1;
        }
        if(nVM_l<0){
          nVM_l*=-1;
          negative=true;
        }
        if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
          return 1;
        }
        nVM_uc=OP_PUSHPC;
        if(add_to_outstream((unsigned char *)&nVM_uc,1)){
          return 1;
        }
        if(negative){
          nVM_uc=OP_SUBL;
          if(add_to_outstream((unsigned char *)&nVM_uc,1)){
            return 1;
          }
        }
        else{
          nVM_uc=OP_ADDL;
          if(add_to_outstream((unsigned char *)&nVM_uc,1)){
            return 1;
          }
        }
      }
      else if(phrase_push_statement->child_type==phrase_type_symbol){
        bool negative=false;
        nVM_l=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL)-phrase_push_statement->address-1-SIZEOF_L;
        if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
          nVM_l*=-1;
        }
        if(nVM_l<0){
          nVM_l*=-1;
          negative=true;
        }
        if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
          return 1;
        }
        nVM_uc=OP_PUSHPC;
        if(add_to_outstream((unsigned char *)&nVM_uc,1)){
          return 1;
        }
        if(negative){
          nVM_uc=OP_SUBL;
          if(add_to_outstream((unsigned char *)&nVM_uc,1)){
            return 1;
          }
        }
        else{
          nVM_uc=OP_ADDL;
          if(add_to_outstream((unsigned char *)&nVM_uc,1)){
            return 1;
          }
        }
      }
    }
    else{
      if(phrase_push_statement->child_type==lex_type_string_constant){
        nVM_l=lookup_in_string_table(phrase_push_statement->child.lex_string_constant,string_table);
        if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
          nVM_l*=-1;
        }
        if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
          return 1;
        }
      }
      else if(phrase_push_statement->child_type==phrase_type_symbol){
        nVM_l=lookup_in_symbol_table(phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,phrase_push_statement->parent->tu,false,NULL);
        if(phrase_push_statement->phrase_sign!=NULL && phrase_push_statement->phrase_sign->sign_type==sign_negative){
          nVM_l*=-1;
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
  ysm_uc pad_nop=OP_NOP;
  for(ysm_ui i=0;i<phrase_instruction->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  ysm_uc nVM_uc;
  if(phrase_instruction->instruction_type==instruction_addc){
    ysm_uc nVM_uc=OP_ADDUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_adds){
    ysm_uc nVM_uc=OP_ADDUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_addi){
    ysm_uc nVM_uc=OP_ADDUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_addp){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ADDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+(SIZEOF_L+1+1+5+1+SIZEOF_L)+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ADDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<5;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ORL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_addl){
    ysm_uc nVM_uc=OP_ADDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_adduc){
    ysm_uc nVM_uc=OP_ADDUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_addus){
    ysm_uc nVM_uc=OP_ADDUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_addui){
    ysm_uc nVM_uc=OP_ADDUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_subc){
    ysm_uc nVM_uc=OP_SUBC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_subs){
    ysm_uc nVM_uc=OP_SUBS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_subi){
    ysm_uc nVM_uc=OP_SUBL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_subp){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_SUBL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+(SIZEOF_L+1+1+5+1+SIZEOF_L)+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_SUBL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<5;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ORL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_subl){
    ysm_uc nVM_uc=OP_SUBL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_subuc){
    ysm_uc nVM_uc=OP_SUBUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_subus){
    ysm_uc nVM_uc=OP_SUBUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_subui){
    ysm_uc nVM_uc=OP_SUBUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_mulc){
    ysm_uc nVM_uc=OP_MULC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_muls){
    ysm_uc nVM_uc=OP_MULS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_muli){
    ysm_uc nVM_uc=OP_MULL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_mull){
    ysm_uc nVM_uc=OP_MULL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_muluc){
    ysm_uc nVM_uc=OP_MULUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_mulus){
    ysm_uc nVM_uc=OP_MULUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_mului){
    ysm_uc nVM_uc=OP_MULUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_divc){
    ysm_uc nVM_uc=OP_DIVC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_divs){
    ysm_uc nVM_uc=OP_DIVS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_divi){
    ysm_uc nVM_uc=OP_DIVL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_divl){
    ysm_uc nVM_uc=OP_DIVL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_divuc){
    ysm_uc nVM_uc=OP_DIVUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_divus){
    ysm_uc nVM_uc=OP_DIVUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_divui){
    ysm_uc nVM_uc=OP_DIVUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_remc){
    ysm_uc nVM_uc=OP_REMC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rems){
    ysm_uc nVM_uc=OP_REMS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_remi){
    ysm_uc nVM_uc=OP_REML;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_reml){
    ysm_uc nVM_uc=OP_REML;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_remuc){
    ysm_uc nVM_uc=OP_REMUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_remus){
    ysm_uc nVM_uc=OP_REMUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_remui){
    ysm_uc nVM_uc=OP_REMUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_lshc){
    ysm_uc nVM_uc=OP_LSHC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_lshs){
    ysm_uc nVM_uc=OP_LSHS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_lshi){
    ysm_uc nVM_uc=OP_LSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_lshl){
    ysm_uc nVM_uc=OP_LSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_lshuc){
    ysm_uc nVM_uc=OP_LSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_lshus){
    ysm_uc nVM_uc=OP_LSHUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_lshui){
    ysm_uc nVM_uc=OP_LSHUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rshc){
    ysm_uc nVM_uc=OP_RSHC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rshs){
    ysm_uc nVM_uc=OP_RSHS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rshi){
    ysm_uc nVM_uc=OP_RSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rshl){
    ysm_uc nVM_uc=OP_RSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rshuc){
    ysm_uc nVM_uc=OP_RSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rshus){
    ysm_uc nVM_uc=OP_RSHUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rshui){
    ysm_uc nVM_uc=OP_RSHUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_orc){
    ysm_uc nVM_uc=OP_ORC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_ors){
    ysm_uc nVM_uc=OP_ORS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_ori){
    ysm_uc nVM_uc=OP_ORL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_orl){
    ysm_uc nVM_uc=OP_ORL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_oruc){
    ysm_uc nVM_uc=OP_ORUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_orus){
    ysm_uc nVM_uc=OP_ORUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_orui){
    ysm_uc nVM_uc=OP_ORUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_andc){
    ysm_uc nVM_uc=OP_ANDC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_ands){
    ysm_uc nVM_uc=OP_ANDS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_andi){
    ysm_uc nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_andl){
    ysm_uc nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_anduc){
    ysm_uc nVM_uc=OP_ANDUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_andus){
    ysm_uc nVM_uc=OP_ANDUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_andui){
    ysm_uc nVM_uc=OP_ANDUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_notc){
    ysm_uc nVM_uc=OP_NOTC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_nots){
    ysm_uc nVM_uc=OP_NOTS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_noti){
    ysm_uc nVM_uc=OP_NOTL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_notl){
    ysm_uc nVM_uc=OP_NOTL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_notuc){
    ysm_uc nVM_uc=OP_NOTUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_notus){
    ysm_uc nVM_uc=OP_NOTUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_notui){
    ysm_uc nVM_uc=OP_NOTUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_xorc){
    ysm_uc nVM_uc=OP_XORC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_xors){
    ysm_uc nVM_uc=OP_XORS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_xori){
    ysm_uc nVM_uc=OP_XORL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_xorl){
    ysm_uc nVM_uc=OP_XORL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_xoruc){
    ysm_uc nVM_uc=OP_XORUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_xorus){
    ysm_uc nVM_uc=OP_XORUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_xorui){
    ysm_uc nVM_uc=OP_XORUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_swap){
    ysm_uc nVM_uc=OP_SWAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_pop){
    ysm_uc nVM_uc=OP_POP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_dup){
    ysm_uc nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_ret){
    ysm_uc nVM_uc=OP_RET;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_rcall){
    ysm_uc nVM_uc=OP_JMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_eq){
    ysm_uc nVM_uc=OP_CEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_gt){
    ysm_uc nVM_uc=OP_CGT;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_ls){
    ysm_uc nVM_uc=OP_CLS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_le){
    ysm_uc nVM_uc=OP_CLE;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_ge){
    ysm_uc nVM_uc=OP_CGE;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_ne){
    ysm_uc nVM_uc=OP_CNE;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_zr){
    ysm_uc nVM_uc=OP_CZ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_nz){
    ysm_uc nVM_uc=OP_CNZ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_over){
    ysm_uc nVM_uc=OP_OVER;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_panic){
    ysm_uc nVM_uc=OP_PANIC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_write0 || phrase_instruction->instruction_type==instruction_vwrite0){
    ysm_uc nVM_uc=OP_STR0;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_write1 || phrase_instruction->instruction_type==instruction_vwrite1){
    ysm_uc nVM_uc=OP_STR1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_write2 || phrase_instruction->instruction_type==instruction_vwrite2){
    ysm_uc nVM_uc=OP_STR2;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_write3 || phrase_instruction->instruction_type==instruction_vwrite3){
    ysm_uc nVM_uc=OP_STR3;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_write4 || phrase_instruction->instruction_type==instruction_vwrite4){
    ysm_uc nVM_uc=OP_STR4;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_write5 || phrase_instruction->instruction_type==instruction_vwrite5){
    ysm_uc nVM_uc=OP_STR5;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_write6 || phrase_instruction->instruction_type==instruction_vwrite6){
    ysm_uc nVM_uc=OP_STR6;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_write7 || phrase_instruction->instruction_type==instruction_vwrite7){
    ysm_uc nVM_uc=OP_STR7;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_get0 || phrase_instruction->instruction_type==instruction_vget0){
    ysm_uc nVM_uc=OP_LDR0;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_get1 || phrase_instruction->instruction_type==instruction_vget1){
    ysm_uc nVM_uc=OP_LDR1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_get2 || phrase_instruction->instruction_type==instruction_vget2){
    ysm_uc nVM_uc=OP_LDR2;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_get3 || phrase_instruction->instruction_type==instruction_vget3){
    ysm_uc nVM_uc=OP_LDR3;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_get4 || phrase_instruction->instruction_type==instruction_vget4){
    ysm_uc nVM_uc=OP_LDR4;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_get5 || phrase_instruction->instruction_type==instruction_vget5){
    ysm_uc nVM_uc=OP_LDR5;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_get6 || phrase_instruction->instruction_type==instruction_vget6){
    ysm_uc nVM_uc=OP_LDR6;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_get7 || phrase_instruction->instruction_type==instruction_vget7){
    ysm_uc nVM_uc=OP_LDR7;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_loadc || phrase_instruction->instruction_type==instruction_vloadc){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDCC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDHC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_loads || phrase_instruction->instruction_type==instruction_vloads){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDCS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDHS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_loadi || phrase_instruction->instruction_type==instruction_vloadi){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDCI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDHI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_loadl || phrase_instruction->instruction_type==instruction_loadp || phrase_instruction->instruction_type==instruction_vloadl || phrase_instruction->instruction_type==instruction_vloadp){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDCL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_loaduc || phrase_instruction->instruction_type==instruction_vloaduc){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDCUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_loadus || phrase_instruction->instruction_type==instruction_vloadus){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDCUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDHUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_loadui || phrase_instruction->instruction_type==instruction_vloadui){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDCUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_LDHUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_aloadc || phrase_instruction->instruction_type==instruction_valoadc){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDCC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDHC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_aloads || phrase_instruction->instruction_type==instruction_valoads){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDCS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDHS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_aloadi || phrase_instruction->instruction_type==instruction_valoadi){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDCI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDHI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_aloadl || phrase_instruction->instruction_type==instruction_aloadp || phrase_instruction->instruction_type==instruction_valoadl || phrase_instruction->instruction_type==instruction_valoadp){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDCL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_aloaduc || phrase_instruction->instruction_type==instruction_valoaduc){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDCUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_aloadus || phrase_instruction->instruction_type==instruction_valoadus){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDCUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDHUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_aloadui || phrase_instruction->instruction_type==instruction_valoadui){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDCUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_ALDHUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_storec || phrase_instruction->instruction_type==instruction_vstorec){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_stores || phrase_instruction->instruction_type==instruction_vstores){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_storei || phrase_instruction->instruction_type==instruction_vstorei){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_storel || phrase_instruction->instruction_type==instruction_storep || phrase_instruction->instruction_type==instruction_vstorel || phrase_instruction->instruction_type==instruction_vstorep){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_storeuc || phrase_instruction->instruction_type==instruction_vstoreuc){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_storeus || phrase_instruction->instruction_type==instruction_vstoreus){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_storeui || phrase_instruction->instruction_type==instruction_vstoreui){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_astorec || phrase_instruction->instruction_type==instruction_vastorec){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_astores || phrase_instruction->instruction_type==instruction_vastores){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_astorei || phrase_instruction->instruction_type==instruction_vastorei){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_astorel || phrase_instruction->instruction_type==instruction_astorep || phrase_instruction->instruction_type==instruction_vastorel || phrase_instruction->instruction_type==instruction_vastorep){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_astoreuc || phrase_instruction->instruction_type==instruction_vastoreuc){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_astoreus || phrase_instruction->instruction_type==instruction_vastoreus){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHUS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_astoreui || phrase_instruction->instruction_type==instruction_vastoreui){
    nVM_uc=OP_DUP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    ysm_l nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_NOP;
    for(unsigned int i=0;i<6;i++){
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x4000000000000000;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+1+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJEQ;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_TRAP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHUC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=1+SIZEOF_L+1+1+1;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_RJMP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_PUSHL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_l=0x3fffffffffffffff;
    if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
      return 1;
    }
    nVM_uc=OP_ANDL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
    nVM_uc=OP_STHUI;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_hlt){
    ysm_uc nVM_uc=OP_HALT;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_copy){
    ysm_uc nVM_uc=OP_COPY;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_pcopy){
    ysm_uc nVM_uc=OP_PCOPY;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_popa){
    ysm_uc nVM_uc=OP_POPA;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_put){
    ysm_uc nVM_uc=OP_STS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_pushsp){
    ysm_uc nVM_uc=OP_PUSHSP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_hltr){
    ysm_uc nVM_uc=OP_HALTR;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_incsp){
    ysm_uc nVM_uc=OP_INCSP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_decsp){
    ysm_uc nVM_uc=OP_DECSP;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_exit){
    ysm_uc nVM_uc=OP_HALT;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_force_panic){
    ysm_uc nVM_uc=OP_FORCE_PANIC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_pushlt){
    ysm_uc nVM_uc=OP_PUSHLT;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_open){
    if(comp_attr.std==std_chlore2x){
      fprintf(stderr,"warning: open is an implementation-specific keyword of 4NI\n");
    }
    ysm_uc nVM_uc=OP_OPEN;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_invoke){
    if(comp_attr.std==std_chlore2x){
      fprintf(stderr,"warning: invoke is an implementation-specific keyword of 4NI\n");
    }
    ysm_uc nVM_uc=OP_INVOKE;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_pushpc){
    ysm_uc nVM_uc=OP_PUSHPC;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_pushcs){
    ysm_uc nVM_uc=OP_PUSHCS;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  else if(phrase_instruction->instruction_type==instruction_call){
    ysm_uc nVM_uc=OP_CALL;
    if(add_to_outstream((unsigned char *)&nVM_uc,1)){
      return 1;
    }
  }
  return 0;
}

ysm_l get_size_of_type(translation_unit *tu, struct_tag_definition_sequence *phrase_struct_tag_definition_sequence, struct_definition_table *struct_table, char *tag, char **type_matched){
  ysm_l size=0;
  ysm_l add_to_size=0;
  while(1){
    struct_tag_definition *phrase_struct_tag_definition=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition;
    if(phrase_struct_tag_definition->child_type==phrase_type_type){
      if(phrase_struct_tag_definition->child.phrase_type->type_type==type_uc){
        add_to_size+=1;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_type==type_us){
        add_to_size+=SIZEOF_US;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_type==type_ui){
        add_to_size+=SIZEOF_UI;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_type==type_c){
        add_to_size+=1;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_type==type_s){
        add_to_size+=SIZEOF_S;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_type==type_i){
        add_to_size+=SIZEOF_I;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_type==type_l){
        add_to_size+=SIZEOF_L;
        *type_matched=NULL;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_type==type_p){
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

unsigned int gen_code_tagged_statement(tagged_statement *phrase_tagged_statement, struct_definition_table *struct_table){
  ysm_l size=0;
  struct_definition *phrase_struct_definition;
  char *lookup=phrase_tagged_statement->phrase_struct_name->lex_identifier;
  lookup_in_struct_definition_table(lookup,struct_table,phrase_tagged_statement->parent->tu,false,&phrase_struct_definition,ret_size);
  tag_sequence *phrase_tag_sequence=phrase_tagged_statement->phrase_tag_sequence;
  while(phrase_tag_sequence!=NULL){
    size+=get_size_of_type(phrase_tagged_statement->parent->tu,phrase_struct_definition->phrase_struct_tag_definition_sequence,struct_table,phrase_tag_sequence->phrase_tag->lex_identifier,&lookup);
    if(lookup!=NULL){
      lookup_in_struct_definition_table(lookup,struct_table,phrase_tagged_statement->parent->tu,false,&phrase_struct_definition,ret_size);
    }
    else{
      break;
    }
    phrase_tag_sequence=phrase_tag_sequence->phrase_tag_sequence;
  }
  ysm_uc pad_nop=OP_NOP;
  for(ysm_ui i=0;i<phrase_tagged_statement->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  ysm_uc nVM_uc=OP_PUSHL;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  if(add_to_outstream((unsigned char *)&size,SIZEOF_L)){
    return 1;
  }
  return 0;
}

unsigned int gen_code_else_clause(else_clause *phrase_else_clause, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries){
  ysm_uc pad_nop=OP_NOP;
  for(ysm_ui i=0;i<phrase_else_clause->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  ysm_uc nVM_uc=OP_PUSHL;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  if(add_to_outstream((unsigned char *)&phrase_else_clause->rend_address,SIZEOF_L)){
    return 1;
  }
  nVM_uc=OP_RJMP;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  if(phrase_else_clause->child_type==phrase_type_statement_sequence){
    if(gen_code_statement_sequence(phrase_else_clause->child.phrase_statement_sequence,string_table,symbol_table,struct_table,libraries)){
      return 1;
    }
  }
  else{
    if(gen_code_statement(phrase_else_clause->child.phrase_statement,string_table,symbol_table,struct_table,libraries)){
      return 1;
    }
  }
  return 0;
}

unsigned int gen_code_selection_statement(selection_statement *phrase_selection_statement, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries){
  if(gen_code_statement_sequence(phrase_selection_statement->phrase_statement_sequence_0,string_table,symbol_table,struct_table,libraries)){
    return 1;
  }
  ysm_uc pad_nop=OP_NOP;
  for(ysm_ui i=0;i<phrase_selection_statement->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  ysm_uc nVM_uc=OP_PUSHL;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  if(add_to_outstream((unsigned char *)&phrase_selection_statement->rend_address,SIZEOF_L)){
    return 1;
  }
  nVM_uc=OP_RJZ;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
    if(gen_code_statement_sequence(phrase_selection_statement->child.phrase_statement_sequence_1,string_table,symbol_table,struct_table,libraries)){
      return 1;
    }
  }
  else{
    if(gen_code_statement(phrase_selection_statement->child.phrase_statement,string_table,symbol_table,struct_table,libraries)){
      return 1;
    }
  }
  if(phrase_selection_statement->phrase_else_clause!=NULL){
    if(gen_code_else_clause(phrase_selection_statement->phrase_else_clause,string_table,symbol_table,struct_table,libraries)){
      return 1;
    }
  }
  return 0;
}

unsigned int gen_code_sizeof_statement(sizeof_statement *phrase_sizeof_statement, struct_definition_table *struct_table){
  ysm_uc pad_nop=OP_NOP;
  for(ysm_ui i=0;i<phrase_sizeof_statement->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  ysm_uc nVM_uc=OP_PUSHL;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  ysm_l nVM_l;
  if(phrase_sizeof_statement->child_type==phrase_type_type){
    if(phrase_sizeof_statement->child.phrase_type->type_type==type_uc){
      nVM_l=1;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_type==type_us){
      nVM_l=SIZEOF_US;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_type==type_ui){
      nVM_l=SIZEOF_UI;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_type==type_c){
      nVM_l=1;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_type==type_s){
      nVM_l=SIZEOF_S;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_type==type_i){
      nVM_l=SIZEOF_I;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_type==type_l){
      nVM_l=SIZEOF_L;
    }
    else if(phrase_sizeof_statement->child.phrase_type->type_type==type_p){
      nVM_l=SIZEOF_L;
    }
  }
  else if(phrase_sizeof_statement->child_type==phrase_type_struct_name){
    nVM_l=lookup_in_struct_definition_table(phrase_sizeof_statement->child.phrase_struct_name->lex_identifier,struct_table,phrase_sizeof_statement->parent->tu,false,NULL,ret_size);
  }
  if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
    return 1;
  }
  return 0;
}

unsigned int gen_code_alignof_statement(alignof_statement *phrase_alignof_statement, struct_definition_table *struct_table){
  ysm_uc pad_nop=OP_NOP;
  for(ysm_ui i=0;i<phrase_alignof_statement->align_pad;i++){
    if(add_to_outstream((unsigned char *)&pad_nop,1)){
      return 1;
    }
  }
  ysm_uc nVM_uc=OP_PUSHL;
  if(add_to_outstream((unsigned char *)&nVM_uc,1)){
    return 1;
  }
  ysm_l nVM_l;
  if(phrase_alignof_statement->child_type==phrase_type_type){
    if(phrase_alignof_statement->child.phrase_type->type_type==type_uc){
      nVM_l=1;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_type==type_us){
      nVM_l=ALIGNOF_US;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_type==type_ui){
      nVM_l=ALIGNOF_UI;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_type==type_c){
      nVM_l=1;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_type==type_s){
      nVM_l=ALIGNOF_S;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_type==type_i){
      nVM_l=ALIGNOF_I;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_type==type_l){
      nVM_l=ALIGNOF_L;
    }
    else if(phrase_alignof_statement->child.phrase_type->type_type==type_p){
      nVM_l=ALIGNOF_L;
    }
  }
  else if(phrase_alignof_statement->child_type==phrase_type_struct_name){
    nVM_l=lookup_in_struct_definition_table(phrase_alignof_statement->child.phrase_struct_name->lex_identifier,struct_table,phrase_alignof_statement->parent->tu,false,NULL,ret_align);
  }
  if(add_to_outstream((unsigned char *)&nVM_l,SIZEOF_L)){
    return 1;
  }
  return 0;
}

static unsigned int write_string_table(str_table *string_table){
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

static unsigned int write_libraries(libs *libraries){
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

static unsigned int write_libraries_init(libs *libraries){
  while(libraries!=NULL){
    ysm_uc pad_nop=OP_NOP;
    for(ysm_ui i=0;i<libraries->align_pad;i++){
      if(add_to_outstream((unsigned char *)&pad_nop,1)){
        return 1;
      }
    }
    if(comp_attr.pic){
      ysm_uc nVM_uc=OP_PUSHL;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
      libraries->address=libraries->refi_address+1+SIZEOF_L-libraries->address;
      if(add_to_outstream((unsigned char *)&libraries->address,SIZEOF_L)){
        return 1;
      }
      nVM_uc=OP_PUSHPC;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
      nVM_uc=OP_SUBL;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
      nVM_uc=OP_OPEN;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
    }
    else{
      ysm_uc nVM_uc=OP_PUSHL;
      if(add_to_outstream((unsigned char *)&nVM_uc,1)){
        return 1;
      }
      if(add_to_outstream((unsigned char *)&libraries->address,SIZEOF_L)){
        return 1;
      }
      nVM_uc=OP_OPEN;
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
  else if(phrase_statement->child_type==phrase_type_tagged_statement){
    if(gen_code_tagged_statement(phrase_statement->child.phrase_tagged_statement,struct_table)){
      return 1;
    }
  }
  else if(phrase_statement->child_type==phrase_type_selection_statement){
    if(gen_code_selection_statement(phrase_statement->child.phrase_selection_statement,string_table,symbol_table,struct_table,libraries)){
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
    ysm_ui magic=0xE3EFF;
    if(add_to_outstream((unsigned char *)&magic,SIZEOF_UI)){
      clean_add_to_stream_res();
      return 1;
    }
    ysm_ui specification_version=0x00023;
    if(add_to_outstream((unsigned char *)&specification_version,SIZEOF_UI)){
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.code_alignment,SIZEOF_UI)){
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.heap_alignment,SIZEOF_UI)){
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.heap_size,SIZEOF_UNS)){
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.stack_size,SIZEOF_UNS)){
      clean_add_to_stream_res();
      return 1;
    }
    nightvm_uns entry_point=lookup_in_symbol_table("main",symbol_table,NULL,0,NULL);
    if(add_to_outstream((unsigned char *)&entry_point,SIZEOF_UNS)){
      clean_add_to_stream_res();
      return 1;
    }
  }
  else if(comp_attr.out_fileformat==oform_esff23x){
    ysm_c *esff23xcom;
    if((esff23xcom=str2cseq(ESFF23xCOM_STRL))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      clean_add_to_stream_res();
      return 1;
    }
    size_t len=cseqlen(esff23xcom);
    for(size_t i=0;i<len;i++){
      if(add_to_outstream((unsigned char *)&esff23xcom[i],1)){
        free(esff23xcom);
        clean_add_to_stream_res();
        return 1;
      }
    }
    free(esff23xcom);
    ysm_uc zero=0;
    if(add_to_outstream((unsigned char *)&zero,1)){
      clean_add_to_stream_res();
      return 1;
    }
    ysm_ui magic=0xE3EF6;
    if(add_to_outstream((unsigned char *)&magic,SIZEOF_UI)){
      clean_add_to_stream_res();
      return 1;
    }
    ysm_ui specification_version=0x00023;
    if(add_to_outstream((unsigned char *)&specification_version,SIZEOF_UI)){
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.code_alignment,SIZEOF_UI)){
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.heap_alignment,SIZEOF_UI)){
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.heap_size,SIZEOF_UNS)){
      clean_add_to_stream_res();
      return 1;
    }
    if(add_to_outstream((unsigned char *)&comp_attr.stack_size,SIZEOF_UNS)){
      clean_add_to_stream_res();
      return 1;
    }
    nightvm_uns entry_point=lookup_in_symbol_table("main",symbol_table,NULL,0,NULL);
    if(add_to_outstream((unsigned char *)&entry_point,SIZEOF_UNS)){
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
