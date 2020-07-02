
void Char_definition(void);
void Seg_definitions(void);
void Display_driver(void);
void update_timer(void);
void update_milli_sec_display(void);







/*****************************************************************************************/
void Char_definition()
{switch (display_buf[buf_ptr]){
case '0': zero; break;
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



/*****************************************************************************************/
void Seg_definitions(){ char m=0;
switch(buf_ptr){
case 0: m = 0x01; break;
case 1: m = 0x02; break;
case 2: m = 0x04; break;
case 3: m = 0x08; break;
case 4: m = 0x10; break;
case 5: m = 0x20; break;
case 6: m = 0x40; break;
case 7: m = 0x80; break;}
if (display_buf[0] & m) PORTB &= (~(seg_a));
if (display_buf[1] & m) PORTD &= (~(seg_g));
if (display_buf[2] & m) PORTD &= (~(seg_d));
if (display_buf[3] & m) PORTD &= (~(seg_b));
if (display_buf[4] & m) PORTD &= (~(seg_c));
if (display_buf[5] & m) PORTD &= (~(seg_e));
if (display_buf[6] & m) PORTD &= (~(seg_f));}


/*****************************************************************************************/
void Display_driver()			//ISR calls this
{buf_ptr++; buf_ptr = buf_ptr%8;
clear_digits; clear_display; 
switch(buf_ptr){
	case 0: if (display_mask & 0x01){if ((strobe[0]++)%18 <= 12) {digit_0;}} else{digit_0;} break;
	case 1: if (display_mask & 0x02){if ((strobe[1]++)%18 <= 12) {digit_1;}} else{digit_1;} break;
	case 2: if (display_mask & 0x04){if ((strobe[2]++)%18 <= 12) {digit_2;}} else{digit_2;} break;
	case 3: if (display_mask & 0x08){if ((strobe[3]++)%18 <= 12) {digit_3;}} else{digit_3;} break;
	case 4: if (display_mask & 0x10){if ((strobe[4]++)%18 <= 12) {digit_4;}} else{digit_4;} break;
	case 5: if (display_mask & 0x20){if ((strobe[5]++)%18 <= 12) {digit_5;}} else{digit_5;} break;
	case 6: if (display_mask & 0x40){if ((strobe[6]++)%18 <= 12) {digit_6;}} else{digit_6;} break;
	case 7: if (display_mask & 0x80){if ((strobe[7]++)%18 <= 12) {digit_7;}} else{digit_7;} break;}

switch(mode){
case 4: case 6: case 7: case 8: case 9:
case 'A': case 'B': case 'C': 
case 'D': case 'E': case 'F': 
case 'I': case 'J': case 'K':
case 'L': case 'P': 
Char_definition(); break;

case 2:
case 3: 
case 'G': Seg_definitions(); break; 
  
case 1:
switch(buf_ptr){
case 0: 	if(display_buf[0] & 0x01) one_U; if (display_buf[0] & 0x02) ONE_U;
			if(display_buf[2] & 0x01) one_L; if (display_buf[2] & 0x02) ONE_L; break;
case 1: 	if(display_buf[0] & 0x04) one_U; if (display_buf[0] & 0x08) ONE_U;
			if(display_buf[2] & 0x04) one_L; if (display_buf[2] & 0x08) ONE_L; break;
case 2: 	if(display_buf[0] & 0x10) one_U; if (display_buf[0] & 0x20) ONE_U;
			if(display_buf[2] & 0x10) one_L; if (display_buf[2] & 0x20) ONE_L; break;
case 3: 	if(display_buf[0] & 0x40) one_U; if (display_buf[0] & 0x80) ONE_U;
			if(display_buf[2] & 0x40) one_L; if (display_buf[2] & 0x80) ONE_L; break;


case 4: 	if(display_buf[1] & 0x01) one_U; if (display_buf[1] & 0x02) ONE_U;
			if(display_buf[3] & 0x01) one_L; if (display_buf[3] & 0x02) ONE_L; break;
case 5: 	if(display_buf[1] & 0x04) one_U; if (display_buf[1] & 0x08) ONE_U;
			if(display_buf[3] & 0x04) one_L; if (display_buf[3] & 0x08) ONE_L; break;
case 6: 	if(display_buf[1] & 0x10) one_U; if (display_buf[1] & 0x20) ONE_U;
			if(display_buf[3] & 0x10) one_L; if (display_buf[3] & 0x20) ONE_L; break;
case 7: 	if(display_buf[1] & 0x40) one_U; if (display_buf[1] & 0x80) ONE_U;
			if(display_buf[3] & 0x40) one_L; if (display_buf[3] & 0x80) ONE_L; break;}break;


case 5: Char_definition(); 
switch(buf_ptr){
case 4: if(output_2 & 0x01) one; if(output_2 & 0x02) ONE; break;
case 5: if(output_2 & 0x04) one; if(output_2 & 0x08) ONE; break;
case 6: if(output_2 & 0x10) one; if(output_2 & 0x20) ONE; break;
case 7:if(output_2 & 0x40) one; if(output_2 & 0x80) ONE; break; }break;
}}


/********************************************************************************************/
void update_timer (){ 

if (timer_mode == 1){
if (clock_buf[0] < '9') clock_buf[0]++; else {clock_buf[0] = '0'; 
if (clock_buf[1] < '5') clock_buf[1]++;else {clock_buf[1] = '0'; 
if (clock_buf[3] < '9') clock_buf[3]++; else {clock_buf[3] = '0'; 
if (clock_buf[4] < '5') clock_buf[4]++;else {clock_buf[4] = '0'; 
switch (clock_buf[7]){
case '0': case '1': if (clock_buf[6] < '9') clock_buf[6]++; else {clock_buf[6] = '0';clock_buf[7]++;}break;
case '2': if (clock_buf[6] < '3') clock_buf[6]++; else {clock_buf[6] = '0';clock_buf[7] = '0';}}}}}}}

if (timer_mode == 2){
if (stop_watch_buf[3] < '9') stop_watch_buf[3]++; else {stop_watch_buf[3] = '0'; 
if (stop_watch_buf[4] < '5') stop_watch_buf[4]++;else {stop_watch_buf[4] = '0'; 
if (stop_watch_buf[6] < '9') stop_watch_buf[6]++; else {stop_watch_buf[6] = '0'; 
if (stop_watch_buf[7] < '5') stop_watch_buf[7]++;else {stop_watch_buf[7] = '0'; 
}}}}}

if (timer_mode == 3){
if (stop_watch_buf[2] < '9') stop_watch_buf[2]++; else {stop_watch_buf[2] = '0'; 
if (stop_watch_buf[3] < '5') stop_watch_buf[3]++;else {stop_watch_buf[3] = '0'; 
if (stop_watch_buf[5] < '9') stop_watch_buf[5]++; else {stop_watch_buf[5] = '0'; 
if (stop_watch_buf[6] < '5') stop_watch_buf[6]++;else {stop_watch_buf[6] = '0';
if (stop_watch_buf[7] < '9') stop_watch_buf[7]++; else {stop_watch_buf[7] = '0';}
}}}}}}


/*****************************************************************************************/
void update_milli_sec_display(){ 
switch (timer_mode){
case 3:	if (stop_watch_buf[0] < '9') {stop_watch_buf[0]++;} else {stop_watch_buf[0] = '0';}break;
case 2: if (stop_watch_buf[0] < '9') stop_watch_buf[0]++; else {stop_watch_buf[0] = '0';
if (stop_watch_buf[1] < '9') stop_watch_buf[1]++; else {stop_watch_buf[1] = '0';}}break;}}








