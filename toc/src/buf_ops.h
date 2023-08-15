#ifndef TOC_BUF_OPS_H
#define TOC_BUF_OPS_H

extern unsigned int flush_to_buf(char *buf, size_t *pos, char **write_buf, size_t *write_buf_written_contents);

extern unsigned int append_to_buf(char add_char, char *buf, size_t *pos, size_t buf_max, bool write_thresh, char **write_buf, size_t *write_buf_written_contents);

#endif
