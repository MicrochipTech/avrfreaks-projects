#include <avr/io.h>

#ifndef F_CPU
	#define F_CPU 12000000UL
#endif

#include <util/delay.h>



/*_________________________________________________________________________________________*/

/************************************************
	LCD CONNECTIONS
*************************************************/

#define LCD_TYPE 4	//If you have lcd 16x4 keep this and comment the below
//#define LCD_TYPE 2	//If you have lcd 16x2 keep this and comment the above

//************************************************



/***************************************************
			F U N C T I O N S
****************************************************/



void initial_menu(uint8_t table_number);

//If direction = 0 rotates menu up, if 1 rotates down else if 2 returns possition
//Also returns the possition of the "<" after the rotation 
//If table_number = 0 we select the first table if 1 the second etc
uint8_t rotate_menu(uint8_t direction, uint8_t table_number);



/***************************************************
			F U N C T I O N S     E N D
****************************************************/


/***************************************************
	M A C R O S
***************************************************/

/***************************************************/




/*_________________________________________________________________________________________*/






