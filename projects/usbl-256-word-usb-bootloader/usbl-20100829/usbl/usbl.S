;;;
;;; AVR USB Bootloader by David Madden <dhm@mersenne.com>
;;; Copyright (c) 2010.
;;;
;;; This code is licensed under the GPL Version 2 ONLY.  It may not be
;;; used under the terms of any earlier or later version of the GPL.
;;; For licensing on other terms, contact the author.
;;;
;;; Implements a USB bootloader (with read, erase & write capability)
;;; in less than 256 words.
;;;
;;; FUSE-L may be set so that the reset vector is at the beginning of
;;; the boot block (FUSEH.BOOTRST=0).  However, this bootloader ONLY
;;; runs after an external reset.  After any other reset (power-on, BOD,
;;; WDT, etc.), we just jump to location 0x0000.
;;;
;;; This means that a blank device will just loop from 0x0000 to BootBlock-1
;;; (0xffff invalid opcode).
;;;
;;; Ideas to save space:
;;;
;;;  * Allocate registers for each ISR, don't bother to save/restore
;;;  * Don't even bother to save/restore condition codes, since no
;;;    nested interrupts, and main code doesn't do anything
;;;  * Hide code & data in interrupt vector table
;;;  * Use 2-byte instructions in vector table
;;;  * Use Y to point to USB registers, then use LDD/STD
;;;    (Breaks even at 2 LDS/STS ops, bonus thereafter.  Maybe use for
;;;     other setup too.)
;;;  * Make the programmer GUI do all the work: bootloader only knows how to
;;;    enumerate and read or write Flash (using [E]LPM, SPM).  GUI must do
;;;    everything, including reading signature & fuses using LPM and using
;;;    appropriately-sized Out transactions (i.e., PageSize) to write Flash.
;;;
;;; ---------------------------------------------------------------
;;; 
;;; Register Assignments
;;;
;;; R0  =  temp
;;; R1  =  temp (not === 0, since SPM uses R1:R0)
;;; R2 === 0 (use instead of traditional R1)
;;; Y  === USB_BASE
;;; 

#include <avr/io.h>

;;;
;;; Make sure RAMPZ is set correctly.  Some of the libc-avr header files
;;; aren't right.  If the MCU uses any bits of RAMPZ (i.e., it has more
;;; than 64K of Flash) then both RAMPZ and RAMPZ0 (and possibly RAMPZ1,
;;; RAMPZ2, etc.) should be defined.  We use RAMPZ0 to determine whether
;;; to use LPM or ELPM -- if there's less than 64K of Flash, then we don't
;;; have to set RAMPZ, and can use LPM.
;;; 
#if defined(RAMPZ) && !defined(RAMPZ0)
#	error "Check header files for this MCU -- RAMPZ is defined, but not RAMPZ0."
#endif

;;;
;;; Set BOOT_BASE to match your Boot/Lock Bits settings
;;;
#warning "Did you remember to set BOOT_BASE?"
#define BOOT_BASE   0x1FC00 /**/
/*#define BOOT_BASE   0 /**/

;;;
;;; Define BAREBONES to remove optional code and reduce bootloader size.
;;; If not defined, it adds a few extra things (mostly LED twiddling
;;; on the AT90USBKey)
;;; 
#define BAREBONES       /**/
/*#undef  BAREBONES       /**/

#define rZERO       R2
        
