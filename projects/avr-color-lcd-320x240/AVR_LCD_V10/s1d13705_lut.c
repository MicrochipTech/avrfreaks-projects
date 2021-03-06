/**************************************
        LUT table values:
- 16 Colors
- 256 Colors
- own bmp Colors
**************************************/

/*************************************
        LUT values 16 colors
*************************************/
flash unsigned char LUT_16[16*3] =
{ 
	0x00, 0x00, 0x00,  0x00, 0x00, 0xA0,  0x00, 0xA0, 0x00,  0x00, 0xA0, 0xA0,
	0xA0, 0x00, 0x00,  0xA0, 0x00, 0xA0,  0xA0, 0xA0, 0x00,  0xA0, 0xA0, 0xA0,
	0x00, 0x00, 0x00,  0x00, 0x00, 0xF0,  0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0,
	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,  0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0
};
//*************************************

/**************************************
        LUT values 256 colors
**************************************/
flash unsigned char LUT_256[256*3] =
{
/* Primary and secondary colors */
	0x00, 0x00, 0x00,  0x00, 0x00, 0xA0,  0x00, 0xA0, 0x00,  0x00, 0xA0, 0xA0,  
	0xA0, 0x00, 0x00,  0xA0, 0x00, 0xA0,  0xA0, 0xA0, 0x00,  0xA0, 0xA0, 0xA0,  
	0x50, 0x50, 0x50,  0x00, 0x00, 0xF0,  0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0,  
	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,  0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0,  

/* Gray shades */
	0x00, 0x00, 0x00,  0x10, 0x10, 0x10,  0x20, 0x20, 0x20,  0x30, 0x30, 0x30,  
	0x40, 0x40, 0x40,  0x50, 0x50, 0x50,  0x60, 0x60, 0x60,  0x70, 0x70, 0x70,  
	0x80, 0x80, 0x80,  0x90, 0x90, 0x90,  0xA0, 0xA0, 0xA0,  0xB0, 0xB0, 0xB0,  
	0xC0, 0xC0, 0xC0,  0xD0, 0xD0, 0xD0,  0xE0, 0xE0, 0xE0,  0xF0, 0xF0, 0xF0,  

/* Black to red */
	0x00, 0x00, 0x00,  0x10, 0x00, 0x00,  0x20, 0x00, 0x00,  0x30, 0x00, 0x00,  
	0x40, 0x00, 0x00,  0x50, 0x00, 0x00,  0x60, 0x00, 0x00,  0x70, 0x00, 0x00,  
	0x80, 0x00, 0x00,  0x90, 0x00, 0x00,  0xA0, 0x00, 0x00,  0xB0, 0x00, 0x00,  
	0xC0, 0x00, 0x00,  0xD0, 0x00, 0x00,  0xE0, 0x00, 0x00,  0xF0, 0x00, 0x00,  

/* Black to green */
	0x00, 0x00, 0x00,  0x00, 0x10, 0x00,  0x00, 0x20, 0x00,  0x00, 0x30, 0x00,  
	0x00, 0x40, 0x00,  0x00, 0x50, 0x00,  0x00, 0x60, 0x00,  0x00, 0x70, 0x00,  
	0x00, 0x80, 0x00,  0x00, 0x90, 0x00,  0x00, 0xA0, 0x00,  0x00, 0xB0, 0x00,  
	0x00, 0xC0, 0x00,  0x00, 0xD0, 0x00,  0x00, 0xE0, 0x00,  0x00, 0xF0, 0x00,  

/* Black to blue */
	0x00, 0x00, 0x00,  0x00, 0x00, 0x10,  0x00, 0x00, 0x20,  0x00, 0x00, 0x30,  
	0x00, 0x00, 0x40,  0x00, 0x00, 0x50,  0x00, 0x00, 0x60,  0x00, 0x00, 0x70,  
	0x00, 0x00, 0x80,  0x00, 0x00, 0x90,  0x00, 0x00, 0xA0,  0x00, 0x00, 0xB0,  
	0x00, 0x00, 0xC0,  0x00, 0x00, 0xD0,  0x00, 0x00, 0xE0,  0x00, 0x00, 0xF0,

/* Blue to cyan (blue and green) */
	0x00, 0x00, 0xF0,  0x00, 0x10, 0xF0,  0x00, 0x20, 0xF0,  0x00, 0x30, 0xF0,  
	0x00, 0x40, 0xF0,  0x00, 0x50, 0xF0,  0x00, 0x60, 0xF0,  0x00, 0x70, 0xF0,  
	0x00, 0x80, 0xF0,  0x00, 0x90, 0xF0,  0x00, 0xA0, 0xF0,  0x00, 0xB0, 0xF0,  
	0x00, 0xC0, 0xF0,  0x00, 0xD0, 0xF0,  0x00, 0xE0, 0xF0,  0x00, 0xF0, 0xF0,

/* Cyan (blue and green) to green */
	0x00, 0xF0, 0xF0,  0x00, 0xF0, 0xE0,  0x00, 0xF0, 0xD0,  0x00, 0xF0, 0xC0,  
	0x00, 0xF0, 0xB0,  0x00, 0xF0, 0xA0,  0x00, 0xF0, 0x90,  0x00, 0xF0, 0x80,  
	0x00, 0xF0, 0x70,  0x00, 0xF0, 0x60,  0x00, 0xF0, 0x50,  0x00, 0xF0, 0x40,  
	0x00, 0xF0, 0x30,  0x00, 0xF0, 0x20,  0x00, 0xF0, 0x10,  0x00, 0xF0, 0x00,  

/* Green to yellow (red and green) */
	0x00, 0xF0, 0x00,  0x10, 0xF0, 0x00,  0x20, 0xF0, 0x00,  0x30, 0xF0, 0x00,  
	0x40, 0xF0, 0x00,  0x50, 0xF0, 0x00,  0x60, 0xF0, 0x00,  0x70, 0xF0, 0x00,  
	0x80, 0xF0, 0x00,  0x90, 0xF0, 0x00,  0xA0, 0xF0, 0x00,  0xB0, 0xF0, 0x00,  
	0xC0, 0xF0, 0x00,  0xD0, 0xF0, 0x00,  0xE0, 0xF0, 0x00,  0xF0, 0xF0, 0x00,  

/* Yellow (red and green) to red */
	0xF0, 0xF0, 0x00,  0xF0, 0xE0, 0x00,  0xF0, 0xD0, 0x00,  0xF0, 0xC0, 0x00,  
	0xF0, 0xB0, 0x00,  0xF0, 0xA0, 0x00,  0xF0, 0x90, 0x00,  0xF0, 0x80, 0x00,  
	0xF0, 0x70, 0x00,  0xF0, 0x60, 0x00,  0xF0, 0x50, 0x00,  0xF0, 0x40, 0x00,  
	0xF0, 0x30, 0x00,  0xF0, 0x20, 0x00,  0xF0, 0x10, 0x00,  0xF0, 0x00, 0x00,  

/* Red to magenta (blue and red) */
	0xF0, 0x00, 0x00,  0xF0, 0x00, 0x10,  0xF0, 0x00, 0x20,  0xF0, 0x00, 0x30,  
	0xF0, 0x00, 0x40,  0xF0, 0x00, 0x50,  0xF0, 0x00, 0x60,  0xF0, 0x00, 0x70,  
	0xF0, 0x00, 0x80,  0xF0, 0x00, 0x90,  0xF0, 0x00, 0xA0,  0xF0, 0x00, 0xB0,  
	0xF0, 0x00, 0xC0,  0xF0, 0x00, 0xD0,  0xF0, 0x00, 0xE0,  0xF0, 0x00, 0xF0,  

/* Magenta (blue and red) to blue */
	0xF0, 0x00, 0xF0,  0xE0, 0x00, 0xF0,  0xD0, 0x00, 0xF0,  0xC0, 0x00, 0xF0,  
	0xB0, 0x00, 0xF0,  0xA0, 0x00, 0xF0,  0x90, 0x00, 0xF0,  0x80, 0x00, 0xF0,  
	0x70, 0x00, 0xF0,  0x60, 0x00, 0xF0,  0x50, 0x00, 0xF0,  0x40, 0x00, 0xF0,  
	0x30, 0x00, 0xF0,  0x20, 0x00, 0xF0,  0x10, 0x00, 0xF0,  0x00, 0x00, 0xF0,

/* Black to magenta (blue and red) */
	0x00, 0x00, 0x00,  0x10, 0x00, 0x10,  0x20, 0x00, 0x20,  0x30, 0x00, 0x30,  
	0x40, 0x00, 0x40,  0x50, 0x00, 0x50,  0x60, 0x00, 0x60,  0x70, 0x00, 0x70,  
	0x80, 0x00, 0x80,  0x90, 0x00, 0x90,  0xA0, 0x00, 0xA0,  0xB0, 0x00, 0xB0,  
	0xC0, 0x00, 0xC0,  0xD0, 0x00, 0xD0,  0xE0, 0x00, 0xE0,  0xF0, 0x00, 0xF0,  

/* Black to cyan (blue and green) */
	0x00, 0x00, 0x00,  0x00, 0x10, 0x10,  0x00, 0x20, 0x20,  0x00, 0x30, 0x30,  
	0x00, 0x40, 0x40,  0x00, 0x50, 0x50,  0x00, 0x60, 0x60,  0x00, 0x70, 0x70,  
	0x00, 0x80, 0x80,  0x00, 0x90, 0x90,  0x00, 0xA0, 0xA0,  0x00, 0xB0, 0xB0,  
	0x00, 0xC0, 0xC0,  0x00, 0xD0, 0xD0,  0x00, 0xE0, 0xE0,  0x00, 0xF0, 0xF0,  

/* Red to white */
	0xF0, 0x00, 0x00,  0xF0, 0x10, 0x10,  0xF0, 0x20, 0x20,  0xF0, 0x30, 0x30,  
	0xF0, 0x40, 0x40,  0xF0, 0x50, 0x50,  0xF0, 0x60, 0x60,  0xF0, 0x70, 0x70,  
	0xF0, 0x80, 0x80,  0xF0, 0x90, 0x90,  0xF0, 0xA0, 0xA0,  0xF0, 0xB0, 0xB0,  
	0xF0, 0xC0, 0xC0,  0xF0, 0xD0, 0xD0,  0xF0, 0xE0, 0xE0,  0xF0, 0xF0, 0xF0,  

/* Green to white */
	0x00, 0xF0, 0x00,  0x10, 0xF0, 0x10,  0x20, 0xF0, 0x20,  0x30, 0xF0, 0x30,  
	0x40, 0xF0, 0x40,  0x50, 0xF0, 0x50,  0x60, 0xF0, 0x60,  0x70, 0xF0, 0x70,  
	0x80, 0xF0, 0x80,  0x90, 0xF0, 0x90,  0xA0, 0xF0, 0xA0,  0xB0, 0xF0, 0xB0,  
	0xC0, 0xF0, 0xC0,  0xD0, 0xF0, 0xD0,  0xE0, 0xF0, 0xE0,  0xF0, 0xF0, 0xF0,  

/* Blue to white */
	0x00, 0x00, 0xF0,  0x10, 0x10, 0xF0,  0x20, 0x20, 0xF0,  0x30, 0x30, 0xF0,  
	0x40, 0x40, 0xF0,  0x50, 0x50, 0xF0,  0x60, 0x60, 0xF0,  0x70, 0x70, 0xF0,  
	0x80, 0x80, 0xF0,  0x90, 0x90, 0xF0,  0xA0, 0xA0, 0xF0,  0xB0, 0xB0, 0xF0,  
	0xC0, 0xC0, 0xF0,  0xD0, 0xD0, 0xF0,  0xE0, 0xE0, 0xF0,  0xF0, 0xF0, 0xF0
};
//*************************************

