

#include <avr/io.h>
#include "comm.h"
#include <string.h>

void process_command(const unsigned char *command); 
void die(unsigned char d);

int main(void) {
	USART_Init(BAUD(9600));

	// Send a welcome message
	send_string("\r\n\r\nWelcome to ATMega16 shell!\r\n");
	send_string("> ");

	unsigned char command[100];
	unsigned int length;
	while (1) {
		length = receive_string(command, 100, 1);
		send_string("\r\n");
		
		process_command(command);

		send_string("> ");		
	}

	return 0;
}

void process_command(const unsigned char *command) {
	unsigned int length = strlen(command);

	if (!strncmp(command, "die", 3) && length == 5) {
		die(command[4] - '0');
	} 
}

void die(unsigned char d) {
	if (d < 1 || d > 6) {
		send_string("A die only has six sides!\r\n");
	} else {
		unsigned char t = d + '0';
		send_string("Setting die to ");
		send_char(t);
		send_string("\r\n");

		switch (d) {
			case 1:
				t = 0x08;
				break;
			case 2:
				t = 0x22;
				break;
			case 3:
				t = 0x01 | 0x08 | 0x40;
				break;
			case 4:
				t = 0x01 | 0x04 | 0x10 | 0x40;
				break;
			case 5:
				t = 0x01 | 0x04 | 0x08 | 0x10 | 0x40;
				break;
			case 6:
				t = 0x01 | 0x02 | 0x04 | 0x10 | 0x20 | 0x40;
				break;
		}			
		DDRA  = 0xff;
		PORTA = t;
	}
}
