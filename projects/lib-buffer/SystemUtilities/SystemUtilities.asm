.Equ OverflowFlag            = 0
.Equ FlagBufferFull          = 1
.Equ FlagBufferEmpty         = 2

.Equ CommandSystemCreateNormalBuffers_           = 1
.Equ CommandSystemClearNormalBuffers_            = 2
.Equ CommandSystemPopFromAdressNormalBuffers_    = 3
.Equ CommandSystemPopNormalBuffers_              = 4
.Equ CommandSystemPushToAdressNormalBuffers_     = 5
.Equ CommandSystemPushNormalBuffers_             = 6

.Equ CommandSystemCreateCircleBuffers_           = 1
.Equ CommandSystemPushCircleBuffers_             = 2
.Equ CommandSystemPopCircleBuffers_              = 3
.Equ CommandSystemClearCircleBuffers_            = 4
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
BeginSystemUtilitySections:
SystemNormalBuffers:
	Rjmp SystemNormalBuffers_ /**0**/
SystemCircleBuffers:
	Rjmp SystemCircleBuffers_ /**2**/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
SystemNormalBuffers_:
	Push R16
	Mov R16,R15
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemPushNormalBuffers_
	Brne IsNotCommandSystemPushNormalBuffers_
	Pop R16
	Rjmp SystemPushNormalBuffers_
IsNotCommandSystemPushNormalBuffers_:
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemPushToAdressNormalBuffers_
	Brne IsNotCommandSystemPushToAdressNormalBuffers_
	Pop R16
	Rjmp SystemPushToAdressNormalBuffers_
IsNotCommandSystemPushToAdressNormalBuffers_:
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemPopNormalBuffers_
	Brne IsNotCommandSystemPopNormalBuffers_
	Pop R16
	Rjmp SystemPopNormalBuffers_
IsNotCommandSystemPopNormalBuffers_:
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemPopFromAdressNormalBuffers_
	Brne IsNotCommandSystemPopFromAdressNormalBuffers_
	Pop R16
	Rjmp SystemPopFromAdressNormalBuffers_
IsNotCommandSystemPopFromAdressNormalBuffers_:
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemClearNormalBuffers_
	Brne IsNotCommandSystemCommandSystemClearNormalBuffers_
	Pop R16
	Rjmp SystemClearNormalBuffers_
IsNotCommandSystemCommandSystemClearNormalBuffers_:
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemCreateNormalBuffers_
	Brne IsNotCommandSystemCreateNormalBuffers_
	Pop R16
	Rjmp SystemCreateNormalBuffers_
IsNotCommandSystemCreateNormalBuffers_:
/*--------------------------------------------------------------------*/
    Pop R16
ReturnFromSystemClearNormalBuffers_:
ReturnFromSystemPopFromAdressNormalBuffers_:
ReturnFromSystemPopNormalBuffers_:
ReturnFromSystemPushToAdressNormalBuffers_:
ReturnFromSystemPushNormalBuffers_:
ReturnFromSystemCreateNormalBuffers_:
	Ret
/**********************************************************************/
/**********************************************************************/
;Z = Adr of buffer
;X = Dimension of buffer
SystemCreateNormalBuffers_:
	Push R16
	Std Z+0,Xl
	Std Z+1,Xh
	Clr R16
	Std Z+2,R16
	Std Z+3,R16
	Std Z+4,R16
	Std Z+5,R16
	Pop R16
	Rjmp ReturnFromSystemCreateNormalBuffers_
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
;Z = Adr of buffer
;R16 = Byte to be write
SystemPushNormalBuffers_:
	Push Xl
	Push Xh
	Push Yl
	Push Yh
	Ldd Xl,Z+2
	Ldd Xh,Z+3
	Ldd Yl,Z+0
	Ldd Yh,Z+1
	Cp Xl,Yl
	Cpc Xh,Yh
	Breq SystemPushOverflowNormalBuffers_
	Movw Yl:Yh,Zl:Zh
	Adiw Yl:Yh,6
	Add Yl,Xl
	Adc Yh,Xh
	St Y,R16
	Adiw Xl:Xh,1
	Std Z+2,Xl
	Std Z+3,Xh
	Clr R17
	Rjmp SkipSystemPushOverflowNormalBuffers_
SystemPushOverflowNormalBuffers_:
    Clr R17
	Sbr R17,1<<OverflowFlag
