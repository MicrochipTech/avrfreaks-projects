/* bms/cpu2/eeprom.c */


#define ReadEE( a )	eeprom_read_byte( (void*)(a) )
#define WriteEE( a, b )	eeprom_write_byte( (void*)(a), (unsigned char)(b) )

#define isEEfree()	(!(EECR & (1 << EEWE)))


// log for 250 record
LOGGER( TF_LOG, 250, sizeof(eeprom_t) );


static eeprom_t old_Tf;

static void
InitEEPROM( void )
{
	unsigned short k, i;

	Log_Init( TF_LOG );
	Log_ReadLast( TF_LOG, (void*) &cur_Tf );
	cur_Tf.flag = 0;

	k = get_K_filter( cur_Tf.ADC_Tf, FULL_ADC_CYCLE );
	i = 7;
	while ( i ) {
		-- i;
		ADC_Filters[i].k = k;
	}

	k = get_K_filter( cur_Tf.SPI_Tf, FULL_SPI_CYCLE );
	i = 7;
	while ( i ) {
		-- i;
		SPI_Filters[i].k = k;
	}
	old_Tf = cur_Tf;
} // InitEEPROM


unsigned char
WriteCurTf( void )
{
	return Log_NoblockingWrite( TF_LOG, (void*) &cur_Tf );
} // WriteCurTf


static void
ProcEEPROM( void )
{
#define abs( a )	(((a) > 0) ? (a) : -(a))

	if ( (abs(cur_Tf.Vol_Up_Limit - old_Tf.Vol_Up_Limit) >= 200)
		|| (abs(cur_Tf.Vol_Down_Limit - old_Tf.Vol_Down_Limit) >= 200)
		|| (abs(cur_Tf.Temp_Up_Limit - old_Tf.Temp_Up_Limit) >= 3)
		|| (abs(cur_Tf.Temp_Down_Limit - old_Tf.Temp_Down_Limit) >= 3)
		|| (abs(cur_Tf.ADC_Tf - old_Tf.ADC_Tf) >= 20)
		|| (abs(cur_Tf.SPI_Tf - old_Tf.SPI_Tf) >= 20) )
	{
		if ( WriteCurTf() ) old_Tf = cur_Tf;
	} else {
		Log_NoblockingWrite( TF_LOG, 0 );
	}
} // ProcEEPROM


/*
int main( void )
{
	....
	InitEEPROM();
	....
	do {
		....
		ProcEEPROM();
		....
	} while (1);
}
*/


/* End of file  eeprom.c */
