/*
	Interface for text module.

*/

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

	//Call to initialise text drawing with a string
	void text_set(char *ptr);
	void text_set_P(PGM_P ptr);
	
	//Call to draw the next line of text in the Y direction, x,y,z is top, font is 5 pixels high
	void text_draw_y(unsigned char x, unsigned char y, unsigned char z);
	
	//Call to draw the next line of text in the X direction, x,y,z is left
	void text_draw_x(unsigned char x, unsigned char y, unsigned char z);
	
	//If the last line of text has been drawn, this will return true.
	//Subsequent calls to text_draw will have no effect until text_set is called again.
	char text_isfinished(void);
