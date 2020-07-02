
char I2C_master_receive(char);
void Ext_tick(void);

/*****************************************************************************************/



ISR(TIMER0_OVF_vect) {

if(eeprom_read_byte((uint8_t*)0x3FB) == 0xFF){	TCNT0 = 0;		//High brightness 2ms interrupts TCNT0 = 0

if(mode == 'F'){
TIMSK2 &= (!((1 << OCIE2A) | (1 << TOV2)));
sei();
Display_driver(); 
TIMSK2 |= ((1 << OCIE2A) | (1 << TOV2));}
else {Display_driver(); }}

if(eeprom_read_byte((uint8_t*)0x3FB) == 0xFE){					//normal brightness 500uS interrupts TCNT0 = 192;
if(!(T0_interupt_cnt)){

if(mode == 'F'){
TIMSK2 &= (!((1 << OCIE2A) | (1 << TOV2)));
sei();
Display_driver(); 
TIMSK2 |= ((1 << OCIE2A) | (1 << TOV2));}
else {Display_driver(); }}

switch (MUX_cntl){
case 0: TCNT0 = 194; break;		//500uS period
case 1:	TCNT0 = 128; break;		//1mS period  CHECK
case 2: TCNT0 = 69; break;		//1500uS period CHECK
case 3: TCNT0 = 0; break;		//2mS period
case 4: TCNT0 = 178; break;		//2500uS period
case 5: TCNT0 = 162; break;		//3mS period
case 6: TCNT0 = 100;break;		//5ms period
case 7: TCNT0 = 194;break;		//8ms period
default: TCNT0 = 194; break;}	//500uSS period

switch(T0_interupt_cnt){
case 0: T0_interupt_cnt = 1;break;
case 1: {clear_display;} T0_interupt_cnt = 2; break;
case 2: T0_interupt_cnt = 3; break;
case 3: T0_interupt_cnt = 0; break;}}

if(eeprom_read_byte((uint8_t*)0x3FB) == 0xFD){					//low brightness 125uS interrupts TCNT0 = 240;
if(!(T0_interupt_cnt)){
if(mode == 'F'){
TIMSK2 &= (!((1 << OCIE2A) | (1 << TOV2)));
sei();
Display_driver(); 
TIMSK2 |= ((1 << OCIE2A) | (1 << TOV2));}
else {Display_driver(); }}
TCNT0 = 240;

switch(T0_interupt_cnt){
case 0: T0_interupt_cnt = 1;break;
case 1: {clear_display;} T0_interupt_cnt = 2; break;
case 2: T0_interupt_cnt = 3; break;
case 3:
case 4:
case 5:
case 6:
case 7:
case 8:
case 9:
case 10:
case 11:
case 12:
case 13:
case 14: T0_interupt_cnt++; break;
case 15: T0_interupt_cnt = 0; break;}}}



/*****************************************************************************************/

ISR(TIMER1_OVF_vect) {
switch (mode){
case 9:
{if(!(mode_C_ptr)){for(int m = 0; m<=7; m++)display_buf[m]=0;}
display_buf[7-mode_C_ptr] = '_';
mode_C_ptr++; mode_C_ptr=mode_C_ptr%8;
timer_T1_sub_with_interrupt(T1_delay_500ms);} break;

case 'M':  case 'N': case 'T':  case 'X': T1_OVF += 1; break;

default: T1_ovf_flag = 1; TCCR1B = 0; break;}}



/*****************************************************************************************/

ISR(TIMER2_OVF_vect) {
long TCNT1_BKP;

switch (mode){

case 'K':  timer_2_counter++; if(timer_2_counter == 3)
{timer_2_counter=0; display_float(Sc_Num_string);} break;


case 'M':  case 'N': case 'T':  case 'X':  
TCCR1B = 0;																			//Halt T1
TCNT1_BKP = TCNT1;																	//Copy the value of TCNT1
TCNT1 = 0;																			//Clear TCNT1
TCCR1B = 1;																			//Get T1 running again ASAP (Note T2 has not stopped running)
if(EA_counter < cal_mode)T1_OVF = 0;														//Ignore first 5 results
else
{switch(T1_OVF){
case 0: error_SUM = error_SUM + (TCNT1_BKP - 62500); break;
case 1: error_SUM = error_SUM + (TCNT1_BKP - 62500 + 65536); break;
case 2: error_SUM = error_SUM + (TCNT1_BKP - 62500 + 131072); break;}
T1_OVF = 0;}EA_counter++;break;


default:
timer_2_counter++; if(timer_2_counter == 16){timer_2_counter=0; 
if(mode != 'F') {update_timer (); Ten_mS_tick = 41;}									//Internal clock modes 7,8 and 9
if(mode == 'F') { OCR2A = 41;Ext_tick();}}break;}}



/*****************************************************************************************/
ISR(TIMER2_COMPA_vect){OCR2A = OCR2A + 41;  OCR2A = OCR2A%256; Ext_tick();}			//call every 10mS mode G only



/*****************************************************************************************/
ISR(TWI_vect){
switch(entry_point){																	//Address of slave (master read operation)  SLA + R
case 'A': TWDR = 0x03; TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE); 				//Send slave address + 'R'
			entry_point='B'; break;
case 'B': if (TWSR == 0x40){TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);			//Ack received wait for payload size
			entry_point='C';} 
			else {TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE); 
			entry_point='A';} break;													//Repeated Start: Nack received
case 'C': payload_size = TWDR; 
			if(!(payload_size))entry_point = 'Z'; 										//No payload: Exit
			else {TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);	//Send a repeated start condition
			entry_point='E';} break;
case 'E': TWDR = 0x03; TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE); 				//Send slave address + 'R'
			entry_point='F'; break;
case 'F': if (TWSR != 0x40){TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);		
			entry_point='E';} 															//Nack received: Send repeated start
			else {TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE); 	//Ack received: Continue
			entry_point='G';} break;
case 'G': display_buf[disp_ptr] = TWDR; disp_ptr++; 									//Save first char in display buff
			if (disp_ptr < (payload_size-1)){TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
			entry_point = 'G';}															//Save subsequent chars in display buff
			else{TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);entry_point = 'H';}break;
case 'H': display_buf[disp_ptr] = TWDR;													//Last char
			entry_point = 'Z';
			break;}																		//Data transfer complete: Exit
if(entry_point=='Z'){TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
wdr();}}




/*****************************************************************************************/
void Ext_tick(){entry_point='A';
TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);		//send a start condition with TWI interupt
disp_ptr = 0;
payload_size=0;}		
