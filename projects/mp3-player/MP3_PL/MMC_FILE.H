
#define READ 1
#define WRITE 2
#define APPEND 3

struct	{	unsigned char sec_in_cluster,first_letter,eof,fast_fat,current_fatsector;
			unsigned int adress_in_sector,current_cluster,next_cluster,dir_entry;
			unsigned long int current_sector,filesize,chars_read,dir_sector;
		}	file;

byte sector[512];

byte mmc_boot();
byte mmc_dir(char* entry,word* startcluster,unsigned long int *filesize,char* extension,word* entrynumber);
byte mmc_open_file (char* filename,byte control);
byte mmc_read_line (char* line,byte max_chars);
byte mmc_write_line (char* buffer,byte buffersize);
byte mmc_close_file();
byte mmc_read_sector();
byte get_id3(char* song,char* artist);








