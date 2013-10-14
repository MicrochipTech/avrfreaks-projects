#if defined(__ATmega48__) || defined(__ATmega88__)
	Goto	RESET				; Reset vector
	Goto	INT0_int			; External Interrupt Request 0
	Goto	INT1_int			; External Interrupt Request 1
	Goto	PCI0_int			; Pin Change Interrupt Request 0
	Goto	PCI1_int			; Pin Change Interrupt Request 1
	Goto	PCI2_int			; Pin Change Interrupt Request 2
	Goto	WDT_int				; Watchdog Time-out Interrupt
	Goto	DDS_int				; Timer/Counter2 Compare Match A
	Goto	OC2B_int			; Timer/Counter2 Compare Match B
	Goto	OVF2_int			; Timer/Counter2 Overflow
	Goto	ICP1_int			; Timer/Counter1 Capture Event
	Goto	OC1A_int			; Timer/Counter1 Compare Match A
	Goto	OC1B_int			; Timer/Counter1 Compare Match B
	Goto	OVF1_int			; Timer/Counter1 Overflow
	Goto	OC0A_int			; TimerCounter0 Compare Match A
	Goto	OC0B_int			; TimerCounter0 Compare Match B
	Goto	OVF0_int			; Timer/Couner0 Overflow
	Goto	SPI_int				; SPI Serial Transfer Complete
	Goto	URXC_int			; USART Rx Complete
	Goto	UDRE_int			; USART, Data Register Empty
	Goto	UTXC_int			; USART Tx Complete
	Goto	ADCC_int			; ADC Conversion Complete
	Goto	ERDY_int			; EEPROM Ready
	Goto	ACI_int				; Analog Comparator
	Goto	TWI_int				; Two-wire Serial Interface
	Goto	SPMR_int			; Store Program Memory Read
