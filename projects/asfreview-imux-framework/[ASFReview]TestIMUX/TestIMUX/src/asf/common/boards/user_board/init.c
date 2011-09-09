/**
 * \file
 *
 * \brief User board initialization template
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

/* Prototype definitions */
void init_IOPorts(void);
void InitSPI_F(void);
void init_ADCA(void);
void init_USART(USART_t serport);
void init_TWI(TWI_t I2CPort);
void init_PowerControl(void);

extern void SetExpansionOutputs(char output);

/* Main function of this file/block: initialization */
void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	Disable_global_interrupt();
	sysclk_init(); //Start up the system clock
	init_IOPorts(); //Let's configure the on board IO ports.
	CLK.RTCCTRL = 0x0B; //Time to start up the RTC.
	//osc_wait_ready(XOSC_TYPE_32KHZ);//Wait Ocillator startup.
	InitSPI_F();//Now it is a good moment to configure the SPI, since the PWR pin at the expansion output port might be required.
	SetExpansionOutputs(PWR|GREENLED|YELLOWLED);//Thus here we also init the Output Expansion port
	//Let's continue initializing the remaining peripherals required, starting by the TWI on port F
	init_TWI(TWIF);
	//Serial port on port F for USB comms.
	init_USART(DEBUGPORT);
	init_ADCA();//ADC setup but without starting it (intended to be used with DMA).
	//IRQ Controller setup.
	//Port F timers set up for ADC conversions and system tick(s).
	init_PowerControl();//Power control
	//CPU init (if required, not known at this stage of development).
	Enable_global_interrupt();
}

/*
 * \brief Ports and pins initializations
 * 
 */
void init_IOPorts(void)
{
	port_pin_t tPin;
	port_pin_flags_t tPinFlags;
	void *pPort;
	
	/* Initialize Port A
	 * Analog inputs for all analog sensors: accelerometers, gyros, mic+pot, battery
	 */
	tPin = 0xFF; //All inputs are equal.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_INPUT_DISABLE; //Configured as analog inputs
	pPort = (void *)((uintptr_t)&PORTA);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);
	
	/* Initialize Port B
	 * External analog inputs + J TAG for the AVRDragon used to debug the initial template project.
	 */
	tPin = 0xFF; //All inputs are equal.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_INPUT_DISABLE; //Configured as analog inputs
	pPort = (void *)((uintptr_t)&PORTB);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);
	
	/* Initialize Port C: Expansion, initially all inputs with pull ups
	 */
	tPin = 0xFF; //All inputs are equal.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_PULL_UP; //Configured as inputs
	pPort = (void *)((uintptr_t)&PORTC);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);

	/* Initialize Port D: Expansion, initially all inputs with pull ups
	 */
	tPin = 0xFF; //All inputs are equal.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_PULL_UP; //Configured as inputs
	pPort = (void *)((uintptr_t)&PORTD);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);

	/* Initialize Port E: Expansion, initially all inputs with pull ups
	 */
	tPin = 0xFF; //All inputs are equal.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_PULL_UP; //Configured as inputs
	pPort = (void *)((uintptr_t)&PORTE);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);

	/* Initialize Port F
	 * I2C used for the barometer/altimeter with temperature, magnetometer/compass and eventually WII nun chuck - or similar
	 * SPI used for three major components: expansion outputs (LED's, Power, scale, etc), ZigBee-like wireless module link, and Flash memory
	 * USART that can be used with USB interface if assembled
	 * and a signal to control the SPI periphery, in this port, for the Flash
	 */
	tPin = 0xFF; //All inputs are equal.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_PULL_UP; //Configured as inputs
	pPort = (void *)((uintptr_t)&PORTF);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);
	
	/* Initialize Port H: Expansion Bus Interface for external SRAM in BGA (1MB!!)
	 * This particular port also has one pin used to sense the user/power pushbutton.
	 */
	tPin = 0xFF; //All inputs are equal.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_PULL_UP; //Configured as inputs
	pPort = (void *)((uintptr_t)&PORTH);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);

	/* Initialize Port J: Expansion Bus Interface for external SRAM in BGA (1MB!!)
	 */
	tPin = 0xEF; //All inputs are equal except PF4, output
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_PULL_UP; //Configured as inputs
	pPort = (void *)((uintptr_t)&PORTJ);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);
	
	tPin = 0x10; //Mem Active High.
	tPinFlags = IOPORT_DIR_OUTPUT + IOPORT_TOTEM; //Configured as inputs
	pPort = (void *)((uintptr_t)&PORTJ);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);
	PORTF.OUTSET = 0x10;

	/* Initialize Port K: Expansion Bus Interface for external SRAM in BGA (1MB!!)
	 */
	tPin = 0xFF; //All outputs are equal.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_PULL_UP; //Configured as inputs
	pPort = (void *)((uintptr_t)&PORTK);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);

	/* Initialize Port Q: External RTC crystal
	 * and two interrupts:
	 * EOC - End of Conversion from the Barometer,
	 * INT - Interrupt request from the wireless module
	 */
	tPin = 0x03; //RTC pins.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_INPUT_DISABLE; //Configured as analog inputs
	pPort = (void *)((uintptr_t)&PORTQ);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);
	
	tPin = 0x0C; //IRQ pins.
	tPinFlags = IOPORT_DIR_INPUT + IOPORT_PULL_UP + IOPORT_FALLING; //Configured as falling edge IRQ inputs
	ioport_configure_port_pin(pPort,tPin,tPinFlags);
	/* NOTE: Interrupts for these pins are not enabled, neither configured, at this point. 
	 * Only when the IRQ Controller is configured, they can be assigned to request an interrupt 
	 */

	/* Initialize Port R: 
	 * Extra output signals to control SPI for Wireless (WL) module or Strobe for Output extension port
	 */
	tPin = 0x03; //All outputs are equal. Remember that this port has only two in/outputs.
	tPinFlags = IOPORT_DIR_OUTPUT + IOPORT_INIT_HIGH + IOPORT_SRL_ENABLED + IOPORT_TOTEM; //Configured as analog inputs
	pPort = (void *)((uintptr_t)&PORTR);
	ioport_configure_port_pin(pPort,tPin,tPinFlags);
}

