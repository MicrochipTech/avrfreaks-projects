/* Includes */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

/* Prototypes */
void init(void);
void DisplayValue(uint8_t bpm, uint8_t quads);
void LedUpdate(uint8_t quad_rythm);
void SwitchPressed(void);
void CalculateBpmValue(uint8_t bpm_value);
void Tone(uint8_t tone_value, uint8_t tone_length_value);

/* Defines */
#define FALSE           0
#define TRUE            1

#define TONE_LOW        20
#define TONE_MID        100
#define TONE_HIGH       180
#define TONE_SHORT      50
#define TONE_MED        100
#define TONE_LONG       150

#define SEGMENT_DATA    PORTD
#define SEGMENT_LCD     PORTB
#define LED_PORT        PORTC

#define LCD_X100        PB5
#define LCD_X10         PB4
#define LCD_X1          PB1
#define LCD_BPM         PB2

#define SEGMENT_X100    SEGMENT_LCD &= ~(1 << LCD_X100);                                \
                        SEGMENT_LCD |= (1 << LCD_X10) | (1 << LCD_X1) | (1 << LCD_BPM)

#define SEGMENT_X10     SEGMENT_LCD &= ~(1 << LCD_X10);                                 \
                        SEGMENT_LCD |= (1 << LCD_X100) | (1 << LCD_X1) | (1 << LCD_BPM)

#define SEGMENT_X1      SEGMENT_LCD &= ~(1 << LCD_X1);                                  \
                        SEGMENT_LCD |= (1 << LCD_X100) | (1 << LCD_X10) | (1 << LCD_BPM)

#define SEGMENT_BPM     SEGMENT_LCD &= ~(1 << LCD_BPM);                                 \
                        SEGMENT_LCD |= (1 << LCD_X100) | (1 << LCD_X10) | (1 << LCD_X1)

#define LED_RED         LED_PORT &= ~(1 << PC2);                                        \
                        LED_PORT |= (1 << PC3) | (1 << PC4) | (1 << PC5) 

#define LED_GREEN       LED_PORT &= ~(1 << PC3);                                        \
                        LED_PORT |= (1 << PC2) | (1 << PC4) | (1 << PC5)

#define LED_BLUE        LED_PORT &= ~(1 << PC4);                                        \
                        LED_PORT |= (1 << PC2) | (1 << PC3) | (1 << PC5)

#define LED_YELLOW      LED_PORT &= ~(1 << PC5);                                        \
                        LED_PORT |= (1 << PC2) | (1 << PC3) | (1 << PC4)

//speaker PB3

#define SW_BPM_PLUS     (PINB & _BV(PB0))
#define SW_BPM_MINUS    (PINC & _BV(PC1))
#define SW_QUARTERS     (PINC & _BV(PB0)) 

enum
{
  X100,
  X10,
  X1,
  BPM  
} lcd_ciffer;

volatile enum
{
   SWITCH_READY,
   SWITCH_PRESSED = 5
} timer_flag;

/* Global variables */
volatile uint8_t metronome_flag = FALSE;
volatile uint8_t tone_length = 0;
volatile uint16_t bpm_timer_value = 0;
volatile uint8_t bpm_value = 120;
volatile uint8_t quad_rythm = 4;
uint8_t EEMEM ee_bpm_value = 120;
uint8_t EEMEM ee_quad_rythm = 4;

/* 7-segment table made with following data
    dot 0
    a   1
    b   2
    c   3 
    d   4
    e   5
    f   6
    g   7
*/
uint8_t segment_table[11] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x0E, 0xFE, 0xDE, 0x01};


/* Timer/Counter0 Overflow interrupt (switch delay) */
ISR (TIMER0_OVF_vect)
{
  if(SWITCH_READY != timer_flag)
  {
    timer_flag--;
  }
  else
  {
    /* Stop timer */
    TCCR0 = 0x00;
  }  
}   
//-----------------------------------------------------------------------------

/* Timer/Counter1 Overflow interrupt (metronom effect) */
ISR (TIMER1_OVF_vect)
{
  metronome_flag = TRUE;
  TCNT1 = bpm_timer_value;
}
//-----------------------------------------------------------------------------

/* Timer/Counter2 Overflow interrupt (audio signal) */
ISR (TIMER2_OVF_vect)
{
  tone_length--;
  
  if(0 == tone_length)
  {
    /* Stop tone generator */
    TCCR2 = 0x00;
  }
}
//-----------------------------------------------------------------------------

