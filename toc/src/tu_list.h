#ifndef TOC_TU_LIST_H
#define TOC_TU_LIST_H

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H
#include "tree.h"

typedef struct s_translation_unit_list {
  translation_unit *tu;
  struct s_translation_unit_list *next_tu;
} translation_unit_list;

#endif
