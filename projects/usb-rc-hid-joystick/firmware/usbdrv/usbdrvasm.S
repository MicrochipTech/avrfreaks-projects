/* Name: usbdrvasm.S
 * Project: AVR USB driver
 * Author: Christian Starkjohann
 * Creation Date: 2004-12-29
 * Tabsize: 4
 * Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: Proprietary, free under certain conditions. See Documentation.
 * This Revision: $Id: usbdrvasm.S 218 2006-07-15 17:08:14Z cs $
 */

/*
General Description:
This module implements the assembler part of the USB driver. See usbdrv.h
for a description of the entire driver.
Since almost all of this code is timing critical, don't change unless you
really know what you are doing! Many parts require not only a maximum number
of CPU cycles, but even an exact number of cycles!


Timing constraints according to spec (in bit times):
timing subject                                      min max    CPUcycles
---------------------------------------------------------------------------
EOP of OUT/SETUP to sync pattern of DATA0 (both rx) 2   16     16-128
EOP of IN to sync pattern of DATA0 (rx, then tx)    2   7.5    16-60
DATAx (rx) to ACK/NAK/STALL (tx)                    2   7.5    16-60
*/

#include "iarcompat.h"
#ifndef __IAR_SYSTEMS_ASM__
    /* configs for io.h */
#   define __SFR_OFFSET 0
#   define _VECTOR(N)   __vector_ ## N   /* io.h does not define this for asm */
#   include <avr/io.h> /* for CPU I/O register definitions and vectors */
#endif  /* __IAR_SYSTEMS_ASM__ */
#include "usbdrv.h" /* for common defs */


/* register names */
#define x1      r16
#define x2      r17
#define shift   r18
#define cnt     r19
#define x3      r20
#define x4      r21

/* Some assembler dependent definitions and declarations: */

#ifdef __IAR_SYSTEMS_ASM__

#   define nop2     rjmp    $+2 /* jump to next instruction */
#   define XL       r26
#   define XH       r27
#   define YL       r28
#   define YH       r29
#   define ZL       r30
#   define ZH       r31
#   define lo8(x)   LOW(x)
#   define hi8(x)   ((x)>>8)    /* not HIGH to allow XLINK to make a proper range check */

    extern  usbRxBuf, usbDeviceAddr, usbNewDeviceAddr, usbInputBuf
    extern  usbCurrentTok, usbRxLen, usbRxToken, usbAppBuf, usbTxLen
    extern  usbTxBuf, usbMsgLen, usbTxLen1, usbTxBuf1, usbTxLen3, usbTxBuf3
    public  usbCrc16
    public  usbCrc16Append

    COMMON  INTVEC
    ORG     INT0_vect
    rjmp    SIG_INTERRUPT0
    RSEG    CODE

#else /* __IAR_SYSTEMS_ASM__ */

#   define nop2     rjmp    .+0 /* jump to next instruction */

    .text
    .global SIG_INTERRUPT0
    .type   SIG_INTERRUPT0, @function
    .global usbCrc16
    .global usbCrc16Append

#endif /* __IAR_SYSTEMS_ASM__ */


SIG_INTERRUPT0:
;Software-receiver engine. Strict timing! Don't change unless you can preserve timing!
;interrupt response time: 4 cycles + insn running = 7 max if interrupts always enabled
;max allowable interrupt latency: 32 cycles -> max 25 cycles interrupt disable
;max stack usage: [ret(2), x1, SREG, x2, cnt, shift, YH, YL, x3, x4] = 11 bytes
usbInterrupt:
;order of registers pushed:
;x1, SREG, x2, cnt, shift, [YH, YL, x3]
    push    x1              ;2  push only what is necessary to sync with edge ASAP
    in      x1, SREG        ;1
    push    x1              ;2
;sync byte (D-) pattern LSb to MSb: 01010100 [1 = idle = J, 0 = K]
;sync up with J to K edge during sync pattern -- use fastest possible loops
;first part has no timeout because it waits for IDLE or SE1 (== disconnected)
#if !USB_CFG_SAMPLE_EXACT
    ldi     x1, 5           ;1 setup a timeout for waitForK
#endif
waitForJ:
    sbis    USBIN, USBMINUS ;1 wait for D- == 1
    rjmp    waitForJ        ;2
