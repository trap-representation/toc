#ifndef TOC_TABLES_H
#define TOC_TABLES_H

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H
#include "tree.h"
#include "libs.h"
#include "conv.h"

#define ret_any 0
#define ret_size 1
#define ret_align 2

#define sym_label 0
#define sym_nonlabel 1

typedef struct s_str_table {
  ysm_c *string;
  ysm_l address;
  struct s_str_table *next_node;
} str_table;

typedef struct s_sym_table {
  char *symbol_identifier;
  unsigned int visibility_type;
  ysm_l val;
  translation_unit *tu;
  struct s_sym_table *next_node;
} sym_table;

typedef struct s_struct_definition_table {
  char *struct_name_identifier;
  struct_definition *address;
  ysm_l size;
  ysm_ui alignment;
  translation_unit *tu;
  struct s_struct_definition_table *next_node;
} struct_definition_table;

extern str_table *create_str_table(str_table **last_string_table_node, str_table *e_string_table, translation_unit *tu, unsigned int *ret);

extern ysm_l lookup_in_string_table(char *s, str_table *string_table);

extern ysm_l lookup_in_symbol_table(char *s, sym_table *symbol_table, translation_unit *tu, bool stOP_EARLY, bool *found);

extern sym_table *create_sym_table(sym_table **last_symbol_table_node, sym_table *e_symbol_table, translation_unit *tu, str_table *string_table, libs *libraries, ysm_l *address, unsigned int *ret);

extern ysm_l lookup_in_struct_definition_table(char *s, struct_definition_table *struct_table, translation_unit *tu, bool stOP_EARLY, struct_definition **phrase_struct_definition, unsigned int ret_type);

extern struct_definition_table* create_struct_definition_table(struct_definition_table **last_struct_table_node, struct_definition_table *e_struct_table, translation_unit *tu, unsigned int *ret);

#endif
