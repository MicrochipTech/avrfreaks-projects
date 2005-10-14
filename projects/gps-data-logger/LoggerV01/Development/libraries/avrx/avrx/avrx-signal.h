#if !defined(__IAR_SYSTEMS_ICC__) && !defined(__IAR_SYSTEMS_ASM__)
//?MS?#include <avr\sig-avr.h>
#else

#ifndef _SIG_AVR_H_
#define _SIG_AVR_H_

// Use the GCC names so make conversion as seamless as possible
// NB THIS IS BROKEN FOR GCC 3.0 and above as the vector mechanism
// has changed  I don't know what IAR does.  If they match GCC
// then avrx_iar_vect.s needs to be changed to use the vectorX
// method

/* External Interrupt0 Function name */
#define SIG_INTERRUPT0 _interrupt0_

/* External Interrupt1 Function name */
#define	SIG_INTERRUPT1 _interrupt1_

/* External Interrupt2 Function name (ATmega) */
#define	SIG_INTERRUPT2 _interrupt2_

/* External Interrupt3 Function name (ATmega[16]03)*/
#define	SIG_INTERRUPT3 _interrupt3_

/* External Interrupt4 Function name (ATmega[16]03) */
#define	SIG_INTERRUPT4 _interrupt4_

/* External Interrupt5 Function name (ATmega[16]03) */
#define	SIG_INTERRUPT5 _interrupt5_

/* External Interrupt6 Function name (ATmega[16]03) */
#define	SIG_INTERRUPT6 _interrupt6_

/* External Interrupt7 Function name (ATmega[16]03) */
#define	SIG_INTERRUPT7 _interrupt7_

/* Output Compare2 Interrupt Function name */
#define SIG_OUTPUT_COMPARE2 _output_compare2_

/* Overflow2 Interrupt Function name */
#define	SIG_OVERFLOW2 _overflow2_

/* Input Capture1 Interrupt Function name */
#define SIG_INPUT_CAPTURE1 _input_capture1_

/* Output Compare1(A) Interrupt Function name */
#define SIG_OUTPUT_COMPARE1A _output_compare1a_

/* Output Compare1B Interrupt Function name */
#define	SIG_OUTPUT_COMPARE1B _output_compare1b_

/* Overflow1 Interrupt Function name */
#define	SIG_OVERFLOW1 _overflow1_

/* Output Compare0 Interrupt Function name */
#define SIG_OUTPUT_COMPARE0 _output_compare0_

/* Overflow0 Interrupt Function name */
#define	SIG_OVERFLOW0 _overflow0_

/* SPI Interrupt Function name */
#define	SIG_SPI _spi_

/* UART(0) Receive Complete Interrupt Function name */
#define	SIG_UART_RECV _uart_recv_

/* UART1 Receive Complete Interrupt Function name (ATmega161) */
#define	SIG_UART1_RECV _uart1_recv_

/* UART(0) Data Register Empty Interrupt Function name */
#define	SIG_UART_DATA _uart_data_

/* UART1 Data Register Empty Interrupt Function name (ATmega161) */
#define	SIG_UART1_DATA _uart1_data_

/* UART(0) Transmit Complete Interrupt Function name */
#define	SIG_UART_TRANS _uart_trans_

/* UART1 Transmit Complete Interrupt Function name (ATmega161) */
#define	SIG_UART1_TRANS _uart1_trans_

/* ADC Conversion complete */
#define	SIG_ADC _adc_

/* Eeprom ready */
#define	SIG_EEPROM _eeprom_ready_

/* Analog Comparator Interrupt Function name */
#define	SIG_COMPARATOR _comparator_

#endif /* _SIG_AVR_H_ */
#endif  /* __IAR_SYSTEMS_ICC__ */

