#include "board.h"
#include "terminal.h"
#include "process.h"
#include "usart.h"

char *text;  // For sending strings to usart

char* itoa(int val, int base) { // Convert integer to string, leading spaces.
	static char buf[16] = {0};
	int i = 14, sign = val;
	if (sign < 0) val = -val;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	if (sign <= 0) {
		if (sign == 0) buf[i]='0'; else buf[i]='-';
		i--;
	}
	for (; i >=0 ; i--) buf[i]=' ';
	return &buf[i+1];
}

int ctoi(char inp) {	// Convert a char to int, within 0..9 and A..Z
	if ((inp>64)&&(inp<91)) return inp-55; 
	if ((inp>47)&&(inp<58)) return inp-48;
	return 255;	// Zero is valid, use 255 as fail
}

void print_ruler(volatile avr32_usart_t *usart) {
	usart_write_line(usart, "________________________________________________________________________________\n");
}

void print_summary(volatile avr32_usart_t *usart) {	// TODO: collect errors from process and display it here.
	text = itoa(pulses,10);
	usart_write_line(usart, text);
	usart_write_line(usart, " pulses total,");
	text = itoa(types+1,10);
	usart_write_line(usart, text);
	usart_write_line(usart, " types.\n");
}

void print_pulses(volatile avr32_usart_t *usart) {
	int i;
	usart_write_line(usart, "\nCaptured pulses:\n");
	print_ruler(usart);
	usart_write_line(usart, "                   Sorted                      Not Sorted\n");
	usart_write_line(usart, "           Length           Type         Length           Type\n\n");
	for (i=0;i<pulses;i++) {
		text = itoa(s[i]->len,10);
		usart_write_line(usart, text);
		text = itoa(s[i]->type,10);
		usart_write_line(usart, text);
		text = itoa(p[i].len,10);
		usart_write_line(usart, text);
		text = itoa(p[i].type,10);
		usart_write_line(usart, text);
		usart_write_line(usart, "\n");
	}
	usart_write_line(usart, "\n");
	print_summary(usart);
}

void print_types(volatile avr32_usart_t *usart) {
	int i,temp;
	char p,q;
	usart_write_line(usart, "        Type number       Length         Pulses       Bits equ.\n\n");
	for (i=0;i<=types;i++) { 
		text = itoa(i,10);
		usart_write_line(usart, text);
		temp=t[i].min*267;		// "min" was overwritten by average value in quant. function
		if (temp>100000) {temp/=1000; p = 'm';} else p='u'; // Prefix
		text = itoa(temp,10);
		q=text[14];	text[14]=text[13];	// Copy "decimals"
		text[13]='.';
		usart_write_line(usart, text);
		usart_putchar(usart,q);
		usart_putchar(usart,p);
		usart_putchar(usart,'s');
		text = itoa(t[i].num,10);
		usart_write_line(usart, text);
		text = itoa(t[i].bit,10);
		usart_write_line(usart, text);
		usart_write_line(usart, "\n");
	}
	usart_write_line(usart, "\n");
}

void print_menu(volatile avr32_usart_t *usart, char type) {
	print_ruler(usart);			// Print a horisontal line over the screen
	usart_write_line(usart, decode_str);
	usart_putchar(usart,'\n');
	print_ruler(usart);			// Print a horisontal line over the screen
	usart_write_line(usart, "Replace [");
	usart_putchar(usart, type);	// Print type number in menu above	
	usart_write_line(usart, "] with [V]tab  [L]ine  [T]ab [S]pace       [U]ndo [C]lear [R]estart\n");
	usart_write_line(usart, "Pattern [Z]ero:");
	usart_putchar(usart,dec.num[0]+48);
	usart_write_line(usart," [O]ne:");
	usart_putchar(usart,dec.num[1]+48);
	usart_write_line(usart," [M]od:");
	usart_putchar(usart,dec.mod+48);
	usart_write_line(usart," [G]roup:");
	usart_putchar(usart,dec.num[2]+48);
	usart_write_line(usart," [+-]             [Q]uit  [P]ulses\n");
	print_ruler(usart);			// Print a horisontal line over the screen
	print_types(usart);			
}

void getkey(volatile avr32_usart_t *usart, int pos, unsigned int out[]) { // Get pattern
	usart_write_line(usart,"Enter pattern: ");
	int i=0; char input;
	do {
		input=usart_getchar(usart);
		usart_putchar(usart,input);	// Echo back
		out[i]=ctoi(input);	// Store
		i++;
	} while((i<8)&&(input!=13));	// Enter = finish
	dec.num[pos]=i-1; // Do not count the enter
	undo_code=200+pos;
}


