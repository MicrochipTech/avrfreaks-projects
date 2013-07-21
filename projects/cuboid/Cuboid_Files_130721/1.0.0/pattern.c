/*

5,6,10 shitty
*/

#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************
 
	#define DEFAULT_TIMEOUT	30

//**********
//	pattern diamond_scroll config
	#define DIAMOND_SCROLL_FRAMERATE	100
	#define DIAMOND_SCROLL_FRAMESTEP	6

//**********
//	pattern_popcorn config
	#define POPCORN_FRAMERATE	60
	#define POPCORN_NUMBEROF	20
	
//**********
//	Firework config, note if gravity is too low, rocket will exit top of cube and not explode
	#define FIREWORK_SPRAYPOINTS	30
	#define FIREWORK_FRAMERATE		50
	#define FIREWORK_GRAVITY		50
	#define FIREWORK_NUMBEROF		2

	//initial speeds of spray particles, rand must be masked with N^2-1 (1023, 2047, 4095 ect..)
	#define FIREWORK_BURST_XZ		-512+(rand()&1023)
	#define FIREWORK_BURST_Y		-1000+(rand()&1023)

	//initial speed of rocket, rocket bursts when it starts falling
	//providing it starts to fall before it exits the cube, if you lower gravity, lower this
	#define FIREWORK_LAUNCH_Y		-1300-(rand()&255)

//**********
//	Screw config
	#define SCREW_FRAMERATE			100
	#define SCREW_FRAMESTEP			5
	#define SCREW_ROTATIONS			5

//**********
//	Cube stretch config
	#define CUBE_STRETCH_FRAMERATE	50
	#define CUBE_STRETCH_FRAMESTEP	3
	
//**********
//	Fountain config
	#define FOUNTAIN_NUMBEROF		8
	#define FOUNTAIN_GRAVITY		632
	#define FOUNTAIN_XZ				1023+(rand()&511) 
	#define FOUNTAIN_Y				(3500+(rand()&2047))
	#define FOUNTAIN_FRAMESTEP		1
	#define FOUNTAIN_FRAMERATE		50
	//number of frames/corner
	#define FOUNTAIN_MOVE			250

//**********
//	Noise scroll config
	#define NOISE_SCROLL_FRAMERATE	100
	#define NOISE_SCROLL_FRAMESTEP	4
	
//**********
//	Spinface config
	#define SPINFACE_FRAMERATE		100
	#define SPINFACE_FRAMESTEP		5

//**********
//	Flapping sheet config
	#define FLAPSHEET_FRAMERATE		50
	#define FLAPSHEET_FRAMESTEP		1
	#define FLAPSHEET_SPEED			5

//**********
//	Text1 config
	#define TEXT1_FRAMERATE			100
	#define TEXT1_FRAMESTEP			6


//********************************************************************************************************
// Local defines
//********************************************************************************************************

	enum
	{
		FIREWORK_STATE_LAUNCH,
		FIREWORK_STATE_SPRAY,
		FIREWORK_STATE_DONE
	};


	//data structure for physics of a single point
	struct physical_struct
	{
		unsigned int position_x;	//0-32767, 4096/led, coordinates are bits 14:13:12
		unsigned int position_y;	//point becomes inactive/off screen when MSB is set
		unsigned int position_z;
		
		int speed_x;	//speed, 4096 = 1 led/frame
		int speed_y;
		int speed_z;
		
		int accel_x;	//acceleration
		int accel_y;	//for a 1" grid, actual gravity works out to 1580346/framerate^2 
		int accel_z;	//of 632 if framerate is 50
		
		char active;	//1= active, point is moving
						//0= inactive, point has left cube
	};
	
	struct firework_struct
	{
		uint8_t state;
		struct physical_struct rocket;
		struct physical_struct spray[FIREWORK_SPRAYPOINTS];
	};	

	enum
	{
		ROTATE_NONE,
		ROTATE_UP,
		ROTATE_DOWN,
		ROTATE_LEFT,
		ROTATE_RIGHT,
		ROTATE_CLOCKWISE,
		ROTATE_ANTICLOCKWISE
	};
	
	enum
	{
		DIRECTION_NONE,
		DIRECTION_UP,
		DIRECTION_DOWN,
		DIRECTION_LEFT,
		DIRECTION_RIGHT,
		DIRECTION_FORWARD,
		DIRECTION_BACKWARD
	};


//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static unsigned int	time;
	static unsigned int	duration=0;
	static char		fallout = FALSE;	//set by pattern_break(), aborts pattern to return to menu

	//sin table 256 values range 0x02 - 0x7C
	static const uint8_t sintable[256] PROGMEM = {
	0x3F, 0x40, 0x42, 0x43, 0x45, 0x46, 0x48, 0x49, 0x4B, 0x4C, 0x4E, 0x4F, 0x51, 0x52, 0x54, 0x55
	, 0x56, 0x58, 0x59, 0x5A, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x62, 0x63, 0x65, 0x66, 0x67, 0x68, 0x69
	, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x73, 0x74, 0x75, 0x75, 0x76, 0x77
	, 0x77, 0x78, 0x78, 0x79, 0x79, 0x7A, 0x7A, 0x7B, 0x7B, 0x7B, 0x7B, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C
	, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7B, 0x7B, 0x7B, 0x7B, 0x7A, 0x7A, 0x79, 0x79, 0x78, 0x78
	, 0x77, 0x77, 0x76, 0x75, 0x75, 0x74, 0x73, 0x73, 0x72, 0x71, 0x70, 0x6F, 0x6E, 0x6D, 0x6C, 0x6B
	, 0x6A, 0x69, 0x68, 0x67, 0x66, 0x65, 0x63, 0x62, 0x61, 0x60, 0x5E, 0x5D, 0x5C, 0x5A, 0x59, 0x58
	, 0x56, 0x55, 0x54, 0x52, 0x51, 0x4F, 0x4E, 0x4C, 0x4B, 0x49, 0x48, 0x46, 0x45, 0x43, 0x42, 0x40
	, 0x3F, 0x3E, 0x3C, 0x3B, 0x39, 0x38, 0x36, 0x35, 0x33, 0x32, 0x30, 0x2F, 0x2D, 0x2C, 0x2A, 0x29
	, 0x28, 0x26, 0x25, 0x24, 0x22, 0x21, 0x20, 0x1E, 0x1D, 0x1C, 0x1B, 0x19, 0x18, 0x17, 0x16, 0x15
	, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0B, 0x0A, 0x09, 0x09, 0x08, 0x07
	, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02
	, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06
	, 0x07, 0x07, 0x08, 0x09, 0x09, 0x0A, 0x0B, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13
	, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1B, 0x1C, 0x1D, 0x1E, 0x20, 0x21, 0x22, 0x24, 0x25, 0x26
	, 0x28, 0x29, 0x2A, 0x2C, 0x2D, 0x2F, 0x30, 0x32, 0x33, 0x35, 0x36, 0x38, 0x39, 0x3B, 0x3C, 0x3E };

	//frame bitmap for a diamond shape (sort of)
	static const char progframe_diamond[64] PROGMEM =\
	{\
	 0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00\
	,0x00,0x00,0x18,0x24,0x24,0x18,0x00,0x00\
	,0x00,0x18,0x24,0x42,0x42,0x24,0x18,0x00\
	,0x18,0x24,0x42,0x81,0x81,0x42,0x24,0x18\
	,0x18,0x24,0x42,0x81,0x81,0x42,0x24,0x18\
	,0x00,0x18,0x24,0x42,0x42,0x24,0x18,0x00\
	,0x00,0x00,0x18,0x24,0x24,0x18,0x00,0x00\
	,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00\
	};

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************


	//rotate a point around the surface of the cube see DIR enum
	static void point_rotate(uint8_t dir, uint8_t* x_ptr, uint8_t* y_ptr, uint8_t* z_ptr);

	//process physics of a point
	static void physical_process(struct physical_struct *physical_ptr);
	
	//plot or erase a physical point
	static void physical_plot(struct physical_struct *physical_ptr, char on_off);


	//init firework structure
	static void firework_init(struct firework_struct *firework_ptr);

	//draw a cube between 2 points
	static void cube_draw(uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, char on_off);

	//spin an image on the y axis
	static void spin_y(uint8_t step);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

void pattern_tick(void)
{
	if(time)
		time--;
}

void pattern_break(void)
{
	fallout=TRUE;
}

void pattern_duration_set(unsigned int x)
{
	duration = x;
}

/*
Starting point for a pattern, copy this out and replace 'template' with the name of the pattern
void pattern_template(void)
{
	uint8_t index;

	fallout = FALSE;
	time = duration;

	led_framerate(TEMPLATE_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	//running process
	while(!fallout && (!duration || time))
	{
		//process running pattern
		led_waitframe(TEMPLATE_FRAMESTEP);
	};

	//end pattern
	index=8;
	while(!fallout && index--)
	{
		led_frame_down();
		led_waitframe(TEMPLATE_FRAMESTEP);
	};
}
*/

