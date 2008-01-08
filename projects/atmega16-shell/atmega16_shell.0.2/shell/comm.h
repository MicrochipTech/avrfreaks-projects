#ifndef COMM_H_
#define COMM_H_

#define FOSC 8000000
#define BAUD(b)  ((FOSC/16/b) - 1)

/* Atomic init, send and receive functions */
void 			USART_Transmit	(unsigned char data);
unsigned char 	USART_Receive	(void);
void			USART_Init		(unsigned int ubrr);

/* 
 * API send/receive functions 
 */

/* sends a string, use \r\n full newlines */
void 			send_string		(const unsigned char *s);

/* wrapper for USART_Transmit */
void 			send_char		(unsigned char c);

/* receives a string of data, ending with char 13 (enter) or buffer full, 
 * returns read characters 
 */
unsigned int	receive_string	(unsigned char *s, 
								 unsigned int buffer_length, 
								 unsigned char echo);

/* wrapper for USART_Receive */ 
void			receive_char	(unsigned char *c);

#endif /*COMM_H_*/
