

/*Subroutines which interface with the serial programming ports of the target device*/



unsigned char Atmel_config ( unsigned int, unsigned int );
unsigned char Read_write_mem(char, int, char);
void Load_page(char, int, unsigned char);

void Timer_T0_sub(char, unsigned char);
void Timer_T2_sub(char, unsigned char);






unsigned char Atmel_config ( unsigned int Atmelcmd_h, unsigned int Atmelcmd_l){
char Echo =0;
unsigned long cmd;

cmd = Atmelcmd_h;
cmd = (cmd<<16) + Atmelcmd_l;  


for (int n=0; n<=15; n++){
PGC_L; PGClock_L;  
if ((Atmelcmd_h<<n) & 0x8000){PGD_cmd_H;}  else PGD_cmd_L;   
PGC_H;	PGClock_H; } 

  
for (int n=0; n<=15; n++){
PGC_L; PGClock_L;
if ((Atmelcmd_l<<n) & 0x8000){PGD_cmd_H;}  else PGD_cmd_L; 
PGC_H;	PGClock_H;

switch (cmd){	
	
case (Prog_enable): 
if ((n < 8) && (PGD_resp_H)) Echo = Echo | (1<< (7-n)) ; break; 

case (signature_bit_1): 
case (signature_bit_2):
case (signature_bit_3): 
if((n > 7) && (PGD_resp_H)) Echo = Echo | (1<< (15-n)); break;

case (read_fuse_bits):
case (read_extended_fuse_bits):
case (read_fuse_bits_H): 
if((n > 7) && (PGD_resp_H)) Echo = Echo | (1<< (15-n));break;

case (read_lock_bits): 
if((n > 9) && (PGD_resp_H)) Echo = Echo | (1<< (15-n));break;}

}PGC_L;	PGClock_L;  

Timer_T0_sub(T0_delay_10ms);
return Echo;}                





/*************************************************************************************************************/
unsigned char Read_write_mem(char Operation,  int mem_address, char EEPROM_data){
unsigned char cmd = 0;
char n,Echo=0;


switch (Operation){
case 'H': cmd = 0x28; break;		//read flash high byte
case 'L': cmd = 0x20; break;		//read flash low byte
case 'W': cmd = 0x4C; break;		//write page to flash  (32 or 64 commands)
case 'I': cmd = 0xC0; break;      	//EEPROM data in   5mS DELAY will be needed
case 'O': cmd = 0xA0; break;}      //EEPROM data out

for (n=0; n<=7; n++){
PGC_L;	PGClock_L;				
if ((cmd<<n) & 0x80)PGD_cmd_H;	else PGD_cmd_L;	
PGC_H; PGClock_H;}
   
      
for (n=0; n<=15; n++){PGC_L; PGClock_L;			
if ((mem_address<<n) & 0x8000)PGD_cmd_H; else 	PGD_cmd_L;
PGC_H; PGClock_H;}

       
for (n=0; n<=7; n++){PGC_L;	PGClock_L;		
if ((Operation == 'I') && ((EEPROM_data <<n) & 0x80))PGD_cmd_H;  else PGD_cmd_L; 
PGC_H; PGClock_H; 
  
if (PGD_resp_H) (Echo = (Echo | (1<< (7-n))));		
}PGC_L;	PGClock_L;

if(Operation == 'W')Timer_T0_sub(T0_delay_5ms);
if(Operation == 'I'){if (Twd_EEPROM == 10)Timer_T0_sub(T0_delay_10ms);	
else Timer_T0_sub(T0_delay_5ms);}
return Echo;}




/*************************************************************************************************************/
void Load_page(char high_low, int address_on_page, unsigned char data){
int cmd = 0;
unsigned char n,Echo;

switch (high_low){
case 'H': cmd = 0x4800; break;
case 'L': cmd = 0x4000; break;}
Echo = 0;

for (n=0; n<=15; n++){PGC_L;PGClock_L;  
if((cmd<<n) & 0x8000)PGD_cmd_H; else 	PGD_cmd_L;
PGC_H; PGClock_H;}            

for (n=0; n<=7; n++){PGC_L;PGClock_L; 
if ((address_on_page<<n) & 0x80)PGD_cmd_H; else 	PGD_cmd_L;          
PGC_H; PGClock_H;}       

for (n=0; n<=7; n++){PGC_L; PGClock_L;  
if ((data<<n) & 0x80)PGD_cmd_H;  else 	PGD_cmd_L;     
PGC_H;   PGClock_H;}   
PGC_L;	PGClock_L;}     
    












