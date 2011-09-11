
_Timer0_COMP_Interrupt:
	PUSH       R30
	PUSH       R31
	PUSH       R27
	IN         R27, SREG+0
	PUSH       R27

;remote.c,90 :: 		void Timer0_COMP_Interrupt() iv IVT_ADDR_TIMER0_COMP
;remote.c,92 :: 		Time++;
	LDS        R16, _Time+0
	LDS        R17, _Time+1
	SUBI       R16, 255
	SBCI       R17, 255
	STS        _Time+0, R16
	STS        _Time+1, R17
;remote.c,93 :: 		}
L_end_Timer0_COMP_Interrupt:
	POP        R27
	OUT        SREG+0, R27
	POP        R27
	POP        R31
	POP        R30
	RETI
; end of _Timer0_COMP_Interrupt

_Ext_Interrupt:
	PUSH       R30
	PUSH       R31
	PUSH       R27
	IN         R27, SREG+0
	PUSH       R27

;remote.c,94 :: 		void Ext_Interrupt() iv IVT_ADDR_INT0
;remote.c,96 :: 		GICR&=(~(1<<INT0));        //Disable INT0
	IN         R27, GICR+0
	CBR        R27, 64
	OUT        GICR+0, R27
;remote.c,97 :: 		SREG_I_bit = 1;
	IN         R27, SREG_I_bit+0
	SBR        R27, 128
	OUT        SREG_I_bit+0, R27
;remote.c,99 :: 		TempTime = Time;
	LDS        R16, _Time+0
	LDS        R17, _Time+1
	STS        _TempTime+0, R16
	STS        _TempTime+1, R17
;remote.c,100 :: 		Time=0;
	LDI        R27, 0
	STS        _Time+0, R27
	STS        _Time+1, R27
;remote.c,101 :: 		TCNT0=0;
	LDI        R27, 0
	OUT        TCNT0+0, R27
;remote.c,102 :: 		switch(State)
	JMP        L_Ext_Interrupt0
;remote.c,104 :: 		case IR_HIGH:
L_Ext_Interrupt2:
;remote.c,106 :: 		if(Edge)
	LDS        R16, _Edge+0
	TST        R16
	BRNE       L__Ext_Interrupt87
	JMP        L_Ext_Interrupt3
L__Ext_Interrupt87:
;remote.c,109 :: 		if((TempTime>800) && (TempTime<990))
	LDS        R18, _TempTime+0
	LDS        R19, _TempTime+1
	LDI        R16, 32
	LDI        R17, 3
	CP         R16, R18
	CPC        R17, R19
	BRLO       L__Ext_Interrupt88
	JMP        L__Ext_Interrupt74
L__Ext_Interrupt88:
	LDS        R16, _TempTime+0
	LDS        R17, _TempTime+1
	CPI        R17, 3
	BRNE       L__Ext_Interrupt89
	CPI        R16, 222
L__Ext_Interrupt89:
	BRLO       L__Ext_Interrupt90
	JMP        L__Ext_Interrupt73
L__Ext_Interrupt90:
L__Ext_Interrupt72:
;remote.c,112 :: 		State=IR_LOW;
	LDI        R27, 1
	STS        _State+0, R27
;remote.c,114 :: 		MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
	IN         R16, MCUCR+0
	ANDI       R16, 252
	OUT        MCUCR+0, R16
;remote.c,115 :: 		MCUCR|=(1<<ISC01);
	IN         R16, MCUCR+0
	ORI        R16, 2
	OUT        MCUCR+0, R16
;remote.c,116 :: 		Edge=0;
	LDI        R27, 0
	STS        _Edge+0, R27
;remote.c,117 :: 		}
	JMP        L_Ext_Interrupt7
;remote.c,109 :: 		if((TempTime>800) && (TempTime<990))
L__Ext_Interrupt74:
L__Ext_Interrupt73:
;remote.c,125 :: 		}
L_Ext_Interrupt7:
;remote.c,126 :: 		}
	JMP        L_Ext_Interrupt8
