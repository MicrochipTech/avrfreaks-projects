#include <avr/io.h>

#include "lcd.h"

void t6963_init(void) {

    /* set I/O */
    LCD_NCE_SET_OUTPUT_HIGH();
    LCD_NWR_SET_OUTPUT_HIGH();
    LCD_NRD_SET_OUTPUT_HIGH();
    LCD_CND_SET_OUTPUT();
    LCD_DB_SET_OUTPUT();
    
#ifdef LCD_NPWR_SET_OUTPUT_LOW   
    /* power on */
    LCD_NPWR_SET_OUTPUT_LOW();
#endif
  
    t6963_status_check_mask = T6963_STATUS_CHECK_COMMAND_DATA;
    
    t6963_send_command_uint16(T6963_SET_TEXT_HOME_ADDRESS,T6963_TEXT_HOME_ADDRESS);
    t6963_send_command_columns(T6963_SET_TEXT_AREA);
    t6963_send_command_uint16(T6963_SET_GRAPHIC_HOME_ADDRESS,T6963_GRAPHIC_HOME_ADDRESS);
    t6963_send_command_columns(T6963_SET_GRAPHIC_AREA);
    t6963_send_command_uint8_nul(T6963_SET_OFFSET_REGISTER,T6963_EXTERNAL_CG_HOME_ADDRESS>>11);

    t6963_send_command(T6963_OR_MODE|T6963_INTERNAL_CG_ROM_MODE);
    t6963_send_command(T6963_GRAPHIC_OFF|T6963_TEXT_ON|T6963_CURSOR_OFF);
    
    /* Clear display */
    {
	unsigned i;
	t6963_home();
	t6963_set_data_auto_write();
	for (i=0; i<LCD_LINES; i++) t6963_erase_line();
	t6963_auto_reset();
    }

    /* return home */        
    t6963_home();
}
