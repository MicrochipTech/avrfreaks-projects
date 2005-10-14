;
; AVRX General interrupt table code
;
; This is a compatibility file to make IAR C compiler work similar to
; the GCC compiler.  As of 2.95 it worked great (2001) but with GCC 3.0
; and later this is broken.  GCC implements vectors explicitely in each
; header file (rather than a do-all table, like below).  The generic
; vector in GCC looks like _vector1_, _vector2_, ...
;
;
#if (((__TID__ >> 8) & 0x7F) != 90)
#error This file should only be assembled by aa90
#endif

#define A90_PROC_OPTION		((__TID__ >> 4) & 0x0F)

/* Long or relative jumps and calls */
#if (A90_PROC_OPTION == 0) || (A90_PROC_OPTION == 1)
#define XCALL	RCALL
#define XJMP	RJMP
#else
#define XCALL	CALL
#define XJMP	JMP
#endif

#include "avrx-io.h"	        // Parse to include proper table
#include "avrx-signal.h"        // Signal aliases

        MODULE  INTVEC_STUB
        PUBWEAK _stub_vector_
        PUBWEAK _interrupt0_
        PUBWEAK _interrupt1_
        PUBWEAK _interrupt2_
        PUBWEAK _interrupt3_
        PUBWEAK _interrupt4_
        PUBWEAK _interrupt5_
        PUBWEAK _interrupt6_
        PUBWEAK _interrupt7_
        PUBWEAK _output_compare2_
        PUBWEAK _overflow2_
        PUBWEAK _input_capture1_
        PUBWEAK _output_compare1a_
        PUBWEAK _output_compare1b_
        PUBWEAK _overflow1_
        PUBWEAK _output_compare0_
        PUBWEAK _overflow0_
        PUBWEAK _spi_
        PUBWEAK _uart_recv_
        PUBWEAK _uart1_recv_
        PUBWEAK _uart_data_
        PUBWEAK _uart1_data_
        PUBWEAK _uart_trans_
        PUBWEAK _uart1_trans_
        PUBWEAK _adc_
        PUBWEAK _eeprom_ready_
        PUBWEAK _comparator_

        RSEG    CODE
_stub_vector_:
        RSEG    CODE
_interrupt0_:
        RSEG    CODE
_interrupt1_:
        RSEG    CODE
_interrupt2_:
        RSEG    CODE
_interrupt3_:
        RSEG    CODE
_interrupt4_:
        RSEG    CODE
_interrupt5_:
        RSEG    CODE
_interrupt6_:
        RSEG    CODE
_interrupt7_:
        RSEG    CODE
_output_compare2_:
        RSEG    CODE
_overflow2_:
        RSEG    CODE
_input_capture1_:
        RSEG    CODE
_output_compare1a_:
        RSEG    CODE
_output_compare1b_:
        RSEG    CODE
_overflow1_:
        RSEG    CODE
_output_compare0_:
        RSEG    CODE
_overflow0_:
        RSEG    CODE
_spi_:
        RSEG    CODE
_uart_recv_:
        RSEG    CODE
_uart1_recv_:
        RSEG    CODE
_uart_data_:
        RSEG    CODE
_uart1_data_:
        RSEG    CODE
_uart_trans_:
        RSEG    CODE
_uart1_trans_:
        RSEG    CODE
_adc_:
        RSEG    CODE
_eeprom_ready_:
        RSEG    CODE
_comparator_:
        RETI
        ENDMOD

	NAME	AVRX_IAR_VECT
;	ASEG
	COMMON	INTVEC:CODE:ROOT(1)
	EXTERN	?C_STARTUP
        EXTERN _interrupt0_
        EXTERN _interrupt1_
        EXTERN _interrupt2_
        EXTERN _interrupt3_
        EXTERN _interrupt4_
        EXTERN _interrupt5_
        EXTERN _interrupt6_
        EXTERN _interrupt7_
        EXTERN _output_compare2_
        EXTERN _overflow2_
        EXTERN _input_capture1_
        EXTERN _output_compare1a_
        EXTERN _output_compare1b_
        EXTERN _overflow1_
        EXTERN _output_compare0_
        EXTERN _overflow0_
        EXTERN _spi_
        EXTERN _uart_recv_
        EXTERN _uart1_recv_
        EXTERN _uart_data_
        EXTERN _uart1_data_
        EXTERN _uart_trans_
        EXTERN _uart1_trans_
        EXTERN _adc_
        EXTERN _eeprom_ready_
        EXTERN _comparator_

