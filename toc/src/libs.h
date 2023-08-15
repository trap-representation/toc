#ifndef TOC_LIBS_H
#define TOC_LIBS_H

typedef struct s_libs {
  nightVM_c *lib_name;
  nightVM_l loc;
  struct s_libs *next;
} libs;

#endif
