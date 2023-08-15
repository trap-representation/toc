#ifndef TOC_VERIFY_H
#define TOC_VERIFY_H

#include "tables.h"
#include "tree.h"

extern compilation_attributes comp_attr;

extern bool is_tagged_expression_construct_ok(translation_unit *tu, tagged_expression *phrase_tagged_expression, struct_definition_table *struct_table);

extern bool tagged_expression_constructs_ok(translation_unit *tu, struct_definition_table *struct_table);

extern bool no_dup_tag_in_struct_tag_definition_sequence(struct_definition_table *struct_table);

extern bool no_use_of_unset_symbol_in_push_statements(translation_unit *tu, sym_table *symbol_table);

extern bool alignof_statements_ok(translation_unit *tu, struct_definition_table *struct_table);

extern bool sizeof_statements_ok(translation_unit *tu, struct_definition_table *struct_table);

extern bool entrypoint_exists(sym_table *symbol_table);

#endif