;;; 
;;; Y points to first USB register; use LDD / STD (Y+oU....) to address registers
;;; 
#define USB_BASE    UHWCON
#define oUHWCON     (UHWCON-USB_BASE)
#define oUSBCON     (USBCON-USB_BASE)
#define oUSBSTA     (USBSTA-USB_BASE)
#define oUSBINT     (USBINT-USB_BASE)
#define oUDCON      (UDCON-USB_BASE)
#define oUDINT      (UDINT-USB_BASE)
#define oUDIEN      (UDIEN-USB_BASE)
#define oUDADDR     (UDADDR-USB_BASE)
#define oUEINTX     (UEINTX-USB_BASE)
#define oUENUM      (UENUM-USB_BASE)
#define oUERST      (UERST-USB_BASE)
#define oUECONX     (UECONX-USB_BASE)
#define oUECFG0X    (UECFG0X-USB_BASE)
#define oUECFG1X    (UECFG1X-USB_BASE)
#define oUESTA0X    (UESTA0X-USB_BASE)
#define oUESTA1X    (UESTA1X-USB_BASE)
#define oUEIENX     (UEIENX-USB_BASE)
#define oUEDATX     (UEDATX-USB_BASE)
#define oUEBCX      (UEBCX-USB_BASE)
#define oUEBCLX     (UEBCLX-USB_BASE)
#define oUEINT      (UEINT-USB_BASE)
#define oUEINTX     (UEINTX-USB_BASE)

        .section    .vectors
        .fill       BOOT_BASE, 1, 0xff

        .global     reset_vector
reset_vector:
        cli                     ; Possibly unnecessary, maybe do something else?
        rjmp        start

;       .word       0
;       .long       0       /* External Interrupt Request 0 */
;       .long       0       /* External Interrupt Request 1 */
;       .long       0       /* External Interrupt Request 2 */
;       .long       0       /* External Interrupt Request 3 */
;       .long       0       /* Reserved */
;       .long       0       /* Reserved */
;       .long       0       /* External Interrupt Request 6 */
;       .long       0       /* Reserved */
;       .long       0       /* Pin Change Interrupt Request 0 */

