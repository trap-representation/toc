#ifndef TOC_LEXER_H
#define TOC_LEXER_H

//lexer states
enum state {
  state_pending,
  state_identifier_or_keyword,
  state_character_sequence_string,
  state_punctuator,
  state_character_sequence_character,
  state_decimal_constant,
  state_hexadecimal_constant,
  state_binary_constant,
  state_octal_constant,
  state_escape_sequence_string,
  state_escape_sequence_character,
  state_multiline_comment,
  state_singleline_comment,
  state_hexadecimal_constant_at_least_one_digit,
  state_binary_constant_at_least_one_digit,
  state_octal_constant_at_least_one_digit,
  state_numeric_constant,
  state_octal_or_hexadecimal_or_binary_or_decimal_constant
};

extern unsigned int lex_buffer(char *buf, size_t read_size, token **token_head, token **tokens_curr, enum state *lstate, char *token_string_buf, size_t *token_string_buf_pt, uintmax_t *curr_x, uintmax_t *curr_y, char *file_name);

extern token* lex(char *file_name, unsigned int *lex_ret);

#endif
