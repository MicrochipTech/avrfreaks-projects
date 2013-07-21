
//********************************************************************************************************
// Public defines
//********************************************************************************************************

	enum {SEQUENCE_MODE_DAY, SEQUENCE_MODE_NIGHT};
    
	//(more than 10 sequences will require mods to the sequence menu which currently only displays 1 digit)
	#define	NUMBER_OF_SEQUENCES	4

        
//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

	void sequence_tick(void);
    void sequence_init(void);
    
     //display sequence
    void sequence_run(void);
   
	//stop displaying current sequence and exit
	void sequence_exit(void);

	// save brightness setting for current sequence
	void sequence_save_brightness(void);
	
    //called when buttons are pressed
    void sequence_skip(void);   
    void sequence_select_press(void);
    void sequence_rotate_en(void);
    
    //sequence selection menu
    void sequence_menu(void);
    
