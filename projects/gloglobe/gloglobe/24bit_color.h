#ifndef _24BIT_COLOR_
#define _24BIT_COLOR_

/*
 * $Id: 24bit_color.h,v 1.3 2006/10/22 18:20:19 nemo Exp $ 
 *
 * this file describes a simple
 * state machine that takes you around 
 * the color wheel.  note that not all 
 * colors are possible with incColor.
 * States are color values in -1*RGB 
 * (0=LED on, 1=LED off)
 * 
 * state1:ff0000
 * state2:ffff00
 * state3:00ff00
 * state4:00ffff
 * state5:0000ff
 * state6:ff00ff
 * state1:ff0000 (you are back at the start)
 * init state = 1;
 * */

#define COL_LOW 0x07
#define COL_HIGH 0xff

static uint8_t colorState;
static uint8_t r_pt;
static uint8_t g_pt;
static uint8_t b_pt;

static void initColor(uint8_t state){
	colorState = state;
        switch(state){
	case 1:
                r_pt=COL_HIGH; g_pt=COL_LOW; b_pt=COL_LOW;
		break;
	case 2:
                r_pt=COL_HIGH; g_pt=COL_HIGH; b_pt=COL_LOW;
		break;
        }
}
static void incColor(uint16_t step){
        switch (colorState){
		case 1:
	                //init r=ff, g=00, b=00;
		    (g_pt < COL_HIGH)?(g_pt+=step):colorState++;
		  break;
                case 2:
                  //init r=ff, g=ff, b=00;
		  (r_pt>COL_LOW)?(r_pt-= step):colorState++;
                case 3:
                  //init r=00, g=ff, b=00;
		  (b_pt<COL_HIGH)?b_pt+=step:colorState++;            
		  break;
                case 4:
		  //init r=00, g=ff, b=ff;
		  (g_pt>COL_LOW)?g_pt-=step:colorState++;
		  break;
                case 5:
                  //init r=00, g=00, b=ff;
                  (r_pt<COL_HIGH)?r_pt+=step:colorState++;
                  break;
                case 6:
                  //init r=ff, g=00, b=ff;
                  (b_pt>COL_LOW)?(b_pt-=step):(colorState=1);
                  break;
		default:
		  break;
                }
}

#endif /* _24BIT_COLOR_ */
