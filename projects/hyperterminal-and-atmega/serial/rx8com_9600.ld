LDmicro0.1
MICRO=Atmel AVR ATmega8 28-PDIP
CYCLE=5000
CRYSTAL=4000000
BAUD=4800
COMPILED=C:\Documents and Settings\simon\Desktop\serial\rx8com_4800.hex

IO LIST
    Yiesire at 4
    Yled1 at 5
    Yrel5 at 12
    Yrel6 at 13
    Yrel7 at 14
    Yrel8 at 15
    Yrosu at 6
    Yverde at 11
END

PROGRAM
RUNG
    UART_RECV receiv
END
RUNG
    EQU receiv 'a'
    OSR
    COIL Yiesire 0 1 0
END
RUNG
    EQU receiv 'b'
    OSR
    COIL Yiesire 0 0 1
END
RUNG
    CONTACTS Yiesire 0
    COIL Yverde 0 0 0
END
RUNG
    CONTACTS Yiesire 1
    COIL Yrosu 0 0 0
END
RUNG
    EQU receiv 'c'
    TOF Tc 100000
    COIL Yled1 0 0 0
END
RUNG
    EQU receiv 'd'
    OSR
    COIL Yrel5 0 0 0
END
RUNG
    EQU receiv 'e'
    OSR
    COIL Yrel6 0 0 0
END
RUNG
    EQU receiv 'f'
    OSR
    COIL Yrel7 0 0 0
END
RUNG
    EQU receiv 'g'
    OSR
    COIL Yrel8 0 0 0
END
