#ifndef TOC_TABLES_H
#define TOC_TABLES_H

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H
#include "tree.h"
#include "libs.h"

#define ret_any 0
#define ret_size 1
#define ret_align 2

#define sym_label 0
#define sym_nonlabel 1

typedef struct s_str_table {
  nightVM_c *string;
  nightVM_l ptr;
  struct s_str_table *next_node;
} str_table;

typedef struct s_sym_table {
  char *symbol_identifier;
  unsigned int visibility_code;
  unsigned int sym_type;
  bool val_filled;
  union sym_table_loc_union {
    label *label_loc;
    set_statement *set_statement_loc;
  } tu_loc;
  nightVM_l val;
  translation_unit *tu;
  struct s_sym_table *next_node;
} sym_table;

typedef struct s_struct_definition_table {
  char *struct_name_identifier;
  struct_definition *loc;
  nightVM_l size;
  nightVM_ui alignment;
  translation_unit *tu;
  struct s_struct_definition_table *next_node;
} struct_definition_table;

extern nightVM_uc numeric_constant_to_uc(char *numeric_constant);

extern nightVM_us numeric_constant_to_us(char *numeric_constant);

extern nightVM_ui numeric_constant_to_ui(char *numeric_constant);

extern nightVM_c numeric_constant_to_c(char *numeric_constant);

extern nightVM_s numeric_constant_to_s(char *numeric_constant);

extern nightVM_i numeric_constant_to_i(char *numeric_constant);

extern nightVM_l numeric_constant_to_l(char *numeric_constant);

extern nightVM_l numeric_constant_to_p(char *numeric_constant);

extern str_table* create_str_table(str_table **last_string_table_node, str_table *e_str_table, translation_unit *tu, unsigned int *ret);

extern nightVM_l lookup_in_string_table(char *s, str_table *string_table);

extern nightVM_l lookup_in_symbol_table(char *s, sym_table *symbol_table, label **ret_label, set_statement **ret_set_statement, translation_unit *tu, bool stop_early);

extern sym_table *create_sym_table_and_calculate_addresses(sym_table **last_symbol_table_node, sym_table *e_symbol_table, translation_unit *tu, str_table *string_table, libs *libraries, nightVM_l *address, unsigned int *ret);

extern nightVM_l lookup_in_struct_definition_table(char *s, struct_definition_table *struct_table, translation_unit *tu, bool stop_early, struct_definition **phrase_struct_definition, unsigned int ret_type);

extern struct_definition_table* create_struct_definition_table(struct_definition_table **last_struct_table_node, struct_definition_table *e_struct_table, translation_unit *tu, unsigned int *ret);

#endif
