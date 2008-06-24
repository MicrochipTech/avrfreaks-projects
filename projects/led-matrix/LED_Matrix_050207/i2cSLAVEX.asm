;...........................................................................
;........ i2c slave modul ..................................................
;...........................................................................

;................................................................
;i2c slave interface
;i2cFLAG		=	0x79		;i2c status flags
;flags
;i2cCST		=	0		;clock stretching

;i2cNACK		=	2		;NACK
;i2cACK		=	3		;acknowledge senden
;i2cSDA		=	4		;temp. ablage pinlevel
;i2cSCL		=	5		; "         "        "
;i2cACTIV		=	6		;kommunikation aktiv
;i2cBUSY		=	7		;bus nicht frei
;
;
;i2c if
;sclPIN		= 	6					; SCL    Pin#6 (portd)
;sdaPIN		= 	7					; SDA    Pin#7 (portd)


;i2cTIMER		=	0x7a		;timer	
;i2cADR		=	0x7b		;i2c slaveadress
;i2bBcnt		=	0x7c		;bit counter
;i2cDAT		=	0x7d		;data

;................................................................
.cseg
_i2c000A:
	lds	r17,i2cFLAG		;status holen
	;kommunikation aktiv?
	sbrs	r17,i2cACTIV
	rjmp	_i2c001A		;0= nicht aktiv - warte auf start condition
	;aktiv!
	;deviceadresse bereits
	;empfangen - verifiziert!
	;acknowledge?
	sbrc	r17,i2cACK
	rjmp	_i2c003AX		;acknowledge....
	rjmp	_i2c002A		;empfang....

	




	
	;..........................................................
	;warte auf start-condition
	;voraussetzung: sda/scl waren beim letzten mal high <=> i2cBUSY=0 !
_i2c001A:
	sbrc	r17,i2cBUSY
	rjmp	_i2c010A		;kommunikation mit anderem slave <=> warte bis bus frei ist

	;......................................
	sbic	pind,sdaPIN		   
	rjmp	_i2c001C		;nicht gut 
	rjmp	_i2cSTAdet		;start !

	;......................................
	;sda =1 <=> keine aktivität !
_i2c001C:	
	;aktuelle pegel sichern
	sbr	r17,1<<i2cSDA	;SDA high	
	cbr	r17,1<<i2cBUSY
	rjmp	_i2c999BACK		;zurück...<=> keine aktivität !



	;..............................................
	;start ok...
_i2cSTAdet:
	clr	r17
	sbr	r17,1<<i2cACTIV	;kommunikation aktiv
	sbr	r17,1<<i2cBUSY	;bus nun belegt
	;pegel aktuallisieren
	cbr	r17,1<<i2cSDA	;SDA low

	;counter etc. vorbereiten
	clr	r18
	sts	i2cTIMER,r18	;timeout counter =0
	sts	i2bBcnt,r18		;bit counter=0
	sts	i2cDAT,r18		;data clr
	sts	i2cSTATE,r18	;status =0

	;..........................................
	;clock stretching...
	cbi	portd,sclPIN	;auf 0 ziehen...
	sbi	ddrd,sclPIN
	cbr	r17,1<<i2cCST	;stretch scl
	cbr	r17,1<<i2cSCL	;SCL low
	rjmp	_i2c999BACK		;zurück...<=> keine aktivität !
	;............................................................









	;............................................................
	;empfange data
	;eingangsvoraussetzung: i2cACTIV = 1
	;		      
	;achtung: clockstretching !
_i2c002A:
	;wenn i2cCST=1 dann noch einen slot (32µs)
	;scl auf low halten - dann lesen !
	;
	sbrs	r17,i2cCST		
	rjmp	_i2c002GO		;bit=0
	cbr	r17,1<<i2cCST	;beim nächsten aufruf daten holen	
	rjmp	_i2c999BACK		;zurück

	;daten holen....
_i2c002GO:
	;bit counter laden
	lds	r18,i2cDAT
	sbi	portd,pind5
	nop
	cbi	ddrd,sclPIN
	sbi	portd,sclPIN	;scl=1 (Open Collector)
	nop
	nop	
	sbis	pind,sclPIN		
	rjmp	_i2c002F		;nicht gut - scl =0 (da ist jemand ncoh nicht soweit...)

