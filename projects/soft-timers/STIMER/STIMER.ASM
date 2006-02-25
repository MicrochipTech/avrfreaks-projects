.CSEG

;************************************************************************
;*  RESPONSIBLE ROUTINES FOR SOFT TIMER EVENTS
;************************************************************************


ON_STIMER1:
    SBI     PORTB,1
    CBR     STIMER_OV_FLAGS,(1<<_STIMER1_OV)
    RET

ON_STIMER2:

    CBR     STIMER_OV_FLAGS,(1<<_STIMER2_OV)
    RET

ON_STIMER3:

    CBR     STIMER_OV_FLAGS,(1<<_STIMER3_OV)
    RET

ON_STIMER4:

    CBR     STIMER_OV_FLAGS,(1<<_STIMER4_OV)
    RET

ON_STIMER5:

    CBR     STIMER_OV_FLAGS,(1<<_STIMER5_OV)
    RET

ON_STIMER6:

    CBR     STIMER_OV_FLAGS,(1<<_STIMER6_OV)
    RET

ON_STIMER7:

    CBR     STIMER_OV_FLAGS,(1<<_STIMER7_OV)
    RET

ON_STIMER8:

    CBR     STIMER_OV_FLAGS,(1<<_STIMER8_OV)
    RET