L_Ext_Interrupt3:
;remote.c,130 :: 		MCUCR|=((1<<ISC01)|(1<<ISC00));        //Set INT on Rising Edge
	IN         R16, MCUCR+0
	ORI        R16, 3
	OUT        MCUCR+0, R16
;remote.c,131 :: 		Edge=1;
	LDI        R27, 1
	STS        _Edge+0, R27
;remote.c,132 :: 		}
L_Ext_Interrupt8:
;remote.c,133 :: 		break;
	JMP        L_Ext_Interrupt1
;remote.c,135 :: 		case IR_LOW:
L_Ext_Interrupt9:
;remote.c,137 :: 		if((TempTime>400) && (TempTime<500))
	LDS        R18, _TempTime+0
	LDS        R19, _TempTime+1
	LDI        R16, 144
	LDI        R17, 1
	CP         R16, R18
	CPC        R17, R19
	BRLO       L__Ext_Interrupt91
	JMP        L__Ext_Interrupt76
L__Ext_Interrupt91:
	LDS        R16, _TempTime+0
	LDS        R17, _TempTime+1
	CPI        R17, 1
	BRNE       L__Ext_Interrupt92
	CPI        R16, 244
L__Ext_Interrupt92:
	BRLO       L__Ext_Interrupt93
	JMP        L__Ext_Interrupt75
L__Ext_Interrupt93:
L__Ext_Interrupt71:
;remote.c,140 :: 		State=IR_RECEIVE_BITS;
	LDI        R27, 3
	STS        _State+0, R27
;remote.c,141 :: 		BitNo=0;
	LDI        R27, 0
	STS        _BitNo+0, R27
;remote.c,142 :: 		ByteNo=0;
	LDI        R27, 0
	STS        _ByteNo+0, R27
;remote.c,143 :: 		MCUCR|=((1<<ISC01)|(1<<ISC00));        //Set INT on Rising Edge
	IN         R16, MCUCR+0
	ORI        R16, 3
	OUT        MCUCR+0, R16
;remote.c,144 :: 		Edge=1;
	LDI        R27, 1
	STS        _Edge+0, R27
;remote.c,146 :: 		}
	JMP        L_Ext_Interrupt13
;remote.c,137 :: 		if((TempTime>400) && (TempTime<500))
L__Ext_Interrupt76:
L__Ext_Interrupt75:
;remote.c,147 :: 		else if((TempTime>200) && (TempTime<245))
	LDS        R18, _TempTime+0
	LDS        R19, _TempTime+1
	LDI        R16, 200
	LDI        R17, 0
	CP         R16, R18
	CPC        R17, R19
	BRLO       L__Ext_Interrupt94
	JMP        L__Ext_Interrupt78
L__Ext_Interrupt94:
	LDS        R16, _TempTime+0
	LDS        R17, _TempTime+1
	CPI        R17, 0
	BRNE       L__Ext_Interrupt95
	CPI        R16, 245
L__Ext_Interrupt95:
	BRLO       L__Ext_Interrupt96
	JMP        L__Ext_Interrupt77
L__Ext_Interrupt96:
L__Ext_Interrupt70:
;remote.c,150 :: 		}
	JMP        L_Ext_Interrupt17
;remote.c,147 :: 		else if((TempTime>200) && (TempTime<245))
L__Ext_Interrupt78:
L__Ext_Interrupt77:
;remote.c,158 :: 		}
L_Ext_Interrupt17:
L_Ext_Interrupt13:
;remote.c,159 :: 		break;
	JMP        L_Ext_Interrupt1
;remote.c,161 :: 		case IR_RECEIVE_BITS:
L_Ext_Interrupt18:
;remote.c,163 :: 		if(Edge)
	LDS        R16, _Edge+0
	TST        R16
	BRNE       L__Ext_Interrupt97
	JMP        L_Ext_Interrupt19
