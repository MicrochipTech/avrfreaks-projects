; Logic-Sampler and PulseGenerator
; Version 0.15  Neu: PulseGenerator-Modes. Erwartet 7 Bytes!
; Version 0.09  Verkuerzte Routine fuer cntclk und ram_wr
; Version 0.07  Neue Modes fuer Test-Routinen
; Version 0.06  Anpassung an AT90S1200
; Version 0.05  funktionsfaehig fuer AT90S2313
;/hi1201
;
; Nach dem Starten werden folgende Bytes erwartet:         / PulseGen:
; 1. TriggerByte (don't-cares auf 1 setzen!)               / TiL
; 2. Masken-Byte (enthaelt an don't-care Stellen eine 1)   / TiH
; 3. Timer-Intervall                                       / TpL
; 4. Timer-Prescaler                                       / TpH
; 5. Pre-Trigger in 256-Byte Schritten;                    / TnL
; 6. unbenutzt                                             / TnH
; 7. Modus-Byte (int-/ext-Clock, int-/ext-Trigger)         / Mode
;
; Beispiel fuer Triggerword: 0 1 1 0 X X X 1
; TriggerByte:               0 1 1 0 1 1 1 1
; Masken-Byte:               0 0 0 0 1 1 1 0
;
; Sampling-Byte:             0 1 1 0 0 1 1 1
; mit Masken-OR:             0 1 1 0 1 1 1 1 = TriggerByte --> TRIGGER!
;


.nolist
.include ".\INC\1200def.inc"
.list

.def TpCntH  = R27 ;PulseGen: Tp-Counter-High
.def TpCntL  = R26 ;PulseGen: Tp-Counter-Low
.def TiCntH  = R25 ;PulseGen: Ti-Counter-High
.def TiCntL  = R24 ;PulseGen: Ti-Counter-Low
.def temp    = R23 ;temporary storage register
.def bitcnt  = R22 ;bit counter
.def TXbyte  = R21 ;Data to be transmitted
.def RXbyte  = R20 ;Received data
.def bufcntH = R19 ;Ringspeicher-Zaehler High
.def TnCntH  = R19 ;PulseGen: Burst-Count-High
.def bufcntL = R18 ;Ringspeicher-Zaehler Low
.def TnCntL  = R18 ;PulseGen: Burst-Count-Low
.def mode    = R17 ;Modus
.def wreg    = R16 ;Working-Register
.def trig    = R15 ;Trigger-Register
.def TiL     = R15 ;(PulseGen)
.def mask    = R14 ;Masken-Register
.def TiH     = R14 ;(PulseGen)
.def timereg = R13 ;fuer Timervergleich
.def TnH     = R13 ;(PulseGen)
.def prescal = R12 ;Timer PreScaler
.def TpH     = R12 ;(PulseGen)
.def delta   = R11 ;Timer-Intervall
.def TpL     = R11 ;(PulseGen)
.def pretrig = R10 ;Pre-Trigger
.def TnL     = R10 ;(PulseGen)
.def aux1    = R9  ;Hilfsregister
.def aux0    = R8  ;Hilfsregister
.def store1  = R7  ;PORTD, cntclk=H und ram_wr=L
.def store2  = R6  ;PORTD, cntclk=L und ram_wr=H (Ruhezustand)

;---------------------------------------------------------------------------
;Hardware-Vorgaben
;
;Teilerfaktor fuer Baudrate 38400Bd:
.equ  baud  = 66       ;Bei 16MHz Quarztakt (90S1200 uebertaktet!)
;.equ baud  = 48       ;Bei 12MHz Quarztakt (zulaessige Taktfrequ.)
;
;Port-Pinbelegung:
.equ ram_wr = PD5      ;RAM /WR Write-Enable  (Low-aktiv)
.equ ram_oe = PD6      ;RAM /OE Output-Enable (Low-aktiv)
.equ sample = PD4      ;Clock (positiv) und Output-Enable (Low) des 74HC574
.equ PGout  = PD3      ;PulseGenerator Output
.equ cntclk = PD0      ;Clock (positiv) des Ripple-Counters 74HC4040
.equ RxD    = PD2      ;RxD an PD2 wg. INT
.equ TxD    = PD1      ;TxD an PD1
.equ extpin = PB0      ;Externer Trigger-/Clock-Eingang an Port-B
;
;---------------------------------------------------------------------------

.org 0x0000
         rjmp start           ;Sprung nach Reset

.org 0x0001

INT0_External_Interrupt:      ;Interrupt erfolgt nur bei BREAK
         rjmp RAMfull         ;Alle Daten senden

start:
;-------------------------------------------------------------------
;Stackpointer initialisieren
;-------------------------------------------------------------------
; Der Stackpointer kann beim 90S1200 nicht initialisiert werden.
; Da es zum INT0 keinen RETI gibt kommt es zum Stackueberlauf,
; dieser ist aber ohne Bedeutung.

;-------------------------------------------------------------------
;Ports initialisieren
;-------------------------------------------------------------------
         ldi wreg,0b01111011  ;PORT-D Richtungs-Register
         out DDRD,wreg        ;
         ldi wreg,0b11110010  ;PORT-D Ausgabezustand
                              ;cntclk = low
                              ;TxD    = high
                              ;RxD    = Eingang, kein Pull-Up
                              ;PGout  = low
                              ;sample = abgekoppelt
                              ;ram_oe = inaktiv (high)
                              ;ram_wr = inaktiv (high)
         out PORTD,wreg       ;PORT-D setzen
         mov store2,wreg      ;store inaktiv
         ldi wreg,0b11010011  ;cntclk=high, ram_wr=low
         mov store1,wreg      ;store aktiv
         ldi wreg,0b00000000  ;PORT-B Richtungs-Register
         out DDRB,wreg        ;alles Eingaenge
         out PORTB,wreg       ;keine Pull-Up's

;-------------------------------------------------------------------
;Lade Hilfsregister aux0 und aux1, werden fuer ADIW-Ersatz gebraucht
;-------------------------------------------------------------------
         ldi wreg,0x00        ;Hilfsregister-0 mit 0 laden
         mov aux0,wreg        ;"
         ldi wreg,0x01        ;Hilfsregister-1 mit 1 laden
         mov aux1,wreg        ;"

;-------------------------------------------------------------------
;Zeichen von der seriellen Schnittstelle einlesen
;-------------------------------------------------------------------

         rcall GetChar
         mov trig,RXbyte    ; = TiL

         rcall GetChar
         mov mask,RXbyte    ; = TiH

         rcall GetChar
         mov delta,RXbyte   ; = TpL

         rcall GetChar
         mov prescal,RXbyte ; = TpH

         rcall GetChar
         mov pretrig,RXbyte ; = TnL

         rcall GetChar
         mov TnH,RxByte     ; bei LogicAnalyzer ignoriert

         rcall GetChar
         mov mode,RXbyte


;-------------------------------------------------------------------
;INT0 freigeben fuer Break
;-------------------------------------------------------------------
         ldi wreg,0x00        ;Zuerst ev. anstehenden INT loeschen,
         out GIMSK,wreg       ;dazu INT0 sperren
         ldi wreg,0x00        ;und Level-Triggerung einstellen
         out MCUCR,wreg       ;" (Anweisung aus 90S1200-Manual)
                              ;danach die gewuenschte Triggerung:
         ldi wreg,0x02        ;Fallende Flanke an INT0
         out MCUCR,wreg       ;"
         ldi wreg,0x40        ;INT0-Freigabe
         out GIMSK,wreg       ;"
         sei                  ;Globale Freigabe


;-------------------------------------------------------------------
;PulseGenerator-Modes filtern
;-------------------------------------------------------------------
PGMode10:
         cpi mode,10          ;Range-1, Continous
         brne PGMode11
         rjmp PG_Start0
PGMode11:
         cpi mode,11          ;Range-1, Burst
         brne PGMode12
         rjmp PG_Start1
PGMode12:
         cpi mode,12          ;Range-2, Continous
         brne PGMode13
         rjmp PG_Start2
PGMode13:
         cpi mode,13          ;Range-2, Burst
         brne ClearRAM
         rjmp PG_Start3

;-------------------------------------------------------------------
;RAM-Speicher loeschen
;-------------------------------------------------------------------
ClearRAM:
         ldi bufcntL,LOW (0x0000-2048)  ;Alle 2048 Speicherplaetze
         ldi bufcntH,HIGH(0x0000-2048)  ;loeschen

         ldi wreg,0xFF        ;PORTB auf ausgabe
         out DDRB,wreg        ;"
         ldi wreg,0x00        ;Bitmuster anlegen
         out PORTB,wreg       ;"
RAMclr:
         out PORTD,store1     ;RAM schreiben und
         out PORTD,store2     ;Counter increment.
         nop                  ;
         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc RAMclr

         ldi wreg,0x00        ;PORTB auf eingabe
         out DDRB,wreg        ;"
         ldi wreg,0b11111111  ;PORT-B
         out PORTB,wreg       ;Pull-Ups aktivieren
         cbi PORTD,sample     ;register auf BUS legen


;-------------------------------------------------------------------
;Anzahl der Samples nach Trigger einstellen
;z.B.: 0%   PreTrigger-Wert=8, $00-$08=$F8 (high-Byte)
;Sample-Nr.0 ist Triggersample, es muessen noch 2047 Samples folgen.
;Samplezahl=$0000-2047=$F801
;z.B.: 25%  PreTrigger-Wert=6, $00-$06=$FA (high-Byte)
;Sample-Nr. 512 ist Triggersample, es muessen noch 1535 Samples folgen.
;Samplezahl=$0000-1535=$FA01
;-------------------------------------------------------------------
         ldi wreg,0x00
         sub wreg,pretrig
         ldi bufcntL,0x01
         mov bufcntH,wreg


;-------------------------------------------------------------------
;Mode auswerten und passende Routine aufrufen (Logic-Analyzer)
;-------------------------------------------------------------------
Mode0:
         cpi mode,0         ;internal Clock, internal Trigger
         brne Mode1
         rjmp M0_Start
Mode1:
         cpi mode,1         ;external rising Clock, int. Trigger
         brne Mode2
         rjmp M1_Start
Mode2:
         cpi mode,2         ;external falling Clock, int. Trigger
         brne Mode3
         rjmp M2_Start
Mode3:
         cpi mode,3         ;internal Clock, external Trigger=pos
         brne Mode4
         rjmp M3_Start
Mode4:
         cpi mode,4         ;internal Clock, external Trigger=neg
         brne Mode5
         rjmp M4_Start
Mode5:
         cpi mode,5         ;State
         brne Test1
         rjmp M5_Start
Test1:
         cpi mode,7         ;Binary-Counter
         brne Test2
         rjmp T1_Start
Test2:
         cpi mode,8         ;AA-55-Pattern
         brne Test3
         rjmp T2_Start
Test3:
         cpi mode,9         ;Walking-Zero (Periode=9Takte)
         brne M0_Start
         rjmp T3_Start


M0_Start:
;-------------------------------------------------------------------
;MODE=0 internal Clock, internal Trigger
;-------------------------------------------------------------------
;Timer-0 initialisieren
         mov timereg,delta    ;Sampletime-Variable setzen
         out TCCR0,prescal    ;Timer-0 mit passendem PreScaler verbinden
         in temp,TCNT0
M0_synchronisieren:
         in wreg,TCNT0        ;da Prescaler-Stand unbekannt, auf Timer-
         cp wreg,temp         ;sprung synchronisieren
         breq M0_synchronisieren
         ldi wreg,0x00        ;Timer-0 nullsetzen
         out TCNT0,wreg

M0_WaitForTrigger:
M0_TimingLoop1:
         in wreg,TCNT0        ;timer einlesen und mit
         cp timereg,wreg      ;mit samplezeitpunkt vergleichen
         brne M0_TimingLoop1  ;
         add timereg,delta    ;naechsten samplezeitpunkt berechnen
         sbi PORTD,sample     ;sample nehmen und
         cbi PORTD,sample     ;register auf BUS legen
         nop
         in wreg,PINB         ;sample auf triggerung pruefen
         or wreg,mask         ;
         out PORTD,store1
         out PORTD,store2

         cpse wreg,trig       ;war sample=triggerbyte?
         rjmp M0_WaitForTrigger ;nein..

M0_GetSamples:
M0_TimingLoop2:
         in wreg,TCNT0
         cp timereg,wreg
         brne M0_TimingLoop2
         add timereg,delta
         sbi PORTD,sample     ;sample nehmen und
         cbi PORTD,sample     ;register auf BUS legen
         nop
         out PORTD,store1
         out PORTD,store2

         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc M0_GetSamples
         rjmp RAMFull


M3_Start:
;-------------------------------------------------------------------
;MODE=3 internal Clock, externe positive Triggerflanke
;-------------------------------------------------------------------
         out TCCR0,prescal    ;Timer-0 mit passendem PreScaler verbinden
M3_WaitForTrigger:
M3_TriggerLoop1:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbic PINB,extpin     ;warte auf triggerpin=L
         rjmp M3_TriggerLoop1
M3_TriggerLoop2:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbis PINB,extpin     ;warte auf triggerpin=H
         rjmp M3_TriggerLoop2
         in timereg,TCNT0     ;timer einlesen
         add timereg,delta    ;naechsten samplezeitpunkt berechnen
         out PORTD,store1
         out PORTD,store2

         rjmp M0_GetSamples   ;


M4_Start:
;-------------------------------------------------------------------
;MODE=4 internal Clock, externe negative Triggerflanke
;-------------------------------------------------------------------
         out TCCR0,prescal    ;Timer-0 mit passendem PreScaler verbinden
M4_WaitForTrigger:
M4_TriggerLoop1:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbis PINB,extpin     ;warte auf triggerpin=H
         rjmp M4_TriggerLoop1
M4_TriggerLoop2:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbic PINB,extpin     ;warte auf triggerpin=L
         rjmp M4_TriggerLoop2
         in timereg,TCNT0     ;timer einlesen
         add timereg,delta    ;naechsten samplezeitpunkt berechnen
         out PORTD,store1
         out PORTD,store2

         rjmp M0_GetSamples   ;


M1_Start:
;-------------------------------------------------------------------
;MODE=1 external Clock, rising Edge
;-------------------------------------------------------------------
M1_WaitForTrigger:
M1_ClockLoop1:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbic PINB,extpin     ;warte auf clockpin=L
         rjmp M1_ClockLoop1
M1_ClockLoop2:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbis PINB,extpin     ;warte auf clockpin=H
         rjmp M1_ClockLoop2
         in wreg,PINB         ;sample auf triggerung pruefen
         or wreg,mask         ;
         out PORTD,store1
         out PORTD,store2

         cpse wreg,trig       ;war sample=triggerbyte?
         rjmp M1_WaitForTrigger ;nein..
M1_GetSamples:
M1_ClockLoop3:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbic PINB,extpin     ;warte auf clockpin=L
         rjmp M1_ClockLoop3
M1_ClockLoop4:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbis PINB,extpin     ;warte auf clockpin=H
         rjmp M1_ClockLoop4
         out PORTD,store1
         out PORTD,store2

         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc M1_GetSamples
         rjmp RAMFull


M2_Start:
;-------------------------------------------------------------------
;MODE=2 external Clock, falling Edge
;-------------------------------------------------------------------
M2_WaitForTrigger:
M2_ClockLoop1:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbis PINB,extpin     ;warte auf clockpin=H
         rjmp M2_ClockLoop1
M2_ClockLoop2:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbic PINB,extpin     ;warte auf clockpin=L
         rjmp M2_ClockLoop2
         in wreg,PINB         ;sample auf triggerung pruefen
         or wreg,mask         ;
         out PORTD,store1
         out PORTD,store2

         cpse wreg,trig       ;war sample=triggerbyte?
         rjmp M2_WaitForTrigger ;nein..
M2_GetSamples:
M2_ClockLoop3:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbis PINB,extpin     ;warte auf clockpin=H
         rjmp M2_ClockLoop3
M2_ClockLoop4:
         sbi PORTD,sample
         cbi PORTD,sample
         nop
         sbic PINB,extpin     ;warte auf clockpin=L
         rjmp M2_ClockLoop4
         out PORTD,store1
         out PORTD,store2

         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc M2_GetSamples
         rjmp RAMFull


M5_Start:
;-------------------------------------------------------------------
;MODE=5 State-Analyse
;jede Aenderung an den Eingaengen wird erfasst, ohne Zeitbezug
;-------------------------------------------------------------------
         sbi PORTD,sample     ;erstes sample nehmen
         cbi PORTD,sample     ;"
         nop                  ;### ist noetig!
         in temp,PINB         ;fuer Vergleich speichern
         out PORTD,store1
         out PORTD,store2

M5_Loop:
         sbi PORTD,sample     ;sample erfassen
         cbi PORTD,sample     ;"
         nop                  ;### ist noetig!
         in wreg,PINB         ;mit vorgaenger vergleichen
         cp wreg,temp         ;"
         breq M5_Loop         ;
         mov temp,wreg        ;neues vergleichsmuster speichern
         out PORTD,store1
         out PORTD,store2

         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc M5_Loop
         rjmp RAMFull

T1_Start:
;-------------------------------------------------------------------
;Test-Mode-1: Bitpattern LO-Counter
;-------------------------------------------------------------------
         sbi PORTD,sample     ;Sample-Register abkoppeln
         ldi bufcntL,LOW (0x0000-2048)  ;Gesamtes RAM fuellen
         ldi bufcntH,HIGH(0x0000-2048)  ;"
         ldi wreg,0xFF        ;PORTB auf ausgabe
         out DDRB,wreg        ;"
T1_loop:
         out PORTB,bufcntL    ;LO-Counter
         nop                  ;
         out PORTD,store1
         out PORTD,store2

         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc T1_loop
         ldi wreg,0x00        ;PORTB auf eingabe
         out DDRB,wreg        ;"
         ldi wreg,0b11111111  ;PORT-B
         out PORTB,wreg       ;Pull-Ups aktivieren
         rjmp RAMfull

T2_Start:
;-------------------------------------------------------------------
;Test-Mode-2: Pattern: AA 55 AA 55 ..
;-------------------------------------------------------------------
         sbi PORTD,sample     ;Sample-Register abkoppeln
         ldi bufcntL,LOW (0x0000-2048)  ;Gesamtes RAM fuellen
         ldi bufcntH,HIGH(0x0000-2048)  ;"
         ldi wreg,0xFF        ;PORTB auf ausgabe
         out DDRB,wreg        ;"
         ldi wreg,0xAA
T2_loop:
         out PORTB,wreg       ;Pattern ausgeben
         com wreg             ;invertieren
         out PORTD,store1
         out PORTD,store2

         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc T2_loop
         ldi wreg,0x00        ;PORTB auf eingabe
         out DDRB,wreg        ;"
         ldi wreg,0b11111111  ;PORT-B
         out PORTB,wreg       ;Pull-Ups aktivieren
         rjmp RAMfull

T3_Start:
;-------------------------------------------------------------------
;Test-Mode-3: Walking Zero  FF FE FD FB F7 EF DF BF 7F
;-------------------------------------------------------------------
         sbi PORTD,sample     ;Sample-Register abkoppeln
         ldi bufcntL,LOW (0x0000-2048)  ;Gesamtes RAM fuellen
         ldi bufcntH,HIGH(0x0000-2048)  ;"
         ldi wreg,0xFF        ;PORTB auf ausgabe
         out DDRB,wreg        ;"
         clc                  ;Null einschieben
         in temp,SREG         ;Carry retten
T3_loop:
         out SREG,temp        ;Carry zurueckholen
         out PORTB,wreg       ;Bitmuster ausgeben
         nop                  ;
         out PORTD,store1
         out PORTD,store2

         rol wreg             ;naechstes Muster
         in temp,SREG         ;Carry retten
         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc T3_loop
         ldi wreg,0x00        ;PORTB auf eingabe
         out DDRB,wreg        ;"
         ldi wreg,0b11111111  ;PORT-B
         out PORTB,wreg       ;Pull-Ups aktivieren
;        rjmp RAMfull

;-------------------------------------------------------------------
;Datenausgabe fuer LogicAnalyzer
;-------------------------------------------------------------------
RAMfull:
         cli                  ;Interrupts sperren
         ldi wreg,0x00        ;Freigabe fuer INT0 aufheben
         out GIMSK,wreg       ;"

         out PORTD,store2     ;PORTD Ruhezustand
         ldi TXByte, 'D'      ;sende "D" fuer "DATEN folgen" ...
         rcall PutChar
                              ;LogicAn-Modes:0..9, PulseGen-Modes:10..13
         cpi mode,10          ;Fuer LogicAnalyzer
         brcs RAMful1         ;erfolgt Datenausgabe.
         rjmp start           ;Fuer PulseGen ist Schluss..

RAMful1:
         cbi PORTD,ram_oe     ;RAM auf BUS legen

         ldi bufcntL,LOW (0x0000-2048) ;gesamten Speicher auslesen
         ldi bufcntH,HIGH(0x0000-2048) ;"

readRAM:
         in TXbyte,PINB       ;RAM lesen
         sbi PORTD,cntclk     ;counter-clock erzeugen
         cbi PORTD,cntclk     ;"
         rcall PutChar
         add bufcntL,aux1     ;incr. BufferCounter
         adc bufcntH,aux0     ;"
         brcc readRAM
         rjmp start



;-------------------------------------------------------------------
;Pulse-Generator, Range-1, Ti_min=1us, Tp_min=1us, Resolution=250ns
;Minimal: Ti=Tp=1us, Per=2us -> 500kHz
;Maximal: Ti=Tp=1us+(65535*250ns)=1us+16383.75us, Per=32769.5us
;-------------------------------------------------------------------
PG_Start0:
PG00:
         cbi PORTD,sample    ;Trigger-Pulse
         nop                 ;"
         sbi PORTD,sample    ;"
         mov TpCntL,TpL      ;load TpCounter
         mov TpCntH,TpH
PG01:
         add TpCntL,aux1     ;incr. TpCounter
         adc TpCntH,aux0     ;"
         brcc PG01
         brcs PG02
PG02:
         sbi PORTD,PGout
         mov TiCntL,TiL      ;load TiCounter
         mov TiCntH,TiH
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
PG03:
         add TiCntL,aux1     ;incr. TiCounter
         adc TiCntH,aux0     ;"
         brcc PG03
         cbi PORTD,PGout
         brcs PG00


;-------------------------------------------------------------------
;Pulse-Generator, Range-1, BurstMode
;-------------------------------------------------------------------
PG_Start1:
         cbi PORTD,sample    ;Trigger-Pulse
         nop                 ;"
         sbi PORTD,sample    ;"
         mov TnCntL,TnL      ;load Burst-Counter
         mov TnCntH,TnH
PG10:
         mov TpCntL,TpL      ;load TpCounter
         mov TpCntH,TpH
         nop
         nop
         nop
         nop
         nop
PG11:
         add TpCntL,aux1     ;incr. TpCounter
         adc TpCntH,aux0     ;"
         brcc PG11
         brcs PG12
PG12:
         sbi PORTD,PGout
         mov TiCntL,TiL      ;load TiCounter
         mov TiCntH,TiH
         nop
         nop
         nop
         nop
         nop
         nop
         nop
PG13:
         add TiCntL,aux1     ;incr. TiCounter
         adc TiCntH,aux0     ;"
         brcc PG13
         cbi PORTD,PGout
         add TnCntL,aux1     ;incr. BurstCounter
         adc TnCntH,aux0     ;"
         brcc PG10

         rjmp RAMfull

;-------------------------------------------------------------------
;Pulse-Generator, Range-2, Ti_min=125us, Tp_min=125us,
;Resolution=125us, Ti_max=Tp_max=65535*125us=8191.875ms (ca. 8.2s)
;-------------------------------------------------------------------
PG_Start2:

PG20:
         cbi PORTD,sample    ;Trigger-Pulse
         nop                 ;"
         sbi PORTD,sample    ;"
         mov TpCntL,TpL      ;load TpCounter
         mov TpCntH,TpH
         ldi temp,8
PG21:
         nop
         nop
         nop
         nop
PG21a:
         add temp,aux1
         nop
         brcc PG21
         ldi temp,6
         adc TpCntL,aux0     ;incr. TpCounter
         adc TpCntH,aux0     ;"
         brcc PG21a
         brcs PG22
PG22:
         sbi PORTD,PGout
         mov TiCntL,TiL      ;load TiCounter
         mov TiCntH,TiH
         ldi temp,8
         nop
         nop
         nop
         nop
         nop
         nop
         nop
PG23:
         nop
         nop
         nop
         nop
PG23a:
         add temp,aux1
         nop
         brcc PG23
         ldi temp,6
         adc TiCntL,aux0     ;incr. TiCounter
         adc TiCntH,aux0     ;"
         brcc PG23a
         cbi PORTD,PGout
         brcs PG20

;-------------------------------------------------------------------
;Pulse-Generator, Range-2, Burst-Mode
;-------------------------------------------------------------------
PG_Start3:
         cbi PORTD,sample    ;Trigger-Pulse
         nop                 ;"
         sbi PORTD,sample    ;"
         mov TnCntL,TnL      ;load Burst-Counter
         mov TnCntH,TnH
PG30:
         mov TpCntL,TpL      ;load TpCounter
         mov TpCntH,TpH
         ldi temp,8
         nop
         nop
         nop
PG31:
         nop
         nop
         nop
         nop
PG31a:
         add temp,aux1
         nop
         brcc PG31
         ldi temp,6
         adc TpCntL,aux0     ;incr. TpCounter
         adc TpCntH,aux0     ;"
         brcc PG31a
         brcs PG32
PG32:
         sbi PORTD,PGout
         mov TiCntL,TiL      ;load TiCounter
         mov TiCntH,TiH
         ldi temp,8
         nop
         nop
         nop
         nop
         nop
         nop
         nop
PG33:
         nop
         nop
         nop
         nop
PG33a:
         add temp,aux1
         nop
         brcc PG33
         ldi temp,6
         adc TiCntL,aux0     ;incr. TiCounter
         adc TiCntH,aux0     ;"
         brcc PG33a
         cbi PORTD,PGout

         add TnCntL,aux1     ;incr. BurstCounter
         adc TnCntH,aux0     ;"
         brcc PG30

         rjmp RAMfull

;-------------------------------------------------------------------
;Unterprogramme zur seriellen Kommunikation. (AppNote 305)
;-------------------------------------------------------------------

;***************************************************************************
;*
;* "putchar"
;*
;* This subroutine transmits the byte stored in the "Txbyte" register
;* The number of stop bits used is set with the sb constant
;*
;* Number of words      :14 including return
;* Number of cycles     :Depens on bit rate
;* Low registers used   :None
;* High registers used  :2 (bitcnt,Txbyte)
;* Pointers used        :None
;*
;***************************************************************************
.equ            sb      =2              ;Number of stop bits (1, 2, ...)


putchar:        ldi     bitcnt,9+sb     ;1+8+sb (sb is # of stop bits)
                com     Txbyte          ;Inverte everything
                sec                     ;Start bit

putchar0:       brcc    putchar1        ;If carry set
                cbi     PORTD,TxD       ;    send a '0'
                rjmp    putchar2        ;else


putchar1:       sbi     PORTD,TxD       ;    send a '1'
                nop


putchar2:       rcall UART_delay        ;One bit delay
                rcall UART_delay


                lsr     Txbyte          ;Get next bit
                dec     bitcnt          ;If not all bit sent
                brne    putchar0        ;   send next
                                        ;else
               ret                     ;   return




;***************************************************************************
;*
;* "getchar"
;*
;* This subroutine receives one byte and returns it in the "Rxbyte" register
;*
;* Number of words      :14 including return
;* Number of cycles     :Depens on when data arrives
;* Low registers used   :None
;* High registers used  :2 (bitcnt,Rxbyte)
;* Pointers used        :None
;*
;***************************************************************************


getchar:        ldi     bitcnt,9        ;8 data bit + 1 stop bit


getchar1:       sbic    PIND,RxD        ;Wait for start bit
                rjmp    getchar1


                rcall UART_delay        ;0.5 bit delay


getchar2:       rcall UART_delay        ;1 bit delay
                rcall UART_delay


                clc                     ;clear carry
                sbic    PIND,RxD        ;if RX pin high
                sec                     ;


                dec     bitcnt          ;If bit is stop bit
                breq    getchar3        ;   return
                                        ;else
                ror     Rxbyte          ;   shift bit into Rxbyte
                rjmp    getchar2        ;   go get next


getchar3:       ret




;***************************************************************************
;*
;* "UART_delay"
;*
;* This delay subroutine generates the required delay between the bits when
;* transmitting and receiving bytes. The total execution time is set by the
;* constant "b":
;*
;*      3·b + 7 cycles (including rcall and ret)
;*
;* Number of words      :4 including return
;* Low registers used   :None
;* High registers used  :1 (temp)
;* Pointers used        :None
;*
;***************************************************************************

; 38400Bd bei 16MHz:
; 38400Bd -> 26us,  16MHz -> 62,5ns,   26us/62,5ns=416 Takte
; 416-8=408, 408/2=204, (204-7)/3=66   ->>>> baud = 66
; Kontrollrechnung: (((66*3)+7)*2)+8=418cy -> 26,125ns
;
; 38400Bd bei 12MHz:                   ->>>> baud = 48


UART_delay:     ldi     temp,baud
UART_delay1:    dec     temp
                brne    UART_delay1

                ret


