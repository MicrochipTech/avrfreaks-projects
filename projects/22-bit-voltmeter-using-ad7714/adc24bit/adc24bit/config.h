#define F_CPU 16000000UL
#define F_ADC 2457600L

// If true, read ADC values using INT0 pin change interrupt.
// DRDY must connect to PD2 if this is defined true.
#define ADC_USE_ISR 1

// Chip select pin for ADC
#define ADC_CS_BIT PB2
#define ADC_CS_PORT PORTB
#define ADC_CS_DDR DDRB

// Don't define these if ADC buffer pin is not connected to AVR
#define ADC_BUFFER_BIT PD3
#define ADC_BUFFER_PORT PORTD
#define ADC_BUFFER_DDR DDRD

// Latch pin for LCD SPI interface
#define LCD_CS_DDR DDRB
#define LCD_CS_PORT PORTB
#define LCD_CS_BIT PB1

// Rotary switch pins
#define ROTARY_PORT PORTD
#define ROTARY_DDR DDRD
#define ROTARY_PIN PIND
#define ROTARY1A_BIT PD4
#define ROTARY1B_BIT PD5
#define ROTARY2A_BIT PD6
#define ROTARY2B_BIT PD7