L__Ext_Interrupt97:
;remote.c,166 :: 		if((TempTime>=45) && (TempTime<69))
	LDS        R16, _TempTime+0
	LDS        R17, _TempTime+1
	CPI        R17, 0
	BRNE       L__Ext_Interrupt98
	CPI        R16, 45
L__Ext_Interrupt98:
	BRSH       L__Ext_Interrupt99
	JMP        L__Ext_Interrupt80
L__Ext_Interrupt99:
	LDS        R16, _TempTime+0
	LDS        R17, _TempTime+1
	CPI        R17, 0
	BRNE       L__Ext_Interrupt100
	CPI        R16, 69
L__Ext_Interrupt100:
	BRLO       L__Ext_Interrupt101
	JMP        L__Ext_Interrupt79
L__Ext_Interrupt101:
L__Ext_Interrupt69:
;remote.c,170 :: 		MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
	IN         R16, MCUCR+0
	ANDI       R16, 252
	OUT        MCUCR+0, R16
;remote.c,171 :: 		MCUCR|=(1<<ISC01);
	IN         R16, MCUCR+0
	ORI        R16, 2
	OUT        MCUCR+0, R16
;remote.c,172 :: 		Edge=0;
	LDI        R27, 0
	STS        _Edge+0, R27
;remote.c,173 :: 		}
	JMP        L_Ext_Interrupt23
;remote.c,166 :: 		if((TempTime>=45) && (TempTime<69))
L__Ext_Interrupt80:
L__Ext_Interrupt79:
;remote.c,181 :: 		}
L_Ext_Interrupt23:
;remote.c,182 :: 		}
	JMP        L_Ext_Interrupt24
L_Ext_Interrupt19:
;remote.c,186 :: 		if((TempTime>=44) && (TempTime<=66))
	LDS        R16, _TempTime+0
	LDS        R17, _TempTime+1
	CPI        R17, 0
	BRNE       L__Ext_Interrupt102
	CPI        R16, 44
L__Ext_Interrupt102:
	BRSH       L__Ext_Interrupt103
	JMP        L__Ext_Interrupt82
L__Ext_Interrupt103:
	LDS        R18, _TempTime+0
	LDS        R19, _TempTime+1
	LDI        R16, 66
	LDI        R17, 0
	CP         R16, R18
	CPC        R17, R19
	BRSH       L__Ext_Interrupt104
	JMP        L__Ext_Interrupt81
L__Ext_Interrupt104:
L__Ext_Interrupt68:
;remote.c,189 :: 		BitNo++;
	LDS        R16, _BitNo+0
	SUBI       R16, 255
	STS        _BitNo+0, R16
;remote.c,190 :: 		if(BitNo==8)
	LDS        R16, _BitNo+0
	CPI        R16, 8
	BREQ       L__Ext_Interrupt105
	JMP        L_Ext_Interrupt28
L__Ext_Interrupt105:
;remote.c,192 :: 		BitNo=0;
	LDI        R27, 0
	STS        _BitNo+0, R27
;remote.c,193 :: 		ByteNo++;
	LDS        R16, _ByteNo+0
	SUBI       R16, 255
	STS        _ByteNo+0, R16
;remote.c,194 :: 		if(ByteNo==4)
	LDS        R16, _ByteNo+0
	CPI        R16, 4
	BREQ       L__Ext_Interrupt106
	JMP        L_Ext_Interrupt29
L__Ext_Interrupt106:
;remote.c,196 :: 		State=IR_WAIT_STOP_BIT;
	LDI        R27, 4
	STS        _State+0, R27
;remote.c,197 :: 		}
L_Ext_Interrupt29:
;remote.c,198 :: 		}
L_Ext_Interrupt28:
;remote.c,199 :: 		MCUCR|=((1<<ISC01)|(1<<ISC00));        //Set INT on Rising Edge
	IN         R16, MCUCR+0
	ORI        R16, 3
	OUT        MCUCR+0, R16
