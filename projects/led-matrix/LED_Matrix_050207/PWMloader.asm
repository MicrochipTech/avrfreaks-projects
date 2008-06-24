;-------------------------------------------------------------------
;---------- PWM Loader ---------------------------------------------
;-------------------------------------------------------------------
;RGBstorage 	= ab 0x96....
;edRGBstorage	= ab 0x12a
;RGBpointer		= 0,3,6,9,12,... aktuell laufende position im RGBstorage
;loadPOINTER	= 0...47
;E2Pstart		= 0x9	;speicherbereich ab 0x9

;.....................................................................
;sollen werte ins RGBstorage geschrieben werden ?
_PWM000load:
	
	lds	r17,RGBdoFLAG
	sbrs	r17,LOADe2p		;parameter aus e2p nach edRGBstorage!
	rjmp	_PWM001load
	;laden...
	sbic	eecr,eewe		; EEWE nicht gelöscht ?
	rjmp	_PWM000loadBACK	; warte...
	;szene *2
	lds	yl,szene
	lsl	yl
	clr	yh
	ldi	zl,low(SZENEtable*2)
	ldi	zh,high(SZENEtable*2)
	;+table
	add	zl,yl
	adc	zh,yh
	;...........................
	lpm	;lsb
	mov	r18,r0
	mov	yl,r18
	adiw	zh:zl,1
	lpm	;msb
	mov	r18,r0
	mov	yh,r18
	;berechne adresse
	lds	zl,E2Ppointer	;pointer laden
	clr	zh
	add	yl,zl
	adc	yh,zh
	;-----------------------------------------------------------------
	subi	yl,low(-E2PtableA)
	sbci	yh,high(-E2PtableA)
	out	eearh,yh
	out	eearl,yl		; adressierung e2prom
	sbi	eecr,eere		; bereite übertragung aus e2prom vor 
	in	r18,eedr		; kopiere daten aus e2prom nach r18
	;nun ins edRGBstorage...
	lds	yl,E2Ppointer	;pointer laden
	clr	yh
	subi	yl,low(-edRGBstorage)
	sbci	yh,high(-edRGBstorage)
	st	y,r18
	;pointer nochmal holen
	lds	yl,E2Ppointer	;pointer laden
	;range testen....
	cpi	yl,47
	brlo	_PWM000loadRUN
	clr	yl
	sts	E2Ppointer,yl
	cbr	r17,1<<LOADe2P	;fertig == edRGBstorage komplett gefüllt
	sbr	r17,1<<EDITrun	;ausführen...
	rjmp	_PWM001load		;go!

_PWM000loadRUN:
	inc	yl
	sts	E2Ppointer,yl
	rjmp	_PWM000loadBACK	; warte...loop







;...............................................................................................
;daten im edRGBstorage...
;weitere funkionen:

;A) 1:1   komplettkopie (edRGBstorage) -> (RGBstorage)  time: sequenziell	toggle: ja/nein	ein-,ausdimmen: ja/nein
;B) R,G,B zeilenkopie   (edRGBstorage) -> (RGBstorage)  time: sequenziell	toggle: ja/nein	ein-,ausdimmen: ja/nein
;C) R,G,B einzelkopie	(edRGBstorage) -> (RGBstorage)  time: sequenziell	toggle: ja/nein	ein-,ausdimmen: ja/nein

_PWM001load:
	;-> funktion?
	lds	r20,RGBfunkB
	lds	r18,RGBfunkA	;funktionsflags laden
	tst	r18
	breq	_PWM000loadBACKz	;keine funktion...
	sbrc	r18,fullCOPY
	rjmp	_PWM002load		;1:1 kopie
	sbrc	r18,lineCOPY	;
	rjmp	_PWM003loadX	;line kopie










_PWM000loadBACKz:
	rjmp	_PWM000loadBACK

_PWM003loadX:
	rjmp	_PWM003load








;.............................................................................................
;1:1 kopie
;funktionen: 	* spontan / sequenziell 
;			* kein toggeln / toggeln
;			* kein dimmen / aus-,eindimmen
_PWM002load:
	
	sbrs	r18,sequFU		;sequnzielles copieren (timer)
	rjmp	_PWM002loadB	;=nein
	;ja!
	;sequenz counter flag prüfen...
	sbrs	r17,SEQUslot
	rjmp	_PWM000loadBACK	;nicht gesetzt == zurück
