// prototypes
void ihex_init(void);
void ihex_char(char);

#define IHEX_STATUS_EOF 1
#define IHEX_STATUS_ERROR 2

// output
	extern unsigned long 	ihex_address;		//actual data address = (ext_lin_add<<16)+(ext_seg_add<<4)+address;
	extern unsigned char 	ihex_byte_count;	//number of data bytes available
	extern char 			ihex_data[16];
	extern unsigned char	ihex_status;		//
	