;remote.c,200 :: 		Edge=1;
	LDI        R27, 1
	STS        _Edge+0, R27
;remote.c,201 :: 		}
	JMP        L_Ext_Interrupt30
;remote.c,186 :: 		if((TempTime>=44) && (TempTime<=66))
L__Ext_Interrupt82:
L__Ext_Interrupt81:
;remote.c,202 :: 		else if((TempTime>152) && (TempTime<186))
	LDS        R18, _TempTime+0
	LDS        R19, _TempTime+1
	LDI        R16, 152
	LDI        R17, 0
	CP         R16, R18
	CPC        R17, R19
	BRLO       L__Ext_Interrupt107
	JMP        L__Ext_Interrupt84
L__Ext_Interrupt107:
	LDS        R16, _TempTime+0
	LDS        R17, _TempTime+1
	CPI        R17, 0
	BRNE       L__Ext_Interrupt108
	CPI        R16, 186
L__Ext_Interrupt108:
	BRLO       L__Ext_Interrupt109
	JMP        L__Ext_Interrupt83
L__Ext_Interrupt109:
L__Ext_Interrupt67:
;remote.c,205 :: 		IrData[ByteNo]|=(1<<BitNo);
	LDI        R17, #lo_addr(_IrData+0)
	LDI        R18, hi_addr(_IrData+0)
	LDS        R16, _ByteNo+0
	MOV        R19, R16
	LDI        R20, 0
	ADD        R19, R17
	ADC        R20, R18
	LDS        R27, _BitNo+0
	LDI        R17, 1
	TST        R27
	BREQ       L__Ext_Interrupt111
L__Ext_Interrupt110:
	LSL        R17
	DEC        R27
	BRNE       L__Ext_Interrupt110
L__Ext_Interrupt111:
	MOV        R30, R19
	MOV        R31, R20
	LD         R16, Z
	OR         R16, R17
	MOV        R30, R19
	MOV        R31, R20
	ST         Z, R16
;remote.c,206 :: 		BitNo++;
	LDS        R16, _BitNo+0
	SUBI       R16, 255
	STS        _BitNo+0, R16
;remote.c,207 :: 		if(BitNo==8)
	LDS        R16, _BitNo+0
	CPI        R16, 8
	BREQ       L__Ext_Interrupt112
	JMP        L_Ext_Interrupt34
L__Ext_Interrupt112:
;remote.c,209 :: 		BitNo=0;
	LDI        R27, 0
	STS        _BitNo+0, R27
;remote.c,210 :: 		ByteNo++;
	LDS        R16, _ByteNo+0
	SUBI       R16, 255
	STS        _ByteNo+0, R16
;remote.c,211 :: 		if(ByteNo==4)
	LDS        R16, _ByteNo+0
	CPI        R16, 4
	BREQ       L__Ext_Interrupt113
	JMP        L_Ext_Interrupt35
L__Ext_Interrupt113:
;remote.c,213 :: 		State=IR_WAIT_STOP_BIT;
	LDI        R27, 4
	STS        _State+0, R27
;remote.c,214 :: 		}
L_Ext_Interrupt35:
;remote.c,215 :: 		}
L_Ext_Interrupt34:
;remote.c,216 :: 		MCUCR|=((1<<ISC01)|(1<<ISC00));        //Set INT on Rising Edge
	IN         R16, MCUCR+0
	ORI        R16, 3
	OUT        MCUCR+0, R16
;remote.c,217 :: 		Edge=1;
	LDI        R27, 1
	STS        _Edge+0, R27
;remote.c,219 :: 		}
	JMP        L_Ext_Interrupt36
;remote.c,202 :: 		else if((TempTime>152) && (TempTime<186))
L__Ext_Interrupt84:
L__Ext_Interrupt83:
;remote.c,228 :: 		}
L_Ext_Interrupt36:
L_Ext_Interrupt30:
;remote.c,229 :: 		}
L_Ext_Interrupt24:
;remote.c,230 :: 		break;
	JMP        L_Ext_Interrupt1
