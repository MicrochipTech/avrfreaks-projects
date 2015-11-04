/*
 /////////////////////////////////////////////////////////////////////////////////////////

		______            _   _____ _                       ______  _____
		|  ___|          | | |_   _(_)                     |___  / /  ___|
		| |_ ___  ___  __| |   | |  _ _ __ ___   ___ _ __     / /  \ `--.  ___  __ _
		|  _/ _ \/ _ \/ _` |   | | | | '_ ` _ \ / _ \ '__|   / /    `--. \/ _ \/ _` |
		| ||  __/  __/ (_| |   | | | | | | | | |  __/ |    ./ /    /\__/ /  __/ (_| |
		\_| \___|\___|\__,_|   \_/ |_|_| |_| |_|\___|_|    \_/     \____/ \___|\__, |
																				__/ |
																				|___/

 /////////////////////////////////////////////////////////////////////////////////////////
  ----------------------- Feed Time IO C++ File (tab 4) ----------------------------------
*/

/*
 +--------------------------------------+
 |        LOCAL LIBRARY INCLUDES	    |
 +--------------------------------------+
*/
#include "_Feed_Timer_IO.h"

/*
	+--------------------------------------+
	|     FORWARD FUNCTION DEFINITIONS     |
	+--------------------------------------+
*/


/*
	+--------------------------------------+
	|     CONSTANT DEFINITIONS		       |
	+--------------------------------------+
*/
#define _DISP_7_SEG_NUM_DIGITS	4
#define FS	0X1C
#define GS	0X1D
#define RS	0X1E
#define US	0X1F
/*
 +--------------------------------------+
 |        STRUCTS               	 	|
 +--------------------------------------+
*/
struct _FEED_TIMER_DISP_7_SEG_Struct
{
    uint8_t     BufferIndex=0;
    uint8_t     Buffer[_DISP_7_SEG_NUM_DIGITS];
    uint8_t     PtrBufferBits=0;
    int8_t      Col=1;
	void		(*SecondCallBack)(void)=nullptr;
}_Display_7_Seg;

/*
 +--------------------------------------+
 |        LOCAL FLASH CONSTANTS		 	|
 +--------------------------------------+
*/
	//+------------------------------+
	//| SEGMENTS DEFINTIONS TABLE	 |
	//| FOR ASCII CHARACTERCS		 |
	//+------------------------------+--------------------------------------------------------------------------------------------
	
	//SEGMENTS LOGICAL BIT DEFINITIONS
	
#define  _DISPLAY_SEG_A	0
#define  _DISPLAY_SEG_B	1
#define  _DISPLAY_SEG_C	2
#define  _DISPLAY_SEG_D	3
#define  _DISPLAY_SEG_E	4
#define  _DISPLAY_SEG_F	5
#define  _DISPLAY_SEG_G	6
#define  _DISPLAY_SEG_P	7