#if USB_CFG_SAMPLE_EXACT
;The following code represents the unrolled loop in the else branch. It
;results in a sampling window of 1/4 bit which meets the spec.
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
    nop
    nop2
foundK:
#else
waitForK:
    dec     x1              ;1
    sbic    USBIN, USBMINUS ;1 wait for D- == 0
    brne    waitForK        ;2
#endif
;{2, 6} after falling D- edge, average delay: 4 cycles [we want 4 for center sampling]
;we have 1 bit time for setup purposes, then sample again:
    push    x2              ;2
    push    cnt             ;2
    push    shift           ;2
shortcutEntry:
    ldi     cnt, 1          ;1 pre-init bit counter (-1 because no dec follows, -1 because 1 bit already sampled)
    ldi     x2, 1<<USB_CFG_DPLUS_BIT    ;1 -> 8   edge sync ended with D- == 0
;now wait until SYNC byte is over. Wait for either 2 bits low (success) or 2 bits high (failure)
waitNoChange:
    in      x1, USBIN       ;1 <-- sample, timing: edge + {2, 6} cycles
    eor     x2, x1          ;1
    sbrc    x2, USBMINUS    ;1 | 2
    ldi     cnt, 2          ;1 | 0 cnt = numBits - 1 (because dec follows)
    mov     x2, x1          ;1
    dec     cnt             ;1
    brne    waitNoChange    ;2 | 1
    sbrc    x1, USBMINUS    ;2
    rjmp    sofError        ;0 two consecutive "1" bits -> framing error
;start reading data, but don't check for bitstuffing because these are the
;first bits. Use the cycles for initialization instead. Note that we read and
;store the binary complement of the data stream because eor results in 1 for
;a change and 0 for no change.
    in      x1, USBIN       ;1 <-- sample bit 0, timing: edge + {3, 7} cycles
    eor     x2, x1          ;1
    ldi     shift, 0x00     ;1 prepare for bitstuff check later on in loop
    bst     x2, USBMINUS    ;1
    bld     shift, 0        ;1
    push    YH              ;2 -> 7
    in      x2, USBIN       ;1 <-- sample bit 1, timing: edge + {2, 6} cycles
    eor     x1, x2          ;1
    bst     x1, USBMINUS    ;1
    bld     shift, 1        ;1
    push    YL              ;2
    lds     YL, usbInputBuf ;2 -> 8
    in      x1, USBIN       ;1 <-- sample bit 2, timing: edge + {2, 6} cycles
    eor     x2, x1          ;1
    bst     x2, USBMINUS    ;1
    bld     shift, 2        ;1
    ldi     cnt, USB_BUFSIZE;1
    ldi     YH, hi8(usbRxBuf);1 assume that usbRxBuf does not cross a page
    push    x3              ;2 -> 8
    in      x2, USBIN       ;1 <-- sample bit 3, timing: edge + {2, 6} cycles
    eor     x1, x2          ;1
    bst     x1, USBMINUS    ;1
    bld     shift, 3        ;1
    ser     x3              ;1
    nop                     ;1
    rjmp    rxbit4          ;2 -> 8

shortcutToStart:            ;{,43} into next frame: max 5.5 sync bits missed
#if !USB_CFG_SAMPLE_EXACT
    ldi     x1, 5           ;2 setup timeout
#endif
waitForJ1:
    sbis    USBIN, USBMINUS ;1 wait for D- == 1
    rjmp    waitForJ1       ;2
#if USB_CFG_SAMPLE_EXACT
;The following code represents the unrolled loop in the else branch. It
;results in a sampling window of 1/4 bit which meets the spec.
    sbis    USBIN, USBMINUS
    rjmp    foundK1
    sbis    USBIN, USBMINUS
    rjmp    foundK1
    sbis    USBIN, USBMINUS
    rjmp    foundK1
    nop
    nop2
foundK1:
#else
waitForK1:
    dec     x1              ;1
    sbic    USBIN, USBMINUS ;1 wait for D- == 0
    brne    waitForK1       ;2
#endif
    pop     YH              ;2 correct stack alignment
    nop2                    ;2 delay for the same time as the pushes in the original code
    rjmp    shortcutEntry   ;2

