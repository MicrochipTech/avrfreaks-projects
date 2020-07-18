
char PRN_8bit_GEN(void);
unsigned int PRN_16bit_GEN(unsigned int);
void I2C_Tx_snowstorm_display(void);

unsigned char PRN_8bit_GEN_UNO(void);
unsigned int PRN_16bit_GEN_UNO(unsigned int);
void I2C_Tx_snowstorm_display_UNO(void);

void prime_no_generator(int,int, int*);
void prime_no_generator_plus(int,int,  int*);
int Product_search (int);
int Prime_search (int, int);




/*****************************************************************/
char PRN_8bit_GEN(void){
unsigned int bit;
char lfsr;

lfsr = eeprom_read_byte((uint8_t*)(0x1FB));
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 1;
lfsr = (lfsr >> 1) | (bit << 7);
eeprom_write_byte((uint8_t*)(0x1FB),lfsr);
return lfsr;}


/*****************************************************************/

unsigned int PRN_16bit_GEN(unsigned int start){
unsigned int bit, lfsr;

if(!(start)) lfsr = (eeprom_read_byte((uint8_t*)(0x1FB)) << 8) + eeprom_read_byte((uint8_t*)(0x1FA));
else lfsr = start;
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
lfsr = (lfsr >> 1) | (bit << 15);
if(!(start)){
eeprom_write_byte((uint8_t*)(0x1FB),(lfsr>>8));
eeprom_write_byte((uint8_t*)(0x1FA),lfsr);}

return lfsr;}



/************************************************************************/
void I2C_Tx_snowstorm_display(void){

int PRN;

while(1){
PRN = PRN_16bit_GEN (0);									//Generate a new PRN (0) tells subroutine to use the EEPROM
I2C_Tx_2_integers(PRN, (PRN<<1));							//Display two "pseudo random numbers"
Timer_T1_sub(T1_delay_100ms);}}



/*****************************************************************/
unsigned char PRN_8bit_GEN_UNO(void){
unsigned int bit;
unsigned char lfsr;

lfsr = eeprom_read_byte((uint8_t*)(0x3F3));
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 1;
lfsr = (lfsr >> 1) | (bit << 7);
eeprom_write_byte((uint8_t*)(0x3F3),lfsr);
return lfsr;}


/*****************************************************************/

unsigned int PRN_16bit_GEN_UNO(unsigned int start){
unsigned int bit, lfsr;

if(!(start)) lfsr = (eeprom_read_byte((uint8_t*)(0x3F3)) << 8) + eeprom_read_byte((uint8_t*)(0x3F2));
else lfsr = start;
bit = (( lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
lfsr = (lfsr >> 1) | (bit << 15);
if(!(start)){
eeprom_write_byte((uint8_t*)(0x3F3),(lfsr>>8));
eeprom_write_byte((uint8_t*)(0x3F2),lfsr);}

return lfsr;}



/************************************************************************/
void I2C_Tx_snowstorm_display_UNO(void){

int PRN;

while(1){
PRN = PRN_16bit_GEN_UNO (0);									//Generate a new PRN (0) tells subroutine to use the EEPROM
I2C_Tx_2_integers(PRN, (PRN<<1));							//Display two "pseudo random numbers"
Timer_T1_sub(T1_delay_100ms);}}






/*****************************************************************/
void prime_no_generator(int l,int n,  int reg[]){
int i, m;
i=2; 
while (i*i < n*(l+1)){
m=0;
while(i*(i+m) <= n*l)m++; 
while(i*(i+m) <= n*(1+l)){reg[i*(i+m) -1 - n*l] = 0; m++; }	
i++;}}


/*****************************************************************/
void prime_no_generator_plus( int l,int n,  int reg[]){
int i, m;
i=2; 
while (i*i < n*(l+1)){
m=0;
while(i*(i+m) <= n*l)m++; 
if(reg[i*(i+m) -1 - n*l]){
while(i*(i+m) <= n*(1+l)){reg[i*(i+m) -1 - n*l] = 0; m++; }	}
i++;}}


/*****************************************************************/
int Product_search	(int number)	{
int search_array[10];	
int j=0;
int n=10;
int l=0;
while(1){
for(int k =0; k < n; k++) {search_array[k] = k+1+n*l;}
prime_no_generator(l,n,search_array);
j=0; while(search_array[j] == 0)j++;
while (j<=10){if ((search_array[j] >= 2) &&  (number%search_array[j] == 0)){return search_array[j];}j++;}
if(l*10 > number)return 0;
l++;}}



/*****************************************************************/
int Prime_search ( int number, int array_size)	{
int search_array[array_size];	
int j=0;
int l=0;
while (1){
for(int k =0; k < array_size; k++) {search_array[k] = k+1+array_size*l;}
prime_no_generator(l,array_size,search_array);
j=0;
while (j<array_size){if ((search_array[j] >= 2) &&  (number%search_array[j] == 0)){
if (search_array[j]== number) {return 1;} else return 0;}j++;}
if(l*array_size > number)return 0;
l++;}	}
