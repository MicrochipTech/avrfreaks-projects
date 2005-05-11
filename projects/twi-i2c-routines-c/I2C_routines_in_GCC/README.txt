 This is a Two Wire Interface (I2C) module. There are two methods - poll based
and hardware based. The poll based method doesn't need special AVR hardware. 
The hardware method, on the other hand, is believed to be more robust because 
it utilizes the native AVR TWI hardware module.
 
 The module includes some simple routines for accessing I2C EEPROM devices.
 
 If timeout handling is required, the user must provide decrementing in
Timer/Counter ISR for two variables:
	cntr_twi 	: for TWI timeout
	cntr_tweep_wait	: for EEPROM timeout
The exact decrement timing is not critical as the timeout value is user-defined
(look at TWI_TIMEOUT/TWEEP_TIMEOUT in twi_m_conf.h).  An example ISR :

SIGNAL(SIG_OVERFLOW0)
{
	if(cntr_twi) cntr_twi--;
	if(cntr_tweep_wait) cntr_tweep_wait--;
}


WARNING: The hardware based routines are not fully tested so I would appreciate
any feedback.


FILES:
twi_m.c      : Main code for the module (both SW and HW methods)
twi_m.h      : Header file for the outside world (list of exported functions)
twi_m_conf.h : Configuration file with various module-specific options.
main.h       : Excerpt from my usual "main.h" with helpful macroses and global
               configuration options.
