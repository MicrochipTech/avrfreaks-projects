#include <avr/io.h>
#include <avr/interrupt.h>

#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))

int iTempo[181]	=  {34286,34799,35295,35775,36240,36690,37127,37551,37963,38363,38751,39128,39495,39852,40199,40536,40865,41186,41498,41802,42099,42388,42671,42946,43215,
					43478,43734,43985,44230,44469,44703,44932,45156,45375,45590,45800,46005,46207,46404,46597,46786,46972,47154,47333,47508,47679,47848,48013,48175,48335,
					48491,48645,48795,48944,49089,49232,49373,49511,49647,49780,49911,50041,50168,50293,50416,50536,50656,50773,50888,51002,51113,51224,51332,51439,51544,
					51648,51750,51850,51950,52047,52144,52239,52332,52425,52516,52605,52694,52781,52868,52953,53036,53119,53201,53282,53361,53440,53517,53594,53669,53744,
					53818,53891,53962,54033,54104,54173,54241,54309,54376,54442,54507,54572,54635,54698,54761,54822,54883,54943,55003,55062,55120,55177,55234,55291,55346,
					55401,55456,55510,55563,55616,55668,55720,55771,55821,55872,55921,55970,56019,56067,56114,56161,56208,56254,56300,56345,56390,56435,56479,56522,56565,
					56608,56650,56692,56734,56775,56816,56856,56896,56936,56975,57014,57052,57091,57128,57166,57203,57240,57277,57313,57349,57384,57420,57455,57489,57524,
					57558,57592,57625,57658,57691,57724};

unsigned char znaki[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
int iLicz = 0, iNum = 2, iRazy = 4;
int jedn = 0, dzies = 2, setki = 1, metrum = 4;
char start=0, stan0 = 0, stan1 = 0;

void waitms(unsigned char x)
{
unsigned char a, b;
for(; x>0; --x)
	for(b=10; b>0; --b)
		for(a=25*4; a>0; --a)
			asm("nop");
}

SIGNAL(SIG_INTERRUPT0)
{
if(stan0 == 0)
{
	stan0 = 1;
	if(start == 0)
	{
		start = 1;
		int tempo = setki*100+dzies*10+jedn;
		TCNT1	= iTempo[tempo-60];
		TIMSK	= 0x05;
	}
	else
	{
		start	= 0;
		TIMSK	= 0x01;
		iRazy = metrum;
	}
	sei();
	waitms(150);
}
else
	stan0 = 0;
}

SIGNAL(SIG_INTERRUPT1)
{
if(stan1 == 0)
{
	stan1 = 1;
	metrum++;
	if(metrum > 4)
		metrum = 1;
	sei();
	waitms(150);
	while(bit_is_clear(PIND, 3))
	{
		metrum++;
		if(metrum > 4)
			metrum = 1;
		waitms(150);
	}
}
else
	stan1 = 0;
}

SIGNAL(SIG_OVERFLOW0)
{
	if(iLicz == 3)
	{
		if(iNum == 5)
			iNum=1;
		if(iNum == 1)
		{
			PORTC	= *(znaki+setki);
			PORTB	= 0xc8;
		}
		if(iNum == 2)
		{
			PORTC	= *(znaki+dzies);
			PORTB	= 0xc4;
		}
		if(iNum == 3)
		{
			PORTC	= *(znaki+jedn);
			PORTB	= 0xc2;
		}
		if(iNum == 4)
		{
			PORTC	= *(znaki+metrum);
			PORTB	= 0xc1;
		}
		iLicz	= 0;
		iNum++;
	}
	iLicz++;
	TCNT0	= 0x9C;
	sei();
}

SIGNAL(SIG_OVERFLOW1)
{
	int tempo = setki*100+dzies*10+jedn;
	TCNT1	= iTempo[tempo-60];
	sei();
	if(iRazy == metrum)
		sbi(PORTD, PD6);
	else
		sbi(PORTD, PD7);
	iRazy++;
	if(iRazy > metrum)
		iRazy = 1;
	waitms(50);
	cbi(PORTD, PD6);
	cbi(PORTD, PD7);
}

void sprgor()
{
	if(jedn > 9)
	{
		dzies++;
		jedn = 0;
	}
	if(dzies > 9)
	{
		setki++;
		dzies = 0;
	}
	int tempo = setki*100+dzies*10+jedn;
	if(tempo >= 240)
	{
		setki = 2;
		dzies = 4;
		jedn = 0;
	}
}

void sprdol()
{
	if(jedn < 0)
	{
		dzies--;
		jedn = 9;
	}
	if(dzies < 0)
	{
		setki--;
		dzies = 9;
	}
	int tempo = setki*100+dzies*10+jedn;
	if(tempo <= 60)
	{
		setki = 0;
		dzies = 6;
		jedn = 0;
	}
}

int main()
{
	DDRC	= 0x3f;

	DDRB	= 0xff;

	DDRD	= 0xff;
	PORTD	= 0x3f;

	TCCR0	= 0x03;
	TCNT0	= 0x9C;
	TIMSK	= 0x01;

	TCCR1B	= 0x04;

	MCUCR	= 0x05;
	GICR	= 0xC0;

	PORTB	= 0xc8;
	PORTC	= *(znaki+setki);

	sei();

	while(1)
	{
		if(bit_is_clear(PIND, 4))
		{
			jedn++;
			sprgor();
			waitms(150);
			while(bit_is_clear(PIND, 4))
			{
				jedn++;
				sprgor();
				waitms(150);
			}
		}
		if(bit_is_clear(PIND, 0))
		{
			jedn--;
			sprdol();
			waitms(150);
			while(bit_is_clear(PIND, 0))
			{
				jedn--;
				sprdol();
				waitms(150);
			}
		}
		if(bit_is_clear(PIND, 5))
		{

			dzies++;
			sprgor();
			waitms(150);
			while(bit_is_clear(PIND, 5))
			{
				dzies++;
				sprgor();
				waitms(150);
			}
		}
		if(bit_is_clear(PIND, 1))
		{

			dzies--;
			sprdol();
			waitms(150);
			while(bit_is_clear(PIND, 1))
			{
				dzies--;
				sprdol();
				waitms(150);
			}
		}
	}
}