; ################# receiver loop #################
; extra jobs done during bit interval:
; bit 6:    se0 check
; bit 7:    or, store, clear
; bit 0:    recover from delay  [SE0 is unreliable here due to bit dribbling in hubs]
; bit 1:    se0 check
; bit 2:    se0 check
; bit 3:    overflow check
; bit 4:    se0 check
; bit 5:    rjmp

; stuffed* helpers have the functionality of a subroutine, but we can't afford
; the overhead of a call. We therefore need a separate routine for each caller
; which jumps back appropriately.

stuffed5:               ;1 for branch taken
    in      x2, USBIN   ;1 <-- sample @ +1
    andi    x2, USBMASK ;1
    breq    se0a        ;1
    andi    x3, ~0x20   ;1
    ori     shift, 0x20 ;1
    rjmp    rxbit6      ;2

stuffed6:               ;1 for branch taken
    in      x1, USBIN   ;1 <-- sample @ +1
    andi    x1, USBMASK ;1
    breq    se0a        ;1
    andi    x3, ~0x40   ;1
    ori     shift, 0x40 ;1
    rjmp    rxbit7      ;2

; This is somewhat special because it has to compensate for the delay in bit 7
stuffed7:               ;1 for branch taken
    andi    x1, USBMASK ;1 already sampled by caller
    breq    se0a        ;1
    mov     x2, x1      ;1 ensure correct NRZI sequence
    ori     shift, 0x80 ;1 no need to set reconstruction in x3: shift has already been used
    in      x1, USBIN   ;1 <-- sample bit 0
    rjmp    unstuffed7  ;2

stuffed0:               ;1 for branch taken
    in      x1, USBIN   ;1 <-- sample @ +1
    andi    x1, USBMASK ;1
    breq    se0a        ;1
    andi    x3, ~0x01   ;1
    ori     shift, 0x01 ;1
    rjmp    rxbit1      ;2

;-----------------------------
rxLoop:
    breq    stuffed5    ;1
rxbit6:
    in      x1, USBIN   ;1 <-- sample bit 6
    andi    x1, USBMASK ;1
    breq    se0a        ;1
    eor     x2, x1      ;1
    bst     x2, USBMINUS;1
    bld     shift, 6    ;1
    cpi     shift, 0x02 ;1
    brlo    stuffed6    ;1
rxbit7:
    in      x2, USBIN   ;1 <-- sample bit 7
    eor     x1, x2      ;1
    bst     x1, USBMINUS;1
    bld     shift, 7    ;1
    eor     x3, shift   ;1 x3 is 0 at bit locations we changed, 1 at others
    st      y+, x3      ;2 the eor above reconstructed modified bits and inverted rx data
    ser     x3          ;1
rxbit0:
    in      x1, USBIN   ;1 <-- sample bit 0
    cpi     shift, 0x04 ;1
    brlo    stuffed7    ;1
unstuffed7:
    eor     x2, x1      ;1
    bst     x2, USBMINUS;1
    bld     shift, 0    ;1
    andi    shift, 0xf9 ;1
    breq    stuffed0    ;1
rxbit1:
    in      x2, USBIN   ;1 <-- sample bit 1
    andi    x2, USBMASK ;1
se0a:                   ; enlarge jump range to SE0
    breq    se0         ;1 check for SE0 more often close to start of byte
    eor     x1, x2      ;1
    bst     x1, USBMINUS;1
    bld     shift, 1    ;1
    andi    shift, 0xf3 ;1
    breq    stuffed1    ;1
rxbit2:
    in      x1, USBIN   ;1 <-- sample bit 2
    andi    x1, USBMASK ;1
    breq    se0         ;1
    eor     x2, x1      ;1
    bst     x2, USBMINUS;1
    bld     shift, 2    ;1
    andi    shift, 0xe7 ;1
    breq    stuffed2    ;1
rxbit3:
    in      x2, USBIN   ;1 <-- sample bit 3
    eor     x1, x2      ;1
    bst     x1, USBMINUS;1
    bld     shift, 3    ;1
    dec     cnt         ;1  check for buffer overflow
    breq    overflow    ;1
    andi    shift, 0xcf ;1
    breq    stuffed3    ;1