const uint8_t PROGMEM _FEED_TIMER_DISP_7_SEG_ASCII_Table[]=
{
	//ASCII FS 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII GS 	     a
	//		   ####
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII RS 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ####
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII US 	     a
	//		   ####
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ####
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII SPACE 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII ! 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII " 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII # 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII $ 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII % 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII & 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),	
	//ASCII ' 	     a
	//		   ----
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),	
	//ASCII ( 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ----
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII ) 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ----
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII * 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII + 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII , 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII - 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ####
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII . 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (1<<_DISPLAY_SEG_P),
	//ASCII / 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 0 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ----
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 1 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ----
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ----
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 2 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 3 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 4 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ----
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 5 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 6 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 7 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ----
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ----
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 8 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII 9	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII : 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),	
	//ASCII ; 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII < 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII = 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ####
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII > 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII ? 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ---- 
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII @ 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII A 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ----
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII B 	     a
	//		   ----
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII C 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ----
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII D 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII E 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII F 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ----
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII G 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII H 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ----
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII I 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ----
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ----
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII J 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ----
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII K 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),	
	//ASCII L 	     a
	//		   ----
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ----
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII M 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ----
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ----
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII N 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII O 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ----
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII P 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ----
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII Q 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ----
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII R 	     a
	//		   ####
	//		  #    #
	//		f #    #b
	//		  #  g #
	//		   ----
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ----
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII S 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII T 	     a
	//		   ----
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),	
	//ASCII U 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ----
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII V 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII W 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ####
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII X 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ----
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII Y	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII Z 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ####
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII [ 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ----
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ####
	(1<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (1<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII \ 	     a
	//		   ----
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ####
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ----
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (1<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII ]	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ----
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ####
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (1<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII ^ 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(1<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (1<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII _ 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ####
	(0<<_DISPLAY_SEG_A) | (0<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (1<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	//ASCII ' 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ----
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----
	(0<<_DISPLAY_SEG_A) | (1<<_DISPLAY_SEG_B) | (0<<_DISPLAY_SEG_C) | (0<<_DISPLAY_SEG_D) | (0<<_DISPLAY_SEG_E) | (0<<_DISPLAY_SEG_F) | (0<<_DISPLAY_SEG_G) | (0<<_DISPLAY_SEG_P),
	
};

/*
 +--------------------------------------+
 |        LOCAL FUNCTIONS CONSTANTS 	|
 +--------------------------------------+
*/

/*
 +--------------------------------------+
 |        STRUCTS AND UNIONS		    |
 +--------------------------------------+
*/

/*
        +-------------------------------------------------------+
        |       DISLAY SPI					                    |
        +-------------------------------------------------------+
        | @PURPOSE      : Send via SPI characters to show on    |
        |                 Display and read keys and send back   |
		|				  via SPI too						    |
        | @OBSERVATIONS :                                       |
        +-------------------------------------------------------+
*/

/*
        +-----------------------------------------------+
        |       PRIVATE FUNCTIONS IMPLEMENTATION        |
        +-----------------------------------------------+
*/


/*
	+---------------------------+
	|	FORWARD DEFINITIONS		|
	+---------------------------+
*/

void    DisplayScan();

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize KEYs					                                |
//	| @NAME    :  KEYsInitialize	              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::KeysInitialize()
{
		//set keys input with pull up
		_FEED_TIMER_DISP_7_SEG_KEY_PORT_OUTPUT|=(1<<_FEED_TIMER_DISP_7_SEG_KEY_ENTER_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_KEY_UP_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_KEY_DOWN_BIT);
		_FEED_TIMER_DISP_7_SEG_KEY_PORT_DIR   &=~((1<<_FEED_TIMER_DISP_7_SEG_KEY_ENTER_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_KEY_UP_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_KEY_DOWN_BIT));
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  UnInitialize KEYs					                                |
//	| @NAME    :  KEYsUnInitialize	              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::KeysUnInitialize()
{
	//set keys input high z
	_FEED_TIMER_DISP_7_SEG_KEY_PORT_DIR   &=~((1<<_FEED_TIMER_DISP_7_SEG_KEY_ENTER_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_KEY_UP_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_KEY_DOWN_BIT));
	_FEED_TIMER_DISP_7_SEG_KEY_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_KEY_ENTER_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_KEY_UP_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_KEY_DOWN_BIT);
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize LED 7 Segments Display	                                |
//	| @NAME    :  DisplayInitialize              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::DisplaysInitialize()
{
	uint8_t sreg;
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//program Segments and digits to output off state
	_FEED_TIMER_DISP_7_SEG_SEG1_PORT_OUTPUT  |=(1<<_FEED_TIMER_DISP_7_SEG_SEG1_C_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_SEG1_P_BIT);
	_FEED_TIMER_DISP_7_SEG_SEG1_PORT_DIR	 |=(1<<_FEED_TIMER_DISP_7_SEG_SEG1_C_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_SEG1_P_BIT);
	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT  |=(1<<_FEED_TIMER_DISP_7_SEG_SEG2_F_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_A_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_B_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_G_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_D_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_E_BIT);
	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_DIR	 |=(1<<_FEED_TIMER_DISP_7_SEG_SEG2_F_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_A_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_B_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_G_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_D_BIT)|		\
											   (1<<_FEED_TIMER_DISP_7_SEG_SEG2_E_BIT);
	_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_OUTPUT|= (1<<_FEED_TIMER_DISP_7_SEG_DIGITS_1_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_2_BIT)|   \
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_3_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_4_BIT);
	_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_DIR   |=	(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_1_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_2_BIT)|   \
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_3_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_4_BIT);
    //Set interrupt timer routine
    Time.SetTimeInterruptCallback(DisplayScan);
    //enable timer interrupt
    Time.SetInterruptState(true);					 
	//restore SREG
	SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize 7 segments LED Display	                                |
//	| @NAME    :  DisplayUnInitialize              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::DisplaysUnInitialize()
{
	uint8_t sreg;
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//program all select displays and segments to input high z
	_FEED_TIMER_DISP_7_SEG_SEG1_PORT_DIR	 &=~((1<<_FEED_TIMER_DISP_7_SEG_SEG1_C_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_SEG1_P_BIT));
	_FEED_TIMER_DISP_7_SEG_SEG1_PORT_OUTPUT	 &=~((1<<_FEED_TIMER_DISP_7_SEG_SEG1_C_BIT)|(1<<_FEED_TIMER_DISP_7_SEG_SEG1_P_BIT));
	
	
	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_DIR	 &=~((1<<_FEED_TIMER_DISP_7_SEG_SEG2_F_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_A_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_B_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_G_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_D_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_E_BIT));
	
	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT&=~((1<<_FEED_TIMER_DISP_7_SEG_SEG2_F_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_A_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_B_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_G_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_D_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_E_BIT));


	_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_DIR   &=~((1<<_FEED_TIMER_DISP_7_SEG_DIGITS_1_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_2_BIT)|   \
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_3_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_4_BIT));

	_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_OUTPUT&=~((1<<_FEED_TIMER_DISP_7_SEG_DIGITS_1_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_2_BIT)|    \
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_3_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_4_BIT));
	//Set interrupt timer routine
	//restore SREG
	SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: call back routine for TIMER0 for scan display						|
//	| @NAME    :           															|
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+


void    DisplayScan()
{
	uint8_t tmp;
	
	//first turn all digits off
	_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_OUTPUT|= (1<<_FEED_TIMER_DISP_7_SEG_DIGITS_1_BIT)|	\
								 				(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_2_BIT)|   \
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_3_BIT)|	\
												(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_4_BIT);
	//turn all segments off
	_FEED_TIMER_DISP_7_SEG_SEG1_PORT_OUTPUT|=	(1<<_FEED_TIMER_DISP_7_SEG_SEG1_C_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG1_P_BIT);	
	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT|=	(1<<_FEED_TIMER_DISP_7_SEG_SEG2_F_BIT)|		\
		    									(1<<_FEED_TIMER_DISP_7_SEG_SEG2_A_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_B_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_G_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_D_BIT)|		\
												(1<<_FEED_TIMER_DISP_7_SEG_SEG2_E_BIT);
									
	//pump display index count
	_Display_7_Seg.BufferIndex++;
	if (_Display_7_Seg.BufferIndex>=_DISP_7_SEG_NUM_DIGITS)	_Display_7_Seg.BufferIndex=0;
	//put display buffer content on display
	tmp=_Display_7_Seg.Buffer[_Display_7_Seg.BufferIndex] & (~(1<<_DISPLAY_SEG_P));
	//check bits
	if ((tmp & (1<<_DISPLAY_SEG_A))!=0)	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_SEG2_A_BIT);
	if ((tmp & (1<<_DISPLAY_SEG_B))!=0)	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_SEG2_B_BIT);
	if ((tmp & (1<<_DISPLAY_SEG_C))!=0)	_FEED_TIMER_DISP_7_SEG_SEG1_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_SEG1_C_BIT);
	if ((tmp & (1<<_DISPLAY_SEG_D))!=0)	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_SEG2_D_BIT);
	if ((tmp & (1<<_DISPLAY_SEG_E))!=0)	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_SEG2_E_BIT);
	if ((tmp & (1<<_DISPLAY_SEG_F))!=0)	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_SEG2_F_BIT);
	if ((tmp & (1<<_DISPLAY_SEG_G))!=0)	_FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_SEG2_G_BIT);
    //turn point on or off
     if ((_Display_7_Seg.PtrBufferBits & (1<<_Display_7_Seg.BufferIndex))!=0)
	 {
		_FEED_TIMER_DISP_7_SEG_SEG1_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_SEG1_P_BIT);
	 }
	//enable digit
	switch (_Display_7_Seg.BufferIndex)
	{
		case 0:
			_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_4_BIT);
		break;
		case 1:
			_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_3_BIT);
		break;
		case 2:
			_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_2_BIT);
		break;
		case 3:
			_FEED_TIMER_DISP_7_SEG_DIGITS_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_DIGITS_1_BIT);
		break;
	}
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize Motor					                                |
//	| @NAME    :  MotorInitialize	              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::MotorInitialize()
{
	//PROGRAM PORT TO INPUT HI-Z TO DISABLE MOTOR
	_FEED_TIMER_DISP_7_SEG_MOTOR_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_MOTOR_BIT);
	_FEED_TIMER_DISP_7_SEG_MOTOR_PORT_DIR	&=~(1<<_FEED_TIMER_DISP_7_SEG_MOTOR_BIT);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  UnInitialize Motor				                                |
