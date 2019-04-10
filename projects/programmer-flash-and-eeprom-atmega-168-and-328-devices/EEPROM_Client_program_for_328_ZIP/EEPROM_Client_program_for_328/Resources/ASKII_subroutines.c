
void Num_to_PC(char, long);					
void NumericString_to_PC(char*);			
void SBtoAL(char*, long, char);
void Hex_to_PC(unsigned int);				
void Num_to_PC_U(char, unsigned long);



/************************************************************************************************/
void Num_to_PC(char radix, long long_num){
char array[12];											//Long has 10 chars + sign + null terminator	
SBtoAL(array, long_num, radix);							//calls the Binary to askii subroutine
NumericString_to_PC(array);}	



/************************************************************************************************/
void NumericString_to_PC(char* s){						//Prints characters in reverse order				
int n=0;
while (s[n] != '\0')n++;								//scroll to end of string counting the number of characters
for(int m = n; m; m--)sendChar(*(s + m-1));}			//print last character first
			


/************************************************************************************************/
void SBtoAL(char array[], long num, char radix){		//Signed Binary to Askii
int m=0;
long sign = 0;

if (num == 0x80000000){									//This is a special case
switch(radix){
case 10: array[0] = '8';array[1] = '4'; array[2] = '6';			
array[3] = '3';array[4] = '8';array[5] = '4'; array[6] = '7';
array[7] = '4';array[8] = '1';array[9] = '2';
array[10] = '-'; array[11] = '\0'; break;

case 16: array[0] = '0';array[1] = '0'; array[2] = '0';			
array[3] = '0';array[4] = '0';array[5] = '0'; array[6] = '0';
array[7] = '8';	array[8] = '\0';array[9] = '\0';
array[10] = '\0'; array[11] = '\0'; break; } return;}

for (int n = 0; n <=11; n++)array[n] = 0;				//All other numbers
if ((sign = num) < 0) num = num * (-1);

do {array[m] = num % radix;
if(array[m] < 10)array[m] += '0'; else array[m] += '7';
m++;} while ((num = num/radix) > 0);
if (sign < 0) {array[m] = '-';m++;}}



/************************************************************************************************/
void Hex_to_PC(unsigned int Int_num){
char array[5];								
int m=0;
for (int n = 0; n <=4; n++)array[n] = 0;	
do {array[m] = Int_num % 16;
if(array[m] < 10)array[m] += '0'; else array[m] += '7';
m++;} while ((Int_num = Int_num/16) > 0);
NumericString_to_PC(array);}	



/************************************************************************************************/
void Num_to_PC_U(char radix, unsigned long long_num){
char array[12];	
int m=0;
for (int n = 0; n <=11; n++)array[n] = 0;	
do {array[m] = long_num % radix;
if(array[m] < 10)array[m] += '0'; else array[m] += '7';
m++;} while ((long_num = long_num/radix) > 0);
NumericString_to_PC(array);}	


