#ifndef TOC_LEXER_H
#define TOC_LEXER_H

// lexer states
#define state_pending 0
#define state_identifier_or_keyword 1
#define state_character_sequence_string 2
#define state_punctuator 3
#define state_character_sequence_character 4
#define state_decimal_constant 5
#define state_hexadecimal_constant 6
#define state_binary_constant 7
#define state_octal_constant 8
#define state_escape_sequence_string 9
#define state_escape_sequence_character 10
#define state_multiline_comment 11
#define state_singleline_comment 12
#define state_hexadecimal_constant_at_least_one_digit 13
#define state_binary_constant_at_least_one_digit 14
#define state_octal_constant_at_least_one_digit 15
#define state_numeric_constant 16
#define state_octal_or_hexadecimal_or_binary_or_decimal_constant 17

extern unsigned int lex_buffer(char *buf, size_t read_size, token **token_head, token **tokens_curr, unsigned int *lstate, char *token_string_buf, size_t *token_string_buf_pt, uintmax_t *curr_x, uintmax_t *curr_y, char *file_name);

extern token* lex(char *file_name, unsigned int *lex_ret);

#endif
