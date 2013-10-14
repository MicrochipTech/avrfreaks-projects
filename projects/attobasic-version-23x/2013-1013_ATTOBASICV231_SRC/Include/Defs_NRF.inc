;*********************************************************************
; nRF24L01 definitions
;	Portions (C) 2012 Mike McCauley, NRF library for ARDUINO
;*********************************************************************
;
 #if ( defined(__ATmega88__) || \
	defined(__ATmega88PA__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) )
	.equ	NRFCE	= PORTB0		;Pin for nRF24's CE pin
	.equ	NRFPORT	= PORTB			;Port for nRF24's CE pin
;
 #elif defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__)
	.equ	NRFCE	= PORTB4		;Pin for nRF24's CE pin
	.equ	NRFPORT	= PORTB			;Port for nRF24's CE pin
;
#endif
;
;=======================================================================================
; Keep track of the mode the NRF24 is in
.equ	NRF24_MODE_IDLE				= 0
.equ	NRF24_MODE_RX				= 1
.equ	NRF24_MODE_TX				= 2

; nRF24L01 Command names
.equ	NRF24_CMD_R_REGISTER		= 0x00
.equ	NRF24_CMD_W_REGISTER		= 0x20
.equ	NRF24_CMD_R_RX_PAYLOAD		= 0x61
.equ	NRF24_CMD_W_TX_PAYLOAD		= 0xa0
.equ	NRF24_CMD_FLUSH_TX			= 0xe1
.equ	NRF24_CMD_FLUSH_RX			= 0xe2
.equ	NRF24_CMD_REUSE_TX_PL		= 0xe3
.equ	NRF24_CMD_R_RX_PL_WID		= 0x60
.equ	NRF24_CMD_W_ACK_PLD			= 0xa8
.equ	NRF24_CMD_W_TX_PLD_NACK		= 0xb0
.equ	NRF24_CMD_NOP				= 0xff

; Register names
.equ	NRF24_REG_MASK				= 0x1f
.equ	NRF24_R00_CFG				= 0x00
.equ	NRF24_R01_EN_AA				= 0x01
.equ	NRF24_R02_EN_RXADDR			= 0x02
.equ	NRF24_R03_SETUP_AW			= 0x03
.equ	NRF24_R04_SETUP_RETR		= 0x04
.equ	NRF24_R05_RF_CH				= 0x05
.equ	NRF24_R06_RF_SETUP			= 0x06
.equ	NRF24_R07_STAT				= 0x07
.equ	NRF24_R08_OBSERVE_TX		= 0x08
.equ	NRF24_R09_RPD				= 0x09
.equ	NRF24_R0A_RX_ADDR_P0		= 0x0a
.equ	NRF24_R0B_RX_ADDR_P1		= 0x0b
.equ	NRF24_R0C_RX_ADDR_P2		= 0x0c
.equ	NRF24_R0D_RX_ADDR_P3		= 0x0d
.equ	NRF24_R0E_RX_ADDR_P4		= 0x0e
.equ	NRF24_R0F_RX_ADDR_P5		= 0x0f
.equ	NRF24_R10_TX_ADDR			= 0x10
.equ	NRF24_R11_RX_PW_P0			= 0x11
.equ	NRF24_R12_RX_PW_P1			= 0x12
.equ	NRF24_R13_RX_PW_P2			= 0x13
.equ	NRF24_R14_RX_PW_P3			= 0x14
.equ	NRF24_R15_RX_PW_P4			= 0x15
.equ	NRF24_R16_RX_PW_P5			= 0x16
.equ	NRF24_R17_FIFO_STAT			= 0x17
.equ	NRF24_R1C_DYNPD				= 0x1c
.equ	NRF24_R1D_FEAT				= 0x1d

; These register masks etc are named wherever possible
; corresponding to the bit and field names in the nRF24L01 Product Specification

;	NRF24_REG_00_CONFIG 0x00
.equ	NRF24_MSK_RX_DR				= 0x40
.equ	NRF24_MSK_TX_DS				= 0x20
.equ	NRF24_MSK_MAX_RT			= 0x10
.equ	NRF24_EN_CRC				= 0x08
.equ	NRF24_CRCO					= 0x04
.equ	NRF24_PWR_UP				= 0x02
.equ	NRF24_PRIM_RX				= 0x01
.equ	NRF24_REG_00_DEF			= NRF24_EN_CRC	; Reg00 defaults

;	NRF24_REG_01_EN_AA 0x01
.equ	NRF24_ENAA_P5				= 0x20
.equ	NRF24_ENAA_P4				= 0x10
.equ	NRF24_ENAA_P3				= 0x08
.equ	NRF24_ENAA_P2				= 0x04
.equ	NRF24_ENAA_P1				= 0x02
.equ	NRF24_ENAA_P0				= 0x01
.equ	NRF24_REG_01_DEF			= ( NRF24_ENAA_P5 | \
					 					NRF24_ENAA_P4 | \
					 					NRF24_ENAA_P3 | \
					 					NRF24_ENAA_P2 | \
					 					NRF24_ENAA_P1 | \
					 					NRF24_ENAA_P0 )	; Reg01 defaults

;	NRF24_REG_02_EN_RXADDR 0x02
.equ	NRF24_ERX_MASK				= 0x3f				;mask to pipe enable bits
.equ	NRF24_ERX_P5				= 0x20
.equ	NRF24_ERX_P4				= 0x10
.equ	NRF24_ERX_P3				= 0x08
.equ	NRF24_ERX_P2				= 0x04
.equ	NRF24_ERX_P1				= 0x02
.equ	NRF24_ERX_P0				= 0x01
.equ	NRF24_REG_02_DEF			= ( NRF24_ERX_P1 | \
					 					NRF24_ERX_P0 )	; Reg02 defaults

