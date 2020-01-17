
/*
Loaded with Hex_txt_programmer but requires PCB_A_Mini_OS_I2C_V16_1_CC to function.

Random display based on Pseudo Random Number PRN generator.
Default user project loaded onto UNO to ensure that there is always something to run.
To rienstate the default project press D at the h/r/t prompt.

Note: The PRN generator uses locations 0x3F5 and 0x3F6 to store the next number
*/

#include "Random_gen_header_file.h"
#include "../6_Resources_UNO/Random_gen_subroutines.c"



int main (void){	
unsigned int PRN;

CLKPR = (1 << CLKPCE);
CLKPR = (1 << CLKPS0);

setup_HW;

while(1){	
PRN = PRN_16bit_GEN (0);									//Generate a new PRN (0) tells subroutine to use the EEPROM
I2C_Tx_2_integers(PRN, (PRN<<1));							//Display two "pseudo random numbers"
Timer_T1_sub(T1_delay_100ms);	
I2C_Tx_LED_dimmer();
if((switch_1_down) && (switch_2_down))asm("jmp 0x5C10");}}		


/************************************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}




/************************************************************************************/
unsigned int PRN_16bit_GEN(unsigned int start){
unsigned int bit, lfsr;

if(!(start)) lfsr = (eeprom_read_byte((uint8_t*)(0x3F6)) << 8) + eeprom_read_byte((uint8_t*)(0x3F5));
else lfsr = start;
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
lfsr = (lfsr >> 1) | (bit << 15);
if(!(start)){
eeprom_write_byte((uint8_t*)(0x3F6),(lfsr>>8));
eeprom_write_byte((uint8_t*)(0x3F5),lfsr);}

return lfsr;}



/*********************************************************************/
void USART_init (unsigned char UBRROH_N, unsigned char UBRR0L_N ){
Timer_T1_sub(T1_delay_100ms);
UCSR0B = 0;
UBRR0H = UBRROH_N;  									
UBRR0L = UBRR0L_N;  								
UCSR0A = (1 << U2X0);
UCSR0B = (1 << RXEN0) | (1<< TXEN0);
UCSR0C =  (1 << UCSZ00)| (1 << UCSZ01);}


	
/************************************************************************************/

void I2C_Tx_LED_dimmer(void){			//I2C_Tx_LED_dimmer_UNO
char Dimmer_control = 0; 
unsigned int n = 0, Port_1=0xCCCC;

while((PINB & 0x04)^0x04) 							//while switch_3 down
{n++; if (n >= 60000)break;}


while((PINB & 0x04)^0x04)
{Dimmer_control = (Dimmer_control + 1)%4;
if(!(Dimmer_control))continue;
I2C_Tx(1, 'Q', &Dimmer_control);
I2C_Tx_2_integers(Port_1, ~Port_1);
Timer_T1_sub(T1_delay_500ms);}}





/*
void I2C_Tx_LED_dimmer(void){
char Dimmer_control = 0; 
int m = 0,n = 0;

while((PINB & 0x04)^0x04) 
{n++;
if (n>1200) {m+=1;n = 0;}}
if (m >= 50){Dimmer_control = 1;
I2C_Tx(1, 'Q', &Dimmer_control);}}*/

