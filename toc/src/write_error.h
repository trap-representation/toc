#ifndef TOC_WRITE_ERROR_H
#define TOC_WRITE_ERROR_H

#include "token.h"

extern void write_error_from_tokens(char *err, token *tok_pri, token *tok_sec, char *file_name);

extern void write_error(char *err, uintmax_t start_x, uintmax_t start_y, uintmax_t end_x, uintmax_t end_y, char *file_name);

#endif
