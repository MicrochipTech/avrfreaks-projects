
void sendHex(char, unsigned int);
void sendCharasASKI(char, unsigned char); 
void sendsignedHex (int);
void my_utoa(char, unsigned int, char*, char);
void my_chartoa(char, unsigned char, char*);
int my_atoi (char *);
int my_atoi_with_sign (char*);
char non_numeric_char (char);
int askiX2_to_hex (char*);
int askiX4_to_hex_V2 (char*);




void sendHex(char radix, unsigned int Hex){ char print_out_string[6];
my_utoa(radix, Hex, print_out_string, 'Z'); 
sendString_with_pause(print_out_string);sendString(" ");}										//include leading zeros



void sendCharasASKI(char radix, unsigned char CHR){ char print_out_string[4];
my_chartoa(radix, CHR, print_out_string); 
sendString_with_pause(print_out_string);sendString(" ");}



void sendsignedHex (int Hex){
char print_out_string[6];
char sign_char = '+';
if (Hex & 0x8000) {Hex = ~Hex + 1; sign_char = '-';}
my_utoa(10, Hex, print_out_string, 'N'); 											//Exclude leading zeros
if (sign_char == '-')
{sendChar('-'); sendString_with_pause(print_out_string);sendString("  ");}
else {sendChar(' '); sendString_with_pause(print_out_string);sendString("  ");}}




void my_utoa(char radix, unsigned int n, char s[], char mode){						//unsigned to askii
unsigned char i,sl,j,c;
i=0; sl=0;
do{sl++;
s[i] = n % radix;
if (s[i] < 10) s[i]+= '0';
else s[i] += '7';
i++;
}while((n/=radix)>0);
if ((i < 4) && (mode == 'Z')){while(i < 4) {s[i++] = '0';sl++;}}	
s[i] = '\0';
for(i=0, j=sl-1; i<j; i++,j--){
c=s[i];  s[i] = s[j];  s[j] = c;}}



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




int my_atoi (char s[]){ int i,n;													//askii to integer
n=0;
for(i=0; s[i] >= '0' && s[i] <= '9'; ++i)
n= 10*n + (s[i] - '0');
return n;}




int my_atoi_with_sign (char s[]){ int i,n;											//askii to interger with sign
n=0;
switch (s[0]){
case '-' : 
for(i=1; s[i] >= '0' && s[i] <= '9'; ++i)
n= 10*n + (s[i] - '0'); n = 0-n; break;

case '+' : 
for(i=1; s[i] >= '0' && s[i] <= '9'; ++i)
n= 10*n + (s[i] - '0'); break;

default:
for(i=0; s[i] >= '0' && s[i] <= '9'; ++i)
n= 10*n + (s[i] - '0'); break;}
return n;}




char non_numeric_char (char data){													//test for a numeric chatr
if (((data > '9') && (data < 'A')) || (data < '0') || (data > 'F'))
return 1;
else return 0;}




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




int askiX4_to_hex_V2 ( char a[])														//convert askii to binary integer																			
{int m, n;  unsigned int hex;
hex=0;n=3;
for(m=0; m<=3; m++){ 
if (a[m]=='\0') {  hex = hex >> (4*(n+1)); return hex;}
if(non_numeric_char(a[m])) return 0;
a[m] = a[m] - '0'; if (a[m] >=17)a[m] = a[m]-7;
hex = hex + (a[m] << (4*n)); n--;}
return hex;}



