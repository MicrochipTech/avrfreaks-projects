;
;	Starting with the Atmel C code for multi-master slave...
;



;/****************************************************************************
;  TWI State codes
;****************************************************************************/
;// General TWI Master staus codes                      
.equ TWI_START                  = 0x08  ;// START has been transmitted  
.equ TWI_REP_START              = 0x10  ;// Repeated START has been transmitted
.equ TWI_ARB_LOST               = 0x38  ;// Arbitration lost

;// TWI Master Transmitter staus codes                      
.equ TWI_MTX_ADR_ACK            = 0x18  ;// SLA+W has been tramsmitted and ACK received
.equ TWI_MTX_ADR_NACK           = 0x20  ;// SLA+W has been tramsmitted and NACK received 
.equ TWI_MTX_DATA_ACK           = 0x28  ;// Data byte has been tramsmitted and ACK received
.equ TWI_MTX_DATA_NACK          = 0x30  ;// Data byte has been tramsmitted and NACK received 

;// TWI Master Receiver staus codes  
.equ TWI_MRX_ADR_ACK            = 0x40  ;// SLA+R has been tramsmitted and ACK received
.equ TWI_MRX_ADR_NACK           = 0x48  ;// SLA+R has been tramsmitted and NACK received
.equ TWI_MRX_DATA_ACK           = 0x50  ;// Data byte has been received and ACK tramsmitted
.equ TWI_MRX_DATA_NACK          = 0x58  ;// Data byte has been received and NACK tramsmitted

;// TWI Slave Transmitter staus codes
.equ TWI_STX_ADR_ACK            = 0xA8  ;// Own SLA+R has been received; ACK has been returned
.equ TWI_STX_ADR_ACK_M_ARB_LOST = 0xB0  ;// Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
.equ TWI_STX_DATA_ACK           = 0xB8  ;// Data byte in TWDR has been transmitted; ACK has been received
.equ TWI_STX_DATA_NACK          = 0xC0  ;// Data byte in TWDR has been transmitted; NOT ACK has been received
.equ TWI_STX_DATA_ACK_LAST_BYTE = 0xC8  ;// Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received

;// TWI Slave Receiver staus codes
.equ TWI_SRX_ADR_ACK            = 0x60  ;// Own SLA+W has been received ACK has been returned
.equ TWI_SRX_ADR_ACK_M_ARB_LOST = 0x68  ;// Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
.equ TWI_SRX_GEN_ACK            = 0x70  ;// General call address has been received; ACK has been returned
.equ TWI_SRX_GEN_ACK_M_ARB_LOST = 0x78  ;// Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
.equ TWI_SRX_ADR_DATA_ACK       = 0x80  ;// Previously addressed with own SLA+W; data has been received; ACK has been returned
.equ TWI_SRX_ADR_DATA_NACK      = 0x88  ;// Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
.equ TWI_SRX_GEN_DATA_ACK       = 0x90  ;// Previously addressed with general call; data has been received; ACK has been returned
.equ TWI_SRX_GEN_DATA_NACK      = 0x98  ;// Previously addressed with general call; data has been received; NOT ACK has been returned
.equ TWI_SRX_STOP_RESTART       = 0xA0  ;// A STOP condition or repeated START condition has been received while still addressed as Slave

;// TWI Miscellaneous status codes
.equ TWI_NO_STATE               = 0xF8  ;// No relevant state information available; TWINT = “0”
.equ TWI_BUS_ERROR              = 0x00  ;// Bus error due to an illegal START or STOP condition

.equ TWI_BUFFER_SIZE = 8

.equ	TWI_Slave_DevAdr = 0xF0

;.equ	I2C_sw_config = 0x3D	;enable primary smb and on-board buses, no power supply IPMB's
;.equ	I2C_sw_config = 0x3C	;dis-enable primary smb for now to aviod driver issue
;.equ	I2C_sw_DevAdr = 0xE0

;union TWI_statusReg                       // Status byte holding flags.
;{
;    unsigned char all;
 ;   struct
;    {
;        unsigned char lastTransOK:1;      
.equ	TWI_Stat_lastTransOK_mask = 0x01
.equ	TWI_Stat_lastTransOK_bit = 0
.equ	TWI_Stat_Master_mask = 0x02		;set this bit when starting a master operation, clear it when done
.equ	TWI_Stat_Master_bit = 1
.equ	TWI_Stat_Slave_mask = 0x04		;set this bit at the start of a slave operation, clear it when done
.equ	TWI_Stat_Slave_bit = 2
;
.equ	TWI_Stat_RxAdr_mask = 0x08
.equ	TWI_Stat_RxAdr_bit = 3
;        unsigned char genAddressCall:1;                        // TRUE = General call, FALSE = TWI Address;
.equ	TWI_Stat_genAddressCall_mask = 0x10
.equ	TWI_Stat_genAddressCall_bit = 4

.equ	TWI_Stat_tryother_mask = 0x20
.equ	TWI_Stat_tryother_bit = 5
;        unsigned char unusedBits:5;
;    };
;};

