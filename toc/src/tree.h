#ifndef TOC_TREE_H
#define TOC_TREE_H

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H

enum phrase_type {
  phrase_type_sizeof_statement,
  phrase_type_else_clause,
  phrase_type_selection_statement,
  phrase_type_alignof_statement,
  phrase_type_push_statement,
  phrase_type_label,
  phrase_type_set_statement,
  phrase_type_import_statement,
  phrase_type_tag_sequence,
  phrase_type_tagged_statement,
  phrase_type_struct_tag_definition,
  phrase_type_struct_tag_definition_sequence,
  phrase_type_struct_definition,
  phrase_type_statement,
  phrase_type_statement_sequence,
  phrase_type_type,
  phrase_type_struct_name,
  phrase_type_sign,
  phrase_type_instruction,
  phrase_type_symbol,
  phrase_type_tag,
  phrase_type_visibility,
  phrase_type_push_instruction,
  phrase_type_translation_unit,
  lex_type_keyword,
  lex_type_identifier,
  lex_type_character_constant,
  lex_type_string_constant,
  lex_type_numeric_constant,
  lex_type_punctuator,
  fill_incomplete
};

enum e_type_type {
  type_uc,
  type_us,
  type_ui,
  type_c,
  type_s,
  type_i,
  type_l,
  type_p
};

enum e_sign_type {
  sign_positive,
  sign_negative
};

enum e_visibility_type {
  visibility_hidden,
  visibility_exposed
};