_PWM002loadB:
	;flag löschen
	cbr	r17,1<<SEQUslot
	;edit run prüfen
	sbrs	r17,EDITrun
	rjmp	_PWM000loadBACK	;nicht gesetzt == zurück
	;toggeln?
	sbrs	r18,toggleFU	;bei endwert toggeln?
	rjmp	_PWM002loadK	;=nein













;------------------------------------------------------------------
	;toggeln
_PWM002loadC:
	;led adressieren
	clr	yh
	lds	yl,loadPOINTERed	;0...47 
	subi	yl,low(-edRGBstorage)
	sbci	yh,high(-edRGBstorage)
	ld	r19,y			;wert aus ed-storage
	clr	yh
	lds	yl,loadPOINTER	;0...47
	subi	yl,low(-RGBstorage)
	sbci	yh,high(-RGBstorage)

						;++++++++++++++++++++++++++++++++++++++++++++++++++
						;u/d-dimmen?
						sbrs	r18,dimmFU		;dimmen?
						rjmp	_PWM002loadCX	;=nein
						ld	zl,y			;aktuellen wert holen
						sbrc	r20,togDIM		
						rjmp	_PWM002loadCup	;=1 neuen wert eindimmen
						;0=altenwert runter dimmen (pwmmax)
						cpi	zl,pwmMAX
						brsh	_PWM002loadCready	;erreicht...
						inc	zl
						st	y,zl			;speichern
						rjmp	_PWM000loadBACK	;== zurück

					_PWM002loadCready:
						sbr	r20,1<<togDIM
						ldi	zl,pwmMAX
						sts	helpVARa,zl
						rjmp	_PWM002loadC
						;=1 neuen wert eindimmen
					_PWM002loadCup:
						cpi	r19,pwmMAX
						brlo	_PWM002loadCnxt
						cbr	r20,1<<togDIM
						rjmp	_PWM002loadCX

					_PWM002loadCnxt:
						lds	zl,helpVARa
						cp	r19,zl
						brlo	_PWM002loadCY
						cbr	r20,1<<togDIM
						rjmp	_PWM002loadD

					_PWM002loadCY:
						dec	zl
						sts	helpVARa,zl
						clr	yh
						lds	yl,loadPOINTER	;0...47
						subi	yl,low(-RGBstorage)
						sbci	yh,high(-RGBstorage)
						st	y,zl
						rjmp	_PWM000loadBACK	;== zurück
						;++++++++++++++++++++++++++++++++++++++++++

_PWM002loadCX:
	st	y,r19			;wert ins dynamische RGB-RAM









	;----------------------------------------------------------------
_PWM002loadD:
	;pointer prüfen
	lds	yh,loadPOINTER	;0...47 
	lds	yl,loadPOINTERed	;0...47 
	tst	yl
	brne	_PWM002loadE
	tst	yh
	brne	_PWM002loadE

			;-----------------------------------------------------
			;beide counter sind 0 
			;-----------------------------------------------------
			sbrc	r20,AtogFL	
			rjmp	_PWM002loadDD	;A=1
			sbrc	r20,BtogFL	
			rjmp	_PWM002loadDDD	;A=0 / B=1

			;-----------------------------------------------------
			;beide flags =0
			;anschließend synchron hochzählen
			;flag =0
			ldi	yl,1			;yl - loadPOINTERed = 1
			ldi	yh,1			;yh - loadPOINTER   = 1
			sts	loadPOINTER,yh
			sts	loadPOINTERed,yl	
			rjmp	_PWM000loadBACK	;== zurück

			;-----------------------------------------------------
			;AtogFL=1
			;BtogFL=?
		_PWM002loadDD:
			sbrc	r20,BtogFL	
			rjmp	_PWM002loadDDX	;A=1 / B=1
			;B=0
			rjmp	_PWM000loadBACK	;== zurück
		_PWM002loadDDX:
			cbr	r20,1<<AtogFL
			cbr	r20,1<<BtogFL
			rjmp	_PWM002loadD

			;-----------------------------------------------------
			;AtogFL=0
			;BtogFL=1
		_PWM002loadDDD:
			sbr	r20,1<<AtogFL
			ldi	yl,0			;yl - loadPOINTERed = 0
			ldi	yh,47			;yh - loadPOINTER   = 47
			sts	loadPOINTER,yh
			sts	loadPOINTERed,yl	
			rjmp	_PWM000loadBACK	;== zurück








	;-------------------------------------------------------
	;counter u/d
	;loadPOINTERed >0
	;loadPOINTER    ?
	;-------------------------------------------------------