.dseg
TWI_Master_Buf:	.DB	0,0,0,0,0,0,0,0,0,0	;TWI_BUFFER_SIZE
TWI_msgSize:	.db 0
TWI_State:		.db	0
TWI_bufPtr:		.db 0
TWI_statusReg:	.db 0
TWI_Last_RegAdr:	.db 0

.cseg

/****************************************************************************
Call this function to set up the TWI master/slave interface to its initial standby state.
Remember to enable interrupts from the main application after initializing the TWI.
****************************************************************************/
TWI_Init:
;  TWAR = TWI_ownAddress;                            // Set own TWI slave address. Accept TWI General Calls.
	ldi	Temp, TWI_Slave_DevAdr
	sts	TWAR, Temp
;  TWDR = 0xFF;                                      // Default content = SDA released.
	clr Temp
	com Temp
	sts	TWDR, Temp

; now the other stuff to be done...

	ldi Temp, twbr_init
	sts	TWBR, Temp			; set the baud rate control register
	ldi	Temp, twsr_init
	sts	TWSR, Temp
	clr Temp
	sts TWI_Last_RegAdr, Temp
	sts	TWI_State, Temp
	sts	TWI_statusReg, Temp
	sts	TWI_bufPtr, Temp
	sts	TWI_msgSize, Temp


;  TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins.
;         (1<<TWIE)|(1<<TWINT)|                      // Disable TWI Interupt.
;         (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Do not ACK on any requests, yet.
;         (0<<TWWC);                                 //
	ldi	Temp, 0xC5		;put it into passive mode with IRQs
	sts	TWCR, Temp

	sei			;enable interrupts...

	ret
;}


; Now the hard part, the IRQ service routine...
/****************************************************************************
This function is the Interrupt Service Routine (ISR), and called when the TWI interrupt is triggered;
that is whenever a TWI event has occurred. This function should not be called directly from the main
application.
****************************************************************************/

TWI_ISR:
	st	-Y, Temp
	in	Temp, SREG
	st	-Y, Temp
	st	-Y, T2
;	st	-Y, XL
;	st	-Y, XH

	lds	Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_tryother_mask
	sts	TWI_StatusReg, Temp
	sbrc	Temp, TWI_Stat_Slave_bit		;check to see if there is a slave operation going on, and if so, go deal with it
	rjmp	TWI_SLV
	sbrs	Temp, TWI_Stat_Master_bit		;then check for master since slave has priority
	rjmp	TWI_SLV
								;and fall into slave if neither is set
	st	-Y, XL
	st	-Y, XH
	rjmp TWI_MISR