/**************************************
        LUT TABLE PIC 256 Colors
BMP hs not RGB but BGR value, in function
S1D13705_LUT_256_BMP these values are
automaticly being switch
***************************************/
flash unsigned char LUT_256_BMP[256*3] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x80, 0x80, 
	0x80, 0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 
	0xC0, 0xDC, 0xC0, 0xF0, 0xCA, 0xA6, 0x00, 0x20, 0x40, 0x00, 0x20, 0x60, 
	0x00, 0x20, 0x80, 0x00, 0x20, 0xA0, 0x00, 0x20, 0xC0, 0x00, 0x20, 0xE0, 
	
	0x00, 0x40, 0x00, 0x00, 0x40, 0x20, 0x00, 0x40, 0x40, 0x00, 0x40, 0x60, 
	0x00, 0x40, 0x80, 0x00, 0x40, 0xA0, 0x00, 0x40, 0xC0, 0x00, 0x40, 0xE0, 
	0x00, 0x60, 0x00, 0x00, 0x60, 0x20, 0x00, 0x60, 0x40, 0x00, 0x60, 0x60, 
	0x00, 0x60, 0x80, 0x00, 0x60, 0xA0, 0x00, 0x60, 0xC0, 0x00, 0x60, 0xE0, 

	0x00, 0x80, 0x00, 0x00, 0x80, 0x20, 0x00, 0x80, 0x40, 0x00, 0x80, 0x60, 
	0x00, 0x80, 0x80, 0x00, 0x80, 0xA0, 0x00, 0x80, 0xC0, 0x00, 0x80, 0xE0, 
	0x00, 0xA0, 0x00, 0x00, 0xA0, 0x20, 0x00, 0xA0, 0x40, 0x00, 0xA0, 0x60, 
	0x00, 0xA0, 0x80, 0x00, 0xA0, 0xA0, 0x00, 0xA0, 0xC0, 0x00, 0xA0, 0xE0, 

	0x00, 0xC0, 0x00, 0x00, 0xC0, 0x20, 0x00, 0xC0, 0x40, 0x00, 0xC0, 0x60, 
	0x00, 0xC0, 0x80, 0x00, 0xC0, 0xA0, 0x00, 0xC0, 0xC0, 0x00, 0xC0, 0xE0, 
	0x00, 0xE0, 0x00, 0x00, 0xE0, 0x20, 0x00, 0xE0, 0x40, 0x00, 0xE0, 0x60, 
	0x00, 0xE0, 0x80, 0x00, 0xE0, 0xA0, 0x00, 0xE0, 0xC0, 0x00, 0xE0, 0xE0, 

	0x40, 0x00, 0x00, 0x40, 0x00, 0x20, 0x40, 0x00, 0x40, 0x40, 0x00, 0x60, 
	0x40, 0x00, 0x80, 0x40, 0x00, 0xA0, 0x40, 0x00, 0xC0, 0x40, 0x00, 0xE0, 
	0x40, 0x20, 0x00, 0x40, 0x20, 0x20, 0x40, 0x20, 0x40, 0x40, 0x20, 0x60, 
	0x40, 0x20, 0x80, 0x40, 0x20, 0xA0, 0x40, 0x20, 0xC0, 0x40, 0x20, 0xE0, 

	0x40, 0x40, 0x00, 0x40, 0x40, 0x20, 0x40, 0x40, 0x40, 0x40, 0x40, 0x60, 
	0x40, 0x40, 0x80, 0x40, 0x40, 0xA0, 0x40, 0x40, 0xC0, 0x40, 0x40, 0xE0, 
	0x40, 0x60, 0x00, 0x40, 0x60, 0x20, 0x40, 0x60, 0x40, 0x40, 0x60, 0x60, 
	0x40, 0x60, 0x80, 0x40, 0x60, 0xA0, 0x40, 0x60, 0xC0, 0x40, 0x60, 0xE0, 
	
	0x40, 0x80, 0x00, 0x40, 0x80, 0x20, 0x40, 0x80, 0x40, 0x40, 0x80, 0x60, 
	0x40, 0x80, 0x80, 0x40, 0x80, 0xA0, 0x40, 0x80, 0xC0, 0x40, 0x80, 0xE0, 
	0x40, 0xA0, 0x00, 0x40, 0xA0, 0x20, 0x40, 0xA0, 0x40, 0x40, 0xA0, 0x60, 
	0x40, 0xA0, 0x80, 0x40, 0xA0, 0xA0, 0x40, 0xA0, 0xC0, 0x40, 0xA0, 0xE0, 
	
	0x40, 0xC0, 0x00, 0x40, 0xC0, 0x20, 0x40, 0xC0, 0x40, 0x40, 0xC0, 0x60, 
	0x40, 0xC0, 0x80, 0x40, 0xC0, 0xA0, 0x40, 0xC0, 0xC0, 0x40, 0xC0, 0xE0, 
	0x40, 0xE0, 0x00, 0x40, 0xE0, 0x20, 0x40, 0xE0, 0x40, 0x40, 0xE0, 0x60, 
	0x40, 0xE0, 0x80, 0x40, 0xE0, 0xA0, 0x40, 0xE0, 0xC0, 0x40, 0xE0, 0xE0, 
	
	0x80, 0x00, 0x00, 0x80, 0x00, 0x20, 0x80, 0x00, 0x40, 0x80, 0x00, 0x60, 
	0x80, 0x00, 0x80, 0x80, 0x00, 0xA0, 0x80, 0x00, 0xC0, 0x80, 0x00, 0xE0, 
	0x80, 0x20, 0x00, 0x80, 0x20, 0x20, 0x80, 0x20, 0x40, 0x80, 0x20, 0x60, 
	0x80, 0x20, 0x80, 0x80, 0x20, 0xA0, 0x80, 0x20, 0xC0, 0x80, 0x20, 0xE0, 
	
	0x80, 0x40, 0x00, 0x80, 0x40, 0x20, 0x80, 0x40, 0x40, 0x80, 0x40, 0x60, 
	0x80, 0x40, 0x80, 0x80, 0x40, 0xA0, 0x80, 0x40, 0xC0, 0x80, 0x40, 0xE0, 
	0x80, 0x60, 0x00, 0x80, 0x60, 0x20, 0x80, 0x60, 0x40, 0x80, 0x60, 0x60, 
	0x80, 0x60, 0x80, 0x80, 0x60, 0xA0, 0x80, 0x60, 0xC0, 0x80, 0x60, 0xE0, 
	
	0x80, 0x80, 0x00, 0x80, 0x80, 0x20, 0x80, 0x80, 0x40, 0x80, 0x80, 0x60, 
	0x80, 0x80, 0x80, 0x80, 0x80, 0xA0, 0x80, 0x80, 0xC0, 0x80, 0x80, 0xE0, 
	0x80, 0xA0, 0x00, 0x80, 0xA0, 0x20, 0x80, 0xA0, 0x40, 0x80, 0xA0, 0x60, 
	0x80, 0xA0, 0x80, 0x80, 0xA0, 0xA0, 0x80, 0xA0, 0xC0, 0x80, 0xA0, 0xE0, 

	0x80, 0xC0, 0x00, 0x80, 0xC0, 0x20, 0x80, 0xC0, 0x40, 0x80, 0xC0, 0x60, 
	0x80, 0xC0, 0x80, 0x80, 0xC0, 0xA0, 0x80, 0xC0, 0xC0, 0x80, 0xC0, 0xE0, 
	0x80, 0xE0, 0x00, 0x80, 0xE0, 0x20, 0x80, 0xE0, 0x40, 0x80, 0xE0, 0x60, 
	0x80, 0xE0, 0x80, 0x80, 0xE0, 0xA0, 0x80, 0xE0, 0xC0, 0x80, 0xE0, 0xE0, 

	0xC0, 0x00, 0x00, 0xC0, 0x00, 0x20, 0xC0, 0x00, 0x40, 0xC0, 0x00, 0x60, 
	0xC0, 0x00, 0x80, 0xC0, 0x00, 0xA0, 0xC0, 0x00, 0xC0, 0xC0, 0x00, 0xE0, 
	0xC0, 0x20, 0x00, 0xC0, 0x20, 0x20, 0xC0, 0x20, 0x40, 0xC0, 0x20, 0x60, 
	0xC0, 0x20, 0x80, 0xC0, 0x20, 0xA0, 0xC0, 0x20, 0xC0, 0xC0, 0x20, 0xE0, 

	0xC0, 0x40, 0x00, 0xC0, 0x40, 0x20, 0xC0, 0x40, 0x40, 0xC0, 0x40, 0x60, 
	0xC0, 0x40, 0x80, 0xC0, 0x40, 0xA0, 0xC0, 0x40, 0xC0, 0xC0, 0x40, 0xE0, 
	0xC0, 0x60, 0x00, 0xC0, 0x60, 0x20, 0xC0, 0x60, 0x40, 0xC0, 0x60, 0x60, 
	0xC0, 0x60, 0x80, 0xC0, 0x60, 0xA0, 0xC0, 0x60, 0xC0, 0xC0, 0x60, 0xE0, 

	0xC0, 0x80, 0x00, 0xC0, 0x80, 0x20, 0xC0, 0x80, 0x40, 0xC0, 0x80, 0x60, 
	0xC0, 0x80, 0x80, 0xC0, 0x80, 0xA0, 0xC0, 0x80, 0xC0, 0xC0, 0x80, 0xE0, 
	0xC0, 0xA0, 0x00, 0xC0, 0xA0, 0x20, 0xC0, 0xA0, 0x40, 0xC0, 0xA0, 0x60, 
	0xC0, 0xA0, 0x80, 0xC0, 0xA0, 0xA0, 0xC0, 0xA0, 0xC0, 0xC0, 0xA0, 0xE0, 

	0xC0, 0xC0, 0x00, 0xC0, 0xC0, 0x20, 0xC0, 0xC0, 0x40, 0xC0, 0xC0, 0x60, 
	0xC0, 0xC0, 0x80, 0xC0, 0xC0, 0xA0, 0xF0, 0xFB, 0xFF, 0xA4, 0xA0, 0xA0, 
	0x80, 0x80, 0x80, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 
	0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF 
};
//*************************************

