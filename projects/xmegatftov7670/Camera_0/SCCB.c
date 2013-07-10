#include "SCCB.h"

void InitSCCB(void)
{
 SCCB_DDR|=(1<<SCCB_SIO_C)|(1<<SCCB_SIO_D);
 SCCB_PORTSET=(1<<SCCB_SIO_C)|(1<<SCCB_SIO_D);
}

void startSCCB(void)
{
 SIO_D_SET;
 delay_us(100);

 SIO_C_SET;
 delay_us(100);
 
 SIO_D_CLR;
 delay_us(100);

 SIO_C_CLR;
 delay_us(100);
}

void stopSCCB(void)
{
 SIO_D_CLR;
 delay_us(100);
 
 SIO_C_SET;
 delay_us(100);  

 SIO_D_SET;
 delay_us(100);
}

void noAck(void)
{	
 SIO_D_SET;
 delay_us(100);
	
 SIO_C_SET;
 delay_us(100);
	
 SIO_C_CLR;
 delay_us(100);
	
 SIO_D_CLR;
 delay_us(100);
}


uchar SCCBwriteByte(uchar m_data)
{
 unsigned char j,tem;

 for(j=0;j<8;j++)
 {
  if((m_data<<j)&0x80)
  {
   SIO_D_SET;
  }
  else
  {
   SIO_D_CLR;
  }
  delay_us(100);
  SIO_C_SET;
  delay_us(100);
  SIO_C_CLR;
  delay_us(100);
 }
 delay_us(100);	
 SIO_D_IN;
 delay_us(100);
 SIO_C_SET;
 delay_us(1000);
 if(SIO_D_STATE)
 {
  tem=0;   //SDA=1
 }
 else
 {
  tem=1;   //SDA=0 
 }
 SIO_C_CLR;
 delay_us(100);	
 SIO_D_OUT;

 return(tem);  
}


uchar SCCBreadByte(void)
{
 unsigned char read,j;
 read=0x00;
	
 SIO_D_IN;
 delay_us(100);
 for(j=8;j>0;j--)
 {		     
  delay_us(100);
  SIO_C_SET;
  delay_us(100);
  read=read<<1;
  if(SIO_D_STATE) 
  {
   read=read+1;
  }
  SIO_C_CLR;
  delay_us(100);
 }	
 return(read);
}
