/*****************************************************
File                    : rtc.h
Version                 : 1.0
Date                    : 15/06/2007
Author                  : Ehab Anwar
                          flik_80@yahoo.com
                          +20107697744                            
Company                 : BDR Electronics                            
*****************************************************/

#define SCLK PORTC.4
#define CE PORTC.3

/////////////////////////
void ds1302_reset()    //
 {                     //
 SCLK = 0;             //
 CE = 0;               // 
 delay_us(1);          //
 CE = 1;               // 
 delay_us(1);          //
 }                     //
/////////////////////////

//////////////////////////////////////////////
void ds1302_wbyte(char data)	            //
{                                           //
char a;                                     //
DDRC|=0x20;//IO=output                      //
for(a=0;a<8;++a)                            //
 {                                          //
 PORTC.5=(0x01&(data>>a));                  //
 SCLK = 0;                                  // 
 delay_us(1);                               //
 SCLK = 1;                                  // 
 delay_us(1);                               //
 }                                          //
}                                           //
//////////////////////////////////////////////

//////////////////////////////////////////////
/* --- read one byte from the device --- */ //
char ds1302_rbyte(void)	                    //
{                                           //
char a;                                     //
char data=0;                                //
DDRC&=0xdf;//IO=input                       //
for(a=0;a<8;a++)                            //
 {                                          //
 SCLK = 1;                                  // 
 delay_us(1);                               //
 SCLK = 0;                                  // 
 delay_us(1);                               //
 if(PINC.5)                                 //
  data|=((char)1)<<a;                       //
 }                                          //
return data;                                //
}                                           //
//////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/* --- write one byte using values entered by user --- */  //
void ds1302_writebyte(char ClkAdd,char ClkData)	           //
{                                                          //
ds1302_reset();                                            //
ds1302_wbyte(ClkAdd);                                      //
ds1302_wbyte(ClkData);                                     //
ds1302_reset();                                            //
}                                                          //
/////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/* --- initialize time & date from user entries ---  */                          //
void write_clk_regs(char sec,char min,char hr) //
{                                                                                //
ds1302_reset();                                                                  //
ds1302_writebyte(0x8e,0); /* control register */                                 //
ds1302_reset();                                                                  //
ds1302_writebyte(0x90,0xab);/* trickle charger register */                       //
ds1302_reset();                                                                  //
ds1302_wbyte(0xbe); /* clock burst write (eight registers) */                    //
ds1302_wbyte(sec);                                                               //
ds1302_wbyte(min);                                                               //
ds1302_wbyte(hr);                                                                //
ds1302_wbyte(1);                                                              //
ds1302_wbyte(1);                                                                //
ds1302_wbyte(1);                                                                //
ds1302_wbyte(1);                                                                //
ds1302_wbyte(0x80);    /* must write control register in burst mode */           //
ds1302_reset();                                                                  //
}                                                                                //
///////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
void read_clk_regs(char *sec,char *min,char *hr) //
 {                                                                                     //
 char temp;
 ds1302_reset();                                                                       //
 ds1302_wbyte(0xBF);	/* clock burst */                                              //
 *sec=ds1302_rbyte();                                                                  //
 *min=ds1302_rbyte();                                                                  //
 *hr=ds1302_rbyte();                                                                   //
 temp=ds1302_rbyte();                                                                 //
 temp=ds1302_rbyte();                                                                   //
 temp=ds1302_rbyte();                                                                   //
 temp=ds1302_rbyte();                                                                   //
 ds1302_reset();                                                                       //
 }                                                                                     //
 ////////////////////////////////////////////////////////////////////////////////////////