/**************************************
        LUT TABEL PIC 16 Colors  
BMP has not RGB value but BGR in function:
S1D13705_LUT_16_BMP, the red and blue
are switch automaticly!
**************************************/
flash unsigned char LUT_16_BMP[16*3] =
{    
	0x00, 0x00, 0x00,  0x00, 0x00, 0x80,   0x00, 0x80, 0x00,  0x00, 0x80, 0x80, 
	0x80, 0x00, 0x00,  0x80, 0x00, 0x80,   0x80, 0x80, 0x00,  0x80, 0x80, 0x80, 
	0xC0, 0xC0, 0xC0,  0x00, 0x00, 0xF0,   0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0, 
	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,   0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0
};
//*************************************
                                        
/**************************************
Functie: Activeer LUT 16 colors
**************************************/
void S1D13705_LUT_16(void)
{
        flash unsigned char *pLUT;
        unsigned char i;
        
        //Stel LUT tabel informatie in
        S1D13705_Write_Register(0x15,0);                                //Stel eerst LUT adres register in
        
        pLUT = LUT_16;                                                  //pLUT wijst naar de LUT array 
        for(i=0;i<16;i++)
        {
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet rood
                pLUT++;        
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet groen
                pLUT++;        
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet blauw
                pLUT++;                                                   
        }
        
        return;
}
//*************************************

