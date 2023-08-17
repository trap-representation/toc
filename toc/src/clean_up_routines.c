#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H
#include "clean_up_routines.h"

void clean_up_string_table(str_table *string_table){
  while(string_table!=NULL){
    str_table *prev=string_table;
    string_table=string_table->next_node;
    free(prev->string);
    free(prev);
  }
}

void clean_up_symbol_table(sym_table *symbol_table){
  while(symbol_table!=NULL){
    sym_table *prev=symbol_table;
    symbol_table=symbol_table->next_node;
    free(prev);
  }
}

void clean_up_struct_definition_table(struct_definition_table *struct_table){
  while(struct_table!=NULL){
    struct_definition_table *prev=struct_table;
    struct_table=struct_table->next_node;
    free(prev);
  }
}

void clean_up_libraries(libs *libraries){
  while(libraries!=NULL){
    libs *prev=libraries;
    libraries=libraries->next;
    free(prev->lib_name);
    free(prev);
  }
}

void clean_up_tokens_list(token *tokens){
  if(tokens!=NULL){
    while(tokens->token_class!=tokens_list_end){
      if(tokens->token_class==tk_identifier || tokens->token_class==tk_string_constant || tokens->token_class==tk_character_constant || tokens->token_class==tk_numeric_constant){
        free(tokens->token_string);
      }
      token *curr=tokens;
      tokens=tokens->next_token;
      free(curr);
    }
    free(tokens);
  }
}

void clean_up_symbol(symbol *phrase_symbol){
  if(phrase_symbol!=NULL){
    free(phrase_symbol->lex_identifier);
    free(phrase_symbol);
  }
}

void clean_up_push_instruction(push_instruction *phrase_push_instruction){
  free(phrase_push_instruction);
}

void clean_up_sign(sign *phrase_sign){
  free(phrase_sign);
}

void clean_up_push_statement(push_statement *phrase_push_statement){
  if(phrase_push_statement!=NULL){
    clean_up_sign(phrase_push_statement->phrase_sign);
    clean_up_push_instruction(phrase_push_statement->phrase_push_instruction);
    if(phrase_push_statement->child_type==lex_type_numeric_constant){
      free(phrase_push_statement->child.lex_numeric_constant);
    }
    else if(phrase_push_statement->child_type==lex_type_character_constant){
      free(phrase_push_statement->child.lex_character_constant);
    }
    else if(phrase_push_statement->child_type==lex_type_string_constant){
      free(phrase_push_statement->child.lex_string_constant);
    }
    else if(phrase_push_statement->child_type==phrase_type_symbol){
      clean_up_symbol(phrase_push_statement->child.phrase_symbol);
    }
    free(phrase_push_statement);
  }
}

void clean_up_import_statement(import_statement *phrase_import_statement){
  if(phrase_import_statement!=NULL){
    free(phrase_import_statement->lex_string_constant);
    free(phrase_import_statement);
  }
}

void clean_up_visibility(visibility *phrase_visibility){
  free(phrase_visibility);
}

void clean_up_label(label *phrase_label){
  if(phrase_label!=NULL){
    clean_up_visibility(phrase_label->phrase_visibility);
    free(phrase_label->lex_identifier);
    free(phrase_label);
  }
}

void clean_up_set_statement(set_statement *phrase_set_statement){
  if(phrase_set_statement!=NULL){
    free(phrase_set_statement->lex_identifier);
    clean_up_sign(phrase_set_statement->phrase_sign);
    if(phrase_set_statement->child1_type==lex_type_numeric_constant){
      free(phrase_set_statement->child1.lex_numeric_constant);
    }
    else if(phrase_set_statement->child1_type==lex_type_string_constant){
      free(phrase_set_statement->child1.lex_string_constant);
    }
    else if(phrase_set_statement->child1_type==phrase_type_symbol){
      clean_up_symbol(phrase_set_statement->child1.phrase_symbol);
    }
    free(phrase_set_statement);
  }
}

void clean_up_tag(tag *phrase_tag){
  if(phrase_tag!=NULL){
    free(phrase_tag->lex_identifier);
    free(phrase_tag);
  }
}

void clean_up_tag_sequence(tag_sequence *phrase_tag_sequence){
  while(phrase_tag_sequence!=NULL){
    tag_sequence *prev=phrase_tag_sequence;
    phrase_tag_sequence=phrase_tag_sequence->phrase_tag_sequence;
    clean_up_tag(prev->phrase_tag);
    free(prev);
  }
}

void clean_up_phrase_struct_name(struct_name *phrase_struct_name){
  if(phrase_struct_name!=NULL){
    free(phrase_struct_name->lex_identifier);
    free(phrase_struct_name);
  }
}

void clean_up_tagged_expression(tagged_expression *phrase_tagged_expression){
  if(phrase_tagged_expression!=NULL){
    clean_up_phrase_struct_name(phrase_tagged_expression->phrase_struct_name);
    clean_up_tag_sequence(phrase_tagged_expression->phrase_tag_sequence);
    free(phrase_tagged_expression);
  }
}

