;Programmer: Austin Hanson
;Date:       6-15-2003
 
.include "8515def.inc"                 ;Includes the 8515 definitions file

.def    Temp = R16                     ;Define Temp as R16
.def    Temp2 = R17                    ;Define Temp2 as R17
.def    SpeedCounter1 = R18            ;Define SpeedCounter1 as R18
.def    SpeedCounter2 = R19            ;Define SpeedCounter2 as R19
.def    SpeedControl = R20             ;Define SpeedControl as R20
.def    LevelCounter = R21             ;Define LevelCounter as R21
.def    LEDDirection = R22             ;Define LEDDirection as R22
.def    LEDData = R23                  ;Define LEDData as R23

.org 0x0000                            ;Places the following code from address
                                       ;0x0000.
    rjmp    RESET                      ;Jumps to Reset.
    
RESET:                                 ;Reset Label
    ser     SpeedControl
    ser     Temp                       ;Set all the bits on Temp.
    out     DDRB,Temp                  ;Toggle Port B to output.
    out     PORTB,Temp                 ;Turn off all of the LEDS.
    clr     LevelCounter               ;Reset LevelCounter.
    ser     SpeedCounter1              ;Set SpeedCounter1 to 255 which makes the
                                       ;speed the slowest.
    ser     SpeedCounter2              ;Set SpeedCounter2 to 255 which makes the
                                       ;speed the slowest.
    ldi     LEDData,$F7                ;Set LEDS to starting position
    ldi     LEDDirection,$00           ;Set LEDDirection to starting direction.

;MainLoop contains the code that will initiate the game when it is first ran.
MainLoop:                              ;MainLoop Label;
    sbis    PIND,$07                   ;Checks to see if Switch 7  has been
                                       ;pressed.
    rjmp    WFBRMainLoop               ;If it has it will then rjmp to
                                       ;WFBRMainLoop.
                                       ;WFBR = Wait For Button Release.
    rjmp    MainLoop                   ;Rjmp to MainLoop until Play button is
                                       ;pressed.

WFBRMainLoop:
    in      Temp,PIND                  ;Get the current situation of PIND
WFBRMainLoopPFB:                       ;Main loop label.
                                       ;PFB = Pause For Button
    in      Temp2,PIND                 ;Get the new situation of PIND
    cp      Temp2,Temp                 ;Compare Temp2 to Temp
    breq    WFBRMainLoopPFB            ;Start over in loop unless the two are
                                       ;not equal.
                                       ;Play button is depressed so now we go
                                       ;to the actual DisplayLevel, but since
                                       ;it's right below we can just drop
                                       ;through to it.
DisplayLevel:
    com     LevelCounter               ;Invert it because the LEDS are
                                       ;backwords from it.
    out     PORTB,LevelCounter         ;Send out the level on the LEDS.
                                       ;The level is in binary.
    com     LevelCounter               ;Invert it again to turn it back to
                                       ;normal.
DisplayLevelDelay:                     ;Beginning of DisplayLevelDelay.
    ldi     Temp2,255                  ;that will delay for about 65025 cycles.
DisplayLevelDelay2:
    ldi     Temp,255
DisplayLevelDelay3:
    dec     Temp
    cpi     Temp,0
    brne    DisplayLevelDelay3

    dec     Temp2
    cpi     Temp2,0
    brne    DisplayLevelDelay2

