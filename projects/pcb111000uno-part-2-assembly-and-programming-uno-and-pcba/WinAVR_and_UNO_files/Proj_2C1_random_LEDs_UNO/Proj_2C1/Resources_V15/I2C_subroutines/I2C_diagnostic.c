

void I2C_Rx_get_version(char);
void check328cal (void);
void I2C_TX_328_check(void);
void PCB_test(void);
void scan_328_cal_factors(void);
void shred_328_cal(void);
void Char_to_PC(char);

void I2C_Tx_initiate_mode(char);

/************************************************************************/
void I2C_Rx_get_version(char str_type){
char num_bytes=1; char mode='P';
char s[2];

s[0]= str_type; s[1]=0;
I2C_Tx(num_bytes,mode, s);
waiting_for_I2C_master;
num_bytes = (receive_byte_with_Ack() - '0') * 10;
num_bytes += (receive_byte_with_Ack() - '0');
for (int m = 0; m < num_bytes; m++){
if (m ==(num_bytes-1)){Char_to_PC(receive_byte_with_Nack());}
else {Char_to_PC(receive_byte_with_Ack());}}
TWCR = (1 << TWINT);}



/************************************************************************/
void check328cal (void){									//MODE NOW DISCONTINUED

unsigned char  OSCCAL_WV;  
int cal_error;
unsigned char  New_UC_value;  

I2C_Tx_initiate_mode('X');								//Cal spot check
waiting_for_I2C_master;									//Receive table giving 40 calibration error results
OSCCAL_WV = receive_byte_with_Ack();					//I2C_slave_receive();
cal_error = receive_byte_with_Ack();					//I2C_slave_receive(); 	
cal_error = (cal_error << 8) + receive_byte_with_Nack();//I2C_slave_receive();
clear_I2C_interrupt;
String_to_PC("\r\nATMEGA 328");
String_to_PC("    Cal factor ");Num_to_PC(16, OSCCAL_WV);Char_to_PC('\t');
String_to_PC("error ");Num_to_PC(10, cal_error);


String_to_PC ("\r\nQuick cal? y or AOK\r\n");
if(waitforkeypress() == 'y'){

String_to_PC ("Temp cal factor ");

I2C_Tx_initiate_mode('Z');
waiting_for_I2C_master;										//Confirm User Cal has been correctly saved to EEPROM
New_UC_value = receive_byte_with_Ack();						//I2C_slave_receive();

cal_error = receive_byte_with_Ack();						//I2C_slave_receive();
cal_error = (cal_error << 8) + receive_byte_with_Nack();	//I2C_slave_receive();
clear_I2C_interrupt;

Num_to_PC(16, New_UC_value);Char_to_PC('\t');
String_to_PC ("error  "); Num_to_PC(10, cal_error); 
newline();
}}



/************************************************************************/
void I2C_TX_328_check(void){			//Tests to see whether or not user wants to chech version info
char num_bytes=0;						//or test clock calibration 
char mode = 'V';

waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT) | (1 << TWEN);		//clear interrupt and leave I2C slave active
TWCR |= (1 << TWEA) | (1 << TWIE);} 	//Slave mode with interrupt and Enable Acknowledge



/************************************************************************/
void PCB_test(void){					
char num_bytes=0;		 
char mode = 'O';

waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT);}



/************************************************************************/
void scan_328_cal_factors(void){					
char num_bytes=0;		 
char mode = 'M';

waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT);}



/************************************************************************/
void shred_328_cal(void){
I2C_Tx_initiate_mode('Y');}

