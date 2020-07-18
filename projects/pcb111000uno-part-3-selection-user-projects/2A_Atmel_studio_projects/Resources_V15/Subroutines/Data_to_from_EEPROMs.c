
/*
All these routines interact with the on-chip EEPROM
Reading/writing to/from a target devive is done in programming mode
*/

void Char_to_EEPROM(int, char);
char Char_from_EEPROM(int);
void Text_to_EEPROM(int*, char);
char Text_from_EEPROM(int*);
void Read_on_chip_EEPROM(int);
void save_cal_bytes(char*, char*);
void restore_cal_bytes(char, char);
void Read_Hello_world_string(void);

void I2C_Tx_initiate_mode(char);

char PRN_8bit_GEN(void);

void Char_to_PC(char);
void newline(void);
void Num_to_PC(char, long);	
void send_byte_with_Ack(char);
void send_byte_with_Nack(char);
unsigned char receive_byte_with_Ack(void);
unsigned char receive_byte_with_Nack(void);


void Char_to_EEPROM(int location, char text){eeprom_write_byte((uint8_t*)(location), text);}
char Char_from_EEPROM(int location){return eeprom_read_byte((uint8_t*) location);}

/****************************************************/
void Text_to_EEPROM(int*w_a, char byte){					//on-chip EEPROM
eeprom_write_byte((uint8_t*)(*w_a),byte); 					//macro provided by winavr
*w_a = *w_a + 1;}


/****************************************************/
char Text_from_EEPROM(int*r_a){
return eeprom_read_byte((uint8_t*)((*r_a)++));}			//macro provided by winavr


/****************************************************/
void Read_on_chip_EEPROM(int EEPROM_address){
char temp_char;
while(1){
temp_char =  eeprom_read_byte((uint8_t*)(EEPROM_address++));   // Read_write_mem('O',(EEPROM_address--),0);
if (temp_char != '\0') Char_to_PC(temp_char);
else {newline(); break;}}}


/****************************************************/
void save_cal_bytes(char*cal_1, char*cal_2){
*cal_1 =eeprom_read_byte((uint8_t*)(0x1FE));
*cal_2 =eeprom_read_byte((uint8_t*)(0x1FF));
Num_to_PC(16,*cal_1); Char_to_PC(' ');
Num_to_PC(16,*cal_2); }


/****************************************************/
void restore_cal_bytes(char cal_1, char cal_2){
eeprom_write_byte((uint8_t*)(0x1FE), cal_1);
eeprom_write_byte((uint8_t*)(0x1FF), cal_2);
Num_to_PC (16,eeprom_read_byte((uint8_t*)(0x1FE)));
Char_to_PC(' ');
Num_to_PC (16,eeprom_read_byte((uint8_t*)(0x1FF)));}


/****************************************************/
void Read_Hello_world_string(void){
unsigned char receive_byte;

I2C_Tx_initiate_mode('H');
waiting_for_I2C_master;									//Initiate comuninations:master to send string
do{receive_byte = receive_byte_with_Ack();				//print out string as characters are received
if(receive_byte)Char_to_PC(receive_byte);}
while(receive_byte);									//Detect '\0' character used to terminate a string
receive_byte_with_Nack();								//Receve a second null char at the end of the string
clear_I2C_interrupt;}									//Complete transaction