;remote.c,232 :: 		case IR_WAIT_STOP_BIT:
L_Ext_Interrupt37:
;remote.c,234 :: 		if(Edge)
	LDS        R16, _Edge+0
	TST        R16
	BRNE       L__Ext_Interrupt114
	JMP        L_Ext_Interrupt38
L__Ext_Interrupt114:
;remote.c,237 :: 		if(IrData[2]==((unsigned char)~IrData[3]))
	LDS        R16, _IrData+3
	MOV        R17, R16
	COM        R17
	LDS        R16, _IrData+2
	CP         R16, R17
	BREQ       L__Ext_Interrupt115
	JMP        L_Ext_Interrupt39
L__Ext_Interrupt115:
;remote.c,240 :: 		Data_receive = 1;
	LDI        R27, 1
	STS        _Data_receive+0, R27
;remote.c,242 :: 		MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
	IN         R16, MCUCR+0
	ANDI       R16, 252
	OUT        MCUCR+0, R16
;remote.c,243 :: 		MCUCR|=(1<<ISC01);
	IN         R16, MCUCR+0
	ORI        R16, 2
	OUT        MCUCR+0, R16
;remote.c,246 :: 		}
L_Ext_Interrupt39:
;remote.c,247 :: 		}
L_Ext_Interrupt38:
;remote.c,248 :: 		break;
	JMP        L_Ext_Interrupt1
;remote.c,251 :: 		}
L_Ext_Interrupt0:
	LDS        R16, _State+0
	CPI        R16, 0
	BRNE       L__Ext_Interrupt116
	JMP        L_Ext_Interrupt2
L__Ext_Interrupt116:
	LDS        R16, _State+0
	CPI        R16, 1
	BRNE       L__Ext_Interrupt117
	JMP        L_Ext_Interrupt9
L__Ext_Interrupt117:
	LDS        R16, _State+0
	CPI        R16, 3
	BRNE       L__Ext_Interrupt118
	JMP        L_Ext_Interrupt18
L__Ext_Interrupt118:
	LDS        R16, _State+0
	CPI        R16, 4
	BRNE       L__Ext_Interrupt119
	JMP        L_Ext_Interrupt37
L__Ext_Interrupt119:
L_Ext_Interrupt1:
;remote.c,252 :: 		GICR|=(1<<INT0);        //Enable INT1
	IN         R16, GICR+0
	ORI        R16, 64
	OUT        GICR+0, R16
;remote.c,253 :: 		}
L_end_Ext_Interrupt:
	POP        R27
	OUT        SREG+0, R27
	POP        R27
	POP        R31
	POP        R30
	RETI
; end of _Ext_Interrupt

_main:
	LDI        R27, 255
	OUT        SPL+0, R27
	LDI        R27, 0
	OUT        SPL+1, R27
	IN         R28, SPL+0
	IN         R29, SPL+1
	SBIW       R28, 1
	OUT        SPL+0, R28
	OUT        SPL+1, R29
	ADIW       R28, 1

;remote.c,258 :: 		void main()
;remote.c,261 :: 		DDD2_bit = 0;
	PUSH       R2
	PUSH       R3
	PUSH       R4
	PUSH       R5
	IN         R27, DDD2_bit+0
	CBR        R27, 4
	OUT        DDD2_bit+0, R27
;remote.c,262 :: 		Lcd_Init();                        // Initialize LCD
	CALL       _Lcd_Init+0
;remote.c,263 :: 		Lcd_Cmd(_LCD_CLEAR);               // Clear display
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Cmd+0
;remote.c,264 :: 		Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
	LDI        R27, 12
	MOV        R2, R27
	CALL       _Lcd_Cmd+0
;remote.c,266 :: 		Lcd_Out(1, 1, "IR RemoteDecoder");
	LDI        R27, #lo_addr(?lstr1_remote+0)
	MOV        R4, R27
	LDI        R27, hi_addr(?lstr1_remote+0)
	MOV        R5, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Out+0
