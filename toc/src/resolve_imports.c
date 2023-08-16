#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H
#include "parser.h"
#include "clean_up_routines.h"
#include "lexer.h"
#include "resolve_imports.h"

unsigned int resolve_imports(translation_unit *tu){
  statement_sequence *phrase_statement_sequence=tu->phrase_statement_sequence;
  while(phrase_statement_sequence!=NULL){
    if(phrase_statement_sequence->phrase_statement->child_type==phrase_type_import_statement){
      char *file_name=phrase_statement_sequence->phrase_statement->child.phrase_import_statement->string_constant;
      if(strcmp(file_name,"stdlib.chloreh")==0){
        file_name="/usr/local/include/stdlib.chloreh";
      }
      else if(strcmp(file_name,"io.chloreh")==0){
        file_name="/usr/local/include/io.chloreh";
      }
      else if(strcmp(file_name,"alloc2.chloreh")==0){
        file_name="/usr/local/include/alloc2.chloreh";
      }
      else if(strcmp(file_name,"math.chloreh")==0){
        file_name="/usr/local/include/math.chloreh";
      }
      else if(strcmp(file_name,"stddef.chloreh")==0){
        file_name="/usr/local/include/stddef.chloreh";
      }
      else if(strcmp(file_name,"string.chloreh")==0){
        file_name="/usr/local/include/string.chloreh";
      }
      else if(strcmp(file_name,"exit.chloreh")==0){
        file_name="/usr/local/include/exit.chloreh";
      }
      else if(strcmp(file_name,"types.chloreh")==0){
        file_name="/usr/local/include/types.chloreh";
      }
      else if(strcmp(file_name,"sys.chloreh")==0){
        file_name="/usr/local/include/sys.chloreh";
      }
      else if(strcmp(file_name,"errors.chloreh")==0){
        file_name="/usr/local/include/errors.chloreh";
      }
      else if(strcmp(file_name,"locale.chloreh")==0){
        file_name="/usr/local/include/locale.chloreh";
      }
      else if(strcmp(file_name,"wchar.chloreh")==0){
        file_name="/usr/local/include/wchar.chloreh";
      }
      else if(strcmp(file_name,"socket.chloreh")==0){
        file_name="/usr/local/include/socket.chloreh";
      }
      else if(strcmp(file_name,"watch.chloreh")==0){
        file_name="/usr/local/include/watch.chloreh";
      }
      token *tokens;
      unsigned int ret=0;
      tokens=lex(file_name,&ret);
      if(ret==1){
        clean_up_tokens_list(tokens);
        return 1;
      }
      if(tokens!=NULL){
        statement_sequence *append_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
        statement_sequence *end_statement_sequence;
        token *curr=tokens;
        if((end_statement_sequence=parse_statement_sequence(NULL,phrase_statement_sequence,&curr,file_name))==NULL){
          clean_up_tokens_list(tokens);
          return 1;
        }
        end_statement_sequence->phrase_statement_sequence=append_statement_sequence;
        append_statement_sequence->parent.phrase_statement_sequence=end_statement_sequence;
        clean_up_tokens_list(tokens);
      }
    }
    phrase_statement_sequence=phrase_statement_sequence->phrase_statement_sequence;
  }
  return 0;
}