void clean_up_phrase_type(type *phrase_type){
  free(phrase_type);
}

void clean_up_struct_tag_definition(struct_tag_definition *phrase_struct_tag_definition){
  if(phrase_struct_tag_definition!=NULL){
    if(phrase_struct_tag_definition->child_type==phrase_type_struct_name){
      clean_up_phrase_struct_name(phrase_struct_tag_definition->child.phrase_struct_name);
    }
    else if(phrase_struct_tag_definition->child_type==phrase_type_type){
      clean_up_phrase_type(phrase_struct_tag_definition->child.phrase_type);
    }
    free(phrase_struct_tag_definition->lex_identifier);
    free(phrase_struct_tag_definition);
  }
}

void clean_up_struct_tag_definition_sequence(struct_tag_definition_sequence *phrase_struct_tag_definition_sequence){
  while(phrase_struct_tag_definition_sequence!=NULL){
    struct_tag_definition_sequence *prev=phrase_struct_tag_definition_sequence;
    phrase_struct_tag_definition_sequence=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence;
    clean_up_struct_tag_definition(prev->phrase_struct_tag_definition);
    free(prev);
  }
}

void clean_up_struct_definition(struct_definition *phrase_struct_definition){
  if(phrase_struct_definition!=NULL){
    free(phrase_struct_definition->lex_identifier);
    clean_up_struct_tag_definition_sequence(phrase_struct_definition->phrase_struct_tag_definition_sequence);
    free(phrase_struct_definition);
  }
}

void clean_up_sizeof_statement(sizeof_statement *phrase_sizeof_statement){
  if(phrase_sizeof_statement!=NULL){
    if(phrase_sizeof_statement->child_type==phrase_type_struct_name){
      clean_up_phrase_struct_name(phrase_sizeof_statement->child.phrase_struct_name);
    }
    else if(phrase_sizeof_statement->child_type==phrase_type_type){
      clean_up_phrase_type(phrase_sizeof_statement->child.phrase_type);
    }
    free(phrase_sizeof_statement);
  }
}

void clean_up_alignof_statement(alignof_statement *phrase_alignof_statement){
  if(phrase_alignof_statement!=NULL){
    if(phrase_alignof_statement->child_type==phrase_type_struct_name){
      clean_up_phrase_struct_name(phrase_alignof_statement->child.phrase_struct_name);
    }
    else if (phrase_alignof_statement->child_type==phrase_type_type){
      clean_up_phrase_type(phrase_alignof_statement->child.phrase_type);
    }
    free(phrase_alignof_statement);
  }
}

void clean_up_instruction(instruction *phrase_instruction){
  free(phrase_instruction);
}

void clean_up_statement(statement *phrase_statement){
  if(phrase_statement->child_type==phrase_type_push_statement){
      clean_up_push_statement(phrase_statement->child.phrase_push_statement);
  }
  else if(phrase_statement->child_type==phrase_type_sizeof_statement){
    clean_up_sizeof_statement(phrase_statement->child.phrase_sizeof_statement);
  }
  else if(phrase_statement->child_type==phrase_type_alignof_statement){
    clean_up_alignof_statement(phrase_statement->child.phrase_alignof_statement);
  }
  else if(phrase_statement->child_type==phrase_type_import_statement){
    clean_up_import_statement(phrase_statement->child.phrase_import_statement);
  }
  else if(phrase_statement->child_type==phrase_type_set_statement){
    clean_up_set_statement(phrase_statement->child.phrase_set_statement);
  }
  else if(phrase_statement->child_type==phrase_type_label){
    clean_up_label(phrase_statement->child.phrase_label);
  }
  else if(phrase_statement->child_type==phrase_type_tagged_expression){
    clean_up_tagged_expression(phrase_statement->child.phrase_tagged_expression);
  }
  else if(phrase_statement->child_type==phrase_type_struct_definition){
    clean_up_struct_definition(phrase_statement->child.phrase_struct_definition);
  }
  else if(phrase_statement->child_type==phrase_type_instruction){
    clean_up_instruction(phrase_statement->child.phrase_instruction);
  }
  free(phrase_statement);
}

void clean_up_statement_sequence(statement_sequence *phrase_statement_sequence){
  while(phrase_statement_sequence!=NULL){
    statement_sequence *prev=phrase_statement_sequence;
    phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
    clean_up_statement(prev->phrase_statement);
    free(prev);
  }
}

void clean_up_translation_unit(translation_unit *phrase_translation_unit){
  clean_up_statement_sequence(phrase_translation_unit->phrase_statement_sequence);
  free(phrase_translation_unit);
}

void clean_up_translation_unit_list(translation_unit_list *tu_list){
  while(tu_list!=NULL){
    if(tu_list->tu!=NULL){
      clean_up_translation_unit(tu_list->tu);
    }
    translation_unit_list *prev=tu_list;
    tu_list=tu_list->next_tu;
    free(prev);
  }
}