void pattern_diamond_scroll(void)
{
	uint8_t index;

	fallout = FALSE;
	time = duration;

	led_framerate(DIAMOND_SCROLL_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	index=0;
	//running process
	while(!fallout && (!duration || time))
	{
		led_frame_up();
		memcpy_P(&led_frame_bitmap[56], &progframe_diamond[index], 8);
		index+=8;
		index &= 63;
		
		led_waitframe(DIAMOND_SCROLL_FRAMESTEP);
	};

	//end pattern
	index=8;
	while(!fallout && index--)
	{
		led_frame_up();
		led_waitframe(DIAMOND_SCROLL_FRAMESTEP);
	};
}

void pattern_diamond(void)
{
	int intensity[8];
	int index=0;

	fallout=FALSE;
	time = duration;

	//frame rate
	led_framerate(100);

	//set intensitys to 0
	led_plane_intensity_all(0);

	//draw
	led_blank();

	memcpy_P(led_frame_bitmap, progframe_diamond, 64);

	//fade in
	intensity[7]=0;
	intensity[6]=-31;
	intensity[5]=-63;
	intensity[4]=-95;
	intensity[3]=-127;
	intensity[2]=-159;
	intensity[1]=-191;
	intensity[0]=-223;
	while(intensity[0] != 255 && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]++;
			index++;
		};
		led_waitframe(1);
	};

	//running process
	while(!fallout && (!duration || time))
		led_waitframe(1);

	//exit process (fade out)
	intensity[7]=256;
	intensity[6]=288;
	intensity[5]=320;
	intensity[4]=352;
	intensity[3]=384;
	intensity[2]=416;
	intensity[1]=448;
	intensity[0]=480;
	while(0 < intensity[0] && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]--;
			index++;
		};
		led_waitframe(1);
	};
}

void pattern_cubewalls(void)
{
	int intensity[8];
	int index=0;

	fallout=FALSE;
	time = duration;


	//frame rate
	led_framerate(100);
	
	//set intensitys to 0
	led_plane_intensity_all(0);
	
	//draw
	led_blank();

	//left & right walls
	memset(led_frame_bitmap, 0x81, 64);	
	
	//front & back walls
	led_frame_bitmap[0] =0xFF;
	led_frame_bitmap[7] =0xFF;
	led_frame_bitmap[8] =0xFF; 
	led_frame_bitmap[15]=0xFF;
	led_frame_bitmap[16]=0xFF; 
	led_frame_bitmap[23]=0xFF;
	led_frame_bitmap[24]=0xFF; 
	led_frame_bitmap[31]=0xFF;
	led_frame_bitmap[32]=0xFF; 
	led_frame_bitmap[39]=0xFF;
	led_frame_bitmap[40]=0xFF; 
	led_frame_bitmap[47]=0xFF;
	led_frame_bitmap[48]=0xFF; 
	led_frame_bitmap[55]=0xFF;
	led_frame_bitmap[56]=0xFF; 
	led_frame_bitmap[63]=0xFF;

	//top & bottom walls
	led_frame_bitmap[0] =0xFF;
	led_frame_bitmap[1] =0xFF;
	led_frame_bitmap[2] =0xFF;
	led_frame_bitmap[3] =0xFF;
	led_frame_bitmap[4] =0xFF;
	led_frame_bitmap[5] =0xFF;
	led_frame_bitmap[6] =0xFF;
	led_frame_bitmap[7] =0xFF;
	led_frame_bitmap[56] =0xFF;
	led_frame_bitmap[57] =0xFF;
	led_frame_bitmap[58] =0xFF;
	led_frame_bitmap[59] =0xFF;
	led_frame_bitmap[60] =0xFF;
	led_frame_bitmap[61] =0xFF;
	led_frame_bitmap[62] =0xFF;
	led_frame_bitmap[63] =0xFF;
			
	//fade in
	intensity[7]=0;
	intensity[6]=-31;
	intensity[5]=-63;
	intensity[4]=-95;
	intensity[3]=-127;
	intensity[2]=-159;
	intensity[1]=-191;
	intensity[0]=-223;
	while(intensity[0] != 255 && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]++;
			index++;
		};
		led_waitframe(1);
	};

	//running process
	while(!fallout && (!duration || time))
		led_waitframe(1);

	//exit process (fade out)
	intensity[7]=256;
	intensity[6]=288;
	intensity[5]=320;
	intensity[4]=352;
	intensity[3]=384;
	intensity[2]=416;
	intensity[1]=448;
	intensity[0]=480;
	while(0 < intensity[0] && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]--;
			index++;
		};
		led_waitframe(1);
	};
}


void pattern_tesseract(void)
{
	int intensity[8];
	int index=0;

	fallout=FALSE;
	time = duration;


	//frame rate
	led_framerate(100);
	
	//set intensitys to 0
	led_plane_intensity_all(0);
	
	//draw wireframe
	led_blank();
	
	led_line(0,0,0,7,0,0,TRUE);
	led_line(0,0,7,7,0,7,TRUE);
	led_line(0,7,0,7,7,0,TRUE);
	led_line(0,7,7,7,7,7,TRUE);

	led_line(0,0,0,0,7,0,TRUE);
	led_line(7,0,0,7,7,0,TRUE);	
	led_line(0,0,7,0,7,7,TRUE);
	led_line(7,0,7,7,7,7,TRUE);	

	led_line(0,0,0,0,0,7,TRUE);
	led_line(7,0,0,7,0,7,TRUE);
	led_line(0,7,0,0,7,7,TRUE);
	led_line(7,7,0,7,7,7,TRUE);

	led_line(2,2,2,5,2,2,TRUE);
	led_line(2,2,5,5,2,5,TRUE);
	led_line(2,5,2,5,5,2,TRUE);
	led_line(2,5,5,5,5,5,TRUE);

	led_line(2,2,2,2,5,2,TRUE);
	led_line(5,2,2,5,5,2,TRUE);	
	led_line(2,2,5,2,5,5,TRUE);
	led_line(5,2,5,5,5,5,TRUE);	

	led_line(2,2,2,2,2,5,TRUE);
	led_line(5,2,2,5,2,5,TRUE);
	led_line(2,5,2,2,5,5,TRUE);
	led_line(5,5,2,5,5,5,TRUE);

	led_point(1,1,1,TRUE);
	led_point(6,1,1,TRUE);
	led_point(1,1,6,TRUE);
	led_point(6,1,6,TRUE);
	led_point(1,6,1,TRUE);
	led_point(6,6,1,TRUE);
	led_point(1,6,6,TRUE);
	led_point(6,6,6,TRUE);
			
	//fade in
	intensity[7]=0;
	intensity[6]=-31;
	intensity[5]=-63;
	intensity[4]=-95;
	intensity[3]=-127;
	intensity[2]=-159;
	intensity[1]=-191;
	intensity[0]=-223;
	while(intensity[0] != 255 && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]++;
			index++;
		};
		led_waitframe(1);
	};

	//running process
	while(!fallout && (!duration || time))
		led_waitframe(1);

	//exit process (fade out)
	intensity[7]=256;
	intensity[6]=288;
	intensity[5]=320;
	intensity[4]=352;
	intensity[3]=384;
	intensity[2]=416;
	intensity[1]=448;
	intensity[0]=480;
	while(0 < intensity[0] && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]--;
			index++;
		};
		led_waitframe(1);
	};
}


void pattern_wirecube(void)
{
	int intensity[8];
	int index=0;

	fallout=FALSE;
	time = duration;


	//frame rate
	led_framerate(100);
	
	//set intensitys to 0
	led_plane_intensity_all(0);
	
	//draw wireframe
	led_blank();
	
	led_line(0,0,0,7,0,0,TRUE);
	led_line(0,0,7,7,0,7,TRUE);
	led_line(0,7,0,7,7,0,TRUE);
	led_line(0,7,7,7,7,7,TRUE);

	led_line(0,0,0,0,7,0,TRUE);
	led_line(7,0,0,7,7,0,TRUE);	
	led_line(0,0,7,0,7,7,TRUE);
	led_line(7,0,7,7,7,7,TRUE);	

	led_line(0,0,0,0,0,7,TRUE);
	led_line(7,0,0,7,0,7,TRUE);
	led_line(0,7,0,0,7,7,TRUE);
	led_line(7,7,0,7,7,7,TRUE);


	//fade in
	intensity[7]=0;
	intensity[6]=-31;
	intensity[5]=-63;
	intensity[4]=-95;
	intensity[3]=-127;
	intensity[2]=-159;
	intensity[1]=-191;
	intensity[0]=-223;
	while(intensity[0] != 255 && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]++;
			index++;
		};
		led_waitframe(1);
	};

	//running process
	while(!fallout && (!duration || time))
		led_waitframe(1);

	//exit process (fade out)
	intensity[7]=256;
	intensity[6]=288;
	intensity[5]=320;
	intensity[4]=352;
	intensity[3]=384;
	intensity[2]=416;
	intensity[1]=448;
	intensity[0]=480;
	while(0 < intensity[0] && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]--;
			index++;
		};
		led_waitframe(1);
	};
}

void pattern_wirecubediag(void)
{
	int intensity[8];
	int index=0;

	fallout=FALSE;
	time = duration;


	//frame rate
	led_framerate(100);
	
	//set intensitys to 0
	led_plane_intensity_all(0);
	
	//draw wireframe
	led_blank();
	
	led_line(0,0,0,7,0,0,TRUE);
	led_line(0,0,7,7,0,7,TRUE);
	led_line(0,7,0,7,7,0,TRUE);
	led_line(0,7,7,7,7,7,TRUE);

	led_line(0,0,0,0,7,0,TRUE);
	led_line(7,0,0,7,7,0,TRUE);	
	led_line(0,0,7,0,7,7,TRUE);
	led_line(7,0,7,7,7,7,TRUE);	

	led_line(0,0,0,0,0,7,TRUE);
	led_line(7,0,0,7,0,7,TRUE);
	led_line(0,7,0,0,7,7,TRUE);
	led_line(7,7,0,7,7,7,TRUE);

	led_line(0,0,0,7,7,7,TRUE);
	led_line(7,0,0,0,7,7,TRUE);
	led_line(0,0,7,7,7,0,TRUE);
	led_line(7,0,7,0,7,0,TRUE);


	//fade in
	intensity[7]=0;
	intensity[6]=-31;
	intensity[5]=-63;
	intensity[4]=-95;
	intensity[3]=-127;
	intensity[2]=-159;
	intensity[1]=-191;
	intensity[0]=-223;
	while(intensity[0] != 255 && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]++;
			index++;
		};
		led_waitframe(1);
	};

	//running process
	while(!fallout && (!duration || time))
		led_waitframe(1);

	//exit process (fade out)
	intensity[7]=256;
	intensity[6]=288;
	intensity[5]=320;
	intensity[4]=352;
	intensity[3]=384;
	intensity[2]=416;
	intensity[1]=448;
	intensity[0]=480;
	while(0 < intensity[0] && !fallout)
	{
		index=0;
		while(index!=8)
		{
			led_plane_intensity(index, (unsigned char)clip_int(intensity[index],0,255));
			intensity[index]--;
			index++;
		};
		led_waitframe(1);
	};
}


