

byte mmc_command (byte *dat,word count,byte min_waitcycles,byte max_waitcycles);

byte mmc_init(byte pol, byte pha);
byte mmc_read_block (byte* buff,unsigned long int blocknumber);
//byte mmc_write_block (byte* buff,unsigned long int blocknumber);
void change_byteorder (byte *dat);
void mmc_resume(void);

//byte mmc_write_wait();