;	NRF24_REG_03_SETUP_AW 0x03
.equ	NRF24_AW_3_BYTES			= 0x01
.equ	NRF24_AW_4_BYTES			= 0x02
.equ	NRF24_AW_5_BYTES			= 0x03
.equ	NRF24_REG_03_DEF			= NRF24_AW_5_BYTES	; Reg03 defaults

;	NRF24_REG_04_SETUP_RETR 0x04
.equ	NRF24_ARD					= 0xf0
.equ	NRF24_ARC					= 0x0f
.equ	NRF24_REG_04_DEF			= 3					; Reg04 defaults

;	NRF24_REG_05_RF_CH 0x05
.equ	NRF24_RF_CH					= 0x7f
.equ	NRF24_REG_05_DEF			= 2					; Reg05 defaults

;	NRF24_REG_06_RF_SETUP 0x06
.equ	NRF24_CONT_WAVE				= 0x80
.equ	NRF24_RF_DR_LOW				= 0x20
.equ	NRF24_PLL_LOCK				= 0x10
.equ	NRF24_RF_DR_HIGH			= 0x08
.equ	NRF24_PWR					= 0x06
.equ	NRF24_PWR_m18dBm			= 0x00
.equ	NRF24_PWR_m12dBm			= 0x02
.equ	NRF24_PWR_m6dBm				= 0x04
.equ	NRF24_PWR_0dBm				= 0x06
.equ	NRF24_REG_06_DEF			= ( NRF24_RF_DR_HIGH | \
					 					NRF24_PWR_0dBm ); Reg06 defaults

;	NRF24_REG_07_STATUS 0x07
.equ	NRF24_RX_DR					= 0x40
.equ	NRF24_TX_DS					= 0x20
.equ	NRF24_MAX_RT				= 0x10
.equ	NRF24_RX_P_NO				= 0x0e
.equ	NRF24_STATUS_TX_FULL		= 0x01

;	NRF24_REG_08_OBSERVE_TX 0x08
.equ	NRF24_PLOS_CNT				= 0xf0
.equ	NRF24_ARC_CNT				= 0x0f

;	NRF24_REG_09_RPD 0x09
.equ	NRF24_RPD					= 0x01

;	NRF24_R0A_RX_ADDR_P0 0x0a
.equ	RX_ADDR_P0					= 0xE7			;OEM default (5 bytes repeating)

;	NRF24_R0B_RX_ADDR_P1 0x0b
.equ	RX_ADDR_P1					= 0xC2			;OEM default (5 bytes repeating)

;	NRF24_R0C_RX_ADDR_P2 0x0c
.equ	RX_ADDR_P2					= 0xC3			;OEM default

;	NRF24_R0D_RX_ADDR_P3 0x0d
.equ	RX_ADDR_P3					= 0xC4			;OEM default

;	NRF24_R0E_RX_ADDR_P4 0x0e
.equ	RX_ADDR_P4					= 0xC5			;OEM default

;	NRF24_R0F_RX_ADDR_P5 0x0f
.equ	RX_ADDR_P5					= 0xC6			;OEM default

;	NRF24_R10_TX_ADDR	 0x10
.equ	TX_ADDR						= 0xE7			;OEM default (5 bytes repeating)

;	NRF24_R11_RX_PW_P0	 0x11
.equ	NRF24_R11_RX_PW_P0_DEF		= 0x00			;OEM default

;	NRF24_R12_RX_PW_P0	 0x12
.equ	NRF24_R12_RX_PW_P1_DEF		= 0x00			;OEM default

;	NRF24_R13_RX_PW_P0	 0x13
.equ	NRF24_R13_RX_PW_P2_DEF		= 0x00			;OEM default

;	NRF24_R14_RX_PW_P0	 0x14
.equ	NRF24_R14_RX_PW_P3_DEF		= 0x00			;OEM default

;	NRF24_R15_RX_PW_P0	 0x15
.equ	NRF24_R15_RX_PW_P4_DEF		= 0x00			;OEM default

;	NRF24_R16_RX_PW_P0	 0x16
.equ	NRF24_R16_RX_PW_P5_DEF		= 0x00			;OEM default

;	NRF24_REG_17_FIFO_STATUS 0x17
.equ	NRF24_TX_REUSE				= 0x40
.equ	NRF24_TX_FULL				= 0x20
.equ	NRF24_TX_EMPTY				= 0x10
.equ	NRF24_RX_FULL				= 0x02
.equ	NRF24_RX_EMPTY				= 0x01

;	NRF24_REG_1C_DYNPD 0x1c
.equ	NRF24_DPL_P5				= 0x20
.equ	NRF24_DPL_P4				= 0x10
.equ	NRF24_DPL_P3				= 0x08
.equ	NRF24_DPL_P2				= 0x04
.equ	NRF24_DPL_P1				= 0x02
.equ	NRF24_DPL_P0				= 0x01
.equ	NRF24_REG_1C_DEF			= 0					; Reg1C defaults

;	NRF24_REG_1D_FEATURE 0x1d
.equ	NRF24_EN_DPL				= 0x04
.equ	NRF24_EN_ACK_PAY			= 0x02
.equ	NRF24_EN_DYN_ACK			= 0x01
.equ	NRF24_REG_1D_DEF			= 0					; Reg1D defaults
