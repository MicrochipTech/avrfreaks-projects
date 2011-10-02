#define	IN_0	PIND0	//mode set 	(new_pos == 0)
#define	IN_1	PIND1	//down		(new_pos == 1)
#define	IN_2	PIND2	//up		(new_pos == 2)
#define	IN_3	PIND3	//store		(new_pos == 3)

#define PIN_0	PIND
#define PIN_1	PIND
#define PIN_2	PIND
#define PIN_3	PIND

#define OUT_G	PORTD6
#define OUT_R	PORTB0
#define OUT_B	PORTB1

#define PORT_OUT_G	PORTD
#define PORT_OUT_R	PORTB
#define PORT_OUT_B	PORTB

#define DELAY_MS	200

#define	PWM_RED		OCR0A	//czerwony
#define	PWM_GREEN	OCR1A	//zielony
#define	PWM_BLUE	OCR1B	//niebieski

//predefinicje funkcji
void demo(void);