SkipSystemPushOverflowNormalBuffers_:
	Pop Xh
	Pop Xl
	Pop Yh
	Pop Yl
	Rjmp ReturnFromSystemPushNormalBuffers_
;Result:
;R17 = Flag Returned
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
;Z = Adr of buffer
;R16 = Byte to be write
;X = Adress in Buffer
SystemPushToAdressNormalBuffers_:
	Push Yl
	Push Yh
	Ldd Yl,Z+0
	Ldd Yh,Z+1
	Cp Xl,Yl
	Cpc Xh,Yh
	Brge OverflowSystemPushToAdressNormalBuffers_
	Movw Yl:Yh,Zl:Zh
	Adiw Yl:Yh,6
	Add Yl,Xl
	Adc Yh,Xh
	St Y,R16
	Clr R17
	Rjmp SkipOverflowSystemPushToAdressNormalBuffers_
OverflowSystemPushToAdressNormalBuffers_:
	Clr R17
	Sbr R17,1<<OverflowFlag
SkipOverflowSystemPushToAdressNormalBuffers_:
	Pop Yh
	Pop Yl
	Rjmp ReturnFromSystemPushToAdressNormalBuffers_
;Result:
;R17 = Flag Returned
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
;Z = Adr of buffer
SystemPopNormalBuffers_:
	Push Xl
	Push Xh
	Push Yl
	Push Yh
	Ldd Xl,Z+4
	Ldd Xh,Z+5
	Ldd Yl,Z+2
	Ldd Yh,Z+3
	Cp Xl,Yl
	Cpc Xh,Yh
	Breq SystemPopOverflowNormalBuffers_
	Movw Yl:Yh,Zl:Zh
	Adiw Yl:Yh,6
	Add Yl,Xl
	Adc Yh,Xh
	Ld R16,Y
	Adiw Xl:Xh,1
	Std Z+4,Xl
	Std Z+5,Xh
	Clr R17
	Rjmp SkipSystemPopOverflowNormalBuffers_
SystemPopOverflowNormalBuffers_:
    Clr R17
	Sbr R17,1<<OverflowFlag
SkipSystemPopOverflowNormalBuffers_:
	Pop Xh
	Pop Xl
	Pop Yh
	Pop Yl
	Rjmp ReturnFromSystemPopNormalBuffers_
;Result:
;R16 = Byte Read
;R17 = Flag Returned
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
;Z = Adr of buffer
;X = Adress in Buffer
SystemPopFromAdressNormalBuffers_:
	Push Yl
	Push Yh
	Ldd Yl,Z+2
	Ldd Yh,Z+3
	Cp Xl,Yl
	Cpc Xh,Yh
	Brge OverflowSystemPopToAdressNormalBuffers_
	Movw Yl:Yh,Zl:Zh
	Adiw Yl:Yh,6
	Add Yl,Xl
	Adc Yh,Xh
	Ld R16,Y
	Clr R17
	Rjmp SkipOverflowSystemPopToAdressNormalBuffers_
OverflowSystemPopToAdressNormalBuffers_:
	Clr R17
	Sbr R17,1<<OverflowFlag
SkipOverflowSystemPopToAdressNormalBuffers_:
	Pop Yh
	Pop Yl
	Rjmp ReturnFromSystemPopFromAdressNormalBuffers_
;Result:
;R16 = Byte Read
;R17 = Flag Returned
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
;Z = Adr of buffer
SystemClearNormalBuffers_:
	Push R16
	Clr R16
	Std Z+2,R16
	Std Z+3,R16
	Std Z+4,R16
	Std Z+5,R16
	Pop R16
	Rjmp ReturnFromSystemClearNormalBuffers_
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
SystemCircleBuffers_:
	Push R16
	Mov R16,R15
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemPushCircleBuffers_
	Brne IsNotCommandSystemPushCircleBuffers_
	Pop R16
	Rjmp SystemPushCircleBuffers_
IsNotCommandSystemPushCircleBuffers_:
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemPopCircleBuffers_
	Brne IsNotCommandSystemPopCircleBuffers_
	Pop R16
	Rjmp SystemPopCircleBuffers_
IsNotCommandSystemPopCircleBuffers_:
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemClearCircleBuffers_
	Brne IsNotCommandSystemClearCircleBuffers_
	Pop R16
	Rjmp SystemClearCircleBuffers_
IsNotCommandSystemClearCircleBuffers_:
/*--------------------------------------------------------------------*/
	Cpi R16,CommandSystemCreateCircleBuffers_
	Brne IsNotCommandSystemCreateCircleBuffers_
	Pop R16
	Rjmp SystemCreateCircleBuffers_