_i2c002AZ:
	;scl=1
	sbis	pind,sdaPIN		
	rjmp	_i2c002B		;bit=0
	sec
	rjmp	_i2c002C
_i2c002B:
	clc				;clear carry
_i2c002C:
	rol	r18
	;.............................................
	;clockstretching während bit-bang
	cbi	portd,sclPIN	;scl=0 
	sbi	ddrd,sclPIN
	cbr	r17,1<<i2cCST	;TEST ,sbr'einen slot abwarten		
	cbr	r17,1<<i2cSCL
	;..............................................
	sts	i2cDAT,r18

	;timeout counter =0
	clr	r18
	sts	i2cTIMER,r18
	;bit counter laden
	lds	r18,i2bBcnt
	cpi	r18,7
	brsh	_i2c002E		;erreicht...
	inc	r18
	sts	i2bBcnt,r18
	rjmp	_i2c999BACK		;zurück





	;............................................
	;hat byte empfangen...
_i2c002E:
	clr	r18
	sts	i2bBcnt,r18		;bitcounter =0

	lds	r18,i2cSTATE	;status prüfen
	tst	r18
	brne	_i2c002EA
	rjmp	_i2c002EXA
_i2c002EA:	
	cpi	r18,1
	brne	_i2c002EB
	rjmp	_i2c002EXB
_i2c002EB:	
	cpi	r18,2
	brne	_i2c002EC
	rjmp	_i2c002EXC		
_i2c002EC:	
	cpi	r18,3
	brne	_i2c002ED
	rjmp	_i2c002EXD
_i2c002ED:	
	cpi	r18,4
	brne	_i2c002EE
	rjmp	_i2c002EXE	
_i2c002EE:	
	cpi	r18,5
	brne	_i2c002EF
	rjmp	_i2c002EXF
_i2c002EF:	
	cpi	r18,6
	brne	_i2c002EG
	rjmp	_i2c002EXG
	;out of range