_PWM002loadE:
	tst	yh
	breq	_PWM002loadEX	;loadPOINTER = 0
	;loadPOINTER > 0
	;flags testen wg.ud
	sbrc	r20,AtogFL
	rjmp	_PWM002loadEW	;A=1
	sbrc	r20,BtogFL
	rjmp	_PWM002loadEZ	;A=0 B=1
	

	;--------------------------------------
	;beide up bis max
	;AtogFL=0
	;BtogFL=0
	cpi	yl,47
	brlo	_PWM002loadEY
	ldi	yl,47
	clr	yh
	sts	loadPOINTER,yh
	sts	loadPOINTERed,yl	
	sbr	r20,1<<AtogFL
	rjmp	_PWM000loadBACK	;== zurück	

_PWM002loadEY:
	inc	yl
	inc	yh
	sts	loadPOINTER,yh
	sts	loadPOINTERed,yl	
	rjmp	_PWM000loadBACK	;== zurück	
	
	

	;------------------------------------------
_PWM002loadEX:
	;loadPOINTER = 0
	;loadPOINTERed >0
	;flags testen wg.ud
	sbrs	r20,AtogFL
	rjmp	_PWM000loadBACK	;== zurück	
	sbrc	r20,BtogFL
	rjmp	_PWM002loadEXZ	;A=1 B=1
	;A=1 / B=0
_PWM002loadEXX:
	dec	yl
	inc	yh
	sts	loadPOINTER,yh
	sts	loadPOINTERed,yl	
	rjmp	_PWM000loadBACK	;== zurück	




	;---------------------------------------------
	;loadPOINTER = 0
	;loadPOINTERed >0
	;flags testen wg.ud
	;A=1 B=1
_PWM002loadEXZ:
	ldi	yl,0
	ldi	yh,0
	sts	loadPOINTER,yh
	sts	loadPOINTERed,yl	
	cbr	r20,1<<AtogFL
	cbr	r20,1<<BtogFL
	rjmp	_PWM000loadBACK	;== zurück	
		









	;--------------------------------------
	;u/d ?
	;AtogFL=1
	;BtogFL=?
_PWM002loadEW:
	sbrc	r20,BtogFL
	rjmp	_PWM002loadEWX	;A=1 B=1
	;A=1 B=0
	tst	yl
	brne	_PWM002loadEWZ
	ldi	yl,47
	ldi	yh,47
	sts	loadPOINTER,yh
	sts	loadPOINTERed,yl	
	cbr	r20,1<<AtogFL
	sbr	r20,1<<BtogFL
	rjmp	_PWM000loadBACK	;== zurück	

_PWM002loadEWZ:
	dec	yl
	inc	yh
	sts	loadPOINTER,yh
	sts	loadPOINTERed,yl	
	rjmp	_PWM000loadBACK	;== zurück		






	;----------------------------------------
	;A=1 B=1
_PWM002loadEWX:
	tst	yh
	brne	_PWM002loadEWW
	ldi	yl,0
	clr	yh
	sts	loadPOINTER,yh
	sts	loadPOINTERed,yl	
	cbr	r20,1<<AtogFL
	cbr	r20,1<<BtogFL
	rjmp	_PWM000loadBACK	;== zurück		

