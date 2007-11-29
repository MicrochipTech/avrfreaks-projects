extern void uart0_init();
/**
 @brief    initialize UART0
 @param    none                                    
 @return   none
*/
extern void uart0_putc(unsigned char c);
/**
 @brief    put char to  UART0
 @param    none                                    
 @return   none
*/
extern void uart0_puts(char *s);
/**
 @brief    put string to UART0
 @param    none                                    
 @return   none
*/
extern void uart0_send_initial_char();
/**
 @brief    Send initials char to  UART0
 @param    none                                    
 @return   none
*/

extern void uart1_init();
/**
 @brief    initialize UART1
 @param    none                                    
 @return   none
*/
extern void uart1_putc(unsigned char c);
/**
 @brief    put char to UART1
 @param    none                                    
 @return   none
*/
extern void uart1_puts(char *s);
/**
 @brief    put string UART1
 @param    none                                    
 @return   none
*/
extern void uart1_send_initial_char();
/**
 @brief    Send initials char to  UART1
 @param    none                                    
 @return   none
*/
