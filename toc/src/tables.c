#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define TOC_ADD_NUMERIC_CONSTANT
#define TOC_ADD_VISIBILITY
#define TOC_ADD_KEYWORD
#include "psg_idens.h"
#include "comp_properties.h"
#include "write_error.h"
#include "tables.h"

extern compilation_attributes comp_attr;

#define FILE_READ_CHUNK_SIZE 1048576

static unsigned int add_to_str_table_if_valid(statement *phrase_statement, str_table **string_table, str_table *e_string_table, str_table **curr){
  if(phrase_statement->child_type==phrase_type_push_statement && phrase_statement->child.phrase_push_statement->child_type==lex_type_string_constant && lookup_in_string_table(phrase_statement->child.phrase_push_statement->child.lex_string_constant,e_string_table)==-1){
    if(*string_table==NULL){
      if((*string_table=malloc(sizeof(str_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      (*string_table)->next_node=NULL;
      *curr=*string_table;
    }
    else{
      if(((*curr)->next_node=malloc(sizeof(str_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      *curr=(*curr)->next_node;
      (*curr)->next_node=NULL;
    }
    if(((*curr)->string=str2cseq(phrase_statement->child.phrase_push_statement->child.lex_string_constant))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    (*curr)->address=0;
  }
  else if(phrase_statement->child_type==phrase_type_set_statement && phrase_statement->child.phrase_set_statement->child_type==lex_type_string_constant && lookup_in_string_table(phrase_statement->child.phrase_set_statement->child.lex_string_constant,e_string_table)==-1){
    if(*string_table==NULL){
      if((*string_table=malloc(sizeof(str_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      (*string_table)->next_node=NULL;
      *curr=*string_table;
    }
    else{
      if(((*curr)->next_node=malloc(sizeof(str_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      *curr=(*curr)->next_node;
      (*curr)->next_node=NULL;
    }
    if(((*curr)->string=str2cseq(phrase_statement->child.phrase_set_statement->child.lex_string_constant))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    (*curr)->address=0;
  }
  return 0;
}

static unsigned int walk_tree_and_create_str_table(statement_sequence *phrase_statement_sequence, statement *phrase_statement, str_table **string_table, str_table *e_string_table, str_table **curr){
  if(phrase_statement!=NULL){
    if(phrase_statement->child_type==phrase_type_selection_statement){
      selection_statement *phrase_selection_statement=phrase_statement->child.phrase_selection_statement;
      if(walk_tree_and_create_str_table(phrase_selection_statement->phrase_statement_sequence,NULL,string_table,e_string_table,curr)){
        return 1;
      }
      if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
        if(walk_tree_and_create_str_table(phrase_selection_statement->child.phrase_statement_sequence,NULL,string_table,e_string_table,curr)){
          return 1;
        }
      }
      else{
        if(walk_tree_and_create_str_table(NULL,phrase_selection_statement->child.phrase_statement,string_table,e_string_table,curr)){
          return 1;
        }
      }
      if(phrase_selection_statement->phrase_else_clause!=NULL){
        if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
          if(walk_tree_and_create_str_table(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,string_table,e_string_table,curr)){
            return 1;
          }
        }
        else{
          if(walk_tree_and_create_str_table(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,string_table,e_string_table,curr)){
            return 1;
          }
        }
      }
    }
    else{
      if(add_to_str_table_if_valid(phrase_statement,string_table,e_string_table,curr)){
        return 1;
      }
    }
  }
  else{
    while(phrase_statement_sequence!=NULL){
      if(phrase_statement_sequence->phrase_statement->child_type==phrase_type_selection_statement){
        selection_statement *phrase_selection_statement=phrase_statement_sequence->phrase_statement->child.phrase_selection_statement;
        if(walk_tree_and_create_str_table(phrase_selection_statement->phrase_statement_sequence,NULL,string_table,e_string_table,curr)){
          return 1;
        }
        if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
          if(walk_tree_and_create_str_table(phrase_selection_statement->child.phrase_statement_sequence,NULL,string_table,e_string_table,curr)){
            return 1;
          }
        }
        else{
          if(walk_tree_and_create_str_table(NULL,phrase_selection_statement->child.phrase_statement,string_table,e_string_table,curr)){
            return 1;
          }
        }
        if(phrase_selection_statement->phrase_else_clause!=NULL){
          if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
            if(walk_tree_and_create_str_table(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,string_table,e_string_table,curr)){
              return 1;
            }
          }
          else{
            if(walk_tree_and_create_str_table(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,string_table,e_string_table,curr)){
              return 1;
            }
          }
        }
      }
      else{
        if(add_to_str_table_if_valid(phrase_statement_sequence->phrase_statement,string_table,e_string_table,curr)){
          return 1;
        }
      }
      phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
    }
  }
  return 0;
}

str_table *create_str_table(str_table **last_string_table_node, str_table *e_string_table, translation_unit *tu, unsigned int *ret){
  *ret=1;
  str_table *string_table=*last_string_table_node;
  str_table *curr=string_table;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  if(walk_tree_and_create_str_table(phrase_statement_sequence,NULL,&string_table,e_string_table,&curr)){
    return string_table;
  }
  *ret=0;
  *last_string_table_node=curr;
  return string_table;
}

nightVM_l lookup_in_string_table(char *s, str_table *string_table){
  while(string_table!=NULL){
    if(cseqcmpstr(string_table->string,s)==0){
      return string_table->address;
    }
    string_table=string_table->next_node;
  }
  return -1;
}

nightVM_l lookup_in_symbol_table(char *s, sym_table *symbol_table, translation_unit *tu, bool stop_early, bool *found){
  if(found!=NULL){
    *found=false;
  }
  while(symbol_table!=NULL){
    if(stop_early && symbol_table->next_node==NULL){
      break;
    }
    if(strcmp(symbol_table->symbol_identifier,s)==0 && (symbol_table->tu==tu || symbol_table->visibility_code==visibility_exposed)){
      if(found!=NULL){
        *found=true;
      }
      return symbol_table->val;
    }
    symbol_table=symbol_table->next_node;
  }
  return 0;
}

static void add_libraries_init_to_address(libs *libraries, nightVM_l *address){
  while(libraries!=NULL){
    libraries->align_pad=0;
    if((*address+1)%ALIGNOF_L!=0){
      libraries->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
      *address+=libraries->align_pad;
    }
    libraries->refi_address=*address;
    if(comp_attr.pic){
      *address+=1+SIZEOF_L+1+1+1;
    }
    else{
      *address+=1+SIZEOF_L+1;
    }
    libraries=libraries->next;
  }
}

static void add_libraries_to_address(libs *libraries, nightVM_l *address){
  while(libraries!=NULL){
    libraries->address=*address;
    *address+=cseqlen(libraries->lib_name)+1;
    libraries=libraries->next;
  }
}

static void add_string_table_to_address(str_table *string_table, nightVM_l *address){
  while(string_table!=NULL){
    string_table->address=*address;
    *address+=cseqlen(string_table->string)+1;
    string_table=string_table->next_node;
  }
}

static unsigned int add_to_sym_table_if_valid(statement *phrase_statement, sym_table **symbol_table, sym_table **e_symbol_table, nightVM_l *address, str_table *string_table, libs *libraries, sym_table **curr, translation_unit *tu){
  if(*address>0x3fffffffffffffff){
    write_error("implementation error: executable code is so large that parts of it are unaddressable\n",phrase_statement->start_x,phrase_statement->start_y,phrase_statement->end_x,phrase_statement->end_y,phrase_statement->file);
    return 1;
  }
  if(phrase_statement->child_type==phrase_type_label){
    bool found_symbol;
    lookup_in_symbol_table(phrase_statement->child.phrase_label->lex_identifier,*e_symbol_table,tu,false,&found_symbol);
    if(found_symbol){
      write_error("error: symbol already set\n",phrase_statement->child.phrase_label->start_x,phrase_statement->child.phrase_label->start_y,phrase_statement->child.phrase_label->end_x,phrase_statement->child.phrase_label->end_y,phrase_statement->child.phrase_label->file);
      return 1;
    }
    if(*symbol_table==NULL){
      if((*symbol_table=malloc(sizeof(sym_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      (*symbol_table)->next_node=NULL;
      *e_symbol_table=*symbol_table;
      *curr=*symbol_table;
    }
    else{
      if(((*curr)->next_node=malloc(sizeof(sym_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      *curr=(*curr)->next_node;
      (*curr)->next_node=NULL;
    }
    (*curr)->val=*address;
    if(strcmp(phrase_statement->child.phrase_label->lex_identifier,"main")==0){
      add_libraries_init_to_address(libraries,address);
    }
    if(phrase_statement->child.phrase_label->phrase_visibility!=NULL){
      (*curr)->visibility_code=phrase_statement->child.phrase_label->phrase_visibility->visibility_code;
    }
    else{
      (*curr)->visibility_code=visibility_exposed;
    }
    (*curr)->tu=tu;
    (*curr)->symbol_identifier=phrase_statement->child.phrase_label->lex_identifier;
  }
  else if(phrase_statement->child_type==phrase_type_set_statement){
    bool found_symbol;
    lookup_in_symbol_table(phrase_statement->child.phrase_set_statement->lex_identifier,*e_symbol_table,tu,false,&found_symbol);
    if(found_symbol){
      write_error("error: symbol already set\n",phrase_statement->child.phrase_set_statement->start_x,phrase_statement->child.phrase_set_statement->start_y,phrase_statement->child.phrase_set_statement->end_x,phrase_statement->child.phrase_set_statement->end_y,phrase_statement->child.phrase_set_statement->file);
      return 1;
    }
    if(*symbol_table==NULL){
      if((*symbol_table=malloc(sizeof(sym_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      (*symbol_table)->next_node=NULL;
      *e_symbol_table=*symbol_table;
      *curr=*symbol_table;
    }
    else{
      if(((*curr)->next_node=malloc(sizeof(sym_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      *curr=(*curr)->next_node;
      (*curr)->next_node=NULL;
    }
    if(phrase_statement->child.phrase_set_statement->child_type==lex_type_numeric_constant){
      (*curr)->val=numeric_constant_to_l(phrase_statement->child.phrase_set_statement->child.lex_numeric_constant);
      if(phrase_statement->child.phrase_set_statement->phrase_sign!=NULL){
        (*curr)->val*=phrase_statement->child.phrase_set_statement->phrase_sign->sign_code;
      }
    }
    else if(phrase_statement->child.phrase_set_statement->child_type==lex_type_character_constant){
      (*curr)->val=phrase_statement->child.phrase_set_statement->child.lex_character_constant[0];
      if(phrase_statement->child.phrase_set_statement->phrase_sign!=NULL){
        (*curr)->val*=phrase_statement->child.phrase_set_statement->phrase_sign->sign_code;
      }
    }
    else if(phrase_statement->child.phrase_set_statement->child_type==lex_type_string_constant){
      (*curr)->val=lookup_in_string_table(phrase_statement->child.phrase_set_statement->child.lex_string_constant,string_table);
      if(phrase_statement->child.phrase_set_statement->phrase_sign!=NULL){
        (*curr)->val*=phrase_statement->child.phrase_set_statement->phrase_sign->sign_code;
      }
    }
    else if(phrase_statement->child.phrase_set_statement->child_type==phrase_type_symbol){
      bool found_symbol;
      (*curr)->val=lookup_in_symbol_table(phrase_statement->child.phrase_set_statement->child.phrase_symbol->lex_identifier,*e_symbol_table,tu,true,&found_symbol);
      if(phrase_statement->child.phrase_set_statement->phrase_sign!=NULL){
        (*curr)->val*=phrase_statement->child.phrase_set_statement->phrase_sign->sign_code;
      }
      if(!found_symbol){
        write_error("error: use of unset symbol\n",phrase_statement->child.phrase_set_statement->start_x,phrase_statement->child.phrase_set_statement->start_y,phrase_statement->child.phrase_set_statement->end_x,phrase_statement->child.phrase_set_statement->end_y,phrase_statement->child.phrase_set_statement->file);
      }
    }
    (*curr)->visibility_code=visibility_hidden;
    (*curr)->tu=tu;
    (*curr)->symbol_identifier=phrase_statement->child.phrase_set_statement->lex_identifier;
  }
  else if(phrase_statement->child_type==phrase_type_sizeof_statement){
    phrase_statement->child.phrase_sizeof_statement->align_pad=0;
    if((*address+1)%ALIGNOF_L!=0){
      phrase_statement->child.phrase_sizeof_statement->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
      *address+=phrase_statement->child.phrase_sizeof_statement->align_pad;
    }
    *address+=1+SIZEOF_L;
  }
  else if(phrase_statement->child_type==phrase_type_alignof_statement){
    phrase_statement->child.phrase_alignof_statement->align_pad=0;
    if((*address+1)%ALIGNOF_L!=0){
      phrase_statement->child.phrase_alignof_statement->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
      *address+=phrase_statement->child.phrase_alignof_statement->align_pad;
    }
    *address+=1+SIZEOF_L;
  }
  else if(phrase_statement->child_type==phrase_type_push_statement){
    phrase_statement->child.phrase_push_statement->address=*address;
    phrase_statement->child.phrase_push_statement->align_pad=0;
    if(phrase_statement->child.phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushuc){
      *address+=1+1;
    }
    else if(phrase_statement->child.phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushus){
      if((*address+1)%ALIGNOF_US!=0){
        phrase_statement->child.phrase_push_statement->align_pad=((*address+1)+ALIGNOF_US-1)-(((*address+1)+ALIGNOF_US-1)%ALIGNOF_US)-(*address+1);
        *address+=phrase_statement->child.phrase_push_statement->align_pad;
        phrase_statement->child.phrase_push_statement->address+=phrase_statement->child.phrase_push_statement->align_pad;
      }
      *address+=1+SIZEOF_US;
    }
    else if(phrase_statement->child.phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushui){
      if((*address+1)%ALIGNOF_UI!=0){
        phrase_statement->child.phrase_push_statement->align_pad=((*address+1)+ALIGNOF_UI-1)-(((*address+1)+ALIGNOF_UI-1)%ALIGNOF_UI)-(*address+1);
        *address+=phrase_statement->child.phrase_push_statement->align_pad;
        phrase_statement->child.phrase_push_statement->address+=phrase_statement->child.phrase_push_statement->align_pad;
      }
      *address+=1+SIZEOF_UI;
    }
    else if(phrase_statement->child.phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushc){
      *address+=1+1;
    }
    else if(phrase_statement->child.phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushs){
      if((*address+1)%ALIGNOF_S!=0){
        phrase_statement->child.phrase_push_statement->align_pad=((*address+1)+ALIGNOF_S-1)-(((*address+1)+ALIGNOF_S-1)%ALIGNOF_S)-(*address+1);
        *address+=phrase_statement->child.phrase_push_statement->align_pad;
        phrase_statement->child.phrase_push_statement->address+=phrase_statement->child.phrase_push_statement->align_pad;
      }
      *address+=1+SIZEOF_S;
    }
    else if(phrase_statement->child.phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushi){
      if((*address+1)%ALIGNOF_I!=0){
        phrase_statement->child.phrase_push_statement->align_pad=((*address+1)+ALIGNOF_I-1)-(((*address+1)+ALIGNOF_I-1)%ALIGNOF_I)-(*address+1);
        *address+=phrase_statement->child.phrase_push_statement->align_pad;
        phrase_statement->child.phrase_push_statement->address+=phrase_statement->child.phrase_push_statement->align_pad;
      }
      *address+=1+SIZEOF_I;
    }
    else if(phrase_statement->child.phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushl){
      if((*address+1)%ALIGNOF_L!=0){
        phrase_statement->child.phrase_push_statement->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
        *address+=phrase_statement->child.phrase_push_statement->align_pad;
        phrase_statement->child.phrase_push_statement->address+=phrase_statement->child.phrase_push_statement->align_pad;
      }
      *address+=1+SIZEOF_L;
    }
    else if(phrase_statement->child.phrase_push_statement->phrase_push_instruction->push_instruction_code==key_pushp){
      if((*address+1)%ALIGNOF_L!=0){
        phrase_statement->child.phrase_push_statement->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
        *address+=phrase_statement->child.phrase_push_statement->align_pad;
        phrase_statement->child.phrase_push_statement->address+=phrase_statement->child.phrase_push_statement->align_pad;
      }
      if(comp_attr.pic && (phrase_statement->child.phrase_push_statement->child_type==lex_type_string_constant || phrase_statement->child.phrase_push_statement->child_type==phrase_type_symbol)){
        *address+=1+SIZEOF_L+1+1;
      }
      else{
        *address+=1+SIZEOF_L;
      }
    }
  }
  else if(phrase_statement->child_type==phrase_type_tagged_statement){
    phrase_statement->child.phrase_tagged_statement->align_pad=0;
    if((*address+1)%ALIGNOF_L!=0){
      phrase_statement->child.phrase_tagged_statement->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
      *address+=phrase_statement->child.phrase_tagged_statement->align_pad;
    }
    *address+=1+SIZEOF_L;
  }
  else if(phrase_statement->child_type==phrase_type_instruction){
    phrase_statement->child.phrase_instruction->align_pad=0;
    if(phrase_statement->child.phrase_instruction->instruction_code==key_addp){
      if((*address+2)%ALIGNOF_L!=0){
        phrase_statement->child.phrase_instruction->align_pad=((*address+2)+ALIGNOF_L-1)-(((*address+2)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+2);
        *address+=phrase_statement->child.phrase_instruction->align_pad;
      }
      *address+=1+1+SIZEOF_L+1+6+1+SIZEOF_L+1+1+1+1+1+1+1+1+SIZEOF_L+1+1+5+1+SIZEOF_L+1;
    }
    else if(phrase_statement->child.phrase_instruction->instruction_code==key_subp){
      if((*address+2)%ALIGNOF_L!=0){
        phrase_statement->child.phrase_instruction->align_pad=((*address+2)+ALIGNOF_L-1)-(((*address+2)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+2);
        *address+=phrase_statement->child.phrase_instruction->align_pad;
      }
      *address+=1+1+SIZEOF_L+1+6+1+SIZEOF_L+1+1+1+1+1+1+1+1+SIZEOF_L+1+1+5+1+SIZEOF_L+1;
    }
    else if(phrase_statement->child.phrase_instruction->instruction_code==key_loadc || phrase_statement->child.phrase_instruction->instruction_code==key_loads || phrase_statement->child.phrase_instruction->instruction_code==key_loadi || phrase_statement->child.phrase_instruction->instruction_code==key_loadp || phrase_statement->child.phrase_instruction->instruction_code==key_loadl || phrase_statement->child.phrase_instruction->instruction_code==key_loaduc || phrase_statement->child.phrase_instruction->instruction_code==key_loadus || phrase_statement->child.phrase_instruction->instruction_code==key_loadui || phrase_statement->child.phrase_instruction->instruction_code==key_vloadc || phrase_statement->child.phrase_instruction->instruction_code==key_vloads || phrase_statement->child.phrase_instruction->instruction_code==key_vloadi || phrase_statement->child.phrase_instruction->instruction_code==key_vloadp || phrase_statement->child.phrase_instruction->instruction_code==key_vloadl || phrase_statement->child.phrase_instruction->instruction_code==key_vloaduc || phrase_statement->child.phrase_instruction->instruction_code==key_vloadus || phrase_statement->child.phrase_instruction->instruction_code==key_vloadui){
      if((*address+2)%ALIGNOF_L!=0){
        phrase_statement->child.phrase_instruction->align_pad=((*address+2)+ALIGNOF_L-1)-(((*address+2)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+2);
        *address+=phrase_statement->child.phrase_instruction->align_pad;
      }
      *address+=1+1+SIZEOF_L+1+6+1+SIZEOF_L+1+1+1+1+1+1+1+1+SIZEOF_L+1+1;
    }
    else if(phrase_statement->child.phrase_instruction->instruction_code==key_aloadc || phrase_statement->child.phrase_instruction->instruction_code==key_aloads || phrase_statement->child.phrase_instruction->instruction_code==key_aloadi || phrase_statement->child.phrase_instruction->instruction_code==key_aloadp || phrase_statement->child.phrase_instruction->instruction_code==key_aloadl || phrase_statement->child.phrase_instruction->instruction_code==key_aloaduc || phrase_statement->child.phrase_instruction->instruction_code==key_aloadus || phrase_statement->child.phrase_instruction->instruction_code==key_aloadui || phrase_statement->child.phrase_instruction->instruction_code==key_valoadc || phrase_statement->child.phrase_instruction->instruction_code==key_valoads || phrase_statement->child.phrase_instruction->instruction_code==key_valoadi || phrase_statement->child.phrase_instruction->instruction_code==key_valoadp || phrase_statement->child.phrase_instruction->instruction_code==key_valoadl || phrase_statement->child.phrase_instruction->instruction_code==key_valoaduc || phrase_statement->child.phrase_instruction->instruction_code==key_valoadus || phrase_statement->child.phrase_instruction->instruction_code==key_valoadui){
      if((*address+2)%ALIGNOF_L!=0){
        phrase_statement->child.phrase_instruction->align_pad=((*address+2)+ALIGNOF_L-1)-(((*address+2)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+2);
        *address+=phrase_statement->child.phrase_instruction->align_pad;
      }
      *address+=1+1+SIZEOF_L+1+6+1+SIZEOF_L+1+1+1+1+1+1+1+1+SIZEOF_L+1+1;
    }
    else if(phrase_statement->child.phrase_instruction->instruction_code==key_storec || phrase_statement->child.phrase_instruction->instruction_code==key_stores || phrase_statement->child.phrase_instruction->instruction_code==key_storei || phrase_statement->child.phrase_instruction->instruction_code==key_storep || phrase_statement->child.phrase_instruction->instruction_code==key_storel || phrase_statement->child.phrase_instruction->instruction_code==key_storeuc || phrase_statement->child.phrase_instruction->instruction_code==key_storeus || phrase_statement->child.phrase_instruction->instruction_code==key_storeui || phrase_statement->child.phrase_instruction->instruction_code==key_vstorec || phrase_statement->child.phrase_instruction->instruction_code==key_vstores || phrase_statement->child.phrase_instruction->instruction_code==key_vstorei || phrase_statement->child.phrase_instruction->instruction_code==key_vstorep || phrase_statement->child.phrase_instruction->instruction_code==key_vstorel || phrase_statement->child.phrase_instruction->instruction_code==key_vstoreuc || phrase_statement->child.phrase_instruction->instruction_code==key_vstoreus || phrase_statement->child.phrase_instruction->instruction_code==key_vstoreui){
      if((*address+2)%ALIGNOF_L!=0){
        phrase_statement->child.phrase_instruction->align_pad=((*address+2)+ALIGNOF_L-1)-(((*address+2)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+2);
        *address+=phrase_statement->child.phrase_instruction->align_pad;
      }
      *address+=1+1+SIZEOF_L+1+6+1+SIZEOF_L+1+1+1+1+1+1+1+1+SIZEOF_L+1+1;
    }
    else if(phrase_statement->child.phrase_instruction->instruction_code==key_astorec || phrase_statement->child.phrase_instruction->instruction_code==key_astores || phrase_statement->child.phrase_instruction->instruction_code==key_astorei || phrase_statement->child.phrase_instruction->instruction_code==key_astorep || phrase_statement->child.phrase_instruction->instruction_code==key_astorel || phrase_statement->child.phrase_instruction->instruction_code==key_astoreuc || phrase_statement->child.phrase_instruction->instruction_code==key_astoreus || phrase_statement->child.phrase_instruction->instruction_code==key_astoreui || phrase_statement->child.phrase_instruction->instruction_code==key_vastorec || phrase_statement->child.phrase_instruction->instruction_code==key_vastores || phrase_statement->child.phrase_instruction->instruction_code==key_vastorei || phrase_statement->child.phrase_instruction->instruction_code==key_vastorep || phrase_statement->child.phrase_instruction->instruction_code==key_vastorel || phrase_statement->child.phrase_instruction->instruction_code==key_vastoreuc || phrase_statement->child.phrase_instruction->instruction_code==key_vastoreus || phrase_statement->child.phrase_instruction->instruction_code==key_vastoreui){
      if((*address+2)%ALIGNOF_L!=0){
        phrase_statement->child.phrase_instruction->align_pad=((*address+2)+ALIGNOF_L-1)-(((*address+2)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+2);
        *address+=phrase_statement->child.phrase_instruction->align_pad;
      }
      *address+=1+1+SIZEOF_L+1+6+1+SIZEOF_L+1+1+1+1+1+1+1+1+SIZEOF_L+1+1;
    }
    else{
      (*address)++;
    }
  }
  return 0;
}

static unsigned int walk_tree_and_create_sym_table(statement_sequence *phrase_statement_sequence, statement *phrase_statement, sym_table **symbol_table, sym_table **e_symbol_table, nightVM_l *address, str_table *string_table, libs *libraries, sym_table **curr, translation_unit *tu){
  if(phrase_statement!=NULL){
    if(phrase_statement->child_type==phrase_type_selection_statement){
      selection_statement *phrase_selection_statement=phrase_statement->child.phrase_selection_statement;
      if(walk_tree_and_create_sym_table(phrase_selection_statement->phrase_statement_sequence,NULL,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
        return 1;
      }
      if((*address+1)%ALIGNOF_L!=0){
        phrase_selection_statement->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
        *address+=phrase_selection_statement->align_pad;
      }
      *address+=1+SIZEOF_L+1;
      nightVM_l staddr=*address;
      if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
        if(walk_tree_and_create_sym_table(phrase_selection_statement->child.phrase_statement_sequence,NULL,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
          return 1;
        }
      }
      else{
        if(walk_tree_and_create_sym_table(NULL,phrase_selection_statement->child.phrase_statement,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
          return 1;
        }
      }
      phrase_selection_statement->rend_address=*address+1-staddr;
      if(phrase_selection_statement->phrase_else_clause!=NULL){
        if((*address+1)%ALIGNOF_L!=0){
          phrase_selection_statement->phrase_else_clause->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
          *address+=phrase_selection_statement->phrase_else_clause->align_pad;
        }
        *address+=1+SIZEOF_L+1;
        phrase_selection_statement->rend_address=*address+1-staddr;
        staddr=*address;
        if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
          if(walk_tree_and_create_sym_table(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
            return 1;
          }
        }
        else{
          if(walk_tree_and_create_sym_table(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
            return 1;
          }
        }
        phrase_selection_statement->phrase_else_clause->rend_address=*address+1-staddr;
      }
    }
    else{
      if(add_to_sym_table_if_valid(phrase_statement,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
        return 1;
      }
    }
  }
  else{
    while(phrase_statement_sequence!=NULL){
      if(phrase_statement_sequence->phrase_statement->child_type==phrase_type_selection_statement){
        selection_statement *phrase_selection_statement=phrase_statement_sequence->phrase_statement->child.phrase_selection_statement;
        if(walk_tree_and_create_sym_table(phrase_selection_statement->phrase_statement_sequence,NULL,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
          return 1;
        }
        if((*address+1)%ALIGNOF_L!=0){
          phrase_selection_statement->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
          *address+=phrase_selection_statement->align_pad;
        }
        *address+=1+SIZEOF_L+1;
        nightVM_l staddr=*address;
        if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
          if(walk_tree_and_create_sym_table(phrase_selection_statement->child.phrase_statement_sequence,NULL,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
            return 1;
          }
        }
        else{
          if(walk_tree_and_create_sym_table(NULL,phrase_selection_statement->child.phrase_statement,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
            return 1;
          }
        }
        phrase_selection_statement->rend_address=*address+1-staddr;
        if(phrase_selection_statement->phrase_else_clause!=NULL){
          if((*address+1)%ALIGNOF_L!=0){
            phrase_selection_statement->phrase_else_clause->align_pad=((*address+1)+ALIGNOF_L-1)-(((*address+1)+ALIGNOF_L-1)%ALIGNOF_L)-(*address+1);
            *address+=phrase_selection_statement->phrase_else_clause->align_pad;
          }
          *address+=1+SIZEOF_L+1;
          phrase_selection_statement->rend_address=*address+1-staddr;
          staddr=*address;
          if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
            if(walk_tree_and_create_sym_table(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
              return 1;
            }
          }
          else{
            if(walk_tree_and_create_sym_table(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
              return 1;
            }
          }
          phrase_selection_statement->phrase_else_clause->rend_address=*address+1-staddr;
        }
      }
      else{
        if(add_to_sym_table_if_valid(phrase_statement_sequence->phrase_statement,symbol_table,e_symbol_table,address,string_table,libraries,curr,tu)){
          return 1;
        }
      }
      phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
    }
  }
  return 0;
}

sym_table *create_sym_table(sym_table **last_symbol_table_node, sym_table *e_symbol_table, translation_unit *tu, str_table *string_table, libs *libraries, nightVM_l *address, unsigned int *ret){
  *ret=1;
  add_string_table_to_address(string_table,address);
  add_libraries_to_address(libraries,address);
  sym_table *symbol_table=*last_symbol_table_node;
  sym_table *curr=symbol_table;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  if(walk_tree_and_create_sym_table(phrase_statement_sequence,NULL,&symbol_table,&e_symbol_table,address,string_table,libraries,&curr,tu)){
    return symbol_table;
  }
  *ret=0;
  *last_symbol_table_node=curr;
  return symbol_table;
}

nightVM_l lookup_in_struct_definition_table(char *s, struct_definition_table *struct_table, translation_unit *tu, bool stop_early, struct_definition **phrase_struct_definition, unsigned int ret_type){
  while(struct_table!=NULL){
    if(stop_early && struct_table->next_node==NULL){
      break;
    }
    if(strcmp(struct_table->struct_name_identifier,s)==0 && struct_table->tu==tu){
      if(phrase_struct_definition!=NULL){
        *phrase_struct_definition=struct_table->address;
      }
      if(ret_type==ret_size){
        return struct_table->size;
      }
      else{
        return struct_table->alignment;
      }
    }
    struct_table=struct_table->next_node;
  }
  if(phrase_struct_definition!=NULL){
    *phrase_struct_definition=NULL;
  }
  return 0;
}

static nightVM_l get_size_of_type(translation_unit *tu, struct_tag_definition_sequence *phrase_struct_tag_definition_sequence, struct_definition_table *struct_table, unsigned int *ret){
  *ret=1;
  if(phrase_struct_tag_definition_sequence==NULL){
    return 0;
  }
  nightVM_l size=0;
  while(phrase_struct_tag_definition_sequence!=NULL){
    struct_tag_definition *phrase_struct_tag_definition=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition;
    if(phrase_struct_tag_definition->child_type==phrase_type_type){
      if(phrase_struct_tag_definition->child.phrase_type->type_code==key_uc){
        size+=1;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_us){
        size+=SIZEOF_US;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_ui){
        size+=SIZEOF_UI;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_c){
        size+=1;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_s){
        size+=SIZEOF_S;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_i){
        size+=SIZEOF_I;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_l){
        size+=SIZEOF_L;
      }
      else if(phrase_struct_tag_definition->child.phrase_type->type_code==key_p){
        size+=SIZEOF_L;
      }
    }
    else if(phrase_struct_tag_definition->child_type==phrase_type_struct_name){
      struct_definition *ret;
      size+=lookup_in_struct_definition_table(phrase_struct_tag_definition->child.phrase_struct_name->lex_identifier,struct_table,tu,true,&ret,ret_size);
      if(ret==NULL){
        write_error("error: no definition of such found\n",phrase_struct_tag_definition->start_x,phrase_struct_tag_definition->start_y,phrase_struct_tag_definition->end_x,phrase_struct_tag_definition->end_y,phrase_struct_tag_definition->file);
        return 0;
      }
    }
    phrase_struct_tag_definition_sequence=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence;
  }
  *ret=0;
  return size;
}

static unsigned int add_to_struct_definition_table_if_valid(statement *phrase_statement, struct_definition_table **struct_table, struct_definition_table **e_struct_table, struct_definition_table **curr, translation_unit *tu){
  if(phrase_statement->child_type==phrase_type_struct_definition){
    struct_definition *ret_sd;
    lookup_in_struct_definition_table(phrase_statement->child.phrase_struct_definition->lex_identifier,*e_struct_table,tu,false,&ret_sd,ret_any);
    if(ret_sd!=NULL){
      write_error("error: struct-name already in use\n",phrase_statement->child.phrase_struct_definition->start_x,phrase_statement->child.phrase_struct_definition->start_y,phrase_statement->child.phrase_struct_definition->end_x,phrase_statement->child.phrase_struct_definition->end_y,phrase_statement->child.phrase_struct_definition->file);
      return 1;
    }
    if(*struct_table==NULL){
      if((*struct_table=malloc(sizeof(struct_definition_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      (*struct_table)->next_node=NULL;
      *e_struct_table=*struct_table;
      *curr=*struct_table;
    }
    else{
      if(((*curr)->next_node=malloc(sizeof(struct_definition_table)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      *curr=(*curr)->next_node;
      (*curr)->next_node=NULL;
    }
    (*curr)->struct_name_identifier=phrase_statement->child.phrase_struct_definition->lex_identifier;
    (*curr)->tu=tu;
    unsigned int ret;
    (*curr)->size=get_size_of_type(phrase_statement->tu,phrase_statement->child.phrase_struct_definition->phrase_struct_tag_definition_sequence,*e_struct_table,&ret);
    (*curr)->alignment=1;
    if(ret==1){
      return 1;
    }
    (*curr)->address=phrase_statement->child.phrase_struct_definition;
  }
  return 0;
}

static unsigned int walk_tree_and_create_struct_definition_table(statement_sequence *phrase_statement_sequence, statement *phrase_statement, struct_definition_table **struct_table, struct_definition_table **e_struct_table, struct_definition_table **curr, translation_unit *tu){
  if(phrase_statement!=NULL){
    if(phrase_statement->child_type==phrase_type_selection_statement){
      selection_statement *phrase_selection_statement=phrase_statement->child.phrase_selection_statement;
      if(walk_tree_and_create_struct_definition_table(phrase_selection_statement->phrase_statement_sequence,NULL,struct_table,e_struct_table,curr,tu)){
        return 1;
      }
      if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
        if(walk_tree_and_create_struct_definition_table(phrase_selection_statement->child.phrase_statement_sequence,NULL,struct_table,e_struct_table,curr,tu)){
          return 1;
        }
      }
      else{
        if(walk_tree_and_create_struct_definition_table(NULL,phrase_selection_statement->child.phrase_statement,struct_table,e_struct_table,curr,tu)){
          return 1;
        }
      }
      if(phrase_selection_statement->phrase_else_clause!=NULL){
        if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
          if(walk_tree_and_create_struct_definition_table(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,struct_table,e_struct_table,curr,tu)){
            return 1;
          }
        }
        else{
          if(walk_tree_and_create_struct_definition_table(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,struct_table,e_struct_table,curr,tu)){
            return 1;
          }
        }
      }
    }
    else{
      if(add_to_struct_definition_table_if_valid(phrase_statement,struct_table,e_struct_table,curr,tu)){
        return 1;
      }
    }
  }
  else{
    while(phrase_statement_sequence!=NULL){
      if(phrase_statement_sequence->phrase_statement->child_type==phrase_type_selection_statement){
        selection_statement *phrase_selection_statement=phrase_statement_sequence->phrase_statement->child.phrase_selection_statement;
        if(walk_tree_and_create_struct_definition_table(phrase_selection_statement->phrase_statement_sequence,NULL,struct_table,e_struct_table,curr,tu)){
          return 1;
        }
        if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
          if(walk_tree_and_create_struct_definition_table(phrase_selection_statement->child.phrase_statement_sequence,NULL,struct_table,e_struct_table,curr,tu)){
            return 1;
          }
        }
        else{
          if(walk_tree_and_create_struct_definition_table(NULL,phrase_selection_statement->child.phrase_statement,struct_table,e_struct_table,curr,tu)){
            return 1;
          }
        }
        if(phrase_selection_statement->phrase_else_clause!=NULL){
          if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
            if(walk_tree_and_create_struct_definition_table(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,struct_table,e_struct_table,curr,tu)){
              return 1;
            }
          }
          else{
            if(walk_tree_and_create_struct_definition_table(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,struct_table,e_struct_table,curr,tu)){
              return 1;
            }
          }
        }
      }
      else{
        if(add_to_struct_definition_table_if_valid(phrase_statement_sequence->phrase_statement,struct_table,e_struct_table,curr,tu)){
          return 1;
        }
      }
      phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
    }
  }
  return 0;
}

struct_definition_table* create_struct_definition_table(struct_definition_table **last_struct_table_node, struct_definition_table *e_struct_table, translation_unit *tu, unsigned int *ret){
  *ret=1;
  struct_definition_table *struct_table=*last_struct_table_node;
  struct_definition_table *curr=struct_table;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  if(walk_tree_and_create_struct_definition_table(phrase_statement_sequence,NULL,&struct_table,&e_struct_table,&curr,tu)){
    return struct_table;
  }
  *ret=0;
  *last_struct_table_node=curr;
  return struct_table;
}
