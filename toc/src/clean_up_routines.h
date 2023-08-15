#ifndef TOC_CLEAN_UP_ROUTINES_H
#define TOC_CLEAN_UP_ROUTINES_H

#include "tree.h"
#include "token.h"
#include "tables.h"
#include "libs.h"
#include "tu_list.h"

extern void clean_up_string_table(str_table *string_table);

extern void clean_up_symbol_table(sym_table *symbol_table);

extern void clean_up_sign(sign *phrase_sign);

extern void clean_up_visibility(visibility *phrase_visibility);

extern void clean_up_phrase_struct_name(struct_name *phrase_struct_name);

extern void clean_up_push_instruction(push_instruction *phrase_push_instruction);

extern void clean_up_instruction(instruction *phrase_instruction);

extern void clean_up_tag(tag *phrase_tag);

extern void clean_up_symbol(symbol *phrase_symbol);

extern void clean_up_phrase_type(type *phrase_type);

extern void clean_up_struct_definition_table(struct_definition_table *struct_table);

extern void clean_up_libraries(libs *libraries);

extern void clean_up_tokens_list(token *tokens);

extern void clean_up_push_statement(push_statement *phrase_push_statement);

extern void clean_up_import_statement(import_statement *phrase_import_statement);

extern void clean_up_label(label *phrase_label);

extern void clean_up_set_statement(set_statement *phrase_set_statement);

extern void clean_up_tag_sequence(tag_sequence *phrase_tag_sequence);

extern void clean_up_tagged_expression(tagged_expression *phrase_tagged_expression);

extern void clean_up_struct_tag_definition(struct_tag_definition *phrase_struct_tag_definition);

extern void clean_up_struct_tag_definition_sequence(struct_tag_definition_sequence *phrase_struct_tag_definition_sequence);

extern void clean_up_struct_definition(struct_definition *phrase_struct_definition);

extern void clean_up_sizeof_statement(sizeof_statement *phrase_sizeof_statement);

extern void clean_up_alignof_statement(alignof_statement *phrase_alignof_statement);

extern void clean_up_statement(statement *phrase_statement);

extern void clean_up_statement_sequence(statement_sequence *phrase_statement_sequence);

extern void clean_up_translation_unit(translation_unit *phrase_translation_unit);

extern void clean_up_translation_unit_list(translation_unit_list *tu_list);

#endif
