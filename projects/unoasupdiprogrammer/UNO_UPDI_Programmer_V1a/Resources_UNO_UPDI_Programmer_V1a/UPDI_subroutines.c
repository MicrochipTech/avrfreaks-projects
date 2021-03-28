
//All subroutines closely follow the ATtiny data sheet



void send_key(const char*);
void Read_NVM_Reg(int, char);
char txt2bin(char A, char B);


/**********************************************************************************************************/
void send_key(const char *Key){						
UART_Tx(0x55);
UART_Tx (send64bitKey_cmd);
char key_h, key_l;
for(int m = 0; m <= 7; m++){
key_h = *(Key + 2*(8 - m));
key_l = *(Key + 2*(8 - m) + 1);
UART_Tx(txt2bin(key_h,key_l));}}




/**********************************************************************************************************/
void Erase_code (void){								
send_key(Key_chip_erase);


UART_Tx(0x55);
UART_Tx(LCDS | ASI_KEY_STATUS_reg);          		/*Test Chip erase bit 3 of ASI_KEY_STATUS register (reg 7)*/
if(UART_Rx() & 0x08);                       		/*should be 0x08*/
else sendString("\r\nChip erase failed");

UPDI_reset;

UPDI_timeout = 0;
Timer_T1_sub_with_interrupt(T1_delay_1Sec);
sei();
do {                                    			/*wait for chip erase to complete*/
Timer_T0_sub(T0_delay_200us);         				// Estimated to takes just under 5ms
UART_Tx(0x55);
UART_Tx(LCDS | ASI_SYS_STATUS_reg);     
Rx_Byte = UART_Rx();								/*Test lockstatus, bit 0 of SYS_STATUS register (reg B)*/
test += 1;
if(UPDI_timeout)break;
}while(Rx_Byte & 0x01);                 			/*Exit when lock status bit is zero*/
cli();

if (UPDI_timeout) 
sendString("\r\nFailure: Device still locked\t");
else sendString("\r\nDevice unlocked\t\t\t");
//sendHex(16, Rx_Byte);
//sendHex(10, test);
}



/**********************************************************************************************************/
char set_up_NVM_prog(void){							
send_key(Key_NVM_prog);
UART_Tx(0x55);
UART_Tx(LCDS | ASI_KEY_STATUS_reg);           		//Test NVM_prog bit 4 of ASI_KEY_STATUS register (reg 7)
UART_Rx();

UPDI_reset;

UPDI_timeout = 0;
Timer_T1_sub_with_interrupt(T1_delay_1Sec);
sei();
do {                                    			//wait for chip erase to complete
Timer_T0_sub(T0_delay_200us);           			//Estimated to takes just under 5ms*/
UART_Tx(0x55);
UART_Tx(LCDS | ASI_SYS_STATUS_reg);     			//Test lockstatus, bit 0 of SYS_STATUS register (reg B)
Rx_Byte = UART_Rx();
test += 1;
if(UPDI_timeout)break;
}while(!(Rx_Byte & 0x08));                 		//Exit when lock status bit is zero
cli();

if (UPDI_timeout) 
{sendString("NVMPROG failure: \t");return 0;}
else {sendString("Ready for NVM programming\t");
return 1;}

newline();
}




/**********************************************************************************************************/
void read_out_signature_bytes(void){				
int start_add = 0x1100;
for(int m = 0; m<=2; m++){
UART_Tx(0x55);
UART_Tx(LDS_from | word_address);
UART_Tx(start_add++);
UART_Tx(start_add >> 8);
sendHex(16, UART_Rx());
Timer_T0_sub(T0_delay_200us);}}





