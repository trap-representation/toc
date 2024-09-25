#ifndef TOC_CONV_H
#define TOC_CONV_H

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H

static ysm_l int_power(ysm_l i, ysm_l e){
  ysm_l ret=1;
  for(ysm_l j=0;j<e;j++){
    ret*=i;
  }
  return ret;
}

static unsigned int get_hexadecimal_digit_val(char c){
  switch(c){
    case 'a': case 'A':
      return 10;
    case 'b': case 'B':
      return 11;
    case 'c': case 'C':
      return 12;
    case 'd': case 'D':
      return 13;
    case 'e': case 'E':
      return 14;
    case 'f': case 'F':
      return 15;
  }
  return c-'0';
}

static ysm_uc numeric_constant_to_uc(char *numeric_constant){
  ysm_uc n=0;
  if(numeric_constant[0]=='0' && numeric_constant[1]=='x'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=get_hexadecimal_digit_val(numeric_constant[i])*int_power(16,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='b'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(2,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='o'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(8,(c_len-1)-i);
    }
  }
  else{
    for(size_t i=0;numeric_constant[i]!='\0';i++){
      n*=10;
      n+=numeric_constant[i]-'0';
    }
  }
  return n;
}

static ysm_us numeric_constant_to_us(char *numeric_constant){
  ysm_us n=0;
  if(numeric_constant[0]=='0' && numeric_constant[1]=='x'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=get_hexadecimal_digit_val(numeric_constant[i])*int_power(16,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='b'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(2,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='o'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(8,(c_len-1)-i);
    }
  }
  else{
    for(size_t i=0;numeric_constant[i]!='\0';i++){
      n*=10;
      n+=numeric_constant[i]-'0';
    }
  }
  return n;
}

static ysm_ui numeric_constant_to_ui(char *numeric_constant){
  ysm_ui n=0;
  if(numeric_constant[0]=='0' && numeric_constant[1]=='x'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=get_hexadecimal_digit_val(numeric_constant[i])*int_power(16,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='b'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(2,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='o'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(8,(c_len-1)-i);
    }
  }
  else{
    for(size_t i=0;numeric_constant[i]!='\0';i++){
      n*=10;
      n+=numeric_constant[i]-'0';
    }
  }
  return n;
}

static ysm_c numeric_constant_to_c(char *numeric_constant){
  ysm_c n=0;
  if(numeric_constant[0]=='0' && numeric_constant[1]=='x'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=get_hexadecimal_digit_val(numeric_constant[i])*int_power(16,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='b'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(2,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='o'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(8,(c_len-1)-i);
    }
  }
  else{
    for(size_t i=0;numeric_constant[i]!='\0';i++){
      n*=10;
      n+=numeric_constant[i]-'0';
    }
  }
  return n;
}

static ysm_s numeric_constant_to_s(char *numeric_constant){
  ysm_s n=0;
  if(numeric_constant[0]=='0' && numeric_constant[1]=='x'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=get_hexadecimal_digit_val(numeric_constant[i])*int_power(16,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='b'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(2,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='o'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(8,(c_len-1)-i);
    }
  }
  else{
    for(size_t i=0;numeric_constant[i]!='\0';i++){
      n*=10;
      n+=numeric_constant[i]-'0';
    }
  }
  return n;
}

static ysm_i numeric_constant_to_i(char *numeric_constant){
  ysm_i n=0;
  if(numeric_constant[0]=='0' && numeric_constant[1]=='x'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=get_hexadecimal_digit_val(numeric_constant[i])*int_power(16,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='b'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(2,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='o'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(8,(c_len-1)-i);
    }
  }
  else{
    for(size_t i=0;numeric_constant[i]!='\0';i++){
      n*=10;
      n+=numeric_constant[i]-'0';
    }
  }
  return n;
}

static ysm_l numeric_constant_to_l(char *numeric_constant){
  ysm_l n=0;
  if(numeric_constant[0]=='0' && numeric_constant[1]=='x'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=get_hexadecimal_digit_val(numeric_constant[i])*int_power(16,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='b'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(2,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='o'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(8,(c_len-1)-i);
    }
  }
  else{
    for(size_t i=0;numeric_constant[i]!='\0';i++){
      n*=10;
      n+=numeric_constant[i]-'0';
    }
  }
  return n;
}

static ysm_l numeric_constant_to_p(char *numeric_constant){
  ysm_l n=0;
  if(numeric_constant[0]=='0' && numeric_constant[1]=='x'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=get_hexadecimal_digit_val(numeric_constant[i])*int_power(16,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='b'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(2,(c_len-1)-i);
    }
  }
  else if(numeric_constant[0]=='0' && numeric_constant[1]=='o'){
    size_t c_len=strlen(numeric_constant);
    for(size_t i=2;i<c_len;i++){
      n+=(numeric_constant[i]-'0')*int_power(8,(c_len-1)-i);
    }
  }
  else{
    for(size_t i=0;numeric_constant[i]!='\0';i++){
      n*=10;
      n+=numeric_constant[i]-'0';
    }
  }
  return n;
}

static unsigned int get_hexadecimal_digit_val_checked(char c){
  switch(c){
    case 'a': case 'A':
      return 10;
    case 'b': case 'B':
      return 11;
    case 'c': case 'C':
      return 12;
    case 'd': case 'D':
      return 13;
    case 'e': case 'E':
      return 14;
    case 'f': case 'F':
      return 15;
  }
  if(c>='0' && c<='9'){
    return c-'0';
  }
  return 16;
}

static nightvm_uns uint_power(ysm_l i, ysm_l e){
  ysm_l ret=1;
  for(ysm_l j=0;j<e;j++){
    ret*=i;
  }
  return ret;
}

static nightvm_uns str_to_uns(char *s, unsigned int *ret){
  *ret=0;
  nightvm_uns n=0;
  if(s[0]=='0' && s[1]=='x'){
    s=&s[2];
    size_t c_len=strlen(s);
    for(size_t i=0;i<c_len;i++){
      unsigned int hval;
      if((hval=get_hexadecimal_digit_val_checked(s[i]))>15){
        n+=(s[i]-'0')*uint_power(16,(c_len-1)-i);
      }
      else{
        *ret=1;
        return 0;
      }
    }
  }
  else if(s[0]=='0' && s[1]=='b'){
    s=&s[2];
    size_t c_len=strlen(s);
    for(size_t i=0;i<c_len;i++){
      if(s[i]=='0' || s[i]=='1'){
        n+=(s[i]-'0')*uint_power(2,(c_len-1)-i);
      }
      else{
        *ret=1;
        return 0;
      }
    }
  }
  else if(s[0]=='0' && s[1]=='o'){
    s=&s[2];
    size_t c_len=strlen(s);
    for(size_t i=0;i<c_len;i++){
      if(s[i]>='0' && s[i]<='7'){
        n+=(s[i]-'0')*uint_power(8,(c_len-1)-i);
      }
      else{
        *ret=1;
        return 0;
      }
    }
  }
  else{
    for(size_t i=0;s[i]!='\0';i++){
      if(s[i]>='0' && s[i]<='9'){
        n*=10;
        n+=s[i]-'0';
      }
      else{
        *ret=1;
        return 0;
      }
    }
  }
  return n;
}

static ysm_ui str_to_ui(char *s, unsigned int *ret){
  *ret=0;
  ysm_ui n=0;
  if(s[0]=='0' && s[1]=='x'){
    s=&s[2];
    size_t c_len=strlen(s);
    for(size_t i=0;i<c_len;i++){
      unsigned int hval;
      if((hval=get_hexadecimal_digit_val_checked(s[i]))>15){
        n+=(s[i]-'0')*uint_power(16,(c_len-1)-i);
      }
      else{
        *ret=1;
        return 0;
      }
    }
  }
  else if(s[0]=='0' && s[1]=='b'){
    s=&s[2];
    size_t c_len=strlen(s);
    for(size_t i=0;i<c_len;i++){
      if(s[i]=='0' || s[i]=='1'){
        n+=(s[i]-'0')*uint_power(2,(c_len-1)-i);
      }
      else{
        *ret=1;
        return 0;
      }
    }
  }
  else if(s[0]=='0' && s[1]=='o'){
    s=&s[2];
    size_t c_len=strlen(s);
    for(size_t i=0;i<c_len;i++){
      if(s[i]>='0' && s[i]<='7'){
        n+=(s[i]-'0')*uint_power(8,(c_len-1)-i);
      }
      else{
        *ret=1;
        return 0;
      }
    }
  }
  else{
    for(size_t i=0;s[i]!='\0';i++){
      if(s[i]>='0' && s[i]<='9'){
        n*=10;
        n+=s[i]-'0';
      }
      else{
        *ret=1;
        return 0;
      }
    }
  }
  return n;
}

#endif
