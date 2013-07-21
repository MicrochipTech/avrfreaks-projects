
//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************
 
//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

	void pattern_tick(void);		//call every timer tick

    void pattern_break(void);       //call to abort current pattern
   
	//set duration for called patterns in timer ticks, 0 = indefinite
	void pattern_duration_set(unsigned int ticks);

	//Patterns
	void pattern_diamond_scroll(void);
	void pattern_popcorn(void);
	void pattern_waveup(int ringsize, int framestep, int distance, int framerate);
	void pattern_rotate1(void);
	void pattern_rain1(void);
	void pattern_fireworks(void);
	void pattern_screw(void);
	void pattern_cube_stretch(void);
	void pattern_noise_scroll(void);
	void pattern_fountain(void);
	void pattern_spinface(void);
	void pattern_flapsheet(void);
	void pattern_text1_P(PGM_P);

	//Night patterns (static shapes slow fade in/out )
	void pattern_diamond(void);
	void pattern_cubewalls(void);
	void pattern_tesseract(void);
	void pattern_wirecube(void);
	void pattern_wirecubediag(void);

