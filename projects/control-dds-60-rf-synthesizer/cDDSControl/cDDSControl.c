/*--------------------------------------------------------------------
  cDDSControl.c - Controls DDS-60 with frequency display on LCD and
                  input on keyboard.

  For ATMega8 at 4 MHz.

  The DDS-60 is a PCB kit available from the American QRP Club,
  except for the Analog Devices AD9851 chip, which must be
  obtained separately.

  The frequency in the AD9851 is controlled by sending a 40-bit
  serial word.  The connection requires 3 wires and ground.

  Keypad is 2x8, All Electronics CAT# KP-24.  KP-23 could be
  substituted (4x4 format).  Pin 9 is not used.

  Keypad functions:

  ENTER <up to 8 digits> ENTER - Direct frequency entry in Hz.
      Displays xx,xxx,xxx Hz.
  CLEAR - Cancel function in progress
  HELP n - Retrieve frequency in Memory n, n = 0 to 9
  2ND HELP n - Store current frequency in Memory n
  UP - Increment frequency digit at cursor position
  DOWN - Decrement frequency digit at cursor position
  2ND UP - Move cursor to left one digit
  2ND DOWN - Move cursor to right one digit
  2ND ENTER +xxxx - Enter freq calibration offset.  Must be
                    exactly 5 characters long, first a + or - 
                    and with leading zeros present if needed.

  The LCD is 2x16 Hitachi 44780 type in 4 bit data mode.

  Startup frequency is value stored in Memory 0.

  Mega8 hardware connections:

    PORTD connected to 4x4 keypad as follows:
      PD0 to row 1, pin 1
      PD1 to row 2, pin 2
      PD2 to row 3, pin 3
      PD3 to row 4, pin 4
      PD4 to column 1, pin 5
      PD5 to column 2, pin 6
      PD6 to column 3, pin 7
      PD7 to column 4, pin 8

      Notes:  Internal pullups are used on row pins.
              Pin numbers are for All Electronics CAT #KP-24,
              pin 9 not used.

    PORTC connected to HD4478 LCD display using 4/6 wire 
    connections and simplified timing and pin assignments:
      PC0 to LCD pin 11-D4
      PC1 to LCD pin 12-D5
      PC2 to LCD pin 13-D6
      PC3 to LCD pin 14-D7
      PC4 to LCD pin 6-E
      PC5 to LCD pin 4-R/S

      Note:  LCD pin 5-R/W is held at ground.

    PORTB connected to DDS-60 digital sythesizer:
      PB0 to DDS-60 Load (Pin 1)
      PB1 to DDS-60 Clock (Pin 2)
      PB2 DDS-60 Serial Data (Pin 3)

  Andrew Palm
  2008.11.30
--------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <avr/eeprom.h>

#define clear_bit( reg, bitnum )    ((reg) &= ~(1<<(bitnum)))
#define set_bit( reg, bitnum )  ((reg) |= (1<<(bitnum)))
#define test_bit( reg, bitnum )   ((reg) & (1<<(bitnum)))
#define toggle_bit( reg, bitnum ) ((reg) ^= (1<<(bitnum)))
// For retrieval of single byte in string stored in program memory
#define pgm_read_string_byte(s, i) \
             pgm_read_byte((PGM_P) (pgm_read_word(&(s))+i))

#define let_port_settle _delay_us(1)

#define LCD_PORT PORTC    // Set port for LCD
#define LCD_DDR DDRC
#define E_BIT_NUM  4      // LCD Control Pin numbers
#define RS_BIT_NUM 5

#define KP_PORT PORTD     // Set port for keypad
#define KP_DDR DDRD
#define KP_PIN PIND

#define DDS_PORT PORTB    // Set port for DDS-60
#define DDS_DDR DDRB
#define DDS_LOAD_BIT 0
#define DDS_CLOCK_BIT 1
#define DDS_DATA_BIT 2

#define MAX_CUR_POS 7     // Maximum cursor position
#define MIN_CUR_POS 0     // Minimum cursor position
#define MAX_FREQ 60000000 // Maximum frequency
#define MIN_FREQ 30000    // Minimum frequency
#define MAX_FREQ_DIGITS 8 // Maximum number of frequency digits
#define MAX_CAL_DIGITS 4  // Maximum number of calibrate digits

unsigned char dds_word[5];  // 40-bit word to control AD9851

unsigned long current_freq;       // Current frequency
char new_freq[] = "00000000";     // Buffer for freq entry

long calib_value;                  // Current calibration value
char new_calib_value[] = "+0000";  // String for calib value entry
char new_calib_buffer[] = "+0000";  // Buffer for string formatting
const char new_calib_format[] = "+0000";  // Format for cal string

unsigned char new_digit;    // Buffer for digit entry
unsigned long freq_incr;    // Freq incr/decr for UP/DOWN keys
unsigned char i, c, n, m;   // Utility values
unsigned char cntl_state;   // Main operation state index
unsigned char digit_count;  // Count of digits entered
unsigned char cursor_pos;   // Cursor digit position
unsigned char kp_state;     // Key push state index
unsigned char flag_2nd_key; // Flag indicates 2ND key pushed
unsigned char cntl_state_7_flag;  // Controls execution of steps
                                  // in main state machine state 7
unsigned char cntl_state_1_flag;  // Controls state 1 execution
unsigned char enter_push;  // Counts pushes of ENTER key

char *start;                // Used for ltoa calls

const unsigned char KEY_TABLE[] =  // Table of kp_state values
//           Label   Col   Row   Index
{ 0x01,   //   1      1     1     0x0
  0x02,   //   2      2     1     0x1
  0x03,   //   3      3     1     0x2
  0xA0,   //  UP      4     1     0x3
  0x04,   //   4      1     2     0x4
  0x05,   //   5      2     2     0x5
  0x06,   //   6      3     2     0x6
  0xA1,   // DOWN     4     2     0x7
  0x07,   //   7      1     3     0x8
  0x08,   //   8      2     3     0x9
  0x09,   //   9      3     3     0xA
  0xA2,   //  2ND     4     3     0xB
  0xA3,   // CLEAR    1     4     0xC
  0x00,   //   0      2     4     0xD
  0xA4,   // HELP     3     4     0xE
  0xA5 }; // ENTER    4     4     0xF

// Display strings for keypad entries
char enter_freq[] PROGMEM = "********";
PGM_P enter_freq_ptr PROGMEM = enter_freq;
char put_mem_msg[] PROGMEM = "  Put Memory ";
PGM_P put_mem_msg_ptr PROGMEM = put_mem_msg;
char get_mem_msg[] PROGMEM = "  Get Memory ";
PGM_P get_mem_msg_ptr PROGMEM = get_mem_msg;
char set_sign_msg[] PROGMEM = " UP=+, DOWN=-";
PGM_P set_sign_msg_ptr PROGMEM = set_sign_msg;
char cal_header[] PROGMEM = "  Cal: ";
PGM_P cal_header_ptr PROGMEM = cal_header;
char freq_Hz[] PROGMEM = " Hz ";
PGM_P freq_Hz_ptr PROGMEM = freq_Hz;
char freq_header[] PROGMEM = " Freq: ";
PGM_P freq_header_ptr PROGMEM = freq_header;


// Eeprom frequency memories
unsigned long ee_mem0 EEMEM = 10000000UL;
unsigned long ee_mem1 EEMEM =    30000UL;
unsigned long ee_mem2 EEMEM =   455000UL;
unsigned long ee_mem3 EEMEM =  1800000UL;
unsigned long ee_mem4 EEMEM =  3500000UL;
unsigned long ee_mem5 EEMEM =  7000000UL;
unsigned long ee_mem6 EEMEM = 14000000UL;
unsigned long ee_mem7 EEMEM = 21000000UL;
unsigned long ee_mem8 EEMEM = 28000000UL;
unsigned long ee_mem9 EEMEM = 54000000UL;

const unsigned long* ee_mem[] =
{ &ee_mem0,
  &ee_mem1,
  &ee_mem2,
  &ee_mem3,
  &ee_mem4,
  &ee_mem5,
  &ee_mem6,
  &ee_mem7,
  &ee_mem8,
  &ee_mem9 };

// Eeprom storage for DDS-60 word calibration
long ee_calib_value EEMEM = 252L;

//--------------------------------------------------------------------
static inline void toggle_e()
{
  set_bit(LCD_PORT, E_BIT_NUM);    // Toggle the E pin on LCD
  #if F_CPU > 4000000UL
    _delay_us(0.5);
  #endif
  clear_bit(LCD_PORT, E_BIT_NUM);
}

//--------------------------------------------------------------------
static void lcd_putchar(unsigned char lcd_data, unsigned char rs_value)
// Write data and RS value to LCD
// LCD is in 4 bit mode, 6 wire setup as per Exp. 46, Evil Genius
{
  LCD_PORT = (lcd_data >> 4) & 0x0F;  // Get high 4 bits for output
  LCD_PORT = LCD_PORT | ((rs_value & 1)*(1<<RS_BIT_NUM));
                                       // and set RS value
  toggle_e();                         // Toggle the high 4 bits out
  
  LCD_PORT = lcd_data & 0x0F;     // Get low 4 bits for output
  LCD_PORT = LCD_PORT | ((rs_value & 1)*(1<<RS_BIT_NUM));
                                  // and set RS value
  toggle_e();                     // Toggle the low 4 bits out
  
  if ((0 == (lcd_data & 0xFC)) && (0 == rs_value))
    _delay_ms(5);             // Set delay interval larger for
  else                        // clear display and return home
    _delay_us(200);
}

//--------------------------------------------------------------------
static void lcd_init()
// Set LCD for 4 bit mode with 6 wire setup
{
  _delay_ms(20);            // Wait for LCD to power up

  LCD_PORT = 3;             // Start initialization process
  toggle_e();               // Send reset command
  _delay_ms(5);

  toggle_e();               // Repeat reset command
  _delay_us(200);           // but wait 200 us
  toggle_e();               // Repeat
  _delay_us(200);

  LCD_PORT = 2;             // Put LCD into 4 bit mode
  toggle_e();
  _delay_us(200);

  lcd_putchar(0b00101000, 0);  // LCD is 4 bit I/F, 2 line
  lcd_putchar(0b00000001, 0);  // Clear LCD 
  lcd_putchar(0b00000110, 0);  // Move cursor after each character
  lcd_putchar(0b00001110, 0);  // Turn on LCD and enable cursor 
//lcd_putchar(0b00001100, 0);  // Turn on LCD with disabled cursor
}
//--------------------------------------------------------------------
static void lcd_clear_line_2()
// Clear second line of LCD display
{
  unsigned char i;

  lcd_putchar(0b11000000, 0);   // Clear Line 2
  for (i=0; i<16; i++)
    lcd_putchar(32, 1);
}

//--------------------------------------------------------------------
static void lcd_clear_line_1()
// Clear first line of LCD display
{
  unsigned char i;

  lcd_putchar(0b10000000, 0);   // Clear Line 1
  for (i=0; i<16; i++)
    lcd_putchar(32, 1);
}

//--------------------------------------------------------------------
static void place_cursor(unsigned char cp)
// Put cursor below freq digit where UP and DOWN keys are active
//   cp = 0 is 1 Hz digit
//   cp = 7 is 10 MHz digit
// Address adjusted for display format " xx,xxx,xxx Hz"
{
  if (cp > MAX_CUR_POS) cp = MAX_CUR_POS;
  cp = (MAX_CUR_POS - cp) + 1;    // Cursor address ones digit
  if (cp > 2) cp++;               // Shift up 1 if below 1 MHz
  if (cp > 6) cp++;               // Shift up 1 if below 1 KHz
  cp = cp + 0x80;                 // Add high bit for LCD command
  lcd_putchar(cp, 0);
}

//--------------------------------------------------------------------
static void place_enter_cursor(unsigned char cp)
// Place cursor below calibrate value digit
{
  cp = cp + 7;             // Shift over for header
  cp = cp + 0x80;          // Add high bit for LCD command
  lcd_putchar(cp, 0);
}

//--------------------------------------------------------------------
static void display_freq(unsigned long freq)
// Display frequency on LCD in format " xx,xxx,xxx Hz"
{
  char freq_buffer[] = "00000000";
  unsigned int i, j, n, flag;

  start = ltoa(freq, freq_buffer, 10);  // Convert to string
  n = strlen(freq_buffer);
  if (n < MAX_FREQ_DIGITS)              // Right-justify string
  {
    for (i=0; i<n; i++)
      freq_buffer[MAX_FREQ_DIGITS-i-1] = freq_buffer[n-i-1];
    for (i=0; i<MAX_FREQ_DIGITS-n; i++)
      freq_buffer[i] = ' ';
  }
  
  lcd_putchar(0b10000000, 0);     // Start line 1
    
  lcd_putchar(32, 1);             // Print leading blank

  flag = 0;                       // Stays zero until first digit
  i = 0;                          // Index for freq digits
  for (j=0; j<MAX_FREQ_DIGITS+2; j++)
  {
    if (((j==2)||(j==6)) && flag)
      lcd_putchar(',', 1);        // Print comma
    else
    {
      if ((j==2)||(j==6))
        lcd_putchar(32, 1);
      else
      {
        if (freq_buffer[i] != ' ') 
          flag = 1;
        lcd_putchar(freq_buffer[i], 1);   // Print digit
        i++;
      }
    }
  }
  for(i=0;; i++)
  {
    c = pgm_read_string_byte(freq_Hz_ptr, i) ;
    if (c!=0) lcd_putchar(c, 1);
    else break;
  }
}

//--------------------------------------------------------------------
static unsigned char get_key_push()
// Return index of key on 4x4 keypad if pushed, otherwise return 0x10.
// Index is 4*(rownum - 1) + colnum
{
  unsigned char i, j, x, index;
  
  index = 0x10;         // Value if no key pushed or error

// Check for key push with debounce
  KP_PORT = 0x0F;       // Set all col lines low (upper nybble)
  let_port_settle;      // Wait for port line to settle
  if ((KP_PIN | 0xF0) == 0xFF) return index;  // No key pushed
  _delay_ms(20);        // Debounce delay
  if ((KP_PIN | 0xF0) == 0xFF) return index;  // Key no longer pushed

// Key still down after 20 ms.  Find row and column of pushed key.
  for (j=0; j<4; j++)
  {
    KP_PORT = 0xFF & ~(1<<(j+4)); // Clear col j+1 line bit
    let_port_settle;
    x = KP_PIN;
    if ((x | 0xF0) != 0xFF)  // A key in this col is down
    {
      for (i=0; i<4; i++)    // Find row of pushed key
      {
        if (!test_bit(x, i)) 
        {
          index = (4*i)+j;  // Row found, calculate index
          break;
        }
      }
      break;
    }
  }
// Debounce key release
  do
  {
    KP_PORT = 0x0F;       // Set all column lines low (upper nybble)
    let_port_settle;      // Wait for port line to settle
    while ((KP_PIN | 0xF0) != 0xFF) ; // Loop until release detected
    _delay_ms(20);        // Debounce delay
  } while ((KP_PIN | 0xF0) != 0xFF) ; // Redo if release not sustained

  return index;
}

//--------------------------------------------------------------------
static void calc_dds_word()
// Calculate AD9851 frequency control word.
//
// Control word is contained in 4 bytes dds_word[0-3] plus a
// fifth control byte dds_word[4] which is always 1.
// Formula for AD9851 is
//
//    Control word = f(MHz) x (2^32) / 180
//
// where 180 is the reference frequency, 6 x ref clock.
// 
// current_freq is the frequency in Hz, an unsigned long integer
//
// The calib_value value is used to adjust the freq word for
// calibration.  The correction frequency is determined by the
// formula
//          corr_freq = current_freq x (calib_value / 10000000)
//
//                    = current_freq / e
// where
//                  e = 10000000 / calib_value
//
// The load frequency is thus
//
//          load_freq = current_freq x (1 + (calib_value/10000000))
//
// Thus this is a correction for a constant relative error.
// The formulas above are used to allow for long integer arithmetic.
//
// Under the assumption of constant relative error, calib_value is
// 10 MHz minus the measured frequency in Hz when 10 MHz is on
// the display and the initial value of calib_value is set to zero.
//
{
  #define DENOM 0xABA95     // (180 x 10^6)/256 where DDS-60 ref
                            // freq is 180 MHz

  unsigned long r, load_freq;
  unsigned long e;          // Intermediate value to allow long
                            // integer arithmetic
  unsigned long corr_freq;  // Freq correction value

  if (calib_value == 0)
    corr_freq = 0;
  else
  {
    e = 10000000 / calib_value;
    corr_freq = current_freq / e;
  }
  
  load_freq = current_freq + corr_freq;

  dds_word[3] = (unsigned char) (load_freq / DENOM);
  r = load_freq % DENOM;
  r = r * 0x100;

  dds_word[2] = (unsigned char) (r / DENOM);
  r = r % DENOM;
  r = r * 0x100;

  dds_word[1] = (unsigned char) (r / DENOM);
  r = r % DENOM;
  r = r * 0x100;

  dds_word[0] = (unsigned char) (r / DENOM);
  r = r % DENOM;
  r = r * 0x10;

  r = r / DENOM;  // Check to see if rounding up needed
  if (((unsigned char) r) > 0x7)
  {
    dds_word[0]++;
    if (dds_word[0] == 0)
    {
      dds_word[1]++;
      if (dds_word[1] == 0)
      {
        dds_word[2]++;
        if (dds_word[2]++ == 0)
          dds_word[3]++;
      }
    }
  }

  dds_word[4] = 0x01;   // Control word

}

//--------------------------------------------------------------------
static void load_dds_word()
// Serially load 40-bit frequency control word into AD9851 DDS chip
{
  unsigned char i, j;

  clear_bit(DDS_PORT, DDS_CLOCK_BIT);
  clear_bit(DDS_PORT, DDS_LOAD_BIT);

  for (i=0; i<5; i++)
  {
    for (j=0; j<8; j++)
    {
      clear_bit(DDS_PORT, DDS_DATA_BIT);
      if (test_bit(dds_word[i], j))
        set_bit(DDS_PORT, DDS_DATA_BIT);
      set_bit(DDS_PORT, DDS_CLOCK_BIT);     // Toggle in bit
      #if F_CPU > 4000000UL
        _delay_us(0.5);
      #endif
      clear_bit(DDS_PORT, DDS_CLOCK_BIT);   // on rising edge
    }
  }

  set_bit(DDS_PORT, DDS_LOAD_BIT);  // Toggle in word on rising edge
  #if F_CPU > 4000000UL
    _delay_us(0.5);
  #endif
  clear_bit(DDS_PORT, DDS_LOAD_BIT);

}

//--------------------------------------------------------------------
void main(void)
{
// Initialize LCD 
  LCD_DDR = 0xFF;
  lcd_init();

// Set up keypad port
  KP_PORT = 0x0F;       // Enable pullups on lower nybble
                        // Set column lines low
  KP_DDR = 0xF0;        // Lower nybble row inputs
                        // Upper nybble column outputs

// Set up output port to dds
  DDS_DDR = 0x07 ;      // Lowest three bits are output

// Retrieve startup frequency from Memory 0
  current_freq = eeprom_read_dword(ee_mem[0]);
// Retrieve freq calibration adjustment from eeprom
  calib_value = eeprom_read_dword(&ee_calib_value);

  calc_dds_word();  // calc freq word for AD9851
  load_dds_word();  // Load freq word
  load_dds_word();  // Load freq word again (needed for startup)

  display_freq(current_freq);
  cursor_pos = 3;           // Initial cursor position
  freq_incr = 1000;         // Freq incr to match cursor position
  place_cursor(cursor_pos); // Put cursor under 1 MHz pos on display

  cntl_state = 0;       // Initialize main state machine
  flag_2nd_key = 0;     // Flag indicates 2ND key pushed
  cntl_state_7_flag = 0;  // Controls execution of main state 7
  cntl_state_1_flag = 0;  // Controls execution of main state 1
  enter_push = 0;       // Counts pushes of ENTER key

  while(1)    // Main loop
  { 
// Check for key push.  If pushed and valid, get key push state index
// and enter key push state machine.
    i = get_key_push();
    if ((i >= 0) && (i < 0x10))     // Valid key push
    {
      kp_state = KEY_TABLE[i];
      switch (kp_state)             // Key push state machine
      {
        case 0xA0:    // UP arrow
          switch (cntl_state)
          {
            case 0:
              if (flag_2nd_key)   // Shift cursor to left
              {
                flag_2nd_key = 0;
                if (cursor_pos < MAX_CUR_POS)
                {
                  cursor_pos++;
                  freq_incr *= 10;
                }
                lcd_putchar(0b11001111, 0);   // Last pos., Line 2
                lcd_putchar(32, 1);           // Erase 2ND symbol
                place_cursor(cursor_pos);
              }
              else                // Incr freq digit
                cntl_state = 5;
              break;
            case 7:
              new_digit = '+';
              digit_count = 1;
              cntl_state_7_flag = 1;
              break;
            default:
              break;
          }
          break;
        case 0xA1:    // DOWN arrow
          switch (cntl_state)
          {
            case 0:
              if (flag_2nd_key)   // Shift cursor to right
              {
                flag_2nd_key = 0;
                if (cursor_pos > MIN_CUR_POS)
                {
                  cursor_pos--;
                  freq_incr /= 10;
                }
                lcd_putchar(0b11001111, 0);   // Last pos., Line 2
                lcd_putchar(32, 1);           // Erase 2ND symbol
                place_cursor(cursor_pos);
              }
              else                // Decr freq digit
                cntl_state = 6;
              break;
            case 7:
              new_digit = '-';
              digit_count = 1;
              cntl_state_7_flag = 1;
              break;
            default:
              break;
          }
          break;
        case 0xA2:    // 2ND key
          switch (cntl_state)
          {
            case 0:
              flag_2nd_key = 1;
              lcd_putchar(0b11001111, 0);  // Last pos., Line 2
              lcd_putchar('*', 1);          // Write 2ND active symbol
              place_cursor(cursor_pos);
              break;
            default:
              break;
          }
          break;
        case 0xA3:    // CLEAR - Cancel operations
          cntl_state = 2;      // Restore normal display
          digit_count = 0;
          flag_2nd_key = 0;
          cntl_state_7_flag = 0;
          cntl_state_1_flag = 0;
          enter_push = 0;
          break;
        case 0xA4:    // HELP - Get/put freq in Memory n
          switch (cntl_state)
          {
            case 0:
              if (flag_2nd_key)   // Store in memory
              {
                cntl_state = 3;
                flag_2nd_key = 0;
                lcd_putchar(0b11001111, 0);   // Last pos., Line 2
                lcd_putchar(32, 1);           // Erase 2ND symbol
                place_cursor(cursor_pos);
              }
              else
              {
                cntl_state = 4;
              }
              digit_count = 0;
              break;
            default:
              break;
          }
          break;
        case 0xA5:    // ENTER - Direct freq entry/calibrate value
          switch (cntl_state)
          {
            case 0:
              if (flag_2nd_key)   // Initiate entry of calibrate value
              {
                cntl_state = 7;
                digit_count = 0;
                cntl_state_7_flag = 1;
                flag_2nd_key = 0;
                lcd_putchar(0b11001111, 0);   // Last pos., Line 2
                lcd_putchar(32, 1);           // Erase 2ND symbol
              }
              else      // Start frequency entry
              {
                enter_push++;        // Count pushes of ENTER
                cntl_state = 1;
                digit_count = 0;
                cntl_state_1_flag = 1;      
              }
              break;
            case 1:     // Freq entry complete
              enter_push++;
              cntl_state_1_flag = 1;
              break;
            default:
              break;
          }
          break;
        default:      // Digits 0 to 9
          switch (cntl_state)
          {
            case 1:
              new_digit = kp_state;
              digit_count++;
              cntl_state_1_flag = 1;
              break;
            case 3:
              new_digit = kp_state;
              digit_count = 1;
              break;
            case 4:
              new_digit = kp_state;
              digit_count = 1;
              break;
            case 7:
              if (digit_count > 0)
              {
                new_digit = kp_state;
                digit_count++;
              }
              cntl_state_7_flag = 1;
              break;
            default:
              break;
          }
      }
    }

// Perform operations according to variables set in key push states
    switch (cntl_state)   // Main operations state machine
    {
      case 0:       // Do nothing
        break;
      case 1:       // Read in new freq digits
        if (cntl_state_1_flag)
        {
          if (enter_push == 1)    // Do frequency entry
          {
            if (digit_count == 0)  // Initialize entry
            {
              lcd_clear_line_1();
              lcd_putchar(0b10000000, 0);   // Go to start of Line 1
              for (i=0;; i++)                // Display header
              {
                c = pgm_read_string_byte(freq_header_ptr, i) ;
                if (c!=0) lcd_putchar(c, 1);
                else break;
              }
              for (i=0; i<MAX_FREQ_DIGITS; i++) // Clear string buffer
                new_freq[i] = 0;
              place_enter_cursor(digit_count);
            }
            else if ((digit_count > 0) && (digit_count <= MAX_FREQ_DIGITS))
            // Load entered digit into buffer and display
            {
              new_freq[digit_count - 1] = new_digit + '0';
              lcd_putchar(new_digit + '0', 1);
            }
          }
          if (enter_push == 2)    // Complete freq entry
          {
            current_freq = atol(&new_freq);
            if (current_freq < MIN_FREQ)
              current_freq = MIN_FREQ;
            if (current_freq > MAX_FREQ)
              current_freq = MAX_FREQ;
            cntl_state = 2;
            enter_push = 0;
            digit_count = 0;
          }
        }
        cntl_state_1_flag = 0;  // Don't act again until next push
        break;
      case 2:       // Load DDS and show normal freq display
        calc_dds_word();
        load_dds_word();
        display_freq(current_freq);
        lcd_clear_line_2();
        place_cursor(cursor_pos);
        cntl_state = 0;
        break;
      case 3:       // Store frequency
        lcd_putchar(0b11000000, 0);   // Start of Line 2
        for(i=0;; i++)                // Display store msg
        {
          c = pgm_read_string_byte(put_mem_msg_ptr, i) ;
          if (c!=0) lcd_putchar(c, 1);
          else break;
        }
        if (digit_count == 1)
        {
          cntl_state = 0;
          digit_count = 0;
          lcd_putchar(new_digit + '0', 1);    // Display mem num
          _delay_ms(1000);
          eeprom_busy_wait();
          eeprom_write_dword(ee_mem[new_digit], current_freq);
                  // Write current freq to selected memory
          lcd_clear_line_2();
          place_cursor(cursor_pos);
        }
        break;
      case 4:       // Retrieve frequency
        lcd_putchar(0b11000000, 0);   // Start of Line 2
        for(i=0;; i++)                // Display retrieve msg
        {
          c = pgm_read_string_byte(get_mem_msg_ptr, i) ;
          if (c!=0) lcd_putchar(c, 1);
          else break;
        }
        if (digit_count == 1)
        {
          digit_count = 0;
          lcd_putchar(new_digit + '0', 1);    // Display mem num
          _delay_ms(1000);
          current_freq = eeprom_read_dword(ee_mem[new_digit]);
                  // Retrieve freq from selected memory
          cntl_state = 2;
        }
        break;
      case 5:       // Increment frequency
        if (current_freq <= (MAX_FREQ - freq_incr))
          current_freq += freq_incr;
        cntl_state = 2;
        break;
      case 6:       // Decrement frequency
        if (current_freq >= (MIN_FREQ + freq_incr))
          current_freq -= freq_incr;
        cntl_state = 2;
        break;
      case 7:       // Enter calibration value
        if (cntl_state_7_flag)  // Only execute if key push done
        {
          if (digit_count == 0)   // Start entry process
          {
            // Get current calibrate value for display
            start = ltoa(calib_value, new_calib_buffer, 10);
            n = strlen(new_calib_buffer);
            for (i=0; i<MAX_CAL_DIGITS+1; i++)
              new_calib_value[i] = new_calib_format[i];
            m = 0;
            if (new_calib_buffer[0] == '-')
              m = 1;
            for (i=n; i>m; i--)
              new_calib_value[MAX_CAL_DIGITS+i-n] = new_calib_buffer[i-1];
            if (calib_value >= 0)  
              new_calib_value[0] = '+';
            else
              new_calib_value[0] = '-';

          // Display set sign message
            lcd_putchar(0b11000000, 0);
            for(i=0;; i++) 
           {
             c = pgm_read_string_byte(set_sign_msg_ptr, i) ;
             if (c!=0) lcd_putchar(c, 1);
             else break;
           }
         }          

          if (digit_count == 1)   // Set value sign
         {
            if ((new_digit == '+') || (new_digit == '-'))
            {
              new_calib_value[digit_count - 1] = new_digit;
              lcd_clear_line_2();
            }
            else        // Try again
              digit_count = 0;
          }
          
          if ((digit_count > 1) && (digit_count < 6))  // Add digit
            new_calib_value[digit_count - 1] = new_digit + '0' ;

          // Display calibrate value so far
          lcd_putchar(0b10000000, 0);   // Go to start of Line 1
          for(i=0;; i++)                // Display header
          {
            c = pgm_read_string_byte(cal_header_ptr, i) ;
            if (c!=0) lcd_putchar(c, 1);
            else break;
          }
          for(i=0; i<5; i++)   // Display calibrate value
            lcd_putchar(new_calib_value[i], 1);
          for(i=0;; i++)                // Display Hz
          {
            c = pgm_read_string_byte(freq_Hz_ptr, i) ;
            if (c!=0) lcd_putchar(c, 1);
            else break;
          }
          if (digit_count < 5)
            place_enter_cursor(digit_count);

          if (digit_count == 5)   // Complete calibrate change
          {
            _delay_ms(1000);
            digit_count = 0;
            calib_value = atol(&new_calib_value);
            eeprom_busy_wait();
            eeprom_write_dword(&ee_calib_value, calib_value);
                    // Write calibrate freq to eeprom
            cntl_state = 2;
          }
        }
        cntl_state_7_flag = 0;  // Don't act until next key push
        break;
      default:      // Do nothing
        break;
    }

  }

}

 
