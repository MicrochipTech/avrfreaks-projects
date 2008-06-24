;----------------------------------------------------------
;Dokumentation
;----------------------------------------------------------

;Funktionen:
.RGBfunkA	<0x6d>

- fullCOPY(.0)		
- lineCOPY(.1)	
- singleCOPY(.2)		
- toggleFU(.3)		
- dimmFU(.4)			
- blendFU(.5)			
- changeSZEN(.6)		
- sequFU(.7)			

;_______________________________________________________________________________________________________
;Funktion	    |Flag -> |sequFU |changeSZEN |blendFU |dimmFU |toggleFU |singleCOPY |lineCOPY |fullCOPY |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Speicher 1:1   |	       |   0   |     0     |    0   |   0   |    0    |     0     |    0    |    1    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Speicher 1:1   |	       |   1   |     0     |    0   |   0   |    0    |     0     |    0    |    1    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;verzögert      |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Speicher 1:1   |	       |   1   |     0     |    0   |   0   |    1    |     0     |    0    |    1    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;verzögert &    |        |       |           |        |       |         |           |         |         |
;wiederholung   |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Speicher 1:1   |	       |   1   |     0     |    0   |   1   |    1    |     0     |    0    |    1    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;verzögert &    |        |       |           |        |       |         |           |         |         |
;wiederholung & |        |       |           |        |       |         |           |         |         |
;aus-,eindimmen |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Zeilenweise    |	       |   0   |     0     |    0   |   0   |    0    |     0     |    1    |    0    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Zeilenweise    |	       |   1   |     0     |    0   |   0   |    0    |     0     |    1    |    0    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;verzögert      |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Zeilenweise    |	       |   1   |     0     |    0   |   0   |    1    |     0     |    1    |    0    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;verzögert &    |        |       |           |        |       |         |           |         |         |
;wiederholung   |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Zeilenweise    |	       |   1   |     0     |    0   |   1   |    1    |     0     |    1    |    0    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;verzögert &    |        |       |           |        |       |         |           |         |         |
;wiederholung & |        |       |           |        |       |         |           |         |         |
;aus-,eindimmen |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|
;Zeilenweise    |	       |   1   |     0     |    1   |   1   |    1    |     0     |    1    |    0    |
;aufs Display   |        |       |           |        |       |         |           |         |         |
;verzögert &    |        |       |           |        |       |         |           |         |         |
;wiederholung & |        |       |           |        |       |         |           |         |         |
;aus-,eindimmen&|        |       |           |        |       |         |           |         |         |
;zeilen überbl. |        |       |           |        |       |         |           |         |         |
;_______________|________|_______|___________|________|_______|_________|___________|_________|_________|





;Display-Setup:
rgbHELL<0x75>
0	 = maximale Helligkeit
pwmMAX = aus

;Timer Szenenwechsel:
cntSZEN <0x76>

;Ablauftimer:
cntTIM <0x77>

;loader flags:
RGBdoFLAG	<0x6b>
.equ	EDITrun(.1)  =gibt funktionen zur ausführung frei
.equ	LOADe2p(.2)  =überträgt e2prom table ins rgb-ram & anschließende ausführung
.equ	SEQUslot(.3) =nicht ändern!

;szene
szene	<0x78>	= 0-9	(je szene 48byte)



;-----------------------------------------------------------------------------------------------------------
;i2c
i2cSLAVEadr		=	0xf8		;1111 100x	;master setzt x=0 write
;INFO: weitere matrix module mit:
;0xfa,0xfc,0xfe adressierbar !

;i2c format:
<S> <adress(w)> <A> <RGBfunkA> <A> <RGBdoFLAG> <A> <cntTIM> <A> <cntSZEN> <A> <szene> <A> <rgbHELL> </A>

;besonderheit:
;Integriertes I2C-Interface arbeitet mit max.15,4 K/s und bestimmt durch clockstretching (scl=0) die
;langsamere kommunikation mit einem schnelleren master.
;-----------------------------------------------------------------------------------------------------------

