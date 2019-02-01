
void Char_definition(void);
void Seg_definitions(void);
void Display_driver(void);


/*****************************************************************************************/
void Display_driver()										//ISR calls this every 2mS
{buf_ptr++; buf_ptr = buf_ptr%8;
reset_digits; reset_segments; 								//Clear display before restoring it
switch(buf_ptr){
	case 0: {digit_0;} break;								//Activate each digit in turn (see header file).
	case 1: {digit_1;} break;
	case 2: {digit_2;} break;
	case 3: {digit_3;} break;
	case 4: {digit_4;} break;
	case 5: {digit_5;} break;
	case 6: {digit_6;} break;
	case 7: {digit_7;} break;}
	
	if(mode == 1)Char_definition();
	if(mode == 2)Seg_definitions();}


/*****************************************************************************************/
void Char_definition()
{switch (display_buf[buf_ptr]){							//"buff_pointer" matches each display digit  
case 0:	blank; break;										//with the keypress to be displayed which is stored in "display_buf"
case '0': zero; break;										//"zero, one" etc define the segments required
case '1': one; break;
case '2': two; break;
case '3': three; break;
case '4': four; break;
case '5': five; break;
case '6': six; break;
case '7': seven; break;
case '8': eight; break;
case '9': nine; break;
case 'P': decimalP; break;
case '.':
case '_': PORTD &= ~(seg_d); break;
case '-': PORTD &= (~(seg_g)); break;
case 'E': case 'e':
case 'X': PORTB &= (~(seg_a)); PORTD &= (~(seg_d | seg_e | seg_f | seg_g ));break;}} //E the exponential function



/********************************************************************************************/

void Seg_definitions(){ char m=0;							//"buff_pointer" matches each display digit	with the segment data					 
															//For each letter the segment data is stored in a single ellement of "display_buf"
m = (1 << buf_ptr);										//"buff_pointer" = 0, 1, 2....7 and "m" defines the display digit being illuminated
if (display_buf[0] & m) PORTB &= (~(seg_a));				//Check: is segment "a" required in the first display digit			
if (display_buf[1] & m) PORTD &= (~(seg_b));				//Check: is segment "b" required in the first display digit
if (display_buf[2] & m) PORTD &= (~(seg_c));				//Check: is segment "c" required in the first display digit
if (display_buf[3] & m) PORTD &= (~(seg_d));
if (display_buf[4] & m) PORTD &= (~(seg_e));
if (display_buf[5] & m) PORTD &= (~(seg_f));
if (display_buf[6] & m) PORTD &= (~(seg_g));}




