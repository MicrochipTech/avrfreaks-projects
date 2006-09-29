
; These are the ATMega128 I/O pins as an example.
;	For this TWI project, this is just example stuff and is not important, but is left here so that
;	 things work with minimal hacking or clean-up.  Please don't get too involved with any of this, it's not
;	 important for this project.
;
; To test inputs, do an IN of the port, followed by an SBRC or SBRS using the bit
; To set or clear outputs, do an IN of the port, followed by an SBR or CBR of one or more masks, and an OUT
;
;	Note that input ports use PINx and output ports use PORTx, so bidirectional pins won't work without directly
;		dealing with the ports
;	Note that PortG and PortF are above the IN/OUT address limit, so STS and LDS must be used for them.

;PortA are output pins, with 5 OC's and 3 normal outputs, so set it up that way. 
;	Initialze to enable primary supplies, disable SCP's, no resets.
;	Most of these are OC driven signals to avoid contension with the secondary PEM and other drivers
;		so the port is the DDR and written to a 1 with a 0 in the PORT bit to drive low (inverting logic).
;
.equ	PORTA_DDR = 0x1F ;SCP_Pri_5V_En_mask | SCP_Sec_5V_En_mask | I2C_Reset_mask	;the normal output pins
.equ	PORTA_Init = 0xF0 ;I2C_Reset_mask + the OC bits for pullups
.equ	PORTA_Init_WD = 0xF4 ;Special value for watchdog reset to eliminate SCP power cycle
;PSA Inhibit	PA0 - AD0	51, drive low to turn off primary supply, otherwise float.  This means writing a 1 to DDR to drive low
.equ PSPri_Inhibit_port = PORTA
.equ PSPri_Inhibit_mask = 0x01
.equ PSPri_Inhibit_bit = 0
;PSB Inhibit	PA1 - AD1	50, drive low to turn off secondary supply, otherwise float
.equ PSSec_Inhibit_port = PORTA
.equ PSSec_Inhibit_mask = 0x02
.equ PSSec_Inhibit_bit = 1
;SCP-A 5V Enable	PA2 - AD2	49, drive high to turn on primary SCP
.equ SCP_Pri_5V_En_port = PORTA
.equ SCP_Pri_5V_En_mask = 0x04
.equ SCP_Pri_5V_En_bit = 2
;SCP-B 5V Enable	PA3 - AD3	48, drive high to turn on secondary SCP
.equ SCP_Sec_5V_En_port = PORTA
.equ SCP_Sec_5V_En_mask = 0x08
.equ SCP_Sec_5V_En_bit = 3
;I2C Reset	PA4 - AD4	47, drive low to reset/disable bus switch and LEDs
.equ I2C_Reset_port = PORTA
.equ I2C_Reset_mask = 0x10
.equ I2C_Reset_bit = 4
;System Reset	PA5 - AD5	46, drive low to reset the line cards, otherwise float
.equ Sys_Reset_port = DDRA
.equ Sys_Reset_mask = 0x20
.equ Sys_Reset_bit = 5
;PWR_GOOD0	PA6 - AD6	45, drive low to indicate primary supply problem, otherwise float
.equ Pwr_Good_Pri_port = DDRA
.equ Pwr_Good_Pri_mask = 0x40
.equ Pwr_Good_Pri_bit = 6
;PWR_GOOD1	PA7 - AD7	44, drive low to indicate secondary supply problem, otherwise float
.equ Pwr_Good_Sec_port = DDRA
.equ Pwr_Good_Sec_mask = 0x80
.equ Pwr_Good_Sec_bit = 7


;PortB are input pins, so things are easier here.  Go ahead and turn on the weak pullups for all of them.
.equ PORTB_DDR = 0x00
.equ PORTB_Init = 0xFF
;
;PSA Present	PB0 - SS	10, low when primary supply is present
.equ PSPri_Present_port = PINB
.equ PSPri_Present_mask = 0x01
.equ PSPri_Present_bit = 0
;PSA Fail	PB1 - SCK	11, low when primary supply fails
.equ PSPri_Fail_port = PINB
.equ PSPri_Fail_mask = 0x02
.equ PSPri_Fail_bit = 1
;PSA Degraded	PB2 - MOSI	12, low when primary supply is degraded
.equ PSPri_Degraded_port = PINB
.equ PSPri_Degraded_mask = 0x04
.equ PSPri_Degraded_bit = 2
;PSB Fail	PB3 - MISO	13, low when secondary supply fails
.equ PSSec_Fail_port = PINB
.equ PSSec_Fail_mask = 0x08
.equ PSSec_Fail_bit = 3
;PSB Degraded	PB4 - OC0	14, low when secondary supply is degraded
.equ PSSec_Degraded_port = PINB
.equ PSSec_Degraded_mask = 0x10
.equ PSSec_Degraded_bit = 4
;PSB Present	PB5 - OC1A	15, low when secondary supply is present
.equ PSSec_Present_port = PINB
.equ PSSec_Present_mask = 0x20
.equ PSSec_Present_bit = 5
;SCP-A Present	PB6 - OC1B	16, low when primary SCP is present
.equ SCP_Pri_Present_port = PINB
.equ SCP_Pri_Present_mask = 0x40
.equ SCP_Pri_Present_bit = 6
;SCP-B Present	PB7 - OC2/OC1C	17, low when secondary SCP is present
.equ SCP_Sec_Present_port = PINB
.equ SCP_Sec_Present_mask = 0x80
.equ SCP_Sec_Present_bit = 7