rxbit4:
    in      x1, USBIN   ;1 <-- sample bit 4
    andi    x1, USBMASK ;1
    breq    se0         ;1
    eor     x2, x1      ;1
    bst     x2, USBMINUS;1
    bld     shift, 4    ;1
    andi    shift, 0x9f ;1
    breq    stuffed4    ;1
rxbit5:
    in      x2, USBIN   ;1 <-- sample bit 5
    eor     x1, x2      ;1
    bst     x1, USBMINUS;1
    bld     shift, 5    ;1
    andi    shift, 0x3f ;1
    rjmp    rxLoop      ;2
;-----------------------------

stuffed1:               ;1 for branch taken
    in      x2, USBIN   ;1 <-- sample @ +1
    andi    x2, USBMASK ;1
    breq    se0         ;1
    andi    x3, ~0x02   ;1
    ori     shift, 0x02 ;1
    rjmp    rxbit2      ;2

stuffed2:               ;1 for branch taken
    in      x1, USBIN   ;1 <-- sample @ +1
    andi    x1, USBMASK ;1
    breq    se0         ;1
    andi    x3, ~0x04   ;1
    ori     shift, 0x04 ;1
    rjmp    rxbit3      ;2

stuffed3:               ;1 for branch taken
    in      x2, USBIN   ;1 <-- sample @ +1
    andi    x2, USBMASK ;1
    breq    se0         ;1
    andi    x3, ~0x08   ;1
    ori     shift, 0x08 ;1
    rjmp    rxbit4      ;2

stuffed4:               ;1 for branch taken
    in      x1, USBIN   ;1 <-- sample @ +1
    andi    x1, USBMASK ;1
    breq    se0         ;1
    andi    x3, ~0x10   ;1
    ori     shift, 0x10 ;1
    rjmp    rxbit5      ;2

;################ end receiver loop ###############

overflow:                   ; ignore package if buffer overflow
    rjmp    rxDoReturn      ; enlarge jump range

;This is the only non-error exit point for the software receiver loop
;{4, 20} cycles after start of SE0, typically {10, 18} after SE0 start = {-6, 2} from end of SE0
;next sync starts {16,} cycles after SE0 -> worst case start: +4 from next sync start
;we don't check any CRCs here because there is no time left.
se0:                            ;{-6, 2} from end of SE0 / {,4} into next frame
    mov     cnt, YL             ;1 assume buffer in lower 256 bytes of memory
    lds     YL, usbInputBuf     ;2 reposition to buffer start
    sub     cnt, YL             ;1 length of message
    ldi     x1, 1<<USB_INTR_PENDING_BIT ;1
    cpi     cnt, 3              ;1
    out     USB_INTR_PENDING, x1;1 clear pending intr and check flag later. SE0 must be over. {,10} into next frame
    brlo    rxDoReturn          ;1 ensure valid packet size, ignore others
    ld      x1, y               ;2 PID
    ldd     x2, y+1             ;2 ADDR + 1 bit endpoint number
    mov     x3, x2              ;1 store for endpoint number
    andi    x2, 0x7f            ;1 mask endpoint number bit
    lds     shift, usbDeviceAddr;2
    cpi     x1, USBPID_SETUP    ;1
    breq    isSetupOrOut        ;2 -> 19 = {13, 21} from SE0 end
    cpi     x1, USBPID_OUT      ;1
    breq    isSetupOrOut        ;2 -> 22 = {16, 24} from SE0 end / {,24} into next frame
    cpi     x1, USBPID_IN       ;1
    breq    handleIn            ;1
#define USB_DATA_MASK   ~(USBPID_DATA0 ^ USBPID_DATA1)
    andi    x1, USB_DATA_MASK   ;1
    cpi     x1, USBPID_DATA0 & USB_DATA_MASK ;1
    brne    rxDoReturn          ;1 not a data PID -- ignore
isData:
    lds     x2, usbCurrentTok   ;2
    tst     x2                  ;1
    breq    rxDoReturn          ;1 for other device or spontaneous data -- ignore
    lds     x1, usbRxLen        ;2
    cpi     x1, 0               ;1
    brne    sendNakAndReti      ;1 no buffer space available / {30, 38} from SE0 end