#elif defined(__ATmega168__) || defined(__ATmega328__)
	Goto	RESET				; Reset vector
	Goto	INT0_int			; External Interrupt Request 0
	Goto	INT1_int			; External Interrupt Request 1
	Goto	PCI0_int			; Pin Change Interrupt Request 0
	Goto	PCI1_int			; Pin Change Interrupt Request 1
	Goto	PCI2_int			; Pin Change Interrupt Request 2
	Goto	WDT_int				; Watchdog Time-out Interrupt
	Goto	DDS_int				; Timer/Counter2 Compare Match A
	Goto	OC2B_int			; Timer/Counter2 Compare Match B
	Goto	OVF2_int			; Timer/Counter2 Overflow
	Goto	ICP1_int			; Timer/Counter1 Capture Event
	Goto	OC1A_int			; Timer/Counter1 Compare Match A
	Goto	OC1B_int			; Timer/Counter1 Compare Match B
	Goto	OVF1_int			; Timer/Counter1 Overflow
	Goto	OC0A_int			; TimerCounter0 Compare Match A
	Goto	OC0B_int			; TimerCounter0 Compare Match B
	Goto	OVF0_int			; Timer/Couner0 Overflow
	Goto	SPI_int				; SPI Serial Transfer Complete
	Goto	URXC_int			; USART Rx Complete
	Goto	UDRE_int			; USART, Data Register Empty
	Goto	UTXC_int			; USART Tx Complete`
	Goto	ADCC_int			; ADC Conversion Complete
	Goto	ERDY_int			; EEPROM Ready
	Goto	ACI_int				; Analog Comparator
	Goto	TWI_int				; Two-wire Serial Interface
	Goto	SPMR_int			; Store Program Memory Read
#elif defined(__ATmega32U4__)
	Goto	RESET				; Reset vector
	Goto	INT0_int			; External Interrupt Request 0
	Goto	INT1_int			; External Interrupt Request 1
	Goto	INT2_int			; External Interrupt Request 2
	Goto	INT3_int			; External Interrupt Request 3
	Goto	Res1addr_int		; Reserved1
	Goto	Res2addr_int		; Reserved2
	Goto	INT6_int			; External Interrupt Request 6
	Goto	Res3addr_int		; Reserved3
	Goto	PCI0_int			; Pin Change Interrupt Request 0
	Goto	USB_GEN_int			; USB General Interrupt Request
	Goto	USB_COM_int			; USB Endpoint/Pipe Interrupt Communication Request
	Goto	WDT_int				; Watchdog Time-out Interrupt
	Goto	Res4addr_int		; Reserved4
	Goto	Res5addr_int		; Reserved5
	Goto	Res6addr_int		; Reserved6
	Goto	ICP1_int			; Timer/Counter1 Capture Event
	Goto	OC1A_int			; Timer/Counter1 Compare Match A
	Goto	OC1B_int			; Timer/Counter1 Compare Match B
	Goto	OC1C_int			; Timer/Counter1 Compare Match C
	Goto	OVF1_int			; Timer/Counter1 Overflow
	Goto	OC0A_int			; TimerCounter0 Compare Match A
	Goto	OC0B_int			; TimerCounter0 Compare Match B
	Goto	OVF0_int			; Timer/Couner0 Overflow
	Goto	SPI_int				; SPI Serial Transfer Complete
	Goto	URXC1_int			; USART1 Rx Complete
	Goto	UDRE1_int			; USART1, Data Register Empty
	Goto	UTXC1_int			; USART1 Tx Complete
	Goto	ACI_int				; Analog Comparator
	Goto	ADCC_int			; ADC Conversion Complete
	Goto	ERDY_int			; EEPROM Ready
	Goto	ICP3_int			; Timer/Counter3 Capture Event
	Goto	OC3A_int			; Timer/Counter3 Compare Match A
	Goto	OC3B_int			; Timer/Counter3 Compare Match B
	Goto	OC3C_int			; Timer/Counter3 Compare Match C
	Goto	OVF3_int			; Timer/Couner3 Overflow
	Goto	TWI_int				; Two-wire Serial Interface
	Goto	SPMR_int			; Store Program Memory Read
	Goto	DDS_int				; Timer/Counter4 Compare Match A
	Goto	OC4B_int			; Timer/Counter4 Compare Match B
	Goto	OC4D_int			; Timer/Counter4 Compare Match C
	Goto	OVF4_int			; Timer/Couner4 Overflow
	Goto	FPF4_int			; Timer/Counter4 Fault Protection Interrupt
#elif defined(__AT90USB1286__)
	Goto	RESET				; Reset vector
	Goto	INT0_int			; External Interrupt Request 0
	Goto	INT1_int			; External Interrupt Request 1
	Goto	INT2_int			; External Interrupt Request 2
	Goto	INT3_int			; External Interrupt Request 3
	Goto	INT4_int			; External Interrupt Request 4
	Goto	INT5_int			; External Interrupt Request 5
	Goto	INT6_int			; External Interrupt Request 6
	Goto	INT7_int			; External Interrupt Request 7
	Goto	PCI0_int			; Pin Change Interrupt Request 0
	Goto	USB_GEN_int			; USB General Interrupt Request
	Goto	USB_COM_int			; USB Endpoint/Pipe Interrupt Communication Request
	Goto	WDT_int				; Watchdog Time-out Interrupt
	Goto	DDS_int				; Timer/Counter2 Compare Match A
	Goto	OC2B_int			; Timer/Counter2 Compare Match B
	Goto	OVF2_int			; Timer/Counter2 Overflow
	Goto	ICP1_int			; Timer/Counter1 Capture Event
	Goto	OC1A_int			; Timer/Counter1 Compare Match A
	Goto	OC1B_int			; Timer/Counter1 Compare Match B
	Goto	OC1C_int			; Timer/Counter1 Compare Match C
	Goto	OVF1_int			; Timer/Counter1 Overflow
	Goto	OC0A_int			; TimerCounter0 Compare Match A
	Goto	OC0B_int			; TimerCounter0 Compare Match B
	Goto	OVF0_int			; Timer/Couner0 Overflow
	Goto	SPI_int				; SPI Serial Transfer Complete
	Goto	URXC1_int			; USART1 Rx Complete
	Goto	UDRE1_int			; USART1, Data Register Empty
	Goto	UTXC1_int			; USART1 Tx Complete
	Goto	ACI_int				; Analog Comparator
	Goto	ADCC_int			; ADC Conversion Complete
	Goto	ERDY_int			; EEPROM Ready
	Goto	ICP3_int			; Timer/Counter3 Capture Event
	Goto	OC3A_int			; Timer/Counter3 Compare Match A
	Goto	OC3B_int			; Timer/Counter3 Compare Match B
	Goto	OC3C_int			; Timer/Counter3 Compare Match C
	Goto	OVF3_int			; Timer/Couner3 Overflow
	Goto	TWI_int				; Two-wire Serial Interface
	Goto	SPMR_int			; Store Program Memory Read
#elif defined(__ATmega2560__)
	Goto	RESET				; Reset vector
	Goto	INT0_int			; External Interrupt Request 0
	Goto	INT1_int			; External Interrupt Request 1
	Goto	INT2_int			; External Interrupt Request 2
	Goto	INT3_int			; External Interrupt Request 3
	Goto	INT4_int			; External Interrupt Request 4
	Goto	INT5_int			; External Interrupt Request 5
	Goto	INT6_int			; External Interrupt Request 6
	Goto	INT7_int			; External Interrupt Request 7
	Goto	PCI0_int			; Pin Change Interrupt Request 0
	Goto	PCI1_int			; Pin Change Interrupt Request 1
	Goto	PCI2_int			; Pin Change Interrupt Request 2
	Goto	WDT_int				; Watchdog Time-out Interrupt
	Goto	DDS_int				; Timer/Counter2 Compare Match A
	Goto	OC2B_int			; Timer/Counter2 Compare Match B
	Goto	OVF2_int			; Timer/Counter2 Overflow
	Goto	ICP1_int			; Timer/Counter1 Capture Event
	Goto	OC1A_int			; Timer/Counter1 Compare Match A
	Goto	OC1B_int			; Timer/Counter1 Compare Match B
	Goto	OC1C_int			; Timer/Counter1 Compare Match C
	Goto	OVF1_int			; Timer/Counter1 Overflow
	Goto	OC0A_int			; TimerCounter0 Compare Match A
	Goto	OC0B_int			; TimerCounter0 Compare Match B
	Goto	OVF0_int			; Timer/Couner0 Overflow
	Goto	SPI_int				; SPI Serial Transfer Complete
	Goto	URXC0_int			; USART0 Rx Complete
	Goto	UDRE0_int			; USART0, Data Register Empty
	Goto	UTXC0_int			; USART0 Tx Complete
	Goto	ACI_int				; Analog Comparator
	Goto	ADCC_int			; ADC Conversion Complete
	Goto	ERDY_int			; EEPROM Ready
	Goto	ICP3_int			; Timer/Counter3 Capture Event
	Goto	OC3A_int			; Timer/Counter3 Compare Match A
	Goto	OC3B_int			; Timer/Counter3 Compare Match B
	Goto	OC3C_int			; Timer/Counter3 Compare Match C
	Goto	OVF3_int			; Timer/Couner3 Overflow
	Goto	URXC1_int			; USART1 Rx Complete
	Goto	UDRE1_int			; USART1, Data Register Empty
	Goto	UTXC1_int			; USART1 Tx Complete
	Goto	TWI_int				; Two-wire Serial Interface
	Goto	SPMR_int			; Store Program Memory Read
	Goto	ICP4_int			; Timer/Counter4 Capture Event
	Goto	OC4A_int			; Timer/Counter4 Compare Match A
	Goto	OC4B_int			; Timer/Counter4 Compare Match B
	Goto	OC4C_int			; Timer/Counter4 Compare Match C
	Goto	OVF4_int			; Timer/Couner4 Overflow
	Goto	ICP5_int			; Timer/Counter5 Capture Event
	Goto	OC5A_int			; Timer/Counter5 Compare Match A
	Goto	OC5B_int			; Timer/Counter5 Compare Match B
	Goto	OC5C_int			; Timer/Counter5 Compare Match C
	Goto	OVF5_int			; Timer/Couner5 Overflow
	Goto	URXC2_int			; USART2 Rx Complete
	Goto	UDRE2_int			; USART2, Data Register Empty
	Goto	UTXC2_int			; USART2 Tx Complete
	Goto	URXC3_int			; USART3 Rx Complete
	Goto	UDRE3_int			; USART3, Data Register Empty
	Goto	UTXC3_int			; USART3 Tx Complete
#endif

