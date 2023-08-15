#ifndef TOC_CODE_GEN_H
#define TOC_CODE_GEN_H

#include "nvm_paths.h"
#include TOC_NVM_IMPLEMENTATION_H
#include "tables.h"
#include "tu_list.h"

#define POINTER_HEAP 0x4000000000000000
#define POINTER_DATA 0x3fffffffffffffff

extern unsigned int gen_code_push_statement(push_statement *phrase_push_statement, nightVM_l *address, str_table *string_table, sym_table *symbol_table);

extern unsigned int gen_code_instruction(instruction *phrase_instruction, nightVM_l *address);

extern unsigned int gen_code_tagged_expression(tagged_expression *phrase_tagged_expression, nightVM_l *address, struct_definition_table *struct_table);

extern unsigned int gen_code_sizeof_statement(sizeof_statement *phrase_sizeof_statement, nightVM_l *address, struct_definition_table *struct_table);

extern unsigned int gen_code_alignof_statement(alignof_statement *phrase_alignof_statement, nightVM_l *address, struct_definition_table *struct_table);

extern unsigned int gen_code_statement(statement *phrase_statement, nightVM_l *address, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries);

extern unsigned int gen_code_statement_sequence(statement_sequence *phrase_statement_sequence, nightVM_l *address, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries);

extern unsigned int gen_code_translation_unit(translation_unit *phrase_translation_unit, nightVM_l *address, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries);

extern unsigned int gen_code_translation_unit_list(translation_unit_list *tu_list, nightVM_l *address, str_table *string_table, sym_table *symbol_table, struct_definition_table *struct_table, libs *libraries);

#endif
