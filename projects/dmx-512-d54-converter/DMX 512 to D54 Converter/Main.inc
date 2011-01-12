; ####################################################################################################### ;
; #
; # © Portia CyberLogic.
; # www.portia-cyberlogic.com
; # mail@portia-cyberlogic.com
; # 
; ####################################################################################################### ;
; #
; # PROJECT INFORMATION:
; # Project	 name:		DMX512 to D54 converter.
; # Project version:	0.9.0.0
; #
; # File name:		Main.inc
; # File title:		Firmware main module include.
; # Target MCU:		ATmega32
; # Clock freq.:	Check MCU_CLOCK_FREQ below.
; # File version:	1.0.0.0
; # Last modefied:	2010.12.09
; #
; # FILE DESCRIPTION:
; # Firmware main module include.
; #
; # FILE HISTORY:
; #
; # 2010.12.01
; # File creation.
; # By: Mahmoud Taher, portia@portia-cyberlogic.com
; #
; ####################################################################################################### ;

#ifndef _MAIN_INC_
#define _MAIN_INC_

; ####################################################################################################### ;
; # Includes
; ####################################################################################################### ;

.include <m32def.inc>

; ####################################################################################################### ;
; # General.
; ####################################################################################################### ;

;Strings..

#define T_FW_VER "0.9.0.0" ; Firmware version.
#define T_FW_VER_SIZE strlen(T_FW_VER) + 1

#define T_FW_AUTHER "By: Mahmoud Taher" ; Auther.
#define T_FW_AUTHER_SIZE strlen(T_FW_AUTHER) + 1

#define T_FW_COPYRIGHT "(C) Portia CyberLogic" ; Copyright.
#define T_FW_COPYRIGHT_SIZE strlen(T_FW_COPYRIGHT) + 1

#define T_FW_WWW "www.portia-cyberlogic.com" ; Auther.
#define T_FW_WWW_SIZE strlen(T_FW_WWW) + 1

; Common prerequest..
#define MCU_CLOCK_FREQ 16000000 ; MCU clock frequancy (Hz).

;Stack defs.
#define STACK_SIZE 64 ; Stack size in bytes.
#define STACK_BASE (SRAM_SIZE + STACK_SIZE) ; Stack pointer base address.

; USART prerequests..
#define USART_BAUD_RATE 38400 ; USART baud rate (BPS).

;Status LED..
#define LED_S_PORT PORTC
#define LED_S_DDR_REG DDRC
#define LED_S_DDR_BIT DDC0
#define LED_S_PIN PINC0

;General flags (Flags register)..
#define F_DMX_OK 0 ;DMX frame recieve OK.
#define F_DMX_TEST 6 ;DMX Test mode.
#define F_D54_TEST 7 ;D54 Test mode.

; ####################################################################################################### ;
; # DMX defs.
; ####################################################################################################### ;

;DMX USART UBR value..
.equ DMX_UBBER_VALUE = INT((MCU_CLOCK_FREQ / (16 * 250000)) - 0.5) ; UBBER value.

;DMX data..
#define DMX_BUFFER_SIZE 513; DMX buffer size.

; ####################################################################################################### ;
; # D54 defs.
; ####################################################################################################### ;

;D54 data..
#define D54_CH_MAX (384) ;D54 max. chennels (384).
#define D54_TIME_SAMPLE 4 ;D54 time sample = 4 us.
#define D54_TIMER_CTC_VAL (D54_TIME_SAMPLE * (MCU_CLOCK_FREQ / 1000000)) ;D54 timer value.

; Time slices.
#define D54_T_START (11) ;START signal = '> 43us' , so T = 43us / D54_TIME_SAMPLE = 10.75 use 11..
#define D54_T_SYNC (3) ;SYNC signal = '10us' , so T = 10us / D54_TIME_SAMPLE = 2.5.
#define D54_T_BYTE (13) ;BYTE signal = '> 50us' , so T = 50us / D54_TIME_SAMPLE = 12.5 use 13..
#define D54_T_EOF (13) ;EOF signal = '> 50us' , so T = 50us / D54_TIME_SAMPLE = 12.5 use 13..

; D54 data port..
#define D54_DATA_PORT PORTA
#define D54_DATA_DDR DDRA
#define D54_DATA_PIN PINA

;D54 sync pin..
#define D54_SYNC_PORT PORTC
#define D54_SYNC_DDR DDRC
#define D54_SYNC_DDR_BIT DDC7
#define D54_SYNC_PIN PINC7

;D54 state..
#define D54_S_DEFAULT 1 ;Idle
#define D54_S_START 2 ;Start
#define D54_S_SYNC 3 ;Sync pulse.
#define D54_S_BYTE 4 ;Channel data.
#define D54_S_EOF 5 ;End Of Frame.

; ####################################################################################################### ;
; # EOF
; ####################################################################################################### ;

#endif