;remote.c,267 :: 		Delay_ms(2000);
	LDI        R18, 163
	LDI        R17, 87
	LDI        R16, lo_addr(R3)
L_main40:
	DEC        R16
	BRNE       L_main40
	DEC        R17
	BRNE       L_main40
	DEC        R18
	BRNE       L_main40
	NOP
;remote.c,268 :: 		Lcd_Cmd(_LCD_CLEAR);               // Clear display
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Cmd+0
;remote.c,270 :: 		RemoteInit();
	CALL       _RemoteInit+0
;remote.c,272 :: 		while(1)
L_main42:
;remote.c,274 :: 		lcd_data = GetRemoteCmd(1);
	LDI        R27, 1
	MOV        R2, R27
	CALL       _GetRemoteCmd+0
	STD        Y+0, R16
;remote.c,275 :: 		SREG_I_bit = 0;
	IN         R27, SREG_I_bit+0
	CBR        R27, 128
	OUT        SREG_I_bit+0, R27
;remote.c,276 :: 		Lcd_Cmd(_LCD_CLEAR);
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Cmd+0
;remote.c,277 :: 		if(lcd_data == 0x0A)
	LDD        R16, Y+0
	CPI        R16, 10
	BREQ       L__main121
	JMP        L_main44
L__main121:
;remote.c,279 :: 		Lcd_Out(1, 1, "You pressed 1!!");
	LDI        R27, #lo_addr(?lstr2_remote+0)
	MOV        R4, R27
	LDI        R27, hi_addr(?lstr2_remote+0)
	MOV        R5, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Out+0
;remote.c,280 :: 		}
	JMP        L_main45
L_main44:
;remote.c,281 :: 		else if(lcd_data == 0x1B)
	LDD        R16, Y+0
	CPI        R16, 27
	BREQ       L__main122
	JMP        L_main46
L__main122:
;remote.c,283 :: 		Lcd_Out(1, 1, "You pressed 2!!");
	LDI        R27, #lo_addr(?lstr3_remote+0)
	MOV        R4, R27
	LDI        R27, hi_addr(?lstr3_remote+0)
	MOV        R5, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Out+0
;remote.c,284 :: 		}
	JMP        L_main47
L_main46:
;remote.c,285 :: 		else if(lcd_data == 0x1F)
	LDD        R16, Y+0
	CPI        R16, 31
	BREQ       L__main123
	JMP        L_main48
L__main123:
;remote.c,287 :: 		Lcd_Out(1, 1, "You pressed 3!!");
	LDI        R27, #lo_addr(?lstr4_remote+0)
	MOV        R4, R27
	LDI        R27, hi_addr(?lstr4_remote+0)
	MOV        R5, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Out+0
;remote.c,288 :: 		}
	JMP        L_main49
L_main48:
;remote.c,289 :: 		else if(lcd_data == 0x0C)
	LDD        R16, Y+0
	CPI        R16, 12
	BREQ       L__main124
	JMP        L_main50
L__main124:
;remote.c,291 :: 		Lcd_Out(1, 1, "You pressed 4!!");
	LDI        R27, #lo_addr(?lstr5_remote+0)
	MOV        R4, R27
	LDI        R27, hi_addr(?lstr5_remote+0)
	MOV        R5, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Out+0
;remote.c,292 :: 		}
	JMP        L_main51
L_main50:
;remote.c,293 :: 		else if(lcd_data == 0x0D)
	LDD        R16, Y+0
	CPI        R16, 13
	BREQ       L__main125
	JMP        L_main52
L__main125:
;remote.c,295 :: 		Lcd_Out(1, 1, "You pressed 5!!");
	LDI        R27, #lo_addr(?lstr6_remote+0)
	MOV        R4, R27
	LDI        R27, hi_addr(?lstr6_remote+0)
	MOV        R5, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Out+0
;remote.c,296 :: 		}
	JMP        L_main53