/**************************************
Functie: Activeer LUT 256 colors
**************************************/
void S1D13705_LUT_256(void)
{
        flash unsigned char *pLUT;
        unsigned int i;
        
        //Stel LUT tabel informatie in
        S1D13705_Write_Register(0x15,0);                                //Stel eerst LUT adres register in
        
        pLUT = LUT_256;                                                 //pLUT wijst naar de LUT array 
        for(i=0;i<256;i++)
        {
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet rood
                pLUT++;        
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet groen
                pLUT++;        
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet blauw
                pLUT++;                                                   
        }
        
        return;
}
//*************************************

/**************************************
Functie: Write LUT table 16 colors for
         BMP. LUT table BMP begins with
         the blue color and not red!
         LUT BMP is BGR, S1D13705 LUT is RGB!
**************************************/
void S1D13705_LUT_16_BMP(void)
{
        flash unsigned char *pLUT;
        unsigned char red;
        unsigned char green;
        unsigned char blue;
        unsigned char i;
        
        //Set first the LUT adress at 0
        S1D13705_Write_Register(0x15,0);                                
        
        pLUT = LUT_16_BMP;                                              //pLUT points to LUT_16_BMP
        
        //Write LUT values to LUT register        
        for(i=0;i<16;i++)
        {
                //Read color values from LUT table
                blue = *pLUT++;                                         //Read the blue value from LUT table
                green = *pLUT++;                                        //Read the green value from LUT table
                red = *pLUT++;                                          //Read the red value from LUT table
                
                //Write color values to LUT register S1D13705
                S1D13705_Write_Register(0x17,red);                      //Write red value
                S1D13705_Write_Register(0x17,green);                    //Write green value
                S1D13705_Write_Register(0x17,blue);                     //Write blue value
        }
        
        return;
}
//*************************************

/**************************************
Functie: Write LUT tabel 256 colors for
         BMP, LUT table BMP begins with
         the blue color and not red!
         LUT BMP is BGR, S1D13705 LUT is RGB!
**************************************/
void S1D13705_LUT_256_BMP(void)
{
        flash unsigned char *pLUT;
        unsigned char red;
        unsigned char green;
        unsigned char blue;
        unsigned int i;
        
        //Set first the LUT adress at 0
        S1D13705_Write_Register(0x15,0);                                
        
        pLUT = LUT_256_BMP;                                             //pLUT points to LUT_16_BMP
        
        //Schrijf LUT waardes, let erop
        for(i=0;i<256;i++)
        {
                //Read color values from LUT table
                blue = *pLUT++;                                         //Read the blue value from LUT table                                    
                green = *pLUT++;                                        //Read the green value from LUT table
                red = *pLUT++;                                          //Read the red value from LUT table
                
                //Write color values to LUT register S1D13705
                S1D13705_Write_Register(0x17,red);                      //Write red value  
                S1D13705_Write_Register(0x17,green);                    //Write green value
                S1D13705_Write_Register(0x17,blue);                     //Write blue value  
        }
        
        return;
}
//************************************* 
        
