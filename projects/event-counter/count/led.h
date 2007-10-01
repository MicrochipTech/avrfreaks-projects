#define NR_LED	4

#define A	0x01
#define B	0x02
#define C 	0x04
#define D 	0x08
#define E	0x10
#define	F	0x20
#define	G	0x40
#define	DOT	0x80




void set_display(int16_t);
char char_to_led(char);
void refresh_led(void);
//hw layer
void set_led_display(uint8_t, uint8_t);

