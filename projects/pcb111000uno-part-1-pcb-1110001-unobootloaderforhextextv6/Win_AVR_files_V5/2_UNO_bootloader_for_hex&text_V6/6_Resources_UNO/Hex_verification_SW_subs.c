




/*********************************************************************************************/
void newline(void){sendString ("\r\n");}



/***********************************************************************************************/
void Timer_T0_sub(char Counter_speed, unsigned char Start_point){ 
TCNT0 = Start_point;
TCCR0B = Counter_speed;
while(!(TIFR0 & (1<<TOV0)));
TIFR0 |= (1<<TOV0); TCCR0B = 0;}



/*********************************************************************/
void Timer_T1_sub(char Counter_speed, unsigned int Start_point){ 
TCNT1H = (Start_point >> 8);
TCNT1L = Start_point & 0x00FF;
TIFR1 = 0xFF;
TCCR1B = Counter_speed;
while(!(TIFR1 & (1<<TOV1)));
TIFR1 |= (1<<TOV1); 
TCCR1B = 0;}



/***********************************************************************************************/
void Hex_to_PC(int hex){
short_num_to_PC(hex>>8);											//Send hex integer to PC
short_num_to_PC(hex);
sendString(" ");}




/***********************************************************************************************/ 
void short_num_to_PC(char x)										//convert a number to three chars
{if((x>>4)>0x9)
sendChar((x>>4) + '0' + 7);
else sendChar((x>>4) + '0');

if((x&0xF)>0x9)
sendChar((x & 0xF) + '0' + 7);		
else sendChar((x & 0xF) + '0');}	



/***********************************************************************************************/ 
void binUnwantedChars (void){char bin_char;
while(1){if (isCharavailable(2)==1)bin_char = receiveChar();else break;}}




/***********************************************************************************************/
int readCMD(int address_in_flash){
int cmd;
address_in_flash = address_in_flash*2;
Prog_mem_address_H = (address_in_flash) >> 8;
Prog_mem_address_L = address_in_flash;
read_flash ();																		//Subroutine provided in assembly file
cmd = Flash_readout << 8;
address_in_flash += 1;
Prog_mem_address_H = (address_in_flash) >> 8;
Prog_mem_address_L = address_in_flash;
read_flash ();
cmd += Flash_readout;
return cmd;}



/***********************************************************************************************/
int askiX2_to_hex (char a[]) {														//convert askii binary char
int hex;
hex=0;
if (non_numeric_char(a[0])) return 0;
if (non_numeric_char(a[1])) return 0;

a[0] = a[0] - '0'; if (a[0] >=17)a[0] = a[0]-7;
hex = hex + (a[0]<<4);
a[1] = a[1] - '0'; if (a[1] >=17)a[1] = a[1]-7;
hex = hex + a[1];
return hex;}



/***********************************************************************************************/
char non_numeric_char (char data){													//test for a numeric char
if (((data > '9') && (data < 'A')) || (data < '0') || (data > 'F'))
return 1;
else return 0;}



/***********************************************************************************************/
void sendCharasASKI(char radix, unsigned char CHR){ char print_out_string[4];
my_chartoa(radix, CHR, print_out_string); 
sendString(print_out_string);sendString(" ");}



/***********************************************************************************************/
void my_chartoa(char radix, unsigned char n, char s[]){							//char to askii
unsigned char i,sl,j,c;
i=0; sl=0;
do{sl++;
s[i] = n % radix;
if (s[i] < 10) s[i]+= '0';
else s[i] += '7';
i++;
}while((n/=radix)>0);
s[i] = '\0';
for(i=0, j=sl-1; i<j; i++,j--){
c=s[i];  s[i] = s[j];  s[j] = c;}}

