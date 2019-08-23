


/*********************************************************************************************/
void sendString(char s[]){
int i = 0;
while(i < 200){
if(s[i] == '\0') break;
sendChar(s[i++]);} }



/*********************************************************************************************/
char isCharavailable (int m){int n = 0;		
while (!(UCSR0A & (1 << RXC0))){n++;
if (n>4000) {m--;n = 0;}if (m == 0)return 0;}
return 1;}



/*********************************************************************************************/
char receiveChar(void)
{return UDR0;}



/*********************************************************************************************/
void sendChar(char data){
while (!(UCSR0A & (1 << UDRE0)));
UDR0 = data;}



/***********************************************************************************************/
void Hex_to_PC(int hex){
short_num_to_PC(hex>>8);											//Send hex integer to PC
short_num_to_PC(hex);}



/***********************************************************************************************/ 
void short_num_to_PC(char x)										//convert a number to three chars
{if((x>>4)>0x9)
sendChar((x>>4) + '0' + 7);
else sendChar((x>>4) + '0');

if((x&0xF)>0x9)
sendChar((x & 0xF) + '0' + 7);		
else sendChar((x & 0xF) + '0');}	



