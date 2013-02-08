/*
 * hd44780.h
 *
 * Created: 12/24/2012 2:31:38 PM
 *  Author: sasquach
 */ 


#ifndef HD44780_H_
#define HD44780_H_

#define LCD_PORT         PORTD     /**< port for the LCD lines   */
#define LCD_DDR          DDRD		  /**< DDR for LCD              */
#define LCD_DATA_NIB     0            /** data nibble whether lower 0 or upper 4 */
#define LCD_DATA0_PIN    0 + LCD_DATA_NIB            /**< pin for 4bit data bit 0  */
#define LCD_DATA1_PIN    1 + LCD_DATA_NIB           /**< pin for 4bit data bit 1  */
#define LCD_DATA2_PIN    2 + LCD_DATA_NIB           /**< pin for 4bit data bit 2  */
#define LCD_DATA3_PIN    3 + LCD_DATA_NIB           /**< pin for 4bit data bit 3  */
#define LCD_DATA_MASK  _BV(LCD_DATA0_PIN) | _BV (LCD_DATA1_PIN) | _BV (LCD_DATA2_PIN) | _BV (LCD_DATA3_PIN)
#define LCD_RS_PIN       6            /**< pin  for RS line         */
#define LCD_E_PIN        4            /**< pin  for Enable line     */
#define LCD_LED_PIN      7            /**< pin  for RS line         */
#define LCD_LIGHT_SENCE  0xc0
#define LCD_SHORT_DELAY_50us  200     /** delay cycle is 4 cpu cycles */

#define LCD_CLR 0x01				// Clear display
#define LCD_HOME 0x02            			// Put cursor to home position
#define LCD_MODE 0x04            			// Set display mode
#define LCD_SHOW 0x08            			// Display show options
#define LCD_SHFT 0x10            			// shift cursor/display
#define LCD_FUNC 0x20            			// LCD function register (init only)
#define LCD_CGAD 0x40            			// Set cg ram address
#define LCD_DDAD 0x80            			// Set display ram address
#define LCD_DDAD_LINE1 0x00                         // display ram address line 1
#define LCD_DDAD_LINE2 0x40                         // display ram address line 2
#define LCD_DDAD_LINE3 0x14                         // display ram address line 3
#define LCD_DDAD_LINE4 0x54                         // display ram address line 4
// LCD_MODE options
#define LCD_MODE_RIGHT 0x02            		// Increment cusor position (not decrement)
#define LCD_MODE_SHIFT 0x01            		// Shift display (not shift cursor only)

// LCD_SHOW options
#define LCD_SHOW_ON 0x04            		// Show display
#define LCD_SHOW_CUR 0x02            		// Show cursor
#define LCD_SHOW_BLINK 0x01            		// Blink cursor position character

// LCD_SHIFT options
#define LCD_SHIFT_RIGHT 0x04            		// Shift cursor/display to right
#define LCD_SHIFT_SHIFT 0x08            		// Shift display (not move cursor)
// (not to left)
// LCD_FUNC options
#define LCD_FUNC_8BIT 0x10            		// 8 bit interface (not 4 bit)
#define LCD_FUNC_2LN 0x08            		// 2 line display (not 1 line)
#define LCD_FUNC_5X10 0x04            		// 5X10 font (not 5X7 font)

// status read
#define LCD_BUSY 0x80            			// busy flag
#define LCD_ADDR_MASK 0x7f            		// address mask

// LCD reset command (sent in 8 bit mode)
#define LCD_FUNC_RESET (LCD_FUNC | LCD_FUNC_8BIT) >> 4

// LCD init command (sent in 4 bit mode)
#define LCD_FUNC_INIT LCD_FUNC | LCD_FUNC_2LN
#define LCD_FUNC_INIT_4bit  0x02 //(LCD_FUNC<<4) | LCD_FUNC




#endif /* HD44780_H_ */

