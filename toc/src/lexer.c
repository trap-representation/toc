#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

#include "token.h"
#include "comp_properties.h"
#define TOC_ADD_PUNCTUATOR
#define TOC_ADD_KEYWORD
#define TOC_ADD_NUMERIC_CONSTANT
#include "psg_idens.h"
#include "write_error.h"
#include "buf_ops.h"
#include "lexer.h"

#define MAX_TOKEN_STRING_BUF_SIZE 1048576
#define FILE_READ_CHUNK_SIZE 1048576

#define create_new_token_node \
  if(*token_head==NULL){ \
    if((*token_head=malloc(sizeof(**token_head)))==NULL){ \
      fprintf(stderr,"implementation error: failed to allocate enough memory\n"); \
      return 1; \
    } \
    *tokens_curr=*token_head; \
  } \
  else{ \
    *tokens_curr=(*tokens_curr)->next_token; \
  } \
  (*tokens_curr)->start_x=*curr_x; \
  (*tokens_curr)->start_y=*curr_y; \
  (*tokens_curr)->end_x=*curr_x; \
  (*tokens_curr)->end_y=*curr_y; \
  (*tokens_curr)->token_string_len=0; \
  (*tokens_curr)->token_string=NULL; \
  if(((*tokens_curr)->next_token=malloc(sizeof(**tokens_curr)))==NULL){ \
    fprintf(stderr,"implementation error: failed to allocate enough memory\n"); \
    return 1; \
  } \
  (*tokens_curr)->next_token->token_class=tokens_list_end; \
  (*tokens_curr)->next_token->prev_token=*tokens_curr;

static inline bool is_latin_character(char c){
  switch(c){
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
      return true;
  }
  return false;
}

static inline bool is_hexadecimal_digit(char c){
  switch(c){
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      return true;
  }
  if(c>='0' && c<='9'){
    return true;
  }
  return false;
}

static inline void transition(unsigned int *lstate, unsigned int new_state){
  *lstate=new_state;
}

static inline bool check(unsigned int lstate, unsigned state){
  if(lstate==state){
    return true;
  }
  return false;
}

static inline bool match(char c, char m){
  if(c==m){
    return true;
  }
  return false;
}

static inline void consume(size_t *c){
  (*c)++;
}