/*
 * \brief Initialization of  PORTF's SPI. Since it is only oneSFR write, it is declared as inline.
 */
/*inline */void InitSPI_F(void)
{
	SPI_t *port;
	char aux;
	
	port = &SPIF;
	aux = 0x51;//SPI_ENABLE_bp + SPI_MASTER_bp + SPI_PRESCALER1_bp;//(1<<SPI_ENABLE_bp)|(1<<SPI_MASTER_bp)|(0 << SPI_PRESCALER1_bp) | (1 << SPI_PRESCALER0_bp);//SPI in master mode, and enabled. Prescaler CLK/16 and mode 00
	SPIF.CTRL = 0x51;//aux;
}	

/*
 * \brief Initialization of PORTA's ADC: it digitizes the three axis accelerometers, three axis gyros, battery/supply and mic/pot
 */
void init_ADCA(void)
{
		uint16_t cal;
		ADC_t *ADC;
		
		//General registers of ADC A
		cal = adc_get_calibration_data(ADC_CAL_ADCA); //Get calibration data from signature by driver
		ADC = &ADCA;
		ADC->CAL = cal; //Store calibration data in its place
		ADC->CTRLB = 0; //12bit, unipolar, no free run.
		ADC->REFCTRL = (1<<ADC_REFSEL0_bp); //Reference voltage set to internal VCC/1.6
		ADC->EVCTRL = 0; //No events currently
		ADC->PRESCALER = 3; //Prescaler = CLK/32;
		
		//Registers for ADC channel 0 setup
		ADC->CH0.CTRL = (ADC_CH_INPUTMODE0_bm);//Single ended
		ADC->CH0.MUXCTRL = (0<<ADC_CH_MUXPOS0_bp); //Channel 0 is input ADCA0 in single ended -> VBattery;
		//Registers for ADC channel 1 setup
		ADC->CH1.CTRL = (ADC_CH_INPUTMODE0_bm);//Single ended
		ADC->CH1.MUXCTRL = (2<<ADC_CH_MUXPOS0_bp); //Channel 1 is input ADCA2 in single ended -> AccelX;
		//Registers for ADC channel 2 setup
		ADC->CH2.CTRL = (ADC_CH_INPUTMODE0_bm);//Single ended
		ADC->CH2.MUXCTRL = (3<<ADC_CH_MUXPOS0_bp); //Channel 0 is input ADCA3 in single ended -> AccelY;
		//Registers for ADC channel 3 setup
		ADC->CH3.CTRL = (ADC_CH_INPUTMODE0_bm);//Single ended
		ADC->CH3.MUXCTRL = (4<<ADC_CH_MUXPOS0_bp); //Channel 0 is input ADCA4 in single ended -> AccelZ;
		
		//And, finally, enable the peripheral
		ADC->CTRLA = ADC_ENABLE_bm; //Enable the ADC
}

/*
 * \brief Initialization of serial port.
 */
void init_USART(USART_t serport)
{
	
}

/*
 * \brief Initialization of the TWI interface for on board peripherals (and an eventual Wii nunchuck)
 */
void init_TWI(TWI_t I2CPort)
{
	twi_options_t options;
	
	options.chip = 0x14;  //TBD.
	options.speed = 400;  //Useless??
	options.speed_reg = 35; //Baudrate per datasheet
	twi_master_init(&I2CPort,&options);
}

/*
 * \brief Stops all unused peripherals
 */
void init_PowerControl(void)
{
	PR.PRGEN = (1<<PR_RTC_bp);//Only RT is used by now
	PR.PRPA = (1<<PR_ADC_bp);//Only ADC in port A is used by now
	PR.PRPB = 0; //Nothing used here
	PR.PRPC = (1<<PR_USART1_bp);//The USARTC1 is used as 'debug port'
	PR.PRPD = 0;
	PR.PRPE = 0;
	PR.PRPF = (1<<PR_USART0_bp)|(1<<PR_TWI_bp)|(1<<PR_SPI_bp); //USARTF0 is used as 'USB serial link', TWI is used for some peripherals, as well as SPI
}