#include "upd7225.h"
#include <avr/interrupt.h>

//#include <avr/eeprom.h>
//#include <avr/pgmspace.h>

//tablica konwersji kodow na znaki
uint8_t symbol_t[] PROGMEM =
{
	0x7d,	//0	-	$30
	0x60,	//1	-	$31
	0x3e,	//2
	0x7a,	//3
	0x63,	//4
	0x5b,	//5
	0x5f,	//6
	0x70,	//7
	0x7f,	//8
	0x7b,	//9
	0x77,	//A
	0x4f,	//b
	0x0e,	//c
	0x6e,	//d
	0x1f,	//E
	0x17,	//F
	0x00,	//pusty
	0x77,	//A - 	A 	$41
	0x4f,	//b - 	B 	$42
	0x0e,	//c - 	C 	$43
	0x6e,	//d	-	D	$44
	0x1f,	//E	-	E	$45
	0x17,	//F	-	F	$46
	0x7b,	//g	-	G	$47
	0x67,	//h	$48
	0x40,	//i	$49
	0x6c,	//j	$4a
	0x57,	//k	$4b
	0x0d,	//l	$4c
	0x56,	//m	$4d
	0x46,	//n	$4e
	0x4e,	//o	$4f
	0x37,	//p $50
	0x73,	//q	$51
	0x06,	//r	$52
	0x5b,	//s	$53
	0x0f,	//t	$54
	0x6d,	//u	$55
	0x4c,	//v	$56
	0x4c,	//w	$57
	0x67,	//x	$58
	0x6a,	//y	$59
	0x3e,	//z	$5a
	0x1d,	//[	$5b
	0x45,	/* \ //$5c */
	0x78,	//]	$5d
	0x10,	//^	$5e
	0x08,	//_	$5f
	0x01,	//'	$60
	0x02	//-	$61
};

//przrwanie
ISR(TIMER1_COMPA_vect)
{
	FLP_LCD_CL1; 
}

ISR(BADISR_vect){};

void SIout(char data) 					// Wysylanie bajtu danych przez SI
{
char c;

	while(bit_is_clear(LCDPIN, busy_pin)){}// czekaj dopoki wyswietlacz zajety
	_delay_us(250);
    for(c = 0x80; c > 0; c >>= 1) 		// wyslij wszystkie bity poczawszy od MSB
    {
		CLR_LCD_SCK;					// LOW -> /SCK
		if(data & c) 					
		{
			SET_LCD_SI;					// HIGH -> SI 
		}
		else 							
		{
			CLR_LCD_SI;					// LOW -> SI
		}
		_delay_us(10); 
		SET_LCD_SCK;					// HIGH -> /SCK
		_delay_us(10); 
	}
}//SIout

void Startx(char mode) 					// Ustawienie trybu transmisji (Command - /Data)
{
	CLR_LCD_CS;							// HIGH -> /CS  - podlacz kontroler LCD
    if(mode) 							// jesli mode > 0 - tryb command
    {
		SET_LCD_CD;						// HIGH -> C-/D - tryb komend
	}
    else
    {
		CLR_LCD_CD;						// LOW -> C-/D - tryb danych
	}
}//Startx
 
void Stoptx() 							// Zakonczenie transmisji przez SI
{
	while(bit_is_clear(LCDPIN, busy_pin)){}// czekaj na gotowosc wyswietlacza

	SET_LCD_CS;							// HIGH -> /CS - zwolnij kontroler
}

void SetDataPointer(char pos)			// ustawienie pozycji w pamieci wyswietlacza
{
    Startx(1); 							// tryb komend
    _delay_us(250);
    SIout(LOAD_DP | pos); 				// komenda LOAD_DP + pozycja
    Stoptx(); 							// zakoncz
}

void WriteData(char data) 				// zapisz dana do pamieci wyswietlacza  (i przesun pozycje)
{
    Startx(0);
    _delay_us(250); 					// tryb danych
    SIout(data); 						// wyslij dana
    Stoptx(); 							// zakoncz
}

//FUNKCJE DODATKOWE - czesciowe zapewnienie kompatybilnosci z HD44780

//wyslij komende do wyswietlacza
void LcdCommand(char comm)
{
	Startx(1);
	SIout(comm);
	Stoptx();
}

//procedura zapisu danej do wyswietlacza LCD sybmoli zpisanych w RAM
void lcd_putc(uint8_t index)
{
	WriteData(symbol_t[index]); 					// zapis do LCD
}
//procedura zapisu danej do wyswietlacza LCD sybmoli zpisanych w PGM
void lcd_putc_p(uint8_t index, uint8_t dot)
{
	if(dot) WriteData(pgm_read_byte(&symbol_t[index])|0x80);	//zapisz do LCD z kropka
	else WriteData(pgm_read_byte(&symbol_t[index])); 			// zapis do LCD bez kropki
}
//procedura zapisu danej do wyswietlacza LCD sybmoli zpisanych w EEPROM
void lcd_putc_e(uint8_t index)
{
	WriteData(eeprom_read_byte(&symbol_t[index]));	// zapis do LCD
}


// procedura zapisu liczby(tekstu) do wyswietlacza LCD z pamieci PGM
void write_text(char *s)
{
	while(*s) 	// do napotkania 0
	{
		uint8_t d;
		uint8_t dot=0;

		s++;
		if((*s)==0x2e) dot=1; 			//detekcja .
		s--;

		if(*s==0x20) d=0x10;			//detekcja spacji
		else if(*s==0x2d) d=0x31;			//detekcja -
		else d = (*s-'0');
		
		//zapisz znak wskazywany przez s na LCD
		lcd_putc_p(d,dot);
		//lcd_putc_e(d);
		//lcd_putc(d);
		if(dot)s++;		// przesun w stringu gdy wykryto kropke
		s++; 			// zwieksz s (przygotuj nastepny znak)
	}
}

//funkcje inicjalizujace wyswietlacz
void InitLcd() 							// inicjaliacja LCD
{
	LCDPORT	=	0b11011111;				//wejscie tylko dla sygnalu /BUSY
	LCDDDR 	= 	_BV(cs_pin) | _BV(si_pin) | _BV(sck_pin) | _BV(res_pin) | _BV(cd_pin) | _BV(cl1_pin);
		
	_delay_ms(100);
	CLR_LCD_CS;							// wybranie ukladu
    CLR_LCD_RES;						// LOW -> /RESET -  zerowanie układu
    _delay_ms(10);
    SET_LCD_RES;						// HIGH -> /RESET
    
    Startx(1); 							// tryb komend
	SIout(MODE_SET);					//tryb 
    SIout(WITHOUT_DECODER);				//tryb bez dekodowania znakow
    SIout(CLEAR_MEM);					//wyczysc pamiec LCD
    SIout(DISPLAY_ON);					//wlacz wyswietlacz
    Stoptx();							//zakoncz transmisje
}

