/*

*/

	extern struct fifo_control *sd_debug_fifo_ptr;

	extern unsigned char 	sd_version;
	extern unsigned char 	sd_sdhc;

void sd_init(void);
char sd_initcard(void);
char sd_block_read(char* buffer, unsigned long blockadd);
char sd_block_write(char* buffer, unsigned long blockadd);

