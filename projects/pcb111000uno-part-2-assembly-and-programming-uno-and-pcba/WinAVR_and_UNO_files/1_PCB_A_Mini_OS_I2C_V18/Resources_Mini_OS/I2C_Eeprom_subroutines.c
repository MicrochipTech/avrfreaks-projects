char PRN_8bit_GEN(void);




void Message_from_the_OS(void){

int text_max, eeprom_ptr;
char string_counter, string_no;

if(((eeprom_read_byte((uint8_t*)(0))) == 0xFF) &&\
((eeprom_read_byte((uint8_t*)(1))) == 0xFF))					//No text

{text_max = 7;
eeprom_write_byte((uint8_t*)(5), '?');
eeprom_write_byte((uint8_t*)(6), 0);}

else
{text_max = (eeprom_read_byte((uint8_t*)(0)) <<8) +\
eeprom_read_byte((uint8_t*)(1));}								//Address of end of text section + 1

string_counter = 0;
eeprom_ptr = 5;													//Address of start of text

while(eeprom_ptr < text_max){									//scan text section
if (!(eeprom_read_byte((uint8_t*)(eeprom_ptr))))				//Count the number of strings
string_counter++;
eeprom_ptr++;}
	
string_no =  (PRN_8bit_GEN()%(string_counter)) + 1;			//Generate random string number (between 1 and String_counter)
string_counter=1;
eeprom_ptr = 5;

while(1){if(string_counter==string_no)break;					//Scan text section untill required string is reached
	else if (!(eeprom_read_byte((uint8_t*)(eeprom_ptr))))
	string_counter++;
	eeprom_ptr++;}

Initialise_I2C_master_write;									//Send string

do{
I2C_master_transmit(eeprom_read_byte((uint8_t*)eeprom_ptr));
eeprom_ptr++;}while(eeprom_read_byte((uint8_t*)eeprom_ptr));
	
I2C_master_transmit('\0');
I2C_master_transmit('\0');
TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);}		



/*****************************************************************/
char PRN_8bit_GEN(void){
unsigned int bit;
char lfsr;

lfsr = eeprom_read_byte((uint8_t*)(0x3F3));
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 1;
lfsr = (lfsr >> 1) | (bit << 7);
eeprom_write_byte((uint8_t*)(0x3F3),lfsr);
return lfsr;}




