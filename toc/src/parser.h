#ifndef TOC_PARSER_H
#define TOC_PARSER_H

#include "tree.h"
#include "token.h"

extern unsigned int parse_instruction(statement *phrase_statement, token **curr, char *file_name);

extern unsigned int parse_sign(push_statement *phrase_push_statement, set_statement *phrase_set_statement, token **curr, char *file_name);

extern unsigned int parse_symbol(push_statement *phrase_push_statement, set_statement *phrase_set_statement, token **curr, char *file_name);

extern unsigned int parse_type(struct_tag_definition *phrase_struct_tag_definition, sizeof_statement *phrase_sizeof_statement, alignof_statement *phrase_alignof_statement, token **curr, char *file_name);

extern unsigned int parse_push_instruction(push_statement *phrase_push_statement, token **curr, char *file_name);

extern unsigned int parse_push_statement(statement *phrase_statement, token **curr, char *file_name);

extern unsigned int parse_import_statement(statement *phrase_statement, token **curr, char *file_name);

extern unsigned int parse_visibility(label *phrase_label, token **curr, char *file_name);

extern unsigned int parse_label(statement *phrase_statement, token **curr, char *file_name);

extern unsigned int parse_tag(tag_sequence *phrase_tag_sequence, token **curr, char *file_name);

extern unsigned int parse_struct_name(sizeof_statement *phrase_sizeof_statement, alignof_statement *phrase_alignof_statement, tagged_expression *phrase_tagged_expression, struct_tag_definition *phrase_struct_tag_definition, token **curr, char *file_name);

extern unsigned int parse_struct_tag_definition(struct_tag_definition_sequence *phrase_struct_tag_definition_sequence, token **curr, char *file_name);

extern unsigned int parse_struct_tag_definition_sequence(struct_definition *phrase_struct_definition, token **curr, char *file_name);

extern unsigned int parse_struct_definition(statement *phrase_statement, token **curr, char *file_name);

extern unsigned int parse_tag_sequence(tagged_expression *phrase_tagged_expression, token **curr, char *file_name);

extern unsigned int parse_alignof_statement(statement *phrase_statement, token **curr, char *file_name);

extern unsigned int parse_sizeof_statement(statement *phrase_statement, token **curr, char *file_name);

extern unsigned int parse_tagged_expression(statement *phrase_statement, token **curr, char *file_name);

extern unsigned int parse_statement(statement_sequence *phrase_statement_sequence, token **curr, char *file_name);

extern statement_sequence *parse_statement_sequence(translation_unit *phrase_translation_unit, statement_sequence *parent, token **curr, char *file_name);

extern unsigned int parse_translation_unit(translation_unit **phrase_translation_unit, token *curr, char *file_name);

extern translation_unit *parse(token *tokens, unsigned int *parse_ret, char *file_name);

#endif
