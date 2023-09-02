#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "write_error.h"
#include "comp_properties.h"
#include "verify.h"

static bool check_tagged_statement(statement *phrase_statement, struct_definition_table *struct_table, translation_unit *tu){
  if(phrase_statement->child_type==phrase_type_tagged_statement){
    tagged_statement *phrase_tagged_statement=phrase_statement->child.phrase_tagged_statement;
    bool found_tag;
    struct_definition *phrase_struct_definition;
    lookup_in_struct_definition_table(phrase_tagged_statement->phrase_struct_name->lex_identifier,struct_table,tu,false,&phrase_struct_definition,ret_any);
    if(phrase_struct_definition==NULL){
      write_error("error: no definition of such struct found\n",phrase_tagged_statement->phrase_struct_name->start_x,phrase_tagged_statement->phrase_struct_name->start_y,phrase_tagged_statement->phrase_struct_name->end_x,phrase_tagged_statement->phrase_struct_name->end_y,phrase_tagged_statement->phrase_struct_name->file);
      return false;
    }
    tag_sequence *phrase_tag_sequence=phrase_tagged_statement->phrase_tag_sequence;
    while(phrase_tag_sequence!=NULL){
      struct_tag_definition_sequence *phrase_struct_tag_definition_sequence=phrase_struct_definition->phrase_struct_tag_definition_sequence;
      found_tag=false;
      while(phrase_struct_tag_definition_sequence!=NULL){
        struct_tag_definition *phrase_struct_tag_definition=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition;
        if(strcmp(phrase_struct_tag_definition->lex_identifier,phrase_tag_sequence->phrase_tag->lex_identifier)==0){
          if(phrase_struct_tag_definition->child_type==phrase_type_struct_name){
            lookup_in_struct_definition_table(phrase_struct_tag_definition->child.phrase_struct_name->lex_identifier,struct_table,tu,false,&phrase_struct_definition,ret_any);
          }
          else if(phrase_struct_tag_definition->child_type==phrase_type_type && phrase_tag_sequence->phrase_tag_sequence!=NULL){
            write_error("error: a tag designating a type cannot be tagged\n",phrase_tag_sequence->phrase_tag->start_x,phrase_tag_sequence->phrase_tag->start_y,phrase_tag_sequence->phrase_tag->end_x,phrase_tag_sequence->phrase_tag->end_y,phrase_tag_sequence->phrase_tag->file);
            return false;
          }
          found_tag=true;
          break;
        }
        phrase_struct_tag_definition_sequence=phrase_struct_tag_definition_sequence->phrase_struct_tag_definition_sequence;
      }
      if(!found_tag){
        write_error("error: no such tag in struct\n",phrase_tag_sequence->phrase_tag->start_x,phrase_tag_sequence->phrase_tag->start_y,phrase_tag_sequence->phrase_tag->end_x,phrase_tag_sequence->phrase_tag->end_y,phrase_tag_sequence->phrase_tag->file);
        return false;
      }
      phrase_tag_sequence=phrase_tag_sequence->phrase_tag_sequence;
    }
  }
  return true;
}

static bool walk_tree_and_check_tagged_statements(statement_sequence *phrase_statement_sequence, statement *phrase_statement, struct_definition_table *struct_table, translation_unit *tu){
  bool ret=true;
  if(phrase_statement!=NULL){
    if(phrase_statement->child_type==phrase_type_selection_statement){
      selection_statement *phrase_selection_statement=phrase_statement->child.phrase_selection_statement;
      if(!walk_tree_and_check_tagged_statements(phrase_selection_statement->phrase_statement_sequence_0,NULL,struct_table,tu)){
        ret=false;
      }
      if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
        if(!walk_tree_and_check_tagged_statements(phrase_selection_statement->child.phrase_statement_sequence_1,NULL,struct_table,tu)){
          ret=false;
        }
      }
      else{
        if(!walk_tree_and_check_tagged_statements(NULL,phrase_selection_statement->child.phrase_statement,struct_table,tu)){
          ret=false;
        }
      }
      if(phrase_selection_statement->phrase_else_clause!=NULL){
        if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
          if(!walk_tree_and_check_tagged_statements(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,struct_table,tu)){
            ret=false;
          }
        }
        else{
          if(!walk_tree_and_check_tagged_statements(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,struct_table,tu)){
            ret=false;
          }
        }
      }
    }
    else{
      if(!check_tagged_statement(phrase_statement,struct_table,tu)){
        ret=false;
      }
    }
  }
  else{
    while(phrase_statement_sequence!=NULL){
      if(phrase_statement_sequence->phrase_statement->child_type==phrase_type_selection_statement){
        selection_statement *phrase_selection_statement=phrase_statement_sequence->phrase_statement->child.phrase_selection_statement;
        if(!walk_tree_and_check_tagged_statements(phrase_selection_statement->phrase_statement_sequence_0,NULL,struct_table,tu)){
          ret=false;
        }
        if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
          if(!walk_tree_and_check_tagged_statements(phrase_selection_statement->child.phrase_statement_sequence_1,NULL,struct_table,tu)){
            ret=false;
          }
        }
        else{
          if(!walk_tree_and_check_tagged_statements(NULL,phrase_selection_statement->child.phrase_statement,struct_table,tu)){
            ret=false;
          }
        }
        if(phrase_selection_statement->phrase_else_clause!=NULL){
          if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
            if(!walk_tree_and_check_tagged_statements(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,struct_table,tu)){
              ret=false;
            }
          }
          else{
            if(!walk_tree_and_check_tagged_statements(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,struct_table,tu)){
              ret=false;
            }
          }
        }
      }
      else{
        if(!check_tagged_statement(phrase_statement_sequence->phrase_statement,struct_table,tu)){
          ret=false;
        }
      }
      phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
    }
  }
  return ret;
}