/**********************************************************************************************************/
void write_fuse(int fuse_add, unsigned char value){		

unsigned int Rx_byte;


UART_Tx(0x55);                              			//Send lock bit address to NVMCTRL.ADD registers
UART_Tx(STS | byte_data);								                               
UART_Tx(NVMCTRL_ADDR_reg);                              //Address of NVMCTRL.ADD registers                        
UART_Tx(NVMCTRL_ADDR_reg >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(fuse_add);                                		//Adddress of lock byte
UART_Rx();
Timer_T0_sub(T0_delay_200us);

UART_Tx(0x55);                              			//Send lock bit address to NVMCTRL.ADD registers
UART_Tx(STS | byte_data);			                                
UART_Tx(NVMCTRL_ADDR_reg + 1);                          //Address of NVMCTRL.ADD registers                        
UART_Tx(NVMCTRL_ADDR_reg >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(fuse_add >> 8); 
UART_Rx();                               				//Adddress of lock byte
Timer_T0_sub(T0_delay_200us);

UART_Tx(0x55);                                			//Send key 0xC5 (device unlocked) to NVMCTRL.DATA register)
UART_Tx(STS | byte_data);								//UART_Tx(0x44);                                
UART_Tx(NVMCTRL_DATA_reg);                          	//Adress of NVMCTRL.DATA registers                       
UART_Tx(NVMCTRL_DATA_reg >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(value); 
UART_Rx();
Timer_T0_sub(T0_delay_200us);

UART_Tx(0x55);                                			//Enter write to fuse byte command into NVM.CTRLA register
UART_Tx(STS | byte_data);								//UART_Tx(0x44);                                
UART_Tx(NVMCTRL_CTRLA);                       			//Register address                         
UART_Tx(NVMCTRL_CTRLA >> 8);
UART_Rx();
Timer_T0_sub(T0_delay_200us);
UART_Tx(cmd_update_fuse);                          		//Command to update the fuses.  CMD is 0x07
UART_Rx();
Timer_T0_sub(T0_delay_5ms);}



/**********************************************************************************************************/
void read_out_fuses(void){								
int start_add = FUSE_base_address - 1;
for(int m = 0; m<=8; m++){
start_add += 1;
  if(m==3)continue;
  if(m==4)continue;
UART_Tx(0x55);
UART_Tx(LDS_from | word_address);
UART_Tx(start_add);
UART_Tx(start_add >> 8);

sendHex(16, UART_Rx());
if(m==5)sendChar('\t');
Timer_T0_sub(T0_delay_200us);}}






/**********************************************************************************************************/
char txt2bin(char A, char B){
A -= '0';
if (A > 9) A -= 7;
B -= '0';
if (B > 9) B -= 7;
return (A * 0x10 + B);}




/**********************************************************************************************************/
void inititalise_UPDI_cmd_write(int add_in_flash){		//Setup repeat command

add_in_flash = (add_in_flash * 2) + flash_start;

SREG_BKP = SREG;
cli();

synch_pulse;
data_byte_Tx = (ST | word_pointer); transmit_byte;
data_byte_Tx = (add_in_flash); transmit_byte;
data_byte_Tx = (add_in_flash >> 8); transmit_byte;
receive_byte; Stop_clock;

synch_pulse;
data_byte_Tx = (setup_repeat_op); transmit_byte;
data_byte_Tx = (block_SZ - 1); transmit_byte;
Stop_clock;

synch_pulse;
data_byte_Tx = (ST | inc_byte_ptr); transmit_byte;

Stop_clock;

SREG = SREG_BKP;
}



/**********************************************************************************************************/
void UPDI_cmd_to_page_buffer(void){

get_next_hex_cmd(); 

SREG_BKP = SREG;
cli();	

Restart_clock;
data_byte_Tx = (Hex_cmd); transmit_byte;

receive_byte; Stop_clock;

Restart_clock;
data_byte_Tx = (Hex_cmd >> 8); transmit_byte;
receive_byte; Stop_clock;

SREG = SREG_BKP;
}



/**********************************************************************************************************/
void Write_page_to_NVM(int PA){							//PA is page address  Interrupts enabled

PA = (PA * 2) + flash_start;							//Convert cmd address to byte address

SREG_BKP = SREG;
cli();	

synch_pulse;
data_byte_Tx = (STS | int_data); transmit_byte;
data_byte_Tx = byte(NVMCTRL_ADDR_reg); transmit_byte;
data_byte_Tx = (NVMCTRL_ADDR_reg >> 8); transmit_byte;
receive_byte; Stop_clock;

Restart_clock;
data_byte_Tx = (PA); transmit_byte;
data_byte_Tx = (PA >> 8); transmit_byte;
receive_byte; Stop_clock;

sendChar('*');

synch_pulse;
data_byte_Tx = (STS | byte_data); transmit_byte;
data_byte_Tx = byte(NVMCTRL_CTRLA); transmit_byte;
data_byte_Tx = (NVMCTRL_CTRLA >> 8); transmit_byte;
receive_byte; Stop_clock;

Restart_clock;
data_byte_Tx = (WP_cmd); transmit_byte;
receive_byte; Stop_clock;
SREG = SREG_BKP;
Timer_T2_sub(T2_delay_3ms);}