enum e_instruction_type {
  instruction_eq,
  instruction_gt,
  instruction_ls,
  instruction_ge,
  instruction_le,
  instruction_ne,
  instruction_zr,
  instruction_nz,
  instruction_addc,
  instruction_adds,
  instruction_addi,
  instruction_addp,
  instruction_addl,
  instruction_adduc,
  instruction_addus,
  instruction_addui,
  instruction_subc,
  instruction_subs,
  instruction_subi,
  instruction_subp,
  instruction_subl,
  instruction_subuc,
  instruction_subus,
  instruction_subui,
  instruction_mulc,
  instruction_muls,
  instruction_muli,
  instruction_mull,
  instruction_muluc,
  instruction_mulus,
  instruction_mului,
  instruction_divc,
  instruction_divs,
  instruction_divi,
  instruction_divl,
  instruction_divuc,
  instruction_divus,
  instruction_divui,
  instruction_remc,
  instruction_rems,
  instruction_remi,
  instruction_reml,
  instruction_remuc,
  instruction_remus,
  instruction_remui,
  instruction_lshc,
  instruction_lshs,
  instruction_lshi,
  instruction_lshl,
  instruction_lshuc,
  instruction_lshus,
  instruction_lshui,
  instruction_rshc,
  instruction_rshs,
  instruction_rshi,
  instruction_rshl,
  instruction_rshuc,
  instruction_rshus,
  instruction_rshui,
  instruction_orc,
  instruction_ors,
  instruction_ori,
  instruction_orl,
  instruction_oruc,
  instruction_orus,
  instruction_orui,
  instruction_andc,
  instruction_ands,
  instruction_andi,
  instruction_andl,
  instruction_anduc,
  instruction_andus,
  instruction_andui,
  instruction_notc,
  instruction_nots,
  instruction_noti,
  instruction_notl,
  instruction_notuc,
  instruction_notus,
  instruction_notui,
  instruction_xorc,
  instruction_xors,
  instruction_xori,
  instruction_xorl,
  instruction_xoruc,
  instruction_xorus,
  instruction_xorui,
  instruction_swap,
  instruction_pop,
  instruction_dup,
  instruction_ret,
  instruction_over,
  instruction_panic,
  instruction_write0,
  instruction_write1,
  instruction_write2,
  instruction_write3,
  instruction_write4,
  instruction_write5,
  instruction_write6,
  instruction_write7,
  instruction_vwrite0,
  instruction_vwrite1,
  instruction_vwrite2,
  instruction_vwrite3,
  instruction_vwrite4,
  instruction_vwrite5,
  instruction_vwrite6,
  instruction_vwrite7,
  instruction_get0,
  instruction_get1,
  instruction_get2,
  instruction_get3,
  instruction_get4,
  instruction_get5,
  instruction_get6,
  instruction_get7,
  instruction_vget0,
  instruction_vget1,
  instruction_vget2,
  instruction_vget3,
  instruction_vget4,
  instruction_vget5,
  instruction_vget6,
  instruction_vget7,
  instruction_loadc,
  instruction_loads,
  instruction_loadi,
  instruction_loadp,
  instruction_loadl,
  instruction_loaduc,
  instruction_loadus,
  instruction_loadui,
  instruction_vloadc,
  instruction_vloads,
  instruction_vloadi,
  instruction_vloadp,
  instruction_vloadl,
  instruction_vloaduc,
  instruction_vloadus,
  instruction_vloadui,
  instruction_aloadc,
  instruction_aloads,
  instruction_aloadi,
  instruction_aloadp,
  instruction_aloadl,
  instruction_aloaduc,
  instruction_aloadus,
  instruction_aloadui,
  instruction_valoadc,
  instruction_valoads,
  instruction_valoadi,
  instruction_valoadp,
  instruction_valoadl,
  instruction_valoaduc,
  instruction_valoadus,
  instruction_valoadui,
  instruction_storec,
  instruction_stores,
  instruction_storei,
  instruction_storep,
  instruction_storel,
  instruction_storeuc,
  instruction_storeus,
  instruction_storeui,
  instruction_vstorec,
  instruction_vstores,
  instruction_vstorei,
  instruction_vstorep,
  instruction_vstorel,
  instruction_vstoreuc,
  instruction_vstoreus,
  instruction_vstoreui,
  instruction_astorec,
  instruction_astores,
  instruction_astorei,
  instruction_astorep,
  instruction_astorel,
  instruction_astoreuc,
  instruction_astoreus,
  instruction_astoreui,
  instruction_vastorec,
  instruction_vastores,
  instruction_vastorei,
  instruction_vastorep,
  instruction_vastorel,
  instruction_vastoreuc,
  instruction_vastoreus,
  instruction_vastoreui,
  instruction_hlt,
  instruction_call,
  instruction_copy,
  instruction_pcopy,
  instruction_popa,
  instruction_put,
  instruction_pushsp,
  instruction_hltr,
  instruction_incsp,
  instruction_decsp,
  instruction_exit,
  instruction_force_panic,
  instruction_pushlt,
  instruction_rcall,
  instruction_open,
  instruction_invoke,
  instruction_pushpc,
  instruction_pushcs
};

enum e_push_instruction_type {
  push_instruction_pushc,
  push_instruction_pushs,
  push_instruction_pushi,
  push_instruction_pushp,
  push_instruction_pushl,
  push_instruction_pushuc,
  push_instruction_pushus,
  push_instruction_pushui
};

typedef struct s_sizeof_statement {
  enum phrase_type child_type;
  union sizeof_statement_child_type_union {
    struct s_type *phrase_type;
    struct s_struct_name *phrase_struct_name;
  } child;
  struct s_statement *parent;
  ysm_ui align_pad;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} sizeof_statement;

typedef struct s_else_clause {
  enum phrase_type child_type;
  union else_clause_child_type_union {
    struct s_statement *phrase_statement;
    struct s_statement_sequence *phrase_statement_sequence;
  } child;
  struct s_selection_statement *parent;
  ysm_ui align_pad;
  ysm_l rend_address;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} else_clause;

typedef struct s_selection_statement {
  struct s_statement_sequence *phrase_statement_sequence_0;
  enum phrase_type child_type;
  union selection_statement_child_type_union {
    struct s_statement *phrase_statement;
    struct s_statement_sequence *phrase_statement_sequence_1;
  } child;
  struct s_else_clause *phrase_else_clause;
  struct s_statement *parent;
  ysm_ui align_pad;
  ysm_l rend_address;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} selection_statement;