_PWM002loadEWW:
	inc	yl
	dec	yh
	sts	loadPOINTER,yh
	sts	loadPOINTERed,yl	
	rjmp	_PWM000loadBACK	;== zurück		





				;------------------------------------------
				;A=0 B=1
			_PWM002loadEZ:
				tst	yl
				brne	_PWM002loadEZZ
				;yl=0
				ldi	yl,0
				ldi	yh,47
				sts	loadPOINTER,yh
				sts	loadPOINTERed,yl	
				sbr	r20,1<<AtogFL
				sbr	r20,1<<BtogFL
				rjmp	_PWM000loadBACK	;== zurück		

			_PWM002loadEZZ:
				dec	yl
				dec	yh
				sts	loadPOINTER,yh
				sts	loadPOINTERed,yl	
				rjmp	_PWM000loadBACK	;== zurück		













;------------------------------------------------------------------
	;nicht toggeln
_PWM002loadK:
	;pointer prüfen
	clr	yh
	lds	yl,loadPOINTER	;0...47
	cpi	yl,48
	brlo	_PWM002loadL
	;nein = fertig!
	;pointer auf 0
	clr	yl
	sts	loadPOINTER,yl
	sts	loadPOINTERed,yl
	cbr	r17,1<<EDITrun
	rjmp	_PWM000loadBACK	;nicht gesetzt == zurück

_PWM002loadL:
	;led adressieren
	subi	yl,low(-edRGBstorage)
	sbci	yh,high(-edRGBstorage)
	ld	r19,y			;wert aus ed-storage
	clr	yh
	lds	yl,loadPOINTER	;0...47
	subi	yl,low(-RGBstorage)
	sbci	yh,high(-RGBstorage)
	st	y,r19			;wert ins dynamische RGB-RAM
	;pointer +1
	lds	yl,loadPOINTER	;0...47
	inc	yl
	sts	loadPOINTER,yl
	rjmp	_PWM000loadBACK	;nicht gesetzt == zurück









;.............................................................................................
;zeile (ohne tabelle)
;funktionen: 	* spontan / sequenziell 
;			* kein toggeln / toggeln
;			* kein dimmen / aus-,eindimmen
_PWM003load:	
	sbrs	r18,sequFU		;sequnzielles copieren (timer)
	rjmp	_PWM003loadB	;=nein
	;ja!
	;sequenz counter flag prüfen...
	sbrs	r17,SEQUslot
	rjmp	_PWM000loadBACK	;nicht gesetzt == zurück
_PWM003loadB:
	;flag löschen
	cbr	r17,1<<SEQUslot
	;edit run prüfen
	sbrs	r17,EDITrun
	rjmp	_PWM000loadBACK	;nicht gesetzt == zurück


