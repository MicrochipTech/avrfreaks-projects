
#include <avr/wdt.h>
#define signature_bit_2_h 0x3000
#define signature_bit_2_l 0x0100
#define signature_bit_3_h 0x3000
#define signature_bit_3_l 0x0200
#define SW_reset {wdt_enable(WDTO_30MS);while(1);}

int sig_byte_2, sig_byte_3;
signed int PageSZ;												        //Size of a page of flash in 16 bit words
signed int PAmask;												        //Used to obtain the flash page address from the hex address
unsigned int FlashSZ;											        //Amount of flash memory in 16 bit words supplied on target device
int EE_size;													            //EEPROM size
unsigned char Fuse_Ex, Fuse_H, Fuse_L, Lock;      //All fuse (configuration byte)setings can be modified by the user

char Twd_EEPROM;

void sendString(const char*);
void sendChar(char);
unsigned char Atmel_config ( unsigned int, unsigned int );
void set_up_target_parameters(void);
char waitforkeypress(void);
void newline(void);





/**************************************************************************************************************/

void set_up_target_parameters(void){


const char *Atmega = "\r\nAtmega ";            
const char *ATtiny = "\r\nATtiny ";
const char *Unrecognised_device = "\r\nUnrecognised_device:  ";

Twd_EEPROM = 5;																            //default delay in mS

sig_byte_2 = Atmel_config(signature_bit_2_h, signature_bit_2_l);
sig_byte_3 = Atmel_config(signature_bit_3_h, signature_bit_3_l);


switch(sig_byte_2)
	{case 0x96: 															            //64KB devices
	switch (sig_byte_3)
	{case 0x09: sendString(Atmega); sendString(" 644");break;	
	default: sendString(Unrecognised_device);sendString("64KB");
	newline();SW_reset;break;} 
	
	PageSZ = 0x80;
	PAmask = 0x7F80;														          //=FlashSZ - PageSZ
	FlashSZ = 0x8000;
	EE_size = 0x800;
	Fuse_Ex = 0xFF;
	Fuse_H = 0xD7;
	Fuse_L = 0xE2;
	Lock = 0xFF;
	break;
	
	case 0x95: 																              //32KB devices
	switch (sig_byte_3)
	{case 0x02: sendString(Atmega);sendString("32A"); 
	Twd_EEPROM = 10; break;													        //EEPROM requires 10mS for write
	case 0x14: sendString(Atmega);sendString("328");break;
	case 0x0F: sendString(Atmega);sendString("328P");break;
	default: sendString(Unrecognised_device);sendString("32KB");
	newline();SW_reset;break;}
	 
	PageSZ = 0x40;
	PAmask = 0x3FC0;														            //=FlashSZ - PageSZ
	FlashSZ = 0x4000;
	EE_size = 0x400;
	Fuse_Ex = 0xFF;
	Fuse_H = 0xD7;
	Fuse_L = 0xE2;
	Lock = 0xFF;
	
	if(sig_byte_3 == 0x02)
	{Fuse_H = 0xD1;
	Fuse_L = 0xE4;}
	break;
	
	
	case 0x94:  															            //16KB devices
	switch (sig_byte_3)
	{case 0x06: sendString(Atmega);sendString("168");break;
	case 0x0B: sendString(Atmega);sendString("168P");break;
	default: sendString(Unrecognised_device);sendString("16KB");
	newline();SW_reset;break;}
	
	PageSZ = 0x40;			
	PAmask = 0x1FC0;														            //=FlashSZ - PageSZ		
	FlashSZ = 0x2000;		
	EE_size = 0x200;		
	Fuse_Ex = 0xFF;
	Fuse_H = 0xD7;
	Fuse_L = 0xE2;
	Lock = 0xFF;
	break;
	
	
	case 0x93:  																            //8KB devices
	switch (sig_byte_3)
	{case 0x0A: sendString(Atmega);sendString("88");break;
	case 0x0F: sendString(Atmega);sendString("88P");break;
	case 0x0C: sendString(ATtiny);sendString("84");break;
	case 0x0D: sendString(ATtiny);sendString("861A");break;
	default: sendString(Unrecognised_device);sendString("8KB");
	newline();SW_reset;break;}
	 
	PageSZ = 0x20;			
	PAmask = 0xFE0;																            //=FlashSZ - PageSZ	
	FlashSZ = 0x1000;		
	EE_size = 0x200;		
	Fuse_Ex = 0xFF;
	Fuse_H = 0xD7;
	Fuse_L = 0xE2;
	Lock = 0xFF;
	break;
	
	
	
	case 0x92: 																	            //4KB devices 
	switch (sig_byte_3)
	{case 0x05: sendString(Atmega);sendString("48");break;
	case 0x0A: sendString(Atmega);sendString("48P");break;
	case 0x07:	sendString(ATtiny);sendString("44A");break;
	case 0x08:  sendString(ATtiny);sendString("461A");break;
	default: sendString(Unrecognised_device);sendString("4KB");
	newline();SW_reset;break;}
	
	PageSZ = 0x20;      
  PAmask = 0x7E0;                                           //=FlashSZ - PageSZ   
  FlashSZ = 0x800;    
  EE_size = 0x100;    
  Fuse_Ex = 0xFF;
  Fuse_H = 0xD7;
  Fuse_L = 0xE2;
  Lock = 0xFF;
	break;

case 0x91:
  switch (sig_byte_3)
  {case 0x09: sendString(ATtiny);sendString("26L");
  Twd_EEPROM = 10; break;
  case 0x0C: sendString(ATtiny);sendString("261A");break;  
  case 0x0B: sendString(ATtiny);sendString("24A");break; 
  default: sendString(Unrecognised_device);sendString("2KB");
  newline();SW_reset;break;}
  
  PageSZ =   0x10;    
  PAmask =   0x3F0;                                     //=FlashSZ - PageSZ   
  FlashSZ =  0x400;   
  EE_size =  0x80; 
  Fuse_Ex = 0xFF;
  Fuse_H = 0xF2;
  Fuse_L = 0xE4;
  Lock = 0xFF;
  break;
  
	
	default: sendString("\r\nUnknown, please enter device data."); 
	sendString("\r\n"); wdt_enable(WDTO_60MS);while(1);break;}}
