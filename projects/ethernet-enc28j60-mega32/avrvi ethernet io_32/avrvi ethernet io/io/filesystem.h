#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include "../net/tcp.h"
#include "../util/string.h"

void filesystem_init(void);
unsigned int filesystem_search_file(unsigned char *filename, unsigned char *filename_ext);
void filesystem_delete_file(unsigned int fileid);
void filesystem_format(void);
//void filesystem_list_files(void);
unsigned int filesystem_allocate_fileid(void);
void filesystem_close_file(void);
unsigned int filesystem_allocate_block(void);

unsigned int filesystem_open_file_wr(unsigned char *filename, unsigned char *filename_ext);
unsigned int filesystem_open_file_rd(unsigned char *filename, unsigned char *filename_ext);

void filesystem_write_data(unsigned char* data, unsigned int len, unsigned char last);
void filesystem_store_current_block(unsigned char last);
unsigned int filesystem_generate_index(unsigned char *buffer, unsigned int datapos, unsigned int *len, unsigned int *offset);
unsigned char filesystem_seek_to(unsigned int pos);
unsigned char filesystem_get_next_byte(unsigned char *data);
unsigned int filesystem_get_data(unsigned char *buffer, unsigned int datapos, unsigned int len, unsigned long streampos, unsigned int file_id, unsigned int *data_ptr);
unsigned int filesystem_get_data_startat(unsigned char *buffer, unsigned int datapos, unsigned int len, unsigned int *data_ptr, unsigned int inside_block_pos);
#endif
