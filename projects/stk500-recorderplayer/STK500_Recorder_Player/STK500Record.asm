;Programmer: Austin Hanson
;Date:       6-15-2003

.include "8515def.inc"                 ;Includes the 8515 definitions file    |

.def    PinData = R16                  ;Define PinData as R16
.def    temp1 = R17

.org 0x0000                            ;Places the following code from address
                                       ;0x0000.
    rjmp    RESET                      ;Jumps to Reset
    
RESET:                                 ;Reset Label
    ldi     XL,$60                     ;Default setting for XL is $60
                                       ;so just in case the person presses
                                       ;play it won't try to play all of
                                       ;the memory.
    ser     PinData                    ;Set all bits in PinData.
    out     DDRB,PinData               ;Toggle Port B to output.
    out     PORTB,PinData              ;Turn of all of the LEDS.

;****************************************************************
;*     $07    $06    $05    $04    $03    $02    $01    $00     *
;*     .-,    .-,    .-,    .-,    .-,    .-,    .-,    .-,     *
;*    /`-'\  /`-'\  /`-'\  /`-'\  /`-'\  /`-'\  /`-'\  /`-'\    *
;*    __|__   _|__  __|__  __|__  __|__  __|__  __|___  _|__    *
;*   |Btn 1||Btn 2||Btn 3||Btn 4||Btn 5||Clear||Record||Play|   *
;*   |_____||_____||_____||_____||_____||_____||______||____|   *
;****************************************************************

;Main "system" loop, checks for the three main buttons, clear, record, and
;play, then goes to the correct label for that.
MainLoop:                              ;MainLoop Label
    sbis    PinD,$02                   ;Check to see if Switch 2 (clear) is
                                       ;pushed.
    rjmp    ClearMemory                ;If Switch 2 (clear) is pressed then
                                       ;goto ClearMemory.
    sbis    PinD,$01                   ;Check to see if Switch 1 (record) is
                                       ;pushed.
    rjmp    BeginRecordMemory          ;If Switch 1 (record) is pressed then
                                       ;move to the begin record loop
                                       ;(BeginRecordMemory) where we can reset
                                       ;X so we can start over at the beginning
                                       ;of ram.
    sbis    PIND,$00                   ;Check to see if Switch 0 (play) is
                                       ;pushed.
    rjmp    PlayMemory                 ;If Switch 0 (play) is pressed then goto
                                       ;PlayMemory.
    rjmp    MainLoop                   ;Return to the main loop (MainLoop).

;PlayMemory contains the main code to cycle through ram and play any data
;contained within.
PlayMemory:
    clr     YH                         ;Clear the High bit in Y just incase
                                       ;there is in it.
    ldi     YL,$60                     ;Load $60 to Y so we have a pointer that
                                       ;we can inc and then compare to X to
                                       ;cycle through only the portion of ram
                                       ;that was used.

                                       ;After Y is reset to zero we will drop
                                       ;down into the actual PlayMemoryLoop
                                       ;which contains the actual code to play
                                       ;back what the user recorded.

PlayMemoryLoop:
    cp      YL,XL                      ;Check to see if Y has reached the end
                                       ;of the recorded data.
    breq    MainLoop                   ;Return to the main loop when the end of
                                       ;the data is reached.
                                       ;continue on recording where we left off.
    ld      PinData,Y                  ;Load the data from the current location
                                       ;that is stored in Y.
    out     PORTB,PinData              ;Sends the data stored in PinData to
                                       ;PORTB.
    inc     YL                         ;Increment Y so we can then move on to a
                                       ;new spot in ram.        

DELAY:                                 ;This is a quick little delay function
    ldi     R18,255                    ;that will delay for about 65025 cycles.
DELAY1i:
    ldi     R17,255
DELAY1:
    dec     R17
    cpi     R17,0
    brne    DELAY1
DELAY2:
    dec     R18
    cpi     R18,0
    brne    DELAY1i

    rjmp    PlayMemoryLoop             ;Return to the top of the PlayMemoryLoop
                                       ;so we can do the whole process over
                                       ;again until we reach the end of the
                                       ;data stored in ram.

;The initialization of all of the registers used in the process of recording
;the keys pressed by the user.
BeginRecordMemory:
    clr     XH                         ;Clear the high bit in X just incase
                                       ;there happens to be something in it.
    ldi     XL,$60                     ;Resets X so we start over in Ram.
    set     PinData                    ;Set all bits in PinData.
    out     PORTB,PinData              ;Shut off all LEDS.

                                       ;After BeginRecordMemory is gone to, the
                                       ;program drops down to Record Memory.

;RecordMemory contains the main code that is used to record the keys pressed
;by the user.
RecordMemory:                          ;RecordMemory Label
    sbis    PIND,$07                   ;Check to see if Switch 7 is pushed.
    rjmp    SetMemory                  ;If Switch 7 is pushed set it to memory.
    sbis    PIND,$06                   ;Check to see if Switch 6 is Pushed.
    rjmp    SetMemory                  ;If Switch 6 is pushed set it to memory.
    sbis    PIND,$05                   ;Check to see if Switch 5 is pushed.
    rjmp    SetMemory                  ;If Switch 5 is pushed set it to memory.
    sbis    PIND,$04                   ;Check to see if Switch 4 is pushed.
    rjmp    SetMemory                  ;If Switch 4 is pushed set it to memory.
    sbis    PIND,$03                   ;Check to see if Switch 3 is pushed.
    rjmp    SetMemory                  ;If Switch 3 is pushed set it to memory.
    sbis    PIND,$02                   ;Check to see if Switch 2 is pushed.
    rjmp    ClearMemory                ;If Switch 2 is pushed clear the memory.
                                       ;We skip of Switch 1 because we are
                                       ;allready recording.
    sbis    PIND,$00                   ;Check to see if Switch 0 is pushed.
    rjmp    PlayMemory                 ;If Switch 0 is pushed play the memory.
    rjmp    RecordMemory               ;Return to RecordMemory

;ClearMemory doesn't actually clear the memory, but rather just clears X
;and thus when we store new data we are writing over the old data.
ClearMemory:
    clr     XH                         ;Clear X just incase there happens to be
                                       ;something in it.
    ldi     XL,$60                     ;Sets XL to $60 so we can start at the
                                       ;the beginning of ram when we record
                                       ;data.
    ser     PinData                    ;Set all of the bits in PinData.
    out     PORTB,PinData              ;Shut off all of the LEDS.
    rjmp    MainLoop                   ;Returns to the MainLoop, when the user
                                       ;presses clear in record mode he will
                                       ;automatically be dropped back to the
                                       ;MainLoop and thus he will have to click
                                       ;the record button again if he was in
                                       ;record mode.

;Portion of the code that sets the situation of the switches to memory so when
;play is hit we can replay it on the LEDS.
SetMemory:
    in      PinData,PIND               ;Load PinData with the current situation
                                       ;of the Switches
    out     PORTB,PinData              ;Turn on the LED corresponding to the
                                       ;switch that is pressed.
    cpi     XL,255                     ;Check to see if X is 255 which would
                                       ;mean that ram is full.
    breq    RecordMemory               ;If ram is full then return to
                                       ;RecordMemory and wait for the user to
                                       ;press Play.
    st      X,PinData                  ;Stores the current situation of the
                                       ;switches into ram.
    inc     XL                         ;Increase X so we move to a new spot in
                                       ;memory for each new key pressed.

;A piece of code used to only count the switch being pressed once.
WaitForRelease:
    in      temp1,PIND
    cp      temp1,PinData
    breq    WaitForRelease
    
    rjmp    RecordMemory               ;Return to RecordMemory.