;  switch (TWSR)					
;  {
TWI_SLV:
	lds	Temp, TWSR

;    case TWI_SRX_STOP_RESTART:       // A STOP condition or repeated START condition has been received while still addressed as Slave    
TWI_ISR_7:						;THIS MUST BE AT THE TOP OF THE ISR due to undocumented performance issues with the interface
	ldi	T2, TWI_SRX_STOP_RESTART
	cpse	Temp, T2
	rjmp	TWI_ISR_0

;	ldi	Temp, 0xC5
;	sts	TWCR, Temp

	lds	Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_Slave_mask | TWI_Stat_RxAdr_mask
	sts	TWI_statusReg, Temp				;done with this transaction, so clear the bit

	sbrs	Temp, TWI_Stat_Master_bit
	rjmp	TWI_ISR_7_1

	ldi	Temp, 15		;do a little delay so that there is at least 5 usec of idle
lp_7:
	dec Temp
	brne	lp_7

	ldi	Temp, 0xE5		;xxxx - was A5 send out a start to begin or restart the master operation
	sts	TWCR, Temp
	rjmp	TWI_ISR_7_2

TWI_ISR_7_1:

	ldi	Temp, 0xC5
	sts	TWCR, Temp
TWI_ISR_7_2:

	ld	T2, Y+
	ld	Temp, Y+		;go ahead and reti quickly here restoring only the minimal regs saved.
	out	SREG, Temp
	ld	Temp, Y+

	reti

;      break;
;	rjmp	TWI_ISR_SLVDONE_BRK


TWI_ISR_0:	;    case TWI_STX_ADR_ACK:            // Own SLA+R has been received; ACK has been returned
	st	-Y, XL
	st	-Y, XH					;save the extra regs used beyond this point

	ldi	T2, TWI_STX_ADR_ACK
	cpse	Temp, T2
	rjmp	TWI_ISR_1

TWI_ISR_0_1:
;so the right thing to do here is to find out the current read address and load that data,
;	and may be inc if the flag is set, and point to it.
;Then go ahead and put the data in to be read out.

	lds	Temp, TWI_statusReg
	sbr	Temp, TWI_Stat_Slave_mask
	sts	TWI_statusReg, Temp

	lds	Temp, TWI_Last_RegAdr
	ldi	XL, low(I2C_Slave_Base)
	ldi	XH, high(I2C_Slave_Base)
	clr	T2
	add	XL, Temp
	adc	XH, T2

TWI_ISR_0_2:
	ld	Temp, X
	sts	TWDR, Temp
;      TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;             (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to send byte
;             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // 
;             (0<<TWWC);                                 //  
	ldi	Temp, 0xC5
	sts	TWCR, Temp

;if it's a read of CTRLREG1, we need to clear the program_ack bit if it's set
	lds Temp, TWI_Last_RegAdr
	cpi	Temp, CTRL1_offset
	brne	TWI_ISR_0_3

	lds	Temp, I2C_CTRL1
	cbr	Temp, CTRL1_ReProg_Actv_mask
	sts	I2C_CTRL1, Temp

TWI_ISR_0_3:			;if the autoinc bit is set, inc and wrap if needed the last regadr
	lds	T2, I2C_CTRL1
	sbrs	T2, CTRL1_Adr_AutoInc_bit
	rjmp	TWI_ISR_BRK	;***** this was _0_2, which is an inf. loop if autoinc is not set *****
	inc Temp
	sts	TWI_Last_RegAdr, Temp
	cpi	Temp, I2C_NumRegs
	brlo	TWI_ISR_0_4
	clr	Temp
	sts	TWI_Last_RegAdr, Temp

TWI_ISR_0_4:
;      break;
	rjmp	TWI_ISR_BRK
	

;    case TWI_STX_DATA_ACK:           // Data byte in TWDR has been transmitted; ACK has been received
TWI_ISR_1:
	ldi	T2, TWI_STX_DATA_ACK
	cpse	Temp, T2
	rjmp	TWI_ISR_2
;      TWDR = TWI_buf[TWI_bufPtr++];
;so the right thing here is to give it the next data byte, and since this was an ACK, incr the reg adr.
	lds	Temp, TWI_Last_RegAdr
TWI_ISR_1_1:
	ldi	XL, low(I2C_Slave_Base)
	ldi	XH, high(I2C_Slave_Base)
	clr	T2
	add	XL, Temp
	adc	XH, T2

	inc Temp
	sts	TWI_Last_RegAdr, Temp
	cpi	Temp, I2C_NumRegs
	brlo	TWI_ISR_1_2
	clr	Temp
	sts	TWI_Last_RegAdr, Temp

TWI_ISR_1_2:
	ld	Temp, X
	sts	TWDR, Temp

;if it's a read of CTRLREG1, we need to clear the program_ack bit if it's set
	lds Temp, TWI_Last_RegAdr
	dec	Temp
	cpi	Temp, CTRL1_offset
	brne	TWI_ISR_1_3

	lds	Temp, I2C_CTRL1
	cbr	Temp, CTRL1_ReProg_Actv_mask
	sts	I2C_CTRL1, Temp

TWI_ISR_1_3:
;      TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;             (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to send byte
;             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // 
;             (0<<TWWC);                                 //  
	ldi	Temp, 0xC5
	sts	TWCR, Temp
;      break;
	rjmp	TWI_ISR_BRK

;    case TWI_STX_DATA_NACK:          // Data byte in TWDR has been transmitted; NACK has been received. 
;                                     // I.e. this is the end of the transmission.
TWI_ISR_2:
	ldi	T2, TWI_STX_DATA_NACK
	cpse	Temp, T2
	rjmp	TWI_ISR_3
;		Store TWI State
	lds Temp, TWSR
	sts	TWI_state, Temp
;
	lds	Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_Slave_mask
	sts	TWI_statusReg, Temp				;done with the slave transaction, so clear the bit

TWI_ISR_2_1:
                                            ;// Put TWI Transceiver in passive mode.
;      TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins
;             (1<<TWIE)|(1<<TWINT)|                  
;             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|    
;             (0<<TWWC);                          
;	ldi	Temp, 0xC5
;	sts	TWCR, Temp
;      break;
	rjmp	TWI_ISR_SLVDONE_BRK

;    case TWI_SRX_GEN_ACK:            // General call address has been received; ACK has been returned
;//    case TWI_SRX_GEN_ACK_M_ARB_LOST: // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
TWI_ISR_3:
	ldi	T2, TWI_SRX_GEN_ACK
	cpse	Temp, T2
	rjmp	TWI_ISR_4
;      TWI_statusReg.genAddressCall = TRUE;
	lds	T2, TWI_statusReg
	sbr	T2, TWI_Stat_genAddressCall_mask
	sts	TWI_StatusReg, T2
; go back into passive here since we don't support general call stuff
                                            ;// Put TWI Transceiver in passive mode.
;      TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins
;             (1<<TWIE)|(1<<TWINT)|                  
;             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|    
;             (0<<TWWC);                          
	ldi	Temp, 0xC5
	sts	TWCR, Temp
;      break;
	rjmp	TWI_ISR_BRK

;    case TWI_SRX_ADR_ACK:            // Own SLA+W has been received ACK has been returned
TWI_ISR_4:
	ldi	T2, TWI_SRX_ADR_ACK
	cpse	Temp, T2
	rjmp	TWI_ISR_5
;
;so we are getting a write, the first byte is the register addr, so set a flag to indicate this.
;	we can ignore the current data register, it's just our slave address.
;                                                        // Dont need to clear TWI_S_statusRegister.generalAddressCall due to that it is the default state.
;      TWI_statusReg.RxDataInBuf = TRUE;
TWI_ISR_4_0:
	lds	T2, TWI_StatusReg
	sbr	T2, TWI_Stat_RxAdr_mask | TWI_Stat_Slave_mask
	sts	TWI_StatusReg, T2
;                                                        // Reset the TWI Interupt to wait for a new event.
;      TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;             (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to send byte
;             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Expect ACK on this transmission
;             (0<<TWWC);                                 //
	ldi	Temp, 0xC5
	sts	TWCR, Temp
;      break;
	rjmp	TWI_ISR_BRK

;    case TWI_SRX_ADR_DATA_ACK:       // Previously addressed with own SLA+W; data has been received; ACK has been returned
TWI_ISR_5:
	ldi	T2, TWI_SRX_ADR_DATA_ACK
	cpse	Temp, T2
;	rjmp	TWI_ISR_6
;	rjmp	TWI_ISR_5_1
;    case TWI_SRX_GEN_DATA_ACK:       // Previously addressed with general call; data has been received; ACK has been returned
;TWI_ISR_6:
;	ldi	T2, TWI_SRX_GEN_DATA_ACK
;	cpse	Temp, T2
	rjmp	TWI_ISR_8

;TWI_ISR_5_1:
;      TWI_buf[TWI_bufPtr++]     = TWDR;
;so this should look at the first recieved byte, and if it's a valid addr, set the last_adr var to it
;	if it's not the first byte, figure out which byte it is, offset the addr correctly, and put it in,
;	remembering to check for wraps, RO regs, etc.
	lds	T2, TWI_statusReg
	sbrs	T2, TWI_Stat_RxAdr_bit
	rjmp	TWI_ISR_5_2

	cbr	T2, TWI_Stat_RxAdr_mask		;clear the bit since we have the adr byte
	sts	TWI_statusReg, T2

	lds	Temp, TWDR					;so this is the first byte, so it's the reg address
	cpi	Temp, I2C_NumRegs
	brge	TWI_ISR_2_1				;if the addr is over the limit, go into passive mode 

	sts	TWI_Last_RegAdr, Temp		;otherwise load the data
                                            ;// Put TWI Transceiver in passive mode.
;      TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins
;             (1<<TWIE)|(1<<TWINT)|                  
;             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|    
;             (0<<TWWC);                          
	ldi	Temp, 0xC5
	sts	TWCR, Temp
;      break;
	rjmp	TWI_ISR_BRK

TWI_ISR_5_2:
	lds	Temp, TWI_Last_RegAdr		;this is really the current regadr...
	cpi	Temp, I2C_RW_Offset
	brlo	TWI_ISR_2_1				;if it's not writeable, go into passive mode

	ldi	XL, low(I2C_Slave_Base)		;this is a real data byte write, so validate the address for writing
	ldi	XH, high(I2C_Slave_Base)
	clr T2
	add	XL, Temp
	adc	XH, T2

	ldi	T2, CTRL0_offset
	cpse	Temp, T2
	rjmp	TWI_ISR_5_5
	rjmp	Save_CTRL0
TWI_ISR_5_5:
	inc	T2
	cpse	Temp, T2
	rjmp	TWI_ISR_5_8

	lds	T2, I2C_CTRL1				;this is a write to CTRL1, so save the previous value and set a flag
	sts	Prev_CTRL1, T2
	lds	T2, CReg_Status
	sbr	T2, CTRL1_Changed_mask
	sts	CReg_Status, T2
	rjmp	TWI_ISR_5_6

Save_CTRL0:
	lds	T2, I2C_CTRL0				;this is a write to CTRL0, so save the previous value and set a flag
	sts	Prev_CTRL0, T2
	lds	T2, CReg_Status
	sbr	T2, CTRL0_Changed_mask
	sts	CReg_Status, T2
	rjmp	TWI_ISR_5_6

TWI_ISR_5_8:
;
;	this stuff is commented out as it does not apply to the TWI example code....
;
;	ldi	T2, Prog_Csum_offset		;check for a cksum write
;	cpse	Temp, T2
;	rjmp	TWI_ISR_5_6

;	ldi	T2, 1
;	sts	Prog_Csum_flg, T2			;set the checksum write flag and handle it outside of the ISR

TWI_ISR_5_6:
	inc	Temp						;update the pointer to the next register
	cpi	Temp, I2C_NumRegs
	brge	TWI_ISR_5_4
	
	sts	TWI_Last_RegAdr, Temp		;save the new register pointer
	rjmp	TWI_ISR_5_7
TWI_ISR_5_4:
	clr	T2
	sts	TWI_Last_RegAdr, T2


TWI_ISR_5_7:
	lds	Temp, TWDR					;get the data
	st	X, Temp						;put it in
;      TWI_statusReg.lastTransOK = TRUE;                 // Set flag transmission successfull.
;	lds	T2, TWI_statusReg
;	sbr	T2, TWI_Stat_lastTransOK_mask
;	sts	TWI_StatusReg, T2
;                                                        // Reset the TWI Interupt to wait for a new event.
;      TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;             (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to send byte
;             (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send ACK after next reception
;             (0<<TWWC);                                 //
	ldi	Temp, 0xC5
	sts	TWCR, Temp
;      break;
	rjmp	TWI_ISR_BRK

;    case TWI_BUS_ERROR:         // Bus error due to an illegal START or STOP condition
;    default:
TWI_ISR_8:
	ldi	T2, TWI_BUS_ERROR
	cpse Temp, T2
	rjmp TWI_ISR_9

	lds Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_Slave_mask | TWI_Stat_RxAdr_mask | TWI_Stat_Master_mask
	sts	TWI_statusReg, Temp

	ldi	Temp, 0xD5				;to clear a bus error and go into active listen slave mode, you have to set the stop bit
	sts	TWCR, Temp				; see the misc states section of the datasheet for details, no stop is actually sent
	rjmp	TWI_ISR_BRK	

	;    case TWI_SRX_ADR_DATA_NACK:      // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
;    case TWI_SRX_GEN_DATA_NACK:      // Previously addressed with general call; data has been received; NOT ACK has been returned
;    case TWI_STX_DATA_ACK_LAST_BYTE: // Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
;//    case TWI_NO_STATE              // No relevant state information available; TWINT = “0”
TWI_ISR_9:
;      TWI_state = TWSR;                                 // Store TWI State as errormessage, operation also clears the Success bit.      
	sts	TWI_state, Temp

	lds	Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_Slave_mask
	sts	TWI_statusReg, Temp
	sbrc	Temp, TWI_Stat_tryother_bit
	rjmp	TWI_ISR_9_0

	sbr	Temp, TWI_Stat_tryother_mask		;go try all of the master codes too
	sts TWI_StatusReg, Temp
	rjmp	TWI_MISR

;  Oh, go ahead and pretend that things are OK and go passive
TWI_ISR_9_0:
	cbr	Temp, TWI_Stat_tryother_mask
	sts	TWI_StatusReg, Temp

;	ldi	Temp, 0xC5
;	sts	TWCR, Temp
;  }
TWI_ISR_SLVDONE_BRK:
						;if the slave operation is done, check to see if there is a master sitting around
						; from an arbitration loss, and if so, go ahead and restart it
	lds	Temp, TWI_statusReg
	sbrs	Temp, TWI_Stat_Master_bit
	rjmp	TWI_ISR_SD1_BRK

	ldi	Temp, 15		;do a little delay so that there is at least 5 usec of idle
lp_done:
	dec Temp
	brne	lp_done

	ldi	Temp, 0xE5		;xxxx - was A5 send out a start to begin or restart the master operation
	sts	TWCR, Temp
	rjmp	TWI_ISR_BRK

TWI_ISR_SD1_BRK:
	ldi	Temp, 0xC5
	sts	TWCR, Temp

TWI_ISR_BRK:
	ld	XH, Y+
	ld	XL, Y+
	ld	T2, Y+
	ld	Temp, Y+
	out	SREG, Temp
	ld	Temp, Y+

	reti


;	Now here is the master code,


; The assumption coming into this routine is that the data and message length has already been loaded into
;	TWI_Master_buf and TWI_msgSize, so all we have to do is put it into master mode and start the start.
;	all of the irq race worry is probably not needed since the ISR can handle a concurrent master and slave
;	operation, but it looks good to do it if anyone is sick enough to read this code.
;	*** changed to not have irq's off to reduce latency, see TWI_Wait ***
;
TWI_Master_Send:

;xxxxxxxxxxxxxxxxx
;	testing code... set XB10 high just before sending a start .....

;	lds	Temp, TWCR
;	cbr	Temp, 0x40
	ldi	Temp, 0x05			;disable the interface until the start command is issued
	sts	TWCR, Temp

	lds	Temp, XB10_port
	sbr	Temp, XB10_mask
	sts	XB10_port, Temp

;	ldi	Temp, 100
;TWI_MS_1:
;	dec Temp
;	brne	TWI_MS_1		;delay a bit to see if it helps the window...

;	lds	Temp, TWI_StatusReg
;	sbrc	Temp, TWI_Stat_Slave_bit
;	rjmp	TWI_MS_1

;TWI_Master_Send_1:
;	ldi	T2, 8
;	wdr
;TWI_Master_Send_2:
;	in	Temp, TWI_port
;	andi	Temp, TWI_mask			;this is to wait for the bus to not be busy, sort of, to try and eliminate a race between slave address recognision and sending a start
;	cpi	Temp, TWI_mask
;	brne	TWI_Master_Send_1
;	dec	T2
;	brne TWI_Master_Send_2
				
;  TWCR = (1<<TWEN)|                             // TWI Interface enabled.
;         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
;         (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
;         (0<<TWWC);                             //
	ldi	Temp, 0xE5	;xxxx was E5
	sts	TWCR, Temp
	sei
	ret


TWI_MISR:
;{
;  static unsigned char TWI_bufPtr;
;  
;  switch (TWSR)
	lds	Temp, TWSR
;  {

;    case TWI_START:             // START has been transmitted  
;    case TWI_REP_START:         // Repeated START has been transmitted
	ldi	T2, TWI_START
	cpse	Temp, T2
	rjmp	TWI_MISR_0_2
	rjmp	TWI_MISR_0_1
TWI_MISR_0_2:
	ldi	T2, TWI_REP_START
	cpse	Temp, T2
	rjmp	TWI_MISR_1

TWI_MISR_0_1:
;      TWI_bufPtr = 0;
	clr	T2                      ;// Set buffer pointer to the TWI Address location
	inc T2						;go ahead and inc it to save time later
	sts	TWI_bufPtr, T2

;here we need to go ahead and send the device addr byte out, so do it.

;	ldi	XL, low(TWI_Master_Buf)
;	ldi	XH,	high(TWI_Master_Buf)
	lds	T2, TWI_Master_Buf					;offset is 0 so just use the base addr
	sts	TWDR, T2
;        TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to send byte
;               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
;               (0<<TWWC);                                 //
	ldi	Temp, 0xC5		;xxxx was 85
	sts	TWCR, Temp
	rjmp	TWI_ISR_BRK

;    case TWI_MTX_ADR_ACK:       // SLA+W has been tramsmitted and ACK received
TWI_MISR_1:
	ldi	T2, TWI_MTX_ADR_ACK
	cpse	Temp, T2
	rjmp	TWI_MISR_1_1
	rjmp	TWI_MISR_1_0

;    case TWI_MTX_DATA_ACK:      // Data byte has been tramsmitted and ACK received
TWI_MISR_1_1:
	ldi	T2, TWI_MTX_DATA_ACK
	cpse	Temp, T2
	rjmp	TWI_MISR_2
TWI_MISR_1_0:
;      if (TWI_bufPtr < TWI_msgSize)
	lds	Temp, TWI_bufPtr
	lds	T2, TWI_msgSize
	cp	Temp, T2
	brge	TWI_MISR_1_2
;      {
;        TWDR = TWI_buf[TWI_bufPtr++];
	ldi	XL, low(TWI_Master_Buf)
	ldi	XH,	high(TWI_Master_Buf)
	clr	T2
	add	XL, Temp
	adc	XH, T2
	ld	T2, X
	sts	TWDR, T2
	inc	Temp
	sts	TWI_bufPtr, Temp
;        TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to send byte
;               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
;               (0<<TWWC);                                 //
	ldi	Temp, 0xC5		;xxxx was 85
	sts	TWCR, Temp
	rjmp	TWI_ISR_BRK
;      }else                    // Send STOP after last byte
TWI_MISR_1_2:
;      {
;        TWI_statusReg.lastTransOK = TRUE;                 // Set status bits to completed successfully.
	lds	Temp, TWI_statusReg
	sbr	Temp, TWI_Stat_lastTransOK_mask
	cbr Temp, TWI_Stat_Master_mask
	sts	TWI_statusReg, Temp 
;        TWCR = (1<<TWEN)|                                 // TWI Interface enabled 
;               (1<<TWIE)|(1<<TWINT)|                      // in passive mode so that slave works,
;               (1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
;               (0<<TWWC);                                 //
	ldi	Temp, 0xD5
	sts	TWCR, Temp

;xxxxxxxxxxxxxxxxx
;	testing code... set XB10 low just before sending a stop .....
	lds	Temp, XB10_port
	cbr	Temp, XB10_mask
	sts	XB10_port, Temp

;      }
;      break;
	rjmp	TWI_ISR_BRK

;    case TWI_MRX_DATA_ACK:      // Data byte has been received and ACK tramsmitted
TWI_MISR_2:
	ldi	T2, TWI_MRX_DATA_ACK
	cpse	Temp, T2
	rjmp	TWI_MISR_2_1
;      TWI_buf[TWI_bufPtr++] = TWDR;	;put the byte into the buffer
	ldi	XL, low(TWI_Master_Buf)
	ldi	XH,	high(TWI_Master_Buf)
	clr	T2
	lds	Temp, TWI_bufPtr
	add	XL, Temp
	adc	XH, T2
	lds	T2, TWDR
	st	X, T2

	inc	Temp
	sts	TWI_bufPtr, Temp
	rjmp	TWI_MISR_2_2

;    case TWI_MRX_ADR_ACK:       // SLA+R has been tramsmitted and ACK received
TWI_MISR_2_1:
	ldi	T2, TWI_MRX_ADR_ACK
	cpse	Temp, T2
	rjmp	TWI_MISR_3

	lds	Temp, TWI_bufPtr

TWI_MISR_2_2:

;      if (TWI_bufPtr < (TWI_msgSize-1) )                  // Detect the last byte to NACK it.
	lds	T2, TWI_msgSize
	dec	T2
	cp	Temp, T2
	brge	TWI_MISR_2_3
;      {
;        TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to read next byte
;               (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send ACK after reception
;               (0<<TWWC);                                 //
	ldi	Temp, 0xC5
	sts	TWCR, Temp
	rjmp	TWI_ISR_BRK
;      }else                    // Send NACK after next reception
TWI_MISR_2_3:
;      {
;        TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to read next byte
;               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send NACK after reception
;               (0<<TWWC);                                 //
	ldi	Temp, 0x85			;xxxx was 85
	sts	TWCR, Temp
;      }    
;      break;
	rjmp	TWI_ISR_BRK

;    case TWI_MRX_DATA_NACK:     // Data byte has been received and NACK tramsmitted
TWI_MISR_3:
	ldi	T2, TWI_MRX_DATA_NACK
	cpse	Temp, T2
	rjmp	TWI_MISR_4

;      TWI_buf[TWI_bufPtr] = TWDR;
	ldi	XL, low(TWI_Master_Buf)
	ldi	XH,	high(TWI_Master_Buf)
	clr	T2
	lds	Temp, TWI_bufPtr
	add	XL, Temp
	adc	XH, T2
	lds	T2, TWDR
	st	X, T2

;      TWI_statusReg.lastTransOK = TRUE;                 // Set status bits to completed successfully. 
	lds	Temp, TWI_statusReg
	sbr	Temp, TWI_Stat_lastTransOK_mask
	cbr	Temp, TWI_Stat_Master_mask						;and master mode finished
	sts	TWI_statusReg, Temp 
;      TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;             (1<<TWIE)|(1<<TWINT)|                      // in passive mode so slave works
;             (1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
;             (0<<TWWC);                                 //
	ldi	Temp, 0xD5
	sts	TWCR, Temp
;xxxxxxxxxxxxxxxxx
;	testing code... set XB10 high just before sending a start .....
	lds	Temp, XB10_port
	cbr	Temp, XB10_mask
	sts	XB10_port, Temp

;      break;
	rjmp	TWI_ISR_BRK

;    case TWI_ARB_LOST:          // Arbitration lost
TWI_MISR_4:
	ldi	T2, TWI_ARB_LOST
	cpse	Temp, T2
	rjmp	TWI_MISR_5
							;If we're master and lose arb, just restart for now, but...
	clr	T2
	sts	TWI_bufPtr, T2		;reset the buffer pointer so when we get it we start over

;      TWCR = (1<<TWEN)|                                 // TWI Interface enabled
;             (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag
;             (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|           // Initiate a (RE)START condition.
;             (0<<TWWC);                                 //
	ldi	Temp, 0xE5
	sts	TWCR, Temp
;xxxxxxxxxxxxxxxxx
;	testing code... set XB10 high just before sending a start .....
	lds	Temp, XB10_port
	cbr	Temp, XB10_mask
	sts	XB10_port, Temp

;      break;
	rjmp	TWI_ISR_BRK




;//    case TWI_STX_ADR_ACK_M_ARB_LOST: // Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
TWI_MISR_5:
	ldi	T2, TWI_STX_ADR_ACK_M_ARB_LOST
	cpse	Temp, T2
	rjmp	TWI_MISR_5_0
	rjmp	TWI_MISR_5_1
TWI_MISR_5_0:	
	ldi	T2, TWI_STX_ADR_ACK			;this can also happen in a race when the slave address comes in after the master bit is set
	cpse	Temp, T2
	rjmp	TWI_MISR_5_5	
;	So this is the ugly race problem between master and slave, so the master loses, process the slave request
;	This generates the state where both master and slave mode are set for the duration of the slave process
TWI_MISR_5_1:
	lds	Temp, TWI_statusReg
	sbr	Temp, TWI_Stat_Slave_mask
	sts	TWI_statusReg, Temp

	clr	T2
	sts	TWI_bufPtr, T2			;reset the Master mode buffer pointer to the start

;	ldi	Temp, 0xC5				;do the slave thing
;	sts	TWCR, Temp
;xxxxxxxxxxxxxxxxx
;	testing code... set XB10 high just before sending a start .....
	lds	Temp, XB10_port
	cbr	Temp, XB10_mask
	sts	XB10_port, Temp

	jmp	TWI_ISR_0_1			;jump to the right slave code

;//    case TWI_SRX_ADR_ACK_M_ARB_LOST: // Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
TWI_MISR_5_5:
	ldi	T2, TWI_SRX_ADR_ACK_M_ARB_LOST
	cpse	Temp, T2
	rjmp	TWI_MISR_5_6
	rjmp	TWI_MISR_5_7
TWI_MISR_5_6:
	ldi	T2, TWI_SRX_ADR_ACK 				;this can also happen when the slave is addressed after master is set
	cpse	Temp, T2
	rjmp	TWI_MISR_6

;	So this is the ugly race problem between master and slave, so the master loses, process the slave request
;	This generates the state where both master and slave mode are set for the duration of the slave process
TWI_MISR_5_7:
	lds	Temp, TWI_statusReg
	sbr	Temp, TWI_Stat_Slave_mask
	sts	TWI_statusReg, Temp

	clr	T2
	sts	TWI_bufPtr, T2			;reset the Master mode buffer pointer to the start

;	ldi	Temp, 0xC5				;do the slave thing
;	sts	TWCR, Temp
;xxxxxxxxxxxxxxxxx
;	testing code... set XB10 high just before sending a start .....
	lds	Temp, XB10_port
	cbr	Temp, XB10_mask
	sts	XB10_port, Temp

	jmp	TWI_ISR_4_0			;jump to the right slave code

;    case TWI_MTX_ADR_NACK:      // SLA+W has been tramsmitted and NACK received
;    case TWI_MRX_ADR_NACK:      // SLA+R has been tramsmitted and NACK received 
TWI_MISR_6:
	ldi	T2, TWI_MTX_ADR_NACK
	cpse	Temp, T2
	rjmp	TWI_MISR_6_1
	rjmp	TWI_MISR_6_0

TWI_MISR_6_1:
	ldi	T2, TWI_MRX_ADR_NACK
	cpse	Temp, T2
	rjmp	TWI_MISR_7
TWI_MISR_6_0:					;so we have addressed a device that's not there, get the state and send a stop
;        TWI_statusReg.lastTransOK = FALSE;                 // Set status bits to completed unsuccessfully.
	lds	Temp, TWI_statusReg
	cbr	Temp, TWI_Stat_lastTransOK_mask | TWI_Stat_Master_mask
	sts	TWI_statusReg, Temp 
;        TWCR = (1<<TWEN)|                                 // TWI Interface enabled 
;               (1<<TWIE)|(1<<TWINT)|                      // in passive mode so that slave works,
;               (1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
;               (0<<TWWC);                                 //
	ldi	Temp, 0xD5
	sts	TWCR, Temp
;xxxxxxxxxxxxxxxxx
;	testing code... set XB10 high just before sending a start .....
	lds	Temp, XB10_port
	cbr	Temp, XB10_mask
	sts	XB10_port, Temp

;      }
;      break;
	rjmp	TWI_ISR_BRK

;    case TWI_MTX_DATA_NACK:     // Data byte has been tramsmitted and NACK received
;//    case TWI_NO_STATE              // No relevant state information available; TWINT = “0”
;    case TWI_BUS_ERROR:         // Bus error due to an illegal START or STOP condition
;    default:     
TWI_MISR_7:
;      TWI_state = TWSR;                                 // Store TWSR and automatically clears noErrors bit.
	lds	Temp, TWSR
	sts	TWI_state, Temp

	lds	Temp,TWI_StatusReg 
	sbrc	Temp, TWI_Stat_tryother_bit
	rjmp	TWI_MISR_7_0

	sbr	Temp, TWI_Stat_tryother_mask		;go try all of the slave codes too
	sts	TWI_StatusReg, Temp
	rjmp	TWI_SLV

;  Oh, go ahead and pretend that things are OK and go passive
TWI_MISR_7_0:
	cbr	Temp, TWI_Stat_tryother_mask
	sts	TWI_StatusReg, Temp
;                                                        // Send a Stop
;      TWCR = (1<<TWEN)|                                 // 
;             (1<<TWIE)|(1<<TWINT)|                      // 
;             (1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // 
;             (0<<TWWC);                                 //
	ldi	Temp, 0xD5
	sts	TWCR, Temp
;xxxxxxxxxxxxxxxxx
;	testing code... set XB10 high just before sending a start .....
	lds	Temp, XB10_port
	cbr	Temp, XB10_mask
	sts	XB10_port, Temp

	lds	Temp, TWI_statusReg									;clear the state bits 
	cbr	Temp, TWI_Stat_Master_mask  | TWI_Stat_lastTransOK_mask
	sts	TWI_statusReg, Temp

	rjmp	TWI_ISR_BRK
;  }

;}