;PortC are input pins, so things are easy here.  Go ahead and turn on the weak pullups for all of them.
.equ PORTC_DDR = 0x00
.equ PORTC_Init = 0xFF
;
;Alarm_in0	PC0 - A8	35, low when external alarm interface input 1 is active
.equ Alarm_in0_port = PINC
.equ Alarm_in0_mask = 0x01
.equ Alarm_in0_bit = 0
;Alarm_in1	PC1 - A9	36, low when external alarm interface input 2 is active
.equ Alarm_in1_port = PINC
.equ Alarm_in1_mask = 0x02
.equ Alarm_in1_bit = 1
;Other PEM Present	PC2 - A10	37, low when the other PEM is present
.equ Other_PEM_Present_port = PINC
.equ Other_PEM_Present_mask = 0x04
.equ Other_PEM_Present_bit = 2
;PEM ID	PC3 - A11	38, low for PEM0, high for PEM1
.equ PEM_ID_port = PINC
.equ PEM_ID_mask = 0x08
.equ PEM_ID_bit = 3
;Fan Full	PC4 - A12	39, low when either temp sensor hits thermostat point
.equ Fan_Full_port = PINC
.equ Fan_Full_mask = 0x10
.equ Fan_Full_bit = 4
;SCP-A 5V Status	PC5 - A13	40, low when the primary  SCP 5V goes over-current
.equ SCP_Pri_5V_Stat_port = PINC
.equ SCP_Pri_5V_Stat_mask = 0x20
.equ SCP_Pri_5V_Stat_bit = 5
;SCP-B 5V Status	PC6 - A14	41, low when the secondary SCP 5V goes over-current
.equ SCP_Sec_5V_Stat_port = PINC
.equ SCP_Sec_5V_Stat_mask = 0x40
.equ SCP_Sec_5V_Stat_bit = 6
;Input Fuse Status	PC7 - A15	42, low when either of the input power fuses are blown
.equ In_Fuse_Stat_port = PINC
.equ In_Fuse_Stat_mask = 0x80
.equ In_Fuse_Stat_bit = 7


;UC SCL	PD0 - SCL/INT0	25, I2C/SMB clock
;UC SDA	PD1 - SDA/INT1	26, I2C/SMB data
.equ TWI_port = PIND
.equ TWI_mask = 0x03
;UART1 Rx	PD2 - RXD1/INT2	27, serial port 1Rx from other PEM, or IRQ
;UART1 TX	PD3 - TXD1/INT3	28, serial port 1 Tx to other PEM or IRQ to other PEM

;PortD is a mix of special purpose and I/O pins, so things are a bit messy here.
;	Go ahead and turn on the weak pullups for all of them.
.equ PORTD_DDR = 0xC0
.equ PORTD_Init = 0x7F
.equ PORTD_Init_WD = 0xFF
;
;PXB2	PD4 - ICP1	29, extra connection to other PEM's PXB3 signal, let's make this an input for now
.equ PXB2_port = PIND
.equ PXB2_mask = 0x10
.equ PXB2_bit = 4
;PXB3	PD5 - XCK1	30, extra connection to other PEM's PXB2 signal
.equ PXB3_port = PORTD
.equ PXB3_mask = 0x20
.equ PXB3_bit = 5
;PEM Present	PD6 - T1	31, drive low to indicate PEM is present and happy, connects to other PEM's Other PEM Present
.equ PEM_Present_port = PORTD
.equ PEM_Present_mask = 0x40
.equ PEM_Present_bit = 6
;Osc Enable	PD7 - T2	32, drive high to enable the 20MHz osc for the backplane clocks
.equ Osc_Enable_port = PORTD
.equ Osc_Enable_mask = 0x80
.equ Osc_Enable_bit = 7