//	| @NAME    :  MotorUnInitialize	              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::MotorUnInitialize()
{
	//PORTS input high z (motor automatically disable)
	_FEED_TIMER_DISP_7_SEG_MOTOR_PORT_DIR	&=~(1<<_FEED_TIMER_DISP_7_SEG_MOTOR_BIT);
	_FEED_TIMER_DISP_7_SEG_MOTOR_PORT_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_SEG_MOTOR_BIT);
	
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize PWM of LED Matrix		                                |
//	| @NAME    :  InitializePWMs	              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::Timer2Initialize()
{
	uint8_t sreg;
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//disable timer interrupts
	TIMSK2&=~(1<<OCIE2A);
	//set asynchronous mode external xtal 32.768Hz
	ASSR|=(1<<AS2);
	//set OCR2A to 31 to generate 1Hz interrupt using 32.768Hz xtal
	OCR2A=31;
	//wait assert
	do {} while ((ASSR & (1<<OCR2AUB))==0);
	//clear timer 2
	TCNT2=0;
	//wait assert
	do {} while ((ASSR & (1<<TCN2UB))==0);
	//set mode CTC prescaler x2014
	uint8_t tmp;
	tmp=TCCR2A;
	tmp|=(1<<WGM21);
	tmp&=~(1<<WGM20);
	TCCR2A|=tmp;
	//wait assert
	do {} while ((ASSR & (1<<TCR2AUB))==0);
	tmp=TCCR2B;
	tmp&=(1<<WGM22);
	tmp|=(1<<CS22)|(1<<CS21)|(1<<CS20);
	TCCR2B=tmp;
	//wait assert
	do {} while ((ASSR & (1<<TCR2BUB))==0);
	//enable interrupt
	TIMSK2|=(1<<OCIE2A);
	//restore SREG
	SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  TIMER2 ISR ROUTINE FOR COMPARE A MATH                             |
//	| @NAME    :  ISR(TIMER2_COMPA_vect)           		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :  This ISR is called each second									|
//	+-------------------------------------------------------------------------------+


ISR(TIMER2_COMPA_vect)
{
	if (_Display_7_Seg.SecondCallBack!=nullptr) _Display_7_Seg.SecondCallBack();
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize 7 segments ports drive                                 |
//	| @NAME    :  Initialize                    		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+


void    class_Display_7_Seg::Initialize()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
	//Initialize KEYS Hardware
	KeysInitialize();
	//Initialize Display Hardware
	DisplaysInitialize();
	//Initialize Motor Hardware
	MotorInitialize();
	//Initialize Timer 2
	Timer2Initialize();
	//ADCInitialize
	ADCInitialize(32);
	//clear Display Buffer
	memset(_Display_7_Seg.Buffer,0,sizeof(_Display_7_Seg.Buffer));
    //restore SREG state
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Un Initialize 7 segments ports drive                              |
//	| @NAME    :  UnInitialize                      	                            |
//	| @INPUT   :  none                                                              |
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :  interrupts are disabled and restored				                |
//	+-------------------------------------------------------------------------------+
void    class_Display_7_Seg::UnInitialize()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //disable interrupts
    cli();
	//UnInitialize KEYS Hardware
	KeysUnInitialize();
	//Initialize Display Hardware
	DisplaysUnInitialize();
	//Initialize Motor Hardware
	MotorUnInitialize();
    //restore SREG state
    SREG=sreg;
}

/*
        +-----------------------------------------------+
        |       PUBLIC CONSTRUCTORS AND DESTRUCTORS     |
        +-----------------------------------------------+
*/

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Constructor                                                       |
//	| @NAME    :  General WIFI IO Initialization  		                            |
//	| @INPUT   :  none                                                              |
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :													                |
//	+-------------------------------------------------------------------------------+
class_Display_7_Seg::class_Display_7_Seg()
{
	//Initialize PID
    Initialize();
}
/*
        +-----------------------------------------------+
        |       PUBLIC FUNCTIONS IMPLEMENTATION         |
        +-----------------------------------------------+
*/

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Set Segments at digit N		                                    |
//	| @NAME    : DispSetSegments			  										|
//	| @INPUT   : segments bits, digit							                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::DispSetSegments(uint8_t segmentBits,uint8_t digit)
{
	//check if digit in range
	if (digit>=1 && digit<=_DISP_7_SEG_NUM_DIGITS)
	{
		//yes set segments
		_Display_7_Seg.Buffer[digit-1]=segmentBits;
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Set Point N                	                                    |
//	| @NAME    : DispSetPoint   			  										|
//	| @INPUT   : digit		                					                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void    class_Display_7_Seg::DispSetPoint(uint8_t digit,bool digitState)
{
	//check if digit in range
	if (digit>=1 && digit<=_DISP_7_SEG_NUM_DIGITS)
	{
    	//yes set segments
        digit=digit-1;
        switch (digitState)
        {
            case false:
                _Display_7_Seg.PtrBufferBits&=~(1<<digit);
        	break;
            case true:
                _Display_7_Seg.PtrBufferBits|=(1<<digit);
            break;
        }
    	
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Set ASCII Segments at digit N		                                |
//	| @NAME    : DispSetASCII				  										|
//	| @INPUT   : asciiCode, digit								                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::DispSetASCII(uint8_t asciiCode,uint8_t digit)
{
	//check if digit in range
	if (digit>=1 && digit<=_DISP_7_SEG_NUM_DIGITS)
	{
		//yes set segments
		if (asciiCode>=FS && asciiCode<('_'+2))
		{
			_Display_7_Seg.Buffer[digit-1]=pgm_read_byte(&_FEED_TIMER_DISP_7_SEG_ASCII_Table[asciiCode-FS]);	
		}
		
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Display Clear Screen				                                |
//	| @NAME    : Cls				  												|
//	| @INPUT   : ASCII code to clear							                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::Cls()
{
	Locate(1);
	PutSF(PSTR("    "));
	Locate(1);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Display Set cursor to home col=1	                                |
//	| @NAME    : Home				  												|
//	| @INPUT   :												                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::Home()
{
	Locate(1);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Locate Display Column				                                |
//	| @NAME    : Locate						  										|
//	| @INPUT   : col 1..6										                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::Locate(uint8_t dispCol)
{
	_Display_7_Seg.Col=dispCol;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Put character on display			                                |
//	| @NAME    : PutC						  										|
//	| @INPUT   : asciiCode										                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::PutC(char asciiCode)
{
		if (_Display_7_Seg.Col>=1 && _Display_7_Seg.Col<=_DISP_7_SEG_NUM_DIGITS)
		{
			DispSetASCII(asciiCode,_Display_7_Seg.Col);
		}
		_Display_7_Seg.Col++;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Put characters on display form sram                                |
//	| @NAME    : PutS						  										|
//	| @INPUT   : asciiString									                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::PutS(const char *asciiString)
{
	  unsigned char data;
	  //send until zero is found
	  for(;;)
	  {
		  //get char for sram
		  data=*asciiString;
		  //check if end
		  if (data==0) break;
		  //process according data
		  PutC(data);
		  //next char
		  asciiString++;
	  }
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Put characters on display from flash memory                        |
//	| @NAME    : PutSF						  										|
//	| @INPUT   : asciiStringFlash 								                    |
//	| @OUTPUT  :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::PutSF(const char *asciiStringFlash)
{
	unsigned char data;
	//send until zero is found
	for(;;)
	{
		//get char for sram
		data=pgm_read_byte(asciiStringFlash);
		//check if end
		if (data==0) break;
		//process according data
		PutC(data);
		//next char
		asciiStringFlash++;
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Read Key without wait			                                    |
//	| @NAME    : KeyReadFlash 				  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  : keyCode							                                |
//	| @REMARKS : keyCode must be  _FEED_TIMER_DISP_7_SEG_KEY_DOWN,  _FEED_TIMER_DISP_7_SEG_KEY_UP,		|
//	|							   _FEED_TIMER_DISP_7_SEG_KEY_ENTER or _FEED_TIMER_DISP_7_SEG_KEY_NONE	|
//	+-------------------------------------------------------------------------------+
KeyCode	class_Display_7_Seg::KeyReadFlash()
{
    uint8_t tmp;

    //assume no key read
    tmp=_FEED_TIMER_DISP_7_SEG_KEY_PORT_INPUT;
    //check LED key	
    if ((tmp & (1<<_FEED_TIMER_DISP_7_SEG_KEY_DOWN_BIT))==0) return KeyCode::DOWN;
    //check LED MODE key
    if ((tmp & (1<<_FEED_TIMER_DISP_7_SEG_KEY_UP_BIT))==0) return KeyCode::UP;
    //check DOOR key
    if ((tmp & (1<<_FEED_TIMER_DISP_7_SEG_KEY_ENTER_BIT))==0) return KeyCode::ENTER;
	//else none pressed
    return KeyCode::NONE;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Wait key release				                                    |
//	| @NAME    : KeyWaitRelease				  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  :									                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::KeyWaitRelease()
{
	  while (KeyReadFlash()!=KeyCode::NONE) Time.DelayMsI(20);			//check at 20ms
	  Time.DelayMsI(100);										        //debounce time
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Read Key and wait key press by timeout value                       |
//	| @NAME    : KeyRead					  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  : KeyCode							                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
KeyCode	class_Display_7_Seg::KeyRead()
{
	KeyCode tmp,last;
	uint8_t ct;
	static bool repeat,first;
	
	tmp=KeyReadFlash();
	if (tmp==KeyCode::NONE)
	{
		repeat=false;
		do{
			Time.DelayMsS(20);
			tmp=KeyReadFlash();
		} while (tmp==KeyCode::NONE);
		ct=0;
		do{
			last=tmp;
			Time.DelayMsS(20);
			ct++;
			if (ct>25)
			{
				repeat=true;
				first=true;
				switch (static_cast<uint8_t>(tmp))
				{
					case static_cast<uint8_t>(KeyCode::DOWN):
						return KeyCode::DOWN_OVER;
					case static_cast<uint8_t>(KeyCode::UP):
						return KeyCode::UP_OVER;
					case static_cast<uint8_t>(KeyCode::ENTER):
						return KeyCode::ENTER_OVER;
				}
			}
			tmp=KeyReadFlash();
		} while (tmp!=KeyCode::NONE);
		Time.DelayMsS(50);
		return last;
	}
	else
	{
		if (repeat==true)
		{
			if (first==true)
			{
				Time.DelayMsS(500);
				first=false;
			}
			else
			{
				Time.DelayMsS(150);
			}
			switch (static_cast<uint8_t>(tmp))
			{
				case static_cast<uint8_t>(KeyCode::DOWN):
					return KeyCode::DOWN_REPEAT;
				case static_cast<uint8_t>(KeyCode::UP):
					return KeyCode::UP_REPEAT;
				case static_cast<uint8_t>(KeyCode::ENTER):
					return KeyCode::ENTER_REPEAT;
			}
		}
	}
	Time.DelayMsS(50);
	return tmp;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Put Hardware to Sleep until next timer2 interrupt that occur each  |
//	|			 second																|
//	| @NAME    : PutToSLeep					  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  :									                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::PutToSleep()
{
		//disable interrupts
		cli();
		//set sleep to POWER SAVE to keep timer2 running
		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		//enable sleep
		sleep_enable();
		//enable global interrupt
		sei();
		//put CPU to sleep
		sleep_cpu();
		//disable sleep mode
		sleep_disable();

}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Disable IO pins putting all in hi z stage and input				|
//	| @NAME    : DisableIO					  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  :									                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::DisableIO()
{
		//save SREG
		uint8_t sreg=SREG;
		//disable interrupts
		cli();
		//uninitialized hardware ports
		//KeysUnInitialize();
		DisplaysUnInitialize();
		MotorUnInitialize();	
		//restore SREG state
		SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Enable IO pins putting all in setting state						|
//	| @NAME    : EnableIO					  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  :									                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::EnableIO()
{
		//save SREG
		uint8_t sreg=SREG;
		//disable interrupts
		cli();
		//restore hardware ports state
		//KeysInitialize();
		DisplaysInitialize();
		//MotorInitialize();
		
		//_FEED_TIMER_DISP_7_SEG_MOTOR_PORT_DIR	|=(1<<_FEED_TIMER_DISP_7_SEG_MOTOR_BIT);
		
		//restore SREG state
		SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Get Motor State true=ON false=OFF									|
//	| @NAME    : MotorGetState				  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  :									                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
bool  class_Display_7_Seg::MotorGetState()
{
	if ((_FEED_TIMER_DISP_7_SEG_MOTOR_PORT_DIR & (1<<_FEED_TIMER_DISP_7_SEG_MOTOR_BIT))==0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Set call back function to timer2 call each second					|
//	| @NAME    : PutToSLeep					  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  :									                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::SetSecondCallBack(void (*SecondCallBack)(void))
{
	//Set call back function 
	_Display_7_Seg.SecondCallBack=SecondCallBack;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize ADC					                                |
//	| @NAME    :  InitializeADC		              		                            |
//	| @INPUT   :  ADC prescaler  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS : prescaler must be 2,4,8,16,32,64 or 128							|
//	+-------------------------------------------------------------------------------+
void	class_Display_7_Seg::ADCInitialize(uint8_t prescalerFactor)
{
	uint8_t sreg,prescalerLog;
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//set ADC input with high z
	_FEED_TIMER_DISP_7_ADC_OUTPUT&=~(1<<_FEED_TIMER_DISP_7_ADC_BIT);
	_FEED_TIMER_DISP_7_ADC_DIR   &=~(1<<_FEED_TIMER_DISP_7_ADC_BIT);
	//set reference to internal 1.1volt
	ADMUX|=(1<<REFS1)|(1<<REFS0);
	//no left adjust
	ADMUX&=~(1<<ADLAR);
	//select channel 5
	ADMUX&=~((1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));
	ADMUX|= ((0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(1<<MUX0));
	//first compute log2(prescalerFactor)
	prescalerLog=7;
	if (prescalerFactor==0) prescalerFactor=1;
	for(;;)
	{
		//exit if log computed checking bit 7
		if ((prescalerFactor & (1<<7))!=0) break;
		//shift value
		prescalerFactor<<=1;
		//compute log
		prescalerLog--;
	}
	//Set prescaler
	if ((prescalerLog & (1<<0))==0)	ADCSRA&=~(1<<ADPS0); else ADCSRA|=(1<<ADPS0);
	if ((prescalerLog & (1<<1))==0)	ADCSRA&=~(1<<ADPS1); else ADCSRA|=(1<<ADPS1);
	if ((prescalerLog & (1<<2))==0)	ADCSRA&=~(1<<ADPS2); else ADCSRA|=(1<<ADPS2);
	//start first conversion
	ADCSRA|=(1<<ADEN)|(1<<ADSC);
	do {
		
	} while ((ADCSRA & (1<<ADSC))!=0);
	//restore SREG
	SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get ADC value						                                |
//	| @NAME    :  ADCGetValue		              		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  Value								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
uint16_t class_Display_7_Seg::ADCGetValue()
{
	uint8_t sreg;
	uint16_t adcValue;
	
	//save SREG
	sreg=SREG;
	//start conversion
	ADCSRA|=(1<<ADSC);
	//wait finish
	do{
		
	} while ((ADCSRA & (1<<ADSC))!=0);
	//disable interrupts
	cli();
	//get value
	adcValue=ADC;
	//restore interrupts
	SREG=sreg;
	//return value
	return adcValue;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Range, ajdust input value in range min,max to output range min,max|
//	| @NAME    :  Range				              		                            |
//	| @INPUT   :  valueIn,rangeInMin,rangeInMax,rangeOutMin,rangeOutMax				|
//	| @OUTPUT  :  ValueOut							                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+

int32_t	class_Display_7_Seg::Range(int32_t valueIn,int32_t rangeInMin,int32_t rangeInMax,int32_t rangeOutMin,int32_t rangeOutMax)
{
	return ((valueIn-rangeInMin)*(rangeOutMax-rangeOutMin))/(rangeInMax-rangeInMin)+rangeOutMin;
}
