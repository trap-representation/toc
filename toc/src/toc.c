/* assumptions:
   The setlocale() function is never used (except the implicit setlocale(LC_ALL,"C"); at program startup)
   The character set of the execution and source environment only consists of character-identifier and punctuator as defined in the Chlore grammar
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <search.h>
#include <stdint.h>
#include <stdbool.h>

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H
#include "conv.h"
#include "tu_list.h"
#include "comp_properties.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "tables.h"
#include "code_gen.h"
#include "clean_up_routines.h"
#include "resolve_imports.h"
#include "verify.h"

_Static_assert(_Alignof(ysm_l)%_Alignof(ysm_us)==0 && _Alignof(ysm_l)%_Alignof(ysm_ui)==0 && _Alignof(ysm_l)%_Alignof(ysm_s)==0 && _Alignof(ysm_l)%_Alignof(ysm_i)==0,"static assert failure in " __FILE__);

compilation_attributes comp_attr;

int main(int argc, char *argv[]){
  comp_attr.std=std_chlore2x_toc;
  comp_attr.out_fileformat=oform_esff23x;
  comp_attr.pic=0;
  comp_attr.outfile="a";
  comp_attr.code_alignment=ALIGNOF_L;
  comp_attr.heap_alignment=ALIGNOF_L;
  comp_attr.heap_size=0x6400;
  comp_attr.stack_size=0x4000;
  libs *libraries=NULL;
  libs *libraries_trav=NULL;
  char *stdlib_pathname="/usr/local/lib/elibchlore.so";
  char *infile="a.chlore";
  char **in_file_list=&infile;
  int in_file_n=1;
  for(int i=1;i<argc;i++){
    if(strcmp(argv[i],"-i")==0 || strcmp(argv[i],"--input")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected [file...] after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else if(strcmp(argv[i],"@-")!=0){
        in_file_list=&argv[i];
        i++;
        for(;i<argc && strcmp(argv[i],"@-")!=0;i++){
          in_file_n++;
        }
      }
    }
    else if(strcmp(argv[i],"-l")==0 || strcmp(argv[i],"--load")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected [file...] after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else{
        for(int j=i;j<argc && strcmp(argv[j],"@-")!=0;j++){
          if(libraries==NULL){
            if((libraries=malloc(sizeof(libs)))==NULL){
              fprintf(stderr,"implementation error: failed to allocate enough memory\n");
              return 1;
            }
            libraries_trav=libraries;
          }
          else{
            if((libraries_trav->next=malloc(sizeof(libs)))==NULL){
              fprintf(stderr,"implementation error: failed to allocate enough memory\n");
              clean_up_libraries(libraries);
              return 1;
            }
            libraries_trav=libraries_trav->next;
          }
          if((libraries_trav->lib_name=str2cseq(argv[i]))==NULL){
            fprintf(stderr,"implementation error: failed to allocate enough memory\n");
            clean_up_libraries(libraries);
            return 1;
          }
          libraries_trav->next=NULL;
        }
      }
    }
    else if(strcmp(argv[i],"-o")==0 || strcmp(argv[i],"--out")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected a file after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else{
        comp_attr.outfile=argv[i];
      }
    }
    else if(strcmp(argv[i],"-oform")==0 || strcmp(argv[i],"--out-fileformat")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected an outfileformat after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else{
        if(strcmp(argv[i],"ESFF23")==0){
          comp_attr.out_fileformat=oform_esff23;
        }
        else if(strcmp(argv[i],"ESFF23x")==0){
          comp_attr.out_fileformat=oform_esff23x;
        }
        else{
          fprintf(stderr,"implementation error: not a valid outfileformat %s\n",argv[i]);
          clean_up_libraries(libraries);
          return 1;
        }
      }
    }
    else if(strcmp(argv[i],"-std")==0 || strcmp(argv[i],"--standard")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected a standardtype after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else{
        if(strcmp(argv[i],"non-conforming")==0){
          comp_attr.std=std_nc;
        }
        else if(strcmp(argv[i],"chlore2x")==0){
          comp_attr.std=std_chlore2x;
        }
        else if(strcmp(argv[i],"chlore2x+toc")==0){
          comp_attr.std=std_chlore2x_toc;
        }
        else{
          fprintf(stderr,"implementation error: not a valid standardtype %s\n",argv[i]);
          clean_up_libraries(libraries);
          return 1;
        }
      }
    }
    else if(strcmp(argv[i],"-ca")==0 || strcmp(argv[i],"--code-alignment")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected an unsignedinteger after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else{
        unsigned int ret;
        comp_attr.code_alignment=str_to_ui(argv[i],&ret);
        if(ret==1){
          fprintf(stderr,"implementation error: expected an unsignedinteger after %s\n",argv[i-1]);
          clean_up_libraries(libraries);
          return 1;
        }
        else if(comp_attr.code_alignment==0){
          fprintf(stderr,"implementation error: alignment of the code cannot be 0\n");
          clean_up_libraries(libraries);
          return 1;
        }
      }
    }
    else if(strcmp(argv[i],"-ha")==0 || strcmp(argv[i],"--heap-alignment")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected an unsignedinteger after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else{
        unsigned int ret;
        comp_attr.heap_alignment=str_to_ui(argv[i],&ret);
        if(ret==1){
          fprintf(stderr,"implementation error: expected an unsignedinteger after %s\n",argv[i-1]);
          clean_up_libraries(libraries);
          return 1;
        }
        else if(comp_attr.heap_alignment==0){
          fprintf(stderr,"implementation error: alignment of the heap cannot be 0\n");
          clean_up_libraries(libraries);
          return 1;
        }
      }
    }
    else if(strcmp(argv[i],"-ssz")==0 || strcmp(argv[i],"--stack-size")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected an unsignedinteger after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else{
        unsigned int ret;
        comp_attr.stack_size=str_to_uns(argv[i],&ret);
        if(ret==1){
          fprintf(stderr,"implementation error: expected an unsignedinteger after %s\n",argv[i-1]);
          clean_up_libraries(libraries);
          return 1;
        }
        else if(comp_attr.stack_size>(nightvm_uns)NVMUNS_MAX){
          fprintf(stderr,"implementation error: stack too large to be completely addressable\n");
          clean_up_libraries(libraries);
          return 1;
        }
      }
    }
    else if(strcmp(argv[i],"-hsz")==0 || strcmp(argv[i],"--heap-size")==0){
      i++;
      if(i==argc){
        fprintf(stderr,"implementation error: expected an unsignedinteger after %s\n",argv[i-1]);
        clean_up_libraries(libraries);
        return 1;
      }
      else{
        unsigned int ret;
        comp_attr.heap_size=str_to_uns(argv[i],&ret);
        if(ret==1){
          fprintf(stderr,"implementation error: expected an unsignedinteger after %s\n",argv[i-1]);
          clean_up_libraries(libraries);
          return 1;
        }
        else if(comp_attr.heap_size>0x3fffffffffffffff){
          fprintf(stderr,"implementation error: heap too large to be completely addressable\n");
          clean_up_libraries(libraries);
          return 1;
        }
      }
    }
    else if(strcmp(argv[i],"--nostdlib")==0 || strcmp(argv[i],"-nstd")==0){
      comp_attr.nostdlib=1;
    }
    else if(strcmp(argv[i],"--position-independent")==0 || strcmp(argv[i],"-pic")==0){
      comp_attr.pic=1;
    }
    else if(strcmp(argv[i],"-c")==0 || strcmp(argv[i],"--credits")==0){
      fprintf(stderr,"\
    Copyright (C) 2023  Somdipto Chakraborty\n\n\
\
    This program is free software: you can redistribute it and/or modify\n\
    it under the terms of the GNU General Public License as published by\n\
    the Free Software Foundation, either version 3 of the License, or\n\
    (at your option) any later version.\n\n\
\
    This program is distributed in the hope that it will be useful,\n\
    but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
    GNU General Public License for more details.\n\n\
\
    You should have received a copy of the GNU General Public License\n\
    along with this program.  If not, see <https://www.gnu.org/licenses/>.\n");
      clean_up_libraries(libraries);
      return 0;
    }
    else if(strcmp(argv[i],"-a")==0 || strcmp(argv[i],"--about")==0){
      fprintf(stderr,"\
    Toc is an implementation of the Chlore programming language.\n\
    This program is licensed under GPLv3 Copyright (C) 2023 Somdipto Chakraborty.\n");
      clean_up_libraries(libraries);
      return 0;
    }
    else if(strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0){
      fprintf(stderr,"    --help (-h): display this message and quit\n");
      fprintf(stderr,"*   --input (-i) [file...]: specify the input file (default: %s); if more than one file is present in the sequence, they are all linked\n",infile);
      fprintf(stderr,"    --out (-) file: specify the output file\n");
      fprintf(stderr,"    --out-fileformat (-oform) outfileformat: specify the output file format (default: ESFF23x)\n");
      fprintf(stderr,"*   --load (-l) [file...]: specify one or more libraries that will be opened by nightVM at startup\n");
      fprintf(stderr,"    --nostdlib (-nstd): do not load the standard library\n");
      fprintf(stderr,"    --standard (-std) standardtype: specify a chlore standard (default: non-conforming)\n");
      fprintf(stderr,"    --position-independent (-pic): generate position independent code\n");
      fprintf(stderr,"    --code-alignment (-ca) unsignedinteger: specify the alignment of the code (default: %" PRIYSMUI " bytes)\n",comp_attr.code_alignment);
      fprintf(stderr,"    --heap-alignment (-ha) unsignedinteger: specify the alignment of the heap (default: %" PRIYSMUI " bytes)\n",comp_attr.heap_alignment);
      fprintf(stderr,"    --stack-size (-ssz) unsignedinteger: specify the stack size (default: %" PRINVMxUNS ")\n",comp_attr.stack_size);
      fprintf(stderr,"    --heap-size (-hsz) unsignedinteger: specify the heap size (default: %" PRINVMxUNS ")\n",comp_attr.heap_size);
      fprintf(stderr,"    --credits (-c): display license information and quit\n");
      fprintf(stderr,"    --about (-a): display information about this implementation and quit\n\n");

      fprintf(stderr,"    in the aforementioned list, certain arguments that can be followed by a sequence of arguments are marked with an asterisk (*); those can end with @- to denote the end of the sequence\n");
      fprintf(stderr,"    for a normative version of this list, read the toc manual\n");
      clean_up_libraries(libraries);
      return 0;
    }
    else{
      fprintf(stderr,"implementation error: %s is an invalid argument; use the --help (-h) argument for a list of valid arguments\n",argv[i]);
      clean_up_libraries(libraries);
      return 1;
    }
  }
  if(!comp_attr.nostdlib){
    if(libraries==NULL){
      if((libraries=malloc(sizeof(libs)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        return 1;
      }
      libraries_trav=libraries;
    }
    else{
      if((libraries_trav->next=malloc(sizeof(libs)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        clean_up_libraries(libraries);
        return 1;
      }
      libraries_trav=libraries_trav->next;
    }
    if((libraries_trav->lib_name=str2cseq(stdlib_pathname))==NULL){
      fprintf(stderr,"implementation error: failed to allocate enough memory\n");
      clean_up_libraries(libraries);
      return 1;
    }
    libraries_trav->next=NULL;
  }
  srand(time(NULL));
  translation_unit_list *tu_list=NULL;
  translation_unit_list *trav_tu_list=NULL;
  struct_definition_table *struct_table=NULL;
  struct_definition_table *last_struct_table_node=NULL;
  str_table *string_table=NULL;
  str_table *last_string_table_node=NULL;
  sym_table *symbol_table=NULL;
  sym_table *last_symbol_table_node=NULL;
  token *tokens;
  unsigned int ret;
  for(int i=0;i<in_file_n;i++){
    tokens=lex(in_file_list[i],&ret);
    if(ret==1){
      clean_up_libraries(libraries);
      clean_up_tokens_list(tokens);
      clean_up_translation_unit_list(tu_list);
      return 1;
    }
    if(tokens==NULL){
      fprintf(stderr,"error: empty translation unit\n");
      clean_up_libraries(libraries);
      clean_up_translation_unit_list(tu_list);
      return 0;
    }
    translation_unit *tu=parse(tokens,&ret,in_file_list[i]);
    if(ret==1){
      clean_up_libraries(libraries);
      clean_up_tokens_list(tokens);
      clean_up_translation_unit(tu);
      clean_up_translation_unit_list(tu_list);
      return 1;
    }
    if(tu_list==NULL){
      if((tu_list=malloc(sizeof(translation_unit)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        clean_up_libraries(libraries);
        clean_up_tokens_list(tokens);
        clean_up_translation_unit(tu);
        return 1;
      }
      trav_tu_list=tu_list;
    }
    else{
      if((trav_tu_list->next_tu=malloc(sizeof(translation_unit)))==NULL){
        fprintf(stderr,"implementation error: failed to allocate enough memory\n");
        clean_up_libraries(libraries);
        clean_up_tokens_list(tokens);
        clean_up_translation_unit(tu);
        clean_up_translation_unit_list(tu_list);
        return 1;
      }
      trav_tu_list=trav_tu_list->next_tu;
    }
    trav_tu_list->tu=tu;
    trav_tu_list->next_tu=NULL;
    clean_up_tokens_list(tokens);
    if(resolve_imports(tu)){
      clean_up_libraries(libraries);
      clean_up_translation_unit_list(tu_list);
      return 1;
    }
    str_table *t_string_table=create_str_table(&last_string_table_node,string_table,tu,&ret);
    if(string_table==NULL){
      string_table=t_string_table;
    }
    if(ret==1){
      clean_up_libraries(libraries);
      clean_up_string_table(string_table);
      clean_up_translation_unit_list(tu_list);
      return 1;
    }
    struct_definition_table *t_struct_table=create_struct_definition_table(&last_struct_table_node,struct_table,tu,&ret);
    if(struct_table==NULL){
      struct_table=t_struct_table;
    }
    if(ret==1){
      clean_up_libraries(libraries);
      clean_up_string_table(string_table);
      clean_up_struct_definition_table(struct_table);
      clean_up_translation_unit_list(tu_list);
      return 1;
    }
  }
  ysm_l address=0;
  translation_unit_list *l_tu=tu_list;
  while(l_tu!=NULL){
    sym_table *t_symbol_table=create_sym_table(&last_symbol_table_node,symbol_table,l_tu->tu,string_table,libraries,&address,&ret);
    if(symbol_table==NULL){
      symbol_table=t_symbol_table;
    }
    if(ret==1){
      clean_up_libraries(libraries);
      clean_up_string_table(string_table);
      clean_up_symbol_table(symbol_table);
      clean_up_struct_definition_table(struct_table);
      clean_up_translation_unit_list(tu_list);
      return 1;
    }
    l_tu=l_tu->next_tu;
  }
  if(!entrypoint_exists(symbol_table)){
    ret=1;
  }  
  if(!no_dup_tag_in_struct_tag_definition_sequence(struct_table)){
    ret=1;
  }
  trav_tu_list=tu_list;
  while(trav_tu_list!=NULL){
    if(!tagged_statement_constructs_ok(struct_table,trav_tu_list->tu)){
      ret=1;
    }
    if(!no_use_of_unset_symbols_in_push_statements(symbol_table,trav_tu_list->tu)){
      ret=1;
    }
    if(!alignof_statements_ok(struct_table,trav_tu_list->tu)){
      ret=1;
    }
    if(!sizeof_statements_ok(struct_table,trav_tu_list->tu)){
      ret=1;
    }
    if(ret==1){
      clean_up_libraries(libraries);
      clean_up_string_table(string_table);
      clean_up_symbol_table(symbol_table);
      clean_up_struct_definition_table(struct_table);
      clean_up_translation_unit_list(tu_list);
      return 1;
    }
    trav_tu_list=trav_tu_list->next_tu;
  }
  unsigned int exit_code=gen_code_translation_unit_list(tu_list,string_table,symbol_table,struct_table,libraries);
  clean_up_libraries(libraries);
  clean_up_string_table(string_table);
  clean_up_symbol_table(symbol_table);
  clean_up_struct_definition_table(struct_table);
  clean_up_translation_unit_list(tu_list);
  return exit_code;
}