GameLoop:
    cpi     SpeedCounter2,$00          ;Check to see if SpeedCounter2 has
                                       ;reached 0.
    breq    ChangeLEDS                 ;If so change the LEDS.
    cpi     SpeedCounter1,$00          ;Check to see if SpeedCounter1 is 0.
    breq    SpeedCounter2Dec           ;If so go to SpeedCounter2Dec
    
	sbis    PIND,$07                   ;Check to see if Switch 7 is pushed.
    rjmp    CheckKey                   ;If so check the key.
    sbis    PIND,$06                   ;Check to see if Switch 6 is pushed.
    rjmp    CheckKey                   ;If so check the key.
    sbis    PIND,$05                   ;Check to see if Switch 5 is pushed.
    rjmp    CheckKey                   ;If so check the key.
    sbis    PIND,$04                   ;Check to see if Switch 4 is pushed.
    rjmp    CheckKey                   ;If so check the key.
    sbis    PIND,$03                   ;Check to see if Switch 3 is pushed.
    rjmp    CheckKey                   ;If so check the key.
    sbis    PIND,$02                   ;Check to see if Switch 2 is pushed.
    rjmp    CheckKey                   ;If so check the key.
    sbis    PIND,$01                   ;Check to see if Switch 1 is pushed.
    rjmp    CheckKey                   ;If so check the key.
    sbis    PIND,$00                   ;Check to see if Switch 0 is pushed.
    rjmp    CheckKey                   ;If so check the key.

    dec     SpeedCounter1              ;Decrease the speed counter
    rjmp    GameLoop                   ;Return to the beginning of GameLoop.

SpeedCounter2Dec:
    dec     SpeedCounter2              ;Decrease the second speed counter.
    ser     SpeedCounter1              ;Reset the first speed counter.
    rjmp    GameLoop

ChangeLEDS:
    sec                                ;Set C.
    ser     SpeedCounter1              ;Reset the first speed counter.
    mov     SpeedCounter2,SpeedControl ;Set the second speed counter to the
                                       ;speed control.
    sbrs    LEDData,$07                ;Check to see if the LED that is on
                                       ;has reached the left end.
    ldi     LEDDirection,$01           ;If it has then switch the direction.
    sbrs    LEDData,$00                ;Check to see if the LED that is on
                                       ;has reached the right end.
    ldi     LEDDirection,$00           ;If it has then switch the direction.
    sbrs    LEDDirection,$00           ;Check to see if the direction is left.
    rol     LEDData                    ;If it is rotate the LED to the left.
    sbrc    LEDDirection,$00           ;Check to see if the direction is right.
    ror     LEDData                    ;If it is rotate the LED to the right.
    out     PORTB,LEDData              ;Redisplay the LED in its new position.
    rjmp    GameLoop                   ;Return to the GameLoop.

CheckKey:
    in      Temp,PIND                  ;Get the current situation of the
                                       ;switches.
    cp      Temp,LEDData               ;Compare it to where the LED is at.
    brne    Lost                       ;If it doesn't equal then go to Lost.
    inc     LevelCounter               ;Increase the level since the user won.
    dec     SpeedControl               ;Decrease the speed 5 times, 5 is used to
    dec     SpeedControl               ;create atleast a noticeable change in
    dec     SpeedControl               ;speed.
    dec     SpeedControl
    dec     SpeedControl
WFBRCheckKey:                          ;Wait for the button to be released.
    in      Temp,PIND                  ;Get the current situation of PIND.
WFBRCheckKeyPFB:                       ;Main loop label.
                                       ;PFB = Pause For Button.
    in      Temp2,PIND                 ;Get the new situation of PIND
    cp      Temp2,Temp                 ;Compare Temp2 to Temp
    breq    WFBRCheckKeyPFB            ;Start over in loop unless the two are
                                       ;not equal.
    rjmp    DisplayLevel               ;Go to DisplayLevel to start a new
                                       ;level.

Lost:                                  ;The user must have lost.
    clr     Temp                       ;Clear Temp.
    out     PORTB,Temp                 ;Turn on all lights to let the user know
                                       ;he lost.
LostDelay:                             ;Beginning of LostDelay.
    ldi     Temp2,255                  ;that will delay for about 65025 cycles.
LostDelay2:
    ldi     Temp,255
LostDelay3:
    dec     Temp
    cpi     Temp,0
    brne    LostDelay3

    dec     Temp2
    cpi     Temp2,0
    brne    LostDelay2
    rjmp    RESET                      ;Go to RESET to reset the game.
