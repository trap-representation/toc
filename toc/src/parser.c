#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "parser.h"
#include "write_error.h"
#define TOC_ADD_VISIBILITY
#define TOC_ADD_KEYWORD
#define TOC_ADD_SIGN
#define TOC_ADD_PUNCTUATOR
#include "psg_idens.h"

#define can_be_leftmost_key_in_statement (check_type(*curr,key_pushc) || check_type(*curr,key_pushs) || check_type(*curr,key_pushi) || check_type(*curr,key_pushp) || check_type(*curr,key_pushl) || check_type(*curr,key_pushuc) || check_type(*curr,key_pushus) || check_type(*curr,key_pushui) || check_type(*curr,key_eq) || check_type(*curr,key_gt) || check_type(*curr,key_ls) || check_type(*curr,key_ge) || check_type(*curr,key_le) || check_type(*curr,key_ne) || check_type(*curr,key_zr) || check_type(*curr,key_nz) || check_type(*curr,key_addc) || check_type(*curr,key_adds) || check_type(*curr,key_addi) || check_type(*curr,key_addp) || check_type(*curr,key_addl) || check_type(*curr,key_adduc) || check_type(*curr,key_addus) || check_type(*curr,key_addui) || check_type(*curr,key_subc) || check_type(*curr,key_subs) || check_type(*curr,key_subi) || check_type(*curr,key_subp) || check_type(*curr,key_subl) || check_type(*curr,key_subuc) || check_type(*curr,key_subus) || check_type(*curr,key_subui) || check_type(*curr,key_mulc) || check_type(*curr,key_muls) || check_type(*curr,key_muli) || check_type(*curr,key_mull) || check_type(*curr,key_muluc) || check_type(*curr,key_mulus) || check_type(*curr,key_mului) || check_type(*curr,key_divc) || check_type(*curr,key_divs) || check_type(*curr,key_divi) || check_type(*curr,key_divl) || check_type(*curr,key_divuc) || check_type(*curr,key_divus) || check_type(*curr,key_divui) || check_type(*curr,key_remc) || check_type(*curr,key_rems) || check_type(*curr,key_remi) || check_type(*curr,key_reml) || check_type(*curr,key_remuc) || check_type(*curr,key_remus) || check_type(*curr,key_remui) || check_type(*curr,key_lshc) || check_type(*curr,key_lshs) || check_type(*curr,key_lshi) || check_type(*curr,key_lshl) || check_type(*curr,key_lshuc) || check_type(*curr,key_lshus) || check_type(*curr,key_lshui) || check_type(*curr,key_rshc) || check_type(*curr,key_rshs) || check_type(*curr,key_rshi) || check_type(*curr,key_rshl) || check_type(*curr,key_rshuc) || check_type(*curr,key_rshus) || check_type(*curr,key_rshui) || check_type(*curr,key_orc) || check_type(*curr,key_ors) || check_type(*curr,key_ori) || check_type(*curr,key_orl) || check_type(*curr,key_oruc) || check_type(*curr,key_orus) || check_type(*curr,key_orui) || check_type(*curr,key_andc) || check_type(*curr,key_ands) || check_type(*curr,key_andi) || check_type(*curr,key_andl) || check_type(*curr,key_anduc) || check_type(*curr,key_andus) || check_type(*curr,key_andui) || check_type(*curr,key_notc) || check_type(*curr,key_nots) || check_type(*curr,key_noti) || check_type(*curr,key_notl) || check_type(*curr,key_notuc) || check_type(*curr,key_notus) || check_type(*curr,key_notui) || check_type(*curr,key_xorc) || check_type(*curr,key_xors) || check_type(*curr,key_xori) || check_type(*curr,key_xorl) || check_type(*curr,key_xoruc) || check_type(*curr,key_xorus) || check_type(*curr,key_xorui) || check_type(*curr,key_swap) || check_type(*curr,key_pop) || check_type(*curr,key_dup) || check_type(*curr,key_ret) || check_type(*curr,key_over) || check_type(*curr,key_panic) || check_type(*curr,key_write0) || check_type(*curr,key_write1) || check_type(*curr,key_write2) || check_type(*curr,key_write3) || check_type(*curr,key_write4) || check_type(*curr,key_write5) || check_type(*curr,key_write6) || check_type(*curr,key_write7) || check_type(*curr,key_vwrite0) || check_type(*curr,key_vwrite1) || check_type(*curr,key_vwrite2) || check_type(*curr,key_vwrite3) || check_type(*curr,key_vwrite4) || check_type(*curr,key_vwrite5) || check_type(*curr,key_vwrite6) || check_type(*curr,key_vwrite7) || check_type(*curr,key_get0) || check_type(*curr,key_get1) || check_type(*curr,key_get2) || check_type(*curr,key_get3) || check_type(*curr,key_get4) || check_type(*curr,key_get5) || check_type(*curr,key_get6) || check_type(*curr,key_get7) || check_type(*curr,key_vget0) || check_type(*curr,key_vget1) || check_type(*curr,key_vget2) || check_type(*curr,key_vget3) || check_type(*curr,key_vget4) || check_type(*curr,key_vget5) || check_type(*curr,key_vget6) || check_type(*curr,key_vget7) || check_type(*curr,key_loadc) || check_type(*curr,key_loads) || check_type(*curr,key_loadi) || check_type(*curr,key_loadp) || check_type(*curr,key_loadl) || check_type(*curr,key_loaduc) || check_type(*curr,key_loadus) || check_type(*curr,key_loadui) || check_type(*curr,key_vloadc) || check_type(*curr,key_vloads) || check_type(*curr,key_vloadi) || check_type(*curr,key_vloadp) || check_type(*curr,key_vloadl) || check_type(*curr,key_vloaduc) || check_type(*curr,key_vloadus) || check_type(*curr,key_vloadui) || check_type(*curr,key_aloadc) || check_type(*curr,key_aloads) || check_type(*curr,key_aloadi) || check_type(*curr,key_aloadp) || check_type(*curr,key_aloadl) || check_type(*curr,key_aloaduc) || check_type(*curr,key_aloadus) || check_type(*curr,key_aloadui) || check_type(*curr,key_valoadc) || check_type(*curr,key_valoads) || check_type(*curr,key_valoadi) || check_type(*curr,key_valoadp) || check_type(*curr,key_valoadl) || check_type(*curr,key_valoaduc) || check_type(*curr,key_valoadus) || check_type(*curr,key_valoadui) || check_type(*curr,key_storec) || check_type(*curr,key_stores) || check_type(*curr,key_storei) || check_type(*curr,key_storep) || check_type(*curr,key_storel) || check_type(*curr,key_storeuc) || check_type(*curr,key_storeus) || check_type(*curr,key_storeui) || check_type(*curr,key_vstorec) || check_type(*curr,key_vstores) || check_type(*curr,key_vstorei) || check_type(*curr,key_vstorep) || check_type(*curr,key_vstorel) || check_type(*curr,key_vstoreuc) || check_type(*curr,key_vstoreus) || check_type(*curr,key_vstoreui) || check_type(*curr,key_astorec) || check_type(*curr,key_astores) || check_type(*curr,key_astorei) || check_type(*curr,key_astorep) || check_type(*curr,key_astorel) || check_type(*curr,key_astoreuc) || check_type(*curr,key_astoreus) || check_type(*curr,key_astoreui) || check_type(*curr,key_vastorec) || check_type(*curr,key_vastores) || check_type(*curr,key_vastorei) || check_type(*curr,key_vastorep) || check_type(*curr,key_vastorel) || check_type(*curr,key_vastoreuc) || check_type(*curr,key_vastoreus) || check_type(*curr,key_vastoreui) || check_type(*curr,key_hlt) || check_type(*curr,key_call) || check_type(*curr,key_copy) || check_type(*curr,key_pcopy) || check_type(*curr,key_popa) || check_type(*curr,key_put) || check_type(*curr,key_pushsp) || check_type(*curr,key_set) || check_type(*curr,key_hltr) || check_type(*curr,key_incsp) || check_type(*curr,key_decsp) || check_type(*curr,key_exit) || check_type(*curr,key_force_panic) || check_type(*curr,key_pushlt) || check_type(*curr,key_import) || check_type(*curr,key_hidden) || check_type(*curr,key_exposed) || check_type(*curr,key_struct) || check_type(*curr,key_uc) || check_type(*curr,key_us) || check_type(*curr,key_ui) || check_type(*curr,key_c) || check_type(*curr,key_s) || check_type(*curr,key_i) || check_type(*curr,key_l) || check_type(*curr,key_p) || check_type(*curr,key_alignof) || check_type(*curr,key_sizeof) || check_type(*curr,key_if) || check_type(*curr,key_rcall) || check_type(*curr,key_open) || check_type(*curr,key_invoke) || check_type(*curr,key_pushpc) || check_type(*curr,key_pushcs))

