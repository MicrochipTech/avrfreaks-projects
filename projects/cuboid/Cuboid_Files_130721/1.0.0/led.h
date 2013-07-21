/*

	Interface to LED module

	All coordinates are 0-7

	x 0-7 = left to right
	y 0-7 = top to bottom
	z 0-7 = front to back

	A plane is referred to by the dimension in which it's size is 1.
	
		Y plane has the same orientation as a tabletop.
		X plane is a verticle wall running front to back, with the faces on the left and right.
		Z plane is a verticle wall running left to right, with the faces front and back.
	
	
*/

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

	extern unsigned char 		led_frame_bitmap[];	// 8x8x8 bitmap

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

	//init led controller
	void led_init(void);

	//turn off all leds and stop driving leds
	void led_shutdown(void);

	//change framerate (16-200)
	void led_framerate(int rate);

	//set intensity for a plane (0-255)
	void led_plane_intensity(unsigned char plane, unsigned char intensity);

	//set intensity for all planes
	void led_plane_intensity_all(unsigned char intensity);

	//turn on/off a single led
	void led_point(unsigned char x, unsigned char y, unsigned char z, unsigned char on_off);

	//turn off all leds
	void led_blank(void);

	//wait for N frames, (1) waits for next frame, (0) returns immediatly
	void led_waitframe(unsigned int x);

	//draw a line
	void led_line(int x1, int y1, int z1, int x2, int y2, int z2, unsigned char on_off);
	
	//flip display upside down (true/false), handy if you want to hang your cube. 
	void led_flip(char x);
	
//****************************************************
// Plane scrolling
//****************************************************

	//scroll a verticle plane 1 pixel backwards, new front columb is blank
	void led_plane_x_back(unsigned char x);

	//scroll a verticle plane 1 pixel forwards, new back columb is blank
	void led_plane_x_forward(unsigned char x);

	//scroll a verticle plane 1 pixel up, new bottom row is blank
	void led_plane_x_up(unsigned char x);

	//scroll a verticle plane 1 pixel down, new top row is blank
	void led_plane_x_down(unsigned char x);

	//scroll a verticle plane running in the x direction down
	void led_plane_z_down(unsigned char z);

	//scroll a verticle plane running in the x direction up
	void led_plane_z_up(unsigned char z);

	//scroll a verticle plane to the left, new columb at the right is blank
	void led_plane_z_left(unsigned char z);

	//scroll a verticle plane to the right, new columb at the left is blank
	void led_plane_z_right(unsigned char z);

	//scroll a horizontal plane back
	void led_plane_y_back(unsigned char y);

	//scroll a horizontal plane forwards
	void led_plane_y_forward(unsigned char y);

	//scroll a horizontal plane left
	void led_plane_y_left(unsigned char y);

	//scroll a horizontal plane right
	void led_plane_y_right(unsigned char y);

//****************************************************
// Perimeter rotation, with reference to front face
//****************************************************

	//rotate perimiter front left
	void led_perimeter_left(void);

	//rotate perimiter front right
	void led_perimeter_right(void);

	//rotate perimiter front up
	void led_perimeter_up(void);

	//rotate perimiter front down
	void led_perimeter_down(void);

	//rotate perimiter clockwise (top right)
	void led_perimeter_clockwise(void);

	//rotate perimiter clockwise (top left)
	void led_perimeter_anticlockwise(void);

//****************************************************
//Entire cube scrolling
//****************************************************

	//scroll entire frame down 1 pixel
	void led_frame_down(void);

	//scroll entire frame up 1 pixel
	void led_frame_up(void);

	//scroll entire frame left 1 pixel
	void led_frame_left(void);

	//scroll entire frame right 1 pixel
	void led_frame_right(void);

	//scroll entire frame forward 1 pixel
	void led_frame_forward(void);

	//scroll entire frame back 1 pixel
	void led_frame_back(void);
