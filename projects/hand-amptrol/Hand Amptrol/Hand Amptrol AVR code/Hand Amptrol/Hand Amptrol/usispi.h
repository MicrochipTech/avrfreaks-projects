/*
 * usispi.h
 *
 * Created: 9/6/2014 5:34:40 PM
 *  Author: Mike
 */ 


#ifndef USISPI_H_
#define USISPI_H_
#define USI_PORT PORTB
#define USI_DDR DDRB
#define USI_DO PORTB1
#define USI_DI PORTB0
#define USI_CLK PORTB2
#define USI_SPI_CS1 PORTB5
#define USI_SPI_CS2 PORTB6
#define USI_CLK_Delay 50 // in microseconds for spi freq/2

/* Old code not using timer going to straight control of spi
#define USI_TIMER_CS (1<<CS10)
#define USI_TIMER_COUNT 80
*/

#define usi_putc(putc_in) usi_spi_out(USI_SPI_CS2,putc_in)

void usi_putv(uint16_t lcd_varible, uint8_t lcd_numtype, uint8_t lcd_digits );
void usi_spi_out(uint8_t usi_spi_cs, uint8_t usi_spi_out);
void usi_spi_init(void);

#endif /* USISPI_H_ */