;UART0 Rx	PE0 - RXD0/PDI	2, serial port 0 Rx from debug header
;UART0 Tx	PE1 - TXD0/PDO	3, serial port 0 Tx to debug header
;
;PortE are all regular output pins, so things are easy here.
.equ PORTE_DDR = 0xFC			;just the upper two are normal outputs
.equ PORTE_Init = 0x00
.equ PORTE_Init_WD = 0x40		;for watchdog resets, don't kill the clock
;
;Relay0-0	PE2 - XCK0/AIN0	4, pulse high to reset alarm output relay 0
.equ Alarm_Rly0_Clr_port = PORTE
.equ Alarm_Rly0_Clr_mask = 0x04
.equ Alarm_Rly0_Clr_bit = 2
;Relay0-1	PE3 - OC3A/AIN1	5, pulse high to set alarm output relay 0
.equ Alarm_Rly0_Set_port = PORTE
.equ Alarm_Rly0_Set_mask = 0x08
.equ Alarm_Rly0_Set_bit = 3
;Relay1-0	PE4 - OC3B/INT4	6, pulse high to reset alarm output relay 1
.equ Alarm_Rly1_Clr_port = PORTE
.equ Alarm_Rly1_Clr_mask = 0x10
.equ Alarm_Rly1_Clr_bit = 4
;Relay1-1	PE5 - OC3C/INT5	7, pulse high to set alarm output relay 1
.equ Alarm_Rly1_Set_port = PORTE
.equ Alarm_Rly1_Set_mask = 0x20
.equ Alarm_Rly1_Set_bit = 5
;Primary Clk Enable	PE6 - T3/INT6	8, drive high to enable primary SCP/SDB clock driver
.equ Pri_Clk_En_port = PORTE
.equ Pri_Clk_En_mask = 0x40
.equ Pri_Clk_En_bit = 6
;Secondary Clk Enable	PE7 - ICP3/INT7	9, drive high to enable secondary SCP/SDB clock driver
.equ Sec_Clk_En_port = PORTE
.equ Sec_Clk_En_mask = 0x80
.equ Sec_Clk_En_bit = 7

.equ PORTF_DDR = 0x00
.equ PORTF_Init = 0x00		;make them inputs without pullups, although this shouldn't matter.
;3.3V Monitor	PF0 - ADC0	61, analog input for 3.3 supply, 2.0V nominal
;5V Monitor	PF1 - ADC1	60, analog input for 5V supply, 2.0V nominal
;12V Monitor	PF2 - ADC2	59, analog input for 12V supply, 2.0V nominal
;SCP-A 5V Monitor	PF3 - ADC3	58, analog input for primary SCP supply, 2.0V
;SCP-B 5V Monitor	PF4 - ADC4/TCLK	57, analog input for secondary SCP supply, also JTAG TCK, so can not be monitored when JTAG is active
;TMS	PF5 - ADC5/TMS	56, JTAG TMS to header
;TDO	PF6 - ADC6/TDO	55, JTAG TDO to header
;TDI	PF7 - ADC7/TDI	54, JTAG TDI to header

;PortG pins are currently not used, so set it up as an input with pullups, driving the enable to the quickswitch low
;.equ PORTG_DDR = 0x10
.equ PORTG_DDR = 0x11	;xxxxxxx for testing, make XB10 an output
;.equ PORTG_Init = 0x1F
.equ PORTG_Init = 0x0E	;xxxxxxxx and init it low
;XB10	PG0 - WR	33, extra bussed line 10 to linecards
.equ XB10_port = PORTG
.equ XB10_mask = 0x01
.equ XB10_bit = 0
;XB11	PG1 - RD	34, extra bussed line 11 to linecards
.equ XB11_port = PORTG
.equ XB11_mask = 0x02
.equ XB11_bit = 1
;XB12	PG2 - ALE	43, extra bussed line 12 to linecards
.equ XB12_port = PORTG
.equ XB12_mask = 0x04
.equ XB12_bit = 2
;XB13	PG3 - TOSC2	18, extra bussed line 13 to linecards
.equ XB13_port = PORTG
.equ XB13_mask = 0x08
.equ XB13_bit = 3
;XB Enable	PG4 - TSOC1	19, drive low to enable quickswitch for XB lines, init to disabled
.equ XB_En_port = PORTG
.equ XB_En_mask = 0x10
.equ XB_En_bit = 4

;
; Go ahead and initialize all of the I/O pins per the above information
;
Init_IO:

	ldi	Temp, PORTA_Init
	in T2, MCUCSR
	sbrc	T2, WDRF
	ldi	Temp, PORTA_Init_WD
	out PORTA, Temp
	ldi	Temp, PORTA_DDR
	out	DDRA, Temp

	ldi	Temp, PORTB_Init
	out	PORTB, Temp
	ldi	Temp, PORTB_DDR
	out DDRB, Temp

	ldi	Temp, PORTC_Init
	out	PORTC, Temp
	ldi	Temp, PORTC_DDR
	out DDRC, Temp

	ldi	Temp, PORTD_Init
	in T2, MCUCSR
	sbrc	T2, WDRF
	ldi	Temp, PORTD_Init_WD
	out	PORTD, Temp
	ldi	Temp, PORTD_DDR
	out DDRD, Temp

	ldi	Temp, PORTE_Init
	in T2, MCUCSR
	sbrc	T2, WDRF
	ldi	Temp, PORTE_Init_WD
	out	PORTE, Temp
	ldi	Temp, PORTE_DDR
	out	DDRE, Temp

	ldi	Temp, PORTF_Init
	sts	PORTF, Temp
	ldi	Temp, PORTF_DDR
	sts	DDRF, Temp

	ldi	Temp, PORTG_Init
	sts	PORTG, Temp
	ldi	Temp, PORTG_DDR
	sts	DDRG, Temp

	ret

