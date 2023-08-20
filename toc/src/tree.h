#ifndef TOC_TREE_H
#define TOC_TREE_H

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H

typedef struct s_sizeof_statement {
  unsigned int child_type;
  union sizeof_statement_child_type_union {
    #define phrase_type_type 0
    struct s_type *phrase_type;
    #define phrase_type_struct_name 1
    struct s_struct_name *phrase_struct_name;
  } child;
  struct s_statement *parent;
  nightVM_ui align_pad;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} sizeof_statement;

typedef struct s_else_clause {
  unsigned int child_type;
  union else_clause_child_type_union {
    #define phrase_type_statement 2
    struct s_statement *phrase_statement;
    struct s_statement_sequence *phrase_statement_sequence;
  } child;
  struct s_selection_statement *parent;
  nightVM_ui align_pad;
  nightVM_l rend_address;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} else_clause;

typedef struct s_selection_statement {
  struct s_statement_sequence *phrase_statement_sequence;
  unsigned int child_type;
  union selection_statement_child_type_union {
    struct s_statement *phrase_statement;
    struct s_statement_sequence *phrase_statement_sequence;
  } child;
  struct s_else_clause *phrase_else_clause;
  struct s_statement *parent;
  nightVM_ui align_pad;
  nightVM_l rend_address;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} selection_statement;

typedef struct s_alignof_statement {
  unsigned int child_type;
  union alignof_statement_child_type_union {
    struct s_type *phrase_type;
    struct s_struct_name *phrase_struct_name;
  } child;
  struct s_statement *parent;
  nightVM_ui align_pad;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} alignof_statement;

typedef struct s_push_statement {
  struct s_push_instruction *phrase_push_instruction;
  struct s_sign *phrase_sign;
  unsigned int child_type;
  union push_statement_child_type_union {
    #define lex_type_numeric_constant 3
    char *lex_numeric_constant;
    #define lex_type_string_constant 4
    char *lex_string_constant;
    #define lex_type_character_constant 5
    char *lex_character_constant;
    #define phrase_type_symbol 6
    struct s_symbol *phrase_symbol;
  } child;
  struct s_statement *parent;
  nightVM_ui align_pad;
  nightVM_l address;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} push_statement;

typedef struct s_label {
  char *lex_identifier;
  struct s_visibility *phrase_visibility;
  struct s_statement *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} label;

typedef struct s_set_statement {
  char *lex_identifier;
  struct s_sign *phrase_sign;
  unsigned int child_type;
  union set_statement_child_type_union {
    char *lex_numeric_constant;
    char *lex_character_constant;
    char *lex_string_constant;
    struct s_symbol *phrase_symbol;
  } child;
  struct s_statement *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} set_statement;

typedef struct s_import_statement {
  char *lex_string_constant;
  struct s_statement *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} import_statement;

typedef struct s_tag_sequence {
  struct s_tag *phrase_tag;
  struct s_tag_sequence *phrase_tag_sequence;
  unsigned int parent_type;
  union tag_sequence_parent_type_union {
    #define phrase_type_tagged_statement 7
    struct s_tagged_statement *phrase_tagged_statement;
    #define phrase_type_tag_sequence 8
    struct s_tag_sequence *phrase_tag_sequence;
  } parent;
  char *file;
} tag_sequence;

typedef struct s_tagged_statement {
  struct s_struct_name *phrase_struct_name;
  struct s_tag_sequence *phrase_tag_sequence;
  struct s_statement *parent;
  nightVM_ui align_pad;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} tagged_statement;

typedef struct s_struct_tag_definition {
  unsigned int child_type;
  union struct_tag_definition_child_type_union {
    struct s_type *phrase_type;
    struct s_struct_name *phrase_struct_name;
  } child;
  char *lex_identifier;
  struct s_struct_tag_definition_sequence *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} struct_tag_definition;

typedef struct s_struct_tag_definition_sequence {
  struct s_struct_tag_definition *phrase_struct_tag_definition;
  struct s_struct_tag_definition_sequence *phrase_struct_tag_definition_sequence;
  unsigned int parent_type;
  union struct_tag_definition_sequence_parent_union {
    #define phrase_type_struct_definition 9
    struct s_struct_definition *phrase_struct_definition;
    #define phrase_type_struct_tag_definition_sequence 10
    struct s_struct_tag_definition_sequence *phrase_struct_tag_definition_sequence;
  } parent;
  char *file;
} struct_tag_definition_sequence;

