#ifndef _emm_SPI_h_
#define _emm_SPI_h_

void    spi_write( uint8_t );     /* Send data from SD card                 */
uint8_t spi_read( void );         /* Receive data from SD card              */
void    spi_init( void );         /* SPI setup and start at low speed F/128 */
void    spi_stop( void );         /* Stop SPI interface : unselect slave    */
void    spi_max( void );          /* Set SPI to maximum speed               */

#endif