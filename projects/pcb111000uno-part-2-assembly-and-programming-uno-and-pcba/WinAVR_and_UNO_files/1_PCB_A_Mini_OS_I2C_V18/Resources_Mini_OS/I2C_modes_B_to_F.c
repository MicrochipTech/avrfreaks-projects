


void I2C_Tx_2URNs_from_IO (void){									//Acquires 2 Unsigned Real Numbers from the user switches

char SW3_keypress, sw2_keypress,Return, keypres_counter;

if ((!(I2C_data[1]))&&(!(I2C_data[2]))&&(!(I2C_data[3]))\
&&(!(I2C_data[4]))&&(!(I2C_data[5]))&&(!(I2C_data[6]))\
&&(!(I2C_data[7])))												//Test for new sum and initialise variables
	{if(I2C_data[0]=='A')
		{exponent[0]=0;exponent[1]=0; display_mask = 0; 
		RN=0;keypres_counter_old=0; exponent_BKP[0]=0; 
		exponent_BKP[1]=0; overflow=0; return;}}

if ((I2C_data[0] =='P') & (!(exponent[RN])))						//Initialise exponent and display mask
	{exponent[RN]=-1;for (int m = 0; m < 8; m++)
	display_buf[m] = I2C_data[m];return;}

{int m=0;while (I2C_data[7-m]==0)m++; keypres_counter=8-m;}  		//Detect & count sw3 keypresses
if ((keypres_counter != keypres_counter_old) ||\
 ((keypres_counter == keypres_counter_old)\
 && (I2C_data[0] =='P')))
		SW3_keypress=1; 
		else SW3_keypress=0;  
keypres_counter_old=keypres_counter;

sw2_keypress=0;														//Detect SW2 keypresses
for(int m=0; m<8; m++)
{if(display_buf[m] == I2C_data[m])sw2_keypress++;} 
if ((sw2_keypress == 8) && (I2C_data[0] !='P')) 
Return = 1; else Return = 0; 
																	//Check for overflow condition
if((display_buf[7]) && ((I2C_data[7] != display_buf[7])
|| (I2C_data[6] != display_buf[6])\
|| (I2C_data[5] != display_buf[5])\
|| (I2C_data[4] != display_buf[4])\
|| (I2C_data[3] != display_buf[3])
|| (I2C_data[2] != display_buf[2]))){overflow++;} 
if(overflow)Return = 1;

if((!(Return)) &&(!(overflow))){									//Update display and exponent during entry of a single number
for (int m = 0; m < 8; m++){display_buf[m] = I2C_data[m];}
if(SW3_keypress){if((!(RN)) && exponent[0])
	{display_mask = (display_mask <<1) + 1; exponent[0]--;
	for(int p = 0; p < 8; p++)strobe[p] = 0;}						//synchronise strobe

if(RN){
if(!(exponent[1])){display_mask = (display_mask <<1);
for(int p = 0; p < 8; p++)strobe[p] = 0;}
if (exponent[1]<-1){display_mask =  (display_mask <<1) + 1; 
exponent[1]--; for(int p = 0; p < 8; p++)strobe[p] = 0;}
if(exponent[1]==-1){display_mask +=1;
for(int p = 0; p < 8; p++)strobe[p] = 0;exponent[1]--;}}}}
	
																	//Enter second number when complete
if((Return)){														//sw2 keypress
if(RN){																//Second sw2 keypress
for(int m = 0; m<=7; m++)
{I2C_data[m]=display_buf[m];display_buf[m] = 0;}
TIMSK1 |= (1<<TOIE1);
timer_T1_sub_with_interrupt(T1_delay_500ms);while(!(T1_ovf_flag)); //Flash display
T1_ovf_flag=0;	TIMSK1 &= (~(1<<TOIE1));
for(int m = 0; m<=7; m++)display_buf[m] = I2C_data[m];

if((!(overflow))||(overflow==1)){
if (exponent[0])exponent[0]++;
if (exponent[1])exponent[1]++;}
																	//calculate Number[0] and [1]
{int n,q;
n=0; while(display_buf[n])n++;	q=n;								//determine length of number string
number[1] = 0;
while(n){
number[1] = number[1] * 10 + display_buf[n-1] - '0'; n--;} 		//convert the numerical string to a long number (4 bytes)

n=q+1;while((display_buf[n]) && (n<=7))n++;
number[0] = 0;
while(n-q-1){
number[0] = number[0] * 10 + display_buf[n-1] - '0'; n--;}}

																	//backup the display, numbers and exponents
for (int n = 0; n<=7; n++)display_backup[n] = display_buf[n]; 
display_backup[8] = display_mask;
exponent_BKP[0] = exponent[0]; exponent_BKP[1] = exponent[1];
number[4] = number[0]; number[5] = number[1];
return;}
																	//Enter first number when complete
if(!(RN)){		//First sw2 keypress
	display_mask = (display_mask << 2);	
	for(int p = 0; p < 8; p++)strobe[p] = 0;						//synchronise strobe
	{for (int n = 0; n < 7; n++)
	{display_buf[7-n] = display_buf[6-n];}} 
	display_buf[0] = '\0';											//shift display left and blank digit 0
	{for (int n = 0; n < 7; n++)
	{display_buf[7-n] = display_buf[6-n];}}
	display_buf[0] = '0';											//shift display left and initialise digit 0
	keypres_counter_old = keypres_counter+2;
	RN=1;}}}
	
	
	
