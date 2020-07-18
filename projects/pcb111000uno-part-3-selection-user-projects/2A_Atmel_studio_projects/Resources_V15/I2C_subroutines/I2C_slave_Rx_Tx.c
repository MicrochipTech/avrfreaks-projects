
void I2C_Tx_initiate_mode(char);
void I2C_Tx(char, char, char*);
void send_byte_with_Ack(char);
void send_byte_with_Nack(char);
unsigned char receive_byte_with_Ack(void);
unsigned char receive_byte_with_Nack(void);
void I2C_Rx_RN(unsigned long *, signed char *);		//Real numbers only


void I2C_Tx_initiate_mode(char mode){
char num_bytes = 0;
waiting_for_I2C_master;		
send_byte_with_Ack(num_bytes);
send_byte_with_Nack(mode);
TWCR = (1 << TWINT);}



/***********************************************************/
void I2C_Tx(char num_bytes, char mode, char s[]){
waiting_for_I2C_master;
send_byte_with_Ack(num_bytes);
send_byte_with_Ack(mode);
for (int m = 0;  m < num_bytes; m++){
if (m==num_bytes-1){send_byte_with_Nack(s[m]);}
else {send_byte_with_Ack(s[m]);}}
TWCR = (1 << TWINT);}										//Clear interrupt and close I2C slave


/***********************************************************/
void send_byte_with_Ack(char byte){
TWDR = byte;											//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);		//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}

/***********************************************************/
void send_byte_with_Nack(char byte){
TWDR = byte;										//Send payload size: Zero in this case
TWCR = (1 << TWINT) | (1 << TWEN);		//clear interrupt and set Enable Acknowledge
while (!(TWCR & (1 << TWINT)));}

/***********************************************************/
unsigned char receive_byte_with_Ack(void){
char byte;
TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);		//Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));						//Wait for interrupt
byte = TWDR;
return byte;}

/***********************************************************/
unsigned char receive_byte_with_Nack(void){
char byte;
TWCR = (1 << TWEN) | (1 << TWINT);		//Set Ack enable and clear interrupt
while (!(TWCR & (1 << TWINT)));						//Wait for interrupt
byte = TWDR;
return byte;}

/***********************************************************/
void I2C_Rx_RN(unsigned long *RHSofDP, signed char *res_exp){
unsigned char receive_byte;				

waiting_for_I2C_master;						

for (int m = 0; m<=3; m++){				//Receive 4 chars and assemble into unsigned long result
receive_byte = receive_byte_with_Ack();
switch(m){
case 0: case 1: case 2:*RHSofDP =  *RHSofDP + receive_byte; 
*RHSofDP = *RHSofDP << 8; break;
case 3: *RHSofDP =  *RHSofDP + receive_byte; break;}}

*res_exp = receive_byte_with_Nack();		//last char is the exponent
clear_I2C_interrupt;}					
