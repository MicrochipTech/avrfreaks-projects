#ifdef HAS_EEPROM

//_____ D E F I N I T I O N S ______________________________________________

//! @defgroup DALI_define EEPROM defines EEPROM parameters
//! 28 bytes EEprom are needed to store dali registers
//! @{

#define PROG_MODE           0
//!<0: Erase & Write (3.6ms)  1: Erase Only (1.8ms)  2: Write Only (1.8ms)
#define EEREADY_INTERRUPT   0
//!<0: Interrupt when EEprom Ready Disabled   1: interrupt enabled

//! @}

//_____ M A C R O S ________________________________________________________

//! @defgroup EEprom_Macros macros used to setup EEprom
//! @{

#define ee_setup_eecr()           ( EECR = 0,\
                                  EECR = (PROG_MODE<<EEPM0 | EEREADY_INTERRUPT<<EERIE) )    

//! @}

void ee_write(U8 address, U8 data);
U8 ee_read(U8 address);

#endif

