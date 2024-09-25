#ifndef TOC_LIBS_H
#define TOC_LIBS_H

typedef struct s_libs {
  ysm_c *lib_name;
  ysm_l refi_address;
  ysm_ui align_pad;
  ysm_l address;
  struct s_libs *next;
} libs;

#endif