;;;
;;;  USB Descriptors (hide 'em in unused vector space)
;;;
device_descriptor:
	    .byte       18					// bLength
	    .byte       1					// bDescriptorType
	    .byte       0x00, 0x02			// bcdUSB
	    .byte       0					// bDeviceClass
	    .byte       0					// bDeviceSubClass
	    .byte       0					// bDeviceProtocol
	    .byte       64                  // bMaxPacketSize0
	    .byte       0xc0, 0x16  		// idVendor
	    .byte       0x79, 0x04          // idProduct
	    .byte       0x00, 0x01			// bcdDevice
	    .byte       0					// iManufacturer
	    .byte       0					// iProduct
	    .byte       0					// iSerialNumber
	    .byte       1					// bNumConfigurations

config_descriptor:
	    // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	    .byte       9 					// bLength;
	    .byte       2					// bDescriptorType;
	    .byte       9 + 9, 0            // wTotalLength
	    .byte       1					// bNumInterfaces
	    .byte       1					// bConfigurationValue
	    .byte       0					// iConfiguration
	    .byte       0xC0				// bmAttributes
	    .byte       50					// bMaxPower

interface_descriptor:   
	    // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	    .byte       9					// bLength
	    .byte       4					// bDescriptorType
	    .byte       0					// bInterfaceNumber
	    .byte       0					// bAlternateSetting
	    .byte       0					// bNumEndpoints
	    .byte       0xFF				// bInterfaceClass (0xFF = Vendor Specific)
	    .byte       0x00				// bInterfaceSubClass
	    .byte       0x00				// bInterfaceProtocol
	    .byte       0					// iInterface

        .org        BOOT_BASE + 0x28
        .global     USB_General_Vector
USB_General_Vector:    
        rjmp        USB_General_ISR     /* USB General Interrupt Request */

        .org        BOOT_BASE + 0x2c
        .global     USB_Endpoint_Vector
USB_Endpoint_Vector:    
        rjmp        USB_Endpoint_ISR    /* USB Endpoint/Pipe Interrupt Communication Request */

;;; Save RJMP space by putting Endpoint ISR here.
        
;       .word       0
;       .long       0       /* Watchdog Time-out Interrupt */
;       .long       0       /* Reserved */
;       .long       0       /* Reserved */
;       .long       0       /* Reserved */
;       .long       0       /* Timer/Counter1 Capture Event */
;       .long       0       /* Timer/Counter1 Compare Match A */
;       .long       0       /* Timer/Counter1 Compare Match B */
;       .long       0       /* Timer/Counter1 Compare Match C */
;       .long       0       /* Timer/Counter1 Overflow */
;       .long       0       /* Timer/Counter0 Compare Match A */
;       .long       0       /* Timer/Counter0 Compare Match B */
;       .long       0       /* Timer/Counter0 Overflow */
;       .long       0       /* SPI Serial Transfer Complete */
;       .long       0       /* USART1, Rx Complete */
;       .long       0       /* USART1 Data register Empty */
;       .long       0       /* USART1, Tx Complete */
;       .long       0       /* Analog Comparator */
;       .long       0       /* ADC Conversion Complete */
;       .long       0       /* EEPROM Ready */
;       .long       0       /* Timer/Counter3 Capture Event */
;       .long       0       /* Timer/Counter3 Compare Match A */
;       .long       0       /* Timer/Counter3 Compare Match B */
;       .long       0       /* Timer/Counter3 Compare Match C */
;       .long       0       /* Timer/Counter3 Overflow */
;       .long       0       /* 2-wire Serial Interface         */
;       .long       0       /* Store Program Memory Read */
;       .long       0       /* Timer/Counter4 Compare Match A */
;       .long       0       /* Timer/Counter4 Compare Match B */
;       .long       0       /* Timer/Counter4 Compare Match D */
;       .long       0       /* Timer/Counter4 Overflow */
;       .long       0       /* Timer/Counter4 Fault Protection Interrupt */

;;; =================================================================
;;; ==  Entry point!
;;; =================================================================

        .global     start
start:  in          rZERO, _SFR_IO_ADDR(MCUSR)
        sbrc        rZERO, EXTRF
        rjmp        1f

        clr         rZERO       ; Not external reset (must be a POR, JTAG,
        jmp         0			; BOD or WDT instead).  Just jump to 0x0000.

1:      clr         rZERO
        out         _SFR_IO_ADDR(MCUSR), rZERO  ; Clear reset flags

        ldi         r16, _BV(WDCE) | _BV(WDE)   ; Enable WDT, ~2s timeout
        ldi         r17, _BV(WDE) | _BV(WDP2) | _BV(WDP1) | _BV(WDP0)
        sts         WDTCSR, r16
        sts         WDTCSR, r17
        wdr

#if (BOOT_BASE != 0)
        in          r16, _SFR_IO_ADDR(MCUCR)    ; Set IRQ vector table to
        mov         r17, r16                    ; beginning of boot Flash
        ori         r16, _BV(IVCE)
        ori         r17, _BV(IVSEL)
        out         _SFR_IO_ADDR(MCUCR), r16
        out         _SFR_IO_ADDR(MCUCR), r17
#endif

        ldi         r16, hi8(RAMEND)            ; Set up stack
        out         _SFR_IO_ADDR(SPH), r16      ; (**not necessary?**)
        ldi         r16, lo8(RAMEND)            ; ("Default value for SP is last
        out         _SFR_IO_ADDR(SPL), r16      ;   address of internal RAM")

#if     defined(_AVR_IOM32U4_H_)

        lds         r16, CLKSEL0                ; Set up clock (16MHz XTAL)
        ori         r16, _BV(EXTE)
        sts         CLKSEL0, r16
1:      lds         r17, CLKSTA
        sbrs        r17, EXTON
        rjmp        1b

        ldi         r17, _BV(CLKPCE)
        sts         CLKPR, r17
        sts         CLKPR, rZERO

#elif   defined(_AVR_AT90USB1287_H_)

        ldi         r16, _BV(CLKPCE)            ; Set clock divisor = 1
        sts         CLKPR, r16
        sts         CLKPR, rZERO

#else
#       error       "No clock setup code for selected CPU"
#endif

;;; ----Basic Chip Setup Finished----

;;; ----enable-debugging-------
#ifndef BAREBONES
        ldi         r16, _BV(DDD4) | _BV(DDD5) | _BV(DDD6) | _BV(DDD7)
        out         _SFR_IO_ADDR(DDRD), r16
        out         _SFR_IO_ADDR(PORTD), rZERO
#endif
;;; ----enable-debugging--------
        
;;; Set up USB ---
        ldi         YL, lo8(USB_BASE)          ; Y === USB_BASE
        ldi         YH, hi8(USB_BASE)

#if     defined(_AVR_IOM32U4_H_)

        ldi         r16, _BV(FRZCLK)            ; Stop / disable / reset USB
        std         Y+oUSBCON, r16

        ldi         r16, _BV(PINDIV)            ; Set PLL: SYSCLK/2, PLL target 48MHz
        sts         PLLCSR, r16
        ldi         r16, _BV(PDIV2)
        sts         PLLFRQ, r16
        ldi         r16, _BV(PINDIV) | _BV(PLLE)
        sts         PLLCSR, r16

1:      in          r16, _SFR_IO_ADDR(PLLCSR)    ; Wait for PLL lock
        sbrs        r16, PLOCK
        rjmp        1b

        ldi         r16, _BV(USBE) | _BV(FRZCLK) ; Enable USB, clock still frozen
        std         Y+oUSBCON, r16
        ldi         r16, _BV(UVREGE)
        std         Y+oUHWCON, r16
        std         Y+oUDIEN, rZERO
        std         Y+oUDINT, rZERO
        ldi         r16, _BV(USBE) | _BV(OTGPADE) ; Enable USB, clock running
        std         Y+oUSBCON, r16

#elif   defined(_AVR_AT90USB1287_H_)

        ldi         r16, _BV(FRZCLK)            ; Stop / disable / reset USB
        std         Y+oUSBCON, r16

#if     F_CPU == 8000000UL
        ldi         r16, (_BV(PLLP1) | _BV(PLLP0) | _BV(PLLE))
        out         _SFR_IO_ADDR(PLLCSR), r16
#elif   F_CPU == 16000000UL
        ldi         r16, (_BV(PLLP2) | _BV(PLLP0) | _BV(PLLE))
        out         _SFR_IO_ADDR(PLLCSR), r16
#else
#       error       "F_CPU must be 8MHz or 16MHz!"
#endif

1:      in          r0, _SFR_IO_ADDR(PLLCSR)    ; Wait for PLL lock
        sbrs        r0, PLOCK
        rjmp        1b

        ldi         r16, _BV(UIMOD)
        std         Y+oUHWCON, r16
        ldi         r16, _BV(USBE) | _BV(FRZCLK) ; Enable USB, clock still frozen
        std         Y+oUSBCON, r16
        ldi         r16, _BV(UIMOD) | _BV(UVCONE) | _BV(UVREGE)
        std         Y+oUHWCON, r16
        std         Y+oUDIEN, rZERO
        std         Y+oUDINT, rZERO
        
        ldi         r16, _BV(USBE) | _BV(OTGPADE) ; Enable USB, clock running
        std         Y+oUSBCON, r16
#else
#       error       "No USB setup code for selected CPU"
#endif
        ;; 
        ;; Common USB setup code...
        ;; 
        ldi         r16, 0x7f   ; Reset all endpoints
        std         Y+oUERST, r16
        clr         r16
1:      std         Y+oUENUM, r16
        std         Y+oUECONX, rZERO
        inc         r16
        cpi         r16, 7
        brlo        1b
        std         Y+oUERST, rZERO

        std         Y+oUDCON, rZERO   ; Clear DETACH (==connect)

        ldi         r16, _BV(EORSTE) | _BV(SOFE)
        std         Y+oUDIEN, r16
        sei
;;; USB is running, interrupts enabled ---

;;; ---- Do whatever idle tasks there may be...
loop:   wdr
        rjmp        loop
;;; ---- End idle task loop

;;; -----------------------------------------------------------------
;;; 
;;; USB General ISR
;;;
;;; This just watches for EORSTI (End of Reset Interrupt) and enables
;;; EP0.  You could do something with SOFI (Start of Frame Interrupt)
;;; if you had some extra bootblock space...
;;; 
        .global     USB_General_ISR
USB_General_ISR:
        ;; WARNING: REGS NOT SAVED!
        ldd         r17, Y+oUDINT   ; Copy interrupt flags
        std         Y+oUDINT, rZERO ; then clear UDINT register

        sbrs        r17, EORSTI
        rjmp        not_EORSTI

do_EORSTI:
#ifndef BAREBONES
        sbi         _SFR_IO_ADDR(PORTD), DDD4 ; Red-1 on after bus reset (until SET_ADDRESS)
#endif
        std         Y+oUENUM, rZERO   ; End of Reset Interrupt -- set up EP0

        ldi         r16, _BV(EPEN)    ; Enable EP0
        std         Y+oUECONX, r16
        std         Y+oUECFG0X, rZERO ; UECFG0X = Control, Out
        ldi         r16, _BV(EPSIZE1) | _BV(EPSIZE0) | _BV(ALLOC)
        std         Y+oUECFG1X, r16   ; UECFG1X = EPSIZE_64 | EPBUF_SINGLE | ALLOC
        std         Y+oUESTA0X, rZERO ; Clear under/overflow status interrupt flags
        std         Y+oUEINTX, rZERO  ; Clear other status & interrupt flags
        ldi         r16, _BV(RXSTPE)  ; Enable EP0 interrupt on SETUP packet
        std         Y+oUEIENX, r16

        ;; Fall through to check other interesting interrupt bits

not_EORSTI:
#ifndef BAREBONES
        sbrs        r17, SOFI
        rjmp        not_SOFI
do_SOFI:        
        sbi         _SFR_IO_ADDR(PIND), DDD6 ; Toggle Green-2
#endif
not_SOFI:
        reti
        
;;; -----------------------------------------------------------------
;;;
;;; USB Endpoint Interrupt
;;;
;;; Since only EP0 is ever enabled, this should be for that endpoint.
;;; If some other EP interrupts, we don't handle it.
;;; 
        .global     USB_Endpoint_ISR
USB_Endpoint_ISR:
        ;; WARNING: REGS NOT SAVED!
#ifndef BAREBONES
        sbi         _SFR_IO_ADDR(PORTD), DDD7 ; Red-2 while Endpoint ISR
#endif
        
        std         Y+oUENUM, rZERO ; Select EP0
        ldd         r17, Y+oUEINTX  ; Check interrupts

        sbrs        r17, RXSTPI     ; Received SETUP packet?
        rjmp        EP_ISR_END      ; We only handle SETUP packets

        ;;
        ;; Get the 8-byte setup packet
        ;; 
#if 1   /* This is a little shorter... */
        clr         XH
        ldi         XL, 18
        ldi         r16, 8
1:      ldd         r0, Y+oUEDATX
        st          X+, r0
        dec         r16
        brne        1b
#else   /* ...than this. */
        ldd         r18, Y+oUEDATX  ; bmRequestType
        ldd         r19, Y+oUEDATX  ; bRequest
        ldd         r20, Y+oUEDATX  ; wValue
        ldd         r21, Y+oUEDATX  ; wValue
        ldd         r22, Y+oUEDATX  ; wIndex
        ldd         r23, Y+oUEDATX  ; wIndex
        ldd         r24, Y+oUEDATX  ; wLength
        ldd         r25, Y+oUEDATX  ; wLength
#endif

        ;; Clear interrupt bits
        ldi         r16, ~(_BV(RXSTPI) | _BV(NAKINI) | _BV(RXOUTI) | _BV(TXINI))
        std         Y+oUEINTX, r16

        ;; Our response is based on data direction...
        sbrs        r18, 7
        rjmp        host2device

device2host:                    ; IN transaction
        cpi         r18, 0x80   ; IN, Standard, Device
        brne        not_GET_DESCRIPTOR
        cpi         r19, 6
        brne        not_GET_DESCRIPTOR

GET_DESCRIPTOR:
        ;; The BAREBONES version only has descriptors 0x0100 (Device) and
        ;; 0x0200 (Configuration).  If you had extra space, you could add
        ;; string descriptors, etc, and return them.  Just get the address
        ;; into [RAMPZ:]Z, and the length into R24, between GET_DESCRIPTOR
        ;; and send_descriptor.
        cpi         r20, 0
        brne        1f
        cpi         r21, 1
        breq        send_device_descriptor
        cpi         r21, 2
        breq        send_config_descriptor
1:      jmp         EP0_STALL
        
send_config_descriptor: 
        ldi         ZL, lo8(config_descriptor)
        ldi         ZH, hi8(config_descriptor)
        rjmp        1f
send_device_descriptor:
        ldi         ZL, lo8(device_descriptor)
        ldi         ZH, hi8(device_descriptor)
1:
#ifdef  RAMPZ0
        ldi         r16, (BOOT_BASE >> 16)
        out         _SFR_IO_ADDR(RAMPZ), r16
#endif
        cpi         r24, 18     ; Send min(requested_bytes, 18)
        brlo        send_descriptor
        ldi         r24, 18

        ;; However you get here, make sure RAMPZ:Z points to the descriptor you
        ;; want to send, and r24 has the length.  (Can't send descriptors longer
        ;; than 63 bytes, sorry.)
send_descriptor:
        ldd         r16, Y+oUEINTX ; Wait for In token
        andi        r16, _BV(TXINI)|_BV(RXOUTI)
        breq        send_descriptor

#ifndef RAMPZ0
        lpm         r0, Z+
#else
        elpm        r0, Z+
#endif
        std         y+oUEDATX, r0
        dec         r24
        brne        send_descriptor

send_packet_done:
        ldi         r16, ~_BV(TXINI)
        std         Y+oUEINTX, r16
        rjmp        EP_ISR_END

not_GET_DESCRIPTOR:
        cpi         r18, 0xc0   ; IN, Vendor, Device
        breq        do_LPM
        jmp         EP0_STALL

;;;
;;; Read device ([E]LPM, set SPSCR=r19 (bRequest)).  If wLength == 0, reset
;;; by WDT.
;;;
do_LPM: mov         r0, r24
        or          r0, r25
        breq        do_LPM      ; Hang here until WDT fires

#ifdef  RAMPZ0
        out         _SFR_IO_ADDR(RAMPZ), r20 ; lo(wValue)
#endif
        movw        r30, r22    ; wIndex

1:      call        wait_TXINI
        
        ldi         r16, 64     ; Start [first|next] 64-byte packet
2:      mov         r0, r24
        or          r0, r25
        breq        send_packet_done
        out         _SFR_IO_ADDR(SPMCSR), r19 ; bRequest

#ifndef RAMPZ0
        lpm         r0, Z+
#else
        elpm        r0, Z+
#endif

        std         y+oUEDATX, r0
        sbiw        r24, 1
        dec         r16
        brne        2b

        ldi         r16, ~_BV(TXINI) ; 64 byte packet filled ... send it
        std         Y+oUEINTX, r16
        rjmp        1b

;;;
;;; OUT Transactions: SET_ADDRESS and Write Flash
;;; 
host2device:                    ; OUT transactions

        cpi         r18, 0      ; OUT, Standard, Device
        brne        not_SET_ADDRESS
        cpi         r19, 5      ; SET_ADDRESS
        brne        not_SET_ADDRESS

SET_ADDRESS:    
#ifndef BAREBONES
        cbi         _SFR_IO_ADDR(PORTD), DDD4 ; Red-1 Off
        sbi         _SFR_IO_ADDR(PORTD), DDD5 ; Green-1 On
#endif
        std         Y+oUDADDR, r20

        ldi         r16, ~_BV(TXINI)
        std         Y+oUEINTX, r16

        call        wait_TXINI

        ori         r20, _BV(ADDEN)
        std         Y+oUDADDR, r20
        rjmp        EP_ISR_END

not_SET_ADDRESS:
;;;     (other OUT transactions -- SPM)
        cpi         r18, 0x40   ; OUT, Vendor, Device
        brne        EP0_STALL
;;;
;;; Write device (SPM, set SPSCR=r19 (bRequest))
;;;
#ifndef BAREBONES
        sbi         _SFR_IO_ADDR(PORTD), DDD4 ; Red-1 while writing
#endif

        mov         r0, r24     ; if wLength == 0, erase page @ lo(wValue):wIndex
        or          r0, r25
        brne        do_write

do_erase:
#ifdef  RAMPZ0
        out         _SFR_IO_ADDR(RAMPZ), r20 ; lo(wValue)
#endif
        movw        r30, r22

        ldi         r17, _BV(PGERS)|_BV(SPMEN)
        call        do_SPM

1:      ldd         r0, Y+oUEINTX
        sbrc        r0, NAKINI
        rjmp        rcv_data_done
        sbrs        r0, RXOUTI  ; not expecting to receive any data
        rjmp        1b
        rjmp        EP0_STALL

do_write:       
        ldd         r0, Y+oUEINTX ; Check interrupt flags
        sbrs        r0, NAKINI    ; If NAKINI, we should have received all wLength
        rjmp        2f            ; bytes.

        mov         r0, r24     ; Check for R25:R24 == 0
        or          r0, r25
        brne        EP0_STALL   ; OUT finished too soon!

        ;; wLength bytes received (and put into temp SPM buffer).  Do
        ;; SPM to write the page, then done.
#ifdef  RAMPZ0
        out         _SFR_IO_ADDR(RAMPZ), r20 ; lo(wValue)
#endif
        movw        r30, r22
        ldi         r17, _BV(PGWRT)|_BV(SPMEN)
        call        do_SPM

        rjmp        rcv_data_done
        
2:      sbrs        r0, RXOUTI     ; USB Packet ready?
        rjmp        do_write       ; No, keep waiting.

        ;;
        ;;  OUT packet ready, process the received bytes
        ;; 
        ldd         r16, Y+oUEBCLX ; # bytes received
        
3:      mov         r0, r1
        ldd         r1, Y+oUEDATX

        adiw        r30, 1

        sbrc        r30, 0      ; Sorry, this Z -= 2, SPM, Z += 2 is ugly.
        rjmp        4f          ; There's probably a cleaner way.
        sbiw        r30, 2
        ldi         r17, _BV(SPMEN)
        call        do_SPM
        adiw        r30, 2

#ifdef  RAMPZ0
        brne        4f
        in          r0, _SFR_IO_ADDR(RAMPZ)
        inc         r0
        out         _SFR_IO_ADDR(RAMPZ), r0
#endif

4:      sbiw        r24, 1
        brmi        EP0_STALL   ; Host sent too many bytes!
        dec         r16
        brne        3b

        ldi         r16, ~_BV(RXOUTI) ; This packet has been processed, wait for next
        std         Y+oUEINTX, r16
        rjmp        do_write

rcv_data_done:
        ldi         r16, ~_BV(TXINI) ; Handshake the last packet
        std         Y+oUEINTX, r16
        
#ifndef BAREBONES
        cbi         _SFR_IO_ADDR(PORTD), DDD4 ; Red-1 off after writing
#endif
        rjmp        EP_ISR_END

EP0_STALL:
        ldi         r16, _BV(STALLRQ) | _BV(EPEN)
        std         Y+oUECONX, r16

EP_ISR_END:
#ifndef BAREBONES
        cbi         _SFR_IO_ADDR(PORTD), DDD7 ; Red-2 off after EP Interrupt
#endif
        reti

wait_TXINI:
        ldd         r15, Y+oUEINTX ; Wait for TXINI (OK to transmit)
        sbrs        r15, TXINI
        rjmp        wait_TXINI
        ret

do_SPM: in          r15, _SFR_IO_ADDR(SPMCSR)
        sbrc        r15, SPMEN
        rjmp        do_SPM
        out         _SFR_IO_ADDR(SPMCSR), r17
        spm

        .end