_i2c002EG:
	clr	r17
	sbr	r17,1<<i2cBUSY	
	rjmp	_i2c999BACK		;zurück		
				




				;........................
				;=0 adresse checken
			_i2c002EXA:
				lds	r19,i2cADR		;8bit adressvariable holen xxxxxxx0 write
				lds	r20,i2cDAT
				cp	r19,r20
				brne	_i2c002Enoequ
				inc	r18
				sts	i2cSTATE,r18	;state =1
				;.............................................
				;cbi	portd,sdaPIN	;auf 0 ziehen...
				;sbi	ddrd,sdaPIN
				;..............................................
				rjmp	_i2c003A
			
				
				
				;.................................................
				;nicht ok- kein acknowledge etc...
			_i2c002Enoequ:
				cbr	r17,1<<i2cCST	;kein stretch scl
				cbr	r17,1<<i2cACTIV	;kommunikation nicht mehr aktiv
				sbr	r17,1<<i2cBUSY	;bus nun belegt
				;counter etc. vorbereiten
				clr	r18
				sts	i2cTIMER,r18	;timeout counter =0
				sts	i2bBcnt,r18		;bit counter=0
				sts	i2cDAT,r18		;data clr
				sts	i2cSTATE,r18	;status =0
				cbi	ddrd,sclPIN
				sbi	portd,sclPIN	;scl=1 (Open Collector)
				;cbi	portd,pind5
				rjmp	_i2c999BACK		;zurück
				;.................................................



				
				
				
				;.................................................
				;=1 funktionsbyte empfangen
			_i2c002EXB:
				inc	r18
				sts	i2cSTATE,r18	;state =2
				;funktionsbyte laden
				lds	r20,i2cDAT
				sts	RGBfunkA,r20
				;.............................................
				;cbi	portd,sdaPIN	;auf 0 ziehen...
				;sbi	ddrd,sdaPIN
				;..............................................
				rjmp	_i2c003A


		
				;.................................................
				;=2 loaderbyte empfangen
			_i2c002EXC:
				inc	r18
				sts	i2cSTATE,r18	;state =3
				;loaderbyte laden
				lds	r20,i2cDAT
				sts	RGBdoFLAG,r20
				;.............................................
				;cbi	portd,sdaPIN	;auf 0 ziehen...
				;sbi	ddrd,sdaPIN
				;..............................................
				rjmp	_i2c003A



				;.................................................
				;=3 ablauftimer-byte empfangen
			_i2c002EXD:
				inc	r18
				sts	i2cSTATE,r18	;state =4
				;ablauftimer laden
				lds	r20,i2cDAT
				sts	cntTIMte,r20

				;.............................................
				;cbi	portd,sdaPIN	;auf 0 ziehen...
				;sbi	ddrd,sdaPIN
				;..............................................
				rjmp	_i2c003A



				;.................................................
				;=4 szenentimer-byte empfangen
			_i2c002EXE:
				inc	r18
				sts	i2cSTATE,r18	;state =5
				;szenentimer laden
				lds	r20,i2cDAT
				sts	cntSZENte,r20
				;.............................................
				;cbi	portd,sdaPIN	;auf 0 ziehen...
				;sbi	ddrd,sdaPIN
				;..............................................
				rjmp	_i2c003A

			
			
			
				;.................................................
				;=5 szene-byte empfangen
			_i2c002EXF:
				inc	r18
				sts	i2cSTATE,r18	;state =6
				;szenen byte (0-9) laden
				lds	r20,i2cDAT

				cpi	r20,10
				brlo	_i2c002EXFF
				clr	r20
			_i2c002EXFF:
				sts	szeneTE,r20
				;.............................................
				;cbi	portd,sdaPIN	;auf 0 ziehen...
				;sbi	ddrd,sdaPIN
				;..............................................
				rjmp	_i2c003A	;zurück	


				;.................................................
				;=6 helligkeit-byte empfangen <=> anschließend nack!
			_i2c002EXG:
				clr	r18
				sts	i2cSTATE,r18	;state =7
				sts	i2cTIMER,r18	
				;helligkeits byte (0-pwmMAX) laden
				lds	r20,i2cDAT
				cpi	r20,pwmMAX+1
				brlo	_i2c002EXGG
				ldi	r20,pwmMAX
			_i2c002EXGG:
				;umkehren min(0)..max(31)
				com	r20
				andi	r20,0b00011111
				sts	rgbHELLte,r20
				rjmp	_i2c004A	;zurück










	;...............................................
	;warte auf daten... scl =0
	;Bedeutet: Der Master oder ein anderer Slave sind
	;noch nicht soweit...
	;-> diesen zustand bis timeout beobachten !
	;...............................................
_i2c002F:
	;scl ist noch 0
	;clock stretching...
	cbi	portd,sclPIN	;auf 0 ziehen...
	sbi	ddrd,sclPIN
	cbr	r17,1<<i2cCST	;kein stretch scl = daten beim nächsten durchgang
	;aktuelle pegel sichern
	cbr	r17,1<<i2cSCL	;
	;..........................................

	lds	r18,i2cTIMER
	cpi	r18,i2cTIT		;timeout erreicht?
	brsh	_i2c002G		;<timeout
	inc	r18
	sts	i2cTIMER,r18
	rjmp	_i2c999BACK		;zurück

	;timeout !
_i2c002G:
	clr	r18
	sts	i2cTIMER,r18
	cbr	r17,1<<i2cCST	;kein scl stretch
	cbr	r17,1<<i2cACTIV	;kommunikation aktiv
	sbr	r17,1<<i2cBUSY	;bus nun belegt
	cbr	r17,1<<i2cACK	;acknowledge senden
	cbi	ddrd,sclPIN
	sbi	portd,sclPIN	;scl=1 (Open Collector)
	rjmp	_i2c999BACK		;zurück
	;...............................................








	;..................................................................
	;acknowledge senden // entry: sda=? & scl=?
	;i2cSCL=0
	;..................................................................
