// spi.h

volatile byte block_complete;

void spi_select(void);
void spi_deselect(void);
void spi_irq_enable();
void spi_irq_disable();
void spi_off();

void spi_init (char pol,char pha,char ds,char data_order,char master,char clock_pattern);
byte spi_master_transfer_byte(byte txd);
char spi_master_transfer_block (char direction,char* buff,word size);	//1=transmit,0=receive, interrupt function mandatory