static void consume(token **curr){
  *curr=(*curr)->next_token;
}

static bool check(token *tk, unsigned int m){
  if(tk->token_class==m){
    return true;
  }
  return false;
}

static bool match_type(token **tk, unsigned int m){
  if((*tk)->token_type==m){
    consume(tk);
    return true;
  }
  return false;
}

static bool check_type(token *tk, unsigned int m){
  if(tk->token_type==m){
    return true;
  }
  return false;
}

unsigned int parse_instruction(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_instruction=malloc(sizeof(instruction)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  instruction *phrase_instruction=parent_phrase_statement->child.phrase_instruction;
  phrase_instruction->parent=parent_phrase_statement;
  phrase_instruction->file=file_name;
  phrase_instruction->start_x=(*curr)->start_x;
  phrase_instruction->start_y=(*curr)->start_y;
  phrase_instruction->end_x=(*curr)->end_x;
  phrase_instruction->end_y=(*curr)->end_y;
  phrase_instruction->instruction_code=(*curr)->token_type;
  parent_phrase_statement->child.phrase_instruction=phrase_instruction;
  consume(curr);
  return 0;
}

unsigned int parse_sign(push_statement *parent_phrase_push_statement, set_statement *parent_phrase_set_statement, token **curr, char *file_name){
  sign *phrase_sign;
  if(parent_phrase_push_statement!=NULL){
    if((parent_phrase_push_statement->phrase_sign=malloc(sizeof(sign)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_sign=parent_phrase_push_statement->phrase_sign;
    phrase_sign->parent_type=phrase_type_push_statement;
    phrase_sign->parent.phrase_push_statement=parent_phrase_push_statement;
  }
  else{
    if((parent_phrase_set_statement->phrase_sign=malloc(sizeof(sign)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_sign=parent_phrase_set_statement->phrase_sign;
    phrase_sign->parent_type=phrase_type_set_statement;
    phrase_sign->parent.phrase_set_statement=parent_phrase_set_statement;
  }
  phrase_sign->file=file_name;
  phrase_sign->start_x=(*curr)->start_x;
  phrase_sign->start_y=(*curr)->start_y;
  phrase_sign->end_x=(*curr)->end_x;
  phrase_sign->end_y=(*curr)->end_y;
  if(check_type(*curr,punc_minus)){
    phrase_sign->sign_code=sign_negative;
    consume(curr);
  }
  else{
    phrase_sign->sign_code=sign_positive;
    consume(curr);
  }
  return 0;
}

unsigned int parse_push_instruction(push_statement *parent_phrase_push_statement, token **curr, char *file_name){
  if((parent_phrase_push_statement->phrase_push_instruction=malloc(sizeof(push_instruction)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  push_instruction *phrase_push_instruction=parent_phrase_push_statement->phrase_push_instruction;
  phrase_push_instruction->file=file_name;
  phrase_push_instruction->parent=parent_phrase_push_statement;
  phrase_push_instruction->start_x=(*curr)->start_x;
  phrase_push_instruction->start_y=(*curr)->start_y;
  phrase_push_instruction->end_x=(*curr)->end_x;
  phrase_push_instruction->end_y=(*curr)->end_y;
  if(check_type(*curr,key_pushc)){
    phrase_push_instruction->push_instruction_code=key_pushc;
    consume(curr);
  }
  else if(check_type(*curr,key_pushs)){
    phrase_push_instruction->push_instruction_code=key_pushs;
    consume(curr);
  }
  else if(check_type(*curr,key_pushi)){
    phrase_push_instruction->push_instruction_code=key_pushi;
    consume(curr);
  }
  else if(check_type(*curr,key_pushl)){
    phrase_push_instruction->push_instruction_code=key_pushl;
    consume(curr);
  }
  else if(check_type(*curr,key_pushp)){
    phrase_push_instruction->push_instruction_code=key_pushp;
    consume(curr);
  }
  else if(check_type(*curr,key_pushuc)){
    phrase_push_instruction->push_instruction_code=key_pushuc;
    consume(curr);
  }
  else if(check_type(*curr,key_pushus)){
    phrase_push_instruction->push_instruction_code=key_pushus;
    consume(curr);
  }
  else if(check_type(*curr,key_pushui)){
    phrase_push_instruction->push_instruction_code=key_pushui;
    consume(curr);
  }
  return 0;
}

unsigned int parse_symbol(push_statement *parent_phrase_push_statement, set_statement *parent_phrase_set_statement, token **curr, char *file_name){
  symbol *phrase_symbol;
  if(parent_phrase_push_statement!=NULL){
    if((parent_phrase_push_statement->child.phrase_symbol=malloc(sizeof(symbol)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_symbol=parent_phrase_push_statement->child.phrase_symbol;
    phrase_symbol->parent_type=phrase_type_push_statement;
    phrase_symbol->parent.phrase_push_statement=parent_phrase_push_statement;
  }
  else{
    if((parent_phrase_set_statement->child.phrase_symbol=malloc(sizeof(symbol)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_symbol=parent_phrase_set_statement->child.phrase_symbol;
    phrase_symbol->parent_type=phrase_type_set_statement;
    phrase_symbol->parent.phrase_set_statement=parent_phrase_set_statement;
  }
  phrase_symbol->file=file_name;
  phrase_symbol->start_x=(*curr)->start_x;
  phrase_symbol->start_y=(*curr)->start_y;
  phrase_symbol->end_x=(*curr)->end_x;
  phrase_symbol->end_y=(*curr)->end_y;
  if((phrase_symbol->lex_identifier=malloc((*curr)->token_string_len+1))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  strncpy(phrase_symbol->lex_identifier,(*curr)->token_string,(*curr)->token_string_len);
  phrase_symbol->lex_identifier[(*curr)->token_string_len]='\0';
  consume(curr);
  return 0;
}

unsigned int parse_push_statement(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_push_statement=malloc(sizeof(push_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  push_statement *phrase_push_statement=parent_phrase_statement->child.phrase_push_statement;
  phrase_push_statement->file=parent_phrase_statement->file;
  phrase_push_statement->parent=parent_phrase_statement;
  phrase_push_statement->child_type=fill_incomplete;
  phrase_push_statement->phrase_sign=NULL;
  phrase_push_statement->start_x=(*curr)->start_x;
  phrase_push_statement->start_y=(*curr)->start_y;
  if(parse_push_instruction(phrase_push_statement,curr,file_name)){
    return 1;
  }
  bool has_sign=false;
  if(check(*curr,tk_punctuator) && (check_type(*curr,punc_minus) || check_type(*curr,punc_plus))){
    if(parse_sign(phrase_push_statement,NULL,curr,file_name)){
      return 1;
    }
    has_sign=true;
  }
  if(check(*curr,tk_identifier)){
    phrase_push_statement->child_type=phrase_type_symbol;
    if(parse_symbol(phrase_push_statement,NULL,curr,file_name)){
      return 1;
    }
  }
  else if(check(*curr,tk_string_constant)){
    phrase_push_statement->child_type=lex_type_string_constant;
    if((phrase_push_statement->child.lex_string_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    if((*curr)->token_string!=NULL){
      strncpy(phrase_push_statement->child.lex_string_constant,(*curr)->token_string,(*curr)->token_string_len);
      phrase_push_statement->child.lex_string_constant[(*curr)->token_string_len]='\0';
    }
    else{
      phrase_push_statement->child.lex_string_constant[0]='\0';
    }
    consume(curr);
  }
  else if(check(*curr,tk_character_constant)){
    phrase_push_statement->child_type=lex_type_character_constant;
    if((phrase_push_statement->child.lex_character_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    if((*curr)->token_string!=NULL){
      strncpy(phrase_push_statement->child.lex_character_constant,(*curr)->token_string,(*curr)->token_string_len);
      phrase_push_statement->child.lex_character_constant[(*curr)->token_string_len]='\0';
    }
    else{
      phrase_push_statement->child.lex_character_constant[0]='\0';
    }
    consume(curr);
  }
  else if(check(*curr,tk_numeric_constant)){
    phrase_push_statement->child_type=lex_type_numeric_constant;
    if((phrase_push_statement->child.lex_numeric_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    strncpy(phrase_push_statement->child.lex_numeric_constant,(*curr)->token_string,(*curr)->token_string_len);
    phrase_push_statement->child.lex_numeric_constant[(*curr)->token_string_len]='\0';
    consume(curr);
  }
  else{
    if(has_sign){
      write_error_from_tokens("error: expected a constant or a symbol\n",*curr,(*curr)->prev_token,file_name);
      return 1;
    }
    else{
      write_error_from_tokens("error: expected an optional sign followed by a constant or a symbol\n",*curr,(*curr)->prev_token,file_name);
      return 1;
    }
  }
  phrase_push_statement->end_x=(*curr)->prev_token->end_x;
  phrase_push_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_set_statement(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_set_statement=malloc(sizeof(set_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  set_statement *phrase_set_statement=parent_phrase_statement->child.phrase_set_statement;
  phrase_set_statement->file=parent_phrase_statement->file;
  phrase_set_statement->parent=parent_phrase_statement;
  phrase_set_statement->lex_identifier=NULL;
  phrase_set_statement->phrase_sign=NULL;
  phrase_set_statement->child_type=fill_incomplete;
  phrase_set_statement->start_x=(*curr)->start_x;
  phrase_set_statement->start_y=(*curr)->start_y;
  consume(curr);
  if(check(*curr,tk_identifier)){
    if((phrase_set_statement->lex_identifier=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    strncpy(phrase_set_statement->lex_identifier,(*curr)->token_string,(*curr)->token_string_len);
    phrase_set_statement->lex_identifier[(*curr)->token_string_len]='\0';
    consume(curr);
  }
  else{
    write_error_from_tokens("error: expected a symbol\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  bool has_sign=false;
  if(check(*curr,tk_punctuator) && (check_type(*curr,punc_minus) || check_type(*curr,punc_plus))){
    if(parse_sign(NULL,phrase_set_statement,curr,file_name)){
      return 1;
    }
    has_sign=true;
  }
  if(check(*curr,tk_identifier)){
    phrase_set_statement->child_type=phrase_type_symbol;
    if(parse_symbol(NULL,phrase_set_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check(*curr,tk_character_constant)){
    phrase_set_statement->child_type=lex_type_character_constant;
    if((phrase_set_statement->child.lex_character_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    if((*curr)->token_string!=NULL){
      strncpy(phrase_set_statement->child.lex_character_constant,(*curr)->token_string,(*curr)->token_string_len);
      phrase_set_statement->child.lex_character_constant[(*curr)->token_string_len]='\0';
    }
    else{
      phrase_set_statement->child.lex_character_constant[0]='\0';
    }
    consume(curr);
  }
  else if(check(*curr,tk_string_constant)){
    phrase_set_statement->child_type=lex_type_string_constant;
    if((phrase_set_statement->child.lex_string_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    if((*curr)->token_string!=NULL){
      strncpy(phrase_set_statement->child.lex_string_constant,(*curr)->token_string,(*curr)->token_string_len);
      phrase_set_statement->child.lex_string_constant[(*curr)->token_string_len]='\0';
    }
    else{
      phrase_set_statement->child.lex_string_constant[0]='\0';
    }
    consume(curr);
  }
  else if(check(*curr,tk_numeric_constant)){
    phrase_set_statement->child_type=lex_type_numeric_constant;
    if((phrase_set_statement->child.lex_numeric_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    strncpy(phrase_set_statement->child.lex_numeric_constant,(*curr)->token_string,(*curr)->token_string_len);
    phrase_set_statement->child.lex_numeric_constant[(*curr)->token_string_len]='\0';
    consume(curr);
  }
  else{
    if(has_sign){
      write_error_from_tokens("error: expected a constant or a symbol\n",*curr,(*curr)->prev_token,file_name);
    }
    else{
      write_error_from_tokens("error: expected an optional sign followed by a constant or a symbol\n",*curr,(*curr)->prev_token,file_name);
    }
    return 1;
  }
  phrase_set_statement->end_x=(*curr)->prev_token->end_x;
  phrase_set_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_import_statement(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_import_statement=malloc(sizeof(import_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  import_statement *phrase_import_statement=parent_phrase_statement->child.phrase_import_statement;
  phrase_import_statement->file=parent_phrase_statement->file;
  phrase_import_statement->parent=parent_phrase_statement;
  phrase_import_statement->lex_string_constant=NULL;
  phrase_import_statement->start_x=(*curr)->start_x;
  phrase_import_statement->start_y=(*curr)->start_y;
  consume(curr);
  if(!check(*curr,tk_string_constant)){
    write_error_from_tokens("error: expected a string-constant\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  if((phrase_import_statement->lex_string_constant=malloc((*curr)->token_string_len+1))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  strncpy(phrase_import_statement->lex_string_constant,(*curr)->token_string,(*curr)->token_string_len);
  phrase_import_statement->lex_string_constant[(*curr)->token_string_len]='\0';
  consume(curr);
  phrase_import_statement->end_x=(*curr)->prev_token->end_x;
  phrase_import_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

#define visibility_hidden 0
#define visibility_exposed 1

unsigned int parse_visibility(label *parent_phrase_label, token **curr, char *file_name){
  if((parent_phrase_label->phrase_visibility=malloc(sizeof(visibility)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  visibility *phrase_visibility=parent_phrase_label->phrase_visibility;
  phrase_visibility->file=file_name;
  phrase_visibility->parent=parent_phrase_label;
  phrase_visibility->start_x=(*curr)->start_x;
  phrase_visibility->start_y=(*curr)->start_y;
  phrase_visibility->end_x=(*curr)->end_x;
  phrase_visibility->end_y=(*curr)->end_y;
  if(check_type(*curr,key_hidden)){
    phrase_visibility->visibility_code=visibility_hidden;
    consume(curr);
  }
  else{
    phrase_visibility->visibility_code=visibility_exposed;
    consume(curr);
  }
  return 0;
}

unsigned int parse_label(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_label=malloc(sizeof(label)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  label *phrase_label=parent_phrase_statement->child.phrase_label;
  phrase_label->file=parent_phrase_statement->file;
  phrase_label->parent=parent_phrase_statement;
  phrase_label->lex_identifier=NULL;
  phrase_label->phrase_visibility=NULL;
  phrase_label->start_x=(*curr)->start_x;
  phrase_label->start_y=(*curr)->start_y;
  consume(curr);
  bool has_visibility=false;
  if(check(*curr,tk_keyword) && (check_type(*curr,key_hidden) || check_type(*curr,key_exposed))){
    if(parse_visibility(phrase_label,curr,file_name)){
      return 1;
    }
    has_visibility=true;
  }
  if(!check(*curr,tk_identifier)){
    if(!has_visibility){
      write_error_from_tokens("error: expected a symbol\n",*curr,(*curr)->prev_token,file_name);
    }
    else{
      write_error_from_tokens("error: expected an optional visibility followed by a symbol\n",*curr,(*curr)->prev_token,file_name);
    }
    return 1;
  }
  if((phrase_label->lex_identifier=malloc((*curr)->token_string_len+1))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  strncpy(phrase_label->lex_identifier,(*curr)->token_string,(*curr)->token_string_len);
  phrase_label->lex_identifier[(*curr)->token_string_len]='\0';
  consume(curr);
  phrase_label->end_x=(*curr)->prev_token->end_x;
  phrase_label->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_tag(tag_sequence *parent_phrase_tag_sequence, token **curr, char *file_name){
  if((parent_phrase_tag_sequence->phrase_tag=malloc(sizeof(tag)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  tag *phrase_tag=parent_phrase_tag_sequence->phrase_tag;
  phrase_tag->file=file_name;
  phrase_tag->parent=parent_phrase_tag_sequence;
  phrase_tag->start_x=(*curr)->start_x;
  phrase_tag->start_y=(*curr)->start_y;
  phrase_tag->end_x=(*curr)->end_x;
  phrase_tag->end_y=(*curr)->end_y;
  if((phrase_tag->lex_identifier=malloc((*curr)->token_string_len+1))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  strncpy(phrase_tag->lex_identifier,(*curr)->token_string,(*curr)->token_string_len);
  phrase_tag->lex_identifier[(*curr)->token_string_len]='\0';
  consume(curr);
  return 0;
}

extern unsigned int parse_struct_name(sizeof_statement *parent_phrase_sizeof_statement, alignof_statement *parent_phrase_alignof_statement, tagged_statement *parent_phrase_tagged_statement, struct_tag_definition *parent_phrase_struct_tag_definition, token **curr, char *file_name){
  struct_name *phrase_struct_name;
  if(parent_phrase_sizeof_statement!=NULL){
    if((parent_phrase_sizeof_statement->child.phrase_struct_name=malloc(sizeof(struct_name)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_struct_name=parent_phrase_sizeof_statement->child.phrase_struct_name;
    phrase_struct_name->parent_type=phrase_type_sizeof_statement;
    phrase_struct_name->parent.phrase_sizeof_statement=parent_phrase_sizeof_statement;
  }
  else if(parent_phrase_alignof_statement!=NULL){
    if((parent_phrase_alignof_statement->child.phrase_struct_name=malloc(sizeof(struct_name)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_struct_name=parent_phrase_alignof_statement->child.phrase_struct_name;
    phrase_struct_name->parent_type=phrase_type_alignof_statement;
    phrase_struct_name->parent.phrase_alignof_statement=parent_phrase_alignof_statement;
  }
  else if(parent_phrase_tagged_statement!=NULL){
    if((parent_phrase_tagged_statement->phrase_struct_name=malloc(sizeof(struct_name)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_struct_name=parent_phrase_tagged_statement->phrase_struct_name;
    phrase_struct_name->parent_type=phrase_type_tagged_statement;
    phrase_struct_name->parent.phrase_tagged_statement=parent_phrase_tagged_statement;
  }
  else{
    if((parent_phrase_struct_tag_definition->child.phrase_struct_name=malloc(sizeof(struct_name)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_struct_name=parent_phrase_struct_tag_definition->child.phrase_struct_name;
    phrase_struct_name->parent_type=phrase_type_struct_tag_definition;
    phrase_struct_name->parent.phrase_struct_tag_definition=parent_phrase_struct_tag_definition;
  }
  phrase_struct_name->file=file_name;
  phrase_struct_name->start_x=(*curr)->start_x;
  phrase_struct_name->start_y=(*curr)->start_y;
  phrase_struct_name->end_x=(*curr)->end_x;
  phrase_struct_name->end_y=(*curr)->end_y;
  if((phrase_struct_name->lex_identifier=malloc((*curr)->token_string_len+1))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  strncpy(phrase_struct_name->lex_identifier,(*curr)->token_string,(*curr)->token_string_len);
  phrase_struct_name->lex_identifier[(*curr)->token_string_len]='\0';
  consume(curr);
  return 0;
}

extern unsigned int parse_type(struct_tag_definition *parent_phrase_struct_tag_definition, sizeof_statement *parent_phrase_sizeof_statement, alignof_statement *parent_phrase_alignof_statement, token **curr, char *file_name){
  type *phrase_type;
  if(parent_phrase_struct_tag_definition!=NULL){
    if((parent_phrase_struct_tag_definition->child.phrase_type=malloc(sizeof(type)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_type=parent_phrase_struct_tag_definition->child.phrase_type;
    phrase_type->parent_type=phrase_type_struct_tag_definition;
    phrase_type->parent.phrase_struct_tag_definition=parent_phrase_struct_tag_definition;
  }
  else if(parent_phrase_sizeof_statement!=NULL){
    if((parent_phrase_sizeof_statement->child.phrase_type=malloc(sizeof(type)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_type=parent_phrase_sizeof_statement->child.phrase_type;
    phrase_type->parent_type=phrase_type_sizeof_statement;
    phrase_type->parent.phrase_sizeof_statement=parent_phrase_sizeof_statement;
  }
  else{
    if((parent_phrase_alignof_statement->child.phrase_type=malloc(sizeof(type)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_type=parent_phrase_alignof_statement->child.phrase_type;
    phrase_type->parent_type=phrase_type_alignof_statement;
    phrase_type->parent.phrase_alignof_statement=parent_phrase_alignof_statement;
  }
  phrase_type->file=file_name;
  phrase_type->start_x=(*curr)->start_x;
  phrase_type->start_y=(*curr)->start_y;
  phrase_type->end_x=(*curr)->end_x;
  phrase_type->end_y=(*curr)->end_y;
  phrase_type->type_code=(*curr)->token_type;
  consume(curr);
  return 0;
}

unsigned int parse_struct_tag_definition(struct_tag_definition_sequence *parent_phrase_struct_tag_definition_sequence, token **curr, char *file_name){
  if((parent_phrase_struct_tag_definition_sequence->phrase_struct_tag_definition=malloc(sizeof(struct_tag_definition)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  struct_tag_definition *phrase_struct_tag_definition=parent_phrase_struct_tag_definition_sequence->phrase_struct_tag_definition;
  phrase_struct_tag_definition->file=file_name;
  phrase_struct_tag_definition->parent=parent_phrase_struct_tag_definition_sequence;
  phrase_struct_tag_definition->child_type=fill_incomplete;
  phrase_struct_tag_definition->lex_identifier=NULL;
  phrase_struct_tag_definition->start_x=(*curr)->start_x;
  phrase_struct_tag_definition->start_y=(*curr)->start_y;
  if(check(*curr,tk_identifier)){
    phrase_struct_tag_definition->child_type=phrase_type_struct_name;
    if(parse_struct_name(NULL,NULL,NULL,phrase_struct_tag_definition,curr,file_name)){
      return 1;
    }
  }
  else{
    phrase_struct_tag_definition->child_type=phrase_type_type;
    if(parse_type(phrase_struct_tag_definition,NULL,NULL,curr,file_name)){
      return 1;
    }
  }
  if(!(check(*curr,tk_punctuator) && match_type(curr,punc_colon))){
    write_error_from_tokens("error: expected a punctuator (:)\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  if(check(*curr,tk_identifier)){
    if((phrase_struct_tag_definition->lex_identifier=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    strncpy(phrase_struct_tag_definition->lex_identifier,(*curr)->token_string,(*curr)->token_string_len);
    phrase_struct_tag_definition->lex_identifier[(*curr)->token_string_len]='\0';
    consume(curr);
  }
  else{
    write_error_from_tokens("error: expected a tag\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  phrase_struct_tag_definition->end_x=(*curr)->prev_token->end_x;
  phrase_struct_tag_definition->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_struct_tag_definition_sequence(struct_definition *parent_phrase_struct_definition, token **curr, char *file_name){
  struct_tag_definition_sequence *phrase_struct_tag_definition_sequence;
  struct_tag_definition_sequence *parent_phrase_struct_tag_definition_sequence=NULL;
  while(1){
    if(parent_phrase_struct_tag_definition_sequence==NULL){
      if((parent_phrase_struct_definition->phrase_struct_tag_definition_sequence=malloc(sizeof(struct_tag_definition_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      phrase_struct_tag_definition_sequence=parent_phrase_struct_definition->phrase_struct_tag_definition_sequence;
      phrase_struct_tag_definition_sequence->parent_type=phrase_type_struct_definition;
      phrase_struct_tag_definition_sequence->parent.phrase_struct_definition=parent_phrase_struct_definition;
    }
    else{
      if((parent_phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence=malloc(sizeof(struct_tag_definition_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      phrase_struct_tag_definition_sequence=parent_phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence;
      phrase_struct_tag_definition_sequence->parent_type=phrase_type_struct_tag_definition_sequence;
      phrase_struct_tag_definition_sequence->parent.phrase_struct_tag_definition_sequence=parent_phrase_struct_tag_definition_sequence;
    }
    phrase_struct_tag_definition_sequence->file=file_name;
    phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence=NULL;
    if(parse_struct_tag_definition(phrase_struct_tag_definition_sequence,curr,file_name)){
      return 1;
    }
    if((check(*curr,tk_keyword) && (check_type(*curr,key_uc) || check_type(*curr,key_us) || check_type(*curr,key_ui) || check_type(*curr,key_c) || check_type(*curr,key_s) || check_type(*curr,key_i) || check_type(*curr,key_l) || check_type(*curr,key_p))) || (check(*curr,tk_identifier))){
      parent_phrase_struct_tag_definition_sequence=phrase_struct_tag_definition_sequence;
    }
    else{
      break;
    }
  }
  return 0;
}

unsigned int parse_else_clause(selection_statement *parent_phrase_selection_statement, token **curr, char *file_name){
  if((parent_phrase_selection_statement->phrase_else_clause=malloc(sizeof(else_clause)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  else_clause *phrase_else_clause=parent_phrase_selection_statement->phrase_else_clause;
  phrase_else_clause->file=parent_phrase_selection_statement->file;
  phrase_else_clause->parent=parent_phrase_selection_statement;
  phrase_else_clause->child_type=fill_incomplete;
  phrase_else_clause->start_x=(*curr)->start_x;
  phrase_else_clause->start_y=(*curr)->start_y;
  consume(curr);
  if(check(*curr,tk_punctuator) && check_type(*curr,punc_openingcbrace)){
    consume(curr);
    phrase_else_clause->child_type=phrase_type_statement_sequence;
    phrase_else_clause->child.phrase_statement_sequence=NULL;
    if((check(*curr,tk_punctuator) && check_type(*curr,punc_colon)) || (check(*curr,tk_punctuator) && check_type(*curr,punc_plus)) || (check(*curr,tk_keyword))){
      if(parse_statement_sequence(NULL,NULL,NULL,phrase_else_clause,NULL,curr,file_name)==NULL){
        return 1;
      }
    }
    if(!(check(*curr,tk_punctuator) && match_type(curr,punc_closingcbrace))){
      write_error_from_tokens("error: expected a punctuator (})\n",*curr,(*curr)->prev_token,file_name);
      return 1;
    }
  }
  else{
    phrase_else_clause->child_type=phrase_type_statement;
    if((check(*curr,tk_punctuator) && check_type(*curr,punc_colon)) || (check(*curr,tk_punctuator) && check_type(*curr,punc_plus)) || (check(*curr,tk_keyword) && can_be_leftmost_key_in_statement)){
      if(parse_statement(NULL,NULL,phrase_else_clause,curr,file_name)){
        return 1;
      }
    }
    else{
      write_error_from_tokens("error: expected a statement\n",*curr,(*curr)->prev_token,file_name);
      return 1;
    }
  }
  phrase_else_clause->end_x=(*curr)->prev_token->end_x;
  phrase_else_clause->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_selection_statement(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_selection_statement=malloc(sizeof(selection_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  selection_statement *phrase_selection_statement=parent_phrase_statement->child.phrase_selection_statement;
  phrase_selection_statement->file=parent_phrase_statement->file;
  phrase_selection_statement->parent=parent_phrase_statement;
  phrase_selection_statement->phrase_statement_sequence=NULL;
  phrase_selection_statement->child_type=fill_incomplete;
  phrase_selection_statement->phrase_else_clause=NULL;
  phrase_selection_statement->start_x=(*curr)->start_x;
  phrase_selection_statement->start_y=(*curr)->start_y;
  consume(curr);
  if(!(check(*curr,tk_punctuator) && match_type(curr,punc_openingparen))){
    write_error_from_tokens("error: expected a punctuator (()\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  if((check(*curr,tk_punctuator) && check_type(*curr,punc_colon)) || (check(*curr,tk_punctuator) && check_type(*curr,punc_plus)) || (check(*curr,tk_keyword))){
    if(parse_statement_sequence(NULL,phrase_selection_statement,NULL,NULL,NULL,curr,file_name)==NULL){
      return 1;
    }
  }
  if(!(check(*curr,tk_punctuator) && match_type(curr,punc_closingparen))){
    write_error_from_tokens("error: expected a punctuator ())\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  if(check(*curr,tk_punctuator) && check_type(*curr,punc_openingcbrace)){
    consume(curr);
    phrase_selection_statement->child_type=phrase_type_statement_sequence;
    phrase_selection_statement->child.phrase_statement_sequence=NULL;
    if((check(*curr,tk_punctuator) && check_type(*curr,punc_colon)) || (check(*curr,tk_punctuator) && check_type(*curr,punc_plus)) || (check(*curr,tk_keyword))){
      if(parse_statement_sequence(NULL,NULL,phrase_selection_statement,NULL,NULL,curr,file_name)==NULL){
        return 1;
      }
    }
    if(!(check(*curr,tk_punctuator) && match_type(curr,punc_closingcbrace))){
      write_error_from_tokens("error: expected a punctuator (})\n",*curr,(*curr)->prev_token,file_name);
      return 1;
    }
  }
  else{
    phrase_selection_statement->child_type=phrase_type_statement;
    if((check(*curr,tk_punctuator) && check_type(*curr,punc_colon)) || (check(*curr,tk_punctuator) && check_type(*curr,punc_plus)) || (check(*curr,tk_keyword) && can_be_leftmost_key_in_statement)){
      if(parse_statement(NULL,phrase_selection_statement,NULL,curr,file_name)){
        return 1;
      }
    }
    else{
      write_error_from_tokens("error: expected a statement\n",*curr,(*curr)->prev_token,file_name);
      return 1;
    }
  }
  if(check(*curr,tk_keyword) && check_type(*curr,key_else)){
    if(parse_else_clause(phrase_selection_statement,curr,file_name)){
      return 1;
    }
  }
  phrase_selection_statement->end_x=(*curr)->prev_token->end_x;
  phrase_selection_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_struct_definition(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_struct_definition=malloc(sizeof(struct_definition)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  struct_definition *phrase_struct_definition=parent_phrase_statement->child.phrase_struct_definition;
  phrase_struct_definition->file=parent_phrase_statement->file;
  phrase_struct_definition->parent=parent_phrase_statement;
  phrase_struct_definition->lex_identifier=NULL;
  phrase_struct_definition->phrase_struct_tag_definition_sequence=NULL;
  phrase_struct_definition->start_x=(*curr)->start_x;
  phrase_struct_definition->start_y=(*curr)->start_y;
  consume(curr);
  if(!check(*curr,tk_identifier)){
    write_error_from_tokens("error: expected a struct-name\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  if((phrase_struct_definition->lex_identifier=malloc((*curr)->token_string_len+1))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  strncpy(phrase_struct_definition->lex_identifier,(*curr)->token_string,(*curr)->token_string_len);
  phrase_struct_definition->lex_identifier[(*curr)->token_string_len]='\0';
  consume(curr);
  if(!(check(*curr,tk_punctuator) && match_type(curr,punc_openingcbrace))){
    write_error_from_tokens("error: expected a punctuator ({)\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  if((check(*curr,tk_keyword) && (check_type(*curr,key_uc) || check_type(*curr,key_us) || check_type(*curr,key_ui) || check_type(*curr,key_c) || check_type(*curr,key_s) || check_type(*curr,key_i) || check_type(*curr,key_l) || check_type(*curr,key_p))) || (check(*curr,tk_identifier))){
    if(parse_struct_tag_definition_sequence(phrase_struct_definition,curr,file_name)){
      return 1;
    }
  }
  if(!(check(*curr,tk_punctuator) && match_type(curr,punc_closingcbrace))){
    write_error_from_tokens("error: expected a punctuator (})\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  phrase_struct_definition->end_x=(*curr)->prev_token->end_x;
  phrase_struct_definition->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_tag_sequence(tagged_statement *parent_phrase_tagged_statement, token **curr, char *file_name){
  tag_sequence *phrase_tag_sequence;
  tag_sequence *parent_phrase_tag_sequence=NULL;
  while(1){
    if(parent_phrase_tag_sequence==NULL){
      if((parent_phrase_tagged_statement->phrase_tag_sequence=malloc(sizeof(tag_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      phrase_tag_sequence=parent_phrase_tagged_statement->phrase_tag_sequence;
      phrase_tag_sequence->parent_type=phrase_type_tagged_statement;
      phrase_tag_sequence->parent.phrase_tagged_statement=parent_phrase_tagged_statement;
    }
    else{
      if((parent_phrase_tag_sequence->phrase_tag_sequence=malloc(sizeof(tag_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      phrase_tag_sequence=parent_phrase_tag_sequence->phrase_tag_sequence;
      phrase_tag_sequence->parent_type=phrase_type_tag_sequence;
      phrase_tag_sequence->parent.phrase_tag_sequence=parent_phrase_tag_sequence;
    }
    phrase_tag_sequence->file=file_name;
    phrase_tag_sequence->phrase_tag_sequence=NULL;
    consume(curr);
    if(check(*curr,tk_identifier)){
      if(parse_tag(phrase_tag_sequence,curr,file_name)){
        return 1;
      }
    }
    else{
      write_error_from_tokens("error: expected a tag\n",*curr,(*curr)->prev_token,file_name);
      return 1;
    }
    if(check(*curr,tk_punctuator) && check_type(*curr,punc_dot)){
      parent_phrase_tag_sequence=phrase_tag_sequence;
    }
    else{
      break;
    }
  }
  return 0;
}

unsigned int parse_alignof_statement(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_alignof_statement=malloc(sizeof(alignof_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  alignof_statement *phrase_alignof_statement=parent_phrase_statement->child.phrase_alignof_statement;
  phrase_alignof_statement->file=parent_phrase_statement->file;
  phrase_alignof_statement->parent=parent_phrase_statement;
  phrase_alignof_statement->child_type=fill_incomplete;
  phrase_alignof_statement->start_x=(*curr)->start_x;
  phrase_alignof_statement->start_y=(*curr)->start_y;
  consume(curr);
  if(check(*curr,tk_keyword) && (check_type(*curr,key_uc) || check_type(*curr,key_us) || check_type(*curr,key_ui) || check_type(*curr,key_c) || check_type(*curr,key_s) || check_type(*curr,key_i) || check_type(*curr,key_l) || check_type(*curr,key_p))){
    phrase_alignof_statement->child_type=phrase_type_type;
    if(parse_type(NULL,NULL,phrase_alignof_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check(*curr,tk_identifier)){
    phrase_alignof_statement->child_type=phrase_type_struct_name;
    if(parse_struct_name(NULL,phrase_alignof_statement,NULL,NULL,curr,file_name)){
      return 1;
    }
  }
  else{
    write_error_from_tokens("error: expected a type or a struct-name\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  phrase_alignof_statement->end_x=(*curr)->prev_token->end_x;
  phrase_alignof_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_sizeof_statement(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_sizeof_statement=malloc(sizeof(sizeof_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  sizeof_statement *phrase_sizeof_statement=parent_phrase_statement->child.phrase_sizeof_statement;
  parent_phrase_statement->child.phrase_sizeof_statement->file=parent_phrase_statement->file;
  parent_phrase_statement->child.phrase_sizeof_statement->parent=parent_phrase_statement;
  parent_phrase_statement->child.phrase_sizeof_statement->child_type=fill_incomplete;
  phrase_sizeof_statement->start_x=(*curr)->start_x;
  phrase_sizeof_statement->start_y=(*curr)->start_y;
  consume(curr);
  if(check(*curr,tk_keyword) && (check_type(*curr,key_uc) || check_type(*curr,key_us) || check_type(*curr,key_ui) || check_type(*curr,key_c) || check_type(*curr,key_s) || check_type(*curr,key_i) || check_type(*curr,key_l) || check_type(*curr,key_p))){
    phrase_sizeof_statement->child_type=phrase_type_type;
    if(parse_type(NULL,phrase_sizeof_statement,NULL,curr,file_name)){
      return 1;
    }
  }
  else if(check(*curr,tk_identifier)){
    phrase_sizeof_statement->child_type=phrase_type_struct_name;
    if(parse_struct_name(phrase_sizeof_statement,NULL,NULL,NULL,curr,file_name)){
      return 1;
    }
  }
  else{
    write_error_from_tokens("error: expected a type\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  phrase_sizeof_statement->end_x=(*curr)->prev_token->end_x;
  phrase_sizeof_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_tagged_statement(statement *parent_phrase_statement, token **curr, char *file_name){
  if((parent_phrase_statement->child.phrase_tagged_statement=malloc(sizeof(tagged_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  tagged_statement *phrase_tagged_statement=parent_phrase_statement->child.phrase_tagged_statement;
  phrase_tagged_statement->file=parent_phrase_statement->file;
  phrase_tagged_statement->parent=parent_phrase_statement;
  phrase_tagged_statement->phrase_tag_sequence=NULL;
  phrase_tagged_statement->start_x=(*curr)->start_x;
  phrase_tagged_statement->start_y=(*curr)->start_y;
  consume(curr);
  if(check(*curr,tk_identifier)){
    if(parse_struct_name(NULL,NULL,phrase_tagged_statement,NULL,curr,file_name)){
      return 1;
    }
  }
  else{
    write_error_from_tokens("error: expected a struct-name\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  if(check(*curr,tk_punctuator) && check_type(*curr,punc_dot)){
    if(parse_tag_sequence(phrase_tagged_statement,curr,file_name)){
      return 1;
    }
  }
  phrase_tagged_statement->end_x=(*curr)->prev_token->end_x;
  phrase_tagged_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_statement(statement_sequence *parent_phrase_statement_sequence, selection_statement *parent_phrase_selection_statement, else_clause *parent_phrase_else_clause, token **curr, char *file_name){
  statement *phrase_statement;
  if(parent_phrase_statement_sequence!=NULL){
    if((parent_phrase_statement_sequence->phrase_statement=malloc(sizeof(statement)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_statement=parent_phrase_statement_sequence->phrase_statement;
    phrase_statement->parent_type=phrase_type_statement_sequence;
    phrase_statement->parent.phrase_statement_sequence=parent_phrase_statement_sequence;
    phrase_statement->tu=parent_phrase_statement_sequence->tu;
  }
  else if(parent_phrase_selection_statement!=NULL){
    if((parent_phrase_selection_statement->child.phrase_statement=malloc(sizeof(statement)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_statement=parent_phrase_selection_statement->child.phrase_statement;
    phrase_statement->parent_type=phrase_type_selection_statement;
    phrase_statement->parent.phrase_selection_statement=parent_phrase_selection_statement;
    phrase_statement->tu=parent_phrase_selection_statement->parent->tu;
  }
  else{
    if((parent_phrase_else_clause->child.phrase_statement=malloc(sizeof(statement)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_statement=parent_phrase_else_clause->child.phrase_statement;
    phrase_statement->parent_type=phrase_type_else_clause;
    phrase_statement->parent.phrase_else_clause=parent_phrase_else_clause;
    phrase_statement->tu=parent_phrase_else_clause->parent->parent->tu;
  }
  phrase_statement->file=file_name;
  phrase_statement->child_type=fill_incomplete;
  if(check(*curr,tk_punctuator) && check_type(*curr,punc_colon)){
    phrase_statement->child_type=phrase_type_label;
    if(parse_label(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check(*curr,tk_punctuator) && check_type(*curr,punc_plus)){
    phrase_statement->child_type=phrase_type_tagged_statement;
    if(parse_tagged_statement(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check_type(*curr,key_pushc) || check_type(*curr,key_pushs) || check_type(*curr,key_pushi) || check_type(*curr,key_pushl) || check_type(*curr,key_pushuc) || check_type(*curr,key_pushus) || check_type(*curr,key_pushui) || check_type(*curr,key_pushp)){
    phrase_statement->child_type=phrase_type_push_statement;
    if(parse_push_statement(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check_type(*curr,key_if)){
    phrase_statement->child_type=phrase_type_selection_statement;
    if(parse_selection_statement(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check_type(*curr,key_alignof)){
    phrase_statement->child_type=phrase_type_alignof_statement;
    if(parse_alignof_statement(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check_type(*curr,key_sizeof)){
    phrase_statement->child_type=phrase_type_sizeof_statement;
    if(parse_sizeof_statement(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check_type(*curr,key_set)){
    phrase_statement->child_type=phrase_type_set_statement;
    if(parse_set_statement(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check_type(*curr,key_import)){
    phrase_statement->child_type=phrase_type_import_statement;
    if(parse_import_statement(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check_type(*curr,key_struct)){
    phrase_statement->child_type=phrase_type_struct_definition;
    if(parse_struct_definition(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else{
    phrase_statement->child_type=phrase_type_instruction;
    if(parse_instruction(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  phrase_statement->end_x=(*curr)->prev_token->end_x;
  phrase_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

statement_sequence *parse_statement_sequence(translation_unit *parent_phrase_translation_unit, selection_statement *parent_phrase_selection_statement_0, selection_statement *parent_phrase_selection_statement_1, else_clause *parent_phrase_else_clause, statement_sequence *parent_phrase_statement_sequence, token **curr, char *file_name){
  statement_sequence *phrase_statement_sequence;
  while(1){
    if(parent_phrase_statement_sequence!=NULL){
      if((parent_phrase_statement_sequence->phrase_statement_sequence=malloc(sizeof(statement_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return NULL;
      }
      phrase_statement_sequence=parent_phrase_statement_sequence->phrase_statement_sequence;
      phrase_statement_sequence->parent_type=phrase_type_statement_sequence;
      phrase_statement_sequence->parent.phrase_statement_sequence=parent_phrase_statement_sequence;
      phrase_statement_sequence->tu=parent_phrase_statement_sequence->tu;
    }
    else if(parent_phrase_translation_unit!=NULL){
      if((parent_phrase_translation_unit->phrase_statement_sequence=malloc(sizeof(statement_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return NULL;
      }
      phrase_statement_sequence=parent_phrase_translation_unit->phrase_statement_sequence;
      phrase_statement_sequence->parent_type=phrase_type_translation_unit;
      phrase_statement_sequence->parent.phrase_translation_unit=parent_phrase_translation_unit;
      phrase_statement_sequence->tu=parent_phrase_translation_unit;
    }
    else if(parent_phrase_selection_statement_0!=NULL){
      if((parent_phrase_selection_statement_0->phrase_statement_sequence=malloc(sizeof(statement_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return NULL;
      }
      phrase_statement_sequence=parent_phrase_selection_statement_0->phrase_statement_sequence;
      phrase_statement_sequence->parent_type=phrase_type_selection_statement;
      phrase_statement_sequence->parent.phrase_selection_statement=parent_phrase_selection_statement_0;
      phrase_statement_sequence->tu=parent_phrase_selection_statement_0->parent->tu;
    }
    else if(parent_phrase_selection_statement_1!=NULL){
      if((parent_phrase_selection_statement_1->child.phrase_statement_sequence=malloc(sizeof(statement_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return NULL;
      }
      phrase_statement_sequence=parent_phrase_selection_statement_1->child.phrase_statement_sequence;
      phrase_statement_sequence->parent_type=phrase_type_selection_statement;
      phrase_statement_sequence->parent.phrase_selection_statement=parent_phrase_selection_statement_1;
      phrase_statement_sequence->tu=parent_phrase_selection_statement_1->parent->tu;
    }
    else{
      if((parent_phrase_else_clause->child.phrase_statement_sequence=malloc(sizeof(statement_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return NULL;
      }
      phrase_statement_sequence=parent_phrase_else_clause->child.phrase_statement_sequence;
      phrase_statement_sequence->parent_type=phrase_type_else_clause;
      phrase_statement_sequence->parent.phrase_else_clause=parent_phrase_else_clause;
      phrase_statement_sequence->tu=parent_phrase_else_clause->parent->parent->tu;
    }
    phrase_statement_sequence->file=file_name;
    phrase_statement_sequence->phrase_statement_sequence=NULL;
    if(parse_statement(phrase_statement_sequence,NULL,NULL,curr,file_name)){
      return NULL;
    }
    if((check(*curr,tk_punctuator) && check_type(*curr,punc_colon)) || (check(*curr,tk_punctuator) && check_type(*curr,punc_plus)) || (check(*curr,tk_keyword) && can_be_leftmost_key_in_statement)){
      parent_phrase_statement_sequence=phrase_statement_sequence;
    }
    else{
      break;
    }
  }
  return phrase_statement_sequence;
}

unsigned int parse_translation_unit(translation_unit **phrase_translation_unit, token *curr, char *file_name){
  if((*phrase_translation_unit=malloc(sizeof(translation_unit)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  (*phrase_translation_unit)->file=file_name;
  (*phrase_translation_unit)->phrase_statement_sequence=NULL;
  (*phrase_translation_unit)->start_x=curr->start_x;
  (*phrase_translation_unit)->start_y=curr->start_y;
  if((check(curr,tk_punctuator) && check_type(curr,punc_colon)) || (check(curr,tk_punctuator) && check_type(curr,punc_plus)) || check(curr,tk_keyword)){
    if(parse_statement_sequence(*phrase_translation_unit,NULL,NULL,NULL,NULL,&curr,file_name)==NULL){
      return 1;
    }
  }
  if(!check(curr,tokens_list_end)){
    write_error_from_tokens("error: expected end of translation unit\n",curr,curr->prev_token,file_name);
    return 1;
  }
  (*phrase_translation_unit)->end_x=curr->prev_token->end_x;
  (*phrase_translation_unit)->end_y=curr->prev_token->end_y;
  return 0;
}

translation_unit *parse(token *tokens, unsigned int *parse_ret, char *file_name){
  translation_unit *phrase_translation_unit;
  if(parse_translation_unit(&phrase_translation_unit,tokens,file_name)){
    *parse_ret=1;
    return phrase_translation_unit;
  }
  *parse_ret=0;
  return phrase_translation_unit;
}