L_main52:
;remote.c,297 :: 		else if(lcd_data == 0x0E)
	LDD        R16, Y+0
	CPI        R16, 14
	BREQ       L__main126
	JMP        L_main54
L__main126:
;remote.c,299 :: 		Lcd_Out(1, 1, "You pressed 6!!");
	LDI        R27, #lo_addr(?lstr7_remote+0)
	MOV        R4, R27
	LDI        R27, hi_addr(?lstr7_remote+0)
	MOV        R5, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Out+0
;remote.c,300 :: 		}
	JMP        L_main55
L_main54:
;remote.c,303 :: 		Lcd_Out(1, 1, "You pressed:");
	LDI        R27, #lo_addr(?lstr8_remote+0)
	MOV        R4, R27
	LDI        R27, hi_addr(?lstr8_remote+0)
	MOV        R5, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Out+0
;remote.c,304 :: 		Lcd_Chr(1, 14, lcd_data);
	LDD        R4, Y+0
	LDI        R27, 14
	MOV        R3, R27
	LDI        R27, 1
	MOV        R2, R27
	CALL       _Lcd_Chr+0
;remote.c,305 :: 		}
L_main55:
L_main53:
L_main51:
L_main49:
L_main47:
L_main45:
;remote.c,306 :: 		SREG_I_bit = 1;
	IN         R27, SREG_I_bit+0
	SBR        R27, 128
	OUT        SREG_I_bit+0, R27
;remote.c,307 :: 		}
	JMP        L_main42
;remote.c,308 :: 		}
L_end_main:
	JMP        L_end_main
; end of _main

_RemoteInit:

;remote.c,311 :: 		void RemoteInit()
;remote.c,314 :: 		for(i=0;i<4;i++)
; i start address is: 18 (R18)
; i start address is: 18 (R18)
	LDI        R18, 0
; i end address is: 18 (R18)
; i end address is: 18 (R18)
L_RemoteInit56:
; i start address is: 18 (R18)
	CPI        R18, 4
	BRLO       L__RemoteInit128
	JMP        L_RemoteInit57
L__RemoteInit128:
;remote.c,316 :: 		IrData[i]=0;
	LDI        R16, #lo_addr(_IrData+0)
	LDI        R17, hi_addr(_IrData+0)
	MOV        R30, R18
	LDI        R31, 0
	ADD        R30, R16
	ADC        R31, R17
	LDI        R27, 0
	ST         Z, R27
;remote.c,314 :: 		for(i=0;i<4;i++)
	MOV        R16, R18
	SUBI       R16, 255
	MOV        R18, R16
;remote.c,317 :: 		}
; i end address is: 18 (R18)
	JMP        L_RemoteInit56
L_RemoteInit57:
;remote.c,318 :: 		State=IR_HIGH;
	LDI        R27, 0
	STS        _State+0, R27
;remote.c,319 :: 		Edge=0;
	LDI        R27, 0
	STS        _Edge+0, R27
;remote.c,323 :: 		TCCR0|=((1<<CS00)|(1<<WGM01));//Prescaler : Fcpu   Mode : CTC
	IN         R16, TCCR0+0
	ORI        R16, 9
	OUT        TCCR0+0, R16
;remote.c,324 :: 		TIMSK|=(1<<OCIE0);            //Enable Output Compare Interrupt
	IN         R27, TIMSK+0
	SBR        R27, 2
	OUT        TIMSK+0, R27
;remote.c,325 :: 		OCR0=TIMER_COMP_VAL;          //Set Compare Value
	LDI        R27, 160
	OUT        OCR0+0, R27
;remote.c,328 :: 		MCUCR|=(1<<ISC01);           //INT ON Falling Edge
	IN         R16, MCUCR+0
	ORI        R16, 2
	OUT        MCUCR+0, R16
;remote.c,329 :: 		GICR|=(1<<INT0);             //Enable INT0
	IN         R16, GICR+0
	ORI        R16, 64
	OUT        GICR+0, R16