; 2006-03-11: The following two lines fix a problem where the device was not
; recognized if usbPoll() was called less frequently than once every 4 ms.
    cpi     cnt, 4              ;1 zero sized data packets are status phase only -- ignore and ack
    brmi    sendAckAndReti      ;1 keep rx buffer clean -- we must not NAK next SETUP
    sts     usbRxLen, cnt       ;2 store received data, swap buffers
    sts     usbRxToken, x2      ;2
    lds     x1, usbAppBuf       ;2
    sts     usbAppBuf, YL       ;2
    sts     usbInputBuf, x1     ;2 buffers now swapped
    rjmp    sendAckAndReti      ;2 -> {43, 51} from SE0 end

handleIn:                       ; {18, 26} from SE0 end
    cp      x2, shift           ;1 shift contains our device addr
    brne    rxDoReturn          ;1 other device
#if USB_CFG_HAVE_INTRIN_ENDPOINT
    sbrc    x3, 7               ;2 x3 contains addr + endpoint
    rjmp    handleIn1           ;0
#endif
    lds     cnt, usbTxLen       ;2
    sbrc    cnt, 4              ;2
    rjmp    sendCntAndReti      ;0 -> {27, 35} from SE0 end
    ldi     x1, USBPID_NAK      ;1
    sts     usbTxLen, x1        ;2 buffer is now free
    ldi     YL, lo8(usbTxBuf)   ;1
    ldi     YH, hi8(usbTxBuf)   ;1
    rjmp    usbSendAndReti      ;2 -> {34, 43} from SE0 end

; Comment about when to set usbTxLen to USBPID_NAK:
; We should set it back when we receive the ACK from the host. This would
; be simple to implement: One static variable which stores whether the last
; tx was for endpoint 0 or 1 and a compare in the receiver to distinguish the
; ACK. However, we set it back immediately when we send the package,
; assuming that no error occurs and the host sends an ACK. We save one byte
; RAM this way and avoid potential problems with endless retries. The rest of
; the driver assumes error-free transfers anyway.

otherOutOrSetup:
    clr     x1
    sts     usbCurrentTok, x1
rxDoReturn:
    pop     x3                  ;2
    pop     YL                  ;2
    pop     YH                  ;2
    rjmp    sofError            ;2

isSetupOrOut:                   ; we must be fast here -- a data package may follow / {,24} into next frame
    cp      x2, shift           ;1 shift contains our device addr
    brne    otherOutOrSetup     ;1 other device -- ignore
#if USB_CFG_IMPLEMENT_FN_WRITEOUT   /* if we need second OUT endpoint, store endpoint address */
    andi    x1, 0x7f            ;1 mask out MSb in token
    andi    x3, 0x80            ;1 mask out all but endpoint address
    or      x1, x3              ;1 merge endpoint into currentToken
    sts     usbCurrentTok, x1   ;2
    brmi    dontResetEP0        ;1 endpoint 1 -> don't reset endpoint 0 input
#else
    sts     usbCurrentTok, x1   ;2
#endif
;A transmission can still have data in the output buffer while we receive a
;SETUP package with an IN phase. To avoid that the old data is sent as a reply,
;we abort transmission. We don't need to reset usbMsgLen because it is used
;from the main loop only where the setup is processed anyway.
    ldi     x1, USBPID_NAK      ;1
    sts     usbTxLen, x1        ;2 abort transmission
dontResetEP0:
    pop     x3                  ;2
    pop     YL                  ;2
    in      x1, USB_INTR_PENDING;1
    sbrc    x1, USB_INTR_PENDING_BIT;1 check whether data is already arriving {,41} into next frame
    rjmp    shortcutToStart     ;2 save the pops and pushes -- a new interrupt is aready pending
;If the jump above was not taken, we can be at {,2} into the next frame here
    pop     YH                  ;2
txDoReturn:
sofError:                       ; error in start of frame -- ignore frame
    ldi     x1, 1<<USB_INTR_PENDING_BIT;1 many int0 events occurred during our processing -- clear pending flag
    out     USB_INTR_PENDING, x1;1
    pop     shift               ;2
    pop     cnt                 ;2
    pop     x2                  ;2
    pop     x1                  ;2
    out     SREG, x1            ;1
    pop     x1                  ;2
    reti                        ;4 -> {,21} into next frame -> up to 3 sync bits missed