typedef struct s_alignof_statement {
  enum phrase_type child_type;
  union alignof_statement_child_type_union {
    struct s_type *phrase_type;
    struct s_struct_name *phrase_struct_name;
  } child;
  struct s_statement *parent;
  ysm_ui align_pad;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} alignof_statement;

typedef struct s_push_statement {
  struct s_push_instruction *phrase_push_instruction;
  struct s_sign *phrase_sign;
  enum phrase_type child_type;
  union push_statement_child_type_union {
    char *lex_numeric_constant;
    char *lex_string_constant;
    char *lex_character_constant;
    struct s_symbol *phrase_symbol;
  } child;
  struct s_statement *parent;
  ysm_ui align_pad;
  ysm_l address;
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
  enum phrase_type child_type;
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
  enum phrase_type parent_type;
  union tag_sequence_parent_type_union {
    struct s_tagged_statement *phrase_tagged_statement;
    struct s_tag_sequence *phrase_tag_sequence;
  } parent;
  char *file;
} tag_sequence;

typedef struct s_tagged_statement {
  struct s_struct_name *phrase_struct_name;
  struct s_tag_sequence *phrase_tag_sequence;
  struct s_statement *parent;
  ysm_ui align_pad;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} tagged_statement;

typedef struct s_struct_tag_definition {
  enum phrase_type child_type;
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
  enum phrase_type parent_type;
  union struct_tag_definition_sequence_parent_union {
    struct s_struct_definition *phrase_struct_definition;
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
  enum phrase_type child_type;
  union statement_child_type_union {
    struct s_push_statement *phrase_push_statement;
    struct s_set_statement *phrase_set_statement;
    struct s_alignof_statement *phrase_alignof_statement;
    struct s_sizeof_statement *phrase_sizeof_statement;
    struct s_label *phrase_label;
    struct s_import_statement *phrase_import_statement;
    struct_definition *phrase_struct_definition;
    struct s_tagged_statement *phrase_tagged_statement;
    struct s_instruction *phrase_instruction;
    struct s_selection_statement *phrase_selection_statement;
    struct s_selection_statement *phrase_else_clause;
  } child;
  enum phrase_type parent_type;
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
  enum phrase_type parent_type;
  union statement_sequence_parent_type_union {
    struct s_translation_unit *phrase_translation_unit;
    struct s_statement_sequence *phrase_statement_sequence;
    struct s_selection_statement *phrase_selection_statement;
    struct s_else_clause *phrase_else_clause;
  } parent;
  struct s_translation_unit *tu;
  char *file;
} statement_sequence;

typedef struct s_type {
  enum e_type_type type_type;
  enum phrase_type parent_type;
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
  enum phrase_type parent_type;
  union struct_name_parent_type_union {
    struct s_sizeof_statement *phrase_sizeof_statement;
    struct s_alignof_statement *phrase_alignof_statement;
    struct s_tagged_statement *phrase_tagged_statement;
    struct s_struct_tag_definition *phrase_struct_tag_definition;
  } parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} struct_name;

typedef struct s_sign {
  enum e_sign_type sign_type;
  enum phrase_type parent_type;
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
  enum e_instruction_type instruction_type;
  struct s_statement *parent;
  ysm_ui align_pad;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} instruction;

typedef struct s_symbol {
  char *lex_identifier;
  enum phrase_type parent_type;
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
  enum e_visibility_type visibility_type;
  struct s_label *parent;
  char *file;
  uintmax_t start_x;
  uintmax_t start_y;
  uintmax_t end_x;
  uintmax_t end_y;
} visibility;

typedef struct s_push_instruction {
  enum e_push_instruction_type push_instruction_type;
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

#endif
