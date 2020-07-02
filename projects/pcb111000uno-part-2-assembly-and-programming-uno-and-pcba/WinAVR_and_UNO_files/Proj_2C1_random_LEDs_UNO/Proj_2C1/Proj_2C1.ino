

/**************Proj_2C1_random_LED kaleidoscope***************************

The following project resources are introduced:
  I2C_Tx_any_segment(segment, digit_num);                     This illuminates a single segment
  I2C_Tx_any_segment_clear_all();
  
  PRN_16bit_GEN (0) A Pseudo Random Number Generator          A sequence of numbers is generated, each new number dependent on
                                                              the previous one. As a result of the zero (0)numbers are saved to eeprom
                                                              and read from it.
  "receiceChar()"                                             This reads the receive buffer of the UART (receiver/transmitter module)


Note:
"wdt_enable(WDTO_2S)" an AVR macro can trigger a reset after 2 seconds. Hower the command "wdr()" normally resets
the watch dog timer so preventing the reset.
The UART receive interrupt is introduced.  Any keypress can be used to pause program flow so triggering a SW_reset because
command "wdr()" is nolonger executed.
*/






#include "Proj_2C1_header_file.h"

char display_bkp[7];                                            //One element to backup each segment letter
                                                                //Segments are labelled a,b,c,d,e,f and g

int main (void){
char segment=0, digit_num=0, seg_counter = 0,direction = 0;

setup_UNO;
wdt_enable(WDTO_2S);                                           //WDT prevents display from being completed in either direction

I2C_Tx_any_segment_clear_all();                                //Initialise display

UCSR0B |= (1 << RXCIE0);                                       //Set Interrupt on key press (for test purposes only)
sei();                                                         //Global enable interrupt

while(1){                                                      //Generate pattern
while(seg_counter < 56){                                       //There are 56 segments in total
segment = (PRN_16bit_GEN (0)%7) + 'a';
digit_num = (PRN_16bit_GEN (0)%8);
                                                              //Continue statements skip back to the top of the while-loop
                                                              //This is to ensure segments are not turned-off before
                                                              //they have all been turned on.
      
if ((!(direction)) && (display_bkp[segment - 'a'] & (1 << digit_num))) continue;
if ((direction) && (!(display_bkp[segment - 'a'] & (1 << digit_num)))) continue;

I2C_Tx_any_segment(segment, digit_num);                        //Update display
backup_the_display(segment, digit_num);                        //keep backup up to date
Timer_T0_10mS_delay_x_m(5);  wdr();                            //delay and reset watch dog
seg_counter += 1;}

direction ^= 1;                                                //Toggle the direction_counter value
seg_counter = 0;
Timer_T0_10mS_delay_x_m(100);}}                                 //Just pause before toggling LEDs off one at a time



/*****************************************************************/
void backup_the_display(char segment, char digit_num){
display_bkp[segment - 'a'] = display_bkp[segment - 'a'] ^ (1 << digit_num);}



/*****************************************************************/
ISR(USART_RX_vect){receiveChar();
I2C_Tx_any_segment_clear_all();
while(1);}                                                      //Holt program flow until watch dog times out triggering a reset 