sendCntAndReti:                 ; 19 cycles until SOP
    mov     x3, cnt             ;1
    rjmp    usbSendX3           ;2
sendNakAndReti:                 ; 19 cycles until SOP
    ldi     x3, USBPID_NAK      ;1
    rjmp    usbSendX3           ;2
sendAckAndReti:                 ; 17 cycles until SOP
    ldi     x3, USBPID_ACK      ;1
usbSendX3:
    ldi     YL, 20              ;1 'x3' is R20
    ldi     YH, 0               ;1
    ldi     cnt, 2              ;1
;;;;rjmp    usbSendAndReti      fallthrough

; USB spec says:
; idle = J
; J = (D+ = 0), (D- = 1) or USBOUT = 0x01
; K = (D+ = 1), (D- = 0) or USBOUT = 0x02
; Spec allows 7.5 bit times from EOP to SOP for replies (= 60 cycles)

;usbSend:
;pointer to data in 'Y'
;number of bytes in 'cnt' -- including sync byte
;uses: x1...x4, shift, cnt, Y
usbSendAndReti:             ; SOP starts 13 cycles after call
    push    x4              ;2
    ldi     x4, USBMASK     ;1 exor mask
    sbi     USBOUT, USBMINUS;1 prepare idle state; D+ and D- must have been 0 (no pullups)
    in      x1, USBOUT      ;1 port mirror for tx loop
    sbi     USBDDR, USBMINUS;1
    sbi     USBDDR, USBPLUS ;1 set D+ and D- to output: acquire bus
; need not init x2 (bitstuff history) because sync starts with 0
    ldi     shift, 0x80     ;1 sync byte is first byte sent
    rjmp    txLoop          ;2 -> 13 + 3 = 16 cycles until SOP

#if USB_CFG_HAVE_INTRIN_ENDPOINT    /* placed here due to relative jump range */
handleIn1:                  ;{23, 31} from SE0
    ldi     x1, USBPID_NAK  ;1
#if USB_CFG_HAVE_INTRIN_ENDPOINT3
; 2006-06-10 as suggested by O.Tamura: support second INTR IN / BULK IN endpoint
    ldd     x2, y+2         ;2
    sbrc    x2, 0           ;2 1
    rjmp    handleIn3       ;0 2
#endif
    lds     cnt, usbTxLen1  ;2
    sbrc    cnt, 4          ;2
    rjmp    sendCntAndReti  ;0
    sts     usbTxLen1, x1   ;2
    ldi     YL, lo8(usbTxBuf1);1
    ldi     YH, hi8(usbTxBuf1);1
    rjmp    usbSendAndReti  ;2 -> arrives at usbSendAndReti {34, 42} from SE0

#if USB_CFG_HAVE_INTRIN_ENDPOINT3
handleIn3:
    lds     cnt, usbTxLen3  ;2
    sbrc    cnt, 4          ;2
    rjmp    sendCntAndReti  ;0
    sts     usbTxLen3, x1   ;2
    ldi     YL, lo8(usbTxBuf3);1
    ldi     YH, hi8(usbTxBuf3);1
    rjmp    usbSendAndReti  ;2 -> arrives at usbSendAndReti {39, 47} from SE0
#endif
#endif

bitstuff0:                  ;1 (for branch taken)
    eor     x1, x4          ;1
    ldi     x2, 0           ;1
    out     USBOUT, x1      ;1 <-- out
    rjmp    didStuff0       ;2 branch back 2 cycles earlier
bitstuff1:                  ;1 (for branch taken)
    eor     x1, x4          ;1
    ldi     x2, 0           ;1
    sec                     ;1 set carry so that brsh will not jump
    out     USBOUT, x1      ;1 <-- out
    rjmp    didStuff1       ;2 jump back 1 cycle earler
bitstuff2:                  ;1 (for branch taken)
    eor     x1, x4          ;1
    ldi     x2, 0           ;1
    rjmp    didStuff2       ;2 jump back 3 cycles earlier and do out
bitstuff3:                  ;1 (for branch taken)
    eor     x1, x4          ;1
    ldi     x2, 0           ;1
    rjmp    didStuff3       ;2 jump back earlier