#define LCD_PORT         PORTD        /**< port for the LCD lines   */
#define LCD_DDR          DDRD		  /**< DDR for LCD              */
#define LCD_DATA_NIB     0            /** data nibble whether lower 0 or upper 4 */
#define LCD_DATA0_PIN    0 + LCD_DATA_NIB            /**< pin for 4bit data bit 0  */
#define LCD_DATA1_PIN    1 + LCD_DATA_NIB           /**< pin for 4bit data bit 1  */
#define LCD_DATA2_PIN    2 + LCD_DATA_NIB           /**< pin for 4bit data bit 2  */
#define LCD_DATA3_PIN    3 + LCD_DATA_NIB           /**< pin for 4bit data bit 3  */
#define LCD_DATA_MASK  _BV(LCD_DATA0_PIN) | _BV (LCD_DATA1_PIN) | _BV (LCD_DATA2_PIN) | _BV (LCD_DATA3_PIN)
#define LCD_RS_PORT      LCD_PORT     /**< port for RS line         */
#define LCD_RS_PIN       6            /**< pin  for RS line         */
#define LCD_E_PORT       LCD_PORT     /**< port for Enable line     */
#define LCD_E_PIN        4            /**< pin  for Enable line     */
#define LCD_LED_PORT     LCD_PORT
#define LCD_LED_PIN      7            /**< pin  for RS line         */
#define LCD_LIGHT_SENCE  0xc0
#define LCD_SHORT_DELAY_50us  200     /** delay cycle is 4 cpu cycles */

#define LCD_CLR 0x01				// Clear display
#define LCD_HOME 0x02            			// Put cursor to home position
#define LCD_MODE 0x04            			// Set display mode
#define LCD_SHOW 0x08            			// Display show options
#define LCD_SHFT 0x10            			// shift cursor/display
#define LCD_FUNC 0x20            			// LCD function register (init only)
#define LCD_CGAD 0x40            			// Set cg ram address
#define LCD_DDAD 0x80            			// Set display ram address
#define LCD_DDAD_LINE1 0x00                         // display ram address line 1
#define LCD_DDAD_LINE2 0x40                         // display ram address line 2
#define LCD_DDAD_LINE3 0x14                         // display ram address line 3
#define LCD_DDAD_LINE4 0x54                         // display ram address line 4
// LCD_MODE options
#define LCD_MODE_RIGHT 0x02            		// Increment cusor position (not decrement)
#define LCD_MODE_SHIFT 0x01            		// Shift display (not shift cursor only)

// LCD_SHOW options
#define LCD_SHOW_ON 0x04            		// Show display
#define LCD_SHOW_CUR 0x02            		// Show cursor
#define LCD_SHOW_BLINK 0x01            		// Blink cursor position character

// LCD_SHIFT options
#define LCD_SHIFT_RIGHT 0x04            		// Shift cursor/display to right
#define LCD_SHIFT_SHIFT 0x08            		// Shift display (not move cursor)
// (not to left)
// LCD_FUNC options
#define LCD_FUNC_8BIT 0x10            		// 8 bit interface (not 4 bit)
#define LCD_FUNC_2LN 0x08            		// 2 line display (not 1 line)
#define LCD_FUNC_5X10 0x04            		// 5X10 font (not 5X7 font)

// status read
#define LCD_BUSY 0x80            			// busy flag
#define LCD_ADDR_MASK 0x7f            		// address mask

// LCD reset command (sent in 8 bit mode)
#define LCD_FUNC_RESET (LCD_FUNC | LCD_FUNC_8BIT) >> 4

// LCD init command (sent in 4 bit mode)
#define LCD_FUNC_INIT LCD_FUNC | LCD_FUNC_2LN
#define LCD_FUNC_INIT_4bit  0x02 //(LCD_FUNC<<4) | LCD_FUNC

#define LCD_MODE_NORMAL 0
#define LCD_MODE_DIAG 1

extern uint8_t lcd_display_mode;

void lcd_task(void);

void lcd_init (void);