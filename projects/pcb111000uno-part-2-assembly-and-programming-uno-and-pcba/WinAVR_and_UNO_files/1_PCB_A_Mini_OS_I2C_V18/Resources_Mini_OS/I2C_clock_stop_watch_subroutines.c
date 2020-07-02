

void basic_clock (void){
for (int m = 0; m < 8; m++){									//AT_clock_mode = C receives 8 chars (hours mins secs)
clock_buf[7-m] = I2C_data[m];}	
refresh_clock_display;
TCCR2B = 2;														//Set clock to 4096Hz and start it
TIMSK2 |= (1 << TOIE2);										//Enable timer clock interrupt
clock_flag=1;
timer_mode = 1;}



/*******************************************************************************************************************/
void multi_mode_clock(void){
	PIC_cmd =  I2C_data[0];										//Receive command from the PIC for the 1 second timer/clock
	switch(PIC_cmd){
	case '1': clock_flag=1; break;								//Display clock
	case '2': clock_flag=0; break;								//Hide clock Clock runs in background while display is used else where
	case '3': clock_flag=0; refresh_clock_display; break;		//Pause clock
	case '5': {increment_seconds;} clock_flag=1; break;
	case '6': {decrement_minutes;} clock_flag=1; break;
	case '7': {decrement_seconds;} clock_flag=1; break;
	case '8': {increment_minutes;} clock_flag=1; break;}			
	timer_mode = 1;
	}
	
	

/*******************************************************************************************************************/
void stop_watch(void){

char stop_watch_mode;
char stop_watch_flag;
int timer_record;
int timer_pointer;
int record_counter;
char timer_store [95];

																//I2C_Tx_Clock_command(store_time)
stop_watch_mode =  I2C_data[0];									//I2C_Tx_Clock_command(display_current_time); 
initiate_stop_watch_display;									//I2C_Tx_Clock_command(display_stored_times);
if (stop_watch_mode == '2') 
{timer_mode = 2;stop_watch_buf[2] = 0; 
stop_watch_buf[5] = 0;}
else {timer_mode = 3;stop_watch_buf[1] = 0; 
stop_watch_buf[4] = 0;}
TCNT2 = 0;														//Set clock to 4096Hz and start it												
TCCR2B = 2;													
TIMSK2 |= (1 << TOIE2);	
stop_watch_flag=1;
timer_record = 0;
timer_pointer = 0;
record_counter = 0;

while(1){//A													//stop watch not aqble to run in background
while(1){//B													//in loop B update dislays and wait for command
TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);				//send a start condition
while (!(TWCR & (1 << TWINT)));								//Wait for TWINT flag 
TWDR = 0x03;													//Address of slave (master read operation)  SLA + R
TWCR = (1 << TWINT) | (1 << TWEN);								//Clear TWINT bit to start transmission of address
while (!(TWCR & (1 << TWINT)));								//Wait for TWINT flag 
if (TWSR == 0x40)break;

if (TCNT2 == Ten_mS_tick){//C									//Pole timer T2 for 10mSec interval
Ten_mS_tick = (Ten_mS_tick + 41)%256; 

switch (timer_mode){//D
case 3:															//100ms timer;
Ten_mS_tick_counter++;
if (Ten_mS_tick_counter == 10) 
{update_milli_sec_display(); Ten_mS_tick_counter = 0;} 
break;
case 2: update_milli_sec_display();								//10ms timer;
break;}//D

if (stop_watch_flag==1){
refresh_stop_watch_display; }  
}//C
}//B		 													//Exit loop to receive mode and stop watch command

TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);				// Acquire payload_size and ignore it
while (!(TWCR & (1 << TWINT)));
payload_size = TWDR;
TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);				// Acquire mode either 9 or 10
while (!(TWCR & (1 << TWINT)));
mode = TWDR;

TWCR = (1 << TWINT) | (1 << TWEN);
while (!(TWCR & (1 << TWINT)));
PIC_cmd =  TWDR;
TWCR =  (1 << TWINT ) | (1 << TWEN ) | (1 << TWSTO );
	
if (mode == 10) {TCCR2B = 0; TIMSK2 &= (!(1 << TOIE2));
{clear_display_buffer;}break;}      							//I2C_Tx_Clock_command(AT_exit_stop_watch);

switch(PIC_cmd){
case '1': stop_watch_flag=1; break;
case '2': stop_watch_flag=0; refresh_stop_watch_display; 
for(int m = 0; m < 8; m++) 
{timer_store [timer_record + m] = display_buf[m];}
timer_record += 8; timer_record = timer_record%80; 
record_counter++; break;

case '3': stop_watch_flag=1; initiate_stop_watch_display;  
if (timer_mode == 2){stop_watch_buf[2] = 0; 
stop_watch_buf[5] = 0;}
if (timer_mode == 3){stop_watch_buf[1] = 0;
stop_watch_buf[4] = 0;}
TCNT2 = 0; break;

case '4': if (record_counter){stop_watch_flag=0; 
for(int m = 0; m < 8; m++) 
{display_buf[m] = timer_store[timer_pointer + m];}
if(record_counter < 10){timer_pointer += 8;  
timer_pointer = timer_pointer%timer_record;} 
else{timer_pointer += 8;  
timer_pointer = timer_pointer%80; }}break;}
}//A
}