txLoop:
    sbrs    shift, 0        ;1
    eor     x1, x4          ;1
    out     USBOUT, x1      ;1 <-- out
    ror     shift           ;1
    ror     x2              ;1
didStuff0:
    cpi     x2, 0xfc        ;1
    brsh    bitstuff0       ;1
    sbrs    shift, 0        ;1
    eor     x1, x4          ;1
    ror     shift           ;1
    out     USBOUT, x1      ;1 <-- out
    ror     x2              ;1
    cpi     x2, 0xfc        ;1
didStuff1:
    brsh    bitstuff1       ;1
    sbrs    shift, 0        ;1
    eor     x1, x4          ;1
    ror     shift           ;1
    ror     x2              ;1
didStuff2:
    out     USBOUT, x1      ;1 <-- out
    cpi     x2, 0xfc        ;1
    brsh    bitstuff2       ;1
    sbrs    shift, 0        ;1
    eor     x1, x4          ;1
    ror     shift           ;1
    ror     x2              ;1
didStuff3:
    cpi     x2, 0xfc        ;1
    out     USBOUT, x1      ;1 <-- out
    brsh    bitstuff3       ;1
    nop2                    ;2
    ld      x3, y+          ;2
    sbrs    shift, 0        ;1
    eor     x1, x4          ;1
    out     USBOUT, x1      ;1 <-- out
    ror     shift           ;1
    ror     x2              ;1
didStuff4:
    cpi     x2, 0xfc        ;1
    brsh    bitstuff4       ;1
    sbrs    shift, 0        ;1
    eor     x1, x4          ;1
    ror     shift           ;1
    out     USBOUT, x1      ;1 <-- out
    ror     x2              ;1
    cpi     x2, 0xfc        ;1
didStuff5:
    brsh    bitstuff5       ;1
    sbrs    shift, 0        ;1
    eor     x1, x4          ;1
    ror     shift           ;1
    ror     x2              ;1
didStuff6:
    out     USBOUT, x1      ;1 <-- out
    cpi     x2, 0xfc        ;1
    brsh    bitstuff6       ;1
    sbrs    shift, 0        ;1
    eor     x1, x4          ;1
    ror     shift           ;1
    ror     x2              ;1
didStuff7:
    cpi     x2, 0xfc        ;1
    out     USBOUT, x1      ;1 <-- out
    brsh    bitstuff7       ;1
    mov     shift, x3       ;1
    dec     cnt             ;1
    brne    txLoop          ;2 | 1
    cbr     x1, USBMASK     ;1 prepare SE0 [spec says EOP may be 15 to 18 cycles]
    pop     x4              ;2
    out     USBOUT, x1      ;1 <-- out SE0 -- from now 2 bits = 16 cycles until bus idle
    ldi     cnt, 2          ;| takes cnt * 3 cycles
se0Delay:                   ;|
    dec     cnt             ;|
    brne    se0Delay        ;| -> 2 * 3 = 6 cycles
;2006-03-06: moved transfer of new address to usbDeviceAddr from C-Code to asm:
;set address only after data packet was sent, not after handshake
    lds     x2, usbNewDeviceAddr;2
    subi    YL, 20 + 2          ;1
    sbci    YH, 0               ;1
    breq    skipAddrAssign      ;2
    sts     usbDeviceAddr, x2   ;0  if not skipped: SE0 is one cycle longer
skipAddrAssign:
;end of usbDeviceAddress transfer
    ori     x1, USBIDLE     ;1
    in      x2, USBDDR      ;1
    cbr     x2, USBMASK     ;1 set both pins to input
    out     USBOUT, x1      ;1 <-- out J (idle) -- end of SE0 (EOP signal)
    cbr     x1, USBMASK     ;1 configure no pullup on both pins
    pop     x3              ;2
    pop     YL              ;2
    out     USBDDR, x2      ;1 <-- release bus now
    out     USBOUT, x1      ;1 set pullup state
    pop     YH              ;2
    rjmp    txDoReturn      ;2 [we want to jump to rxDoReturn, but this saves cycles]


