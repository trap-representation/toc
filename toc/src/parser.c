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

static inline void consume(token **curr){
  *curr=(*curr)->next_token;
}

static inline bool check(token *tk, unsigned int m){
  if(tk->token_class==m){
    return true;
  }
  return false;
}

static inline bool match_type(token **tk, unsigned int m){
  if((*tk)->token_type==m){
    consume(tk);
    return true;
  }
  return false;
}

static inline bool check_type(token *tk, unsigned int m){
  if(tk->token_type==m){
    return true;
  }
  return false;
}

unsigned int parse_instruction(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_instruction=malloc(sizeof(instruction)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  instruction *phrase_instruction=phrase_statement->child.phrase_instruction;
  phrase_instruction->parent=phrase_statement;
  phrase_instruction->file=file_name;
  phrase_instruction->start_x=(*curr)->start_x;
  phrase_instruction->start_y=(*curr)->start_y;
  phrase_instruction->end_x=(*curr)->end_x;
  phrase_instruction->end_y=(*curr)->end_y;
  phrase_instruction->instruction_code=(*curr)->token_type;
  phrase_statement->child.phrase_instruction=phrase_instruction;
  consume(curr);
  return 0;
}

unsigned int parse_sign(push_statement *phrase_push_statement, set_statement *phrase_set_statement, token **curr, char *file_name){
  sign *phrase_sign;
  if(phrase_push_statement!=NULL){
    if((phrase_push_statement->phrase_sign=malloc(sizeof(sign)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_sign=phrase_push_statement->phrase_sign;
    phrase_sign->parent_type=phrase_type_push_statement;
    phrase_sign->parent.phrase_push_statement=phrase_push_statement;
  }
  else{
    if((phrase_set_statement->phrase_sign=malloc(sizeof(sign)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_sign=phrase_set_statement->phrase_sign;
    phrase_sign->parent_type=phrase_type_set_statement;
    phrase_sign->parent.phrase_set_statement=phrase_set_statement;
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

unsigned int parse_push_instruction(push_statement *phrase_push_statement, token **curr, char *file_name){
  if((phrase_push_statement->phrase_push_instruction=malloc(sizeof(push_instruction)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  push_instruction *phrase_push_instruction=phrase_push_statement->phrase_push_instruction;
  phrase_push_instruction->file=file_name;
  phrase_push_instruction->parent=phrase_push_statement;
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

unsigned int parse_symbol(push_statement *phrase_push_statement, set_statement *phrase_set_statement, token **curr, char *file_name){
  symbol *phrase_symbol;
  if(phrase_push_statement!=NULL){
    if((phrase_push_statement->child.phrase_symbol=malloc(sizeof(symbol)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_symbol=phrase_push_statement->child.phrase_symbol;
    phrase_symbol->parent_type=phrase_type_push_statement;
    phrase_symbol->parent.phrase_push_statement=phrase_push_statement;
  }
  else{
    if((phrase_set_statement->child1.phrase_symbol=malloc(sizeof(symbol)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_symbol=phrase_set_statement->child1.phrase_symbol;
    phrase_symbol->parent_type=phrase_type_set_statement;
    phrase_symbol->parent.phrase_set_statement=phrase_set_statement;
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

unsigned int parse_push_statement(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_push_statement=malloc(sizeof(push_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  push_statement *phrase_push_statement=phrase_statement->child.phrase_push_statement;
  phrase_push_statement->file=phrase_statement->file;
  phrase_push_statement->parent=phrase_statement;
  phrase_push_statement->child_type=fill_incomplete;
  phrase_push_statement->phrase_sign=NULL;
  phrase_push_statement->start_x=(*curr)->start_x;
  phrase_push_statement->start_y=(*curr)->start_y;
  if(parse_push_instruction(phrase_push_statement,curr,file_name)){
    return 1;
  }
  bool has_sign=false;
  if(check(*curr,tk_punctuator) && (check_type(*curr,punc_minus) || check_type(*curr,punc_plus))){
    parse_sign(phrase_push_statement,NULL,curr,file_name);
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
      write_error_from_tokens("error: expected an optional sign, followed by a constant or a symbol\n",*curr,(*curr)->prev_token,file_name);
      return 1;
    }
  }
  phrase_push_statement->end_x=(*curr)->prev_token->end_x;
  phrase_push_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_set_statement(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_set_statement=malloc(sizeof(set_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  set_statement *phrase_set_statement=phrase_statement->child.phrase_set_statement;
  phrase_set_statement->file=phrase_statement->file;
  phrase_set_statement->parent=phrase_statement;
  phrase_set_statement->lex_identifier=NULL;
  phrase_set_statement->phrase_sign=NULL;
  phrase_set_statement->child1_type=fill_incomplete;
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
    parse_sign(NULL,phrase_set_statement,curr,file_name);
    has_sign=true;
  }
  if(check(*curr,tk_identifier)){
    phrase_set_statement->child1_type=phrase_type_symbol;
    if(parse_symbol(NULL,phrase_set_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check(*curr,tk_character_constant)){
    phrase_set_statement->child1_type=lex_type_character_constant;
    if((phrase_set_statement->child1.lex_character_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    if((*curr)->token_string!=NULL){
      strncpy(phrase_set_statement->child1.lex_character_constant,(*curr)->token_string,(*curr)->token_string_len);
      phrase_set_statement->child1.lex_character_constant[(*curr)->token_string_len]='\0';
    }
    else{
      phrase_set_statement->child1.lex_character_constant[0]='\0';
    }
    consume(curr);
  }
  else if(check(*curr,tk_string_constant)){
    phrase_set_statement->child1_type=lex_type_string_constant;
    if((phrase_set_statement->child1.lex_string_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    if((*curr)->token_string!=NULL){
      strncpy(phrase_set_statement->child1.lex_string_constant,(*curr)->token_string,(*curr)->token_string_len);
      phrase_set_statement->child1.lex_string_constant[(*curr)->token_string_len]='\0';
    }
    else{
      phrase_set_statement->child1.lex_string_constant[0]='\0';
    }
    consume(curr);
  }
  else if(check(*curr,tk_numeric_constant)){
    phrase_set_statement->child1_type=lex_type_numeric_constant;
    if((phrase_set_statement->child1.lex_numeric_constant=malloc((*curr)->token_string_len+1))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    strncpy(phrase_set_statement->child1.lex_numeric_constant,(*curr)->token_string,(*curr)->token_string_len);
    phrase_set_statement->child1.lex_numeric_constant[(*curr)->token_string_len]='\0';
    consume(curr);
  }
  else{
    if(has_sign){
      write_error_from_tokens("error: expected a constant or a symbol\n",*curr,(*curr)->prev_token,file_name);
    }
    else{
      write_error_from_tokens("error: expected an optional sign, followed by a constant or a symbol\n",*curr,(*curr)->prev_token,file_name);
    }
    return 1;
  }
  phrase_set_statement->end_x=(*curr)->prev_token->end_x;
  phrase_set_statement->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_import_statement(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_import_statement=malloc(sizeof(import_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  import_statement *phrase_import_statement=phrase_statement->child.phrase_import_statement;
  phrase_import_statement->file=phrase_statement->file;
  phrase_import_statement->parent=phrase_statement;
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

unsigned int parse_visibility(label *phrase_label, token **curr, char *file_name){
  if((phrase_label->phrase_visibility=malloc(sizeof(visibility)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  visibility *phrase_visibility=phrase_label->phrase_visibility;
  phrase_visibility->file=file_name;
  phrase_visibility->parent=phrase_label;
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

unsigned int parse_label(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_label=malloc(sizeof(label)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  label *phrase_label=phrase_statement->child.phrase_label;
  phrase_label->file=phrase_statement->file;
  phrase_label->parent=phrase_statement;
  phrase_label->lex_identifier=NULL;
  phrase_label->phrase_visibility=NULL;
  phrase_label->start_x=(*curr)->start_x;
  phrase_label->start_y=(*curr)->start_y;
  consume(curr);
  bool doesnot_have_phrase_visibility=true;
  if(check(*curr,tk_keyword) && (check_type(*curr,key_hidden) || check_type(*curr,key_exposed))){
    if(parse_visibility(phrase_label,curr,file_name)){
      return 0;
    }
  }
  if(!check(*curr,tk_identifier)){
    if(doesnot_have_phrase_visibility){
      write_error_from_tokens("error: expected a symbol\n",*curr,(*curr)->prev_token,file_name);
    }
    else{
      write_error_from_tokens("error: expected an optional visibility, followed by a symbol\n",*curr,(*curr)->prev_token,file_name);
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

unsigned int parse_tag(tag_sequence *phrase_tag_sequence, token **curr, char *file_name){
  if((phrase_tag_sequence->phrase_tag=malloc(sizeof(tag)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  tag *phrase_tag=phrase_tag_sequence->phrase_tag;
  phrase_tag->file=file_name;
  phrase_tag->parent=phrase_tag_sequence;
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

extern unsigned int parse_struct_name(sizeof_statement *phrase_sizeof_statement, alignof_statement *phrase_alignof_statement, tagged_expression *phrase_tagged_expression, struct_tag_definition *phrase_struct_tag_definition, token **curr, char *file_name){
  struct_name *phrase_struct_name;
  if(phrase_sizeof_statement!=NULL){
    if((phrase_sizeof_statement->child.phrase_struct_name=malloc(sizeof(struct_name)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_struct_name=phrase_sizeof_statement->child.phrase_struct_name;
    phrase_struct_name->parent_type=phrase_type_sizeof_statement;
    phrase_struct_name->parent.phrase_sizeof_statement=phrase_sizeof_statement;
  }
  else if(phrase_alignof_statement!=NULL){
    if((phrase_alignof_statement->child.phrase_struct_name=malloc(sizeof(struct_name)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_struct_name=phrase_alignof_statement->child.phrase_struct_name;
    phrase_struct_name->parent_type=phrase_type_alignof_statement;
    phrase_struct_name->parent.phrase_alignof_statement=phrase_alignof_statement;
  }
  else if(phrase_tagged_expression!=NULL){
    if((phrase_tagged_expression->phrase_struct_name=malloc(sizeof(struct_name)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_struct_name=phrase_tagged_expression->phrase_struct_name;
    phrase_struct_name->parent_type=phrase_type_tagged_expression;
    phrase_struct_name->parent.phrase_tagged_expression=phrase_tagged_expression;
  }
  else{
    if((phrase_struct_tag_definition->child.phrase_struct_name=malloc(sizeof(struct_name)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_struct_name=phrase_struct_tag_definition->child.phrase_struct_name;
    phrase_struct_name->parent_type=phrase_type_struct_tag_definition;
    phrase_struct_name->parent.phrase_struct_tag_definition=phrase_struct_tag_definition;
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

extern unsigned int parse_type(struct_tag_definition *phrase_struct_tag_definition, sizeof_statement *phrase_sizeof_statement, alignof_statement *phrase_alignof_statement, token **curr, char *file_name){
  type *phrase_type;
  if(phrase_struct_tag_definition!=NULL){
    if((phrase_struct_tag_definition->child.phrase_type=malloc(sizeof(type)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_type=phrase_struct_tag_definition->child.phrase_type;
    phrase_type->parent_type=phrase_type_struct_tag_definition;
    phrase_type->parent.phrase_struct_tag_definition=phrase_struct_tag_definition;
  }
  else if(phrase_sizeof_statement!=NULL){
    if((phrase_sizeof_statement->child.phrase_type=malloc(sizeof(type)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_type=phrase_sizeof_statement->child.phrase_type;
    phrase_type->parent_type=phrase_type_sizeof_statement;
    phrase_type->parent.phrase_sizeof_statement=phrase_sizeof_statement;
  }
  else{
    if((phrase_alignof_statement->child.phrase_type=malloc(sizeof(type)))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      return 1;
    }
    phrase_type=phrase_alignof_statement->child.phrase_type;
    phrase_type->parent_type=phrase_type_alignof_statement;
    phrase_type->parent.phrase_alignof_statement=phrase_alignof_statement;
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

unsigned int parse_struct_tag_definition(struct_tag_definition_sequence *phrase_struct_tag_definition_sequence, token **curr, char *file_name){
  if((phrase_struct_tag_definition_sequence->phrase_struct_tag_definition=malloc(sizeof(struct_tag_definition)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  struct_tag_definition *phrase_struct_tag_definition=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition;
  phrase_struct_tag_definition->file=file_name;
  phrase_struct_tag_definition->parent=phrase_struct_tag_definition_sequence;
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

unsigned int parse_struct_tag_definition_sequence(struct_definition *phrase_struct_definition, token **curr, char *file_name){
  struct_tag_definition_sequence *phrase_struct_tag_definition_sequence;
  struct_tag_definition_sequence *parent=NULL;
  while(1){
    if(parent==NULL){
      if((phrase_struct_definition->phrase_struct_tag_definition_sequence=malloc(sizeof(struct_tag_definition_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      phrase_struct_tag_definition_sequence=phrase_struct_definition->phrase_struct_tag_definition_sequence;
      phrase_struct_tag_definition_sequence->parent_type=phrase_type_struct_definition;
      phrase_struct_tag_definition_sequence->parent.phrase_struct_definition=phrase_struct_definition;
    }
    else{
      if((parent->phrase_struct_tag_definition_sequence=malloc(sizeof(struct_tag_definition_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      phrase_struct_tag_definition_sequence=parent->phrase_struct_tag_definition_sequence;
      phrase_struct_tag_definition_sequence->parent_type=phrase_type_struct_tag_definition_sequence;
      phrase_struct_tag_definition_sequence->parent.phrase_struct_tag_definition_sequence=parent;
    }
    phrase_struct_tag_definition_sequence->file=file_name;
    phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence=NULL;
    if(parse_struct_tag_definition(phrase_struct_tag_definition_sequence,curr,file_name)){
      return 1;
    }
    if((check(*curr,tk_keyword) && (check_type(*curr,key_uc) || check_type(*curr,key_us) || check_type(*curr,key_ui) || check_type(*curr,key_c) || check_type(*curr,key_s) || check_type(*curr,key_i) || check_type(*curr,key_l) || check_type(*curr,key_p))) || (check(*curr,tk_identifier))){
      parent=phrase_struct_tag_definition_sequence;
    }
    else{
      break;
    }
  }
  return 0;
}

unsigned int parse_struct_definition(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_struct_definition=malloc(sizeof(struct_definition)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  struct_definition *phrase_struct_definition=phrase_statement->child.phrase_struct_definition;
  phrase_struct_definition->file=phrase_statement->file;
  phrase_struct_definition->parent=phrase_statement;
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
  if((check(*curr,tk_keyword) && (check_type(*curr,key_uc) || check_type(*curr,key_us) || check_type(*curr,key_ui) || check_type(*curr,key_c) || check_type(*curr,key_s) || check_type(*curr,key_i) || check_type(*curr,key_l) || check_type(*curr,key_p))) || (check(*curr,tk_identifier))){
    if(parse_struct_tag_definition_sequence(phrase_struct_definition,curr,file_name)){
      return 1;
    }
  }
  if(!(check(*curr,tk_punctuator) && match_type(curr,punc_bang))){
    write_error_from_tokens("error: expected a punctuator (!)\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  phrase_struct_definition->end_x=(*curr)->prev_token->end_x;
  phrase_struct_definition->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_tag_sequence(tagged_expression *phrase_tagged_expression, token **curr, char *file_name){
  tag_sequence *phrase_tag_sequence;
  tag_sequence *parent=NULL;
  while(1){
    if(parent==NULL){
      if((phrase_tagged_expression->phrase_tag_sequence=malloc(sizeof(tag_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      phrase_tag_sequence=phrase_tagged_expression->phrase_tag_sequence;
      phrase_tag_sequence->parent_type=phrase_type_tagged_expression;
      phrase_tag_sequence->parent.phrase_tagged_expression=phrase_tagged_expression;
    }
    else{
      if((parent->phrase_tag_sequence=malloc(sizeof(tag_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      phrase_tag_sequence=parent->phrase_tag_sequence;
      phrase_tag_sequence->parent_type=phrase_type_tag_sequence;
      phrase_tag_sequence->parent.phrase_tag_sequence=parent;
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
      parent=phrase_tag_sequence;
    }
    else{
      break;
    }
  }
  return 0;
}

unsigned int parse_alignof_statement(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_alignof_statement=malloc(sizeof(alignof_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  alignof_statement *phrase_alignof_statement=phrase_statement->child.phrase_alignof_statement;
  phrase_alignof_statement->file=phrase_statement->file;
  phrase_alignof_statement->parent=phrase_statement;
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

unsigned int parse_sizeof_statement(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_sizeof_statement=malloc(sizeof(sizeof_statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  sizeof_statement *phrase_sizeof_statement=phrase_statement->child.phrase_sizeof_statement;
  phrase_statement->child.phrase_sizeof_statement->file=phrase_statement->file;
  phrase_statement->child.phrase_sizeof_statement->parent=phrase_statement;
  phrase_statement->child.phrase_sizeof_statement->child_type=fill_incomplete;
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

unsigned int parse_tagged_expression(statement *phrase_statement, token **curr, char *file_name){
  if((phrase_statement->child.phrase_tagged_expression=malloc(sizeof(tagged_expression)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  tagged_expression *phrase_tagged_expression=phrase_statement->child.phrase_tagged_expression;
  phrase_tagged_expression->file=phrase_statement->file;
  phrase_tagged_expression->parent=phrase_statement;
  phrase_tagged_expression->phrase_tag_sequence=NULL;
  phrase_tagged_expression->start_x=(*curr)->start_x;
  phrase_tagged_expression->start_y=(*curr)->start_y;
  consume(curr);
  if(check(*curr,tk_identifier)){
    if(parse_struct_name(NULL,NULL,phrase_tagged_expression,NULL,curr,file_name)){
      return 1;
    }
  }
  else{
    write_error_from_tokens("error: expected a struct-name\n",*curr,(*curr)->prev_token,file_name);
    return 1;
  }
  if(check(*curr,tk_punctuator) && check_type(*curr,punc_dot)){
    if(parse_tag_sequence(phrase_tagged_expression,curr,file_name)){
      return 1;
    }
  }
  phrase_tagged_expression->end_x=(*curr)->prev_token->end_x;
  phrase_tagged_expression->end_y=(*curr)->prev_token->end_y;
  return 0;
}

unsigned int parse_statement(statement_sequence *phrase_statement_sequence, token **curr, char *file_name){
  if((phrase_statement_sequence->phrase_statement=malloc(sizeof(statement)))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    return 1;
  }
  statement *phrase_statement=phrase_statement_sequence->phrase_statement;
  phrase_statement->file=file_name;
  phrase_statement->parent=phrase_statement_sequence;
  phrase_statement->child_type=fill_incomplete;
  if(check(*curr,tk_punctuator) && check_type(*curr,punc_colon)){
    phrase_statement->child_type=phrase_type_label;
    if(parse_label(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check(*curr,tk_punctuator) && check_type(*curr,punc_plus)){
    phrase_statement->child_type=phrase_type_tagged_expression;
    if(parse_tagged_expression(phrase_statement,curr,file_name)){
      return 1;
    }
  }
  else if(check_type(*curr,key_pushc) || check_type(*curr,key_pushs) || check_type(*curr,key_pushi) || check_type(*curr,key_pushl) || check_type(*curr,key_pushuc) || check_type(*curr,key_pushus) || check_type(*curr,key_pushui) || check_type(*curr,key_pushp)){
    phrase_statement->child_type=phrase_type_push_statement;
    if(parse_push_statement(phrase_statement,curr,file_name)){
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

statement_sequence *parse_statement_sequence(translation_unit *phrase_translation_unit, statement_sequence *parent, token **curr, char *file_name){
  statement_sequence *phrase_statement_sequence;
  while(1){
    if(parent==NULL){
      if((phrase_translation_unit->phrase_statement_sequence=malloc(sizeof(statement_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return NULL;
      }
      phrase_statement_sequence=phrase_translation_unit->phrase_statement_sequence;
      phrase_statement_sequence->parent_type=phrase_type_translation_unit;
      phrase_statement_sequence->parent.phrase_translation_unit=phrase_translation_unit;
      phrase_statement_sequence->tu=phrase_translation_unit;
    }
    else{
      if((parent->phrase_statement_sequence=malloc(sizeof(statement_sequence)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return NULL;
      }
      phrase_statement_sequence=parent->phrase_statement_sequence;
      phrase_statement_sequence->parent_type=phrase_type_statement_sequence;
      phrase_statement_sequence->parent.phrase_statement_sequence=parent;
      phrase_statement_sequence->tu=parent->tu;
    }
    phrase_statement_sequence->file=file_name;
    phrase_statement_sequence->phrase_statement_sequence=NULL;
    if(parse_statement(phrase_statement_sequence,curr,file_name)){
      return NULL;
    }
    if((check(*curr,tk_punctuator) && check_type(*curr,punc_colon)) || (check(*curr,tk_punctuator) && check_type(*curr,punc_plus)) || (check(*curr,tk_keyword))){
      parent=phrase_statement_sequence;
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
    if(parse_statement_sequence(*phrase_translation_unit,NULL,&curr,file_name)==NULL){
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
