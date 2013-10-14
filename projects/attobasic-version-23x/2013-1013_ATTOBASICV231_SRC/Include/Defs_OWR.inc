;***************************************************************************
; OWR Constants
;***************************************************************************
.message "Enabling 1-Wire Support"

;****************************************************************
; One-Wire Port Pins and such
;****************************************************************
 #if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
 .if OWR
  .equ	OWRPIN	= PIND3			;Pin for OWR data pin
  .equ	OWRORT	= PORTD			;Port for OWR data pin
 .endif
 #elif defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)
 .if OWR
  .equ	OWRPIN	= PINB5				;Pin for OWR Data (use PCINT's)
  .equ	OWRORT	= PORTB				;Port for OWR data pin
 .endif
 #endif
;
