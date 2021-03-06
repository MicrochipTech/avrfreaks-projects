/*

	Interface to console module

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

	
	//Run console, will return when user exits top level menu.
	void console_main(void);
	
	//Call every timer tick, to provide module timing
	void console_tick(void);
	
	//Hardware test sequence
	void console_test(void);
	
	//Pass input characters
	void console_rx(char text_char);
