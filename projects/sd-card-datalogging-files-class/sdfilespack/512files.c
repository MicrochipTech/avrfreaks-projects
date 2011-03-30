/*
This example will write 512 files one sector long
to a formatted(FAT 16) sd card 
*/
#include "SDCARDFILES.h"

#define setupDDRB DDRB |= 0x0C  //PD1 is busy PD2 is error
#define sdERROR PORTD |= 0x08 //we have an error
#define sdnoERROR PORTD &= ~0x08 //we dont have an error
#define sdBUSY PORTD |= 0x04 //card is busy
#define sdnotBUSY PORTD &= ~0x04 //card id not busy

unsigned char buffer[512] ;  // this 512 bytes read from or written to sd card

int main(void)
{
setupDDRB;
sdnoERROR; //set up indicator lights
         
for(int k = 0; k < 512; k++) //use all the root directory
{
  sdnotBUSY; 
  char error = SDCARDFILES.createfile();
  if (error !=0)
  sdERROR;  //card error
   
   for(int i = 0; i < 512; i++)
        buffer[i] = i % 10 + 48;  //fill with 0123456789
		 
   sdBUSY; //blick busy light     
   error = SDCARDFILES.fileappend();  //append one sector to created file
   if (error !=0)
   sdERROR;  //card error 
}//end of create k files
   
while(1){} //wait here
return 1;
}//end of main