bool tagged_statement_constructs_ok(struct_definition_table *struct_table, translation_unit *tu){
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  if(!walk_tree_and_check_tagged_statements(phrase_statement_sequence,NULL,struct_table,tu)){
    return false;
  }
  return true;
}

bool no_dup_tag_in_struct_tag_definition_sequence(struct_definition_table *struct_table){
  bool ret=true;
  while(struct_table!=NULL){
    struct_tag_definition_sequence *trav0=struct_table->address->phrase_struct_tag_definition_sequence;
    while(trav0!=NULL){
      struct_tag_definition_sequence *trav1=trav0->phrase_struct_tag_definition_sequence;
      while(trav1!=NULL){
        if(strcmp(trav0->phrase_struct_tag_definition->lex_identifier,trav1->phrase_struct_tag_definition->lex_identifier)==0){
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

static bool check_push_statement(statement *phrase_statement, sym_table *symbol_table, translation_unit *tu){
  if(phrase_statement->child_type==phrase_type_push_statement&& phrase_statement->child.phrase_push_statement->child_type==phrase_type_symbol){
    bool found_symbol;
    lookup_in_symbol_table(phrase_statement->child.phrase_push_statement->child.phrase_symbol->lex_identifier,symbol_table,tu,false,&found_symbol);
    if(!found_symbol){
      write_error("error: use of unset symbol\n",phrase_statement->child.phrase_push_statement->child.phrase_symbol->start_x,phrase_statement->child.phrase_push_statement->child.phrase_symbol->start_y,phrase_statement->child.phrase_push_statement->child.phrase_symbol->end_x,phrase_statement->child.phrase_push_statement->child.phrase_symbol->end_y,phrase_statement->child.phrase_push_statement->child.phrase_symbol->file);
      return false;
    }
  }
  return true;
}

static bool walk_tree_and_check_push_statements(statement_sequence *phrase_statement_sequence, statement *phrase_statement, sym_table *symbol_table, translation_unit *tu){
  bool ret=true;
  if(phrase_statement!=NULL){
    if(phrase_statement->child_type==phrase_type_selection_statement){
      selection_statement *phrase_selection_statement=phrase_statement->child.phrase_selection_statement;
      if(!walk_tree_and_check_push_statements(phrase_selection_statement->phrase_statement_sequence_0,NULL,symbol_table,tu)){
        ret=false;
      }
      if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
        if(!walk_tree_and_check_push_statements(phrase_selection_statement->child.phrase_statement_sequence_1,NULL,symbol_table,tu)){
          ret=false;
        }
      }
      else{
        if(!walk_tree_and_check_push_statements(NULL,phrase_selection_statement->child.phrase_statement,symbol_table,tu)){
          ret=false;
        }
      }
      if(phrase_selection_statement->phrase_else_clause!=NULL){
        if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
          if(!walk_tree_and_check_push_statements(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,symbol_table,tu)){
            ret=false;
          }
        }
        else{
          if(!walk_tree_and_check_push_statements(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,symbol_table,tu)){
            ret=false;
          }
        }
      }
    }
    else{
      if(!check_push_statement(phrase_statement,symbol_table,tu)){
        ret=false;
      }
    }
  }
  else{
    while(phrase_statement_sequence!=NULL){
      if(phrase_statement_sequence->phrase_statement->child_type==phrase_type_selection_statement){
        selection_statement *phrase_selection_statement=phrase_statement_sequence->phrase_statement->child.phrase_selection_statement;
        if(!walk_tree_and_check_push_statements(phrase_selection_statement->phrase_statement_sequence_0,NULL,symbol_table,tu)){
          ret=false;
        }
        if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
          if(!walk_tree_and_check_push_statements(phrase_selection_statement->child.phrase_statement_sequence_1,NULL,symbol_table,tu)){
            ret=false;
          }
        }
        else{
          if(!walk_tree_and_check_push_statements(NULL,phrase_selection_statement->child.phrase_statement,symbol_table,tu)){
            ret=false;
          }
        }
        if(phrase_selection_statement->phrase_else_clause!=NULL){
          if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
            if(!walk_tree_and_check_push_statements(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,symbol_table,tu)){
              ret=false;
            }
          }
          else{
            if(!walk_tree_and_check_push_statements(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,symbol_table,tu)){
              ret=false;
            }
          }
        }
      }
      else{
        if(!check_push_statement(phrase_statement_sequence->phrase_statement,symbol_table,tu)){
          ret=false;
        }
      }
      phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
    }
  }
  return ret;
}

bool no_use_of_unset_symbols_in_push_statements(sym_table *symbol_table, translation_unit *tu){
  bool ret=true;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  if(!walk_tree_and_check_push_statements(phrase_statement_sequence,NULL,symbol_table,tu)){
    return false;
  }
  return ret;
}

static bool check_alignof_statement(statement *phrase_statement, struct_definition_table *struct_table, translation_unit *tu){
  if(phrase_statement->child_type==phrase_type_alignof_statement && phrase_statement->child.phrase_alignof_statement->child_type==phrase_type_struct_name){
    struct_definition *ret;
    lookup_in_struct_definition_table(phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->lex_identifier,struct_table,tu,false,&ret,ret_any);
    if(ret==NULL){
      write_error("error: no definition of such struct found\n",phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->start_x,phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->start_y,phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->end_x,phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->end_y,phrase_statement->child.phrase_alignof_statement->child.phrase_struct_name->file);
      ret=false;
    }
  }
  return true;
}

static bool walk_tree_and_check_alignof_statements(statement_sequence *phrase_statement_sequence, statement *phrase_statement, struct_definition_table *struct_table, translation_unit *tu){
  bool ret=true;
  if(phrase_statement!=NULL){
    if(phrase_statement->child_type==phrase_type_selection_statement){
      selection_statement *phrase_selection_statement=phrase_statement->child.phrase_selection_statement;
      if(!walk_tree_and_check_alignof_statements(phrase_selection_statement->phrase_statement_sequence_0,NULL,struct_table,tu)){
        ret=false;
      }
      if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
        if(!walk_tree_and_check_alignof_statements(phrase_selection_statement->child.phrase_statement_sequence_1,NULL,struct_table,tu)){
          ret=false;
        }
      }
      else{
        if(!walk_tree_and_check_alignof_statements(NULL,phrase_selection_statement->child.phrase_statement,struct_table,tu)){
          ret=false;
        }
      }
      if(phrase_selection_statement->phrase_else_clause!=NULL){
        if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
          if(!walk_tree_and_check_alignof_statements(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,struct_table,tu)){
            ret=false;
          }
        }
        else{
          if(!walk_tree_and_check_alignof_statements(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,struct_table,tu)){
            ret=false;
          }
        }
      }
    }
    else{
      if(!check_alignof_statement(phrase_statement,struct_table,tu)){
        ret=false;
      }
    }
  }
  else{
    while(phrase_statement_sequence!=NULL){
      if(phrase_statement_sequence->phrase_statement->child_type==phrase_type_selection_statement){
        selection_statement *phrase_selection_statement=phrase_statement_sequence->phrase_statement->child.phrase_selection_statement;
        if(!walk_tree_and_check_alignof_statements(phrase_selection_statement->phrase_statement_sequence_0,NULL,struct_table,tu)){
          ret=false;
        }
        if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
          if(!walk_tree_and_check_alignof_statements(phrase_selection_statement->child.phrase_statement_sequence_1,NULL,struct_table,tu)){
            ret=false;
          }
        }
        else{
          if(!walk_tree_and_check_alignof_statements(NULL,phrase_selection_statement->child.phrase_statement,struct_table,tu)){
            ret=false;
          }
        }
        if(phrase_selection_statement->phrase_else_clause!=NULL){
          if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
            if(!walk_tree_and_check_alignof_statements(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,struct_table,tu)){
              ret=false;
            }
          }
          else{
            if(!walk_tree_and_check_alignof_statements(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,struct_table,tu)){
              ret=false;
            }
          }
        }
      }
      else{
        if(!check_alignof_statement(phrase_statement_sequence->phrase_statement,struct_table,tu)){
          ret=false;
        }
      }
      phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
    }
  }
  return ret;
}

