#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "write_error.h"
#include "comp_properties.h"
#include "verify.h"

bool tagged_expression_construct_ok(translation_unit *tu, tagged_expression *phrase_tagged_expression, struct_definition_table *struct_table){
  bool found;
  struct_definition *phrase_struct_definition;
  lookup_in_struct_definition_table(phrase_tagged_expression->phrase_struct_name->identifier,struct_table,tu,false,&phrase_struct_definition,ret_any);
  if(phrase_struct_definition==NULL){
    write_error("error: no definition of such struct s_found\n",phrase_tagged_expression->phrase_struct_name->start_x,phrase_tagged_expression->phrase_struct_name->start_y,phrase_tagged_expression->phrase_struct_name->end_x,phrase_tagged_expression->phrase_struct_name->end_y,phrase_tagged_expression->phrase_struct_name->file);
    return false;
  }
  tag_sequence *phrase_tag_sequence=phrase_tagged_expression->phrase_tag_sequence;
  while(phrase_tag_sequence!=NULL){
    struct_tag_definition_sequence *phrase_struct_tag_definition_sequence=phrase_struct_definition->phrase_struct_tag_definition_sequence;
    found=false;
    while(phrase_struct_tag_definition_sequence!=NULL){
      struct_tag_definition *phrase_struct_tag_definition=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition;
      if(strcmp(phrase_struct_tag_definition->identifier,phrase_tag_sequence->phrase_tag->identifier)==0){
        if(phrase_struct_tag_definition->child_type==phrase_type_struct_name){
          lookup_in_struct_definition_table(phrase_struct_tag_definition->child.phrase_struct_name->identifier,struct_table,tu,false,&phrase_struct_definition,ret_any);
        }
        else if(phrase_struct_tag_definition->child_type==phrase_type_type && phrase_tag_sequence->phrase_tag_sequence!=NULL){
          write_error("error: a tag designating a type cannot be tagged\n",phrase_tag_sequence->phrase_tag->start_x,phrase_tag_sequence->phrase_tag->start_y,phrase_tag_sequence->phrase_tag->end_x,phrase_tag_sequence->phrase_tag->end_y,phrase_tag_sequence->phrase_tag->file);
          return false;
        }
        found=true;
        break;
      }
      phrase_struct_tag_definition_sequence=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence;
    }
    if(!found){
      write_error("error: no such tag in struct\n",phrase_tag_sequence->phrase_tag->start_x,phrase_tag_sequence->phrase_tag->start_y,phrase_tag_sequence->phrase_tag->end_x,phrase_tag_sequence->phrase_tag->end_y,phrase_tag_sequence->phrase_tag->file);
      return false;
    }
    phrase_tag_sequence=phrase_tag_sequence->phrase_tag_sequence;
  }
  return true;
}

bool tagged_expression_constructs_ok(translation_unit *tu, struct_definition_table *struct_table){
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  bool ret=true;
  while(phrase_statement_sequence!=NULL){
    statement *phrase_statement=phrase_statement_sequence->phrase_statement;
    if(phrase_statement->child_type==phrase_type_tagged_expression){
      if(!tagged_expression_construct_ok(tu,phrase_statement->child.phrase_tagged_expression,struct_table)){
        ret=false;
      }
    }
    phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
  }
  return ret;
}

bool no_dup_tag_in_struct_tag_definition_sequence(struct_definition_table *struct_table){
  bool ret=true;
  while(struct_table!=NULL){
    struct_tag_definition_sequence *trav0=struct_table->loc->phrase_struct_tag_definition_sequence;
    while(trav0!=NULL){
      struct_tag_definition_sequence *trav1=trav0->phrase_struct_tag_definition_sequence;
      while(trav1!=NULL){
        if(strcmp(trav0->phrase_struct_tag_definition->identifier,trav1->phrase_struct_tag_definition->identifier)==0){
          write_error("error: tag already in use\n",trav1->phrase_struct_tag_definition->start_x,trav1->phrase_struct_tag_definition->start_y,trav1->phrase_struct_tag_definition->end_x,trav1->phrase_struct_tag_definition->end_y,trav1->phrase_struct_tag_definition->file);
          ret=false;
          break;
        }
        trav1=trav1->phrase_struct_tag_definition_sequence;
      }
      trav0=trav0->phrase_struct_tag_definition_sequence;
    }
    struct_table=struct_table->next_node;
  }
  return ret;
}

bool no_use_of_unset_symbol_in_push_statements(translation_unit *tu, sym_table *symbol_table){
  bool ret=true;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  while(phrase_statement_sequence!=NULL){
    statement *phrase_statement=phrase_statement_sequence->phrase_statement;
    if(phrase_statement->child_type==phrase_type_push_statement&& phrase_statement->child.phrase_push_statement->child_type==phrase_type_symbol){
      label *ret_label;
      set_statement *ret_set_statement;
      lookup_in_symbol_table(phrase_statement->child.phrase_push_statement->child.phrase_symbol->identifier,symbol_table,&ret_label,&ret_set_statement,tu,false);
      if(ret_label==NULL && ret_set_statement==NULL){
        write_error("error: use of unset symbol\n",phrase_statement->child.phrase_push_statement->child.phrase_symbol->start_x,phrase_statement->child.phrase_push_statement->child.phrase_symbol->start_y,phrase_statement->child.phrase_push_statement->child.phrase_symbol->end_x,phrase_statement->child.phrase_push_statement->child.phrase_symbol->end_y,phrase_statement->child.phrase_push_statement->child.phrase_symbol->file);
        ret=false;
      }
    }
    phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
  }
  return ret;
}

bool alignof_statements_ok(translation_unit *tu, struct_definition_table *struct_table){
  bool ret=true;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  while(phrase_statement_sequence!=NULL){
    statement *phrase_statement=phrase_statement_sequence->phrase_statement;
    if(phrase_statement->child_type==phrase_type_alignof_statement && phrase_statement->child.phrase_alignof_statement->child_type==phrase_type_struct_name){
      struct_definition *ret;
      lookup_in_struct_definition_table(phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->identifier,struct_table,tu,false,&ret,ret_any);
      if(ret==NULL){
        write_error("error: no definition of such struct s_found\n",phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->start_x,phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->start_y,phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->end_x,phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->end_y,phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->file);
        ret=false;
      }
    }
    phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
  }
  return ret;
}

bool sizeof_statements_ok(translation_unit *tu, struct_definition_table *struct_table){
  bool ret=true;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  while(phrase_statement_sequence!=NULL){
    statement *phrase_statement=phrase_statement_sequence->phrase_statement;
    if(phrase_statement->child_type==phrase_type_sizeof_statement && phrase_statement->child.phrase_sizeof_statement->child_type==phrase_type_struct_name){
      struct_definition *ret;
      lookup_in_struct_definition_table(phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->identifier,struct_table,tu,false,&ret,ret_any);
      if(ret==NULL){
        write_error("error: no definition of such struct s_found\n",phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->start_x,phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->start_y,phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->end_x,phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->end_y,phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->file);
        ret=false;
      }
    }
    phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
  }
  return ret;
}

bool entrypoint_exists(sym_table *symbol_table){
  label *ret_label;
  lookup_in_symbol_table("main",symbol_table,&ret_label,NULL,NULL,false);
  if(ret_label==NULL){
    fprintf(stderr,"error: no entrypoint found\n");
    return false;
  }
  return true;
}
