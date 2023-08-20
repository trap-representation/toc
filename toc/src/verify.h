#ifndef TOC_VERIFY_H
#define TOC_VERIFY_H

#include "tables.h"
#include "tree.h"

extern compilation_attributes comp_attr;

extern bool tagged_statement_constructs_ok(struct_definition_table *struct_table, translation_unit *tu);

extern bool no_dup_tag_in_struct_tag_definition_sequence(struct_definition_table *struct_table);

extern bool no_use_of_unset_symbols_in_push_statements(sym_table *symbol_table, translation_unit *tu);

extern bool alignof_statements_ok(struct_definition_table *struct_table, translation_unit *tu);

extern bool sizeof_statements_ok(struct_definition_table *struct_table, translation_unit *tu);

extern bool entrypoint_exists(sym_table *symbol_table);

#endif
