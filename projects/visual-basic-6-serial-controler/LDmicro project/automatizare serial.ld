LDmicro0.1
MICRO=Atmel AVR ATmega8 28-PDIP
CYCLE=10000
CRYSTAL=16000000
BAUD=9600
COMPILED=D:\proiecte ldmicro\automat_serial.hex

IO LIST
    Yrel1 at 6
    Yrel2 at 11
    Yrel3 at 12
    Yrel4 at 13
    Yrel5 at 14
    Yrel6 at 16
    Yrel7 at 15
END

PROGRAM
RUNG
    UART_RECV receiv
END
RUNG
    PARALLEL
        EQU receiv '1'
        CONTACTS Rrl8 0
    END
    OSR
    COIL Yrel1 0 1 0
END
RUNG
    PARALLEL
        EQU receiv 'q'
        CONTACTS Rrl9 0
    END
    OSR
    COIL Yrel1 0 0 1
END
RUNG
    PARALLEL
        EQU receiv '2'
        CONTACTS Rrl8 0
    END
    OSR
    COIL Yrel2 0 1 0
END
RUNG
    PARALLEL
        EQU receiv 'w'
        CONTACTS Rrl9 0
    END
    OSR
    COIL Yrel2 0 0 1
END
RUNG
    PARALLEL
        EQU receiv '3'
        CONTACTS Rrl8 0
    END
    OSR
    COIL Yrel3 0 1 0
END
RUNG
    PARALLEL
        EQU receiv 'e'
        CONTACTS Rrl9 0
    END
    OSR
    COIL Yrel3 0 0 1
END
RUNG
    PARALLEL
        EQU receiv '4'
        CONTACTS Rrl8 0
    END
    OSR
    COIL Yrel4 0 1 0
END
RUNG
    PARALLEL
        EQU receiv 'r'
        CONTACTS Rrl9 0
    END
    OSR
    COIL Yrel4 0 0 1
END
RUNG
    PARALLEL
        EQU receiv '5'
        CONTACTS Rrl8 0
    END
    OSR
    COIL Yrel5 0 1 0
END
RUNG
    PARALLEL
        EQU receiv 't'
        CONTACTS Rrl9 0
    END
    OSR
    COIL Yrel5 0 0 1
END
RUNG
    PARALLEL
        EQU receiv '6'
        CONTACTS Rrl8 0
    END
    OSR
    COIL Yrel6 0 1 0
END
RUNG
    PARALLEL
        EQU receiv 'y'
        CONTACTS Rrl9 0
    END
    OSR
    COIL Yrel6 0 0 1
END
RUNG
    PARALLEL
        EQU receiv '7'
        CONTACTS Rrl8 0
    END
    OSR
    COIL Yrel7 0 1 0
END
RUNG
    PARALLEL
        EQU receiv 'u'
        CONTACTS Rrl9 0
    END
    OSR
    COIL Yrel7 0 0 1
END
RUNG
    EQU receiv '8'
    OSR
    COIL Rrl8 0 0 0
END
RUNG
    EQU receiv 'i'
    OSR
    COIL Rrl9 0 0 0
END