typedef struct s_struct_definition {
  char *lex_identifier;
  struct s_struct_tag_definition_sequence *phrase_struct_tag_definition_sequence;
  struct s_statement *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} struct_definition;

typedef struct s_statement {
  unsigned int child_type;
  union statement_child_type_union {
    #define phrase_type_push_statement 11
    struct s_push_statement *phrase_push_statement;
    #define phrase_type_set_statement 12
    struct s_set_statement *phrase_set_statement;
    #define phrase_type_alignof_statement 13
    struct s_alignof_statement *phrase_alignof_statement;
    #define phrase_type_sizeof_statement 14
    struct s_sizeof_statement *phrase_sizeof_statement;
    #define phrase_type_label 15
    struct s_label *phrase_label;
    #define phrase_type_import_statement 16
    struct s_import_statement *phrase_import_statement;
    struct_definition *phrase_struct_definition;
    struct s_tagged_statement *phrase_tagged_statement;
    #define phrase_type_instruction 17
    struct s_instruction *phrase_instruction;
    #define phrase_type_selection_statement 18
    struct s_selection_statement *phrase_selection_statement;
    #define phrase_type_else_clause 19
    struct s_selection_statement *phrase_else_clause;
  } child;
  unsigned int parent_type;
  union statement_parent_type_union {
    struct s_statement_sequence *phrase_statement_sequence;
    struct s_selection_statement *phrase_selection_statement;
    struct s_else_clause *phrase_else_clause;
  } parent;
  struct s_translation_unit *tu;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} statement;

typedef struct s_statement_sequence {
  struct s_statement *phrase_statement;
  struct s_statement_sequence *phrase_statement_sequence;
  unsigned int parent_type;
  union statement_sequence_parent_type_union {
    #define phrase_type_translation_unit 20
    struct s_translation_unit *phrase_translation_unit;
    #define phrase_type_statement_sequence 21
    struct s_statement_sequence *phrase_statement_sequence;
    struct s_selection_statement *phrase_selection_statement;
    struct s_else_clause *phrase_else_clause;
  } parent;
  struct s_translation_unit *tu;
  char *file;
} statement_sequence;

typedef struct s_type {
  unsigned int type_code;
  unsigned int parent_type;
  union type_parent_type_union {
    struct s_struct_tag_definition *phrase_struct_tag_definition;
    struct s_sizeof_statement *phrase_sizeof_statement;
    struct s_alignof_statement *phrase_alignof_statement;
  } parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} type;

typedef struct s_struct_name {
  char *lex_identifier;
  unsigned int parent_type;
  union struct_name_parent_type_union {
    struct s_sizeof_statement *phrase_sizeof_statement;
    struct s_alignof_statement *phrase_alignof_statement;
    struct s_tagged_statement *phrase_tagged_statement;
    #define phrase_type_struct_tag_definition 22
    struct s_struct_tag_definition *phrase_struct_tag_definition;
  } parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} struct_name;

typedef struct s_sign {
  int sign_code;
  unsigned int parent_type;
  union sign_parent_type_union {
    struct s_push_statement *phrase_push_statement;
    struct s_set_statement *phrase_set_statement;
  } parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} sign;

typedef struct s_instruction {
  unsigned int instruction_code;
  struct s_statement *parent;
  nightVM_ui align_pad;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} instruction;

typedef struct s_symbol {
  char *lex_identifier;
  unsigned int parent_type;
  union symbol_parent_type_union {
    struct s_push_statement *phrase_push_statement;
    struct s_set_statement *phrase_set_statement;
  } parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} symbol;

typedef struct s_tag {
  char *lex_identifier;
  struct s_tag_sequence *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} tag;

typedef struct s_visibility {
  unsigned int visibility_code;
  struct s_label *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} visibility;

typedef struct s_push_instruction {
  unsigned int push_instruction_code;
  struct s_push_statement *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} push_instruction;

typedef struct s_translation_unit {
  struct s_statement_sequence *phrase_statement_sequence;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} translation_unit;

#define fill_incomplete 23

#endif