;------------------------------------------------------------------




						;++++++++++++++++++++++++++++++++++++++++++++++++++
					_PWM003loadC:
						lds	yl,loadPOINTER
						lds	r22,helpVARb
						tst	r22
						brne	_PWM003loadCNXTa
						rjmp	_PWM003loadCredNXT	;0=rot
					_PWM003loadCNXTa:
						cpi	r22,1
						brne	_PWM003loadCNXTb
						rjmp	_PWM003loadCgreenNXT	;1=grün
					_PWM003loadCNXTb:
						cpi	r22,2
						brne	_PWM003loadCNXTc
						rjmp	_PWM003loadCblueNXT	;2=blau
					_PWM003loadCNXTc:
						;toggeln?
						sbrs	r18,toggleFU	;bei endwert toggeln?
						cbr	r17,1<<EDITrun	;=nein	
						clr	r22
						sts	helpVARb,r22
						rjmp	_PWM000loadBACK		;== zurück


						;---------------------------------------------------
					_PWM003loadCredNXT:	
						;rot
						lds	zl,helpVARred
						lds	r19,helpVARa		;led counter
						tst	r19
						brne	_PWM003loadCredX
						;=0
						ldi	yl,0				;Rot beginnt bei 0
					_PWM003loadCredX:
						rcall	_PWM003loadCud
						sts	helpVARred,zl						
						rjmp	_PWM000loadBACK	;== zurück
						;--------------------------------------------------




						;------------------------------------------------------
					_PWM003loadCgreenNXT:	
						lds	zl,helpVARgreen
						lds	r19,helpVARa		;led counter
						tst	r19
						brne	_PWM003loadCgreenX
						;=0
						ldi	yl,1				;grün beginnt bei 1
					_PWM003loadCgreenX:
						rcall	_PWM003loadCud
						sts	helpVARgreen,zl						
						rjmp	_PWM000loadBACK	;== zurück
						;------------------------------------------------------




						;------------------------------------------------------
					_PWM003loadCblueNXT:	
						lds	zl,helpVARblue
						lds	r19,helpVARa		;led counter
						tst	r19
						brne	_PWM003loadCblueX
						;=0
						ldi	yl,2				;blau beginnt bei 2
					_PWM003loadCblueX:
						rcall	_PWM003loadCud
						sts	helpVARblue,zl						
						rjmp	_PWM000loadBACK	;== zurück
						;------------------------------------------------------









						;******************************************************
					_PWM003loadCud:
						cpi	r19,16
						brsh	_PWM003loadCrgbNXT	;nächste dimmstufe der zeile
						mov	r23,yl
						clr	yh
						subi	yl,low(-RGBstorage)
						sbci	yh,high(-RGBstorage)
						st	y,zl				;speichern
						inc	r19
						sts	helpVARa,r19		;led counter +1
						;pointer +3
						ldi	yh,3
						add	yh,r23
						sts	loadPOINTER,yh
						ret

					_PWM003loadCrgbNXT:						
						clr	r19
						sts	helpVARa,r19
						;pointer =0
						clr	yh
						sts	loadPOINTER,yh
						;
						;toggle prüfen
						sbrc	r20,togDIM		
						rjmp	_PWM003loadCrgbNXTup	;=eindimmen
						;letzte funktion: ausdimmen						
						;pwm prüfen
						cpi	zl,pwmMAX
						brlo	_PWM003loadCrgbNXTdw	;<pwmMAX
						;nächste farbe ..
						lds	r22,helpVARb
						inc	r22
						sts	helpVARb,r22
						sbr	r20,1<<togDIM	;nächste zeile eindimmen
						ret
				

				_PWM003loadCrgbNXTdw:
						;----------------------------------
						;u/d-dimmen?
						sbrs	r18,dimmFU		;dimmen?
						rjmp	_PWM003loadCrgbNXTdwNODIM	;=nein
						inc	zl
						ret
				_PWM003loadCrgbNXTdwNODIM:
						ldi	zl,pwmMAX
						ret
				




				
				
				_PWM003loadCrgbNXTup:
						;letzte funktion: eindimmen						
						;pwm prüfen
						tst	zl
						breq	_PWM003loadCrgbNXTupX	;=0 max
						;----------------------------------
						;u/d-dimmen?
						sbrs	r18,dimmFU		;dimmen?
						rjmp	_PWM003loadCrgbNXTupNODIM	;=nein
						dec	zl
						ret
				_PWM003loadCrgbNXTupNODIM:
						ldi	zl,0
						ret


				_PWM003loadCrgbNXTupX:
						sbrs	r18,blendFU		;überblenden?
						rjmp	_PWM003loadCrgbNXTupNOB
						;nächste farbe überblenden ..
						lds	r22,helpVARb
						inc	r22
						sts	helpVARb,r22
						cbr	r20,1<<togDIM	;nächste farbe eindimmen
						ret

				_PWM003loadCrgbNXTupNOB:
						cbr	r20,1<<togDIM	;nächste zeile eindimmen
						ret
						;*******************************************************





;.............................................................................................
;zurück...
_PWM000loadBACK:
	;sequenz counter prüfen...
	lds	yl,TEcntL
	lds	yh,TEcntH
	lds	zl,cntTIM
	clr	zh
	cp	yl,zl
	cpc	yh,zh
	breq	_PWM000loadBACKb		;erreicht == nächster schritt
	adiw yh:yl,1
	rjmp	_PWM000loadBACKc
_PWM000loadBACKb:
	sbr	r17,1<<SEQUslot
	clr	yl
	clr	yh
	;+++neu+++++++++++
	;cnt wert laden
	lds	zl,cntTIMte
	sts	cntTIM,zl
	;+++++++++++++++++
	;+++neu+++++++++++
	;cnt wert laden
	lds	zl,cntSZENte
	sts	cntSZEN,zl
	;+++++++++++++++++

_PWM000loadBACKc:	
	sts	TEcntL,yl
	sts	TEcntH,yh
	sts	RGBdoFLAG,r17
	sts	RGBfunkB,r20
	ret
