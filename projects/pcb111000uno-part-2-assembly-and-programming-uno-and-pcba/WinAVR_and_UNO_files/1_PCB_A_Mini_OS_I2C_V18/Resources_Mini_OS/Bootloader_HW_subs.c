

char Atmel_config ( unsigned int Atmelcmd_h, unsigned int Atmelcmd_l){
char Echo =0;
unsigned long cmd;
cmd = Atmelcmd_h;
cmd = (cmd<<16) + Atmelcmd_l;  

for (int n=0; n<=15; n++){PGC_L; PGClock_L;  
if ((Atmelcmd_h<<n) & 0x8000){PGD_cmd_H;   }  else 	PGD_cmd_L;   
PGC_H;	PGClock_H; } 
  
for (int n=0; n<=15; n++){PGC_L; PGClock_L;
if ((Atmelcmd_l<<n) & 0x8000){PGD_cmd_H; }  else PGD_cmd_L; 
PGC_H;	PGClock_H;
switch (cmd){		
case (Prog_enable): if ((n < 8) && (PGD_resp_H)) Echo = Echo | (1<< (7-n)) ; break; 
case (signature_bit_1): if((n > 7) && (PGD_resp_H)) Echo = Echo | (1<< (15-n)); break;
}}PGC_L;	PGClock_L;  
{ten_msec_delay;}	
return Echo;}                



/********************************************************************************************/
char Read_write_mem(char Operation,  int mem_address, char EEPROM_data){
unsigned char cmd = 0;
char n,Echo=0;
switch (Operation){
case 'H': cmd = 0x28; break;			//read flash high byte
case 'L': cmd = 0x20; break;			//read flash low byte
case 'W': cmd = 0x4C; break;			//write page to flash 
}

for (n=0; n<=7; n++){PGC_L;	PGClock_L;				
if ((cmd<<n) & 0x80)PGD_cmd_H;	else PGD_cmd_L;	
PGC_H; PGClock_H;}   
      
for (n=0; n<=15; n++){PGC_L; PGClock_L;			
if ((mem_address<<n) & 0x8000)PGD_cmd_H; else 	PGD_cmd_L;
PGC_H; PGClock_H;}
       
for (n=0; n<=7; n++){PGC_L;	PGClock_L;		
if ((Operation == 'I') && ((EEPROM_data <<n) & 0x80))PGD_cmd_H;  else 	PGD_cmd_L; 
PGC_H; PGClock_H;   
if (PGD_resp_H) (Echo = (Echo | (1<< (7-n))));		
}PGC_L;	PGClock_L;
if(Operation == 'W'){five_msec_delay;}	
return Echo;}




/********************************************************************************************/
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
    