bitstuff4:                  ;1 (for branch taken)
    eor     x1, x4          ;1
    ldi     x2, 0           ;1
    out     USBOUT, x1      ;1 <-- out
    rjmp    didStuff4       ;2 jump back 2 cycles earlier
bitstuff5:                  ;1 (for branch taken)
    eor     x1, x4          ;1
    ldi     x2, 0           ;1
    sec                     ;1 set carry so that brsh is not taken
    out     USBOUT, x1      ;1 <-- out
    rjmp    didStuff5       ;2 jump back 1 cycle earlier
bitstuff6:                  ;1 (for branch taken)
    eor     x1, x4          ;1
    ldi     x2, 0           ;1
    rjmp    didStuff6       ;2 jump back 3 cycles earlier and do out there
bitstuff7:                  ;1 (for branch taken)
    eor     x1, x4          ;1
    ldi     x2, 0           ;1
    rjmp    didStuff7       ;2 jump back 4 cycles earlier

; ######################## utility functions ########################

#ifdef __IAR_SYSTEMS_ASM__
/* Register assignments for usbCrc16 on IAR cc */
/* Calling conventions on IAR:
 * First parameter passed in r16/r17, second in r18/r19 and so on.
 * Callee must preserve r4-r15, r24-r29 (r28/r29 is frame pointer)
 * Result is passed in r16/r17
 * In case of the "tiny" memory model, pointers are only 8 bit with no
 * padding. We therefore pass argument 1 as "16 bit unsigned".
 */
RTMODEL "__rt_version", "3"
/* The line above will generate an error if cc calling conventions change.
 * The value "3" above is valid for IAR 4.10B/W32
 */
#   define argLen   r18 /* argument 2 */
#   define argPtrL  r16 /* argument 1 */
#   define argPtrH  r17 /* argument 1 */

#   define resCrcL  r16 /* result */
#   define resCrcH  r17 /* result */

#   define ptrL     ZL
#   define ptrH     ZH
#   define ptr      Z
#   define byte     r22
#   define bitCnt   r19
#   define polyL    r20
#   define polyH    r21
#   define scratch  r23

#else  /* __IAR_SYSTEMS_ASM__ */ 
/* Register assignments for usbCrc16 on gcc */
/* Calling conventions on gcc:
 * First parameter passed in r24/r25, second in r22/23 and so on.
 * Callee must preserve r1-r17, r28/r29
 * Result is passed in r24/r25
 */
#   define argLen   r22 /* argument 2 */
#   define argPtrL  r24 /* argument 1 */
#   define argPtrH  r25 /* argument 1 */

#   define resCrcL  r24 /* result */
#   define resCrcH  r25 /* result */

#   define ptrL     XL
#   define ptrH     XH
#   define ptr      x
#   define byte     r18
#   define bitCnt   r19
#   define polyL    r20
#   define polyH    r21
#   define scratch  r23

#endif

; extern unsigned usbCrc16(unsigned char *data, unsigned char len);
; data: r24/25
; len: r22
; temp variables:
;   r18: data byte
;   r19: bit counter
;   r20/21: polynomial
;   r23: scratch
;   r24/25: crc-sum
;   r26/27=X: ptr
usbCrc16:
    mov     ptrL, argPtrL
    mov     ptrH, argPtrH
    ldi     resCrcL, 0xff
    ldi     resCrcH, 0xff
    ldi     polyL, lo8(0xa001)
    ldi     polyH, hi8(0xa001)
crcByteLoop:
    subi    argLen, 1
    brcs    crcReady
    ld      byte, ptr+
    ldi     bitCnt, 8
crcBitLoop:
    mov     scratch, byte
    eor     scratch, resCrcL
    lsr     resCrcH
    ror     resCrcL
    lsr     byte
    sbrs    scratch, 0
    rjmp    crcNoXor
    eor     resCrcL, polyL
    eor     resCrcH, polyH
crcNoXor:
    dec     bitCnt
    brne    crcBitLoop
    rjmp    crcByteLoop
crcReady:
    com     resCrcL
    com     resCrcH
    ret

; extern unsigned usbCrc16Append(unsigned char *data, unsigned char len);
usbCrc16Append:
    rcall   usbCrc16
    st      ptr+, resCrcL
    st      ptr+, resCrcH
    ret
