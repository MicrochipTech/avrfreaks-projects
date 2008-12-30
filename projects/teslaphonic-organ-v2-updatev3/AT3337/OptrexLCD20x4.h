/*  OptrexLCD20x4.h   

Atmel AVR Design Contest 2006 Registration Number AT3337

    Target..: Optrex C-51847NFQJ-LG-ACN using Novatek Driver IC NT3881D
*/
/************************************************************************/
// Definitions
/************************************************************************/
#define LCD_DISPLAY_CLEAR       0x01
#define LCD_DISPLAY_HOME		0X02
#define LCD_DISPLAY_ON          0X0F
#define LCD_DISPLAY_OFF         0X08
#define LCD_CURSOR_OFF          0X0C
#define LCD_DISPLAY_SHIFT_L     0X18
#define LCD_DISPLAY_SHIFT_R     0X1C
#define LCD_CURSOR_SHIFT_L      0X10
#define LCD_CURSOR_SHIFT_R      0X14
#define LCD_FUNCTION_SET        0X38
#define LCD_INSTRUCTION         0x40
#define LCD_INSTRUCTION_NOT     0x00
#define LCD_HOME                0x80
#define LCD_DATA_OUT            0x41
#define LCD_DATA_OUT_NOT        0x01
