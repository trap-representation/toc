#ifndef TOC_LIBS_H
#define TOC_LIBS_H

typedef struct s_libs {
  nightVM_c *lib_name;
  nightVM_l refi_address;
  nightVM_ui align_pad;
  nightVM_l address;
  struct s_libs *next;
} libs;

#endif
