
//#include "../../Resources_V15/Header_files/I_O_from_switches_UNO.h"


void I2C_Tx_long(long);
void I2C_Tx_real_num(long);
void I2C_Tx_float_num(long, char);
//void I2C_Tx_float_display_control(void);
long I2C_displayToNum(void);						//was I2C_displayToBinary

void I2C_Tx_Initiate_tables(char*);
void I2C_Tx_Inc_tables(char*);
void I2C_Tx_dec_tables(char*);

void I2C_Tx_Uarithmetic_OP(char);
void I2C_Tx_2URNs_from_IO(char*);
void I2C_Tx_accumulator_op(char*,char);

void send_byte_with_Ack(char);
void send_byte_with_Nack(char);
unsigned char receive_byte_with_Ack(void);
unsigned char receive_byte_with_Nack(void);
void I2C_Rx_RN(unsigned long *, signed char *);

void Num_to_PC(char, long);
void String_to_PC(const char*);
void Exp_to_PC (signed char);

/************************************************************************/
void I2C_Tx_long(long L_number){
char s[4];
char num_bytes=4; char mode=6;
for(int m=0; m<=3; m++){s[m] = (L_number >> (8*(3-m)));}
I2C_Tx(num_bytes,mode, s);}


/************************************************************************/
void I2C_Tx_real_num(long L_number){
char s[4];
char num_bytes=4; char mode='J';
for(int m=0; m<=3; m++){s[m] = (L_number >> (8*(3-m)));}
I2C_Tx(num_bytes,mode, s);}

/************************************************************************/
void I2C_Tx_float_num(long L_number, char expnt){
char s[5];
char num_bytes=5; char mode='K';
for(int m=0; m<=3; m++){s[m] = (L_number >> (8*(3-m)));}
s[4] = expnt;
I2C_Tx(num_bytes,mode, s);}



/************************************************************************/
long I2C_displayToNum(void){		
long L_number = 0;
unsigned char receive_byte;
char num_bytes=0;
char mode = 'I';

waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT) | (1 << TWEN);			//clear interrupt and leave I2C active
waiting_for_I2C_master;

for (int m = 0; m<=3; m++){				//Receive 4 chars and assemble into unsigned long result
if (m ==3){receive_byte = receive_byte_with_Nack();}
else {receive_byte = receive_byte_with_Ack();}
switch(m){
case 0: case 1: case 2:L_number =  L_number + receive_byte; 
L_number = L_number << 8; break;
case 3: L_number =  L_number + receive_byte; break;}}
clear_I2C_interrupt;
return L_number;}


/************************************************************************/
void I2C_Tx_Initiate_tables(char s[]){									//was I2C_Tx_1
char num_bytes=8; char mode=7;
I2C_Tx(num_bytes,mode, s);}

/************************************************************************/
void I2C_Tx_Inc_tables(char s[]){
char num_bytes=8; char mode=7;
s[7] = '1';
I2C_Tx(num_bytes,mode, s);}

/************************************************************************/
void I2C_Tx_dec_tables(char s[]){
char num_bytes=8; char mode=7;
s[7] = '2';
I2C_Tx(num_bytes,mode, s);}


/************************************************************************/
void I2C_Tx_Uarithmetic_OP(char Op){
unsigned long RHSofDP; 
signed char res_exp;
char num_bytes=1; char mode=9;
char s[2];
RHSofDP=0; res_exp=0;
s[0] = Op;
I2C_Tx(num_bytes,mode,s);
if(s[0] == 'A')return;{
I2C_Rx_RN(&RHSofDP, &res_exp);
String_to_PC("\r\n0.");Num_to_PC(10,RHSofDP); Exp_to_PC(res_exp);} }


/************************************************************************/
void I2C_Tx_2URNs_from_IO(char s[]){								//Two unsigned real numbers to display
char num_bytes=8; char mode=8;
I2C_Tx(num_bytes,mode, s);}


/************************************************************************/
void I2C_Tx_accumulator_op(char s[],char Op){			//was I2C_Tx_5
char num_bytes, mode;

switch(Op){

case 'I': for (int m = 0; m < 8; m++)s[m] = 0;
s[0] = '0'; num_bytes=8; mode = 4;
I2C_Tx(num_bytes,mode, s); break;
case 'D': num_bytes=8; mode = 'B';
I2C_Tx(num_bytes,mode, s); break;
case 'N': for (int m = 0; m < 8; m++)s[m] = 0;
s[0] = '0'; num_bytes=8; mode = 'B';
I2C_Tx(num_bytes,mode, s); break;
default: mode = 'A'; num_bytes=9;
s[8] = Op; I2C_Tx(num_bytes,mode, s); break;}}


/************************************************************************/
/*I2C_Tx_float_display_control(void){
char PCMSK0_backup, PCMSK2_backup, float_display_mode = '0';
PCMSK0_backup= PCMSK0;
PCMSK2_backup= PCMSK2;
if (PCIenabled){disable_pci_on_sw2;disable_pci_on_sw1_and_sw3;}		
while(1){
if(switch_2_down)float_display_mode = '1'; else float_display_mode = '0';
if((switch_1_down)||(switch_3_down))float_display_mode = '2';
waiting_for_I2C_master;
send_byte_with_Nack(float_display_mode);
clear_I2C_interrupt;
if(float_display_mode == '2')break;}
PCMSK0 = PCMSK0_backup;
PCMSK2 = PCMSK2_backup;}*/



