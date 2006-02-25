

.DEF    STIMER_PVL          =R14
.DEF    STIMER_PVH          =R15


.DEF    TEMP                =R16
.DEF    STIMER_OV_FLAGS     =R20    ;FLAGS FOR EVENTS OF STIMERS
.EQU    _STIMER1_OV         =0
.EQU    _STIMER2_OV         =1
.EQU    _STIMER3_OV         =2
.EQU    _STIMER4_OV         =3
.EQU    _STIMER5_OV         =4
.EQU    _STIMER6_OV         =5
.EQU    _STIMER7_OV         =6
.EQU    _STIMER8_OV         =7

.DEF    STIMER_CVL          =R24    ;CURRENT VALUE OF EACH STIMER
.DEF    STIMER_CVH          =R25    