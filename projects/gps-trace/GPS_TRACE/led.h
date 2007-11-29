int ledstategreen;
int ledstatered;

extern void led_init();
/**
 @brief    Set PC0, PC1 and PC2 as output
 @param    none                                    
 @return   none
*/
extern void led_yellow_on();
/**
 @brief    Turn on the yellow LED
 @param    none                                       
 @return   none
*/
extern void led_yellow_off();
/**
 @brief    Turn off the yellow LED
 @param    none
 @return   none
*/
extern void led_red_on();
/**
 @brief    Turn on the red LED
 @param    none                                       
 @return   none
*/
extern void led_red_off();
/**
 @brief    Turn off the red LED
 @param    none                                       
 @return   none
*/
extern void led_green_on();
/**
 @brief    Turn on the green LED
 @param    none                                       
 @return   none
*/
extern void led_green_off();
/**
 @brief    Turn off the green LED
 @param    none
 @return   none
*/
extern void led_toggle_red();
/**
 @brief    Toggle state of red LED
 @param    none
 @return   none
*/
extern void led_toggle_green(); 
/**
 @brief    Toggle state of  green LED
 @param    none
 @return   none
*/