IsNotCommandSystemCreateCircleBuffers_:
/*--------------------------------------------------------------------*/
	Pop R16
ReturnFromSystemClearCircleBuffers_:
ReturnFromSystemPopCircleBuffers_:
ReturnFromSystemPushCircleBuffers_:
ReturnFromSystemCreateCircleBuffers_:
	Ret
/**********************************************************************/
/**********************************************************************/
SystemCreateCircleBuffers_:
;Z = Adr of buffer
;X = Dimension of buffer
	Push R16
	Std Z+0,Xl
	Std Z+1,Xh
	Clr R16
	Std Z+2,R16
	Std Z+3,R16
	Std Z+4,R16
	Std Z+5,R16
	Inc R16
	Std Z+6,R16
	Pop R16
	Rjmp ReturnFromSystemCreateCircleBuffers_
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
;Z = Adr of buffer
;R16 = Byte to be write
SystemPushCircleBuffers_:
	Push R18
	Push Xl
	Push Xh
	Push Yl
	Push Yh
	Ldd R24,Z+0
	Ldd R25,Z+1
	Ldd Xl,Z+2
	Ldd Xh,Z+3
	Ldd Yl,Z+4
	Ldd Yh,Z+5
	Ldd R17,Z+6
	Ldi R18,0
	Cp Xl,R24
	Cpc Xh,R25
	Brne SkipResetContPushCircleBuffers_
	Clr Xl
	Clr Xh
SkipResetContPushCircleBuffers_:
	Cp Xl,Yl
	Cpc Xh,Yh
	Brne TheBufferIsNotFullPushCircleBuffers_
	Cpc R17,R18
	Breq BufferFullPushCircleBuffers_
TheBufferIsNotFullPushCircleBuffers_:
	Clr R17
	Std Z+6,R17
	Movw Yl:Yh,Zl:Zh
	Adiw Yl:Yh,7
	Add Yl,Xl
	Adc Yh,Xh
	St Y,R16
	Adiw XL:Xh,1
	Std Z+2,Xl
	Std Z+3,Xh
	Clr R17
	Rjmp SkipSetFlagBufferFullPushCircleBuffers_
BufferFullPushCircleBuffers_:
	Clr R17
	Sbr R17,1<<FlagBufferFull
SkipSetFlagBufferFullPushCircleBuffers_:
	Pop Xh
	Pop Xl
	Pop Yh
	Pop Yl
	Pop R18
	Rjmp ReturnFromSystemPushCircleBuffers_
;Result:
;R17 = Flag Returned
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
;Z = Adr of buffer
SystemPopCircleBuffers_:
	Push Xl
	Push Xh
	Push Yl
	Push Yh
	Ldd R24,Z+0
	Ldd R25,Z+1
	Ldd Xl,Z+4
	Ldd Xh,Z+5
	Ldd Yl,Z+2
	Ldd Yh,Z+3
	Cp Xl,Yl
	Cpc Xh,Yh
	Breq BufferEmptyPopCircleBuffers_
	Cp Xl,R24
	Cpc Xh,R25
	Brne SkipResetContPopCircleBuffers_
	Clr Xl
	Clr Xh
SkipResetContPopCircleBuffers_:
	Movw Yl:Yh,Zl:Zh
	Adiw Yl:Yh,7
	Add Yl,Xl
	Adc Yh,Xh
	Ld R16,Y
	Adiw XL:Xh,1
	Std Z+4,Xl
	Std Z+5,Xh
	Clr R17
	Rjmp SkipSetFlagBufferFullPopCircleBuffers_
BufferEmptyPopCircleBuffers_:
	Ldi R17,1
	Std Z+6,R17
	Clr R17
	Sbr R17,1<<FlagBufferFull
SkipSetFlagBufferFullPopCircleBuffers_:
	Pop Xh
	Pop Xl
	Pop Yh
	Pop Yl
	Rjmp ReturnFromSystemPopCircleBuffers_
;Result:
;R16 = Byte Read
;R17 = Flag Returned
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
;Z = Adr of buffer
SystemClearCircleBuffers_:
	Push R16
	Clr R16
	Std Z+2,R16
	Std Z+3,R16
	Std Z+4,R16
	Std Z+5,R16
	Std Z+6,R16
	Pop R16
	Rjmp ReturnFromSystemClearCircleBuffers_
/**********************************************************************/
/**********************************************************************/





