/*****************************************************************************************************************/	
void I2C_Tx_Uarithmetic_OP (void){									//Does arithmetic on the Unsigned Real Numbers

Op = I2C_data[0];
exponent[0] = exponent_BKP[0];	exponent[1] = exponent_BKP[1];	
number[0] = number[4]; number[1] = number[5];						//restore exponents
	switch(Op){
	case 'M': RHSofDP = multiply_real(number, exponent); break;	//Multiply
	case 'D': RHSofDP = divide_real (number, exponent); break;		//Divide
	case 'R':														//Route  
	mode_C_ptr=0;TIMSK1 |= (1<<TOIE1);
	timer_T1_sub_with_interrupt(T1_delay_500ms);
	exponent[1] = number[1];
	RHSofDP = any_root(number, exponent);
	TIMSK1 &= (~(1<<TOIE1)); TCCR1B = 0;																															////	
	for(int m = 0; m<=7; m++)display_buf[m]=0;break;
	case 'P': exponent[1] = number[1]; 							//Power
	RHSofDP = any_power(number,exponent);break;
	case 'A': for(int m = 0; m<=7; m++)
	display_buf[m] = display_backup[m]; 
	display_mask = display_backup[8];
	for(int p = 0; p < 8; p++)strobe[p] = 0; break;}
	
	if(Op == 'A')return;
	Disp_Long(RHSofDP,exponent[2]);expnt_result = exponent[2];
	for(int m = 0; m <= 3; m++)
	{result[3-m] = RHSofDP; RHSofDP = RHSofDP >> 8;}
	
	Initialise_I2C_master_write;									//Return result to UNO device								
	for(int m = 0; m<=3; m++)
	{I2C_master_transmit(result[m]);}
	I2C_master_transmit(expnt_result);
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);}
	
	
	
	
/*****************************************************************************************************************/	
void I2C_Tx_Compile_tables(void){
int product;

if (!(I2C_data[7])){											
for (int m = 0; m < 8; m++){										//8 digits to be downloaded
display_buf[m] = I2C_data[m];}
	
																	//Initiate tables display
for (int n = 0; n < 7; n++)
{display_buf[7-n] = display_buf[6-n];} display_buf[0] = '\0';		//shift display left
for (int n = 0; n < 7; n++)
{display_buf[7-n] = display_buf[6-n];} display_buf[0] = '0';		//shift display left
display_buf[0] = 1 + '0';											//Initialise digits[0] & [1]
display_buf[1]='0';

for(int m = 0; m<= 3; m++)
{display_buf[7-m] = display_buf[3-m];display_buf[3-m]=0;}
if(!(display_buf[7])) display_buf[7] = '0';
if(!(display_buf[5])) display_buf[5] = '0';}
	
else{switch (I2C_data[7]){
case '1':
	if ((display_buf[4] == '9') && (display_buf[5] =='9'))
	{display_buf[4]='0';display_buf[5]='0';}
	if(display_buf[4]== '9')
	{display_buf[4]='0'; display_buf[5]++;}
	else display_buf[4]++; break;

case '2': 
	if ((display_buf[4]=='1') && (display_buf[5]=='0')) break; 
	if (display_buf[4]=='0')
	{display_buf[5]--; display_buf[4]='9';}
	else display_buf[4]--;break;}}
	
product = (((display_buf[7]-'0') * 10) + display_buf[6]-'0')\
* (((display_buf[5]-'0') * 10) + display_buf[4]-'0');
for(int m = 0; m<=3; m++)display_buf[m]=0;
{int m=0;do(display_buf[m++] = product%10 + '0'); 
while ((product/=10) > 0);}
display_mask = 0b00110000;
for(int p = 0; p < 8; p++)strobe[p] = 0;}							//synchronise strobe



	
/*****************************************************************************************************************/	
	void I2C_Tx_accumulator_1(void){								//Performs for add, subtract and clear modes
	int acc_input;
	
	for (int m = 0; m < 9; m++){									//9 digits to be downloaded
	if (m == 8)Op = I2C_data[m];
	else display_buf[m] = I2C_data[m];}							//Store digits for display
	{int m = 0; while((display_buf[m]) && (m < 3))m++; 
	acc_input = 0;
	while(m){
	acc_input = acc_input*10 + (display_buf[m-1] - '0'); m--;}
	if (Op == 'A')accumlator += acc_input;
	if (Op == 'S')accumlator -= acc_input;
	if (Op == 'C')accumlator = 0;
	
	interim_result = accumlator;
		
	if((accumlator) < 0)accumlator = -accumlator;
	m = 0;
	do{display_buf[m+3] = accumlator%10 + '0';m++;} 
	while((accumlator/=10) > 0);
	if(interim_result < 0)display_buf[m+3] = '-';
	display_mask = 0b00000111;
	for(int p = 0; p < 8; p++)strobe[p] = 0;
	accumlator = interim_result;}}
	

	
/*****************************************************************************************************************/		
	void I2C_Tx_accumulator_2(void){								//Performs for data input modes
	for (int m = 0; m < 3; m++){									//8 digits to be downloaded
	display_buf[m] = I2C_data[m];}									//Store digits for display
	interim_result = accumlator;
	if((accumlator) < 0)accumlator = -accumlator;
	{int m = 0;
	do{display_buf[m+3] = accumlator%10 + '0';m++;} 
	while((accumlator/=10) > 0);
	if(interim_result < 0)display_buf[m+3] = '-';
	display_mask = 0b00000111;
	for(int p = 0; p < 8; p++)strobe[p] = 0;
	accumlator = interim_result;}}
	