_i2c003A:
	;.............................................
	cbr	r17,1<<i2cCST	;TEST ,sbr'einen slot abwarten		
	cbr	r17,1<<i2cSCL
	;..............................................
	sbi	portd,pind5
	sbr	r17,1<<i2cACTIV	;kommunikation aktiv
	sbr	r17,1<<i2cBUSY	;bus nun belegt
	sbr	r17,1<<i2cACK	;acknowledge senden
	;sda=0 (ack)
	cbi	portd,sdaPIN	;auf 0 ziehen...
	sbi	ddrd,sdaPIN
	sbr	r17,1<<i2cCST	;kein scl stretch TEST
	cbr	r17,1<<i2cSDA
	cbr	r17,1<<i2cSCL
_i2c003AX:	
	sbrs	r17,i2cCST		
	rjmp	_i2c003AXGO		;bit=0
	cbr	r17,1<<i2cCST	;beim nächsten aufruf daten holen	
	rjmp	_i2c999BACK		;zurück

_i2c003AXGO:
	cbi	ddrd,sclPIN
	sbi	portd,sclPIN	;scl=1 (Open Collector)
	nop
	nop
	nop
	nop
	cbr	r17,1<<i2cSCL
	sbr	r17,1<<i2cSDA
	cbr	r17,1<<i2cACK	;acknowledge fertig
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	clr	r18
	sts	i2cDAT,r18		;data clr
	;clock stretching...
	cbi	portd,sclPIN	;auf 0 ziehen...
	sbi	ddrd,sclPIN
	cbr	r17,1<<i2cCST	;einen slot abwarten	
	nop
	nop
	nop
	nop
	nop
	nop
	cbi	ddrd,sdaPIN
	sbi	portd,sdaPIN	;input pullup
	rjmp	_i2c999BACK		;zurück



	





	;..................................................................
	;NO acknowledge senden // entry: sda=? & scl=?
	;i2cSCL=1
	;..................................................................
_i2c004A:
	cbi	portd,pind5
	;TEST
	cbi	ddrd,sclPIN
	sbi	portd,sclPIN	;scl=1 (Open Collector)

	clr	r17
	sbr	r17,1<<i2cSCL
	sbr	r17,1<<i2cSDA
	sbr	r17,1<<i2cBUSY
	clr	r18
	sts	i2cDAT,r18		;data clr
	rjmp	_i2c999BACK		;zurück








	;....................................................................
	;kommunikation mit anderem slave <=> warte bis bus frei ist
	;müssen scl&sda =1 sein
	;....................................................................
_i2c010A:
	sbis	pind,sclPIN		
	rjmp	_i2c010AX		;nicht gut - scl =0
	sbic	pind,sdaPIN		;gut 	     - scl =1
	rjmp	_i2c010B		;gut - scl =1 & sda =1 <=> keine aktivität !
	rjmp	_i2c010C

	;..........................................
	;scl=0
_i2c010AX:
	cbr	r17,1<<i2cSCL
	sbic	pind,sdaPIN		;
	sbr	r17,1<<i2cSDA	;sda=1
	clr	r18
	sts	i2cTIMER,r18
	rjmp	_i2c999BACK		;zurück

	;.................................................
	;nun bis timeout warten
_i2c010B:
	lds	r18,i2cTIMER
	cpi	r18,i2cTIT		;timeout erreicht?
	brsh	_i2c010BX		;<timeout
	inc	r18
	sts	i2cTIMER,r18
	;aktuelle pegel sichern
	sbr	r17,1<<i2cSCL	;SCL high
	sbr	r17,1<<i2cSDA
	rjmp	_i2c999BACK		;zurück
	;timeout !
_i2c010BX:
	clr	r18
	sts	i2cTIMER,r18
	cbr	r17,1<<i2cBUSY	;bus nun frei !
	rjmp	_i2c001A		;zurück & start prüfen


	;................................................
	;scl=1 / sda=0
_i2c010C:
	sbr	r17,1<<i2cSCL
	cbr	r17,1<<i2cSDA
	clr	r18
	sts	i2cTIMER,r18
	rjmp	_i2c999BACK		;zurück









	;....................................................................
	;zurück
_i2c999BACK:
	sts	i2cFLAG,r17
	ret	