void pattern_popcorn(void)
{
	struct column_struct
	{
		unsigned char x;
		unsigned char z;
	};

	unsigned int index, index2;

	struct physical_struct point[POPCORN_NUMBEROF];

	struct column_struct active_list[POPCORN_NUMBEROF];
	struct column_struct inactive_list[64-POPCORN_NUMBEROF];

	unsigned char x=0,z=0;

	unsigned char deadmark;
	
	char alive;
	
	fallout = FALSE;
	time = duration;

	led_framerate(POPCORN_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	//init active list
	index=0;
	while(index != POPCORN_NUMBEROF)
	{
		active_list[index].x=x;
		active_list[index].z=z;
		
		point[index].position_x= 0;
		point[index].position_y= 0;
		point[index].position_z= 0;
		point[index].speed_x=0;
		point[index].speed_y=0;
		point[index].speed_z=0;
		point[index].accel_x=0;
		point[index].accel_y=0;
		point[index].accel_z=0;
		point[index].active=FALSE;
		
		led_point(x,7,z,TRUE);
		
		x++;
		if(x==8)
		{
			x=0;
			z++;
		};
		index++;
	};

	//init inactive list
	while(index!=64)
	{
		inactive_list[index-POPCORN_NUMBEROF].x=x;
		inactive_list[index-POPCORN_NUMBEROF].z=z;
		
		led_point(x,7,z,TRUE);
		
		x++;
		if(x==8)
		{
			x=0;
			z++;
		};
		index++;
	};

	//running process
	while(!fallout && (!duration || time))
	{
		//process running pattern
		led_waitframe(1);
		
		index=0;
		while(index != POPCORN_NUMBEROF)
		{
			//erase current position
			physical_plot(&point[index], FALSE);
			
			//move to next position
			physical_process(&point[index]);
			
			//if point is finished
			if(point[index].active==FALSE)
			{
				//turn on floor point
				x=active_list[index].x;
				z=active_list[index].z;
				led_point(x, 7, z, TRUE);
				
				//swap with another column from the inactive list
				index2 = rand()%(64-POPCORN_NUMBEROF);
				active_list[index].x=inactive_list[index2].x;
				active_list[index].z=inactive_list[index2].z;
				inactive_list[index2].x=x;
				inactive_list[index2].z=z;
				
				//setup new column
				point[index].position_x= (((unsigned int)(active_list[index].x))<<12) + 0x800;
				point[index].position_y= 0x7800;
				point[index].position_z= (((unsigned int)(active_list[index].z))<<12) + 0x800;;
				point[index].speed_x=0;
				point[index].speed_y=-(rand()&4095);
				point[index].speed_z=0;
				point[index].accel_x=0;
				point[index].accel_y=297;
				point[index].accel_z=0;
				point[index].active=TRUE;
				
			}
			else
			//else plot new point position
				physical_plot(&point[index], TRUE);	
			index++;
		};
	};

	//exit process, continue, only remove gravity untill all columns are gone
	//swap is no longer done with a random column from the inactive list
	//swap is done with inactive[deadmark++] until all columns are dead
	deadmark=0;
	alive=TRUE;
	while(alive && !fallout)
	{
		//process running pattern
		led_waitframe(1);
		
		index=0;
		alive=FALSE;
		while(index != POPCORN_NUMBEROF)
		{
			//erase current position
			physical_plot(&point[index], FALSE);
			
			//move to next position
			physical_process(&point[index]);
			
			//if point is finished
			if(point[index].active==FALSE)
			{
				//if this point does not have gravity, swap column onto the deadlist (confused yet?)
				if(point[index].accel_y==0)
				{
					//inactive but live column available?
					if(deadmark < 64-POPCORN_NUMBEROF)
					{
						alive=TRUE;
						x=active_list[index].x;
						z=active_list[index].z;
						
						//swap with another column from the inactive list
						index2 = deadmark++;
						active_list[index].x=inactive_list[index2].x;
						active_list[index].z=inactive_list[index2].z;
						inactive_list[index2].x=x;
						inactive_list[index2].z=z;
						
						//setup new column, but without gravity
						point[index].position_x= (((unsigned int)(active_list[index].x))<<12) + 0x800;
						point[index].position_y= 0x7800;
						point[index].position_z= (((unsigned int)(active_list[index].z))<<12) + 0x800;;
						point[index].speed_x=0;
						point[index].speed_y=0-400-(rand()&4095);
						point[index].speed_z=0;
						point[index].accel_x=0;
						point[index].accel_y=0;
						point[index].accel_z=0;
						point[index].active=TRUE;
					};
				}
				//if inactive column did have gravity, swap with with an alive but inactive random column
				else
				{
					//inactive but live column available?
					if(deadmark < 64-POPCORN_NUMBEROF)
					{
						alive=TRUE;
						//turn on floor point
						x=active_list[index].x;
						z=active_list[index].z;
						led_point(x, 7, z, TRUE);
						
						//swap with another column from the inactive list after the deadmark
						index2 = deadmark + rand()%(64-POPCORN_NUMBEROF-deadmark);
						active_list[index].x=inactive_list[index2].x;
						active_list[index].z=inactive_list[index2].z;
						inactive_list[index2].x=x;
						inactive_list[index2].z=z;
						
						//setup new column, but without gravity
						point[index].position_x= (((unsigned int)(active_list[index].x))<<12) + 0x800;
						point[index].position_y= 0x7800;
						point[index].position_z= (((unsigned int)(active_list[index].z))<<12) + 0x800;;
						point[index].speed_x=0;
						point[index].speed_y=0-400-(rand()&4095);
						point[index].speed_z=0;
						point[index].accel_x=0;
						point[index].accel_y=0;
						point[index].accel_z=0;
						point[index].active=TRUE;
					};
				};
			}
			else
			{
				//else plot new point position
				alive=TRUE;
				physical_plot(&point[index], TRUE);	
			};
			index++;
		};
	};
}

void pattern_flapsheet(void)
{
	uint8_t index=0;
	int x, z, xd, zd;
	uint8_t q[64];
	uint8_t state[64];
	uint8_t y=0;
	char finished=FALSE, exit=FALSE;
	
	fallout=FALSE;
	time = duration;

	led_framerate(FLAPSHEET_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	x=0;
	while(x != 8)
	{
		z=0;
		while(z != 8)
		{
			//calculate distance from center
			xd=7-x*2;
			if(xd<0)
				xd*=-1;
			zd=7-z*2;
			if(zd<0)
				zd*=-1;
				
			q[(x<<3)+z] = sqrt(xd*xd+zd*zd)*9;
			
			state[(x<<3)+z] = 0;
			z++;
		};
		x++;
	};
	
	//entry pattern
	index=8;
	while(!fallout && index--)
	{
		led_frame_left();
		led_line(7,6,0,7,6,7,1);
		led_waitframe(FLAPSHEET_FRAMESTEP);
	};

	//running process
	while(!fallout && !finished)
	{
		led_blank();
		index=0;
		finished=TRUE;
		while(index!=64)
		{	
			//normal running
			if(!duration || time)
			{
				q[index]+=FLAPSHEET_SPEED;
				finished=FALSE;
			}
			else
			//time up
			{
				//exit triggered?
				if(exit)
				{
					if(q[index] > FLAPSHEET_SPEED)
					{
						q[index]+=FLAPSHEET_SPEED;
						finished=FALSE;
					};
				}
				else
				{
					finished=FALSE;
					q[index]+=FLAPSHEET_SPEED;
					if(q[0] < FLAPSHEET_SPEED) //commence exit once corner hits 0deg
						exit=TRUE;
				};
			};
			
			y = pgm_read_byte(&sintable[q[index]]);
			y >>= 4;
			
			if(y==6)
				state[index]=1;
				
			if(state[index]==0)
				led_point(index & 0x07, 6, (index>>3)&0x07, TRUE );
			else
				led_point(index & 0x07, y, (index>>3)&0x07, TRUE );
			index++;
		};
		
		led_waitframe(FLAPSHEET_FRAMESTEP);
	};

	//end pattern
	index=8;
	while(!fallout && index--)
	{
		led_frame_left();
		led_waitframe(FLAPSHEET_FRAMESTEP);
	};
}

void pattern_spinface(void)
{
	uint8_t index=0, finished=0;

	fallout=FALSE;
	time = duration;

	led_framerate(SPINFACE_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

//##    ##
//##    ##
//   ##      
//   ## 
//  ####  
//##    ##  
//########
// ######

	led_line(0,0,4,1,0,4,TRUE);led_line(6,0,3,7,0,3,TRUE);
	led_line(0,1,4,1,1,4,TRUE);led_line(6,1,3,7,1,3,TRUE);
	led_line(3,2,4,4,2,3,TRUE);
	led_line(3,3,4,4,3,3,TRUE);
	led_line(2,4,4,5,4,3,TRUE);
	led_line(0,5,4,1,5,4,TRUE);led_line(6,5,3,7,5,3,TRUE);
	led_line(0,6,4,7,6,3,TRUE);
	led_line(1,7,4,6,7,3,TRUE);
	
	//running process
	while(!fallout && finished!=8 )
	{
		spin_y(index);
		index++;
		if(index==14)
			index=0;
		
		if(duration && !time)
		{
			led_frame_down();
			finished++;
		};
		
		led_waitframe(SPINFACE_FRAMESTEP);
	};
}

void pattern_fountain(void)
{
	uint8_t index, finished=FALSE;
	struct	physical_struct physical[FOUNTAIN_NUMBEROF];
	unsigned int frame_count=0;
	unsigned char mode=0;
	
	fallout=FALSE;
	time = duration;

	led_framerate(FOUNTAIN_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	index=0;
	while(index!=FOUNTAIN_NUMBEROF)
		physical[index++].active=FALSE;
		index++;
	
	//running process
	while(!fallout && !finished)
	{
		led_blank();
		finished=TRUE;
		index=0;
		while(index!=FOUNTAIN_NUMBEROF)
		{
			if(physical[index].active==FALSE)
			{
				if(!duration || time)
				{
					switch(mode)
					{
						case 0:
							physical[index].position_x=0x0800;
							physical[index].position_z=0x7800;
							physical[index].speed_x = FOUNTAIN_XZ;
							physical[index].speed_z = -(FOUNTAIN_XZ);
							break;
							
						case 1:
							physical[index].position_x=0x7800;
							physical[index].position_z=0x7800;
							physical[index].speed_x = -(FOUNTAIN_XZ);
							physical[index].speed_z = -(FOUNTAIN_XZ);
							break;
							
						case 2:
							physical[index].position_x=0x7800;
							physical[index].position_z=0x0800;
							physical[index].speed_x = -(FOUNTAIN_XZ);
							physical[index].speed_z = FOUNTAIN_XZ;
							break;
							
						case 3:
							physical[index].position_x=0x0800;
							physical[index].position_z=0x0800;
							physical[index].speed_x = FOUNTAIN_XZ;
							physical[index].speed_z = FOUNTAIN_XZ;
							break;
					};
					physical[index].position_y=0x7800;
					physical[index].accel_x = 0;
					physical[index].accel_y = FOUNTAIN_GRAVITY;
					physical[index].accel_z = 0;
					physical[index].speed_y = -(FOUNTAIN_Y);
					physical[index].active=TRUE;
					physical_plot(&physical[index], TRUE);
					finished=FALSE;
				};
			}
			else
			{
				physical_process(&physical[index]);
				physical_plot(&physical[index], TRUE);
				finished=FALSE;
			};
			index++;
		};
		
		led_waitframe(FOUNTAIN_FRAMESTEP);
		frame_count++;
		if(frame_count==FOUNTAIN_MOVE)
		{
			frame_count=0;
			mode++;
			mode &=3;
		};
	};
}


void pattern_noise_scroll(void)
{
	uint8_t index=0;
	
	fallout=FALSE;
	time = duration;

	led_framerate(NOISE_SCROLL_FRAMERATE);
	led_plane_intensity_all(255);	

	while(!fallout && (!duration || time))
	{
		led_frame_up();
		index=0;
		while(index !=8)
			led_frame_bitmap[56+index++] |= (uint8_t)rand() & (uint8_t)rand() & (uint8_t)rand();
		led_waitframe(NOISE_SCROLL_FRAMESTEP);
	}
	led_blank();
}

void pattern_cube_stretch(void)
{
	uint8_t index;
	uint8_t x1, y1, z1, x2, y2, z2;

	fallout=FALSE;
	time = duration;

	led_framerate(CUBE_STRETCH_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	//begin in center
	x1=3;y1=3;z1=3;
	x2=4;y2=4;z2=4;
	cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
	led_waitframe(CUBE_STRETCH_FRAMESTEP);

	//expand out from center
	index=3;
	while(!fallout && (!duration || time) && index--)
	{
		led_blank();
		x1--;y1--;z1--;
		x2++;y2++;z2++;
		cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
		led_waitframe(CUBE_STRETCH_FRAMESTEP);
	};

	//running process
	while(!fallout && (!duration || time))
	{
		//scrunch left top front corner to right bottom back corner, then expand it back out to a full cube
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x1++;y1++;z1++;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x1--;y1--;z1--;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};

		//scrunch right bottom back corner to left top front corner, then expand it back out to a full cube
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x2--;y2--;z2--;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x2++;y2++;z2++;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};

		//scrunch right top front corner to left bottom back corner, then expand it back out to a full cube
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			y1++;z1++;
			x2--;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			y1--;z1--;
			x2++;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};

		//scrunch left top back corner to right bottom front corner, then expand it back out to a full cube
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x1++;y1++;
			z2--;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x1--;y1--;
			z2++;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		
		//scrunch left bottom front corner to right top back corner, then expand it back out to a full cube
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x1++;z1++;
			y2--;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x1--;z1--;
			y2++;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		
		//scrunch right top back corner to left bottom front corner, then expand it back out to a full cube
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x2--;z2--;
			y1++;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			x2++;z2++;
			y1--;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};

		//scrunch left bottom back corner to right top front corner, then expand it back out to a full cube
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			y2--;z2--;
			x1++;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			y2++;z2++;
			x1--;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		
		//scrunch right bottom front corner to left top back corner, then expand it back out to a full cube
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			y2--;x2--;
			z1++;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
		index=6;
		while(!fallout && index--)
		{
			led_blank();
			y2++;x2++;
			z1--;
			cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
			led_waitframe(CUBE_STRETCH_FRAMESTEP);
		};
	};

	//contract back in
	index=3;
	while(!fallout && index--)
	{
		led_blank();
		x1++;y1++;z1++;
		x2--;y2--;z2--;
		cube_draw(x1, y1, z1, x2, y2, z2, TRUE);
		led_waitframe(CUBE_STRETCH_FRAMESTEP);
	};
	led_blank();
	led_waitframe(1);
}

void pattern_screw(void)
{
	uint8_t x1=0,y1=0,z1=0,x2=7,y2=7,z2=0;
	unsigned int count=0;
	uint8_t exit_mode= DIRECTION_BACKWARD;

	fallout=FALSE;
	time = duration;

	led_framerate(SCREW_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	//running process
	while(!fallout && (!duration || time))
	{
		x1=0;y1=0;z1=0;x2=7;y2=7;z2=0;
		
		count=SCREW_ROTATIONS*28;
		while(count-- && !fallout && (!duration || time))
		{
			exit_mode= DIRECTION_BACKWARD;
			led_frame_back();
			point_rotate(ROTATE_CLOCKWISE, &x1, &y1, &z1);
			point_rotate(ROTATE_CLOCKWISE, &x2, &y2, &z2);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};
		
		count=7;
		while(count-- && !fallout && (!duration || time))
		{
			led_frame_back();
			point_rotate(ROTATE_CLOCKWISE, &x1, &y1, &z1);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};
		
		count=7;
		while(count-- && !fallout && (!duration || time))
		{
			exit_mode= DIRECTION_LEFT;
			led_frame_left();
			point_rotate(ROTATE_UP, &x1, &y1, &z1);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};
		count=SCREW_ROTATIONS*28;
		while(count-- && !fallout && (!duration || time))
		{
			led_frame_left();
			point_rotate(ROTATE_UP, &x1, &y1, &z1);
			point_rotate(ROTATE_UP, &x2, &y2, &z2);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};

		count=7;
		while(count-- && !fallout && (!duration || time))
		{
			led_frame_left();
			point_rotate(ROTATE_UP, &x2, &y2, &z2);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};
		
		count=7;
		while(count-- && !fallout && (!duration || time))
		{
			exit_mode= DIRECTION_DOWN;
			led_frame_down();
			point_rotate(ROTATE_LEFT, &x2, &y2, &z2);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};
		count=SCREW_ROTATIONS*28;
		while(count-- && !fallout && (!duration || time))
		{
			led_frame_down();
			point_rotate(ROTATE_LEFT, &x1, &y1, &z1);
			point_rotate(ROTATE_LEFT, &x2, &y2, &z2);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};
		
		count=7;
		while(count-- && !fallout && (!duration || time))
		{
			led_frame_down();
			point_rotate(ROTATE_DOWN, &x1, &y1, &z1);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};
		
		count=7;
		while(count-- && !fallout && (!duration || time))
		{
			exit_mode= DIRECTION_BACKWARD;
			led_frame_back();
			point_rotate(ROTATE_DOWN, &x1, &y1, &z1);
			led_line(x1,y1,z1,x2,y2,z2, TRUE);
			led_waitframe(SCREW_FRAMESTEP);
		};
		
	};

	//end pattern by scrolling down
	count=8;
	while(!fallout && count--)
	{
		switch(exit_mode)
		{
			case DIRECTION_BACKWARD:
				led_frame_back();
				break;

			case DIRECTION_FORWARD:
				led_frame_forward();
				break;

			case DIRECTION_LEFT:
				led_frame_left();
				break;

			case DIRECTION_RIGHT:
				led_frame_right();
				break;

			case DIRECTION_UP:
				led_frame_up();
				break;

			case DIRECTION_DOWN:
				led_frame_down();
				break;
		};
		led_waitframe(SCREW_FRAMESTEP);
	};
}

//intensity ring -ringsize to +ringsize
//intensity -255 - 0 is off to full
//intensity 0 - 255 is full to off
//intensity changes by framestep each frame (affects speed)
//intensity distance of starting locations between frames
void pattern_waveup(int ringsize, int framestep, int distance, int framerate)
{
	char finished=FALSE;
	int intensity[8];
	int index=0;
	int tempint;
	
	fallout=FALSE;
	time = duration;

	//configure initial intensity, begin with all plains off
	intensity[7]=-255;
	intensity[6]=intensity[7]-distance;
	intensity[5]=intensity[6]-distance;
	intensity[4]=intensity[5]-distance;
	intensity[3]=intensity[4]-distance;
	intensity[2]=intensity[3]-distance;
	intensity[1]=intensity[2]-distance;
	intensity[0]=intensity[1]-distance;
	
	//frame rate
	led_framerate(framerate);
	
	//set intensitys to 0
	index=0;
	while(index!=8)
		led_plane_intensity(index++,0);
	
	//all leds on
	memset(led_frame_bitmap, 0xFF, 64);

	//running process
	while(!fallout && (!duration || time))
	{
		index=0;
		while(index!=8)
		{
			tempint = intensity[index];
			tempint++;
			
			if(tempint==ringsize+1)
				tempint=-ringsize;
				
			if(tempint < 0)
			{
				if(tempint > -255)
					led_plane_intensity(index, tempint+255);
				else
					led_plane_intensity(index, 0);
			}
			else
			{
				if(tempint < 255)
					led_plane_intensity(index, 255-tempint);
				else
					led_plane_intensity(index, 0);
			};
			intensity[index]=tempint;
			
			index++;
		};
		led_waitframe(1);
	};
	
	//exit process
	while(!fallout & !finished)
	{
		index=0;
		finished=TRUE;
		while(index!=8)
		{
			tempint = intensity[index];
			if(tempint != -ringsize)
			{
				tempint++;
				finished=FALSE;
			};
			
			if(tempint==ringsize+1)
				tempint=-ringsize;
			
			if(tempint < 0)
			{
				if(tempint > -255)
					led_plane_intensity(index, tempint + 255);
				else
					led_plane_intensity(index, 0);
			}
			else
			{
				if(tempint < 255)
					led_plane_intensity(index, 255 - tempint);
				else
					led_plane_intensity(index, 0);
			};
			intensity[index]=tempint;
			
			index++;
		};
		led_waitframe(1);
	};
}


void pattern_rotate1(void)
{
	enum {LEFT=1, RIGHT, TOP, BOTTOM, FRONT, BACK};
	enum {PERL, PERR, PERU, PERD, PERC, PERA, FRML, FRMR, FRMU, FRMD, FRMB, FRMF};

	uint8_t place=TOP, index, count, oplist[12], lastop=FRMU;
	
	fallout=FALSE;
	time = duration;
	
	led_framerate(100);
	led_blank();
	led_plane_intensity_all(255);	
	
	count=6;
	while(count-- && !fallout)
	{
		led_plane_y_left(0);
		led_frame_bitmap[0] |=0x00;
		led_frame_bitmap[1] |=0x01;
		led_frame_bitmap[2] |=0x01;
		led_frame_bitmap[3] |=0x01;
		led_frame_bitmap[4] |=0x01;
		led_frame_bitmap[5] |=0x01;
		led_frame_bitmap[6] |=0x01;
		led_frame_bitmap[7] |=0x00;
		led_waitframe(4);
	};
	
	led_plane_y_left(0);
	led_waitframe(4);

	while(!fallout && (!duration || time))
	{		
		//clear operations list
		memset(oplist, 0, 12);
		
		//set potential operations with new place
		switch(place)
		{
			case TOP:
				oplist[FRMD]=BOTTOM;
				oplist[PERA]=LEFT;
				oplist[PERC]=RIGHT;
				oplist[PERD]=FRONT;
				oplist[PERU]=BACK;
				break;
			case BOTTOM:
				oplist[FRMU]=TOP;
				oplist[PERA]=RIGHT;
				oplist[PERC]=LEFT;
				oplist[PERD]=BACK;
				oplist[PERU]=FRONT;
				break;
			case LEFT:
				oplist[FRMR]=RIGHT;
				oplist[PERA]=BOTTOM;
				oplist[PERC]=TOP;
				oplist[PERL]=BACK;
				oplist[PERR]=FRONT;
				break;
			case RIGHT:
				oplist[FRML]=LEFT;
				oplist[PERA]=TOP;
				oplist[PERC]=BOTTOM;
				oplist[PERL]=FRONT;
				oplist[PERR]=BACK;
				break;
			case FRONT:
				oplist[FRMB]=BACK;
				oplist[PERD]=BOTTOM;
				oplist[PERU]=TOP;
				oplist[PERL]=LEFT;
				oplist[PERR]=RIGHT;
				break;
			case BACK:
				oplist[FRMF]=FRONT;
				oplist[PERD]=TOP;
				oplist[PERU]=BOTTOM;
				oplist[PERL]=RIGHT;
				oplist[PERR]=LEFT;
				break;
		};
		
		//eliminate reverse operation, leaves us with 4 potential operations
		if(lastop&1)
			oplist[lastop-1]=0;
		else
			oplist[lastop+1]=0;
		
		//select operation 1-4
		count=rand()&3;
		count++;
		index=0;
		while(count)
		{
			if(oplist[index])
				count--;
			index++;
		};
		index--;
		lastop=index;
		
		//determine new place
		place=oplist[index];
		
		count=7;
		while(count-- && !fallout)
		{
			switch(lastop)
			{
			case PERL:
				led_perimeter_left();
				break;
			case PERR:
				led_perimeter_right();
				break;
			case PERU:
				led_perimeter_up();
				break;
			case PERD:
				led_perimeter_down();
				break;
			case PERC:
				led_perimeter_clockwise();
				break;
			case PERA:
				led_perimeter_anticlockwise();
				break;
			case FRML:
				led_frame_left();
				break;
			case FRMR:
				led_frame_right();
				break;
			case FRMU:
				led_frame_up();
				break;
			case FRMD:
				led_frame_down();
				break;
			case FRMB:
				led_frame_back();
				break;
			case FRMF:
				led_frame_forward();
				break;
			};
			
			led_waitframe(4);
		};
	};

	//end pattern by scrolling down
	count=8;
	while(!fallout && count--)
	{
		led_frame_down();
		led_waitframe(4);
	};

}

void pattern_rain1(void)
{
	char finished=FALSE;
	uint8_t state[8][8], x, y, z, tempchar;
	uint8_t position[21] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 7}; //states 128-148

	fallout=FALSE;
	
	led_framerate(50);
	led_blank();
	led_plane_intensity_all(255);	

	x=0;
	while(x!=8)
	{
		z=0;
		while(z!=8)
			state[x][z++]=rand()&127;
		x++;
	};

	time = duration;

	while(!fallout && (!duration || time))
	{
		led_waitframe(1);
		x=0;
		while(x!=8)
		{
			z=0;
			while(z!=8)
			{
				//determine previous state, if previous state had a led on, turn it off
				tempchar = state[x][z];
				if(tempchar & 0x80)
				{
					y = position[tempchar & 0x7F];
					led_point(x,y,z,0);
				};
				
				//advance state
				tempchar++;
				
				//if drop complete, reset to random pre-led state
				if(tempchar==149)
					tempchar=rand()&127;
					
				//if new state has a led on, turn it on
				if(tempchar & 0x80)
				{
					y = position[tempchar & 0x7F];
					led_point(x,y,z,1);
				};
				
				//save state
				state[x][z]=tempchar;
				z++;
			};
			x++;
		};
	};

	while(!fallout && !finished)
	{
		led_waitframe(1);
		finished=TRUE;
		x=0;
		while(x!=8)
		{
			z=0;
			while(z!=8)
			{
				//determine previous state, if previous state had a led on, turn it off
				tempchar = state[x][z];
				if(tempchar & 0x80)
				{
					y = position[tempchar & 0x7F];
					led_point(x,y,z,0);
				};
				
				//advance state only if led is already on
				if(tempchar & 0x80)
					tempchar++;
				
				//if drop complete, reset to OFF state
				if(tempchar==149)
					tempchar=0;
					
				//if new state has a led on, turn it on
				if(tempchar & 0x80)
				{
					y = position[tempchar & 0x7F];
					led_point(x,y,z,1);
					finished=FALSE;
				};
				
				//save state
				state[x][z]=tempchar;
				z++;
			};
			x++;
		};
	};
}

void pattern_fireworks(void)
{	
	char finished=FALSE, flag;
	uint8_t firework_index, spray_index;
	struct firework_struct firework[FIREWORK_NUMBEROF];

	fallout=FALSE;
	
	led_framerate(FIREWORK_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	time = duration;

	//init
	firework_index=0;
	while(firework_index != FIREWORK_NUMBEROF)
		firework_init(&firework[firework_index++]);
	
	while(!fallout && !finished)
	{
		led_blank();
		finished=TRUE;
		firework_index=0;
		while(firework_index != FIREWORK_NUMBEROF)
		{
			//launch
			if(	firework[firework_index].state==FIREWORK_STATE_LAUNCH)
			{
				finished=FALSE;
				physical_process(&firework[firework_index].rocket);			//move rocket
				if(firework[firework_index].rocket.active==FALSE)			//did rocket go off screen before bursting?
					firework[firework_index].state=FIREWORK_STATE_DONE;		//if so re-init new launch
				else if(firework[firework_index].rocket.speed_y > 0)		//is rocket falling?
				{
					spray_index=0;
					while(spray_index != FIREWORK_SPRAYPOINTS)
					{
						memcpy(&firework[firework_index].spray[spray_index], &firework[firework_index].rocket, sizeof(struct physical_struct));
						firework[firework_index].spray[spray_index].speed_x = FIREWORK_BURST_XZ;
						firework[firework_index].spray[spray_index].speed_z = FIREWORK_BURST_XZ;
						firework[firework_index].spray[spray_index].speed_y = FIREWORK_BURST_Y;
						spray_index++;
					};	
					firework[firework_index].state=FIREWORK_STATE_SPRAY;
					firework[firework_index].rocket.active=FALSE;
				}
				else
					physical_plot(&firework[firework_index].rocket, TRUE);	//draw rocket
			}
			//spray
			else if(firework[firework_index].state==FIREWORK_STATE_SPRAY)
			{
				finished=FALSE;
				flag=TRUE;
				spray_index=0;
				while(spray_index != FIREWORK_SPRAYPOINTS)
				{
					physical_process(&firework[firework_index].spray[spray_index]);
					if(firework[firework_index].spray[spray_index].active)
					{
						physical_plot(&firework[firework_index].spray[spray_index], TRUE);
						flag=FALSE;
					};
					spray_index++;
				};
				if(flag)
					firework[firework_index].state = FIREWORK_STATE_DONE;
			}
			//done, re-init, unless time has expired
			else if(firework[firework_index].state==FIREWORK_STATE_DONE && (!duration || time))
			{
				finished=FALSE;
				firework_init(&firework[firework_index]);
			};
			firework_index++;
		};
		
//		led_point(0,7,0,TRUE);	//include this, if led in bottom corner flickers out, processor is not keeping up
		led_waitframe(1);
	};
}

void pattern_text1_P(PGM_P text)
{
	uint8_t finished=0;
	
	fallout=FALSE;
	
	led_framerate(TEXT1_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	text_set_P(text);	
	while(!fallout && finished!=15)
	{
		if(text_isfinished())
		{
			if(!duration || time)
				text_set_P(text);
			else
				finished++;
		};
		if(finished==0)
			text_draw_y(7, 1, 7);
		led_perimeter_left();
		led_line(0, 0, 1, 0, 7, 1, FALSE);		
		led_waitframe(TEXT1_FRAMESTEP);
	};
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

//rotate a point around the surface of the cube
static void point_rotate(uint8_t dir, uint8_t* x_ptr, uint8_t* y_ptr, uint8_t* z_ptr)
{
	uint8_t x,y,z;
	
	x=*x_ptr;
	y=*y_ptr;
	z=*z_ptr;
	
	switch(dir)
	{
		case ROTATE_UP:
			//front face up
			if(z==0 && y)
				y--;
			//top face back
			else if(y==0 && z<7)
				z++;
			//back face down
			else if (z==7 && y<7)
				y++;
			//bottom face forward
			else if (y==7 && z)
				z--;
			break;
			
		case ROTATE_DOWN:
			//front face down
			if(z==0 && y<7)
				y++;
			//top face forward
			else if(y==0 && z)
				z--;
			//back face up
			else if (z==7 && y)
				y--;
			//bottom face back
			else if (y==7 && z<7)
				z++;
			break;
		
		case ROTATE_LEFT:
			//front face left
			if(z==0 && x)
				x--;
			//left face back
			else if(x==0 && z<7)
				z++;
			//back face right
			else if(z==7 && x<7)
				x++;
			//right face forward
			else if(x==7 && z)
				z--;
			break;
		
		case ROTATE_RIGHT:
			//front face right
			if(z==0 && x<7)
				x++;
			//left face forward
			else if(x==0 && z)
				z--;
			//back face left
			else if(z==7 && x)
				x--;
			//right face back
			else if(x==7 && z<7)
				z++;
			break;
		
		case ROTATE_CLOCKWISE:
			//top face right
			if(y==0 && x<7)
				x++;
			//left face up
			else if(x==0 && y)
				y--;
			//bottom face left
			else if(y==7 && x)
				x--;
			//right face down
			else if(x==7 && y<7)
				y++;
			break;

		case ROTATE_ANTICLOCKWISE:
			//top face left
			if(y==0 && x)
				x--;
			//left face down
			else if(x==0 && y<7)
				y++;
			//bottom face right
			else if(y==7 && x<7)
				x++;
			//right face up
			else if(x==7 && y)
				y--;
			break;
	};

	*x_ptr=x;
	*y_ptr=y;
	*z_ptr=z;
}

//todo, add bounce options?
static void physical_process(struct physical_struct *physical_ptr)
{
	static struct physical_struct physical;	//static instance for arithmetic, hopefully speeds things up by removing de-referencing

	memcpy(&physical, physical_ptr, sizeof(struct physical_struct));

	if(physical.active==1)
	{
		//speed affects position
		physical.position_x += physical.speed_x;
		physical.position_y += physical.speed_y;
		physical.position_z += physical.speed_z;
		
		//acceleration affects speed
		physical.speed_x += physical.accel_x;
		physical.speed_y += physical.accel_y;
		physical.speed_z += physical.accel_z;
		
		if((physical.position_x | physical.position_y | physical.position_z ) & 0x8000)
			physical.active=FALSE;
		
		memcpy(physical_ptr, &physical, sizeof(struct physical_struct));
	};
}

//plot or erase a physical point
static void physical_plot(struct physical_struct *physical_ptr, char on_off)
{
	uint8_t x, y, z;
	
	x = (uint8_t)(physical_ptr->position_x >>12);
	y = (uint8_t)(physical_ptr->position_y >>12);
	z = (uint8_t)(physical_ptr->position_z >>12);

	if( ((x|y|z)&0xF8) == 0)
	{
		if(on_off)
			led_frame_bitmap[z+(y<<3)] |= 0x80>>x;
		else
			led_frame_bitmap[z+(y<<3)] &=~(0x80>>x);
	};
}

static void firework_init(struct firework_struct *firework_ptr)
{
	firework_ptr->rocket.position_x=rand()&0x7FFF;
	firework_ptr->rocket.position_y=0x7FFF;
	firework_ptr->rocket.position_z=rand()&0x7FFF;
	firework_ptr->rocket.speed_x=0;
	firework_ptr->rocket.speed_y= FIREWORK_LAUNCH_Y;
	firework_ptr->rocket.speed_z=0;
	firework_ptr->rocket.accel_x=0;
	firework_ptr->rocket.accel_y=FIREWORK_GRAVITY;
	firework_ptr->rocket.accel_z=0;
	firework_ptr->rocket.active=1;
	firework_ptr->state=FIREWORK_STATE_LAUNCH;
}


//draw or erase a cube between two points
//This could be sped up a LOT by not using bresenham lines
static void cube_draw(uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, char on_off)
{
	led_line(x1, y1, z1, x2, y1, z1, on_off);
	led_line(x1, y1, z2, x2, y1, z2, on_off);
	led_line(x1, y2, z1, x2, y2, z1, on_off);
	led_line(x1, y2, z2, x2, y2, z2, on_off);
	
	led_line(x1, y1, z1, x1, y1, z2, on_off);
	led_line(x2, y1, z1, x2, y1, z2, on_off);	
	led_line(x1, y2, z1, x1, y2, z2, on_off);
	led_line(x2, y2, z1, x2, y2, z2, on_off);	

	led_line(x1, y1, z1, x1, y2, z1, on_off);
	led_line(x2, y1, z1, x2, y2, z1, on_off);
	led_line(x1, y1, z2, x1, y2, z2, on_off);
	led_line(x2, y1, z2, x2, y2, z2, on_off);
}

//spin an image on the y axis, image must start as a z plane with depth 4 on the left and 3 on the right
//14 steps 0-13
//  ####
//      ####
static void spin_y(uint8_t step)
{
	switch(step)
	{
	case 0:
	led_plane_x_back(0);
	led_plane_x_back(1);
	led_plane_x_forward(6);
	led_plane_x_forward(7);
	break;
	case 1:
	led_plane_x_back(0);
	led_plane_x_back(2);
	led_plane_x_forward(5);
	led_plane_x_forward(7);
	break;
	case 2:
	led_plane_x_back(0);
	led_plane_x_back(1);
	led_plane_x_forward(6);
	led_plane_x_forward(7);
	break;
	case 3:
	led_plane_z_right(7);
	led_plane_z_right(6);
	led_plane_z_left(1);
	led_plane_z_left(0);
	break;
	case 4:
	led_plane_z_right(7);
	led_plane_z_right(5);
	led_plane_z_left(2);
	led_plane_z_left(0);
	break;
	case 5:
	led_plane_z_right(7);
	led_plane_z_right(6);
	led_plane_z_left(1);
	led_plane_z_left(0);
	break;
	case 6:
	led_plane_z_right(7);
	led_plane_z_right(6);
	led_plane_z_right(5);
	led_plane_z_right(4);
	led_plane_z_left(3);
	led_plane_z_left(2);
	led_plane_z_left(1);
	led_plane_z_left(0);
	break;
	case 7:
	led_plane_z_right(7);
	led_plane_z_right(6);
	led_plane_z_left(1);
	led_plane_z_left(0);
	break;
	case 8:
	led_plane_z_right(7);
	led_plane_z_right(5);
	led_plane_z_left(2);
	led_plane_z_left(0);
	break;
	case 9:
	led_plane_z_right(7);
	led_plane_z_right(6);
	led_plane_z_left(1);
	led_plane_z_left(0);
	break;
	case 10:
	led_plane_x_forward(7);
	led_plane_x_forward(6);
	led_plane_x_back(1);
	led_plane_x_back(0);
	break;        
	case 11:
	led_plane_x_forward(7);
	led_plane_x_forward(5);
	led_plane_x_back(2);
	led_plane_x_back(0);
	break;
	case 12:
	led_plane_x_forward(7);
	led_plane_x_forward(6);
	led_plane_x_back(1);
	led_plane_x_back(0);
	break;
	case 13:
	led_plane_x_forward(7);
	led_plane_x_forward(6);
	led_plane_x_forward(5);
	led_plane_x_forward(4);
	led_plane_x_back(3);
	led_plane_x_back(2);
	led_plane_x_back(1);
	led_plane_x_back(0);
	break;
	};
}







//***************************************************************
// Patterns which got commented out because they do not look good
/*

void pattern_bugs(void)
{
	uint8_t index;
	struct physical_struct point[BUGS_NUMBEROF];

	fallout=FALSE;
	time = duration;

	led_framerate(BUGS_FRAMERATE);
	led_blank();
	led_plane_intensity_all(255);	

	index=0;
	while(index != BUGS_NUMBEROF)
		bugs_point_init(&point[index++]);
	
	//running process
	while(!fallout && (!duration || time))
	{
		index=0;
		led_blank();
		while(index != BUGS_NUMBEROF)
		{
			if(point[index].active==FALSE)
			{
				bugs_point_init(&point[index]);
				physical_plot(&point[index], TRUE);
			}
			else
			{
				physical_process(&point[index]);
				physical_plot(&point[index], TRUE);
			};
			index++;
		};
		led_waitframe(BUGS_FRAMESTEP);
	};

	//end pattern
	index=8;
	while(!fallout && index--)
	{
		led_frame_down();
		led_waitframe(BUGS_FRAMESTEP);
	};
}

static void bugs_point_init(struct physical_struct *point_ptr)
{
	uint8_t temp;
	
	temp=rand()%6;
	
	switch(temp)
	{
		case 0:
			point_ptr->position_x = 0x1000 + (rand()%0x6000);
			point_ptr->position_y = 0x1000 + (rand()%0x6000);
			point_ptr->position_z = 0;	
			point_ptr->speed_x = 0;
			point_ptr->speed_y = 0;
			point_ptr->speed_z = BUGS_SPEED;
			break;
		case 1:
			point_ptr->position_z = 0x1000 + (rand()%0x6000);
			point_ptr->position_y = 0x1000 + (rand()%0x6000);
			point_ptr->position_x = 0;	
			point_ptr->speed_z = 0;
			point_ptr->speed_y = 0;
			point_ptr->speed_x = BUGS_SPEED;
			break;
		case 2:
			point_ptr->position_x = 0x1000 + (rand()%0x6000);
			point_ptr->position_z = 0x1000 + (rand()%0x6000);
			point_ptr->position_y = 0;	
			point_ptr->speed_x = 0;
			point_ptr->speed_z = 0;
			point_ptr->speed_y = BUGS_SPEED;
			break;
		case 3:
			point_ptr->position_x = 0x1000 + (rand()%0x6000);
			point_ptr->position_y = 0x1000 + (rand()%0x6000);
			point_ptr->position_z = 0x7000;	
			point_ptr->speed_x = 0;
			point_ptr->speed_y = 0;
			point_ptr->speed_z = -(BUGS_SPEED);
			break;
		case 4:
			point_ptr->position_z = 0x1000 + (rand()%0x6000);
			point_ptr->position_y = 0x1000 + (rand()%0x6000);
			point_ptr->position_x = 0x7000;	
			point_ptr->speed_z = 0;
			point_ptr->speed_y = 0;
			point_ptr->speed_x = -(BUGS_SPEED);
			break;
		case 5:
			point_ptr->position_x = 0x1000 + (rand()%0x6000);
			point_ptr->position_z = 0x1000 + (rand()%0x6000);
			point_ptr->position_y = 0x7000;	
			point_ptr->speed_x = 0;
			point_ptr->speed_z = 0;
			point_ptr->speed_y = -(BUGS_SPEED);
			break;
	};
	point_ptr->accel_x=0;
	point_ptr->accel_y=0;
	point_ptr->accel_z=0;
	point_ptr->active=TRUE;
}



static void cube_bugs_direction_choose(struct bug_struct *bug_ptr)
{
	uint8_t directions=6, choice, index;
	char direction[7]={0,1,1,1,1,1,1};

	//erase backtrack move
	switch(bug_ptr->direction)
	{
		case DIRECTION_LEFT:
			direction[DIRECTION_RIGHT]=FALSE;
			break;
		case DIRECTION_RIGHT:
			direction[DIRECTION_LEFT]=FALSE;
			break;
		case DIRECTION_UP:
			direction[DIRECTION_DOWN]=FALSE;
			break;
		case DIRECTION_DOWN:
			direction[DIRECTION_UP]=FALSE;
			break;
		case DIRECTION_FORWARD:
			direction[DIRECTION_BACKWARD]=FALSE;
			break;
		case DIRECTION_BACKWARD:
			direction[DIRECTION_FORWARD]=FALSE;
			break;
	};
	directions--;

	if(bug_ptr->x==0)
	{
		direction[DIRECTION_LEFT]=FALSE;
		directions--;
	};
	if(bug_ptr->x==7)
	{
		direction[DIRECTION_RIGHT]=FALSE;
		directions--;
	};
	if(bug_ptr->y==0)
	{
		direction[DIRECTION_UP]=FALSE;
		directions--;
	};
	if(bug_ptr->y==7)
	{
		direction[DIRECTION_DOWN]=FALSE;
		directions--;
	};
	if(bug_ptr->z==0)
	{
		direction[DIRECTION_FORWARD]=FALSE;
		directions--;
	};
	if(bug_ptr->z==7)
	{
		direction[DIRECTION_BACKWARD]=FALSE;
		directions--;
	};

	choice = rand() % directions;
	
	index=0;	
	do
	{
		//find next valid move
		while(direction[index]==FALSE)
			index++;
		
		//if we dont want this move, skip it
		if(choice)
			index++;
			
	//until we get the move we want
	}while(choice--);

	bug_ptr->direction = index;	
}
*/



/*
shitty bugs
void pattern_cube_bugs(void)
{
	struct bug_struct	bug[BUGS_NUMBEROF];
	uint8_t index, change;

	time = duration;

	led_framerate(BUGS_FRAMERATE);
	led_blank();
	led_plane_intensity(0,255);	
	led_plane_intensity(1,255);	
	led_plane_intensity(2,255);	
	led_plane_intensity(3,255);	
	led_plane_intensity(4,255);	
	led_plane_intensity(5,255);	
	led_plane_intensity(6,255);	
	led_plane_intensity(7,255);	

	index=0;
	while(index!=BUGS_NUMBEROF)
	{
		bug[index].x=0;
		bug[index].y=0;
		bug[index].z=0;
		
		cube_bugs_direction_choose(&bug[index]);
		index++;
	};

	//running process
	while(!fallout && (!duration || time))
	{
		led_blank();
		
		index=0;
		while(index != BUGS_NUMBEROF)
		{
			change=FALSE;
			
			if(bug[index].x==0 && bug[index].direction==DIRECTION_LEFT)
				change=TRUE;
			else if(bug[index].x==7 && bug[index].direction==DIRECTION_RIGHT)
				change=TRUE;
			else if(bug[index].y==0 && bug[index].direction==DIRECTION_UP)
				change=TRUE;
			else if(bug[index].y==7 && bug[index].direction==DIRECTION_DOWN)
				change=TRUE;
			else if(bug[index].z==0 && bug[index].direction==DIRECTION_FORWARD)
				change=TRUE;
			else if(bug[index].z==7 && bug[index].direction==DIRECTION_BACKWARD)
				change=TRUE;
			else if((rand()&0x07)==0)
				change=TRUE;
			
			if(change)
				cube_bugs_direction_choose(&bug[index]);
			
			switch(bug[index].direction)
			{
				case DIRECTION_LEFT:
					bug[index].x--;
					break;
				case DIRECTION_RIGHT:
					bug[index].x++;
					break;
				case DIRECTION_UP:
					bug[index].y--;
					break;
				case DIRECTION_DOWN:
					bug[index].y++;
					break;
				case DIRECTION_FORWARD:
					bug[index].z--;
					break;
				case DIRECTION_BACKWARD:
					bug[index].z++;
					break;
			};
			led_point(bug[index].x, bug[index].y, bug[index].z, TRUE);
			index++;
		};
		led_waitframe(BUGS_FRAMESTEP);
	};

	//end pattern by scrolling down
	index=8;
	while(!fallout && index--)
	{
		led_frame_down();
		led_waitframe(BUGS_FRAMESTEP);
	};
}
*/

/*
shitty triangle
void pattern_poly1(void)
{
	uint8_t point_x[3], point_y[3], point_z[3], trip[3], move_list[6], index, choice, dir[3], point_index, x, y, z;

	point_x[0]=0;point_y[0]=0;point_z[0]=0;dir[0]=0;trip[0]=0;
	point_x[1]=0;point_y[1]=0;point_z[1]=0;dir[1]=0;trip[1]=0;
	point_x[2]=0;point_y[2]=0;point_z[2]=0;dir[2]=0;trip[2]=0;

	led_framerate(40);
	led_blank();
	led_plane_intensity(0,255);	
	led_plane_intensity(1,255);	
	led_plane_intensity(2,255);	
	led_plane_intensity(3,255);	
	led_plane_intensity(4,255);	
	led_plane_intensity(5,255);	
	led_plane_intensity(6,255);	
	led_plane_intensity(7,255);	

	time = duration;

	while(!fallout && (!duration || time))
	{
		//turn leds off
		led_line(point_x[0], point_y[0], point_z[0], point_x[1], point_y[1], point_z[1], FALSE);
		led_line(point_x[1], point_y[1], point_z[1], point_x[2], point_y[2], point_z[2], FALSE);
		led_line(point_x[2], point_y[2], point_z[2], point_x[0], point_y[0], point_z[0], FALSE);
		
		//move leds
		point_index=0;
		do
		{
			x = point_x[point_index];
			y = point_y[point_index];
			z = point_z[point_index];
			
			if(!trip[point_index])
			{
				trip[point_index] = 5 + (rand()&15);
				
				//determine a new direction for point x,y,z
				//create list of possible moves
				move_list[0]=ROTATE_UP;
				move_list[1]=ROTATE_DOWN;
				move_list[2]=ROTATE_LEFT;
				move_list[3]=ROTATE_RIGHT;
				move_list[4]=ROTATE_CLOCKWISE;
				move_list[5]=ROTATE_ANTICLOCKWISE;
				
				//scrub out 2 ineffective moves, leaving 4 moves
				if(!point_rotate_iseffective(ROTATE_UP,x,y,z))
					move_list[0]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_DOWN,x,y,z))
					move_list[1]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_LEFT,x,y,z))
					move_list[2]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_RIGHT,x,y,z))
					move_list[3]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_CLOCKWISE,x,y,z))
					move_list[4]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_ANTICLOCKWISE,x,y,z))
					move_list[5]=ROTATE_NONE;
				
				choice=rand()&3;	//n'th selection
				index=0;	
				do
				{
					//find next valid move
					while(move_list[index]==ROTATE_NONE)
						index++;
					
					//if we dont want this move, skip it
					if(choice)
						index++;
						
				//until we get the move we want
				}while(choice--);
				
				dir[point_index]=move_list[index];
			}
			else
			{
				point_rotate(dir[point_index], &x, &y, &z);
				trip[point_index]--;
			};
			point_x[point_index]=x;
			point_y[point_index]=y;
			point_z[point_index]=z;
			
			point_index++;
		}while(point_index != 3);
		
		//turn leds on
		led_line(point_x[0], point_y[0], point_z[0], point_x[1], point_y[1], point_z[1], TRUE);
		led_line(point_x[1], point_y[1], point_z[1], point_x[2], point_y[2], point_z[2], TRUE);
		led_line(point_x[2], point_y[2], point_z[2], point_x[0], point_y[0], point_z[0], TRUE);

		led_waitframe(1);
	};

	//end pattern by scrolling down
	index=8;
	while(!fallout && index--)
	{
		led_frame_down();
		led_waitframe(1);
	};
}
*/

/*
shitty surface bugs
void pattern_surface_bugs(void)
{

	uint8_t point_x[12], point_y[12], point_z[12], trip[12], move_list[6], index, choice, dir[12], point_index, x, y, z;

	point_index=0;
	while(point_index!=12)
	{
		point_x[point_index]=0;point_y[point_index]=0;point_z[point_index]=0;dir[point_index]=0;trip[point_index]=0;
		point_index++;
	};
	
	led_framerate(40);
	led_blank();
	led_plane_intensity(0,255);	
	led_plane_intensity(1,255);	
	led_plane_intensity(2,255);	
	led_plane_intensity(3,255);	
	led_plane_intensity(4,255);	
	led_plane_intensity(5,255);	
	led_plane_intensity(6,255);	
	led_plane_intensity(7,255);	

	time = duration;

	while(!fallout && (!duration || time))
	{		
		//move leds
		point_index=0;
		do
		{
			x = point_x[point_index];
			y = point_y[point_index];
			z = point_z[point_index];

			led_point(x,y,z,FALSE);
			
			if(!trip[point_index])
			{
				trip[point_index] = 5 + (rand()&15);
				
				//determine a new direction for point x,y,z
				//create list of possible moves
				move_list[0]=ROTATE_UP;
				move_list[1]=ROTATE_DOWN;
				move_list[2]=ROTATE_LEFT;
				move_list[3]=ROTATE_RIGHT;
				move_list[4]=ROTATE_CLOCKWISE;
				move_list[5]=ROTATE_ANTICLOCKWISE;
				
				//scrub out 2 ineffective moves, leaving 4 moves
				if(!point_rotate_iseffective(ROTATE_UP,x,y,z))
					move_list[0]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_DOWN,x,y,z))
					move_list[1]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_LEFT,x,y,z))
					move_list[2]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_RIGHT,x,y,z))
					move_list[3]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_CLOCKWISE,x,y,z))
					move_list[4]=ROTATE_NONE;
				if(!point_rotate_iseffective(ROTATE_ANTICLOCKWISE,x,y,z))
					move_list[5]=ROTATE_NONE;
				
				choice=rand()&3;	//n'th selection
				index=0;	
				do
				{
					//find next valid move
					while(move_list[index]==ROTATE_NONE)
						index++;
					
					//if we dont want this move, skip it
					if(choice)
						index++;
						
				//until we get the move we want
				}while(choice--);
				
				dir[point_index]=move_list[index];
			}
			else
			{
				point_rotate(dir[point_index], &x, &y, &z);
				trip[point_index]--;
			};
			point_x[point_index]=x;
			point_y[point_index]=y;
			point_z[point_index]=z;
			
			led_point(x,y,z,TRUE);
			
			point_index++;
		}while(point_index != 12);

		led_waitframe(1);
	};

	//end pattern by scrolling down
	index=8;
	while(!fallout && index--)
	{
		led_frame_down();
		led_waitframe(1);
	};
}
*/

/*
shitty spinning cube
void pattern_cube(void)
{
	uint8_t point[8][3] = {{0,0,0}, {0,0,7}, {0,7,0}, {0,7,7}, {7,0,0}, {7,0,7}, {7,7,0}, {7,7,7}};
	uint8_t count=0, dir=ROTATE_UP;
	
	led_framerate(200);
	led_blank();
	led_plane_intensity(0,255);	
	led_plane_intensity(1,255);	
	led_plane_intensity(2,255);	
	led_plane_intensity(3,255);	
	led_plane_intensity(4,255);	
	led_plane_intensity(5,255);	
	led_plane_intensity(6,255);	
	led_plane_intensity(7,255);	

	time = duration;

	while(!fallout && (!duration || time))
	{
		
		led_line(point[0][0], point[0][1], point[0][2], point[4][0], point[4][1], point[4][2], FALSE);	//top left front to top right front
		led_line(point[4][0], point[4][1], point[4][2], point[5][0], point[5][1], point[5][2], FALSE);	//top right front to top right back
		led_line(point[5][0], point[5][1], point[5][2], point[1][0], point[1][1], point[1][2], FALSE);	//top right back to top left back
		led_line(point[1][0], point[1][1], point[1][2], point[0][0], point[0][1], point[0][2], FALSE);	//top left back to top left front
		
		led_line(point[2][0], point[2][1], point[2][2], point[6][0], point[6][1], point[6][2], FALSE);	//bot left front to bot right front
		led_line(point[6][0], point[6][1], point[6][2], point[7][0], point[7][1], point[7][2], FALSE);	//bot right front to bot right back
		led_line(point[7][0], point[7][1], point[7][2], point[3][0], point[3][1], point[3][2], FALSE);	//bot right back to bot left back
		led_line(point[3][0], point[3][1], point[3][2], point[2][0], point[2][1], point[2][2], FALSE);	//bot left back to bot left front
		
		led_line(point[0][0], point[0][1], point[0][2], point[2][0], point[2][1], point[2][2], FALSE);	//top left front to bot right front
		led_line(point[1][0], point[1][1], point[1][2], point[3][0], point[3][1], point[3][2], FALSE);	//top left back to bot right back
		led_line(point[4][0], point[4][1], point[4][2], point[6][0], point[6][1], point[6][2], FALSE);	//top right front to bot right front
		led_line(point[5][0], point[5][1], point[5][2], point[7][0], point[7][1], point[7][2], FALSE);	//top right back to bot right back
			
		point_rotate(dir, &point[0][0], &point[0][1], &point[0][2]);
		point_rotate(dir, &point[1][0], &point[1][1], &point[1][2]);
		point_rotate(dir, &point[2][0], &point[2][1], &point[2][2]);
		point_rotate(dir, &point[3][0], &point[3][1], &point[3][2]);
		point_rotate(dir, &point[4][0], &point[4][1], &point[4][2]);
		point_rotate(dir, &point[5][0], &point[5][1], &point[5][2]);
		point_rotate(dir, &point[6][0], &point[6][1], &point[6][2]);
		point_rotate(dir, &point[7][0], &point[7][1], &point[7][2]);
		
		count++;
		if(count==56)
		{
			count = 0;
			dir++;
			if(dir == 7)
				dir=1;
		};

		led_line(point[0][0], point[0][1], point[0][2], point[4][0], point[4][1], point[4][2], TRUE);	//top left front to top right front
		led_line(point[4][0], point[4][1], point[4][2], point[5][0], point[5][1], point[5][2], TRUE);	//top right front to top right back
		led_line(point[5][0], point[5][1], point[5][2], point[1][0], point[1][1], point[1][2], TRUE);	//top right back to top left back
		led_line(point[1][0], point[1][1], point[1][2], point[0][0], point[0][1], point[0][2], TRUE);	//top left back to top left front
		
		led_line(point[2][0], point[2][1], point[2][2], point[6][0], point[6][1], point[6][2], TRUE);	//bot left front to bot right front
		led_line(point[6][0], point[6][1], point[6][2], point[7][0], point[7][1], point[7][2], TRUE);	//bot right front to bot right back
		led_line(point[7][0], point[7][1], point[7][2], point[3][0], point[3][1], point[3][2], TRUE);	//bot right back to bot left back
		led_line(point[3][0], point[3][1], point[3][2], point[2][0], point[2][1], point[2][2], TRUE);	//bot left back to bot left front
		
		led_line(point[0][0], point[0][1], point[0][2], point[2][0], point[2][1], point[2][2], TRUE);	//top left front to bot right front
		led_line(point[1][0], point[1][1], point[1][2], point[3][0], point[3][1], point[3][2], TRUE);	//top left back to bot right back
		led_line(point[4][0], point[4][1], point[4][2], point[6][0], point[6][1], point[6][2], TRUE);	//top right front to bot right front
		led_line(point[5][0], point[5][1], point[5][2], point[7][0], point[7][1], point[7][2], TRUE);	//top right back to bot right back
		
		led_waitframe(10);
	};
}
*/




/*
//determine if a particular permiter scroll will effect a particular point
static char point_rotate_iseffective(uint8_t dir, uint8_t x, uint8_t y, uint8_t z)
{
	char retval=TRUE;
	switch(dir)
	{
		case ROTATE_UP:
		case ROTATE_DOWN:		
			if(x==0 || x==7)
				if(z!=0 && z!=7 && y!=0 && y!=7)
					retval=FALSE;
			break;
		
		case ROTATE_LEFT:		
		case ROTATE_RIGHT:
			if(y==0 || y==7)
				if(z!=0 && z!=7 && x!=0 && x!=7)
					retval=FALSE;		
			break;
		
		case ROTATE_CLOCKWISE:
		case ROTATE_ANTICLOCKWISE:
			if(z==0 || z==7)
				if(x!=0 && x!=7 && y!=0 && y!=7)
					retval=FALSE;
			break;
	};
	return retval;
}
*/
