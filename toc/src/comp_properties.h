#ifndef TOC_COMP_PROPERTIES_H
#define TOC_COMP_PROPERTIES_H

#include "nvm_sources.h"
#include TOC_NVM_IMPLEMENTATION_H

enum standard_type {
  std_nc,
  std_chlore2x,
  std_chlore2x_toc
};

enum output_format {
  oform_esff23,
  oform_esff23x
};

typedef struct s_compilation_attributes {
  char *outfile;
  enum output_format out_fileformat;
  enum standard_type std;
  bool nostdlib;
  bool pic;
  ysm_ui code_alignment;
  ysm_ui heap_alignment;
  nightvm_uns heap_size;
  nightvm_uns stack_size;
} compilation_attributes;

#endif
