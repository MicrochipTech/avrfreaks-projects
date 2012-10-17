/*
 * LA_JMP_EXMPL.c
 *
 * Created: 17.10.2012 11:47:25
 *  Author: Григорий
 *
 * Пример использования макросов la_jmp 
 * Включается led_PK затем два раза мигает led_OW и гаснет led_PK
 *
 * In the example show how to use la_jmp macroses. 
 * Program loop is led_PK switch on then double toggle led_OW and switch off led_PK
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "Sys.h"
#include "la_jmp.h"

void mega128_init(void);
void led_tgl(la_str_jmp *i_jmp);

#define set_led_MK PORTA |= (0x01 << PORTA3);
#define clr_led_MK PORTA &= ~(0x01 << PORTA3);
#define tgl_led_MK PORTA ^= (0x01 << PORTA3);
#define set_led_PK PORTA |= (0x01 << PORTA4);
#define clr_led_PK PORTA &= ~(0x01 << PORTA4);
#define tgl_led_PK PORTA ^= (0x01 << PORTA4);
#define set_led_OW PORTA |= (0x01 << PORTA5);
#define clr_led_OW PORTA &= ~(0x01 << PORTA5);
#define tgl_led_OW PORTA ^= (0x01 << PORTA5);

volatile unsigned char t_flag = 0;
volatile unsigned int m_timer = 0;
ISR(TIMER2_COMP_vect) {
	t_flag++;
	m_timer++;
};

la_str_jmp m_pro;
la_str_jmp i_pro;

int main(void)
{
    mega128_init();
    m_pro.jmp = 0;
    m_pro.timer = &m_timer;
	i_pro.jmp = 0;
	i_pro.timer = &m_timer;
    SEI;
		
    while(1)
    {
		wdt_reset();
		
		if (t_flag) {
			t_flag = 0;
			static unsigned int div1Hz = 0;
			if (div1Hz++ > 499) {
				div1Hz = 0;
				tgl_led_MK;
			} // 1Hz
		} // t_flag
		
		la_c_jmp((&m_pro));
		la_jmp_time(ij_m_lab_1, 499, (&m_pro), goto m_lab_exit);
		set_led_PK;

		la_jmp_before_func(l_td_1, (&m_pro));
		led_tgl((&i_pro));
		la_jmp_after_func((&m_pro), (&i_pro), goto m_lab_exit);

		la_jmp_time(ij_m_lab_2, 499, (&m_pro), goto m_lab_exit);
		clr_led_PK;
		m_lab_exit:;						
    }
}

void led_tgl(la_str_jmp *f_pro) {
	la_c_jmp(f_pro);
	la_jmp_time(l_td_t1, 499, f_pro, return);
	set_led_OW;
	la_jmp_time(l_td_t2, 499, f_pro, return);
	clr_led_OW;

	*f_pro->timer = 0;
	la_save(f_pro->jmp ,l_td_t3);
	la_set(l_td_t3);
	if (*f_pro->timer < 499) {
		return;
	}		
	f_pro->jmp = 0;
	set_led_OW;

	*f_pro->timer = 0;
	la_save(f_pro->jmp ,l_td_t4);
	la_set(l_td_t4);
	if (*f_pro->timer < 499) {
		return;
	}
	f_pro->jmp = 0;		
	clr_led_OW;	
}

 /*
 unsigned short GetPC()
 {
	 unsigned char *ptr=(unsigned char *)SP;
	 ptr+=2;
	 unsigned short result=*ptr--;
	 result+=(*ptr)<<8;
	 
	 return result;
 }
 */