/* Local functions */
int main(void)
{  
  bpm_value = eeprom_read_byte(&ee_bpm_value);
  quad_rythm = eeprom_read_byte(&ee_quad_rythm);
  init();
  Tone(TONE_HIGH, TONE_LONG);
  _delay_ms(100);
  Tone(TONE_MID, TONE_MED);
  _delay_ms(100);
  Tone(TONE_LOW, TONE_SHORT);
  CalculateBpmValue(bpm_value);

  while(1)
  {
    DisplayValue(bpm_value, quad_rythm);
    
    if(TRUE == metronome_flag)
    {
      LedUpdate(quad_rythm);
      metronome_flag = FALSE;
    }    
    
    if(0 == SW_QUARTERS && SWITCH_READY == timer_flag)
    {
      if(3 == quad_rythm)
      {
        SwitchPressed();
        quad_rythm = 4;
        eeprom_write_byte(&ee_quad_rythm, quad_rythm);
      }
      else
      {
        SwitchPressed();
        quad_rythm = 3;
        eeprom_write_byte(&ee_quad_rythm, quad_rythm);
      }     
    }

    if(0 == SW_BPM_PLUS && SWITCH_READY == timer_flag)
    {
      if(249 >= bpm_value)
      {
        SwitchPressed();
        CalculateBpmValue(bpm_value);
        bpm_value++;
        eeprom_write_byte(&ee_bpm_value, bpm_value);
      }     
    }
    else if(0 == SW_BPM_MINUS && SWITCH_READY == timer_flag)
    {
      if(51 <= bpm_value)
      {
        SwitchPressed();
        CalculateBpmValue(bpm_value);
        bpm_value--;
        eeprom_write_byte(&ee_bpm_value, bpm_value);
      }      
    }
  }
}
//-----------------------------------------------------------------------------

void init(void)
{
  // in=switch BPM+, out=1x, out=3/4, out=speaker, out=10x, out=100x
  // switch BPM+ with pull-up, Speaker set low.
  DDRB = 0x3E;
  PORTB = 0x09;

  // in=switch 3/4, in=switch BPM-, out=LED red, out=LED green, out=LED blue, out=LED yellow
  // switches 3/4 and BPM- with pull-up
  DDRC = 0x3C;
  PORTC = 0x03;
  
  // 7-segment data set to output
  DDRD = 0xFF;
  PORTD = 0x00;

  // Start timer1 with lowest possible clock
  TCCR1B = 0x05;
  // wait 1 second before first interrupt - and display all LEDs ON
  TCNT1 = 0xE000;

  // Enable interrupts from timers
  TIMSK = 0x45;

  // enable interrupts
  sei();
}
//-----------------------------------------------------------------------------

void DisplayValue(uint8_t bpm, uint8_t quads)
{
  uint8_t calc_temp = 0;
  static uint8_t  hundreds,
                  tens,
                  ones;

  /* calculate 100's */
  hundreds = bpm / 100;

  /* calculate 10's  */
  calc_temp = bpm - (hundreds * 100);
  tens = calc_temp / 10;

  /* calculate 1's   */
  ones = calc_temp - (tens * 10);

  if(X100 == lcd_ciffer)
  {
    lcd_ciffer = X10;
    SEGMENT_X10;    
    SEGMENT_DATA = segment_table[tens];    
  }  
  else if(X10 == lcd_ciffer)
  {
    lcd_ciffer = X1;
    SEGMENT_X1;
    SEGMENT_DATA = segment_table[ones];
  }  
  else if(X1 == lcd_ciffer)
  {
    lcd_ciffer = BPM;
    SEGMENT_BPM;
    SEGMENT_DATA = segment_table[quads];
  }  
  else if(BPM == lcd_ciffer)
  {
    lcd_ciffer = X100;
    SEGMENT_X100;
    SEGMENT_DATA = segment_table[hundreds];
  }       
}
//-----------------------------------------------------------------------------

void LedUpdate(uint8_t quad_rythm)
{
  static uint8_t led_counter = 0;
  if(0 == led_counter)
  {
    LED_RED;
    Tone(TONE_LOW, TONE_MED);
  }
  else if(1 == led_counter)
  {
    LED_GREEN;
    Tone(TONE_HIGH, TONE_SHORT);
  }
  else if(2 == led_counter)
  {
    LED_BLUE;
    Tone(TONE_HIGH, TONE_SHORT);
  }
  else if(3 == led_counter)
  {
    LED_YELLOW;
    Tone(TONE_HIGH, TONE_SHORT);
  }
  led_counter++;

  if(2 < led_counter && 3 == quad_rythm)
  {
    led_counter = 0;
  }    
  else if(3 < led_counter)
    led_counter = 0;
}
//-----------------------------------------------------------------------------

void SwitchPressed(void)
{
  /* Reset timer */
  TCNT0 = 0x00;
  /* Start timer with lowest clk */
  TCCR0 = 0x05;  
  /* Set status flag for delay */
  timer_flag = SWITCH_PRESSED;
}
//-----------------------------------------------------------------------------

void CalculateBpmValue(uint8_t bpm_value)
{
  /* Calculate new timer offset value for timer1 */
  bpm_timer_value = 65536 - ((uint32_t)7814 * 60) / bpm_value;

  /* store new value in eeprom */

}
//-----------------------------------------------------------------------------

void Tone(uint8_t tone_value, uint8_t tone_length_value)
{
  /* Initialize timer2 output compare register with tone value */
  OCR2 = tone_value;
  /* set tone length */
  tone_length = tone_length_value;
  /* Start timer2 with fast PWM on output PB3 */
  TCCR2 = 0x6A;  
}
//-----------------------------------------------------------------------------