bool alignof_statements_ok(struct_definition_table *struct_table, translation_unit *tu){
  bool ret=true;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  if(!walk_tree_and_check_alignof_statements(phrase_statement_sequence,NULL,struct_table,tu)){
    return false;
  }
  return ret;
}

static bool check_sizeof_statement(statement *phrase_statement, struct_definition_table *struct_table, translation_unit *tu){
  if(phrase_statement->child_type==phrase_type_sizeof_statement && phrase_statement->child.phrase_sizeof_statement->child_type==phrase_type_struct_name){
    struct_definition *ret;
    lookup_in_struct_definition_table(phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->lex_identifier,struct_table,tu,false,&ret,ret_any);
    if(ret==NULL){
      write_error("error: no definition of such struct found\n",phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->start_x,phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->start_y,phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->end_x,phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->end_y,phrase_statement->child.phrase_sizeof_statement->child.phrase_struct_name->file);
      ret=false;
    }
  }
  return true;
}

static bool walk_tree_and_check_sizeof_statements(statement_sequence *phrase_statement_sequence, statement *phrase_statement, struct_definition_table *struct_table, translation_unit *tu){
  bool ret=true;
  if(phrase_statement!=NULL){
    if(phrase_statement->child_type==phrase_type_selection_statement){
      selection_statement *phrase_selection_statement=phrase_statement->child.phrase_selection_statement;
      if(!walk_tree_and_check_sizeof_statements(phrase_selection_statement->phrase_statement_sequence_0,NULL,struct_table,tu)){
        ret=false;
      }
      if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
        if(!walk_tree_and_check_sizeof_statements(phrase_selection_statement->child.phrase_statement_sequence_1,NULL,struct_table,tu)){
          ret=false;
        }
      }
      else{
        if(!walk_tree_and_check_sizeof_statements(NULL,phrase_selection_statement->child.phrase_statement,struct_table,tu)){
          ret=false;
        }
      }
      if(phrase_selection_statement->phrase_else_clause!=NULL){
        if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
          if(!walk_tree_and_check_sizeof_statements(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,struct_table,tu)){
            ret=false;
          }
        }
        else{
          if(!walk_tree_and_check_sizeof_statements(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,struct_table,tu)){
            ret=false;
          }
        }
      }
    }
    else{
      if(!check_sizeof_statement(phrase_statement,struct_table,tu)){
        ret=false;
      }
    }
  }
  else{
    while(phrase_statement_sequence!=NULL){
      if(phrase_statement_sequence->phrase_statement->child_type==phrase_type_selection_statement){
        selection_statement *phrase_selection_statement=phrase_statement_sequence->phrase_statement->child.phrase_selection_statement;
        if(!walk_tree_and_check_sizeof_statements(phrase_selection_statement->phrase_statement_sequence_0,NULL,struct_table,tu)){
          ret=false;
        }
        if(phrase_selection_statement->child_type==phrase_type_statement_sequence){
          if(!walk_tree_and_check_sizeof_statements(phrase_selection_statement->child.phrase_statement_sequence_1,NULL,struct_table,tu)){
            ret=false;
          }
        }
        else{
          if(!walk_tree_and_check_sizeof_statements(NULL,phrase_selection_statement->child.phrase_statement,struct_table,tu)){
            ret=false;
          }
        }
        if(phrase_selection_statement->phrase_else_clause!=NULL){
          if(phrase_selection_statement->phrase_else_clause->child_type==phrase_type_statement_sequence){
            if(!walk_tree_and_check_sizeof_statements(phrase_selection_statement->phrase_else_clause->child.phrase_statement_sequence,NULL,struct_table,tu)){
              ret=false;
            }
          }
          else{
            if(!walk_tree_and_check_sizeof_statements(NULL,phrase_selection_statement->phrase_else_clause->child.phrase_statement,struct_table,tu)){
              ret=false;
            }
          }
        }
      }
      else{
        if(!check_sizeof_statement(phrase_statement_sequence->phrase_statement,struct_table,tu)){
          ret=false;
        }
      }
      phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
    }
  }
  return ret;
}

bool sizeof_statements_ok(struct_definition_table *struct_table, translation_unit *tu){
  bool ret=true;
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  if(!walk_tree_and_check_sizeof_statements(phrase_statement_sequence,NULL,struct_table,tu)){
    return false;
  }
  return ret;
}

bool entrypoint_exists(sym_table *symbol_table){
  bool found_symbol;
  lookup_in_symbol_table("main",symbol_table,NULL,false,&found_symbol);
  if(!found_symbol){
    fprintf(stderr,"error: no entrypoint found\n");
    return false;
  }
  return true;
}