unsigned int lex_buffer(char *buf, size_t read_size, token **token_head, token **tokens_curr, unsigned int *lstate, char *token_string_buf, size_t *token_string_buf_pt, uintmax_t *curr_x, uintmax_t *curr_y, char *file_name){
  char *keywords_list[]={
    "pushc","pushs","pushi","pushp","pushl","pushuc","pushus","pushui",
    "jmp","jeq","jgt","jls","jge","jle","jne","jz","jnz",
    "addc","adds","addi","addp","addl","adduc","addus","addui",
    "subc","subs","subi","subp","subl","subuc","subus","subui",
    "mulc","muls","muli","mull","muluc","mulus","mului",
    "divc","divs","divi","divl","divuc","divus","divui",
    "remc","rems","remi","reml","remuc","remus","remui",
    "lshc","lshs","lshi","lshl","lshuc","lshus","lshui",
    "rshc","rshs","rshi","rshl","rshuc","rshus","rshui",
    "orc","ors","ori","orl","oruc","orus","orui",
    "andc","ands","andi","andl","anduc","andus","andui",
    "notc","nots","noti","notl","notuc","notus","notui",
    "xorc","xors","xori","xorl","xoruc","xorus","xorui",
    "swap","pop","dup","ret","over","panic",
    "write0","write1","write2","write3","write4","write5","write6","write7",
    "vwrite0","vwrite1","vwrite2","vwrite3","vwrite4","vwrite5","vwrite6","vwrite7",
    "get0","get1","get2","get3","get4","get5","get6","get7",
    "vget0","vget1","vget2","vget3","vget4","vget5","vget6","vget7",
    "loadc","loads","loadi","loadp","loadl","loaduc","loadus","loadui",
    "vloadc","vloads","vloadi","vloadp","vloadl","vloaduc","vloadus","vloadui",
    "aloadc","aloads","aloadi","aloadp","aloadl","aloaduc","aloadus","aloadui",
    "valoadc","valoads","valoadi","valoadp","valoadl","valoaduc","valoadus","valoadui",
    "storec","stores","storei","storep","storel","storeuc","storeus","storeui",
    "vstorec","vstores","vstorei","vstorep","vstorel","vstoreuc","vstoreus","vstoreui",
    "astorec","astores","astorei","astorep","astorel","astoreuc","astoreus","astoreui",
    "vastorec","vastores","vastorei","vastorep","vastorel","vastoreuc","vastoreus","vastoreui",
    "hlt","call","cleq","clgt","clls","clge","clle","clne","clz","clnz",
    "copy","pcopy","popa","put","pushsp",
    "set","hltr","nop",
    "alignc","aligns","aligni","alignp","alignl","alignuc","alignus","alignui",
    "incsp","decsp","exit",
    "force_panic","jmp_panic",
    "pushlt","import","hidden","exposed","struct",
    "uc","us","ui","c","s","i","l","p",
    "alignof","sizeof",
    "ceq","cgt","cls","cge","cle",
    "cne","cz","cnz",

    //toc-specific
    "open","invoke","pushpc","pushcs"
  };
  size_t last_i=1;
  uintmax_t last_x=0;
  uintmax_t last_y=0;
  for(size_t i=0;i<read_size;){
    if(last_i!=i){
      last_y=*curr_y;
      last_x=*curr_x;
      if(match(buf[i],'\n')){
        if(*curr_y==UINTMAX_MAX){
          write_error("implementation error: input size too large\n",*curr_x,*curr_y,0,0,file_name);
          return 1;
        }
        *curr_y=*curr_y+1;
        *curr_x=0;
      }
      else{
        if(*curr_x==UINTMAX_MAX){
          write_error("implementation error: input size too large\n",*curr_x,*curr_y,0,0,file_name);
          return 1;
        }
        *curr_x=*curr_x+1;
      }
      last_i=i;
    }
    if(check(*lstate,state_pending)){
      if(is_latin_character(buf[i]) || match(buf[i],'_') || match(buf[i],'^') || match(buf[i],'`') || match(buf[i],'@') || match(buf[i],'|') || match(buf[i],'\\') || match(buf[i],'[') || match(buf[i],']') || match(buf[i],',') || match(buf[i],'#') || match(buf[i],'(') || match(buf[i],')') || match(buf[i],'{') || match(buf[i],'}') || match(buf[i],'%') || match(buf[i],'&') || match(buf[i],'~') || match(buf[i],'*')){
        *token_string_buf_pt=0;
        create_new_token_node;
        (*tokens_curr)->token_class=tk_identifier;
        transition(lstate,state_identifier_or_keyword);
      }
      else if(match(buf[i],'0') || match(buf[i],'1') || match(buf[i],'2') || match(buf[i],'3') || match(buf[i],'4') || match(buf[i],'5') || match(buf[i],'6') || match(buf[i],'7') || match(buf[i],'8') || match(buf[i],'9')){
        *token_string_buf_pt=0;
        create_new_token_node;
        (*tokens_curr)->token_class=tk_numeric_constant;
        transition(lstate,state_numeric_constant);
      }
      else if(match(buf[i],'"')){
        *token_string_buf_pt=0;
        create_new_token_node;
        (*tokens_curr)->token_class=tk_string_constant;
        consume(&i);
        transition(lstate,state_character_sequence_string);
      }
      else if(match(buf[i],'\'')){
        *token_string_buf_pt=0;
        create_new_token_node;
        (*tokens_curr)->token_class=tk_character_constant;
        consume(&i);
        transition(lstate,state_character_sequence_character);
      }
      else if(match(buf[i],':') || match(buf[i],'-') || match(buf[i],'+') || match(buf[i],'$') || match(buf[i],'!') || match(buf[i],'.')){
        create_new_token_node;
        (*tokens_curr)->token_class=tk_punctuator;
        transition(lstate,state_punctuator);
      }
      else if(match(buf[i],'/')){
        consume(&i);
        transition(lstate,state_multiline_comment);
      }
      else if(match(buf[i],';')){
        consume(&i);
        transition(lstate,state_singleline_comment);
      }
      else if(match(buf[i],'\n')){
        consume(&i);
      }
      else if(match(buf[i],' ') || match(buf[i],'\t') || match(buf[i],'\f') || match(buf[i],'\n') || match(buf[i],'\r') || match(buf[i],'\v')){
        consume(&i);
      }
      else{
        write_error("error: unexpected character\n",*curr_x,*curr_y,0,0,file_name);
        return 1;
      }
    }
    else if(check(*lstate,state_multiline_comment)){
      if(match(buf[i],'/')){
        transition(lstate,state_pending);
      }
      consume(&i);
    }
    else if(check(*lstate,state_singleline_comment)){
      if(match(buf[i],'\n')){
        transition(lstate,state_pending);
      }
      consume(&i);
    }
    else if(check(*lstate,state_octal_constant_at_least_one_digit)){
      if(buf[i]>='0' && buf[i]<='7'){
        (*tokens_curr)->token_class=tk_numeric_constant;
        (*tokens_curr)->token_type=octal_constant;
        if(append_to_buf('o',token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
        transition(lstate,state_octal_constant);
      }
      else{
        *token_string_buf_pt=0;
        create_new_token_node;
        if(append_to_buf('o',token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        transition(lstate,state_identifier_or_keyword);
      }
    }
    else if(check(*lstate,state_binary_constant_at_least_one_digit)){
      (*tokens_curr)->token_class=tk_numeric_constant;
      (*tokens_curr)->token_type=binary_constant;
      if(match(buf[i],'0') || match(buf[i],'1')){
        if(append_to_buf('b',token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
        transition(lstate,state_binary_constant);
      }
      else{
        *token_string_buf_pt=0;
        create_new_token_node;
        if(append_to_buf('b',token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        transition(lstate,state_identifier_or_keyword);
      }
    }
    else if(check(*lstate,state_hexadecimal_constant_at_least_one_digit)){
      (*tokens_curr)->token_class=tk_numeric_constant;
      (*tokens_curr)->token_type=hexadecimal_constant;
      if(is_hexadecimal_digit(buf[i])){
        if(append_to_buf('x',token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
        transition(lstate,state_hexadecimal_constant);
      }
      else{
        *token_string_buf_pt=0;
        create_new_token_node;
        if(append_to_buf('x',token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        transition(lstate,state_identifier_or_keyword);
      }
    }
    else if(check(*lstate,state_octal_constant)){
      if(buf[i]>='0' && buf[i]<='7'){
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
      }
      else{
        if(flush_to_buf(token_string_buf,token_string_buf_pt,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        (*tokens_curr)->end_x=last_x;
        (*tokens_curr)->end_y=last_y;
        transition(lstate,state_pending);
      }
    }
    else if(check(*lstate,state_binary_constant)){
      if(match(buf[i],'0') || match(buf[i],'1')){
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
      }
      else{
        if(flush_to_buf(token_string_buf,token_string_buf_pt,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        (*tokens_curr)->end_x=last_x;
        (*tokens_curr)->end_y=last_y;
        transition(lstate,state_pending);
      }
    }
    else if(check(*lstate,state_hexadecimal_constant)){
      if(is_hexadecimal_digit(buf[i])){
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
      }
      else{
        if(flush_to_buf(token_string_buf,token_string_buf_pt,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        (*tokens_curr)->end_x=last_x;
        (*tokens_curr)->end_y=last_y;
        transition(lstate,state_pending);
      }
    }
    else if(check(*lstate,state_decimal_constant)){
      if(buf[i]>='0' && buf[i]<='9'){
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
      }
      else{
        if(flush_to_buf(token_string_buf,token_string_buf_pt,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        (*tokens_curr)->end_x=last_x;
        (*tokens_curr)->end_y=last_y;
        transition(lstate,state_pending);
      }
    }
    else if(check(*lstate,state_numeric_constant)){
      (*tokens_curr)->token_type=decimal_constant;
      if(buf[i]>='1' && buf[i]<='9'){
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
        transition(lstate,state_decimal_constant);
      }
      else if(match(buf[i],'0')){
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        consume(&i);
        transition(lstate,state_octal_or_hexadecimal_or_binary_or_decimal_constant);
      }
    }
    else if(check(*lstate,state_octal_or_hexadecimal_or_binary_or_decimal_constant)){
      if(match(buf[i],'x')){
        consume(&i);
        transition(lstate,state_hexadecimal_constant_at_least_one_digit);
      }
      else if(match(buf[i],'o')){
        consume(&i);
        transition(lstate,state_octal_constant_at_least_one_digit);
      }
      else if(match(buf[i],'b')){
        consume(&i);
        transition(lstate,state_binary_constant_at_least_one_digit);
      }
      else{
        if(append_to_buf('0',token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        transition(lstate,state_decimal_constant);
      }
    }
    else if(check(*lstate,state_punctuator)){
      if(match(buf[i],':')){
        (*tokens_curr)->token_type=punc_colon;
      }
      else if(match(buf[i],'-')){
        (*tokens_curr)->token_type=punc_minus;
      }
      else if(match(buf[i],'+')){
        (*tokens_curr)->token_type=punc_plus;
      }
      else if(match(buf[i],'$')){
        (*tokens_curr)->token_type=punc_dollar;
      }
      else if(match(buf[i],'!')){
        (*tokens_curr)->token_type=punc_bang;
      }
      else if(match(buf[i],'.')){
        (*tokens_curr)->token_type=punc_dot;
      }
      consume(&i);
      (*tokens_curr)->end_x=*curr_x;
      (*tokens_curr)->end_y=*curr_y;
      transition(lstate,state_pending);
    }
    else if(check(*lstate,state_escape_sequence_string)){
      char cnv_char;
      bool append=true;
      switch(buf[i]){
      case 'f':
        cnv_char='\f';
        break;
      case 'b':
        cnv_char='\b';
        break;
      case '\'':
        cnv_char='\'';
        break;
      case '"':
        cnv_char='"';
        break;
      case '\\':
        cnv_char='\\';
        break;
      case 'n':
        cnv_char='\n';
        break;
      case 'r':
        cnv_char='\r';
        break;
      case 't':
        cnv_char='\t';
        break;
      case 'v':
        cnv_char='\v';
        break;
      case 'a':
        cnv_char='\a';
        break;
      case '0':
        cnv_char='\0';
        break;
      case '\n':
        append=false;
        break;
      default:
        cnv_char=buf[i];
        break;
      }
      if(append){
        if(append_to_buf(cnv_char,token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
      }
      consume(&i);
      transition(lstate,state_character_sequence_string);
    }
    else if(check(*lstate,state_escape_sequence_character)){
      char cnv_char;
      bool append=true;
      switch(buf[i]){
      case 'f':
        cnv_char='\f';
        break;
      case 'b':
        cnv_char='\b';
        break;
      case '\'':
        cnv_char='\'';
        break;
      case '"':
        cnv_char='"';
        break;
      case '\\':
        cnv_char='\\';
        break;
      case 'n':
        cnv_char='\n';
        break;
      case 'r':
        cnv_char='\r';
        break;
      case 't':
        cnv_char='\t';
        break;
      case 'v':
        cnv_char='\v';
        break;
      case 'a':
        cnv_char='\a';
        break;
      case '0':
        cnv_char='\0';
        break;
      case '\n':
        append=false;
        break;
      default:
        cnv_char=buf[i];
        break;
      }
      if(append){
        if(append_to_buf(cnv_char,token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
      }
      consume(&i);
      transition(lstate,state_character_sequence_character);
    }
    else if(check(*lstate,state_character_sequence_string)){
      if(match(buf[i],'"')){
        if(flush_to_buf(token_string_buf,token_string_buf_pt,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        (*tokens_curr)->end_x=*curr_x;
        (*tokens_curr)->end_y=*curr_y;
        transition(lstate,state_pending);
      }
      else if(match(buf[i],'\\')){
        transition(lstate,state_escape_sequence_string);
      }
      else if(match(buf[i],'\n')){
        write_error("error: character-string cannot be the newline character\n",last_x+1,last_y,0,0,file_name);
        return 1;
      }
      else if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
        return 1;
      }
      consume(&i);
    }
    else if(check(*lstate,state_character_sequence_character)){
      if(match(buf[i],'\'')){
        if(flush_to_buf(token_string_buf,token_string_buf_pt,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
          return 1;
        }
        (*tokens_curr)->end_x=*curr_x;
        (*tokens_curr)->end_y=*curr_y;
        transition(lstate,state_pending);
      }
      else if(match(buf[i],'\\')){
        transition(lstate,state_escape_sequence_character);
      }
      else if(match(buf[i],'\n')){
        write_error("error: character-char cannot be the newline character\n",last_x+1,last_y,0,0,file_name);
        return 1;
      }
      else if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,true,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
        return 1;
      }
      consume(&i);
    }
    else if(check(*lstate,state_identifier_or_keyword)){
      if(is_latin_character(buf[i]) || match(buf[i],'_') || match(buf[i],'^') || match(buf[i],'`') || match(buf[i],'@') || match(buf[i],'|') || match(buf[i],'\\') || match(buf[i],'[') || match(buf[i],']') || match(buf[i],'(') || match(buf[i],')') || match(buf[i],',') || match(buf[i],'#') || match(buf[i],'{') || match(buf[i],'}') || match(buf[i],'%') || match(buf[i],'&') || match(buf[i],'~') || match(buf[i],'*') || (buf[i]>='0' && buf[i]<='9')){
        if(append_to_buf(buf[i],token_string_buf,token_string_buf_pt,MAX_TOKEN_STRING_BUF_SIZE,false,NULL,NULL)){
          return 1;
        }
        consume(&i);
      }
      else{
        bool is_key=false;
        for(size_t j=0;j<sizeof(keywords_list)/sizeof(keywords_list[0]);j++){
          if(*token_string_buf_pt==strlen(keywords_list[j]) && strncmp(token_string_buf,keywords_list[j],*token_string_buf_pt)==0){
            (*tokens_curr)->token_class=tk_keyword;
            (*tokens_curr)->token_type=j;
            is_key=true;
            break;
          }
        }
        if(!is_key){
          if(flush_to_buf(token_string_buf,token_string_buf_pt,&(*tokens_curr)->token_string,&(*tokens_curr)->token_string_len)){
            return 1;
          }
        }
        (*tokens_curr)->end_x=last_x;
        (*tokens_curr)->end_y=last_y;
        transition(lstate,state_pending);
      }
    }
  }
  return 0;
}

token* lex(char *file_name, unsigned int *lex_ret){
  FILE *lex_file=fopen(file_name,"r");
  if(lex_file==NULL){
    fprintf(stderr,"implementation error: failed to open file %s for reading\n",file_name);
    *lex_ret=1;
    return NULL;
  }
  char *read_buf;
  if((read_buf=malloc(FILE_READ_CHUNK_SIZE))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    fclose(lex_file);
    *lex_ret=1;
    return NULL;
  }
  char *token_string_buf;
  if((token_string_buf=malloc(MAX_TOKEN_STRING_BUF_SIZE))==NULL){
    fprintf(stderr,"implementation error: failed to allocate enough memory\n");
    fclose(lex_file);
    free(read_buf);
    *lex_ret=1;
    return NULL;
  }
  size_t read_size;
  size_t token_string_buf_pt;
  token *token_head=NULL;
  token *tokens_curr=NULL;
  uintmax_t curr_x=0;
  uintmax_t curr_y=1;
  unsigned int lstate=state_pending;
  while((read_size=fread(read_buf,1,FILE_READ_CHUNK_SIZE,lex_file))){
    if(ferror(lex_file)){
      fprintf(stderr,"implementation error: error while reading from file %s\n",file_name);
      fclose(lex_file);
      free(read_buf);
      free(token_string_buf);
      *lex_ret=1;
      return token_head;
    }
    if(lex_buffer(read_buf,read_size,&token_head,&tokens_curr,&lstate,token_string_buf,&token_string_buf_pt,&curr_x,&curr_y,file_name)){
      fclose(lex_file);
      free(read_buf);
      free(token_string_buf);
      *lex_ret=1;
      return token_head;
    }
  }
  if(ferror(lex_file)){
    fprintf(stderr,"implementation error: error while reading from file %s\n",file_name);
    fclose(lex_file);
    free(read_buf);
    free(token_string_buf);
    *lex_ret=1;
    return token_head;
  }
  fclose(lex_file);
  free(read_buf);
  free(token_string_buf);
  if(lstate!=state_pending){
    fprintf(stderr,"error: source file ends in a partial token\n");
    *lex_ret=1;
    return token_head;
  }
  *lex_ret=0;
  return token_head;
}