void mega128_init(void) {
		CLI; // запрет прерываний

/////// Настрйка делителя частоты (для сигналов SckI/O, SckADC, SckCPU, SckFlash)
// XDIV      XDIVEN  XDIV6 .. XDIV0
//           XDIVEN - включение делителя 
                //XDIV = 0;
///////
/*
Специальный регистр управления
SFIOR	TSM  -  -  -  ACME  PUD  PSR0  PSR321	
        0       0       0       0       0       0       0       0
        TSM включение синхронного режима таймеров
        PSR0 при включении TSM блокируется сброс (hardware) счетчика таймера 0
             для корректного изменение настоек или содержимого счетчика
        PSR321  то же что и PSR0 но одновременно для таймеров 3, 2, 1
	PUD отключения поддтягивания всех портов к +питания, независимо от 
		их конфигурации
	ACME разрешение мультиплексирования инверсного входа аналогового
		компаратора. Сигнал подается с выхода аналогового мультиплексора АЦП
*/
		SFIOR = 0; 

// MCUCR     SRE  SRW10  SE  SM1  SM0  SM2  IVSEL IVCE
//           SRE - включение интерфейса внешней памяти
//           SRW10 - управление скоростью интерфейса внешней памяти (см. XMCRA)
//           SE  - включение спящего режима
//           SM2  SM1  SM0 Sleep Mode
//            0    0    0   Idle
//            0    0    1   ADC noise reduction
//            0    1    0   Power-Down
//            0    1    1   Power-Save
//            1    0    0   reserved
//            1    0    1   reserved
//            1    1    0   Standby
//            1    1    1   Extended Standby
//            IVSEL - выбор размещения векторов прерываний (flash or boot loader)
//            IVCE - переключение размещения векторов прерываний
                MCUCR = 0;

// MCUSCR    JTD  -  -  JTRF  WDRF  BORF  EXTRF  PORF
//           биты 0..4 флаги сбросов МК
//           JTD - при 0 включен JTAG порт при 1 откючен
                MCUCSR  = 0;

// Настройка Таймера0 
// TCCR0	FOC0	WGM00	COM01	COM00	WGM01	CS02	CS01	CS00
//	        0 W	0	0	0	0	0	0	0
                TCCR0 = 0;
                
                OCR0 = 0;
// TCNT0 - Timer 0 counter
// OCR0 - Compare register Timer 0
// ASSR asinchronous mode control register
// ASSR    -  -  -  -  AS0  TCN0UB  OCR0UB  TCR0UB
// See also SFIOR, PSR0 bit


// Настройка Таймера2 
// TCCR2	FOC2	WGM20	COM21	COM20	WGM21	CS22	CS21	CS20
//	        0 W	0	0	0	0	0	0	0
                TCCR2 = (0x01 << WGM21) | (0x01 << CS22) ;//| (0x01 << CS20);
                
                OCR2 = 29; //7,37MHz/256/29 - 1000Hz  // 7,37MHz/1024/72 - 100Hz  7,37MHz/1 74 - 100kHz // 150 - 50kHz
/*
        CS02    CS01    CS00
        0       0       0       Timer0 Stop
        0       0       1       CK
        0       1       0       CK/8
        0       1       1       CK/64
        1       0       0       CK/256
        1       0       1       CK/1024
        1       1       0       External Pin T0, falling adge
        1       1       1       External Pin T0, rising adge
*/
// TCNT2 - Timer 2 counter
// OCR2 - Compare register Timer 2


// Настройка Таймера1 
// OCR1AH, OCR1AL
			OCR1A = 0; 
// TCCR1A	COM1A1	COM1A0	COM1B1	COM1B0	COM1C1	COM1C0	WGM11	WGM10
//	        0 W	0	0	0	0	0	0	0
			TCCR1A = 0;
// TCCR1B	ICNC1	ICES1	-	WGM13	WGM12	CS12	CS11	CS10
//	        0 W	0	0	0	0	0	0	0
                TCCR1B = 0;//(0x01 << WGM12) | 0x04; // /256
				//TCCR1B = 0;
// TCCR1C	FOC1A s FOC1B  FOC1C  -  -  -  -  - 
//	        0 W	0	0	0	0	0	0	0
                TCCR1C = 0;
                


//////// Настройка прерываний таймеров
// TIMSK  MSB    OCIE2  TOIE2  TICIE1  OCIE1A  OCIE1B  TOIE1  OCIE0  TOIE0
//               0      0      0       0       0       0      0      0
		TIMSK = (0x01 << OCIE2);
// TIFR  
////////

//////// Настройка АЦП
/*
ADMUX	REFS1	REFS0	ADLAR	MUX4	MUX3	MUX2	MUX1	MUX0
        0       0       0       0       0       0       0       0
	REFS1	REFS0 выбор опорного напряжения
		REFS1	REFS0
		0	0	Внутренний опорник отключен
		0	1	AVCC с конденсатором на выводе AREF
		1	0	Reserved
		1	1	Внутренне опорное наапряжение 2.56В подаётся
				на вывод AREF
	ADLAR выбор комбинации битов АЦП при записи в выходной регистр
		при 0
		ADCH   -     -    -    -    -    -  ADC9 ADC8
		ADCL  ADC7 ADC6 ADC5 ADC4 ADC3 ADC2 ADC1 ADC0
		при 1
		ADCH  ADC9 ADC8 ADC7 ADC6 ADC5 ADC4 ADC3 ADC2
		ADCL  ADC1 ADC0   -     -    -    -    -    -
	MUX4	MUX3	MUX2	MUX1	MUX0 выбор входа АЦП	
		MUX4	MUX3	MUX2	MUX1	MUX0	Вывод
		0	0	0	0	0	ADC0
		0	0	0	0	1	ADC1
		0	0	0	1	0	ADC2
		0	0	0	1	1	ADC3
		0	0	1	0	0	ADC4
		0	0	1	0	1	ADC5
		0	0	1	1	0	ADC6
		0	0	1	1	1	ADC7
*/
		ADMUX = 0;
/*
ADCSRA	ADEN	ADSC	ADATE	ADIF	ADIE	ADPS2	ADPS1	ADPS0
        0       0       0       0       0       0       0       0
	ADEN Включение АЦП. При 1 АЦП включен.
	ADSC Разрешение начала преобразования. При одиночных выборках
		каждый раз устаналивать в 1. В режиме Free Running Mode
		(регистр SFIOR) записать 0.
	ADATE Включение режима автостарта по событию определленному в ADCSRB
	ADIF флаг прерывания АЦП. устанавливается в 1 при модификации выходного
		регистра (ADCH и ADCL). 
	ADIE при 1 разрешено выполнение прерывания по завершения АЦП 
		преобразования
	ADPS2	ADPS1	ADPS0 управление предделителем тактов АЦП. Частота 
		кварцевого резонатора от XTAL делиться на заданное чило.
		ADPS2	ADPS1	ADPS0	Делитель
		0	0	0	2
		0	0	1	2
		0	1	0	4
		0	1	1	8
		1	0	0	16
		1	0	1	32
		1	1	0	64 V for 4MHz time conv. 208mks/4807Hz
		1	1	1	128 4807kHz for 8MHz
*/
		ADCSRA = 0;

//end ////// Настройка АЦП


/* /////// Init. Ports 
		DDRx - Type Register 0-input 1-output
	DDRx	PORTx	I/O	Pull-Up	Comment
	0	0	input	no	Tri-State (High-Z)	
	0	1	input	yes	Подтянут к +Vcc через резистор 2кОм
	1	0	output	no	Логический 0
	1	1	output	no	логическая 1
 Фактический сигнал от порта можно узнать прочитав регистр PINx */
		PORTA = 0;
	    DDRA = 0x38;

		PORTB = 0;
		DDRB = 0;

 		PORTC = 0;
		DDRC = 0;

		PORTD = 0;
		DDRD = 0x0;

		PORTE = 0;
		DDRE = 0;

		PORTF = 0;
		DDRF = 0;
				
		PORTG = 0;
		DDRG = 0;
//////// 

//UART
//SFR_B_R(0x0C,UDR)
//SFR_B_N(0x0B,UCSRA, RXC,TXC,UDRE,FE,DOR,PE,U2X,MPCM)
                UCSR0A = 0;
                UCSR1A = 0;
//SFR_B_N(0x0A,UCSRB,  RXCIE,TXCIE,UDRIE,RXEN,TXEN,UCSZ2,RXB8,TXB8)
              //  UCSR0B =  (0x01 << TXEN1);
                UCSR1B =  0;//(0x01 << TXEN1) | (0x01 << RXEN1) | (0x01 << RXCIE1); //(0x01 << TXEN) | (0x01 << UDRIE) |
//UCSRC -,UMSEL,UPM1,UPM0,USBS,UCSZ1,UCSZ0,UCPOL)
             //   UCSR0C = (0x01 << UCSZ10) | (0x01 << UCSZ00);
                UCSR1C = 0;//(0x01 << UCSZ11) | (0x01 << UCSZ10);

				// 11 - 38400, 47-9600, 95 - 4800
			//	UBRR0L = 11;
			//	UBRR1L = 47;
	
	wdt_enable(WDTO_500MS);

// SREG     I   T   H   S   V   N   Z   C
//              ldi R16, 0x80
//                out _SFR_IO_ADDR(SREG), R16 // Enable all interrupt
}