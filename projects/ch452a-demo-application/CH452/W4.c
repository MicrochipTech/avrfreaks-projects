#include	"PIN.H"			
#include	"CH452CMD.H"	

void CH452_Write(unsigned short cmd)
{
	unsigned char i;	  
	CH452_LOAD_CLR;                		
	for(i=0;i!=12;i++)				
	{
		if (cmd&1) {CH452_DIN_SET;}
		else {CH452_DIN_CLR;}  
    	CH452_DCLK_CLR;
    	cmd>>=1;
    	CH452_DCLK_SET;             
	}
  	CH452_LOAD_SET;         
}


unsigned char CH452_Read( void )
{
	unsigned char i;
  	unsigned char cmd,keycode;					               
  	cmd=0x07;			               
  	CH452_LOAD_CLR;
  	for(i=0;i!=4;i++)  
	{
		if (cmd&1) {CH452_DIN_SET;}
		else {CH452_DIN_CLR;}  		      
    	CH452_DCLK_CLR;			 
    	cmd>>=1;			      
    	CH452_DCLK_SET;			      
 	}
  	CH452_LOAD_SET;				      
  	keycode=0;				      
  	for(i=0;i!=7;i++)
	{
		if (CH452_PIN & (1<<CH452_DOUT_PIN)) keycode++;      
    	CH452_DCLK_CLR;			 
		keycode<<=1;     
    	CH452_DCLK_SET;
 	}
  	return(keycode);			     
}

void CH452_Init(void)
{
	CH452_DDR = (1<<CH452_DCLK_PIN) | (1<<CH452_DIN_PIN) | (1<<CH452_LOAD_PIN);
	CH452_DDR &= ~(1<<CH452_DOUT_PIN);
	CH452_PORT |= (1<<CH452_DCLK_PIN) | (1<<CH452_LOAD_PIN) | (1<<CH452_DOUT_PIN);
	CH452_PORT &= ~(1<<CH452_DIN_PIN);
}
