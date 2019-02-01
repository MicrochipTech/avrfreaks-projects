

/*****************************************************************************************/
ISR(TIMER0_OVF_vect) {


if(eeprom_read_byte((uint8_t*)0x3FB) == 0xFF){	TCNT0 = 0;		//High brightness 2ms interrupts TCNT0 = 0
{Display_driver(); }}



if(eeprom_read_byte((uint8_t*)0x3FB) == 0xFE){					//normal brightness 500uS interrupts TCNT0 = 192;
if(!(T0_interupt_cnt)){
{Display_driver(); }}TCNT0 = 192;	

switch(T0_interupt_cnt){
case 0: T0_interupt_cnt = 1;break;
case 1: {reset_segments;} T0_interupt_cnt = 2; break;
case 2: T0_interupt_cnt = 3; break;
case 3: T0_interupt_cnt = 0; break;}}



if(eeprom_read_byte((uint8_t*)0x3FB) == 0xFD){					//low brightness 500uS interrupts TCNT0 = 240;
if(!(T0_interupt_cnt)){
{Display_driver(); }}TCNT0 = 240;


switch(T0_interupt_cnt){
case 0: T0_interupt_cnt = 1;break;
case 1: {reset_segments;} T0_interupt_cnt = 2; break;
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

