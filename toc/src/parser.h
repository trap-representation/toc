#ifndef TOC_PARSER_H
#define TOC_PARSER_H

#include "tree.h"
#include "token.h"

extern unsigned int parse_instruction(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_sign(push_statement *parent_phrase_push_statement, set_statement *parent_phrase_set_statement, token **curr, char *file_name);

extern unsigned int parse_symbol(push_statement *parent_phrase_push_statement, set_statement *parent_phrase_set_statement, token **curr, char *file_name);

extern unsigned int parse_type(struct_tag_definition *parent_phrase_struct_tag_definition, sizeof_statement *parent_phrase_sizeof_statement, alignof_statement *parent_phrase_alignof_statement, token **curr, char *file_name);

extern unsigned int parse_push_instruction(push_statement *parent_phrase_push_statement, token **curr, char *file_name);

extern unsigned int parse_push_statement(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_import_statement(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_visibility(label *parent_phrase_label, token **curr, char *file_name);

extern unsigned int parse_label(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_tag(tag_sequence *parent_phrase_tag_sequence, token **curr, char *file_name);

extern unsigned int parse_struct_name(sizeof_statement *parent_phrase_sizeof_statement, alignof_statement *parent_phrase_alignof_statement, tagged_statement *parent_phrase_tagged_statement, struct_tag_definition *parent_phrase_struct_tag_definition, token **curr, char *file_name);

extern unsigned int parse_struct_tag_definition(struct_tag_definition_sequence *parent_phrase_struct_tag_definition_sequence, token **curr, char *file_name);

extern unsigned int parse_struct_tag_definition_sequence(struct_definition *parent_phrase_struct_definition, token **curr, char *file_name);

extern unsigned int parse_struct_definition(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_tag_sequence(tagged_statement *parent_phrase_tagged_statement, token **curr, char *file_name);

extern unsigned int parse_alignof_statement(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_sizeof_statement(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_tagged_statement(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_else_clause(selection_statement *parent_phrase_selection_statement, token **curr, char *file_name);

extern unsigned int parse_selection_statement(statement *parent_phrase_statement, token **curr, char *file_name);

extern unsigned int parse_statement(statement_sequence *parent_phrase_statement_sequence, selection_statement *parent_phrase_selection_statement, else_clause *parent_phrase_else_clause, token **curr, char *file_name);

extern statement_sequence *parse_statement_sequence(translation_unit *parent_phrase_translation_unit, selection_statement *parent_phrase_selection_statement_0, selection_statement *parent_phrase_selection_statement_1, else_clause *parent_phrase_else_clause, statement_sequence *parent_phrase_statement_sequence, token **curr, char *file_name);

extern unsigned int parse_translation_unit(translation_unit **parent_phrase_translation_unit, token *curr, char *file_name);

extern translation_unit *parse(token *tokens, unsigned int *parse_ret, char *file_name);

#endif
