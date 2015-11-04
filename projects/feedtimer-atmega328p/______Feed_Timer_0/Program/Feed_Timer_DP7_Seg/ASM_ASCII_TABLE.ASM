	.SET	_D7DF	= _DISP7_DIG_OFF				;only to simplify code
	.SET	_D7DO	= _DISP7_DIG_ON
	.SET	_D7D0	= _DISP7_BIT_DIG_0
	.SET	_D7D1	= _DISP7_BIT_DIG_1
	.SET	_D7D2	= _DISP7_BIT_DIG_2
	.SET	_D7D3	= _DISP7_BIT_DIG_3
	.SET	_D7D4	= _DISP7_BIT_DIG_4
	.SET	_D7D5	= _DISP7_BIT_DIG_5
	.SET	_D7D6	= _DISP7_BIT_DIG_6
	.SET	_D7D7	= _DISP7_BIT_DIG_7
	.SET	_D7SFF	= _DISP7_SEG_OFF				;only to simplify code
	.SET	_D7SON	= _DISP7_SEG_ON
	.SET	_D7SA	= _DISP7_BIT_SEG_A
	.SET	_D7SB	= _DISP7_BIT_SEG_B
	.SET	_D7SC	= _DISP7_BIT_SEG_C
	.SET	_D7SD	= _DISP7_BIT_SEG_D
	.SET	_D7SE	= _DISP7_BIT_SEG_E
	.SET	_D7SF	= _DISP7_BIT_SEG_F
	.SET	_D7SG	= _DISP7_BIT_SEG_G
	.SET	_D7SP	= _DISP7_BIT_SEG_P
	;+------------------------------+
	;| DIGITS ENABLE DEFINITIONS	|
	;+------------------------------+--------------------------------------------------------------------------------------------
	.SET	_D7DIG_DISABLE=(_D7DF<<_D7D7) | (_D7DF<<_D7D6) | (_D7DF<<_D7D5) | (_D7DF<<_D7D4) | (_D7DF<<_D7D3) | (_D7DF<<_D7D2) | (_D7DF<<_D7D1) | (_D7DF<<_D7D0)
	.SET	_D7SEG_DISABLE=(_D7SFF<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	.SET	_D7DIG0=(_D7DF<<_D7D7) | (_D7DF<<_D7D6) | (_D7DF<<_D7D5) | (_D7DF<<_D7D4) | (_D7DF<<_D7D3) | (_D7DF<<_D7D2) | (_D7DF<<_D7D1) | (_D7DO<<_D7D0)
	.SET	_D7DIG1=(_D7DF<<_D7D7) | (_D7DF<<_D7D6) | (_D7DF<<_D7D5) | (_D7DF<<_D7D4) | (_D7DF<<_D7D3) | (_D7DF<<_D7D2) | (_D7DO<<_D7D1) | (_D7DF<<_D7D0)
	.SET	_D7DIG2=(_D7DF<<_D7D7) | (_D7DF<<_D7D6) | (_D7DF<<_D7D5) | (_D7DF<<_D7D4) | (_D7DF<<_D7D3) | (_D7DO<<_D7D2) | (_D7DF<<_D7D1) | (_D7DF<<_D7D0)
	.SET	_D7DIG3=(_D7DF<<_D7D7) | (_D7DF<<_D7D6) | (_D7DF<<_D7D5) | (_D7DF<<_D7D4) | (_D7DO<<_D7D3) | (_D7DF<<_D7D2) | (_D7DF<<_D7D1) | (_D7DF<<_D7D0)
	.SET	_D7DIG4=(_D7DF<<_D7D7) | (_D7DF<<_D7D6) | (_D7DF<<_D7D5) | (_D7DO<<_D7D4) | (_D7DF<<_D7D3) | (_D7DF<<_D7D2) | (_D7DF<<_D7D1) | (_D7DF<<_D7D0)
	.SET	_D7DIG5=(_D7DF<<_D7D7) | (_D7DF<<_D7D6) | (_D7DO<<_D7D5) | (_D7DF<<_D7D4) | (_D7DF<<_D7D3) | (_D7DF<<_D7D2) | (_D7DF<<_D7D1) | (_D7DF<<_D7D0)
	.SET	_D7DIG6=(_D7DF<<_D7D7) | (_D7DO<<_D7D6) | (_D7DF<<_D7D5) | (_D7DF<<_D7D4) | (_D7DF<<_D7D3) | (_D7DF<<_D7D2) | (_D7DF<<_D7D1) | (_D7DF<<_D7D0)
	.SET	_D7DIG7=(_D7DO<<_D7D7) | (_D7DF<<_D7D6) | (_D7DF<<_D7D5) | (_D7DF<<_D7D4) | (_D7DF<<_D7D3) | (_D7DF<<_D7D2) | (_D7DF<<_D7D1) | (_D7DF<<_D7D0)
	;+----------------------+
	;| DIGITS ENABLE TABLE	|
	;+----------------------+----------------------------------------------------------------------------------------------------
	_DISP7_DIGITS_TABLE:
		.DB	_D7DIG0,_D7DIG1
		.DB	_D7DIG2,_D7DIG3
		.DB	_D7DIG4,_D7DIG5
		.DB	_D7DIG6,_D7DIG7
	;+------------------------------+
	;| SEGMENTS DEFINTIONS TABLE	|
	;+------------------------------+--------------------------------------------------------------------------------------------
	_DISP7_SEGMENTS_TABLE:
	;ASCII SPACE 	     a
	;		   ----
	;		  |    |
	;		f |    | b
	;		  |  g |
	;		   ---- 
	;		  |    |
	;		e |    | c
	;		  |  d |
	;		   ---- 
	.DB	' ',(_D7SFF<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII . 	     a
	;		   ----
	;		  |    |
	;		f |    | b
	;		  |  g |
	;		   ---- 
	;		  |    |
	;		e |    | c
	;		  |  d |
	;		   ----  ##
	.DB	'.',(_D7SFF<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SON<<_D7SP)
	;ASCII - 	     a
	;		   ----
	;		  |    |
	;		f |    | b
	;		  |  g |
	;		   #### 
	;		  |    |
	;		e |    | c
	;		  |  d |
	;		   ---- 
	.DB	'-',(_D7SFF<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 0 	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   ---- 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   #### 
	.DB	'0',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 1 	     a
	;		   ----
	;		  |    #
	;		f |    # b
	;		  |  g #
	;		   ---- 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   ---- 
	.DB	'1',(_D7SFF<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 2 	     a
	;		   ####
	;		  |    #
	;		f |    # b
	;		  |  g #
	;		   #### 
	;		  #    |
	;		e #    | c
	;		  #  d |
	;		   #### 
	.DB	'2',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 3 	     a
	;		   ####
	;		  |    #
	;		f |    # b
	;		  |  g #
	;		   #### 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   #### 
	.DB	'3',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 4 	     a
	;		   ----
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   ---- 
	.DB	'4',(_D7SFF<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 5 	     a
	;		   ####
	;		  #    |
	;		f #    | b
	;		  #  g |
	;		   #### 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   #### 
	.DB	'5',(_D7SON<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 6 	     a
	;		   ####
	;		  #    |
	;		f #    | b
	;		  #  g |
	;		   #### 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   #### 
	.DB	'6',(_D7SON<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 7 	     a
	;		   ####
	;		  |    #
	;		f |    # b
	;		  |  g #
	;		   ---- 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   ---- 
	.DB	'7',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 8 	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   #### 
	.DB	'8',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII 9	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   #### 
	.DB	'9',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII A 	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   ---- 
	.DB	'A',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII B 	     a
	;		   ----
	;		  #    |
	;		f #    | b
	;		  #  g |
	;		   #### 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   #### 
	.DB	'B',(_D7SFF<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII C 	     a
	;		   ####
	;		  #    |
	;		f #    | b
	;		  #  g |
	;		   ---- 
	;		  #    |
	;		e #    | c
	;		  #  d |
	;		   #### 
	.DB	'C',(_D7SON<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII D 	     a
	;		   ----
	;		  |    #
	;		f |    # b
	;		  |  g #
	;		   #### 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   #### 
	.DB	'D',(_D7SFF<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII E 	     a
	;		   ####
	;		  #    |
	;		f #    | b
	;		  #  g |
	;		   #### 
	;		  #    |
	;		e #    | c
	;		  #  d |
	;		   #### 
	.DB	'E',(_D7SON<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII F 	     a
	;		   ####
	;		  #    |
	;		f #    | b
	;		  #  g |
	;		   #### 
	;		  #    |
	;		e #    | c
	;		  #  d |
	;		   ---- 
	.DB	'F',(_D7SON<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII G 	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   #### 
	.DB	'G',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII H 	     a
	;		   ----
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   ---- 
	.DB	'H',(_D7SFF<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII I 	     a
	;		   ----
	;		  |    #
	;		f |    # b
	;		  |  g #
	;		   ---- 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   ---- 
	.DB	'I',(_D7SFF<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII J 	     a
	;		   ----
	;		  |    #
	;		f |    # b
	;		  |  g #
	;		   ---- 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   #### 
	.DB	'J',(_D7SFF<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII L 	     a
	;		   ----
	;		  #    |
	;		f #    | b
	;		  #  g |
	;		   ---- 
	;		  #    |
	;		e #    | c
	;		  #  d |
	;		   #### 
	.DB	'L',(_D7SFF<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII N 	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   ---- 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   ---- 
	.DB	'N',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII O 	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   ---- 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   #### 
	.DB	'O',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII P 	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  #    |
	;		e #    | c
	;		  #  d |
	;		   ---- 
	.DB	'P',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII Q 	     a
	;		   ####
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   ---- 
	.DB	'Q',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII R 	     a
	;		   ####
	;		  #    #
	;		f #    #b
	;		  #  g #
	;		   ---- 
	;		  #    |
	;		e #    | c
	;		  #  d |
	;		   ---- 
	.DB	'R',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII S 	     a
	;		   ####
	;		  #    |
	;		f #    | b
	;		  #  g |
	;		   #### 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   #### 
	.DB	'S',(_D7SON<<_D7SA) | (_D7SFF<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII U 	     a
	;		   ----
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   ---- 
	;		  #    #
	;		e #    # c
	;		  #  d #
	;		   #### 
	.DB	'U',(_D7SFF<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SON<<_D7SF) | (_D7SFF<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII Y	     a 
	;		   ----
	;		  #    #
	;		f #    # b
	;		  #  g #
	;		   #### 
	;		  |    #
	;		e |    # c
	;		  |  d #
	;		   ---- 
	.DB	'Y',(_D7SFF<<_D7SA) | (_D7SON<<_D7SB) | (_D7SON<<_D7SC) | (_D7SFF<<_D7SD) | (_D7SFF<<_D7SE) | (_D7SON<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	;ASCII Z 	     a
	;		   ####
	;		  |    #
	;		f |    # b
	;		  |  g #
	;		   #### 
	;		  #    |
	;		e #    | c
	;		  #  d |
	;		   #### 
	.DB	'Z',(_D7SON<<_D7SA) | (_D7SON<<_D7SB) | (_D7SFF<<_D7SC) | (_D7SON<<_D7SD) | (_D7SON<<_D7SE) | (_D7SFF<<_D7SF) | (_D7SON<<_D7SG) | (_D7SFF<<_D7SP)
	.DB	0	;END OF TABLE