;	ORG	$0
;	XJMP	?C_STARTUP
#ifdef INT0_vect
	ORG	INT0_vect
	XJMP	SIG_INTERRUPT0
#endif
#ifdef INT1_vect
	ORG	INT1_vect
	XJMP	SIG_INTERRUPT1
#endif
#ifdef INT2_vect
	ORG	INT2_vect
	XJMP	SIG_INTERRUPT2
#endif
#ifdef INT3_vect
	ORG	INT3_vect
	XJMP	SIG_INTERRUPT3
#endif
#ifdef INT4_vect
	ORG	INT4_vect
	XJMP	SIG_INTERRUPT4
#endif
#ifdef INT5_vect
	ORG	INT5_vect
	XJMP	SIG_INTERRUPT5
#endif
#ifdef INT6_vect
	ORG	INT6_vect
	XJMP	SIG_INTERRUPT6
#endif
#ifdef INT7_vect
	ORG	INT7_vect
	XJMP	SIG_INTERRUPT7
#endif
#ifdef TIMER2_COMP_vect
	ORG	TIMER2_COMP_vect
	XJMP	SIG_OUTPUT_COMPARE2
#endif
#ifdef TIMER2_OVF_vect
	ORG	TIMER2_OVF_vect
	XJMP	SIG_OVERFLOW2
#endif
#ifdef TIMER1_CAPT1_vect
	ORG	TIMER1_CAPT1_vect
	XJMP	SIG_INPUT_CAPTURE1
#endif
#ifdef TIMER1_COMPA_vect
	ORG	TIMER1_COMPA_vect
	XJMP	SIG_OUTPUT_COMPARE1A
#endif
#ifdef TIMER1_COMPB_vect
	ORG	TIMER1_COMPB_vect
	XJMP	SIG_OUTPUT_COMPARE1B
#endif
#ifdef TIMER1_OVF1_vect
	ORG	TIMER1_OVF1_vect
	XJMP	SIG_OVERFLOW1
#endif
#ifdef TIMER0_COMP_vect
	ORG	TIMER0_COMP_vect
	XJMP	SIG_OUTPUT_COMPARE0
#endif
#ifdef TIMER0_OVF0_vect
	ORG	TIMER0_OVF0_vect
	XJMP	SIG_OVERFLOW0
#endif
#ifdef SPI_STC_vect
	ORG	SPI_STC_vect
	XJMP	SIG_SPI
#endif
#ifdef UART_RX_vect
	ORG	UART_RX_vect
	XJMP	SIG_UART_RECV
#endif
#ifdef UART_UDRE_vect
	ORG	UART_UDRE_vect
	XJMP	SIG_UART_DATA
#endif
#ifdef UART_TX_vect
	ORG	UART_TX_vect
	XJMP	SIG_UART_TRANS
#endif
/* UART0_... is an alias for UART_... as IAR redefines */
/* the name for the at94k FPGA */
#ifdef UART0_RX_vect
	ORG	UART0_RX_vect
	XJMP	SIG_UART_RECV
#endif
#ifdef UART0_UDRE_vect
	ORG	UART0_UDRE_vect
	XJMP	SIG_UART_DATA
#endif
#ifdef UART0_TX_vect
	ORG	UART0_TX_vect
	XJMP	SIG_UART_TRANS
#endif
#ifdef UART1_RX_vect
	ORG	UART1_RX_vect
	XJMP	SIG_UART1_RECV
#endif
#ifdef UART1_UDRE_vect
	ORG	UART1_UDRE_vect
	XJMP	SIG_UART1_DATA
#endif
#ifdef UART1_TX_vect
	ORG	UART1_TX_vect
	XJMP	SIG_UART1_TRANS
#endif
#ifdef ADC_vect
	ORG	ADC_vect
	XJMP	SIG_ADC
#endif
#ifdef EE_RDY_vect
	ORG	EE_RDY_vect
	XJMP	SIG_EEPROM
#endif
#ifdef ANA_COMP_vect
	ORG	ANA_COMP_vect
	XJMP	SIG_COMPARATOR
#endif

        END
