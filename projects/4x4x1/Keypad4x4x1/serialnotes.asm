; Serial notes
	; These are the text the gadget says to the user

	; Beware - AVRs use 16-bit aligned addresses
	; And sometimes those compiler warnings mean a split
	; serial line might have an extra padded $00 where
	; you might not want it.

WELC_MSG:		; The Welcome message
.db "Welcome to the One-pin 4x4 Keypad project.  Rev. 0.1"
.db $0D, $00		; $0D is a carriage return, and $00 terminates

