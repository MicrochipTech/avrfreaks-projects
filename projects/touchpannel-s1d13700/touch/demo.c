#include "touch.h"
#include "s1d13700.h"
#include "glcd.h"

void add_button(unsigned int xpos, unsigned int ypos, PGM_VOID_P txt)
{
	point_t p1,p2;
	p1.x= p2.x = xpos;
	p1.y= p2.y = ypos;
	if (glcd_text_widthP(txt) < 40) p2.x +=40;
	else p2.x= xpos+ glcd_text_widthP(txt)+8;
	
	p2.y= ypos+glcd_FontHeight+4;
	lcd_layer(BTN_LAYER);
	lcd_print_rect(p1, p2);
	lcd_layer(TXT_LAYER);
	glcd_textP(txt,0,CENTER_VIEW,xpos+2, ypos+2,0);
	add_area(p1, p2);
}

void clear_buttons(void)
{
	clear_areas();
	lcd_clrscrn(BTN_LAYER); // clears Layer : BTN_LAYER
	lcd_clrscrn(TXT_LAYER);	// clears Layer : TXT_LAYER
}

void calibrate_touchscreen(void)
{
	static const char txt_touch1[] PROGMEM = {"TOUCH KALIBRIERUNG"};
	static const char txt_touch2[] PROGMEM = {"Bitte berühre das Fadenkreuz"};
	static const char txt_speichern[] PROGMEM = {"Daten speichern ?"};
	static const char txt_ja[] PROGMEM = {"Ja"};
	static const char txt_nein[] PROGMEM = {"Nein"};
	static const char txt_gespeichert[] PROGMEM = {"gespeichert ...."};
	
	point_t p1, p2;
// draw cross at position 1
	p1.x = 20;
	p1.y = 20;
	lcd_print_cross(p1, 10, 10); // prints a cross onto the display position
	glcd_select_font(F_LARGE);		// this function specifys the font for writing text onto the display
	glcd_textP(txt_touch1,0,LEFT_VIEW,50,100,0);	// this function writes a graphical text onto the display
	glcd_textP(txt_touch2,0,LEFT_VIEW,60,0,0);	// this function writes a graphical text onto the display
	
	clear_settings();  // enable capturing adc values
	capture_point(1);  // capture first point for x and y position
	lcd_clrscrn(1);
	delay_ms(300);
	glcd_textP(txt_touch1,0,LEFT_VIEW,50,100,0);  // this function writes a graphical text onto the display
	glcd_textP(txt_touch2,0,LEFT_VIEW,0,217,0);	// this function writes a graphical text onto the display

// draw cross at position 2
	p2.x = PIXELX - 20;
	p2.y = PIXELY - 20;
	lcd_print_cross(p2, 10, 10);	// prints a cross onto the display position

	capture_point(2);  // capture second point
	lcd_clrscrn(1);		// clears the display screen
	calc_config(p1, p2);  //  calculates parameters for scale and offset of touch screen
	glcd_textP(txt_speichern,0,LEFT_VIEW,80,100,0);// thsi function writes a graphical text onto the display
	clear_areas(); // clears the touch areas which has been defined
	add_button(100,130 ,txt_ja);	// adds a button with a rectangle on the screen and the text
	add_button(170,130, txt_nein);	// adds a button with a rectangle on the screen and the text
	delay_ms(1000);	// delay function 1 second
	int8_t res=capture_area() ;  // captures a voltage for x and y 
	clear_buttons();	// clears buttons and the displayed rectangles and text
	if (res== 1) 
	{
		
		glcd_textP(txt_gespeichert,0,LEFT_VIEW,80,120,0);// this function writes a graphical text onto the display
		save_touch_settings();	// writes settings to EEPROM
		delay_ms(500);
		lcd_clrscrn(TXT_LAYER);
	} 
	lcd_clrscrn(0); // clears all layers of display screen
}


int main(void)
{
	lcd_init();
	if (touch_init(1)==-1) calibrate_touchscreen();
 	point_t pos1;
	while(1==1)
	{
		if (read_touch()==1)
		{
			pos1.x=touch_pos.x;
			pos1.y=touch_pos.y;
			lcd_print_pixel(pos1);	// prints one pixel to the pressed display position
		}
	}
}