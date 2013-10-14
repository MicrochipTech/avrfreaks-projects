;***************************************************************
; This file is currently unused
;***************************************************************
; The TX, RX_P0 and RX_P1 MAC addresses can be defined here.
;	For ease of implementation, a 5-character text string is
;	used.
;***************************************************************

#define STR2DATA(X)	.db #X, 0		;stringification !

; define a table of default MAC addresses
RF_TX_ADDR:
	STR2DATA(SERV1)			;define a 5-character MAC address for TX

RF_RX_ADDR_P0:
	STR2DATA(SERV1)			;define a 5-character MAC address for RX pipe 0

RF_RX_ADDR_P1:
	STR2DATA(CLIE1)			;define a 5-character MAC address for RX pipe 1
