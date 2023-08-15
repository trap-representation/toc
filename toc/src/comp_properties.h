#ifndef TOC_COMP_PROPERTIES_H
#define TOC_COMP_PROPERTIES_H

#include "nvm_paths.h"
#include TOC_NVM_IMPLEMENTATION_H

typedef struct s_compilation_attributes {
  char *outfile;
  unsigned long out_fileformat;
  unsigned long std;
  bool nostdlib;
  bool pic;
  nightVM_ui code_alignment;
  nightVM_ui heap_alignment;
  nightVM_uns heap_size;
  nightVM_uns stack_size;
} compilation_attributes;

#define std_nc 0
#define std_chlore2x 1
#define std_chlore2x_toc 2

#define oform_esff23 0

#endif
