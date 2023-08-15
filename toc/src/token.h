#ifndef TOC_TOKEN_H
#define TOC_TOKEN_H

// token classes
#define tk_keyword 0
#define tk_identifier 1
#define tk_character_constant 2
#define tk_string_constant 3
#define tk_numeric_constant 4
#define tk_punctuator 5
#define tokens_list_end 6

typedef struct s_token {
  unsigned int token_class;
  unsigned int token_type;
  uintmax_t start_y;
  uintmax_t start_x;
  uintmax_t end_x;
  uintmax_t end_y;
  char *token_string;
  size_t token_string_len;
  struct s_token *prev_token;
  struct s_token *next_token;
} token;

#endif