;remote.c,332 :: 		SREG_I_bit = 1;
	IN         R27, SREG_I_bit+0
	SBR        R27, 128
	OUT        SREG_I_bit+0, R27
;remote.c,333 :: 		}
L_end_RemoteInit:
	RET
; end of _RemoteInit

_ResetIR:

;remote.c,335 :: 		void ResetIR()
;remote.c,338 :: 		for(i=0;i<4;i++) IrData[i]=0;
; i start address is: 18 (R18)
; i start address is: 18 (R18)
	LDI        R18, 0
; i end address is: 18 (R18)
; i end address is: 18 (R18)
L_ResetIR59:
; i start address is: 18 (R18)
	CPI        R18, 4
	BRLO       L__ResetIR130
	JMP        L_ResetIR60
L__ResetIR130:
	LDI        R16, #lo_addr(_IrData+0)
	LDI        R17, hi_addr(_IrData+0)
	MOV        R30, R18
	LDI        R31, 0
	ADD        R30, R16
	ADC        R31, R17
	LDI        R27, 0
	ST         Z, R27
	MOV        R16, R18
	SUBI       R16, 255
	MOV        R18, R16
; i end address is: 18 (R18)
	JMP        L_ResetIR59
L_ResetIR60:
;remote.c,339 :: 		State=IR_HIGH;
	LDI        R27, 0
	STS        _State+0, R27
;remote.c,341 :: 		MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
	IN         R16, MCUCR+0
	ANDI       R16, 252
	OUT        MCUCR+0, R16
;remote.c,342 :: 		MCUCR|=(1<<ISC01);
	IN         R27, MCUCR+0
	SBR        R27, 2
	OUT        MCUCR+0, R27
;remote.c,343 :: 		Edge=0;
	LDI        R27, 0
	STS        _Edge+0, R27
;remote.c,344 :: 		Time=0;
	LDI        R27, 0
	STS        _Time+0, R27
	STS        _Time+1, R27
;remote.c,345 :: 		}
L_end_ResetIR:
	RET
; end of _ResetIR

_GetRemoteCmd:

;remote.c,347 :: 		unsigned char GetRemoteCmd(char wait)
;remote.c,351 :: 		if(wait)
	TST        R2
	BRNE       L__GetRemoteCmd132
	JMP        L_GetRemoteCmd62
L__GetRemoteCmd132:
;remote.c,352 :: 		while(Data_receive == 0);
L_GetRemoteCmd63:
	LDS        R16, _Data_receive+0
	CPI        R16, 0
	BREQ       L__GetRemoteCmd133
	JMP        L_GetRemoteCmd64
L__GetRemoteCmd133:
	JMP        L_GetRemoteCmd63
L_GetRemoteCmd64:
	JMP        L_GetRemoteCmd65
L_GetRemoteCmd62:
;remote.c,355 :: 		if(Data_receive == 0)
	LDS        R16, _Data_receive+0
	CPI        R16, 0
	BREQ       L__GetRemoteCmd134
	JMP        L_GetRemoteCmd66
L__GetRemoteCmd134:
;remote.c,356 :: 		return (RC_NONE);
	LDI        R16, 255
	JMP        L_end_GetRemoteCmd
L_GetRemoteCmd66:
;remote.c,357 :: 		}
L_GetRemoteCmd65:
;remote.c,358 :: 		cmd=IrData[2];
; cmd start address is: 17 (R17)
	LDS        R17, _IrData+2
;remote.c,359 :: 		Data_receive = 0;
	LDI        R27, 0
	STS        _Data_receive+0, R27
;remote.c,360 :: 		ResetIR();
	PUSH       R17
	CALL       _ResetIR+0
	POP        R17
;remote.c,361 :: 		return cmd;
	MOV        R16, R17
; cmd end address is: 17 (R17)
;remote.c,362 :: 		}
L_end_GetRemoteCmd:
	RET
; end of _GetRemoteCmd
